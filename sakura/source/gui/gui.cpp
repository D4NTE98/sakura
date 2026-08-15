#include "../client.h"
#include "../fonts/icons.h"
#include "../fonts/weapons.h"
#include "../fonts/font.h"

HWND hGameWnd;
WNDPROC hGameWndProc;
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK HOOK_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (bShowMenu || popoup)
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	return CallWindowProc(hGameWndProc, hWnd, uMsg, wParam, lParam);
}

bool bOldOpenGL = true; 
GLint iMajor, iMinor;
bool bInitializeImGui = false;
void InistalizeImgui(HDC hdc)
{
	if (!bInitializeImGui)
	{
		hGameWnd = WindowFromDC(hdc);
		hGameWndProc = (WNDPROC)SetWindowLong(hGameWnd, GWL_WNDPROC, (LONG)HOOK_WndProc);
		glGetIntegerv(GL_MAJOR_VERSION, &iMajor);
		glGetIntegerv(GL_MINOR_VERSION, &iMinor);
		if ((iMajor * 10 + iMinor) >= 32)
			bOldOpenGL = false;
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hGameWnd);
		if (!bOldOpenGL)
		{
			ImplementGl3();
			ImGui_ImplOpenGL3_Init();
		}
		else
			ImGui_ImplOpenGL2_Init();

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		style.AntiAliasedFill = !bOldOpenGL;
		style.AntiAliasedLines = !bOldOpenGL;
		style.WindowRounding = 12.0f;
		style.ChildRounding = 10.0f;
		style.FrameRounding = 7.0f;
		style.PopupRounding = 10.0f;
		style.ScrollbarRounding = 8.0f;
		style.GrabRounding = 6.0f;
		style.WindowPadding = ImVec2(10.0f, 10.0f);
		style.FramePadding = ImVec2(8.0f, 5.0f);
		style.ItemSpacing = ImVec2(8.0f, 7.0f);
		style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
		style.ScrollbarSize = 9.0f;
		style.WindowBorderSize = 0.0f;
		style.FrameBorderSize = 0.0f;
		style.PopupBorderSize = 1.0f;
		style.WindowMinSize = ImVec2(10.0f, 10.0f);
		ImGui::GetIO().IniFilename = NULL;
		ImGui::GetIO().LogFilename = NULL;

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Text] = ImVec4(0.94f, 0.94f, 0.96f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.49f, 0.50f, 0.56f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.045f, 0.047f, 0.058f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.060f, 0.062f, 0.074f, 0.99f);
		colors[ImGuiCol_Border] = ImVec4(0.18f, 0.18f, 0.22f, 0.65f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.105f, 0.108f, 0.128f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.145f, 0.148f, 0.175f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.175f, 0.178f, 0.205f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.16f, 0.16f, 0.19f, 0.85f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.82f, 0.18f, 0.47f, 0.78f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.88f, 0.20f, 0.50f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.82f, 0.18f, 0.47f, 0.88f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.72f, 0.14f, 0.39f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.82f, 0.18f, 0.47f, 0.85f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.94f, 0.25f, 0.57f, 1.00f);

		ImFontConfig config;
		config.MergeMode = true;
		ImGui::GetIO().Fonts->AddFontDefault();
        ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Droid_compressed_data, Droid_compressed_size, 12.f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        Sakura::Menu::Fonts::weaponsFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(compressedWeaponsData, compressedWeaponsSize, 55.f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        Sakura::Menu::Fonts::icons = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(compressedIconsData, compressedIconsSize, 22.f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        Sakura::Menu::Fonts::titleTabFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Droid_compressed_data, Droid_compressed_size, 14.f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        Sakura::Menu::Fonts::titleCheatFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Droid_compressed_data, Droid_compressed_size, 20.f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        Sakura::Menu::Fonts::defaultFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Droid_compressed_data, Droid_compressed_size, 12.f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
		ImGui::GetIO().Fonts->Build();

		if (cvar.gui_key < 0 || cvar.gui_key > 254)
			cvar.gui_key = K_INS;

		bInitializeImGui = true;
	}
	static ImVec2 screensize = ImGui::GetIO().DisplaySize;
	if (screensize.x != ImGui::GetIO().DisplaySize.x || screensize.y != ImGui::GetIO().DisplaySize.y)
	{
		//ImGui::GetStyle().WindowMaxSize = ImGui::GetIO().DisplaySize;
		screensize = ImGui::GetIO().DisplaySize;
	}
}

bool checkdrawscreen()
{
    bool checkdrawhud = Sakura::ScreenShot::IsDrawing();
	static bool drawhud = checkdrawhud;
	if (drawhud != checkdrawhud)
	{
		drawhud = checkdrawhud;
		return true;
	}
	return false;
}

bool checkmenu()
{
	static bool checkmenu = bShowMenu;
	if (checkmenu != bShowMenu)
	{
		checkmenu = bShowMenu;
		return true;
	}
	return false;
}

void MenuHandle()
{
    if (checkdrawscreen())
    {
        if (bShowMenu)
        {
            if (Sakura::ScreenShot::IsDrawing())
            {
                ImGui::GetIO().MouseDrawCursor = true;

                if (bShowMenu)
                    changewindowfocus = true;
            }
            else
                ImGui::GetIO().MouseDrawCursor = false;
        }
    }
    if (checkmenu())
    {
        if (bShowMenu)
        {
            ImGui::GetIO().MouseDrawCursor = true;
            g_Client.IN_DeactivateMouse();

            if (bShowMenu)
                changewindowfocus = true;
        }
        else
        {
            SetCursorPos(g_Engine.GetWindowCenterX(), g_Engine.GetWindowCenterY());
            ImGui::GetIO().MouseDrawCursor = false;
            g_Client.IN_ActivateMouse();
        }
    }

    if ((bShowMenu) && ::GetActiveWindow() == hGameWnd)
    {
        POINT ppt;
        if (GetCursorPos(&ppt))
        {
            if (ppt.x == g_Engine.GetWindowCenterX() && ppt.y == g_Engine.GetWindowCenterY())
                g_Client.IN_DeactivateMouse();
        }
    }
}

void ClearHudKeys()
{
	for (size_t i = 0; i < 256; ++i)
	{
		if (keysmenu[i] == true)
			keysmenu[i] = false;
	}
}

void ClearSound()
{
	if (Sound_No_Index.size() && GetTickCount() - Sound_No_Index.front().timestamp > 900)
		Sound_No_Index.pop_front();

	if (Sound_Index.size() && GetTickCount() - Sound_Index.front().timestamp > 900)
		Sound_Index.pop_front();
}

void HookImGui(HDC hdc)
{
	ColorChange();
	ClearSound();
	InistalizeImgui(hdc);

	if (!bOldOpenGL)
		ImGui_ImplOpenGL3_NewFrame();
	else
		ImGui_ImplOpenGL2_NewFrame();

	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (Sakura::ScreenShot::IsVisuals())
	{
		DrawFullScreenWindow();
		DrawOverview();
		DrawKzWindows();

		for (size_t i = 0; i < Sakura::Lua::scripts.size(); ++i)
		{
			auto& script = Sakura::Lua::scripts[i];

			if (!script.HasCallback(Sakura::Lua::SAKURA_CALLBACK_TYPE::SAKURA_CALLBACK_AT_RENDERING_WINDOW))
				continue;

			auto callbacks = script.GetCallbacks(Sakura::Lua::SAKURA_CALLBACK_TYPE::SAKURA_CALLBACK_AT_RENDERING_WINDOW);
			for (const auto& callback : callbacks)
			{
				try
				{
					callback();
				}
				catch (luabridge::LuaException const& error)
				{
					if (script.GetState())
					{
						Sakura::Lua::Error("Error has occured in the lua \"On Window Render\" script: %s", error.what());
						script.RemoveAllCallbacks();
					}
				}
			}
		}

        DrawMenuWindow();
	}
    DrawPopupWindow();

    ImGui::Render();
    if (!bOldOpenGL)
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    else
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	MenuHandle();
	ClearHudKeys();
}