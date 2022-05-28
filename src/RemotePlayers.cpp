#include "RemotePlayers.h"

RemoteHost::RemoteHost(const char *address, uint16_t port) : tcp_({ address, port }), udp_()
{
}

bool RemoteHost::Update(const PlayerPacket &packet, const std::vector<BlockUpdate> &blocks)
{
    ClearBlockUpdates();

    // Receive TCP packets
    for (;;)
    {
        BlockUpdate bu;
        int result = tcp_.Receive(&bu, sizeof(bu));

        if (result == 0)
            break;

        if (result == -1)
            return false;

        updates_.push_back(bu);
    }

    // Receive UDP packets
    for (;;)
    {
        // Receive next packet
        IdentifiedPlayerPacket pack;
        int result = udp_.Receive(&pack, sizeof(pack));

        if (result == 0)
            break;

        if (result == -1)
            return false;

        if (pack.playerId >= players_.size())
            players_.resize(pack.playerId + 1); // Currently, clients won't see other clients disconnect

        players_[pack.playerId] = pack.packet;
    }
    // Send packets
    udp_.Send(tcp_.GetRemoteAddress(), &packet, int(sizeof(packet)));
    tcp_.Send(blocks);

    return true;
}

RemoteClients::RemoteClients(uint16_t port) : udp_(port), listen_(port)
{
}

bool RemoteClients::Update(const PlayerPacket &packet, const std::vector<BlockUpdate> &blocks)
{
    ClearBlockUpdates();

    // Accept any new connections
    for (;;)
    {
        std::optional<TCPSocket> result = listen_.AcceptConnection();

        // No more connections
        if (!result.has_value())
            break;

        tcps_.push_back(std::move(*result));
        players_.push_back({});
    }

    // Receive TCP packets
    std::vector<std::vector<BlockUpdate>> updates(tcps_.size());
    for (size_t i = 0; i < tcps_.size(); )
    {
        int result;
        for (;;)
        {
            // Receive next packet
            BlockUpdate bu;
            result = tcps_[i].Receive(&bu, sizeof(bu));

            // No more data or disconnected
            if (result == 0 || result == -1)
                break;

            // Add block update to list
            updates[i].push_back(bu);
        }

        if (result == -1)
            RemovePlayer(i); // Remove if disconnected
        else
            i++;
    }

    // Recieve packets
    int result;
    for (;;)
    {
        // Receive next packet
        PlayerPacket pack;
        SocketAddress address;
        result = udp_.Receive(&pack, sizeof(pack), address);

        // No more packets
        if (result <= 0)
            break;
            
        // Create/lookup index
        uint8_t index = uint8_t(addressMap_.insert(std::make_pair(address, addressMap_.size())).first->second);

        // Apply packet
        players_[index] = pack;
    }

    // Send packets
    for (const auto &address : addressMap_)
    {
        // Send host data
        IdentifiedPlayerPacket hostPacket = { 0, packet };
        udp_.Send(address.first, &hostPacket, sizeof(hostPacket));

        // Send other client's data
        size_t id = 0;
        for (size_t i = 0; i < players_.size(); i++)
        {
            // If data came from here
            if (i == address.second)
                continue;

            IdentifiedPlayerPacket clientPacket = { uint8_t(++id), players_[i] };
            udp_.Send(address.first, &clientPacket, sizeof(clientPacket));  // Players
            tcps_[address.second].Send(updates[i]);                         // Blocks
        }

        tcps_[address.second].Send(blocks);
    }

    // Add all block updates into updates_
    for (const std::vector<BlockUpdate> &update : updates)
    {
        updates_.insert(updates_.end(), update.begin(), update.end());
    }

    // Host can't disconnect
    return true;
}

void RemoteClients::RemovePlayer(size_t i)
{
    RemotePlayers::RemovePlayer(i);

    tcps_.erase(tcps_.begin() + i);

    // Remove from address map
    addressMap_.erase(std::find_if(addressMap_.begin(), addressMap_.end(), [i](const auto &pair) { return pair.second == i; }));
    for (auto &address : addressMap_)
    {
        if (address.second > i)
            address.second--;
    }
}

void RemotePlayers::RemovePlayer(size_t i)
{
    players_.erase(players_.begin() + i);
}

const std::vector<PlayerPacket> &RemotePlayers::GetPlayers()
{
    return players_;
}

const std::vector<BlockUpdate> &RemotePlayers::GetBlockUpdates()
{
    return updates_;
}

void RemotePlayers::ClearBlockUpdates()
{
    updates_.clear();
}
