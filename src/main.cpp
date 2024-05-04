#include "hooks/hooks.h"
#include "directx/directx.h"

bool WINAPI DllMain(HMODULE I, int rtc, void* reversed)
{
	switch (rtc) {
	case DLL_PROCESS_ATTACH: {
		DisableThreadLibraryCalls(I);
		if (auto h = CreateThread(0, 0, [](void* I) -> unsigned long {
			g_directx.initialize(Direct3DCreate9(D3D_SDK_VERSION));
			g_hooks.initialize();
			return 1;
		}, I, 0, 0)) CloseHandle(h);
		break;
	}
	case DLL_PROCESS_DETACH: {
		g_hooks.undo();
		break;
	}
	}

	return true;
}