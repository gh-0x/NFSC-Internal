#include "directx.h"

directx g_directx;

void directx::initialize(IDirect3D9* IDirect3D9)
{
	HWND g_hwnd = CreateWindowA("BUTTON", "x", WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, 0, 0, GetModuleHandleA(0), 0);

	if (g_hwnd == NULL)
		return;

	if (IDirect3D9 == NULL) {
		DestroyWindow(g_hwnd);
		return;
	}

	D3DPRESENT_PARAMETERS d3d_present_params;
	ZeroMemory(&d3d_present_params, sizeof(d3d_present_params));

	d3d_present_params.Windowed = TRUE;
	d3d_present_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3d_present_params.hDeviceWindow = g_hwnd;
	d3d_present_params.BackBufferFormat = D3DFMT_UNKNOWN;

	if (IDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3d_present_params, &device) != D3D_OK) {
		IDirect3D9->Release();
		DestroyWindow(g_hwnd);
		return;
	}

	if (device == NULL) {
		IDirect3D9->Release();
		DestroyWindow(g_hwnd);
		return;
	}
}