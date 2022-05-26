#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <WinSock2.h>

#include <vector>
#include <optional>
#include <iostream>

// Print sockaddr_in nicely
std::ostream &operator<<(std::ostream &output, const sockaddr_in &addr);

class Socket
{
public:
	// Create client socket and connect to address
	Socket(const char *address, uint16_t port);

	// Create Socket from already connected SOCKET
	Socket(const SOCKET &sock);

	// Move
	Socket(Socket &&other) noexcept;
	Socket &operator=(Socket &&other) noexcept;

	// Send bytes on socket
	void Send(const void *data, int size);

	// Recieve bytes on socket
	int Receive(void *buffer, int size);

	~Socket();

	// Don't copy sockets
	Socket(const Socket &other) = delete;
private:
	SOCKET sock_;
};

class ListenSocket
{
public:
	// Create listen socket (for host)
	ListenSocket(uint16_t port);

	// Recieve any sockets attempting to connect
	std::optional<Socket> AcceptConnection();

	~ListenSocket();

private:
	SOCKET sock_;
	sockaddr_in localAddress_;
};