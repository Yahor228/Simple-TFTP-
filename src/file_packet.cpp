#include <file_packet.h>
#include <fstream>

FilePacket::FilePacket(std::fstream& stream)
{
	std::streampos fileSize;
	stream.seekg(0, std::ios::end);
	fileSize = stream.tellg();
	stream.seekg(0, std::ios::beg);

	file.resize(fileSize);

	stream.read((char*)file.data(), fileSize);
}