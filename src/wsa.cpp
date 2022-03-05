#include <wsa.h>


#ifdef _WIN32
#pragma comment (lib, "Ws2_32.lib")

std::string GetSockError() //write on windows
{
    char* s = nullptr;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&s, 0, nullptr);
    std::string r{ s };
    LocalFree(s);
    return r;
}
#else
#include <string.h>
std::string GetSockError()
{
    return strerror(errno);
}
#endif
