#include "NetworkManager.h"
#include "ChunkManager.h"

#include <string>
#include <iostream>

NetworkManager::NetworkManager() : head_(Mesh::CreateCube(0.5f)), headTexture_("resources/player.png", false, true, GL_REPEAT, GL_NEAREST)
{
	// Initialize WinSock
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		assert(!"Error initializing WinSock!");
}


void NetworkManager::Start(const char *ip)
{
	std::string input = ip;
	size_t colon = input.find(':');

	// Connect
	if (colon != std::string::npos)
	{
		std::string address = input.substr(0, colon);
		std::string port = input.substr(colon + 1, input.size() - (colon + 1));
		players_ = std::unique_ptr<RemotePlayers>(new RemoteHost(address.c_str(), std::stoi(port)));
	}
	// Host
	else
	{
		players_ = std::unique_ptr<RemotePlayers>(new RemoteClients(std::stoi(input)));
	}
}

void NetworkManager::RegisterBlockUpdate(const BlockUpdate &update)
{
	blockUpdate_ = update;
}

void NetworkManager::Update(const Player &player)
{
	if (players_ == nullptr)
		return;

	// Prepare packet to send
	PlayerPacket send;
	send.block = blockUpdate_;
	send.player.pitch = player.GetCamera().GetPitch();
	send.player.yaw = player.GetCamera().GetYaw();
	send.player.position = player.GetCamera().GetPosition();

	// Reset block update
	blockUpdate_ = BlockUpdate();

	// Send and receive packets
	if (!players_->Update(send))
	{
		// If disconnected, remove players
		players_.reset();
		return;
	}

	// Handle all block updates
	for (const RemotePlayer &player : players_->GetPlayers())
	{
		for (const BlockUpdate &update : player.blockUpdates)
		{
			ChunkManager::Instance().SetBlock(update.coords, { update.blockType });
		}
	}

}

void NetworkManager::Render(Shader &shader)
{
	if (players_ == nullptr)
		return;

	shader.Use();
	headTexture_.Activate(GL_TEXTURE0);

	// Render each player
	for (const RemotePlayer &player : players_->GetPlayers())
	{
		// All other uniforms are already set when drawing chunks
		shader.SetVar("modelMatrix", glm::translate(glm::mat4(1.0f), player.data.position) * glm::rotate(glm::mat4(1.0f), player.data.yaw, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), player.data.pitch, glm::vec3(1.0f, 0.0f, 0.0f)));

		head_.Draw();
	}
}

NetworkManager::~NetworkManager()
{
	// Destroy players' sockets before WinSock
	players_.reset();

	// Cleanup WinSock
	if (WSACleanup() != 0)
		assert(!"Error shutting down WinSock!");
}
