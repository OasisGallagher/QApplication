#pragma once
#include <string>
#include <glm/glm.hpp>

template<class CountofType, size_t sizeOfArray>
inline char(*__countof_helper(CountofType(&_Array)[sizeOfArray]))[sizeOfArray] {
	return nullptr;
}

#define CountOf(array) (sizeof(*__countof_helper(array)) + 0)
#define Padding(size) char __unused[size]

class Mathf {
public:
	static int MakeDword(int low, int high);
	static int Loword(int dword);
	static int Highword(int dword);

	static float Degrees(float radians);
	static float Radians(float degrees);

	static glm::vec3 Degrees(const glm::vec3& radians);
	static glm::vec3 Radians(const glm::vec3& degrees);

	static bool Approximately(float x, float y = 0.f);

	static float Luminance(const glm::vec3& color);
private:
	Mathf();
};

inline int Mathf::Loword(int dword) {
	return dword & 0xffff;
}

inline int Mathf::Highword(int dword) {
	return (dword >> 16) & 0xffff;
}

inline int Mathf::MakeDword(int low, int high) {
	return (low & 0xffff) | ((high & 0xffff) << 16);
}

inline float Mathf::Degrees(float radians) {
	return radians * 57.29578f;
}

inline float Mathf::Radians(float degrees) {
	return degrees * 0.0174532924f;
}

inline glm::vec3 Mathf::Degrees(const glm::vec3 & radians) {
	return glm::vec3(Degrees(radians.x), Degrees(radians.y), Degrees(radians.z));
}

inline glm::vec3 Mathf::Radians(const glm::vec3 & degrees) {
	return glm::vec3(Radians(degrees.x), Radians(degrees.y), Radians(degrees.z));
}

inline bool Mathf::Approximately(float x, float y) {
	const float E = 0.000001f;
	return fabs(x - y) < E;
}

inline float Mathf::Luminance(const glm::vec3& color) {
	return 0.299f * color.r + 0.587f * color.g + 0.114f * color.b;
}
