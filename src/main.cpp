// TFTP1.cpp: определяет точку входа для приложения.
//

#include <parser.h>
#include <fstream>
#include <filesystem>
#include <error.h>
#include <file_packet.h>
#include <packet.h>


constexpr std::string_view octet = "octet";
constexpr std::string_view ascii = "netascii";
constexpr std::string_view tsz = "tsize";
constexpr std::string_view mc = "multicast";
constexpr std::string_view timeo = "timeout";
constexpr std::string_view tsz_0 = "0";



void Read(Options opt, Socket s)
{
	std::cout << "Request READ from server " << opt.ip.IP() << ':' << opt.ip.Port() << '\n';

	std::fstream file(std::filesystem::path{ opt.f_name }, std::ios::binary | std::ios::out);
	std::vector<uint8_t> paket{ {0,1} };
	std::vector<uint8_t> ACK{ {0,4} };

	paket.resize(paket.size() + opt.f_name.size() + 1);
	std::copy(opt.f_name.begin(), opt.f_name.end(), paket.begin() + 2);

	size_t pos = paket.size();
	if (opt.mod == false) {
		paket.resize(paket.size() + octet.size() + 1);
		std::copy(octet.begin(), octet.end(), paket.begin() + pos);
	}
	else
	{
		paket.resize(paket.size() + ascii.size() + 1);
		std::copy(ascii.begin(), ascii.end(), paket.begin() + pos);
	}
	pos = paket.size();
	paket.resize(paket.size() + tsz.size() + 1);
	std::copy(tsz.begin(), tsz.end(), paket.begin() + pos);

	pos = paket.size();
	paket.resize(paket.size() + tsz_0.size() + 1);
	std::copy(tsz_0.begin(), tsz_0.end(), paket.begin() + pos);

	if (opt.multicast)
	{
		pos = paket.size();
		paket.resize(paket.size() + mc.size() + 2);
		std::copy(mc.begin(), mc.end(), paket.begin() + pos);
	}
	if (opt.timeout)
	{
		pos = paket.size();
		paket.resize(paket.size() + timeo.size() + 1);
		std::copy(timeo.begin(), timeo.end(), paket.begin() + pos);

		auto to = std::to_string(opt.time_out);
		pos = paket.size();
		paket.resize(paket.size() + to.size() + 1);
		std::copy(to.begin(), to.end(), paket.begin() + pos);
	}
	//paket = [02][....][0][mode][0] ... opt
	s.Send(paket, &opt.ip);

	paket.resize(516);
	ACK.resize(512);

	IPParser server;

	int size = 0;
	size = s.RecieveFrom(paket, server); //get back 

	Packet pack;
	pack.Parse(paket, size_t(size), &opt);
	opt.Print();
	ACK[2] = ACK[3] = 0;
	if (pack.Code() == 6) //if OACK packet
	{
		s.Send(ACK, &server);
		size = s.Recieve(paket);
	}

	do {
		if (size < 0)
			throw Exception{ GetSockError() };
		pack.Parse(paket, size);

		if (opt.mod)ConvertFromNetascii(paket);
		file.write((const char*)paket.data() + 4, size - 4);
		ACK[2] = paket[2];
		ACK[3] = paket[3];
		s.Send(ACK, &server);
		size = s.Recieve(paket);
	} while (size == 516);
	std::cout << "Transfer completed without errors\n";

};

void Write(Options opt, Socket s) {
	std::cout << "Request WRITE to server " << opt.ip.IP() << ':' << opt.ip.Port() << '\n';

	std::fstream file(std::filesystem::path{ opt.f_name }, std::ios::binary | std::ios::in);
	Check(file.is_open(), "File Not Found");

	std::vector<uint8_t> paket{ {0,2} };
	std::vector<uint8_t> ACK{ {0,4} };


	paket.resize(paket.size() + opt.f_name.size() + 1);
	std::copy(opt.f_name.begin(), opt.f_name.end(), paket.begin() + 2);

	size_t pos = paket.size();
	if (opt.mod == false) {
		paket.resize(paket.size() + octet.size() + 1);
		std::copy(octet.begin(), octet.end(), paket.begin() + pos);
	}
	else
	{
		paket.resize(paket.size() + ascii.size() + 1);
		std::copy(ascii.begin(), ascii.end(), paket.begin() + pos);
	}
	
	pos = paket.size();
	FilePacket f(file); //load file
	if (opt.mod)f.ConvertToNetascii();

	auto x_tsize = std::to_string(f.size());

	paket.resize(paket.size() + tsz.size() + 1);
	std::copy(tsz.begin(), tsz.end(), paket.begin() + pos);

	pos = paket.size();
	paket.resize(paket.size() + x_tsize.size() + 1);
	std::copy(x_tsize.begin(), x_tsize.end(), paket.begin() + pos);

	if (opt.timeout)
	{
		pos = paket.size();
		paket.resize(paket.size() + timeo.size() + 1);
		std::copy(timeo.begin(), timeo.end(), paket.begin() + pos);

		auto to = std::to_string(opt.time_out);
		pos = paket.size();
		paket.resize(paket.size() + to.size() + 1);
		std::copy(to.begin(), to.end(), paket.begin() + pos);
	}
	//paket = [02][....][0][mode][0] ... opt

	if (s.Send(paket, &opt.ip) < 0)
		throw Exception{ GetSockError() };

	paket.resize(516);
	ACK.resize(512);
	paket[0] = 0;
	paket[1] = 3;


	IPParser server;

	int size = 0;
	size = s.RecieveFrom(ACK, server);
	Packet pack;
	pack.Parse(paket, size_t(size), &opt);//if OACK packet
	opt.Print();


	size_t block_n = 1;
	do {
		if (size < 0) throw Exception{ GetSockError() };
		pack.Parse(ACK, size);

		auto [bl, sz] = f.GetChunk(block_n - 1);
		if (!bl)break;
		paket.resize(4 + sz);
		*((uint16_t*)&paket[2]) = htons((uint16_t)block_n++);

		std::copy(bl, bl + sz, paket.begin() + 4);

		s.Send(paket, &server);
		size = s.Recieve(ACK);
	} while (true);
	std::cout << "Transfer completed without errors\n";
}





int main()
{
	WSA wsa;
	IPParser a;


	while (true)
	{
		std::string cmd;
		std::getline(std::cin, cmd);
		try {
			auto opt = parser(cmd);
			if (!opt) continue;

			Socket s{ opt->ip.IsV6() ? AF_INET6 : AF_INET };
			s.SetTimeout(5);

			if (opt->operation == Options::read)
				Read(*opt, s);
			if (opt->operation == Options::write)
				Write(*opt, s);
		}
		catch (const Exception& e) {
			e.print();
		}
	}


	return 0;
}
