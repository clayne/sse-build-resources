
#include "Stream.h"

namespace Util
{
    namespace Stream
    {
        NiStreamWrapper::NiStreamWrapper() :
            m_data(std::make_unique<std::uint8_t[]>(sizeof(NiStream)))
        {
            this->operator->()->ctor();
        }

        NiStreamWrapper::~NiStreamWrapper() noexcept
        {
            if (m_data.get()) {
                this->operator->()->dtor();
            }
        }
    }
}