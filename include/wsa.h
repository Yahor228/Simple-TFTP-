#pragma once 
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <WS2tcpip.h>
class WSA
{
public:
	WSA()
	{
		WSAStartup(MAKEWORD(2, 2), &data);
	}
	~WSA()
	{
		WSACleanup();
	}
private:
	WSAData data;
};
#else
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>

#include <sys/types.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
class WSA
{};
#define closesocket close
using SOCKET = int;
#endif // _WIN32
#include <string>


std::string GetSockError();
std::wstring GetSockErrorW();

