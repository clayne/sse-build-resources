#pragma once

#include "common/ITypes.h"
#include "PW.h"
#include "Hash.h"

class TESObjectREFR;
class TESForm;

template <class T>
class NiPointer;

namespace Game
{
    struct FormID;
    struct ObjectHandle :
        IntegralWrapper<UInt64>
    {
        static inline constexpr UInt64 HANDLE_UPPER_MASK = 0xFFFFFFFF00000000ULL;

        using IntegralWrapper<UInt64>::IntegralWrapper;
        using IntegralWrapper<UInt64>::operator=;

        template <class T>
        [[nodiscard]] T* Resolve() const;

        template <class T>
        [[nodiscard]] bool Get(T* a_ptr);
        template <class T>
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

        /*template <class T>
        [[nodiscard]] T* Lookup() const;*/

    };

    static_assert(sizeof(FormID) == sizeof(UInt32));

    struct ActorHandle :
        IntegralWrapper<UInt32>
    {
        using IntegralWrapper<UInt32>::IntegralWrapper;
        using IntegralWrapper<UInt32>::operator=;

        [[nodiscard]] bool LookupREFR(NiPointer<TESObjectREFR>& a_out) const;
        [[nodiscard]] bool IsValid() const;
    };

    static_assert(sizeof(ActorHandle) == sizeof(UInt32));

}

#include <skse64/PapyrusVM.h>

namespace Game
{
    template <class T>
    T* ObjectHandle::Resolve() const
    {
        auto policy = (*g_skyrimVM)->GetClassRegistry()->GetHandlePolicy();

        if (m_item == policy->GetInvalidHandle()) {
            return nullptr;
        }

        return static_cast<T*>(policy->Resolve(static_cast<UInt32>(T::kTypeID), *this));
    }

    template <class T>
    bool ObjectHandle::Get(T* a_ptr)
    {
        return Get(T::kTypeID, a_ptr);
    }

    template <class T>
    bool ObjectHandle::Get(NiPointer<T>& a_niptr)
    {
        return Get(T::kTypeID, a_niptr.get());
    }

}

STD_SPECIALIZE_HASH(::Game::ObjectHandle)
STD_SPECIALIZE_HASH(::Game::FormID)
STD_SPECIALIZE_HASH(::Game::ActorHandle)
