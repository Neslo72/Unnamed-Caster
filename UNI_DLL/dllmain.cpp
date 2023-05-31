// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include <stdio.h>

#include "offsets.h"

#define MAX_PLAYER_SIZE 600

DWORD WINAPI MainThread(HMODULE hModule)
{
    AllocConsole();
    FILE* f = new FILE;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "Reporting live from the attached DLL..." << std::endl;
    printf("I MADE CHANGE\n");
    char* base_pointer = (char*)GetModuleHandleA(0);
    if (base_pointer == NULL) {
        printf("GetModuleHanlde Issue %d\n", GetLastError());
    }
    else {
        printf("BIG TEST 0x%08x\n", (unsigned int)base_pointer);
        printf("HI\n");
    }


    char statebuf[MAX_PLAYER_SIZE] = { 0 };
    while (1)
    {
        if (GetAsyncKeyState(VK_F6) & 1)
        {
            std::cout << "You pressed F6!  Changing P1 XVel" << std::endl;
            *(uint32_t*)(base_pointer + P1_XVEL) += 100;
        }

        if (GetAsyncKeyState(VK_F5) & 1)
        {
            std::cout << "Menu mode? ";
            std::cout << *(uint32_t*)(base_pointer + MENU) << std::endl;

        }

        if (GetAsyncKeyState(VK_F4) & 1)
        {
            std::cout << "Attempting P1 save state" << std::endl;  // This saves some version of state...
            memcpy(statebuf, (base_pointer + P1_XPOS), MAX_PLAYER_SIZE);
        }
        if (GetAsyncKeyState(VK_F3) & 1)
        {
            std::cout << "Attempting P1 load state" << std::endl;
            memcpy((base_pointer + P1_XPOS), statebuf, MAX_PLAYER_SIZE);
        }

        if (GetAsyncKeyState(VK_F7) & 1)
        {
            std::cout << "You have choosen poorly.  Now exiting" << std::endl;
            break;
        }
    }
    return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}