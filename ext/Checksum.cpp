#include "Checksum.h"

#include <boost/crc.hpp>

namespace hash
{
    std::uint64_t crc64(const void* a_data, std::size_t a_size)
    {
        // CRC-64/XZ (CRC-64/GO-ECMA)
        boost::crc_optimal<64, 0x42f0e1eba9ea3693, 0xffffffffffffffff, 0xffffffffffffffff, true, true> crc;
        crc.process_bytes(a_data, a_size);
        return crc.checksum();
    }

    std::uint64_t crc64(const std::string& a_in)
    {
        return crc64(a_in.data(), a_in.size());
    }

    void sha1(const void* a_data, std::size_t a_size, boost::uuids::detail::sha1::digest_type& a_out)
    {
        boost::uuids::detail::sha1 sha1;
        sha1.process_bytes(a_data, a_size);
        sha1.get_digest(a_out);
    }

    void sha1(const std::string& a_in, boost::uuids::detail::sha1::digest_type& a_out)
    {
        sha1(a_in.data(), a_in.size(), a_out);
    }
}