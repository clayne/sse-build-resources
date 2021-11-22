#pragma once

#include "PerfCounter.h"
#include "versiondb.h"

#include <memory>

class IAL
{
public:
	static void Release();

	static constexpr bool IsLoaded()
	{
		return m_Instance.m_isLoaded;
	}

	inline static constexpr float GetLoadTime()
	{
		return IPerfCounter::delta(
			m_Instance.m_tLoadStart,
			m_Instance.m_tLoadEnd);
	}

	inline static constexpr long long GetLoadStart()
	{
		return m_Instance.m_tLoadStart;
	}

	inline static constexpr long long GetLoadEnd()
	{
		return m_Instance.m_tLoadEnd;
	}

	inline static constexpr bool HasBadQuery()
	{
		return m_Instance.m_hasBadQuery;
	}

	inline static std::size_t Size()
	{
		return m_Instance.m_database.GetOffsetMap().size();
	}

	template <class T = std::uintptr_t>
	SKMP_NOINLINE static T Addr(
		unsigned long long a_id_se,
		unsigned long long a_id_ae,
		std::ptrdiff_t a_offset_se = 0,
		std::ptrdiff_t a_offset_ae = 0)
	{
		unsigned long long id;
		std::ptrdiff_t offset;

		if (m_Instance.m_isAE)
		{
			id = a_id_ae;
			offset = a_offset_ae;
		}
		else
		{
			id = a_id_se;
			offset = a_offset_se;
		}

		if (id == 0)
		{
			return T(0);
		}

		auto addr = m_Instance.m_database.FindAddressById(id);
		if (!addr)
		{
			m_Instance.m_hasBadQuery = true;
			return T(0);
		}

		auto res = reinterpret_cast<std::uintptr_t>(addr) + offset;

		if constexpr (std::is_same_v<T, std::uintptr_t>)
		{
			return res;
		}
		else
		{
			return reinterpret_cast<T>(res);
		}
	}

	/*static bool Offset(
		unsigned long long id,
		std::uintptr_t& result)
	{
		unsigned long long r;
		if (!m_Instance.m_database.FindOffsetById(id, r))
		{
			m_Instance.m_hasBadQuery = true;
			return false;
		}

		result = static_cast<std::uintptr_t>(r);

		return true;
	}*/

	static std::uintptr_t Offset(
		unsigned long long a_id_se,
		unsigned long long a_id_ae)
	{
		auto id = m_Instance.m_isAE ? a_id_ae : a_id_se;

		unsigned long long r;
		if (!m_Instance.m_database.FindOffsetById(id, r))
		{
			m_Instance.m_hasBadQuery = true;
			return std::uintptr_t(0);
		}
		return static_cast<std::uintptr_t>(r);
	}

	template <class T>
	class Address
	{
	public:
		Address() = delete;

		Address(
			unsigned long long a_id_se,
			unsigned long long a_id_ae,
			std::ptrdiff_t a_offset_se = 0,
			std::ptrdiff_t a_offset_ae = 0) :
			m_offset(IAL::Addr<BlockConversionType*>(
				a_id_se,
				a_id_ae,
				a_offset_se,
				a_offset_ae))
		{
		}

		inline constexpr operator T() const noexcept
		{
			return reinterpret_cast<T>(const_cast<BlockConversionType*>(m_offset));
		}

		inline constexpr std::uintptr_t GetUIntPtr() const noexcept
		{
			return reinterpret_cast<std::uintptr_t>(m_offset);
		}

	private:
		struct BlockConversionType
		{};
		BlockConversionType* m_offset;
	};

	static inline constexpr bool IsAE() noexcept
	{
		return m_Instance.m_isAE;
	}

private:
	bool IAL::get_ver(int (&a_parts)[4], std::uint64_t& a_out);

	IAL();
	~IAL() = default;

	bool m_isLoaded{ false };
	bool m_hasBadQuery{ false };
	long long m_tLoadStart{ 0 };
	long long m_tLoadEnd{ 0 };

	bool m_isAE{ false };

	VersionDb m_database;

	static IAL m_Instance;
};
