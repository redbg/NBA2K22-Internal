#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <mongoose.h>
#include <stdio.h>
#include <string>

#include "hook.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Winmm.lib")

std::string url      = "http://208.110.66.194:8888/vc";
std::string key      = "";
int         interval = 0;

void fn(mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    if (ev == MG_EV_CONNECT)
    {
        mg_str host = mg_url_host(url.c_str());
        // Send request
        mg_printf(c,
                  "GET %s?k=%s&x=%llu&c=%llu HTTP/1.0\r\n"
                  "Host: %.*s\r\n"
                  "\r\n",
                  mg_url_uri(url.c_str()), key.c_str(), *NBA2K22::GetX(), *NBA2K22::CloudSaveId,
                  (int)host.len, host.ptr);
    }

    if (ev == MG_EV_HTTP_MSG)
    {
        time_t now = time(NULL);
        char   timeString[32];
        strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localtime(&now));

        mg_http_message *hm = (mg_http_message *)ev_data;
        printf("%s [%.*s]\n", timeString, (int)hm->body.len, hm->body.ptr);
    }
}

void reset()
{
    printf("key:");
    std::cin >> key;

    do
    {
        printf("interval(second):");
        std::cin >> interval;

        if (interval < 600)
        {
            printf("Error: The minimum interval is 600\n");
        }

    } while (interval < 600);
}

DWORD WINAPI MyThread(LPVOID hModule)
{
    mg_mgr mgr;
    mg_mgr_init(&mgr);

    static int lastTime = 0;

    while (!GetAsyncKeyState(VK_END) & 1)
    {
        mg_mgr_poll(&mgr, 1000);

        if ((GetAsyncKeyState(VK_INSERT) & 1))
        {
            DWORD64 x = 0;
            printf("x:");
            std::cin >> x;
            *NBA2K22::GetX() = x;
        }

        if (GetAsyncKeyState(VK_F10) & 1)
        {
            *NBA2K22::v1    = 4;
            *NBA2K22::time1 = 0;
        }

        if (GetAsyncKeyState(VK_F12) & 1)
        {
            reset();
            lastTime = 0;
        }

        if (interval != 0 && (time(NULL) - lastTime) > interval)
        {
            mg_http_connect(&mgr, url.c_str(), fn, NULL);
            lastTime = time(NULL);
        }

        Sleep(100);
    }

    // mg_mgr_free(&mgr);
    FreeLibraryAndExitThread((HMODULE)hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        AllocConsole();
        FILE *file;
        freopen_s(&file, "CONIN$", "r", stdin);
        freopen_s(&file, "CONOUT$", "w", stdout);
        freopen_s(&file, "CONOUT$", "w", stderr);
        printf("[Start]\n");

        NBA2K22::Hook();
        CreateThread(0, 0, MyThread, (HMODULE)hModule, 0, 0);
        break;
    case DLL_PROCESS_DETACH:
        NBA2K22::UnHook();

        printf("[End]\n");
        FreeConsole();
        break;
    }

    return TRUE;
}
