﻿#include <mongoose.h>

#include "hook.h"
#include "sdk.h"
#include <format>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Winmm.lib")

std::string g_Key         = "";
int         g_Point       = 0;
int         g_Interval    = 0;
const int   g_MinInterval = 600;

#ifdef _DEBUG
std::string LoginUrl = "http://127.0.0.1:8888/2k22/v1/login";
std::string SellUrl  = "http://127.0.0.1:8888/2k22/v1/sell";
std::string VC_Url   = "http://127.0.0.1:8888/2k22/v1/vc";
#else
std::string LoginUrl = "http://208.110.66.194/2k22/v1/login";
std::string SellUrl  = "http://208.110.66.194/2k22/v1/sell";
std::string VC_Url   = "http://208.110.66.194/2k22/v1/vc";
#endif

// ====================================================================================================

void HttpMsg(mg_http_message *hm)
{
    time_t now = time(NULL);
    char   timeString[32];
    strftime(timeString, sizeof(timeString), "%Y/%m/%d %H:%M:%S", localtime(&now));

    for (int i = 0; i < MG_MAX_HTTP_HEADERS && hm->headers[i].name.len > 0; i++)
    {
        mg_str *k = &hm->headers[i].name;
        mg_str *v = &hm->headers[i].value;

        if (std::string(k->ptr, k->len) == "Point")
        {
            g_Point = std::stoi(std::string(v->ptr, v->len));
        }

        // printf("%.*s -> %.*s\n", (int)k->len, k->ptr, (int)v->len, v->ptr);
    }

    printf("%s | %.*s\n", timeString, (int)hm->body.len, hm->body.ptr);
}

// ====================================================================================================

void login(mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    if (ev == MG_EV_CONNECT)
    {
        mg_str host = mg_url_host(LoginUrl.c_str());
        // Send request

        mg_printf(c,
                  "GET %s?k=%s&x=%llu&SteamUser=%s HTTP/1.0\r\n"
                  "Host: %.*s\r\n"
                  "\r\n",
                  mg_url_uri(LoginUrl.c_str()),
                  g_Key.c_str(), *NBA2K22::GetX(), getenv("SteamUser"),
                  (int)host.len, host.ptr);
    }

    if (ev == MG_EV_HTTP_MSG)
    {
        HttpMsg((mg_http_message *)ev_data);
    }
}

// ====================================================================================================

void vc(mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    if (ev == MG_EV_CONNECT)
    {
        mg_str host = mg_url_host(VC_Url.c_str());
        // Send request
        mg_printf(c,
                  "GET %s?k=%s&x=%llu&SteamUser=%s&c=%llu HTTP/1.0\r\n"
                  "Host: %.*s\r\n"
                  "\r\n",
                  mg_url_uri(VC_Url.c_str()),
                  g_Key.c_str(), *NBA2K22::GetX(), getenv("SteamUser"), *NBA2K22::CloudSaveId,
                  (int)host.len, host.ptr);
    }

    if (ev == MG_EV_HTTP_MSG)
    {
        HttpMsg((mg_http_message *)ev_data);
    }
}

// ====================================================================================================

void sell(mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    if (ev == MG_EV_CONNECT)
    {
        mg_str host = mg_url_host(SellUrl.c_str());
        // Send request
        mg_printf(c,
                  "GET %s?k=%s&x=%llu&SteamUser=%s&sell=%s HTTP/1.0\r\n"
                  "Host: %.*s\r\n"
                  "\r\n",
                  mg_url_uri(SellUrl.c_str()),
                  g_Key.c_str(), *NBA2K22::GetX(), getenv("SteamUser"), (char *)fn_data,
                  (int)host.len, host.ptr);
    }

    if (ev == MG_EV_HTTP_MSG)
    {
        HttpMsg((mg_http_message *)ev_data);
    }
}

// ====================================================================================================

std::vector<DWORD32> GetCardSellArray()
{
    NBA2K22::array<NBA2K22::card> *cardArray = (NBA2K22::array<NBA2K22::card> *)SS::Memory::Read<DWORD64>(NBA2K22::NBA2K22 + 0x3EF4918);

    std::vector<DWORD32> id_array;

    if (!cardArray)
        return id_array;

    for (size_t i = 0; i < cardArray->count; i++)
    {
        if ((*cardArray)[i]->id != -1)
        {
            for (size_t j = 0; j < (*cardArray)[i]->number; j++)
            {
                id_array.push_back((*cardArray)[i]->id);
            }
        }
    }

    return id_array;
}

// ====================================================================================================

DWORD WINAPI MyThread(LPVOID hModule)
{
    mg_mgr mgr;
    mg_mgr_init(&mgr);

    static int lastTime = 0;

    static int                  sellMaximum  = 0;
    static int                  sellInterval = 0;
    static int                  lastSellTime = 0;
    static std::vector<DWORD32> sellArray;

    while (!(GetAsyncKeyState(VK_END) & 0x8000))
    {
        mg_mgr_poll(&mgr, 100);

        // 一键结束比赛
        if (GetAsyncKeyState(VK_F5) & 0x8000)
        {
            *NBA2K22::v1    = 4;
            *NBA2K22::time1 = 0;
        }

        // 开始卖收藏
        if ((GetAsyncKeyState(VK_F8) & 0x8000) && g_Point > 0)
        {
            sellArray = GetCardSellArray();

            if (!sellArray.empty())
            {
                printf("SellInterval(Second):");
                std::cin >> sellInterval;
                printf("SellMaximum:");
                std::cin >> sellMaximum;

                lastSellTime = 0;
            }
        }

        // 定时卖卡
        if (!sellArray.empty() && sellInterval != 0 && sellMaximum != 0 && (time(NULL) - lastSellTime) > sellInterval)
        {
            static std::string id_string = "";
            id_string                    = "";

            for (size_t i = 0; (i < sellMaximum) && (!sellArray.empty()); i++)
            {
                id_string += std::to_string(sellArray.back());
                id_string += ",";
                sellArray.pop_back();
            }

            if (!id_string.empty())
            {
                id_string.pop_back();
                printf("Number of cards sell:%d\n", sellMaximum);
                printf("Number of remaining cards:%zd\n", sellArray.size());
                mg_http_connect(&mgr, SellUrl.c_str(), sell, id_string.data());
            }

            lastSellTime = time(NULL);
        }

        // 开始刷VC
        if ((GetAsyncKeyState(VK_F10) & 0x8000) && g_Point > 0)
        {
            printf("Interval(Second):");
            std::cin >> g_Interval;

            if (g_Interval == 0)
            {
                printf("[Stop]\n");
            }
            else if (g_Interval >= g_MinInterval)
            {
                printf("[Start]\n");
            }
            else
            {
                printf("[Error] The minimum interval is %d\n", g_MinInterval);
            }

            lastTime = 0;
        }

        // 定时刷VC
        if (g_Interval >= g_MinInterval && (time(NULL) - lastTime) > g_Interval)
        {
            mg_http_connect(&mgr, VC_Url.c_str(), vc, NULL);
            lastTime = time(NULL);
        }

        // 登录
        if (GetAsyncKeyState(VK_F12) & 0x8000)
        {
            printf("Key:");
            std::cin >> g_Key;
            mg_http_connect(&mgr, LoginUrl.c_str(), login, NULL);
        }

        static DWORD64 x = 0;
        static DWORD64 c = 0;

        if (x)
        {
            *NBA2K22::GetX() = x;
        }

        if (c)
        {
            *NBA2K22::CloudSaveId = c;
        }

        if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
        {
            // 清除控制台
            if (GetAsyncKeyState(VK_DELETE) & 0x8000)
            {
                system("cls");
            }

            // 修改 x
            if (GetAsyncKeyState(VK_INSERT) & 0x8000)
            {
                printf("x:%llu -> x:", *NBA2K22::GetX());
                std::cin >> x;
            }

            // 修改 CloudSaveId
            if (GetAsyncKeyState(VK_HOME) & 0x8000)
            {
                printf("c:%llu -> c:", *NBA2K22::CloudSaveId);
                std::cin >> c;
            }
        }

        Sleep(50);
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
        printf("[Begin]\n");

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
