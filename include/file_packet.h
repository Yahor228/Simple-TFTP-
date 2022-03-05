#pragma once
#include <vector>
#include <iosfwd>
#include <cstdint>

class FilePacket
{
public:
	FilePacket(std::fstream& stream);

	void ConvertToNetascii()
	{
		uint8_t before = 0;
		for (auto i = file.begin(); i < file.end(); i++)
		{
			if (*i == '\r' && ((i + 1 != file.end() && *(i + 1) != '\n') || i + 1 == file.end())) {
				i = file.insert(i + 1, '\0'); before = *i; continue;
			}

			if (*i == '\n' && before != '\r')
				i = file.insert(i, '\r');
			before = *i;
		}
	}
	std::pair<uint8_t*, size_t> GetChunk(size_t n)
	{
		if (n * 512 > file.size())
			return { nullptr, 0 };
		return { file.data() + n * 512,(file.size() - n * 512) >= 512 ? 512 : file.size() - n * 512 };
	}
	size_t size()const noexcept
	{
		return file.size();
	}
private:
	std::vector<uint8_t> file;
};

inline void ConvertFromNetascii(std::vector<uint8_t>& file)
{
	uint8_t before = 0;
	for (auto i = file.begin(); i < file.end(); i++)
	{
#ifndef _WIN32
		if (*i == '\n' && before == '\r')
			i = file.erase(i - 1);
#endif

		if (*i == '\0' && before == '\r')
			i = file.erase(i);

		if (i != file.end())
			before = *i;
	}
}
