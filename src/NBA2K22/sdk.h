#pragma once
#include <Windows.h>
#include <map>

typedef DWORD64 (*Insert_t)(DWORD64 data, DWORD64 *out, DWORD64 k, DWORD64 v, DWORD64 a5);
typedef DWORD64 (*Insert_Float_t)(DWORD64 data, DWORD64 *out, DWORD64 k, float v, DWORD64 a5);

namespace NBA2K22
{
    DWORD64 NBA2K22 = (DWORD64)GetModuleHandle(NULL);

    int    *v1    = (int *)(NBA2K22 + 0x40A96FC);    // 第几节 (5 = 加时赛)
    double *time1 = (double *)(NBA2K22 + 0x40A9770); // 本节比赛剩余时间
    double *time2 = (double *)(NBA2K22 + 0x40A97A0); // 本次进攻剩余时间

    int *ArraySize = (int *)(NBA2K22 + 0x6498E08);

    // 云存档Id
    DWORD64 *CloudSaveId = (DWORD64 *)(NBA2K22 + 0x55301F0);

    DWORD64 GetX()
    {
        // [[6A1DA18]+20]+60
        return *(DWORD64 *)((*(DWORD64 *)((*(DWORD64 *)(NBA2K22 + 0x6A1DA18)) + 0x20)) + 0x60);
    }

    // ================================================================================================
    enum KEYS
    {
        URL    = 0x12BBF3AB,
        Method = 0xA78A16C7,
        X      = 0x31783019
    };

    struct Data
    {
        DWORD   Key;
        DWORD   Type : 5;
        DWORD   Size : 26;
        DWORD   _Pad : 1;
        DWORD64 Value;
    };

    auto GetString = (char *(*)(DWORD64 data, DWORD64 key, DWORD64 r8))(NBA2K22 + 0x1E703C0); //搜字符串 GetString8
    auto GetData   = (Data * (*)(DWORD64 data, DWORD64 key, DWORD64 r8))(NBA2K22 + 0x1E6FB20);

    Insert_t       Insert_6 = (Insert_t)(NBA2K22 + 0x1E74630);
    Insert_t       Insert_C = (Insert_t)(NBA2K22 + 0x1E72280);
    Insert_Float_t Insert_D = (Insert_Float_t)(NBA2K22 + 0x1E72DD0);
} // namespace NBA2K22
