#include "hooks.h"

#include "../logo.h"
#include "../cheats/cheats.h"
#include "../directx/directx.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

hooks g_hooks;

using world_event_fn = int(__stdcall*)();
using end_scene_fn = long(__stdcall*)(IDirect3DDevice9*);
using reset_fn = long(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

world_event_fn world_event_original{};
end_scene_fn end_scene_original{};
reset_fn reset_original{};

std::once_flag m_initialize_imgui_flag,
    m_update_player_cash{};

int __stdcall world_event_h()
{
	g_cheats.initialize();
	return world_event_original();
}

WNDPROC g_wnd_proc;
LRESULT __stdcall wnd_proc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	if (m == WM_KEYDOWN && w == VK_INSERT)
		vars::ui::is_opened = !vars::ui::is_opened;

	LRESULT ImGui_ImplWin32_WndProcHandler(HWND h, UINT m, WPARAM w, LPARAM l);
	if (vars::ui::is_opened && ImGui_ImplWin32_WndProcHandler(h, m, w, l))
		return true;

	return CallWindowProcA(g_wnd_proc, h, m, w, l);
}

HWND g_hwnd;
long __stdcall end_scene_h(IDirect3DDevice9* device)
{
	if (!device)
		return end_scene_original(device);

	std::call_once(m_initialize_imgui_flag, [&]() {
		ImGui::CreateContext();

		ImGui::StyleColorsDark();

		ImGui::GetIO().IniFilename = nullptr;
		ImGui::GetIO().LogFilename = nullptr;
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

		D3DDEVICE_CREATION_PARAMETERS d3d_present_params;
		device->GetCreationParameters(&d3d_present_params);
		g_hwnd = d3d_present_params.hFocusWindow;

		D3DXCreateTextureFromFileInMemoryEx(device, &m_logo, sizeof(m_logo),
			16, 16, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0, &m_game_logo);

		if (g_hwnd != 0) {
			g_wnd_proc = (WNDPROC)SetWindowLongA(g_hwnd, GWLP_WNDPROC, (LONG_PTR)wnd_proc);
			ImGui_ImplWin32_Init(g_hwnd);
			ImGui_ImplDX9_Init(device);
			ImGui::GetIO().ImeWindowHandle = g_hwnd;
		}
	});

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::GetIO().MouseDrawCursor = vars::ui::is_opened;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.08f, 0.50f));
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 0.0f);

	ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.35f, 0.35f, 0.35f, 0.50f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.09f, 0.09f, 0.09f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.26f, 0.59f, 0.98f, 0.67f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.16f, 0.29f, 0.48f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.16f, 0.29f, 0.48f, 1.00f));

	auto ButtonWithCustomID = [](const char* label, const char* label_id)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label_id);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 size_arg = ImVec2(20, 20);
		ImGuiButtonFlags flags = ImGuiButtonFlags_None;

		ImVec2 pos = window->DC.CursorPos;
		if ((ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset)
			pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
		ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
			flags |= ImGuiButtonFlags_Repeat;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

		const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

		ImGui::RenderNavHighlight(bb, id);
		ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
		ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

		return pressed;
	};

	auto MakeTab = [&](const std::string& name, const std::string& popup_name, std::function<void(void)> f)
	{
		ImGui::Text(name.c_str());

		ImGui::SameLine();
		ImGui::SetCursorPosX(370);
		if (ButtonWithCustomID(ImGui::IsPopupOpen(popup_name.c_str()) ? "-" : "+", std::string(popup_name + "ID").c_str()))
			ImGui::OpenPopup(popup_name.c_str());

		if (ImGui::BeginPopup(popup_name.c_str()))
		{
			f();
			ImGui::EndPopup();
		}
	};

	if (vars::ui::is_opened)
	{
		user_profile* m_user_profile = fe_manager::instance->get_user_profile();

		std::call_once(m_update_player_cash, [&]() {
			vars::player::cash = m_user_profile->cash;
		});

		ImGui::SetNextWindowSize(ImVec2(400, 350));
		ImGui::Begin("Need For Speed Carbon", &vars::ui::is_opened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		{
			MakeTab("Global", "GlobalPopup", []() {
				ImGui::SliderFloat("Game Speed", &vars::global::game_speed, 0.1f, 1.0f, "%.2f");
				ImGui::SliderFloat("World Animation Speed", &vars::global::world_animation_speed, 0.1f, 45.0f, "%.2f");

				for (size_t i = 0; i < 2; i++)
					ImGui::Spacing();

				ImGui::Checkbox("Disable Cops", &vars::global::disable_cops);
			});

			MakeTab("Player", "PlayerPopup", [&]() {
				ImGui::SliderInt("Cash", &vars::player::cash, 0, 1000000);
				m_user_profile->cash = vars::player::cash;

				ImGui::SameLine();
				ImGui::Text(std::string("(" + std::to_string(m_user_profile->cash) + ")").c_str());

				ImGui::SliderInt("Starting Cash", &vars::player::starting_cash, 0, 1000000);
			});

			if (NFSC_IsInGame())
			{
				MakeTab("Car", "CarPopup", [&]() {
					ImGui::Checkbox("AI", &vars::car::ai);
					ImGui::SameLine();
					ImGui::Checkbox("Drift Mode", &vars::car::drift_mode);
					ImGui::Combo("Model", &vars::car::model_index, [](void* data, int i, const char** string) {
						*string = m_models[i];
						return true;
					}, nullptr, m_models.size(), 15);
					ImGui::SameLine();
					if (ImGui::Button("Change")) {
						if (debug_vehicle_selection::instance != nullptr)
							debug_vehicle_selection::instance->switch_model(m_models[vars::car::model_index]);
					}
				});
			}

			MakeTab("Teleport", "TeleportPopup", []() {
				if (ImGui::Button("To safe house")) {
					if (NFSC_IsInGame() && !NFSC_IsInMainMenu())
						vars::teleport::to_safe_house = true;
				}

				if (ImGui::Button("To car lot")) {
					if (NFSC_IsInGame() && !NFSC_IsInMainMenu())
						vars::teleport::to_car_lot = true;
				}
			});

			MakeTab("Boost", "BoostPopup", []() {
				ImGui::Checkbox("Infinite Nitro", &vars::boost::infinite_nitro);
				ImGui::Checkbox("Infinite SpeedBreaker", &vars::boost::infinite_speedbreaker);
			});

			MakeTab("Minimap", "MinimapPopup", []() {
				ImGui::Checkbox("Zoom", &vars::minimap::zoom_enable);
				if (vars::minimap::zoom_enable) {
					ImGui::SliderFloat("Value", &vars::minimap::zoom_value, 0.0f, 1.0f, "%.2f");
				}
			});

			MakeTab("Removal", "RemovalPopup", []() {
				ImGui::Checkbox("Remove Cars", &vars::removal::remove_cars);
				ImGui::Checkbox("Remove Car Shadow", &vars::removal::remove_car_shadow);
				ImGui::Checkbox("Remove Car Reflection", &vars::removal::remove_car_reflections);
				ImGui::Checkbox("Remove Sky", &vars::removal::remove_sky);
				ImGui::Checkbox("Remove Light Flares", &vars::removal::remove_light_flares);
			});

			MakeTab("Weather", "WeatherPopup", []() {
				ImGui::Checkbox("Rain", &vars::rain::enabled);
				if (vars::rain::enabled) {
					for (size_t i = 0; i < 2; i++)
						ImGui::Spacing();

					ImGui::SliderFloat("Rain Size", &vars::rain::falling_rain_size, 0.0f, 0.01f, "%.2f");
					ImGui::SliderFloat("Rain Amount", &vars::rain::rain_amount, 0.0f, 1.0f, "%.2f");
					ImGui::SliderFloat("Rain Intensity", &vars::rain::rain_intensity, 0.0f, 0.45f, "%.2f");
					ImGui::SliderFloat("Rain Gravity", &vars::rain::rain_gravity, 0.0f, 0.35f, "%.2f");
					ImGui::SliderFloat("Rain Xing", &vars::rain::rain_xing, 0.0f, 0.02f, "%.2f");
					ImGui::SliderFloat("Rain Fallspeed", &vars::rain::rain_fallspeed, 0.0f, 0.03f, "%.2f");
				}
			});

			ImGui::Separator();

			static auto things_unlocked = false;

			if (!things_unlocked) {
				if (ImGui::Button("Unlock All Things")) {
					g_mem.write<unsigned char>(0xA9E6C0, 0, true);
					things_unlocked = true;
				}
			}
		}
		ImGui::End();
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return end_scene_original(device);
}

long __stdcall reset_h(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* present_params) {
	if (!device)
		return reset_original(device, present_params);

	D3DDEVICE_CREATION_PARAMETERS params{ 0 };
	ImGui_ImplDX9_InvalidateDeviceObjects();
	auto ret = reset_original(device, present_params);
	ImGui_ImplDX9_CreateDeviceObjects();

	return ret;
}

void hooks::initialize()
{
	if (MH_Initialize() != MH_OK)
		return;
	
	MH_CreateHook(reinterpret_cast<world_event_fn>(0x7DCD20),
		world_event_h, reinterpret_cast<void**>(&world_event_original));

	unsigned long* DVTable = (unsigned long*)g_directx.get_device();
	DVTable = (unsigned long*)DVTable[0];

	MH_CreateHook((end_scene_fn)DVTable[42],
		end_scene_h, reinterpret_cast<void**>(&end_scene_original));

	MH_CreateHook((reset_fn)DVTable[16],
		reset_h, reinterpret_cast<void**>(&reset_original));

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		return;
}

void hooks::undo()
{
	MH_Uninitialize();
	MH_DisableHook(MH_ALL_HOOKS);
}