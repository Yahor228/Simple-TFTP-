#pragma once
#include <array>
#include <string>
#include <wsa.h>

class IPParser
{
public:
	IPParser() = default;
	IPParser(sockaddr_storage stor);

	bool Parse(std::string_view ip_port); //192.168.1.235,69
	sockaddr_in convert_to_v4()const;
	sockaddr_in6 convert_to_v6() const;
	bool IsV6()const{
		return ip_v6;
	}
	std::string_view IP()const
	{
		return ip;
	}
	auto Port()const
	{
		return port;
	}
private:
	uint16_t port = 69;
	bool ip_v6 = false;
	std::array<uint8_t, 16> IP_address{};
	std::string ip;
};