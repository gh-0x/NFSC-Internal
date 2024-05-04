#pragma once

#include "../std.h"

#include "../sdk/functions.h"
#include "../sdk/fe_manager.h"
#include "../sdk/debug_vehicle_selection.h"

struct cheats {
private:
	void global_hacks();
	void weather_hacks();
	void teleport_hacks();
	void boost_hacks();
	void removal_hacks();
	void minimap_hacks();
	void car_hacks();
public:
	void initialize();
};

extern cheats g_cheats;