#include <Magick++.h>
#include "image.h"
#include "tools/debug.h"

const void* Image::Read(const std::string& path, int& width, int& height) {
	Magick::Image image;
	static Magick::Blob blob;

	try {
		image.read(path.c_str());
		image.write(&blob, "RGBA");
		width = image.columns();
		height = image.rows();
	}
	catch (Magick::Error& err) {
		Debug::LogError("failed to load " + path + ": " + err.what());
		return nullptr;
	}

	return blob.data();
}

bool Image::Encode(int width, int height, std::vector<unsigned char>& data, const char* encoder) {
	Magick::Image image;
	try {
		image.read(width, height, "RGBA", Magick::CharPixel, &data[0]);
		image.flip();

		Magick::Blob blob;
		image.magick(encoder);
		image.write(&blob);
		unsigned char* ptr = (unsigned char*)blob.data();
		data.assign(ptr, ptr + blob.length());
	}
	catch (Magick::Error& err) {
		Debug::LogError(std::string("failed to encode image: ") + err.what());
		return false;
	}

	return true;
}
