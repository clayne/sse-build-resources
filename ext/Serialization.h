#pragma once

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/iostreams/concepts.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>

#include <iostream>
#include <string>

struct SKSESerializationInterface;

struct IStringSink :
	public boost::iostreams::sink
{
	IStringSink(std::string& a_dataHolder) :
		m_data(a_dataHolder)
	{}

	std::streamsize write(
		const char* a_data,
		std::streamsize a_len)
	{
		m_data.append(a_data, a_len);
		return a_len;
	}

	std::string& m_data;
};

class ISerializationBase :
	virtual ILog
{
public:
	bool WriteRecord(
		SKSESerializationInterface* a_intfc,
		std::uint32_t a_type,
		std::uint32_t a_version);

	bool ReadRecord(
		SKSESerializationInterface* a_intfc,
		std::uint32_t a_type,
		std::uint32_t a_version);

private:
	virtual std::size_t Store(
		boost::archive::binary_oarchive& a_out) = 0;

	virtual std::size_t Load(
		SKSESerializationInterface* a_intfc,
		std::uint32_t a_version,
		boost::archive::binary_iarchive& a_in) = 0;
};
