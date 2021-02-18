#pragma once

#define FN_NAMEPROC(x) virtual const char *ModuleName() const noexcept { return x; };

#define SKMP_FORCEINLINE __forceinline
#define SKMP_NOINLINE __declspec(noinline)
#define SKMP_ALIGN(x) __declspec(align(x))

#include <string>
#include <exception>

namespace except
{
    class descriptor
    {
    public:

        SKMP_FORCEINLINE descriptor() :
            m_desc(std::exception().what())
        {
        }

        SKMP_FORCEINLINE descriptor(std::exception const& a_rhs)
        {
            m_desc = a_rhs.what();
        }

        SKMP_FORCEINLINE descriptor& operator=(std::exception const& a_rhs)
        {
            m_desc = a_rhs.what();
            return *this;
        }

        SKMP_FORCEINLINE descriptor& operator=(const char* a_desc)
        {
            m_desc = a_desc;
            return *this;
        }

        SKMP_FORCEINLINE const char* what() const noexcept {
            return m_desc.c_str();
        }

    private:
        std::string m_desc;
    };
}

template <class T>
class SelectedItem
{
public:
    SKMP_FORCEINLINE SelectedItem() :
        m_isSelected(false)
    {
    }

    SKMP_FORCEINLINE SelectedItem(const T& a_rhs) :
        m_isSelected(true),
        m_item(a_rhs)
    {
    }

    SKMP_FORCEINLINE SelectedItem(T&& a_rhs) :
        m_isSelected(true),
        m_item(a_rhs)
    {
    }

    virtual ~SelectedItem() noexcept = default;

    SKMP_FORCEINLINE void Set(const T& a_rhs) {
        m_isSelected = true;
        m_item = a_rhs;
    }

    SKMP_FORCEINLINE void Set(T&& a_rhs) {
        m_isSelected = true;
        m_item = std::move(a_rhs);
    }

    SKMP_FORCEINLINE SelectedItem<T>& operator=(const T& a_rhs) {
        m_item = a_rhs;
        m_isSelected = true;
        return *this;
    }

    SKMP_FORCEINLINE SelectedItem<T>& operator=(T&& a_rhs) {
        m_item = std::move(a_rhs);
        m_isSelected = true;
        return *this;
    }

    SKMP_FORCEINLINE bool operator==(const T& a_rhs) const {
        return m_isSelected && m_item == a_rhs;
    }

    SKMP_FORCEINLINE bool operator!=(const T& a_rhs) const {
        return !m_isSelected || m_item != a_rhs;
    }

    SKMP_FORCEINLINE bool operator==(const SelectedItem<T>& a_rhs) const {
        return m_isSelected && m_item == a_rhs.m_item;
    }

    SKMP_FORCEINLINE void Clear() noexcept {
        m_isSelected = false;
    }

    [[nodiscard]] SKMP_FORCEINLINE const T& Get() const noexcept {
        return m_item;
    }

    [[nodiscard]] SKMP_FORCEINLINE const T& operator*() const noexcept {
        return m_item;
    }

    [[nodiscard]] SKMP_FORCEINLINE T& operator*() noexcept {
        return m_item;
    }

    [[nodiscard]] SKMP_FORCEINLINE const T* operator->() const noexcept {
        return std::addressof(m_item);
    }

    [[nodiscard]] SKMP_FORCEINLINE T* operator->() noexcept {
        return std::addressof(m_item);
    }

    [[nodiscard]] SKMP_FORCEINLINE bool Has() const noexcept {
        return m_isSelected;
    }

    [[nodiscard]] SKMP_FORCEINLINE explicit operator bool() const noexcept {
        return m_isSelected;
    }

    SKMP_FORCEINLINE void Mark(bool a_switch) {
        m_isSelected = a_switch;
    }

private:
    T m_item;
    bool m_isSelected;
};

#include "Mem.h"
#include "Hash.h"
#include "STL.h"
#include "IMisc.h"
#include "Math.h"
#include "PerfCounter.h"
#include "AddressLibrary.h"
#include "RTTI.h"
#include "Patching.h"
#include "IHook.h"
#include "ILogging.h"
#include "GameHandles.h"