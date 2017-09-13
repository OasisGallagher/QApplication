#pragma once

class Memory {
public:
	template <class Ty>
	static Ty* Create() {
		return new Ty();
	}

	template <class Ty, class P>
	static Ty* Create(const P& p) {
		return new Ty(p);
	}

	template <class Ty, class P0, class P1>
	static Ty* Create(const P0& p0, const P1& p1) {
		return new Ty(p0, p1);
	}

	template <class Ty>
	static void Release(Ty* pointer) {
		delete pointer;
	}

	template <class Ty>
	static Ty* CreateArray(size_t n) {
		return new Ty[n];
	}

	template <class Ty>
	static void ReleaseArray(Ty* pointer) {
		delete[] pointer;
	}
};
