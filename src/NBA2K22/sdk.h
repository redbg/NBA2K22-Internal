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

    DWORD64 *GetX()
    {
        // [[6A1DA18]+20]+60
        return (DWORD64 *)((*(DWORD64 *)((*(DWORD64 *)(NBA2K22 + 0x6A1DA18)) + 0x20)) + 0x60);
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

    template <typename T>
    class array
    {
    public:
        DWORD64 u;
        DWORD   count;
        DWORD   u1;
        T     **objArray;
        T      *operator[](int i)
        {
            return objArray[i];
        }
    };

    struct SName
    {
        union
        {
            struct
            {
                wchar_t name[0x14]; //名
            };
            struct
            {
                char    u[0x28];
                wchar_t surname[0x14]; //姓
            };
        };
    };

    class card
    {
    public:
        union
        {
            struct
            {
                virtual void v0(void);
                virtual void v8(void);
                virtual void v10(void);
                virtual void v18(void);
                virtual void v20(void);
                virtual void v28(void);
                virtual void v30(void);
                virtual void v38(void);
                virtual void v40(void);
                virtual void v48(void);
                virtual void v50(void);
                virtual void v58(void);
                virtual void v60(void);
                virtual void v68(void);
                virtual void v70(void);
                virtual void v78(void);
                virtual void v80(void);
                virtual void v88(void);
                virtual void v90(void);
                virtual void v98(void);
                virtual void v100(void);
                virtual void GetName(void);
            };

            struct
            {
                char u[0x108];
                int  price; //出售价格
            };
            struct
            {
                char u1[0x114];
                int  number; //物品数量
            };
            struct
            {
                char u2[0x138];
                int  type; //类型，例如 球员，教练卡，篮球卡
            };
            struct
            {
                char u3[0x15c];
                int  color; //颜色，例如 暗物质，黄金
            };

            struct
            {
                char    u4[0x120];
                DWORD32 id; // id，卖卡所需
            };
        };
    };

    auto GetString = (char *(*)(DWORD64 data, DWORD64 key, DWORD64 r8))(NBA2K22 + 0x1E703C0); //搜字符串 GetString8
    auto GetData   = (Data * (*)(DWORD64 data, DWORD64 key, DWORD64 r8))(NBA2K22 + 0x1E6FB20);

    Insert_t       Insert_6 = (Insert_t)(NBA2K22 + 0x1E74630);
    Insert_t       Insert_C = (Insert_t)(NBA2K22 + 0x1E72280);
    Insert_Float_t Insert_D = (Insert_Float_t)(NBA2K22 + 0x1E72DD0);
} // namespace NBA2K22
