#include "pch.h"
#include <conio.h>
#include <stdio.h>
#include <iostream>

#include <gl/glew.h>

#include <gl/GL.h>

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_win32.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "mem.h"
#include "hook.h"

#include "hacks.h"



P_PLAYER_ENT playerEnt = nullptr;
uintptr_t showCursorAddr = NULL;

typedef BOOL(__stdcall* twglSwapBuffers)(HDC hDc);
twglSwapBuffers wglSwapBuffersGateway;

typedef int(__cdecl* tSDL_ShowCursor)(int);
tSDL_ShowCursor SDL_ShowCursor;

HWND hGameWindow;
bool menuShown = true;
WNDPROC hGameWindowProc;

bool inOpt = false;

static void* current_weapon = nullptr;
static bool imGuiInitialized = false;

using namespace Hacks;


LRESULT CALLBACK windowProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_KEYDOWN && wParam == VK_DELETE) {
        menuShown = !menuShown;
        return false;
    }
    if (menuShown) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        {
            return true;
        }
        ImGuiIO& io = ImGui::GetIO();

        switch (uMsg) 
        {
        case WM_LBUTTONDOWN:
            io.MouseDown[1] = !io.MouseDown[0];
            return 0;
        case WM_RBUTTONDOWN:
            io.MouseDown[1] = !io.MouseDown[1];
            return 0;
        case WM_MBUTTONDOWN:
            io.MouseDown[2] = !io.MouseDown[2];
            return 0;
        case WM_MOUSEWHEEL:
            return 0;
        case WM_MOUSEMOVE:
            io.MousePos.x = (signed short)(lParam);
            io.MousePos.y = (signed short)(lParam >> 16);
            return 0;
        }
    }
    return CallWindowProc(hGameWindowProc, hWnd, uMsg, wParam, lParam);
}

BOOL __stdcall hkwglSwapBuffers(HDC hDc)
{
    if (!imGuiInitialized) {
        imGuiInitialized = true;
        hGameWindow = WindowFromDC(hDc);

        hGameWindowProc = (WNDPROC)SetWindowLongPtr(hGameWindow,
            GWLP_WNDPROC, (LONG_PTR)windowProc_hook);

        static int hue = 100;
        static float col_main_sat = 180.f / 255.f;
        static float col_main_val = 161.f / 255.f;
        static float col_area_sat = 124.f / 255.f;
        static float col_area_val = 100.f / 255.f;
        static float col_back_sat = 59.f / 255.f;
        static float col_back_val = 40.f / 255.f;

        ImVec4 col_text = ImColor::HSV(hue / 255.f, 20.f / 255.f, 235.f / 255.f);
        ImVec4 col_main = ImColor::HSV(hue / 255.f, col_main_sat, col_main_val);
        ImVec4 col_back = ImColor::HSV(hue / 255.f, col_back_sat, col_back_val);
        ImVec4 col_area = ImColor::HSV(hue / 255.f, col_area_sat, col_area_val);

        glewInit();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hGameWindow);
        ImGui_ImplOpenGL3_Init();
        ImGui::StyleColorsDark();
        ImGui::GetStyle().AntiAliasedFill = false;
        ImGui::GetStyle().AntiAliasedLines = false;
        ImGui::CaptureMouseFromApp();
        ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f);

        ImGuiStyle& style = ImGui::GetStyle();

        style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
        style.Colors[ImGuiCol_Border] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, col_area.w + .1f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.45f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.85f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.90f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(col_main.x, col_main.y, col_main.z, 0.90f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.90f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.50f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);
    }
    if(menuShown)
    {
        bool is_selected = false;
        uint32_t current_weapon = NULL;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
       
        ImGui::NewFrame();
        ImGui::Begin("Krog Internal Overlay");

        ImGui::BeginTabBar("#tabs");

        if (ImGui::BeginTabItem("Player"))
        {
            ImGui::SliderInt("Health", (int*)&playerEnt->Health, 0, 100);
            ImGui::SliderInt("Armor", (int*)&playerEnt->Armor, 0, 100);

            ImGui::Separator();

            ImGui::Spacing();

            UpdateWeaponValues(playerEnt);
            ImGui::Combo("Weapon", &weapKeyIndex, weaponList, weapSize);
            playerEnt->EquippedWeapon = weaponValues[weapKeyIndex];

            uint32_t* magAmmo = nullptr, * reserveAmmo = nullptr;
            if (playerEnt->EquippedWeapon == playerEnt->PistolPtr) magAmmo = &playerEnt->PistolMagAmmo;
            else if (playerEnt->EquippedWeapon == playerEnt->CarbinePtr) magAmmo = &playerEnt->CarbineMagAmmo;
            else if (playerEnt->EquippedWeapon == playerEnt->ShotgunPtr) magAmmo = &playerEnt->ShotgunMagAmmo;
            else if (playerEnt->EquippedWeapon == playerEnt->SubgunPtr) magAmmo = &playerEnt->SubgunMagAmmo;
            else if (playerEnt->EquippedWeapon == playerEnt->SniperPtr) magAmmo = &playerEnt->SniperMagAmmo;
            else if (playerEnt->EquippedWeapon == playerEnt->AssaultPtr) magAmmo = &playerEnt->AssaultRifeMagAmmo;

            if (playerEnt->EquippedWeapon == playerEnt->PistolPtr) reserveAmmo = &playerEnt->PistolReserveAmmo;
            else if (playerEnt->EquippedWeapon == playerEnt->CarbinePtr) reserveAmmo = &playerEnt->CarbineReserveAmmo;
            else if (playerEnt->EquippedWeapon == playerEnt->ShotgunPtr) reserveAmmo = &playerEnt->ShotgunReserveAmmo;
            else if (playerEnt->EquippedWeapon == playerEnt->SubgunPtr) reserveAmmo = &playerEnt->SubgunReserveAmmo;
            else if (playerEnt->EquippedWeapon == playerEnt->SniperPtr) reserveAmmo = &playerEnt->SniperReserveAmmo;
            else if (playerEnt->EquippedWeapon == playerEnt->AssaultPtr) reserveAmmo = &playerEnt->AssaultRifleReserveAmmo;

            ImGui::Spacing();

            if (magAmmo != nullptr && reserveAmmo != nullptr)
            {
                ImGui::SliderInt("Clip Ammo", (int*)magAmmo, 0, 65635);
                ImGui::SliderInt("Reserve Ammo", (int*)reserveAmmo, 0, 65635);
            }

            ImGui::Spacing();
            ImGui::Separator();

            ImGui::SliderFloat("Camera X", (float*)&playerEnt->ViewAngle.x, 0.0f, 360.0f);
            ImGui::SliderFloat("Camera Y", (float*)&playerEnt->ViewAngle.y, -90.0f, 90.0f);

            ImGui::Spacing();
            ImGui::Separator();

            ImGui::SliderFloat("Position X : ", (float*)&playerEnt->Position.x, -500.f, 500.f);
            ImGui::SliderFloat("Position Y : ", (float*)&playerEnt->Position.y, -500.f, 500.f);
            ImGui::SliderFloat("Position Z : ", (float*)&playerEnt->Position.z, -50.f, 50.f);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Cheats"))
        {
            ImGui::Checkbox("Aimbot", &bAimbotEnabled);
            ImGui::Combo("Hold key", &holdKeyIndex, holdKeys, holdSize);
                holdKey = holdKeysCodes[holdKeyIndex];

            ImGui::Spacing();
            ImGui::Separator();

            ImGui::Checkbox("No Recoil", &bNoRecoilEnabled);
            ImGui::Checkbox("Rapid Fire", &bRapidFireEnabled);
            ImGui::Spacing();

            ImGui::Separator();
            ImGui::Checkbox("Infinite Health", &bInfiniteHealthEnabled);
            ImGui::Checkbox("Infinite Armor", &bInfiniteArmorEnabled);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Debug"))
        {
            ImGui::Text("Player Entity Address : ");
            ImGui::SameLine();
            ImGui::TextColored(ImColor::HSV(100 / 255.f, 20.f / 255.f, 235.f / 255.f), "%X", (DWORD)localPlayer);

            ImGui::Spacing();
            ImGui::Separator();

            ImGui::Text("Number of players : ");
            ImGui::SameLine();
            ImGui::TextColored(ImColor::HSV(100 / 255.f, 20.f / 255.f, 235.f / 255.f), "%d", (*(int*)PLAYER_AMOUNT_ADDR));

            ImGui::Spacing();

            ImGui::Text("Closest player : ");
            ImGui::SameLine();
            ImGui::TextColored(ImColor::HSV(100 / 255.f, 20.f / 255.f, 235.f / 255.f), "%s", Hacks::GetClosestPlayer() > 0 ? Hacks::GetClosestPlayer()->name : "None");

            ImGui::Spacing();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
        SDL_ShowCursor(1);

        


        ImGui::End();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    else
    {
        SDL_ShowCursor(0);
    }


    HackTick();

    return wglSwapBuffersGateway(hDc);
}



DWORD WINAPI MainThread(LPVOID reserved) {

    playerEnt = (P_PLAYER_ENT)(*(DWORD*)0x50F4F4);

    showCursorAddr = (uintptr_t)GetProcAddress(GetModuleHandle(L"SDL.dll"), "SDL_ShowCursor");
    SDL_ShowCursor = (tSDL_ShowCursor)showCursorAddr;

    Hook SwapBuffersHook("wglSwapBuffers", "opengl32.dll", (BYTE*)hkwglSwapBuffers, (BYTE*)&wglSwapBuffersGateway, 5);
    SwapBuffersHook.Enable();

    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
    {
        
    }

    FreeLibraryAndExitThread((HMODULE)reserved, NULL);
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainThread, NULL, NULL, NULL);

    return TRUE;
}