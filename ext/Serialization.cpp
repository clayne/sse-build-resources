#include "Serialization.h"

#include <skse64/PluginAPI.h>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

ISerializationBase::ISerializationBase()
{
}

bool ISerializationBase::WriteRecord(
    SKSESerializationInterface* a_intfc,
    std::uint32_t a_type,
    std::uint32_t a_version)
{
    PerfTimer pt;
    pt.Start();

    std::stringstream ss;
    std::string compressed;
    IStringSink out(compressed);
    std::uint32_t length;

    compressed.reserve(1024 * 512);

    std::size_t num;

    try
    {
        boost::archive::binary_oarchive arch(ss);

        num = Store(arch);
    }
    catch (const std::exception& e)
    {
        Error("%s [%.4s] [Store]: %s", __FUNCTION__, &a_type, e.what());
        return false;
    }

    try
    {
        using namespace boost::iostreams;

        filtering_streambuf<input> in;
        in.push(gzip_compressor(gzip_params(1), 1024 * 64));
        in.push(ss);

        auto l = copy(in, out, 1024 * 64);

        if (l > std::numeric_limits<std::uint32_t>::max()) {
            throw std::exception("Data size overflow");
        }

        length = static_cast<std::uint32_t>(l);
    }
    catch (const boost::iostreams::gzip_error& e)
    {
        Error("%s [%.4s]: %s: %d", __FUNCTION__, &a_type, e.what(), e.error());
        return false;
    }
    catch (const std::exception& e)
    {
        Error("%s [%.4s]: %s", __FUNCTION__, &a_type, e.what());
        return false;
    }

    /*if (!length) {
        Error("%s [%.4s]: no data was compressed", __FUNCTION__, &a_type);
        return false;
    }*/

    if (!a_intfc->OpenRecord(a_type, a_version)) {
        Error("%s [%.4s]: OpenRecord failed", __FUNCTION__, &a_type);
        return false;
    }

    if (!a_intfc->WriteRecordData(&length, sizeof(length))) {
        Error("%s [%.4s]: Failed writing record data length", __FUNCTION__, &a_type);
        return false;
    }

    if (!a_intfc->WriteRecordData(compressed.data(), length)) {
        Error("%s [%.4s]: Failed writing record data (%u)", __FUNCTION__, &a_type, length);
        return false;
    }

    Debug("%s [%.4s]: %zu record(s), %fs (%u b)", __FUNCTION__, &a_type, num, pt.Stop(), length);

    return true;
}

bool ISerializationBase::ReadRecord(
    SKSESerializationInterface* a_intfc,
    std::uint32_t a_type,
    std::uint32_t a_version)
{
    PerfTimer pt;
    pt.Start();

    std::uint32_t dataLength;
    if (!a_intfc->ReadRecordData(&dataLength, sizeof(dataLength)))
    {
        Error("%s [%.4s]: Couldn't read record data length", __FUNCTION__, &a_type);
        return false;
    }

    if (dataLength == 0)
    {
        Error("%s [%.4s]: Record data length == 0", __FUNCTION__, &a_type);
        return false;
    }

    auto data = std::make_unique_for_overwrite<char[]>(dataLength);

    if (a_intfc->ReadRecordData(data.get(), dataLength) != dataLength) {
        Error("%s [%.4s]: Couldn't read record data", __FUNCTION__, &a_type);
        return false;
    }

    std::stringstream out;
    std::streamsize length;

    try
    {
        using namespace boost::iostreams;

        using Device = basic_array_source<char>;
        stream<Device> stream(data.get(), dataLength);

        filtering_streambuf<input> in;

        in.push(gzip_decompressor(zlib::default_window_bits, 1024 * 64));
        in.push(stream);

        auto l = copy(in, out, 1024 * 64);

        if (l > std::numeric_limits<std::uint32_t>::max()) {
            throw std::exception("Data size overflow");
        }

        length = static_cast<std::uint32_t>(l);
    }
    catch (const boost::iostreams::gzip_error& e)
    {
        Error("%s [%.4s]: %s: %d", __FUNCTION__, &a_type, e.what(), e.error());
        return false;
    }
    catch (const std::exception& e)
    {
        Error("%s [%.4s]: %s", __FUNCTION__, &a_type, e.what());
        return false;
    }

    /*if (!length) {
        Error("%s [%.4s]: No data was decompressed", __FUNCTION__, &a_type);
        return false;
    }*/

    try
    {
        boost::archive::binary_iarchive arch(out);

        auto num = Load(a_intfc, a_version, arch);

        Debug("%s [%.4s]: %zu record(s), %fs (%u/%lld)", __FUNCTION__, &a_type, num, pt.Stop(), dataLength, length);

        return true;
    }
    catch (const std::exception& e)
    {
        Error("%s [%.4s] [Load]: %s", __FUNCTION__, &a_type, e.what());
        return false;
    }

}