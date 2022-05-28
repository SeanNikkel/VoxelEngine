#pragma once

#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"
#include "RemotePlayers.h"
#include "Player.h"

#include <glm/glm.hpp>

#include <memory>

// Handles updating and rendering of remote players
class NetworkManager
{
public:
	// Singleton pattern
	static NetworkManager &Instance()
	{
		static NetworkManager instance;
		return instance;
	}

	NetworkManager();

	// Host/Connect to server
	//	To host: ip = "[port]"
	//	To connect: ip = "[ip]:[port]" (e.g. "127.0.0.1:25565")
	void Start(const char *ip);

	// Send block update over network (only one per frame currently)
	void RegisterBlockUpdate(const BlockUpdate &update);

	// Send and receive + handle packets
	void Update(const Player &player);

	// Render other players heads
	void Render(Shader &shader);
	 
	~NetworkManager();

private:
	std::unique_ptr<RemotePlayers> players_;
	std::vector<BlockUpdate> blockUpdates_;
	Mesh head_;
	Texture headTexture_;

public: // Remove functions for singleton
	NetworkManager(NetworkManager const &) = delete;
	void operator=(NetworkManager const &) = delete;
};