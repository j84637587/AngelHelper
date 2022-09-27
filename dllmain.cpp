// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        // We don't need to get notified in thread attach- or detachments
        DisableThreadLibraryCalls(static_cast<HMODULE>(hModule));
        CloseHandle(CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)MainThread, hModule, NULL, nullptr));
        //CloseHandle(CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)UI::Render, hModule, NULL, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}