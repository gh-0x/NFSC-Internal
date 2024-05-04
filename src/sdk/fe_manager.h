#pragma once

#include <cstdint>

class fe_vinyl_record {
public:
	uint8_t pad[0x1C];							        // 00-1C
};

class vehicle_customizations {
public:
	uint8_t pad[0x42C];									// 000-42C

	vehicle_customizations()
	{
		((int(__thiscall*)(vehicle_customizations*))(0x412790))(this);
	}
};

class fe_customization_record {
public:
	uint8_t pad[0x24];									// 000-024
	uint32_t m_nEngineUpgrade;							// 024-028
	uint8_t pad2[0x440];								// 028-468
	float m_fRideHeight;								// 468-46C
	uint8_t pad3[0x4];									// 46C-470

	void write_to_game(uint32_t hash, vehicle_customizations* dest)
	{
		((void(__thiscall*)(fe_customization_record*, uint32_t, vehicle_customizations*))(0x4BAD10))(this, hash, dest);
	}
};

class fe_car_record {
public:
	enum flags
	{
		LIST_STOCK = 1,
		LIST_CAREER = 2,
		LIST_MY_CARS = 4,
		LIST_BONUS = 8,
		LIST_CUSTOM = 16,
	};

	uint32_t Handle;							// 00-04
	uint32_t FEKey;								// 04-08
	uint32_t VehicleKey;						// 08-0C
	uint32_t FilterBits;						// 0C-10
	uint8_t Customization;						// 10-11
	uint8_t CareerHandle;						// 11-12
	uint8_t IsPresetSkin;						// 12-13
	uint8_t pad;								// 13-14

	uint32_t get_name_hash()
	{
		return ((uint32_t(__thiscall*)(fe_car_record*))0x4C43A0)(this);
	}

	uint32_t get_fe_key()
	{
		return ((uint32_t(__thiscall*)(fe_car_record*))0x4AE1D0)(this);
	}

	char* get_debug_name()
	{
		return ((char* (__thiscall*)(fe_car_record*))0x4AE320)(this);
	}
};

class fe_player_car_db {
public:
	fe_car_record CarTable[200];					// 00234-011D4
	fe_customization_record Customizations[75];	    // 011D4-15EA4

	fe_car_record* get_car_record_by_handle(uint32_t handle)
	{
		return ((fe_car_record*(__thiscall*)(fe_player_car_db*, uint32_t))0x49C020)(this, handle);
	}

	fe_car_record* create_new_car_record()
	{
		return ((fe_car_record*(__thiscall*)(fe_player_car_db*))0x4BA1F0)(this);
	}
};

class user_profile {
public:
	uint8_t pad[0x10];							// 00000-00010
	char name[16];								// 00010-00020
	uint8_t pad2[0x214];						// 00020-00234
	fe_player_car_db mCarStable;				// 00234-15EA4
	uint8_t pad_15EA4[0x9678];					// 15EA4-1F51C
	fe_vinyl_record m_aVinyls[700];				// 1F51C-241AC
	uint8_t pad4[0x184];						// 241AC-24330
	int cash;							        // 24330-24334
	uint8_t pad5[0x6DF4];						// 24334-2B128
	uint32_t m_aQuickRaceHandles[4];			// 2B128-2B138
};

class fe_manager {
public:
	static inline fe_manager*& instance = *(fe_manager**)0xA97A7C;
	static inline int& mPauseRequest = *(int*)0xA8AD18;

	user_profile* get_user_profile(int id = 0)
	{
		return ((user_profile*(__thiscall*)(fe_manager*, int))(0x572B90))(this, id);
	}
};