#pragma once

#include "../utilities/mem/mem.h"

enum class race_type : int
{
	freeplay = -1,
	sprint = 0,
	circuit = 1,
	drag = 2,
	ko = 3,
	tollbooth = 4,
	speedtrap = 5,
	checkpoint = 6,
	cash_grab = 7,
	challenge_series = 8,
	jump_to_speedtrap = 9,
	jump_to_milestone = 10,
	drift = 11,
	canyon_drift = 12,
	canyon = 13,
	pursuit_tag = 14,
	pursuit_ko = 15,
	encounter = 16,
	max = 17
};

inline bool NFSC_IsInGame() { return *(int*)0xA99BBC == 6; }
inline bool NFSC_IsInMainMenu() { return *(int*)0xA99BBC == 3; }
inline bool NFSC_IsLoadingScreen() { return *(int*)0xA99BBC == 4; }
inline void NFSC_ActivateDriftMode() { ((void(__cdecl*)())0x65C8B0)(); }
inline void NFSC_DeactivateDriftMode() { ((void(__cdecl*)())0x65C910)(); }
inline void NFSC_SetCopEnabled(bool v) { ((void(__cdecl*)(bool))0x6513E0)(v); }
inline void NFSC_ForceAIControl(int unk = 1) { ((void(__cdecl*)(int))0x65C2C0)(unk); }
inline void NFSC_ClearAIControl(int unk = 1) { ((void(__cdecl*)(int))0x65C330)(unk); }
inline void NFSC_JumpToSafeHouse() { ((void(__cdecl*)())0x64B800)(); }
inline void NFSC_JumpToCarLot() { ((void(__cdecl*)())0x64B7A0)(); }
inline void NFSC_SetCopsEnabled(bool v) { ((void(__cdecl*)(bool))0x6513E0)(v); }

inline race_type NFSC_GetRaceType()
{
	uintptr_t g_race_status = g_mem.read<uintptr_t>(0xA98284);

	if (!g_race_status)
		return race_type::freeplay;

	uintptr_t race_parameters = g_mem.read<uintptr_t>(g_race_status + 0x6A1C);

	if (!race_parameters)
		return race_type::freeplay;

	return reinterpret_cast<race_type(__thiscall*)(uintptr_t)>(0x6136A0)(race_parameters);
}