#pragma once
#include <IPParser.h>
#include <vector>



class Socket {
	SOCKET soket;
	bool v6;
public:
	Socket(int ipver) :soket(socket(ipver, SOCK_DGRAM, IPPROTO_UDP)), v6(ipver == AF_INET6) {
	}
	~Socket(){
		closesocket(soket);
	}

	int Send(const std::vector<uint8_t>& data, const IPParser* ip) {
		if (!ip)
			return send(soket, (const char*)data.data(), data.size(), 0) > 0;
		if (ip->IsV6())
		{
			//convert to v6
			auto to = ip->convert_to_v6();
			return sendto(soket, (const char*)data.data(), data.size(), 0, (const sockaddr*)&to, sizeof(to)) > 0;
		}

		auto to = ip->convert_to_v4();
		return sendto(soket, (const char*)data.data(), data.size(), 0, (const sockaddr*)&to, sizeof(to));
	}

	int Recieve(std::vector<uint8_t>& buffer) {
		return recv(soket, (char*)buffer.data(), buffer.size(), 0);
	}
	int RecieveFrom(std::vector<uint8_t>& buffer, IPParser& c) {
		sockaddr_storage stor{};
		socklen_t len = v6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);
		auto ret = recvfrom(soket, (char*)buffer.data(), buffer.size(), 0, (sockaddr*)&stor, &len);
		c = IPParser{ stor };
		return ret;
	}
	int SetTimeout(uint32_t time_sec)
	{
#ifdef _WIN32
		uint32_t tv = 1000 * time_sec;
#else
		timeval tv{};
		tv.tv_sec = time_sec;
#endif // _WIN32
		return setsockopt(soket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	}

};




