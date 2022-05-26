#include "Socket.h"

#include <iostream>
#include <cassert>
#include <WS2tcpip.h>

Socket::Socket(const char *address, uint16_t port)
{
	// Create socket
	sock_ = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_ == INVALID_SOCKET)
		assert(!"Error creating client socket.");

	// Create socket address
	sockaddr_in remoteAddress;
	remoteAddress.sin_family = AF_INET;
	remoteAddress.sin_port = htons(port);
	if (inet_pton(AF_INET, address, &remoteAddress.sin_addr) != 1)
		assert(!"Error converting target address string.");

	// Connect to host
	if (connect(sock_, reinterpret_cast<sockaddr *>(&remoteAddress), sizeof(remoteAddress)))
		assert(!"Error connecting to host socket.");

	std::cout << "Successfully connected to " << address << ":" << port << "." << std::endl;

	// Set to nonblocking
	u_long val = 1;
	if (ioctlsocket(sock_, FIONBIO, &val))
		assert(!"Error setting client socket to nonblocking.");
}

Socket::Socket(const SOCKET &sock)
{
	// Check if socket is valid
#if _DEBUG
	CSADDR_INFO info;
	int infoSize = sizeof(info);
	getsockopt(sock, SOL_SOCKET, SO_BSP_STATE, reinterpret_cast<char *>(&info), &infoSize);
	assert(info.RemoteAddr.lpSockaddr != nullptr);
#endif

	sock_ = sock;

	// Set to nonblocking
	u_long val = 1;
	if (ioctlsocket(sock_, FIONBIO, &val))
		assert(!"Error setting client socket to nonblocking.");
}

Socket::Socket(Socket &&other) noexcept
{
	sock_ = other.sock_;
	other.sock_ = INVALID_SOCKET;
}

Socket &Socket::operator=(Socket &&other) noexcept
{
	std::swap(sock_, other.sock_);
	return *this;
}

void Socket::Send(const void *data, int size)
{
	int result = send(sock_, reinterpret_cast<const char *>(data), size, 0);

	if (result == -1 && WSAGetLastError() != WSAEWOULDBLOCK)
		assert(!"Error sending data.");
}

int Socket::Receive(void *buffer, int size)
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

Socket::~Socket()
{
	if (sock_ != INVALID_SOCKET)
	{
		// Debug log
		sockaddr_in remoteAddress;
		int remoteAddressSize = sizeof(remoteAddress);
		getpeername(sock_, reinterpret_cast<sockaddr *>(&remoteAddress), &remoteAddressSize);
		std::cout << "Closed connection with " << remoteAddress << "." << std::endl;

		shutdown(sock_, SD_BOTH);
		closesocket(sock_);
	}
}

ListenSocket::ListenSocket(uint16_t port)
{
	// Create socket
	sock_ = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_ == INVALID_SOCKET)
		assert(!"Error creating host socket.");

	// Create socket address
	localAddress_.sin_family = AF_INET;
	localAddress_.sin_port = htons(port);
	localAddress_.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(sock_, reinterpret_cast<sockaddr *>(&localAddress_), sizeof(localAddress_)))
		assert(!"Error binding host socket.");

	// Enable listening
	if (listen(sock_, SOMAXCONN))
		assert(!"Error setting host socket to listen.");

	std::cout << "Listening for connections to port " << port << "..." << std::endl;

	// Set to nonblocking
	u_long val = 1;
	if (ioctlsocket(sock_, FIONBIO, &val))
		assert(!"Error setting host socket to nonblocking.");
}

std::optional<Socket> ListenSocket::AcceptConnection()
{
	// Connect to client
	int addrSize = sizeof(localAddress_);
	SOCKET connectionSocket = accept(sock_, reinterpret_cast<sockaddr *>(&localAddress_), &addrSize);

	if (connectionSocket == INVALID_SOCKET)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
			assert(!"Error accepting client socket.");

		// No connections
		return {};
	}
	sockaddr_in remoteAddress;
	int remoteAddressSize = sizeof(remoteAddress);
	getpeername(connectionSocket, reinterpret_cast<sockaddr *>(&remoteAddress), &remoteAddressSize);
	std::cout << "Successfully accepted connection to " << remoteAddress << "." << std::endl;
	return Socket(connectionSocket);
}

ListenSocket::~ListenSocket()
{
	std::cout << "Stopped listening for connections to port " << ntohs(localAddress_.sin_port) << "." << std::endl;

	shutdown(sock_, SD_BOTH);
	closesocket(sock_);
}

std::ostream &operator<<(std::ostream &output, const sockaddr_in &addr)
{
	const auto &ip = addr.sin_addr.S_un.S_un_b;
	std::cout << int(ip.s_b1) << "." << int(ip.s_b2) << "." << int(ip.s_b3) << "." << int(ip.s_b4) << ":" << ntohs(addr.sin_port);
	return output;
}
