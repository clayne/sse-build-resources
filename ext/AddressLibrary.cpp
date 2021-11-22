#include "AddressLibrary.h"
#include "PerfCounter.h"

#pragma warning(disable: 4073)
#pragma init_seg(lib)

IAL IAL::m_Instance;

bool IAL::get_ver(int (&a_parts)[4], std::uint64_t& a_out)
{
	std::uint64_t result{ 0 };

	if (!m_database.GetExecutableVersion(
			a_parts[0],
			a_parts[1],
			a_parts[2],
			a_parts[3]))
	{
		return false;
	}

	for (int i = 0; i < 4; i++)
	{
		if (a_parts[i] > 0xFFFF)
			return false;

		result <<= 16;
		result |= a_parts[i];
	}

	a_out = result;

	return true;
}

IAL::IAL()
{
	PerfCounter pc;

	int parts[4]{ 0 };

	std::uint64_t ver;
	if (!get_ver(parts, ver))
	{
		return;
	}

	m_isAE = ver > 0x0001000500610000;

	m_tLoadStart = pc.Query();

	m_isLoaded = m_database.Load(
		m_isAE ? 2 : 1,
		parts[0],
		parts[1],
		parts[2],
		parts[3]);

	m_tLoadEnd = pc.Query();
}

void IAL::Release()
{
	m_Instance.m_database.clear();
}
