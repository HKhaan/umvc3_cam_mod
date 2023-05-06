// dllmain.cpp : Defines the entry point for the DLL application.
#include <d3d9.h>
#include <string>

#include "Bindings.h"
#include "NoClip.h"
#include "pch.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "kiero/kiero.h"
#include "utils/utils.h"
#include "includes.h"
bool isFocused = false;
bool init = false;
bool menuEnabled = false;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef long (__stdcall* EndScene)(LPDIRECT3DDEVICE9);

Bindings* bindings = new Bindings();
NoClip* noclip = new NoClip();
EndScene oEndScene = NULL;
static HWND window = NULL;
WNDPROC oWndProc;


bool CheckGame()
{
    char* gameName = (char*)(_addr(0x140B12D10));

    if (strcmp(gameName, "umvc3") == 0)
    {
        return true;
    }
    else
    {
        MessageBoxA(
            0, "Invalid game version!\nUMVC3Hook only supports latest (or it needs to be updated) Steam executable.", 0,
            MB_ICONINFORMATION);
        return false;
    }
}


void InitImGui(LPDIRECT3DDEVICE9 pDevice)
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(pDevice);

    ImGuiStyle* style = &ImGui::GetStyle();
    style->WindowRounding = 6.0f;
    style->ItemSpacing = ImVec2(7, 5.5);
    style->FrameRounding = 2.0f;
    style->FramePadding = ImVec2(6, 4.25);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KILLFOCUS:
        isFocused = false;
        break;
    case WM_SETFOCUS:
        isFocused = true;
        break;
        break;
    default:
        break;
    }

    if (menuEnabled)
    {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        return true;
    }

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}


long __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
    if (GetAsyncKeyState(VK_F2))
    {
        menuEnabled = true;
        bindings->Init();
    }

    if (menuEnabled)
    {
        if (!init)
        {
            isFocused = false;
            D3DDEVICE_CREATION_PARAMETERS params;
            pDevice->GetCreationParameters(&params);
            window = params.hFocusWindow;
            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
            InitImGui(pDevice);
            init = true;
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        static bool show_demo_window = true;
        ImGui::Begin("Noclip mod", &menuEnabled, ImGuiWindowFlags_NoSavedSettings);
        noclip->TickMenu();
        ImGui::End();
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    if (noclip->Active)
    {
        noclip->Tick(bindings);
    }
    auto endscne = oEndScene(pDevice);
    return endscne;
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
    Sleep(5000);
    bool attached = false;
    do
    {
        if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success)
        {
            kiero::bind(42, (void**)&oEndScene, hkEndScene);
            attached = true;
        }
    }
    while (!attached);
    return TRUE;
}

void WINAPI HookUpdate()
{
    while (true)
    {
        //UMVC3Hooks::HookProcessStuff();
        Sleep(1);
    }
}


BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
    init = false;
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        if (CheckGame())
        {
            DisableThreadLibraryCalls(hMod);
            CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HookUpdate, hMod, 0, nullptr);
            kiero::bind(42, (void**)&oEndScene, hkEndScene);
        }
        break;
    case DLL_PROCESS_DETACH:
        kiero::shutdown();
        break;
    }
    return TRUE;
}
