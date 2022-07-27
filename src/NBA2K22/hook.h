#pragma once
#include "Memory.h"
#include "sdk.h"
#include <MinHook.h>
#include <Windows.h>
#include <dxgi.h>
#include <format>
#include <intrin.h>
#include <iostream>

typedef HRESULT (*Present_t)(IDXGISwapChain *swapChain, UINT SyncInterval, UINT Flags);
typedef DWORD64 (*SendPacket_t)(DWORD64 a1, DWORD64 a2, DWORD64 a3);
typedef DWORD (*CRC32_t)(const char *a1, DWORD a2);

namespace NBA2K22
{
    Present_t *presentPattern  = (Present_t *)(SS::Memory::FindPattern("GameOverlayRenderer64.dll", "\x48\x89\x6C\x24\x18\x48\x89\x74\x24\x20\x41\x56\x48\x83\xEC\x20\x41\x8B\xE8") + 0x2e);
    Present_t *present         = (Present_t *)((DWORD64)presentPattern + *(DWORD *)((DWORD64)presentPattern + 3) + 7);
    Present_t  originalPresent = *present;

    SendPacket_t *sendPacketPattern  = (SendPacket_t *)(SS::Memory::FindPattern("NBA2K22.exe", "\x48\xC7\x41\x28????\x48\x8D\x05") + 8);
    SendPacket_t *sendPacket         = (SendPacket_t *)((DWORD64)sendPacketPattern + *(DWORD *)((DWORD64)sendPacketPattern + 3) + 7);
    SendPacket_t  originalSendPacket = *(SendPacket_t *)((DWORD64)sendPacket + 0x68);

    CRC32_t CRC32         = (CRC32_t)(NBA2K22 + 0x1EC3000);
    CRC32_t originalCRC32 = NULL;

    HRESULT MyPresent(IDXGISwapChain *swapChain, UINT SyncInterval, UINT Flags)
    {
        HRESULT ret = originalPresent(swapChain, SyncInterval, Flags);
        Sleep(50);
        return ret;
    }

    std::map<DWORD, std::string> HashTable;

    DWORD MyCRC32(char *a1, DWORD a2)
    {
        DWORD hash = originalCRC32(a1, a2);

        if (hash != 0 && HashTable.count(hash) == 0)
        {
            // printf("Hash:%08x = %s\n", hash, a1);
            HashTable[hash] = a1;
        }

        return hash;
    }

    void Insert(DWORD64 a1, DWORD OCCURENCES, DWORD CONTRACT_PAYOUT_EVENT, DWORD MILESTONE_IDENTIFIER, DWORD EVENT)
    {
        DWORD64 ret;

        Insert_6(a1, &ret, CRC32(std::format("OCCURENCES_{}", *ArraySize).c_str(), 0x7FFFFFFE), OCCURENCES, 0);                       // Type:06 (事件触发次数)
        Insert_6(a1, &ret, CRC32(std::format("MODIFIER_FLAG_{}", *ArraySize).c_str(), 0x7FFFFFFE), 0x2c0, 0);                         // Type:06 [2c0] (固定)
        Insert_D(a1, &ret, CRC32(std::format("CONTRACT_MODIFIER_{}", *ArraySize).c_str(), 0x7FFFFFFE), 1.0f, 0);                      // Type:0d [3f800000] (固定)
        Insert_6(a1, &ret, CRC32(std::format("CONTRACT_PAYOUT_EVENT_{}", *ArraySize).c_str(), 0x7FFFFFFE), CONTRACT_PAYOUT_EVENT, 0); // Type:06 [39] (39 = 超远三分, d = 底角三分)
        Insert_C(a1, &ret, CRC32(std::format("MILESTONE_IDENTIFIER_{}", *ArraySize).c_str(), 0x7FFFFFFE), MILESTONE_IDENTIFIER, 0);   // Type:0c [b9bc7343] (与合同相关)
        Insert_C(a1, &ret, CRC32(std::format("EVENT_{}", *ArraySize).c_str(), 0x7FFFFFFE), EVENT, 0);                                 // Type:0c [5c7ab612] (与合同相关)

        (*ArraySize)++;
    }

    void MySendPacket_(DWORD64 a1)
    {
        // URL
        printf("\n%s %s?x=%llu\n", GetString(a1 + 0x48, KEYS::Method, 0), GetString(a1 + 0x48, KEYS::URL, 0), GetData(a1 + 0x48, KEYS::X, 0)->Value);

        if (!strcmp("https://nba2k22-svc.2ksports.com:22120/nba/2k22/VirtualCurrencyV4/earn", GetString(a1 + 0x48, KEYS::URL, 0)))
        {
            // Insert(a1, 50, 0x39, 0x30ff658c, 0x20be9d03);
            // Insert(a1, 50, 0x39, 0x6b45f0e9, 0x286cf8e7);
            // Insert(a1, 50, 0x39, 0x97ec5ae5, 0xee836742);
            // Insert(a1, 50, 0x39, 0x1a45db06, 0x80b96b53);
            // Insert(a1, 50, 0x39, 0xb9bc7343, 0x5c7ab612);
            // Insert(a1, 50, 0x39, 0xfa80ba73, 0x7987632c);
            // Insert(a1, 50, 0x39, 0x7627ad24, 0x375d2a82);

            // Insert(a1, 50, 0xd, 0x30ff658c, 0x20be9d03);
            // Insert(a1, 50, 0xd, 0x6b45f0e9, 0x286cf8e7);
            // Insert(a1, 50, 0xd, 0x97ec5ae5, 0xee836742);
            // Insert(a1, 50, 0xd, 0x1a45db06, 0x80b96b53);
            // Insert(a1, 50, 0xd, 0xb9bc7343, 0x5c7ab612);
            // Insert(a1, 50, 0xd, 0xfa80ba73, 0x7987632c);
            // Insert(a1, 50, 0xd, 0x7627ad24, 0x375d2a82);
        }

        for (int i = 0; i < *(int *)(a1 + 0x18); i++)
        {
            Data *DataArray = *(Data **)(*(DWORD64 *)a1 + (i * 8));

            // if (DataArray->Key == 0xc4886abd)
            // {
            //     DataArray->Value = 0xffffffff;
            // }

            // if (DataArray->Key == 0xfca5f7d2)
            // {
            //     DataArray->Value = 0;
            // }

            printf("[%04d]  Key:%08x  Type:%02x  Size:%02x  Value:%016llx", i, DataArray->Key, DataArray->Type, DataArray->Size, DataArray->Value);

            if (HashTable.count(DataArray->Key))
            {
                printf("  Name:%s", HashTable[DataArray->Key].c_str());
            }

            for (size_t i = 0; i < DataArray->Size; i++)
            {
                if (DataArray->Type == 0xc || DataArray->Type == 0xd)
                {
                    printf("  [%08x]", *(((DWORD *)&DataArray->Value) + i));

                    if (HashTable.count(*(((DWORD *)&DataArray->Value) + i)))
                    {
                        printf(":%s", HashTable[*(((DWORD *)&DataArray->Value) + i)].c_str());
                    }
                }
            }

            printf("\n");
        }
    }

    DWORD64 MySendPacket(DWORD64 a1, DWORD64 a2, DWORD64 a3)
    {
        MySendPacket_(a1 + 0x8);
        return originalSendPacket(a1, a2, a3);
    }

    void Hook()
    {
        *present = MyPresent;

#ifdef _DEBUG
        DWORD OldProtect = 0;
        VirtualProtect((SendPacket_t *)((DWORD64)sendPacket + 0x68), 8, PAGE_EXECUTE_READWRITE, &OldProtect);
        *(SendPacket_t *)((DWORD64)sendPacket + 0x68) = MySendPacket;
        VirtualProtect((SendPacket_t *)((DWORD64)sendPacket + 0x68), 8, OldProtect, &OldProtect);

        if (MH_Initialize() == MH_OK)
        {
            MH_CreateHook(CRC32, MyCRC32, (LPVOID *)(&originalCRC32));
            MH_EnableHook(CRC32);
        }
#endif
    }

    void UnHook()
    {
        *present = originalPresent;
        Sleep(100);

#ifdef _DEBUG
        DWORD OldProtect = 0;
        VirtualProtect((SendPacket_t *)((DWORD64)sendPacket + 0x68), 8, PAGE_EXECUTE_READWRITE, &OldProtect);
        *(SendPacket_t *)((DWORD64)sendPacket + 0x68) = originalSendPacket;
        VirtualProtect((SendPacket_t *)((DWORD64)sendPacket + 0x68), 8, OldProtect, &OldProtect);

        MH_Uninitialize();
#endif
    }
} // namespace NBA2K22
