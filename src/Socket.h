#pragma once

#if _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <WinSock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
typedef int SOCKET;
#endif

#include <vector>
#include <optional>
#include <iostream>
#include <functional>

// Wrapper for sockaddr_in
class SocketAddress
{
public:
	SocketAddress(const char *address, uint16_t port);
	SocketAddress(uint16_t port = 0); // any ip

	void Set(const char *address, uint16_t port);

	uint16_t Port() const;

	operator sockaddr_in() const;
	bool operator==(const SocketAddress &other) const;
	friend struct std::hash<SocketAddress>;
	friend std::ostream &operator<<(std::ostream &output, const SocketAddress &addr);

private:
	sockaddr_in address_;

};

// Hash for SocketAddress
template<>
struct std::hash<SocketAddress>
{
	std::size_t operator()(SocketAddress const &a) const noexcept
	{
		return std::hash<size_t>()((size_t(a.address_.sin_port) << 32) | a.address_.sin_addr.s_addr);
	}
};

class TCPSocket
{
public:
	// Create client socket and connect to address
	TCPSocket(const SocketAddress &address);

	// Create Socket from already connected SOCKET
	TCPSocket(const SOCKET &sock);

	// Move
	TCPSocket(TCPSocket &&other) noexcept;
	TCPSocket &operator=(TCPSocket &&other) noexcept;

	const SocketAddress &GetRemoteAddress() const;

	// Send bytes on socket
	void Send(const void *data, int size);

	template<typename T>
	void Send(const std::vector<T> &buffer)
	{
		if (buffer.size() > 0)
			Send(buffer.data(), int(sizeof(T) * buffer.size()));
	}

	// Recieve bytes on socket
	int Receive(void *buffer, int size);

	~TCPSocket();

	// Don't copy sockets
	TCPSocket(const TCPSocket &other) = delete;
	TCPSocket &operator=(const TCPSocket &other) = delete;
private:
	SOCKET sock_;
	SocketAddress remoteAddress_;
};

// Udp
class UDPSocket
{
public:
	// Create host socket on port (implementation defined if 0)
	UDPSocket(uint16_t port = 0);

	// Move
	UDPSocket(UDPSocket &&other) noexcept;
	UDPSocket &operator=(UDPSocket &&other) noexcept;

	// Send bytes on socket to address
	void Send(const SocketAddress &address, const void *data, int size);

	template<typename T>
	void Send(const SocketAddress &address, const std::vector<T> &buffer)
	{
		Send(address, buffer.data(), sizeof(T) * buffer.size());
	}

	// Recieve bytes on socket
	int Receive(void *buffer, int size);
	int Receive(void *buffer, int size, SocketAddress &address);

	~UDPSocket();

	// Don't copy sockets
	UDPSocket(const UDPSocket &other) = delete;
	UDPSocket &operator=(const UDPSocket &other) = delete;
private:
	SOCKET sock_;
	uint16_t port_;
};

// TCP socket that accepts incoming connections
class ListenSocket
{
public:
	// Create listen socket (for host)
	ListenSocket(uint16_t port);

	// Move
	ListenSocket(ListenSocket &&other) = default;
	ListenSocket &operator=(ListenSocket &&other) = default;

	// Recieve any sockets attempting to connect
	std::optional<TCPSocket> AcceptConnection();

	~ListenSocket();

	// Don't copy sockets
	ListenSocket(const ListenSocket &other) = delete;
	ListenSocket &operator=(const ListenSocket &other) = delete;
private:
	SOCKET sock_;
	SocketAddress localAddress_;
};