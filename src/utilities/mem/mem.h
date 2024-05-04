#pragma once

#include "../../std.h"

#include <map>

struct mem {
	class unprotect {
		void         *addr = nullptr;
		std::size_t   size = 0;
		unsigned long prot = 0;
	public:
		unprotect(void *_addr, std::size_t _size);
		~unprotect();
	};

	struct patch_data
	{
		char* bytes = nullptr;
		size_t len = 0;

		patch_data(uintptr_t address, size_t len) : len(len)
		{
			bytes = new char[len];
			memcpy(bytes, reinterpret_cast<void*>(address), len);
		}

		~patch_data()
		{
			delete[] bytes;
		}
	};

	std::map<uintptr_t, patch_data*> patch_map;

	template <typename T>
	T &read(std::uintptr_t addr, bool vp = true) {
		unprotect prot(vp ? reinterpret_cast<void*>(addr) : nullptr, sizeof(T));
		return *reinterpret_cast<T *>(addr);
	}

	template <typename T>
	void write(std::uintptr_t addr, T val, bool vp = true) {
		unprotect prot(vp ? reinterpret_cast<void*>(addr) : nullptr, sizeof(T));

		patch_data* p = new patch_data(addr, sizeof(T));
		patch_map.insert({ addr, p });

		*reinterpret_cast<T *>(addr) = val;
	}

	template <typename T>
	void fill(std::uintptr_t addr, std::size_t count, T val, bool vp = true) {
		unprotect prot(vp ? reinterpret_cast<void*>(addr) : nullptr,
			sizeof(T) * count);

		std::fill_n(reinterpret_cast<T*>(addr), count, val);
	}

	inline void undo(std::uintptr_t addr)
	{
		if (patch_map.find(addr) == patch_map.end())
			return;

		patch_data* p = patch_map.at(addr);
		patch_map.erase(addr);

		memcpy(reinterpret_cast<void*>(addr), p->bytes, p->len);
		delete p;
	}

	inline void nop(std::uintptr_t addr, bool vp = true) {

		patch_data* p = new patch_data(addr, sizeof(addr));
		patch_map.insert({ addr, p });

		fill<unsigned char>(addr, 1, 0x90, vp);
	}

	template <typename RetT = void, typename... Args>
	RetT call(std::uintptr_t addr, Args... args) {
		return reinterpret_cast<RetT(*)(Args...)>(addr)
			(args...);
	}

	template <typename ClassT, typename RetT = void, typename... Args>
	RetT call_method(std::uintptr_t addr, ClassT _this, Args... args) {
		return reinterpret_cast<RetT(__thiscall*)(ClassT, Args...)>(addr)
			(_this, args...);
	}
};

extern mem g_mem;