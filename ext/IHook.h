#pragma once

#include "skse64_common/BranchTrampoline.h"

namespace Hook
{
#pragma pack(push, 1)
    struct CB5Code
    {
        uint8_t	op;
        int32_t	displ;
    };

    struct CB6Code
    {
        uint8_t	escape;
        uint8_t	modrm;
        uint32_t displ;
    };
#pragma pack(pop)

    template <uint8_t op>
    bool GetDst5(uintptr_t addr, uintptr_t& out)
    {
        auto ins = reinterpret_cast<CB5Code*>(addr);

        if (ins->op != op) {
            return false;
        }

        if (op == uint8_t(0xE8) || op == uint8_t(0xE9)) {
            out = addr + sizeof(CB5Code) + ins->displ;
        }
        else {
            return false;
        }

        return true;
    }

    template <uint8_t modrm>
    bool GetDst6(uintptr_t addr, uintptr_t& out)
    {
        auto ins = reinterpret_cast<CB6Code*>(addr);

        if (ins->escape != 0xFF || ins->modrm != modrm) {
            return false;
        }

        if (modrm == uint8_t(0x15) || modrm == uint8_t(0x25)) {
            out = *reinterpret_cast<uintptr_t*>(addr + sizeof(CB6Code) + ins->displ);
        }
        else {
            return false;
        }

        return true;
    }

    template <typename T>
    bool Call5(uintptr_t addr, uintptr_t dst, T& orig)
    {
        uintptr_t o;
        if (!GetDst5<0xE8>(addr, o)) {
            return false;
        }

        orig = reinterpret_cast<T>(o);

        g_branchTrampoline.Write5Call(addr, dst);

        return true;
    }

    template <typename T>
    bool Jmp5(uintptr_t addr, uintptr_t dst, T& orig)
    {
        uintptr_t o;
        if (!GetDst5<0xE9>(addr, o)) {
            return false;
        }

        orig = reinterpret_cast<T>(o);

        g_branchTrampoline.Write5Branch(addr, dst);

        return true;
    }

    template <typename T>
    bool Call6(uintptr_t addr, uintptr_t dst, T& orig)
    {
        uintptr_t o;
        if (!GetDst6<0x15>(addr, o)) {
            return false;
        }

        orig = reinterpret_cast<T>(o);

        g_branchTrampoline.Write6Call(addr, dst);

        return true;
    }

    template <typename T>
    bool Jmp6(uintptr_t addr, uintptr_t dst, T& orig)
    {
        uintptr_t o;
        if (!GetDst6<0x25>(addr, o)) {
            return false;
        }

        orig = reinterpret_cast<T>(o);

        g_branchTrampoline.Write6Branch(addr, dst);

        return true;
    }
}