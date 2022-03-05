#include <IPParser.h>
#include <iostream>
#include <error.h>

IPParser::IPParser(sockaddr_storage stor)
{
	if (stor.ss_family == AF_INET)
	{
		sockaddr_in& addr = (sockaddr_in&)stor;
		std::copy((uint8_t*)&addr.sin_addr, (uint8_t*)&addr.sin_addr+4, IP_address.begin());
		port = ntohs(addr.sin_port);
	}
	else if (stor.ss_family == AF_INET6)
	{
		ip_v6 = true;
		sockaddr_in6& addr = (sockaddr_in6&)stor;
		std::copy((uint8_t*)&addr.sin6_addr, (uint8_t*)&addr.sin6_addr+16, IP_address.begin());
		port = ntohs(addr.sin6_port);
	}
}

bool IPParser::Parse(std::string_view ip_port)
{
	auto comma = ip_port.find(',');
	Check(comma != ip_port.npos, "No port was specified");

	ip = std::string(ip_port.data(), comma);
	port = uint16_t(std::stoi(std::string(ip_port.begin() + comma + 1, ip_port.end())));

	int dvoetocie = 0;
	for (auto i = ip.begin(); i < ip.end(); i++) {
		if (*i == ':')
			dvoetocie++;
	}
	if (dvoetocie >= 2)
		ip_v6 = true;


	if (ip_v6 == false) {
		in_addr addr{};
		if (inet_pton(AF_INET, ip.data(), &addr) == 1 && addr.s_addr != INADDR_NONE) {
			std::copy((uint8_t*)&addr.s_addr, (uint8_t*)&addr.s_addr + 4, IP_address.data());
		}
		else {
			std::cerr << "Wrong IP address\n";
			return false;
		}
	}
	else {
		in6_addr addr{};
		if (inet_pton(AF_INET6, ip.data(), &addr) == 1) {
			std::copy((uint8_t*)&addr, (uint8_t*)&addr + 16, IP_address.data());
		}
		else {
			std::cerr << "Wrong IP address\n";
			return false;
		}

	}
	return true;
}
sockaddr_in IPParser::convert_to_v4()const {
	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	std::copy(IP_address.cbegin(), IP_address.cbegin() + size_t(4), (uint8_t*)&addr.sin_addr);
	addr.sin_port = htons(port);
	return addr;
}
sockaddr_in6 IPParser::convert_to_v6()const {
	sockaddr_in6 addr{};
	addr.sin6_family = AF_INET6;
	std::copy(IP_address.cbegin(), IP_address.cbegin() + size_t(16), (uint8_t*)&addr.sin6_addr);
	addr.sin6_port = htons(port);
	return addr;
}