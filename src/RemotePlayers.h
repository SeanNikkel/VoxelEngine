#pragma once

#include <glm/glm.hpp>

#include "Socket.h"
#include "Block.h"

// These are sent over the network
#pragma pack(push, 1)
// Info to describe a block change
struct BlockUpdate
{
    Block::BlockType blockType = Block::BlockType::BLOCK_ERROR;
    glm::ivec3 coords;
};

// Sent by clients to host
struct PlayerPacket
{
    glm::vec3 position;
    float pitch;
    float yaw;
};

// Sent by host to clients
struct IdentifiedPlayerPacket
{
    uint8_t playerId;
    PlayerPacket packet;
};
#pragma pack(pop)

// Abstraction of host + client networking differences
class RemotePlayers
{
public:
    // Send given packet and handle any new packets
    virtual bool Update(const PlayerPacket &packet, const std::vector<BlockUpdate> &blocks) = 0;

    // Stop updating/drawing a player
    virtual void RemovePlayer(size_t i);

    // Get the networked player data
    const std::vector<PlayerPacket> &GetPlayers();
    const std::vector<BlockUpdate> &GetBlockUpdates();

    virtual ~RemotePlayers() {};
protected:
    void ClearBlockUpdates();

    std::vector<PlayerPacket> players_;
    std::vector<BlockUpdate> updates_;
};

// Implementation of host communication for clients
class RemoteHost : public RemotePlayers
{
public:
    RemoteHost(const char *address, uint16_t port);
    bool Update(const PlayerPacket &packet, const std::vector<BlockUpdate> &blocks) override;

private:
    TCPSocket tcp_;
    UDPSocket udp_;
};

// Implementation of clients communication for host
class RemoteClients : public RemotePlayers
{
public:
    RemoteClients(uint16_t port);
    bool Update(const PlayerPacket &packet, const std::vector<BlockUpdate> &blocks) override;
    void RemovePlayer(size_t i) override;

private:
    std::vector<TCPSocket> tcps_;
    std::unordered_map<SocketAddress, size_t> addressMap_;
    UDPSocket udp_;
    ListenSocket listen_;
};