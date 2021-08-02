#pragma once

#include "common/ITypes.h"
#include "PW.h"
#include "Hash.h"
#include "RTTI.h"
#include "STLCommon.h"
#include "AddressLibrary.h"

#include <skse64/NiTypes.h>

class TESObjectREFR;
class Actor;
class Projectile;
class TESForm;

namespace Game
{
    struct FormID;

    struct VMHandle :
        IntegralWrapper<std::uint64_t>
    {
        static inline constexpr std::uint64_t HANDLE_UPPER_MASK = 0xFFFFFFFF00000000ui64;
        static inline constexpr std::uint64_t HANDLE_LOWER_MASK = 0x00000000FFFFFFFFui64;

        using IntegralWrapper<std::uint64_t>::IntegralWrapper;
        using IntegralWrapper<std::uint64_t>::operator=;

        template <class T, class form_type = stl::strip_type<T>>
        [[nodiscard]] form_type* Resolve() const;

        template <class T, class form_type = stl::strip_type<T>>
        [[nodiscard]] bool Get(T* a_ptr);
        template <class T, class form_type = stl::strip_type<T>>
        [[nodiscard]] bool Get(NiPointer<T>& a_ptr);

        [[nodiscard]] bool Get(std::uint32_t a_type, void* a_ptr);

        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] bool IsTemporary() const;
        [[nodiscard]] VMHandle StripLower() const;

        [[nodiscard]] FormID GetFormID() const;
        [[nodiscard]] bool GetPluginIndex(std::uint32_t& a_out) const;
        [[nodiscard]] bool GetPluginPartialIndex(std::uint32_t& a_out) const;
    };

    static_assert(sizeof(VMHandle) == sizeof(std::uint64_t));

    class VMHandleRef
    {
    public:

        VMHandleRef() = delete;

        explicit VMHandleRef(VMHandle a_handle);

        SKMP_FORCEINLINE ~VMHandleRef() {
            release();
        }

        void release();
        void invalidate();

        [[nodiscard]] SKMP_FORCEINLINE VMHandle get() const {
            return m_handle;
        }

    private:
        VMHandle m_handle;
    };

    struct FormID :
        IntegralWrapper<std::uint32_t>
    {
        using IntegralWrapper<std::uint32_t>::IntegralWrapper;
        using IntegralWrapper<std::uint32_t>::operator=;

        [[nodiscard]] bool IsTemporary() const;
        [[nodiscard]] bool GetPluginIndex(std::uint32_t& a_out) const;
        [[nodiscard]] bool GetPluginPartialIndex(std::uint32_t& a_out) const;
        [[nodiscard]] bool GetReference(NiPointer<TESObjectREFR>& a_out) const;
        [[nodiscard]] TESForm* Lookup() const;

        template <class T, class form_type = stl::strip_type<T>>
        [[nodiscard]] form_type* Lookup() const;

        template <class T, class form_type = stl::strip_type<T>>
        [[nodiscard]] form_type* As() const;

    };

    static_assert(sizeof(FormID) == sizeof(std::uint32_t));

    template <class T>
    struct BSPointerHandle :
        IntegralWrapper<std::uint32_t>
    {
    public:

        using IntegralWrapper<std::uint32_t>::IntegralWrapper;
        using IntegralWrapper<std::uint32_t>::operator=;

        [[nodiscard]] bool Lookup(NiPointer<T>& a_out) const;
        [[nodiscard]] bool IsValid() const;

    private:

        static inline auto m_invalidHandle = IAL::Address<std::uint32_t*>(514164);
    };

    using ObjectRefHandle = BSPointerHandle<TESObjectREFR>;
    using ActorHandle = BSPointerHandle<Actor>;
    using ProjectileHandle = BSPointerHandle<Projectile>;

    static_assert(sizeof(ObjectRefHandle) == sizeof(std::uint32_t));
    static_assert(sizeof(ActorHandle) == sizeof(std::uint32_t));
    static_assert(sizeof(ProjectileHandle) == sizeof(std::uint32_t));


    template <class T>
    class BSPointerHandleInterface
    {
    public:

        using handle_type = BSPointerHandle<T>;

    private:

        using funcLookupByHandle_t = bool (*)(const handle_type&, NiPointer<T>&);
        using funcGetHandle_t = handle_type(*)(T*);

    public:

        static inline const auto Lookup = IAL::Address<funcLookupByHandle_t>(12204);
        static inline const auto GetHandle = IAL::Address<funcGetHandle_t>(15967);

    };

}

STD_SPECIALIZE_HASH(::Game::VMHandle);
STD_SPECIALIZE_HASH(::Game::FormID);
STD_SPECIALIZE_HASH(::Game::ObjectRefHandle)
STD_SPECIALIZE_HASH(::Game::ActorHandle)
STD_SPECIALIZE_HASH(::Game::ProjectileHandle)

#include <skse64/PapyrusVM.h>

namespace Game
{
    template <class T, class form_type>
    form_type* VMHandle::Resolve() const
    {
        auto policy = (*g_skyrimVM)->GetClassRegistry()->GetHandlePolicy();

        if (m_item == policy->GetInvalidHandle()) {
            return nullptr;
        }

        return static_cast<form_type*>(policy->Resolve(static_cast<std::uint32_t>(form_type::kTypeID), *this));
    }

    template <class T, class form_type>
    bool VMHandle::Get(T* a_ptr)
    {
        return Get(form_type::kTypeID, a_ptr);
    }

    template <class T, class form_type>
    bool VMHandle::Get(NiPointer<T>& a_niptr)
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
    
    template <class T, class form_type>
    form_type* FormID::As() const
    {
        auto form = Lookup();

        if (form) {
            return form->As<form_type>();
        }

        return nullptr;
    }

}

namespace Game
{
    template <class T>
    bool BSPointerHandle<T>::Lookup(NiPointer<T>& a_out) const
    {
        BSPointerHandleInterface<T>::Lookup(*this, a_out);

        return a_out != nullptr;
    }

    template <class T>
    bool BSPointerHandle<T>::IsValid() const
    {
        return (m_item != *m_invalidHandle);
    }
}