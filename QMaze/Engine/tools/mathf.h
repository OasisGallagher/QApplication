#pragma once
#include <string>
#include <glm/glm.hpp>

template<class _CountofType, size_t _SizeOfArray>
inline char(*__countof_helper(_CountofType(&_Array)[_SizeOfArray]))[_SizeOfArray] {
	return nullptr;
}

#define CountOf(_Array) (sizeof(*__countof_helper(_Array)) + 0)
#define Padding(_Size) char __unused[_Size]

class Mathf {
public:
	static int MakeDword(int low, int high);
	static int Loword(int dword);
	static int Highword(int dword);

	static bool Approximately(float x, float y = 0.f);

	static float Luminance(const glm::vec3& color);
private:
	Mathf();
};

inline int Mathf::Loword(int dword) {
	return dword & 0xffff;
}

inline bool Mathf::Approximately(float x, float y) {
	const float E = 0.000001f;
	return fabs(x - y) < E;
}

inline float Mathf::Luminance(const glm::vec3& color) {
	return 0.299f * color.r + 0.587f * color.g + 0.114f * color.b;
}

inline int Mathf::Highword(int dword) {
	return (dword >> 16) & 0xffff;
}

inline int Mathf::MakeDword(int low, int high) {
	return (low & 0xffff) | ((high & 0xffff) << 16);
}
