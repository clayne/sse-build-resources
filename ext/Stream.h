#pragma once

#include <skse64/NiSerialization.h>

#include <cstdint>
#include <memory>

namespace Util
{
	namespace Stream
	{
		class NiStreamWrapper
		{
		public:
			NiStreamWrapper();
			virtual ~NiStreamWrapper() noexcept;

			NiStreamWrapper(NiStreamWrapper&& a_rhs)
			{
				m_data = std::move(a_rhs.m_data);
			}

			NiStreamWrapper& operator=(NiStreamWrapper&& a_rhs)
			{
				m_data = std::move(a_rhs.m_data);
				return *this;
			}

			inline NiStream* operator->()
			{
				return reinterpret_cast<NiStream*>(m_data.get());
			}

			inline const NiStream* operator->() const
			{
				return reinterpret_cast<const NiStream*>(m_data.get());
			}

			inline operator NiStream*()
			{
				return reinterpret_cast<NiStream*>(m_data.get());
			}

			inline operator const NiStream*() const
			{
				return reinterpret_cast<const NiStream*>(m_data.get());
			}

			inline operator NiStream&()
			{
				return *reinterpret_cast<NiStream*>(m_data.get());
			}

			inline operator const NiStream&() const
			{
				return *reinterpret_cast<const NiStream*>(m_data.get());
			}

		private:
			std::unique_ptr<std::uint8_t[]> m_data;
		};

	}
}