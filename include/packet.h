#pragma once
#include <vector>
#include <error.h>

#include <options.h>
#include <sstream>


class Packet
{
	std::vector<std::string_view> GetArgs(const std::vector<uint8_t>& packet, size_t size)
	{
		std::vector<std::string_view> arg;
		const char* beg = (const char*)packet.data() + 2;
		size_t pos = 2;
		while (pos < size) // block option separation by \0
		{
			auto& out = arg.emplace_back(std::string_view{ beg });
			pos += out.size() + 1;
			beg += out.size() + 1;
		}
		return arg;
	}
public:
	Packet() = default;
	Packet(const std::vector<uint8_t>& packet, size_t size)
	{
		Parse(packet, size);
	}
	auto Code()const
	{
		return code;
	}
	void Parse(const std::vector<uint8_t>& packet, size_t size, Options* opt = nullptr)
	{
		code = packet[1];
		auto args = GetArgs(packet, size);

		if(opt)opt->val = 0;


		std::ostringstream oss;
		switch (code)
		{
		case 5:
			oss << "[Error code]:" << ntohs(*(uint16_t*)args[0].data()) << ":\n" << args[1].data() + 1;
			Check(false, oss.str());
			break;
		case 6:
			if (!opt)return;
			for (size_t i = 0; i < args.size(); i+=2)
			{
				const auto& o = args[i]; //option
				const auto& arg = args[i+1]; //option argument
				if (o == "tsize")
				{
					opt->t_size = true;
					opt->tsize = std::stoi(std::string{ arg });
				}
				if (o == "multicast")
				{
					opt->multicast = true;
					size_t pos = arg.rfind(",");
					opt->mul_ip.Parse(arg.substr(0, pos));
				}
				if (o == "timeout")
				{
					opt->timeout = true;
					opt->time_out = std::stoi(std::string{ arg });
				}
			}
		}
	}
private:
	uint8_t code = 0;
};