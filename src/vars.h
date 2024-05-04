#pragma once

namespace vars
{
	namespace ui
	{
		inline bool is_opened = false;
	}

	namespace global
	{
		inline float game_speed = 1.0f;
		inline float world_animation_speed = 45.0f;

		inline bool disable_cops;
	}

	namespace player
	{
		inline int cash, starting_cash = 15000;
	}

	namespace car
	{
		inline int model_index;
		inline bool ai;
		inline bool drift_mode;
	}

	namespace teleport
	{
		inline bool to_safe_house, to_car_lot;
	}

	namespace boost
	{
		inline bool infinite_nitro;
		inline bool infinite_speedbreaker;
	}

	namespace minimap
	{
		inline bool zoom_enable;
		inline float zoom_value = 1.0f;
	}

	namespace removal
	{
		inline bool remove_cars;
		inline bool remove_car_shadow;
		inline bool remove_car_reflections;
		inline bool remove_sky;
		inline bool remove_light_flares;
	}

	namespace rain
	{
		inline bool enabled;

		inline float falling_rain_size = 0.01f;
		inline float rain_amount = 1.0f;
		inline float rain_intensity = 0.45f;
		inline float rain_xing = 0.02f;
		inline float rain_fallspeed = 0.03f;
		inline float rain_gravity = 0.35f;
	}
};