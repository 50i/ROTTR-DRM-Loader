#include <Windows.h>
#include <iostream>
#include <stdio.h>

#include "vendor/minhook/include/MinHook.h"
#include "hooking.hpp"

// called by the game, returns the current FS
QWORD GetFS()
{
    if (Hooking::GetInstance().IsDiskFs())
    {
        return *(QWORD*)DISKFS;
    }

    return *(QWORD*)FS;
}

// game function to check if file exists on disk FS
int(__fastcall* cdc__MSFileSystem__GetFileSize)(QWORD, const char* pFileName);

// orginal game cdc::Resolve::Load
__int64(__fastcall* origcdc__Resolve__Load)(char* a1, QWORD a2, unsigned int a3, QWORD a4, QWORD* a5, __int64 a6, __int64 a7, __int64 a8, unsigned int a9, char a10);

__int64 __fastcall cdc__Resolve__Load(char* a1, QWORD a2, unsigned int a3, QWORD a4, QWORD* a5, __int64 a6, __int64 a7, __int64 a8, unsigned int a9, char a10)
{
    std::cout << a1 << std::endl;

    // check if this file exists on the diskFS
    if (cdc__MSFileSystem__GetFileSize(*(QWORD*)DISKFS, a1))
    {
        std::cout << a1 << " exists on disk, using g_pDiskFS" << std::endl;

        // tells GetFS to return g_pDiskFS for the next call made by cdc::Resolve::Load
        // this should not mess with other requests to GetFS since most of the game code runs from one loop
        Hooking::GetInstance().SetDiskFs(true);
    }

    auto ret = origcdc__Resolve__Load(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);

    // return to normal FS right after cdc::Resolve::Load returns
    Hooking::GetInstance().SetDiskFs(false);

    return ret;
}

Hooking::Hooking()
{
    AllocConsole();

    FILE* cout;
    freopen_s(&cout, "CONOUT$", "w", stdout);

    MH_Initialize();

    // addresses for Steam latest 1.0.820.0
    MH_CreateHook((void*)0x145F96260, cdc__Resolve__Load, (void**)&origcdc__Resolve__Load);
    MH_CreateHook((void*)0x1446A74D0, GetFS, nullptr);
    cdc__MSFileSystem__GetFileSize = reinterpret_cast<int(__fastcall*)(QWORD, const char* pFileName)>(0x1446CFE50);

    MH_EnableHook(MH_ALL_HOOKS);
}

void Hooking::SetDiskFs(bool toggle) noexcept
{
    isDiskFs = toggle;
}

bool Hooking::IsDiskFs() const noexcept
{
    return isDiskFs;
}

DWORD WINAPI HookMain(LPVOID lpParam)
{
    Hooking::GetInstance();

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, HookMain, NULL, 0, NULL);

        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}
