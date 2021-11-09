#pragma once

#include "ICommon.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>
#include <boost/iostreams/concepts.hpp>
#include <boost/archive/binary_oarchive.hpp> 
#include <boost/archive/binary_iarchive.hpp> 

#include <string>
#include <iostream>


#define BOOST_CLASS_TEMPLATE_VERSION(Template, Type, Version) \
namespace boost {                                           \
  namespace serialization {                                 \
    template<Template>                                      \
    struct version<Type> {                                  \
      static constexpr unsigned int value = Version;        \
    };                                                      \
    template<Template>                                      \
    constexpr unsigned int version<Type>::value;            \
  }                                                         \
}

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
        boost::archive::binary_oarchive& a_out
    ) = 0;

    virtual std::size_t Load(
        SKSESerializationInterface* a_intfc,
        std::uint32_t a_version, 
        boost::archive::binary_iarchive& a_in
    ) = 0;
};

template <class T>
class SerializedSOWrapper :
    public SetObjectWrapper<T>
{
    friend class boost::serialization::access;

public:

    enum Serialization : unsigned int
    {
        DataVersion1 = 1
    };

    using SetObjectWrapper<T>::SetObjectWrapper;

private:

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& m_set;
        if (m_set) {
            ar& m_item;
        }
    }
};

BOOST_CLASS_TEMPLATE_VERSION(class T, SerializedSOWrapper<T>, SerializedSOWrapper<T>::Serialization::DataVersion1);