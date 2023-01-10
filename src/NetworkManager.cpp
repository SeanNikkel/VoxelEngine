#include "NetworkManager.h"
#include "ChunkManager.h"

#include <string>
#include <iostream>

NetworkManager::NetworkManager() : head_(Mesh::CreateCube(0.5f)), headTexture_("resources/player.png", false, true, GL_REPEAT, GL_NEAREST)
{
#if _WIN32
	// Initialize WinSock
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		assert(!"Error initializing WinSock!");
#endif
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
	blockUpdates_.push_back(update);
}

void NetworkManager::Update(const Player &player)
{
	if (players_ == nullptr)
		return;

	// Prepare packet to send
	PlayerPacket send;
	send.pitch = player.GetCamera().GetPitch();
	send.yaw = player.GetCamera().GetYaw();
	send.position = player.GetCamera().GetPosition();

	// Send and receive packets
	if (!players_->Update(send, blockUpdates_))
	{
		// If disconnected, remove players
		players_.reset();
		return;
	}

	// Reset block update
	blockUpdates_.clear();

	// Handle all block updates
	for (const BlockUpdate &update : players_->GetBlockUpdates())
	{
		ChunkManager::Instance().SetBlock(update.coords, { update.blockType });
	}
}

void NetworkManager::Render(Shader &shader)
{
	if (players_ == nullptr)
		return;

	shader.Use();
	headTexture_.Activate(GL_TEXTURE0);

	// Render each player
	for (const PlayerPacket &player : players_->GetPlayers())
	{
		// All other uniforms are already set when drawing chunks
		glm::mat4 model =
			glm::translate(glm::mat4(1.0f), player.position) *
			glm::rotate(glm::mat4(1.0f), player.yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), player.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		shader.SetVar("modelMatrix", model);
		shader.SetVar("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

		head_.Draw();
	}
}

NetworkManager::~NetworkManager()
{
	// Destroy players' sockets before WinSock
	players_.reset();

#if _WIN32
	// Cleanup WinSock
	if (WSACleanup() != 0)
		assert(!"Error shutting down WinSock!");
#endif
}
