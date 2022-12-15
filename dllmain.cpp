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
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
       
        ImGui::NewFrame();
        ImGui::Begin("Krog Internal Overlay");

        SDL_ShowCursor(1);

        ImGui::Checkbox("Checkbox Mathilde", &inOpt);

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