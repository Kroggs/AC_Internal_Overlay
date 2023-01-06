#include "pch.h"
#include <conio.h>
#include <stdio.h>
#include <iostream>

#include <gl/glew.h>

#include <gl/GL.h>

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_win32.h"
#include "ImGUI/imgui_impl_opengl3.h"


#include "PlayerEnt.h"
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

        glewInit();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hGameWindow);
        ImGui_ImplOpenGL3_Init();
        ImGui::StyleColorsDark();
        ImGui::GetStyle().AntiAliasedFill = false;
        ImGui::GetStyle().AntiAliasedLines = false;
        ImGui::CaptureMouseFromApp();
        ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f);
        
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

            ImGui::Spacing();

            ImGui::Text("Weapon : ");
            ImGui::SameLine();
            if (ImGui::Button("Knife", ImVec2(75, 20))) playerEnt->EquippedWeapon = playerEnt->KnifePtr; ImGui::SameLine();
            if (ImGui::Button("Pistol", ImVec2(75, 20))) playerEnt->EquippedWeapon = playerEnt->PistolPtr; ImGui::SameLine();
            if (ImGui::Button("Carbine", ImVec2(75, 20))) playerEnt->EquippedWeapon = playerEnt->CarbinePtr; ImGui::SameLine();
            if (ImGui::Button("Shotgun", ImVec2(75, 20))) playerEnt->EquippedWeapon = playerEnt->ShotgunPtr; ImGui::SameLine();
            if (ImGui::Button("Subgun", ImVec2(75, 20))) playerEnt->EquippedWeapon = playerEnt->SubgunPtr; ImGui::SameLine();
            if (ImGui::Button("Sniper", ImVec2(75, 20))) playerEnt->EquippedWeapon = playerEnt->SniperPtr; ImGui::SameLine();
            if (ImGui::Button("Assault", ImVec2(75, 20))) playerEnt->EquippedWeapon = playerEnt->AssaultPtr;

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

            ImGui::SliderFloat("Camera X", (float*)&playerEnt->ViewAngle.x, 0.0f, 360.0f);
            ImGui::SliderFloat("Camera Y", (float*)&playerEnt->ViewAngle.y, -90.0f, 90.0f);

            ImGui::Spacing();

            ImGui::SliderFloat("Position X : ", (float*)&playerEnt->Position.x, -500.f, 500.f);
            ImGui::SliderFloat("Position Y : ", (float*)&playerEnt->Position.y, -500.f, 500.f);
            ImGui::SliderFloat("Position Z : ", (float*)&playerEnt->Position.z, -50.f, 50.f);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Cheats"))
        {
            ImGui::Checkbox("Aimbot", &Hacks::bAimbotEnabled);
            ImGui::Spacing();
            ImGui::Checkbox("No Recoil", &Hacks::bNoRecoilEnabled);
            ImGui::Checkbox("Rapid Fire", &Hacks::bRapidFireEnabled);
            ImGui::Spacing();
            ImGui::Checkbox("Infinite Health", &Hacks::bInfiniteHealthEnabled);
            ImGui::Checkbox("Infinite Armor", &Hacks::bInfiniteArmorEnabled);
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