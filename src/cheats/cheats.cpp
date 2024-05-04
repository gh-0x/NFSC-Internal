#include "cheats.h"

cheats g_cheats;

void cheats::initialize()
{
	g_mem.write<int>(0x4C4CD7, vars::player::starting_cash, true);

	if (!NFSC_IsInGame())
		return;

	global_hacks();
	weather_hacks();
	teleport_hacks();
	boost_hacks();
	removal_hacks();
	minimap_hacks();
	car_hacks();

	NFSC_SetCopsEnabled(!vars::global::disable_cops);
}

void cheats::global_hacks()
{
	if (vars::global::game_speed != 1.0f)
		g_mem.write<float>(0xA712AC, vars::global::game_speed, true);
	else
		g_mem.undo(0xA712AC);

	if (vars::global::world_animation_speed != 45.f)
		g_mem.write<float>(0xA798B4, vars::global::world_animation_speed, true);
	else
		g_mem.undo(0xA798B4);
}

void cheats::weather_hacks()
{
	switch (vars::rain::enabled) {
	case true: {
		g_mem.write<unsigned char>(0xB74D20, 0x01, true);
		g_mem.write<unsigned char>(0x7D83C9, 0xEB, true);

		g_mem.write<float>(0xA7984C, vars::rain::falling_rain_size, true);
		g_mem.write<float>(0xA797D0, vars::rain::rain_amount, true);
		g_mem.write<float>(0xA79850, vars::rain::rain_intensity, true);
		g_mem.write<float>(0xA797E0, vars::rain::rain_xing, true);
		g_mem.write<float>(0xA797E4, vars::rain::rain_fallspeed, true);
		g_mem.write<float>(0xA797E8, vars::rain::rain_gravity, true);
		break;
	}
	case false: {
		g_mem.undo(0xB74D20);
		g_mem.undo(0x7D83C9);

		g_mem.undo(0xA7984C);
		g_mem.undo(0xA797D0);
		g_mem.undo(0xA79850);
		g_mem.undo(0xA797E0);
		g_mem.undo(0xA797E4);
		g_mem.undo(0xA797E8);
		break;
	}
	}
}

void cheats::teleport_hacks()
{
	if (!NFSC_IsInMainMenu() && !NFSC_IsLoadingScreen()) {
		if (vars::teleport::to_safe_house) {
			NFSC_JumpToSafeHouse();
			vars::teleport::to_safe_house = false;
		}

		if (vars::teleport::to_car_lot) {
			NFSC_JumpToCarLot();
			vars::teleport::to_car_lot = false;
		}
	}
}

void cheats::boost_hacks()
{
	switch (vars::boost::infinite_speedbreaker) {
	case true: {
		g_mem.write<float>(0x9EBB8C, 5000, true);
		break;
	}
	case false: {
		g_mem.undo(0x9EBB8C);
		break;
	}
	}

	switch (vars::boost::infinite_nitro) {
	case true: {
		g_mem.nop(0x6E400B);
		g_mem.nop(0x6E400C);
		break;
	}
	case false: {
		g_mem.undo(0x6E400B);
		g_mem.undo(0x6E400C);
		break;
	}
	}
}

void cheats::removal_hacks()
{
	g_mem.write<bool>(0xA73008, !vars::removal::remove_cars, true);
	g_mem.write<bool>(0xA73010, !vars::removal::remove_car_shadow, true);
	g_mem.write<bool>(0xA7300C, !vars::removal::remove_car_reflections, true);
	g_mem.write<bool>(0xA7335C, !vars::removal::remove_sky, true);
	g_mem.write<bool>(0xA6C088, !vars::removal::remove_light_flares, true);
}

void cheats::minimap_hacks()
{
	switch (vars::minimap::zoom_enable) {
	case true: {
		g_mem.write<unsigned char>(0x5D22A6, 0x74, true);
		g_mem.write<float>(0x5D22AE, vars::minimap::zoom_value, true);
		break;
	}
	case false: {
		g_mem.undo(0x5D22A6);
		g_mem.undo(0x5D22AE);
		break;
	}
	}
}

void cheats::car_hacks()
{
	vars::car::ai ? NFSC_ForceAIControl() : NFSC_ClearAIControl();

	if (NFSC_GetRaceType() == race_type::canyon_drift || NFSC_GetRaceType() == race_type::drift) {
		NFSC_ActivateDriftMode();
	}
	else {
		vars::car::drift_mode ? NFSC_ActivateDriftMode() : NFSC_DeactivateDriftMode();
	}
}