#include "mem.h"

mem g_mem;

mem::unprotect::unprotect(void *_addr, std::size_t _size) :
	addr(_addr), size(_size) {
	if (addr && size)
		VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &prot);
}

mem::unprotect::~unprotect() {
	if (addr && size)
		VirtualProtect(addr, size, prot, &prot);
}