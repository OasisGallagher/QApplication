#pragma once
#include <vector>
#include <string>

class Image {
public:
	static const void* Read(const std::string& path, int& width, int& height);
	static bool Encode(int width, int height, std::vector<unsigned char>& data, const char* format);
};
