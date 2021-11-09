#include "Serialization.h"

#include <skse64/PluginAPI.h>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

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
    std::uint32_t compressedSize;

    compressed.reserve(1024 * 512);

    std::size_t num;
    std::streamoff size;

    try
    {
        boost::archive::binary_oarchive arch(ss);

        num = Store(arch);

        size = ss.tellp();
    }
    catch (const std::exception& e)
    {
        Error("%s [%.4s] [Store]: %s", __FUNCTION__, &a_type, e.what());
        return false;
    }

    if (!size)
    {
        Error("%s [%.4s]: no data was written", __FUNCTION__, &a_type);
        return false;
    }

    try
    {
        using namespace boost::iostreams;

        filtering_streambuf<input> in;
        in.push(gzip_compressor(gzip_params(1), 1024 * 64));
        in.push(ss);

        auto l = copy(in, out, 1024 * 64);

        if (l > std::numeric_limits<std::uint32_t>::max())
        {
            throw std::exception("size overflow");
        }

        compressedSize = static_cast<std::uint32_t>(l);
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

    if (!compressedSize)
    {
        Error("%s [%.4s]: compressed data len = 0?", __FUNCTION__, &a_type);
        return false;
    }

    if (!a_intfc->OpenRecord(a_type, a_version))
    {
        Error("%s [%.4s]: OpenRecord failed", __FUNCTION__, &a_type);
        return false;
    }

    if (!a_intfc->WriteRecordData(&compressedSize, sizeof(compressedSize)))
    {
        Error("%s [%.4s]: Failed writing record data length", __FUNCTION__, &a_type);
        return false;
    }

    if (!a_intfc->WriteRecordData(compressed.data(), compressedSize))
    {
        Error("%s [%.4s]: Failed writing record data (%u)", __FUNCTION__, &a_type, compressedSize);
        return false;
    }

    auto ratio = static_cast<long double>(compressedSize) / static_cast<long double>(size);

    Debug("%s [%.4s]: %zu record(s), %fs [%u/%lld r:%.2Lf]", __FUNCTION__, &a_type, num, pt.Stop(), compressedSize, size, ratio);

    return true;
}

bool ISerializationBase::ReadRecord(
    SKSESerializationInterface* a_intfc,
    std::uint32_t a_type,
    std::uint32_t a_version)
{
    PerfTimer pt;
    pt.Start();

    std::uint32_t compressedSize;
    if (!a_intfc->ReadRecordData(&compressedSize, sizeof(compressedSize)))
    {
        Error("%s [%.4s]: Couldn't read record data length", __FUNCTION__, &a_type);
        return false;
    }

    if (compressedSize == 0)
    {
        Error("%s [%.4s]: Record data length == 0", __FUNCTION__, &a_type);
        return false;
    }

    auto data = std::make_unique_for_overwrite<char[]>(compressedSize);

    if (a_intfc->ReadRecordData(data.get(), compressedSize) != compressedSize)
    {
        Error("%s [%.4s]: Couldn't read record data", __FUNCTION__, &a_type);
        return false;
    }

    std::stringstream out;
    std::streamsize size;

    try
    {
        using namespace boost::iostreams;

        using Device = basic_array_source<char>;
        stream<Device> stream(data.get(), compressedSize);

        filtering_streambuf<input> in;

        in.push(gzip_decompressor(zlib::default_window_bits, 1024 * 64));
        in.push(stream);

        size = copy(in, out, 1024 * 64);
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

    if (!size)
    {
        Error("%s [%.4s]: decompressed data len = 0?", __FUNCTION__, &a_type);
        return false;
    }

    std::size_t num;

    try
    {
        boost::archive::binary_iarchive arch(out);

        num = Load(a_intfc, a_version, arch);
    }
    catch (const std::exception& e)
    {
        Error("%s [%.4s] [Load]: %s", __FUNCTION__, &a_type, e.what());
        return false;
    }

    Debug("%s [%.4s]: %zu record(s), %fs [%u/%lld]", __FUNCTION__, &a_type, num, pt.Stop(), compressedSize, size);

    return true;

}