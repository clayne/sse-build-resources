#include <skse64/PapyrusVM.h>
#include <skse64/NiTypes.h>
#include <skse64/GameForms.h>
#include <skse64/GameReferences.h>
#include <skse64/GameRTTI.h>

#include "GameHandles.h"
#include "RTTI.h"

// VMHandle
namespace Game
{

    bool VMHandle::Get(UInt32 a_type, void* a_ptr)
    {
        auto policy = (*g_skyrimVM)->GetClassRegistry()->GetHandlePolicy();
        m_item = policy->Create(a_type, a_ptr);

        return (m_item != policy->GetInvalidHandle());
    }

    VMHandle VMHandle::StripLower() const
    {
        return (m_item & HANDLE_UPPER_MASK);
    }

    FormID VMHandle::GetFormID() const
    {
        return static_cast<FormID::held_type>(m_item & HANDLE_LOWER_MASK);
    }

    bool VMHandle::IsTemporary() const
    {
        return GetFormID().IsTemporary();
    }

    bool VMHandle::GetPluginIndex(UInt32& a_out) const
    {
        return GetFormID().GetPluginIndex(a_out);
    }

    bool VMHandle::GetPluginPartialIndex(UInt32& a_out) const
    {
        return GetFormID().GetPluginPartialIndex(a_out);
    }

    bool VMHandle::IsValid() const
    {
        auto policy = (*g_skyrimVM)->GetClassRegistry()->GetHandlePolicy();

        return (m_item != policy->GetInvalidHandle());
    }

}

// VMHandleRef
namespace Game
{
    VMHandleRef::VMHandleRef(VMHandle a_handle) :
        m_handle(a_handle)
    {
        auto policy = (*g_skyrimVM)->GetClassRegistry()->GetHandlePolicy();
        VMHandle invalidHandle = policy->GetInvalidHandle();

        if (a_handle != invalidHandle)
        {
            policy->AddRef(a_handle);
        }
    }

    void VMHandleRef::release()
    {
        auto policy = (*g_skyrimVM)->GetClassRegistry()->GetHandlePolicy();
        VMHandle invalidHandle = policy->GetInvalidHandle();

        if (m_handle != invalidHandle)
        {
            policy->Release(m_handle);
            m_handle = invalidHandle;
        }
    }

    void VMHandleRef::invalidate()
    {
        auto policy = (*g_skyrimVM)->GetClassRegistry()->GetHandlePolicy();
        m_handle = policy->GetInvalidHandle();
    }
}

// FormID
namespace Game
{
    bool FormID::IsTemporary() const
    {
        return ((m_item & 0xFF000000) >> 24) == 0xFF;
    }

    bool FormID::GetPluginIndex(UInt32& a_out) const
    {
        UInt32 modID = (m_item & 0xFF000000) >> 24;

        if (modID == 0xFF)
            return false;

        a_out = modID;

        return true;
    }

    bool FormID::GetPluginPartialIndex(UInt32& a_out) const
    {
        UInt32 modID = (m_item & 0xFF000000) >> 24;

        if (modID == 0xFF)
            return false;

        if (modID == 0xFE)
            a_out = m_item >> 12;
        else
            a_out = modID;

        return true;
    }

    bool FormID::GetReference(NiPointer<TESObjectREFR>& a_out) const
    {
        auto form = Lookup();
        if (!form)
            return false;

        auto ptr = RTTI<TESObjectREFR>::Cast(form);
        if (!ptr)
            return false;

        a_out = ptr;

        return true;
    }

    TESForm* FormID::Lookup() const
    {
        return LookupFormByID(m_item);
    }

}

// ObjectRefHandle
namespace Game
{
    /*bool ObjectRefHandle::LookupREFR(NiPointer<TESObjectREFR>& a_out)
    {
        if (!IsValid())
            return false;

        LookupREFRByHandle(*this, a_out);

        return a_out != nullptr;
    }*/
    
    bool ObjectRefHandle::LookupREFR(NiPointer<TESObjectREFR>& a_out) const
    {
        if (!IsValid())
            return false;

        LookupREFRObjectByHandle(*this, a_out);

        return a_out != nullptr;
    }

    bool ObjectRefHandle::IsValid() const
    {
        return (m_item != *g_invalidRefHandle);
    }

}