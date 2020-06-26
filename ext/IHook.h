#pragma once

#include "skse64_common/BranchTrampoline.h"

namespace Hook
{
#pragma pack(push, 1)
    struct Call5Code
    {
        uint8_t	op;
        int32_t	displ;
    };

    struct Call6Code
    {
        uint8_t	escape;
        uint8_t	modrm;
        uint32_t displ;
    };
#pragma pack(pop)

    template <uint8_t op>
    __inline bool GetDst5(uintptr_t addr, uintptr_t& out)
    {
        auto ins = reinterpret_cast<Call5Code*>(addr);

        if (ins->op != op) {
            return false;
        }

        if (op == uint8_t(0xE8)) {
            out = addr + sizeof(Call5Code) + ins->displ;
        }
        else {
            return false;
        }

        return true;
    }

    __inline bool GetDst6(uintptr_t addr, uintptr_t& out)
    {
        auto ins = reinterpret_cast<Call6Code*>(addr);

        if (ins->escape != 0xFF ||
            ins->modrm != 0x15) {
            return false;
        }

        out = *reinterpret_cast<uintptr_t*>(addr + sizeof(Call6Code) + ins->displ);

        return true;
    }

    template <typename T>
    bool Call5(uintptr_t addr, uintptr_t dst, T &orig)
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
    bool Call6(uintptr_t addr, uintptr_t dst, T& orig)
    {
        uintptr_t o;
        if (!GetDst6(addr, o)) {
            return false;
        }

        orig = reinterpret_cast<T>(o);

        g_branchTrampoline.Write6Call(addr, dst);

        return true;
    }
}