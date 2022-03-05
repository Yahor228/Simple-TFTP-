// TFTP1.h : включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта.

#pragma once
#include <iostream>
#include <cstring>
#include <array>
#include <IPParser.h>

// TODO: установите здесь ссылки на дополнительные заголовки, требующиеся для программы.
struct Options {
	union
	{
		struct
		{
			bool filename:1 ;
			bool timeout :1 ;
			bool block_size : 1;
			bool address : 1;
			bool multicast : 1;
			bool mod : 1; // ascii == true ; octet == false
			bool t_size : 1;
		};
		uint8_t val;
	};


	enum Operation {
		no_opt, read, write

	};

	int time_out = 0;
	int blocksize = 512;
	size_t tsize = 0;
	IPParser ip;
	IPParser mul_ip;

	Operation operation;
	std::string_view f_name;

	Options()
		:val(0)
	{	
		ip.Parse("127.0.0.1,69");
	}

	void Print()const
	{
		if (!val)return;
		std::cout << "Options accepted:\n";
		if (timeout)
			std::cout << "\ttimeout : " << time_out << '\n';
		if (multicast)
			std::cout << "\tmulticast : " << mul_ip.IP() <<':' << mul_ip.Port() << '\n';
		if (t_size)
			std::cout << "\ttsize : " << tsize << '\n';
	}
};

