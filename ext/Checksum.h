#pragma once

#include <boost/uuid/detail/sha1.hpp>

namespace hash
{
    // these may throw

    std::uint64_t crc64(const void* a_data, std::size_t a_size);
    std::uint64_t crc64(const std::string& a_in);

    void sha1(const void* a_data, std::size_t a_size, boost::uuids::detail::sha1::digest_type &a_out);
    void sha1(const std::string& a_in, boost::uuids::detail::sha1::digest_type &a_out);
}