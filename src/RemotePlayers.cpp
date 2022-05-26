#include "RemotePlayers.h"

RemoteHost::RemoteHost(const char *address, uint16_t port) : host_(address, port)
{
}

bool RemoteHost::Update(const PlayerPacket &packet)
{
    ClearBlockUpdates();

    // Receive packets

    for (;;)
    {
        // Receive next packet
        IdentifiedPlayerPacket pack;
        int result = host_.Receive(&pack, sizeof(pack));

        if (result == 0)
            break;

        if (result == -1)
            return false;


        if (pack.playerId >= players_.size())
            players_.resize(pack.playerId + 1); // Currently, clients won't see other clients disconnect

        UpdatePlayer(pack);
    }
    // Send packets
    host_.Send(&packet, sizeof(packet));

    return true;
}

std::vector<RemotePlayer> RemoteHost::GetPlayers()
{
    return players_;
}

RemoteClients::RemoteClients(uint16_t port) : listen_(port)
{
}

bool RemoteClients::Update(const PlayerPacket &packet)
{
    ClearBlockUpdates();

    // Accept any new connections
    for (;;)
    {
        std::optional<Socket> result = listen_.AcceptConnection();

        if (result.has_value())
        {
            clients_.push_back(std::move(*result));
            players_.push_back({});
        }
        else
            break;
    }

    // Get data from all clients
    for (size_t i = 0; i < clients_.size(); /* i++ below */)
    {
        int result;
        for (;;)
        {
            // Receive next packet
            IdentifiedPlayerPacket pack { uint8_t(i) };
            result = clients_[i].Receive(&pack.packet, sizeof(pack.packet));

            if (result <= 0)
                break;
            
            // Apply packet
            UpdatePlayer(pack);

            // Send this packet to all other clients
            size_t id = i;
            for (size_t j = 0; j < clients_.size(); j++)
            {
                // Continuous and consistent numbering for sending from i to j
                // Table of values for id:
                //            j =
                //         | 0 1 2 ...
                //       --+-------
                //       0 | . 1 1
                //  i =  1 | 1 . 2
                //       2 | 2 2 .
                //      ...

                if (j == i)
                {
                    id++;
                    continue;
                }

                pack.playerId = char(id);
                clients_[j].Send(&pack, sizeof(pack));
            }
        }

        // Remove socket if connection closed
        if (result == -1)
        {
            clients_.erase(clients_.begin() + i);
            players_.erase(players_.begin() + i);
        }
        else
            i++;
    }

    // Send packets
    IdentifiedPlayerPacket idPacket = { 0, packet };
    for (Socket &client : clients_)
    {
        client.Send(&idPacket, sizeof(idPacket));
    }

    // Host can't disconnect
    return true;
}

std::vector<RemotePlayer> RemoteClients::GetPlayers()
{
    return players_;
}

void RemotePlayers::UpdatePlayer(const IdentifiedPlayerPacket &packet)
{
    RemotePlayer &p = players_[packet.playerId];

    p.data = packet.packet.player;

    // Add block update to list if needed
    if (packet.packet.block.blockType != Block::BlockType::BLOCK_ERROR)
        p.blockUpdates.push_back(packet.packet.block);
}

void RemotePlayers::ClearBlockUpdates()
{
    for (RemotePlayer &player : players_)
        player.blockUpdates.clear();
}
