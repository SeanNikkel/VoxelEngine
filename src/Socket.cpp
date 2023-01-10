#include "Socket.h"

#include <iostream>
#include <cassert>

#if _WIN32
#include <WS2tcpip.h>
#define s_addr S_un.S_addr
#else
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#define SD_BOTH SHUT_RDWR
#define ioctlsocket ioctl
#define closesocket close
#define INVALID_SOCKET -1
#define WSAGetLastError() errno
#define WSAEWOULDBLOCK EWOULDBLOCK
#define WSAECONNRESET ECONNRESET
#endif

SocketAddress::SocketAddress(const char *address, uint16_t port)
{
	Set(address, port);
}

SocketAddress::SocketAddress(uint16_t port)
{
	address_.sin_family = AF_INET;
	address_.sin_port = htons(port);
	address_.sin_addr.s_addr = INADDR_ANY;
}

void SocketAddress::Set(const char *address, uint16_t port)
{
	address_.sin_family = AF_INET;
	address_.sin_port = htons(port);
	if (inet_pton(AF_INET, address, &address_.sin_addr) != 1)
		assert(!"Error converting target address string.");
}

uint16_t SocketAddress::Port() const
{
	return ntohs(address_.sin_port);
}

SocketAddress::operator sockaddr_in() const
{
	return address_;
}

bool SocketAddress::operator==(const SocketAddress &other) const
{
	return address_.sin_addr.s_addr == other.address_.sin_addr.s_addr && address_.sin_port == other.address_.sin_port;
}

TCPSocket::TCPSocket(const SocketAddress &address) : remoteAddress_(address)
{
	// Create socket
	sock_ = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_ == INVALID_SOCKET)
		assert(!"Error creating client socket.");

	// Connect to host
	if (connect(sock_, reinterpret_cast<const sockaddr *>(&remoteAddress_), sizeof(remoteAddress_)))
		assert(!"Error connecting to host socket.");

	std::cout << "Successfully connected to " << remoteAddress_ << " on TCP." << std::endl;

	// Set to nonblocking
	unsigned long val = 1;
	if (ioctlsocket(sock_, FIONBIO, &val))
		assert(!"Error setting client socket to nonblocking.");
}

TCPSocket::TCPSocket(const SOCKET &sock)
{
#if _WIN32 && !defined(NDEBUG)
	// Check if socket is valid
	CSADDR_INFO info;
	int infoSize = sizeof(info);
	getsockopt(sock, SOL_SOCKET, SO_BSP_STATE, reinterpret_cast<char *>(&info), &infoSize);
	assert(info.RemoteAddr.lpSockaddr != nullptr);
#endif

	sock_ = sock;

	socklen_t remoteAddressSize = sizeof(remoteAddress_);
	getpeername(sock_, reinterpret_cast<sockaddr *>(&remoteAddress_), &remoteAddressSize);
	std::cout << "Successfully connected to " << remoteAddress_ << " on TCP." << std::endl;

	// Set to nonblocking
	unsigned long val = 1;
	if (ioctlsocket(sock_, FIONBIO, &val))
		assert(!"Error setting client socket to nonblocking.");
}

TCPSocket::TCPSocket(TCPSocket &&other) noexcept
{
	sock_ = other.sock_;
	remoteAddress_ = other.remoteAddress_;
	other.sock_ = INVALID_SOCKET;
}

TCPSocket &TCPSocket::operator=(TCPSocket &&other) noexcept
{
	std::swap(sock_, other.sock_);
	std::swap(remoteAddress_, other.remoteAddress_);
	return *this;
}

const SocketAddress &TCPSocket::GetRemoteAddress() const
{
	return remoteAddress_;
}

void TCPSocket::Send(const void *data, int size)
{
	int result = send(sock_, reinterpret_cast<const char *>(data), size, 0);

	if (result == -1 && WSAGetLastError() != WSAEWOULDBLOCK)
		assert(!"Error sending data.");
}

int TCPSocket::Receive(void *buffer, int size)
{
	// Get data from sockets
	int result = recv(sock_, reinterpret_cast<char *>(buffer), size, 0);

	// Error
	if (result == -1)
	{
		int err = WSAGetLastError();

		// Connection forcibly closed
		if (err == WSAECONNRESET)
			return -1;

		// Other error
		if (err != WSAEWOULDBLOCK)
			assert(!"Error receiving data.");

		return 0;
	}

	// Connection closed
	if (result == 0)
		return -1;

	return result;
}

TCPSocket::~TCPSocket()
{
	if (sock_ != INVALID_SOCKET)
	{
		// Debug log
		std::cout << "Closed connection with " << remoteAddress_ << " on TCP." << std::endl;

		shutdown(sock_, SD_BOTH);
		closesocket(sock_);
	}
}

UDPSocket::UDPSocket(uint16_t port) : port_(port)
{
	// Create socket
	sock_ = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_ == INVALID_SOCKET)
		assert(!"Error creating client socket.");

	// Create socket address
	SocketAddress addr(port);

	// Connect to host
	if (bind(sock_, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)))
		assert(!"Error connecting to host socket.");

	// Get port if implementation defined
	if (port_ == 0)
	{
		socklen_t addrSize = sizeof(addr);
		getsockname(sock_, reinterpret_cast<sockaddr *>(&addr), &addrSize);
		port_ = addr.Port();
	}

	std::cout << "Opened UDP socket on port " << port_ << "." << std::endl;

	// Set to nonblocking
	unsigned long val = 1;
	if (ioctlsocket(sock_, FIONBIO, &val))
		assert(!"Error setting client socket to nonblocking.");
}

UDPSocket::UDPSocket(UDPSocket &&other) noexcept
{
	sock_ = other.sock_;
	port_ = other.port_;
	other.sock_ = INVALID_SOCKET;
}

UDPSocket &UDPSocket::operator=(UDPSocket &&other) noexcept
{
	std::swap(sock_, other.sock_);
	std::swap(port_, other.port_);
	return *this;
}

void UDPSocket::Send(const SocketAddress &address, const void *data, int size)
{
	int result = sendto(sock_, reinterpret_cast<const char *>(data), size, 0, reinterpret_cast<const sockaddr *>(&address), sizeof(address));

	if (result == -1 && WSAGetLastError() != WSAEWOULDBLOCK)
		assert(!"Error sending data.");
}

int UDPSocket::Receive(void *buffer, int size)
{
	SocketAddress s;
	return Receive(buffer, size, s);
}

int UDPSocket::Receive(void *buffer, int size, SocketAddress &address)
{
	// Get data from sockets
	socklen_t addressSize = sizeof(address);
	int result = recvfrom(sock_, reinterpret_cast<char *>(buffer), size, 0, reinterpret_cast<sockaddr *>(&address), &addressSize);

	// Error
	if (result == -1)
	{
		int err = WSAGetLastError();

		// Connection forcibly closed
		if (err == WSAECONNRESET)
			return -1;

		// Other error
		if (err != WSAEWOULDBLOCK)
			assert(!"Error receiving data.");

		return 0;
	}

	// Connection closed
	if (result == 0)
		return -1;

	return result;
}

UDPSocket::~UDPSocket()
{
	if (sock_ != INVALID_SOCKET)
	{
		// Debug log
		std::cout << "Stopped listening for UDP traffic on port " << port_ << "." << std::endl;

		shutdown(sock_, SD_BOTH);
		closesocket(sock_);
	}
}

ListenSocket::ListenSocket(uint16_t port) : localAddress_(port)
{
	// Create socket
	sock_ = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_ == INVALID_SOCKET)
		assert(!"Error creating host socket.");

	// Create socket address
	if (bind(sock_, reinterpret_cast<sockaddr *>(&localAddress_), sizeof(localAddress_)))
	{
		std::cout << WSAGetLastError() << std::endl;
		assert(!"Error binding host socket.");
	}

	// Enable listening
	if (listen(sock_, SOMAXCONN))
		assert(!"Error setting host socket to listen.");

	std::cout << "Listening for TCP connections to port " << port << "..." << std::endl;

	// Set to nonblocking
	unsigned long val = 1;
	if (ioctlsocket(sock_, FIONBIO, &val))
		assert(!"Error setting host socket to nonblocking.");
}

std::optional<TCPSocket> ListenSocket::AcceptConnection()
{
	// Connect to client
	socklen_t addrSize = sizeof(localAddress_);
	SOCKET connectionSocket = accept(sock_, reinterpret_cast<sockaddr *>(&localAddress_), &addrSize);

	if (connectionSocket == INVALID_SOCKET)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
			assert(!"Error accepting client socket.");

		// No connections
		return {};
	}
	return TCPSocket(connectionSocket);
}

ListenSocket::~ListenSocket()
{
	std::cout << "Stopped listening for TCP connections to port " << localAddress_.Port() << "." << std::endl;

	shutdown(sock_, SD_BOTH);
	closesocket(sock_);
}

std::ostream &operator<<(std::ostream &output, const SocketAddress &addr)
{
	const auto ip = addr.address_.sin_addr.s_addr;
	std::cout << int(ip & 0xff) << "." << int(ip>>8 & 0xff) << "." << int(ip>>16 & 0xff) << "." << int(ip>>24 & 0xff) << ":" << ntohs(addr.address_.sin_port);
	return output;
}