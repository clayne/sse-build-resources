#pragma once

#define FN_NAMEPROC(x) virtual const char *ModuleName() const { return x; };

#include <string>
#include <exception>

namespace except
{
    class descriptor
    {
    public:
        descriptor() noexcept
        {
        }

        descriptor(std::exception const& a_rhs) noexcept
        {
            m_desc = a_rhs.what();
        }

        descriptor& operator=(std::exception const& a_rhs) noexcept
        {
            m_desc = a_rhs.what();
            return *this;
        }

        inline const char* what() const noexcept {
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
    SelectedItem() :
        m_isSelected(false)
    {
    }

    SelectedItem(const T& a_rhs) :
        m_isSelected(true),
        m_item(a_rhs)
    {
    }

    SelectedItem(T&& a_rhs) :
        m_isSelected(true),
        m_item(a_rhs)
    {
    }

    virtual ~SelectedItem() noexcept = default;

    inline void Set(const T& a_rhs) {
        m_isSelected = true;
        m_item = a_rhs;
    }

    inline void Set(T&& a_rhs) {
        m_isSelected = true;
        m_item = std::forward<T>(a_rhs);
    }

    inline SelectedItem<T>& operator=(const T& a_rhs) {
        m_isSelected = true;
        m_item = a_rhs;
        return *this;
    }

    inline SelectedItem<T>& operator=(T&& a_rhs) {
        m_isSelected = true;
        m_item = std::forward<T>(a_rhs);
        return *this;
    }

    inline bool operator==(const T& a_rhs) const {
        if (!m_isSelected)
            return false;

        return m_item == a_rhs;
    }

    inline bool operator!=(const T& a_rhs) const {
        if (!m_isSelected)
            return true;

        return m_item != a_rhs;
    }

    inline bool operator==(const SelectedItem<T>& a_rhs) const {
        if (!m_isSelected)
            return false;

        return m_item == a_rhs.m_item;
    }

    inline void Clear() noexcept {
        m_isSelected = false;
    }

    [[nodiscard]] inline const T& Get() const noexcept {
        return m_item;
    }

    [[nodiscard]] inline const T& operator*() const noexcept {
        return m_item;
    }

    [[nodiscard]] inline T& operator*() noexcept {
        return m_item;
    }

    [[nodiscard]] inline const T* operator->() const noexcept {
        return std::addressof(m_item);
    }

    [[nodiscard]] inline T* operator->() noexcept {
        return std::addressof(m_item);
    }

    [[nodiscard]] inline bool Has() const noexcept {
        return m_isSelected;
    }

    [[nodiscard]] inline explicit operator bool() const noexcept {
        return m_isSelected;
    }

private:
    T m_item;
    bool m_isSelected;
};

#include "IMisc.h"
#include "PerfCounter.h"
#include "AddressLibrary.h"
#include "RTTI.h"
#include "Patching.h"
#include "IHook.h"
#include "ILogging.h"
