#pragma once

#include "STLCommon.h"

#include <skse64/GameSettings.h>

class ISettingCollection
{
public:

    template <class T, class type = stl::strip_type<T>>
    [[nodiscard]] static type* GetINISettingAddr(const char* name)
    {
        auto setting = (*g_iniSettingCollection)->Get(name);
        if (setting) {
            return reinterpret_cast<type*>(&setting->data);
        }

        return nullptr;
    };

    template <class T, class type = stl::strip_type<T>>
    [[nodiscard]] static type* GetINIPrefSettingAddr(const char* a_name)
    {
        auto setting = (*g_iniPrefSettingCollection)->Get(a_name);
        if (setting) {
            return reinterpret_cast<type*>(&setting->data);
        }

        return nullptr;
    };

    template <class T, class type = stl::strip_type<T>>
    [[nodiscard]] static type* GetGameSettingAddr(const char* a_name)
    {
        auto setting = (*g_gameSettingCollection)->Get(a_name);
        if (setting) {
            return reinterpret_cast<type*>(&setting->data);
        }

        return nullptr;
    };

};