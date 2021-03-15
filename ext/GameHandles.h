#pragma once

#include "common/ITypes.h"
#include "PW.h"
#include "Hash.h"
#include "RTTI.h"
#include "STLCommon.h"

#include <skse64/NiTypes.h>

class TESObjectREFR;
class TESForm;

namespace Game
{
    struct FormID;

    struct ObjectHandle :
        IntegralWrapper<UInt64>
    {
        static inline constexpr UInt64 HANDLE_UPPER_MASK = 0xFFFFFFFF00000000ui64;
        static inline constexpr UInt64 HANDLE_LOWER_MASK = 0x00000000FFFFFFFFui64;

        using IntegralWrapper<UInt64>::IntegralWrapper;
        using IntegralWrapper<UInt64>::operator=;

        template <class T, class form_type = stl::strip_type<T>>
        [[nodiscard]] form_type* Resolve() const;

        template <class T, class form_type = stl::strip_type<T>>
        [[nodiscard]] bool Get(T* a_ptr);
        template <class T, class form_type = stl::strip_type<T>>
        [[nodiscard]] bool Get(NiPointer<T>& a_ptr);

        [[nodiscard]] bool Get(UInt32 a_type, void* a_ptr);

        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] bool IsTemporary() const;
        [[nodiscard]] ObjectHandle StripLower() const;

        [[nodiscard]] FormID GetFormID() const;
        [[nodiscard]] bool GetPluginIndex(UInt32& a_out) const;
        [[nodiscard]] bool GetPluginPartialIndex(UInt32& a_out) const;


    };

    static_assert(sizeof(ObjectHandle) == sizeof(UInt64));

    struct FormID :
        IntegralWrapper<UInt32>
    {
        using IntegralWrapper<UInt32>::IntegralWrapper;
        using IntegralWrapper<UInt32>::operator=;

        [[nodiscard]] bool IsTemporary() const;
        [[nodiscard]] bool GetPluginIndex(UInt32& a_out) const;
        [[nodiscard]] bool GetPluginPartialIndex(UInt32& a_out) const;
        [[nodiscard]] bool GetReference(NiPointer<TESObjectREFR>& a_out) const;
        [[nodiscard]] TESForm* Lookup() const;

        template <class T, class form_type = stl::strip_type<T>>
        [[nodiscard]] form_type* Lookup() const;

    };

    static_assert(sizeof(FormID) == sizeof(UInt32));

    struct ObjectRefHandle :
        IntegralWrapper<UInt32>
    {
        using IntegralWrapper<UInt32>::IntegralWrapper;
        using IntegralWrapper<UInt32>::operator=;

        [[nodiscard]] bool LookupREFR(NiPointer<TESObjectREFR>& a_out) const;
        [[nodiscard]] bool IsValid() const;
    };

    static_assert(sizeof(ObjectRefHandle) == sizeof(UInt32));

}

STD_SPECIALIZE_HASH(::Game::ObjectHandle);
STD_SPECIALIZE_HASH(::Game::FormID);
STD_SPECIALIZE_HASH(::Game::ObjectRefHandle);

#include <skse64/PapyrusVM.h>

namespace Game
{
    template <class T, class form_type>
    form_type* ObjectHandle::Resolve() const
    {
        auto policy = (*g_skyrimVM)->GetClassRegistry()->GetHandlePolicy();

        if (m_item == policy->GetInvalidHandle()) {
            return nullptr;
        }

        return static_cast<form_type*>(policy->Resolve(static_cast<UInt32>(form_type::kTypeID), *this));
    }

    template <class T, class form_type>
    bool ObjectHandle::Get(T* a_ptr)
    {
        return Get(form_type::kTypeID, a_ptr);
    }

    template <class T, class form_type>
    bool ObjectHandle::Get(NiPointer<T>& a_niptr)
    {
        return Get(form_type::kTypeID, a_niptr.get());
    }

}

namespace Game
{
    template <class T, class form_type>
    form_type* FormID::Lookup() const
    {
        auto form = Lookup();

        if (form) {
            return RTTI<form_type>::Cast(form);
        }

        return nullptr;
    }

}