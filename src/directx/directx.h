#pragma once

#include "../std.h"

struct directx {
private:
	IDirect3DDevice9* device;
public:
	void initialize(IDirect3D9* IDirect3D9);
	IDirect3DDevice9* get_device() {
		return device;
	}
};

extern directx g_directx;