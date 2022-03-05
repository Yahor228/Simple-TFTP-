#pragma once 
#include <options.h>
#include <cctype>
#include <string>
#include <socket.h>
#include <optional>

std::optional<Options> parser(std::string_view cmd)
{
	Options opt;
	if (cmd.find("-R") != cmd.npos) {
		opt.operation = opt.read;
	}

	if (cmd.find("-W") != cmd.npos) {
		opt.operation = opt.write;
	}


	if (opt.operation == opt.no_opt) {
		std::cerr << "Wrong arguments: No operations specified";
		return {};
	}


	if (cmd.find("-d") != cmd.npos) {
		opt.filename = true;
		bool kavychka = false;
		const char* begin = cmd.data() + cmd.find("-d") + 2;

		while (!(std::isalnum(*begin) || *begin == '_')) { if (*begin++ == '\"') { kavychka = true; break; } }

		const char* end = begin;
		if (kavychka == true) {
			while (*end != '\"') end++;
		}
		else
			while (!std::isspace(*end) && *end)end++;
		opt.f_name = std::string_view(begin, end - begin);

	}
	else {
		std::cerr << "Wrong arguments: No filename";
		return {};
	}



	if (cmd.find("-t") != cmd.npos) {

		const char* begin = cmd.data() + cmd.find("-t") + 2;
		while (!(std::isdigit(*begin)) && *begin) {
			begin++;
		}

		char* end_pointer = 0;

		opt.time_out = strtol(begin, &end_pointer, 10);
		if (end_pointer == begin) return {};

		opt.timeout = true;

	}

	if (cmd.find("-s") != cmd.npos) {

		const char* begin = cmd.data() + cmd.find("-s") + 2;
		while (!(std::isdigit(*begin)) && *begin) {
			begin++;
		}

		char* end_pointer = 0;

		opt.blocksize = strtol(begin, &end_pointer, 10);
		if (end_pointer == begin) return {};

		opt.block_size = true;

	}

	if (cmd.find("-m") != cmd.npos) {
		opt.multicast = true;
	}

	if (cmd.find("-c") != cmd.npos) {
		const char* begin = cmd.data() + cmd.find("-c") + 2;
		while (!(std::isalnum(*begin)) && *begin) {
			begin++;
		}

		const char* end = begin;
		while (!isspace(*end) && *end) {
			end++;
		}

		auto kusok_argumenta = std::string_view(begin, end - begin);

		if ((kusok_argumenta == "ascii") || (kusok_argumenta == "netascii")) {
			opt.mod = true;
		}
		else if (kusok_argumenta != "octet" || kusok_argumenta != "binary") {
			std::cerr << "Wrong arguments: Wrong mod";
			return {};
		}
	}

	if (cmd.find("-a") != cmd.npos) { // -a 192.168.1.235,69
		const char* begin = cmd.data() + cmd.find("-a") + 2;
		while (isspace(*++begin));
		const char* end = begin;

		while (!isspace(*end) && *end) 
			end++;
		opt.ip.Parse({ begin, (size_t)(end - begin) });
	}
	return { opt };
}
