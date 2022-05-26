#pragma once

#include <glm/glm.hpp>

#include "Socket.h"
#include "Block.h"

// These are sent over the network
#pragma pack(push, 1)
// Player head data
struct PlayerData
{
    glm::vec3 position;
    float pitch;
    float yaw;
};

// Info to describe a block change
struct BlockUpdate
{
    Block::BlockType blockType = Block::BlockType::BLOCK_ERROR;
    glm::ivec3 coords;
};

// Sent by clients to host
struct PlayerPacket
{
    PlayerData player;
    BlockUpdate block;
};

// Sent by host to clients
struct IdentifiedPlayerPacket
{
    uint8_t playerId;
    PlayerPacket packet;
};
#pragma pack(pop)

// Describes all networked data for a player
struct RemotePlayer
{
    PlayerData data;
    std::vector<BlockUpdate> blockUpdates;
};

// Abstraction of host + client networking differences
class RemotePlayers
{
public:
    // Send given packet and handle any new packets
    virtual bool Update(const PlayerPacket &packet) = 0;

    // Get the networked player data
    virtual std::vector<RemotePlayer> GetPlayers() = 0;

    virtual ~RemotePlayers() {};
protected:
    // Updates networked player data with a new packet
    void UpdatePlayer(const IdentifiedPlayerPacket &packet);
    
    void ClearBlockUpdates();

    std::vector<RemotePlayer> players_;
};

// Implementation of host communication for clients
class RemoteHost : public RemotePlayers
{
public:
    RemoteHost(const char *address, uint16_t port);
    bool Update(const PlayerPacket &packet) override;
    std::vector<RemotePlayer> GetPlayers() override;

private:
    Socket host_;
};

// Implementation of clients communication for host
class RemoteClients : public RemotePlayers
{
public:
    RemoteClients(uint16_t port);
    bool Update(const PlayerPacket &packet) override;
    std::vector<RemotePlayer> GetPlayers() override;

private:
    std::vector<Socket> clients_;
    ListenSocket listen_;
};