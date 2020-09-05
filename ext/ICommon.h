#pragma once

#define FN_NAMEPROC(x) virtual const char *ModuleName() { return x; };

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

#include "IMisc.h"
#include "PerfCounter.h"
#include "AddressLibrary.h"
#include "RTTI.h"
#include "Patching.h"
#include "IHook.h"
#include "ILogging.h"
#include "IAssert.h"
