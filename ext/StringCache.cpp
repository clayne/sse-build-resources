#if defined(SKMP_ENABLE_STRING_CACHE)

#	pragma warning(disable: 4073)
#	pragma init_seg(lib)

#	include "StringCache.h"
#	include "Hash.h"

namespace string_cache
{
	data_pool data_pool::m_Instance;

	data_pool::data_pool()
	{
		auto empty = std::string();

		auto& e = *m_data.emplace(
							 value_type::compute_hash(empty),
							 std::move(empty))
		               .first;

		m_icase_empty = std::addressof(e);
	}

	auto data_pool::insert(
		std::size_t a_hash,
		const std::string& a_string)
		-> const value_type&
	{
		return *m_Instance.m_data.emplace(a_hash, a_string).first;
	}

	auto data_pool::insert(
		std::size_t a_hash,
		std::string&& a_string)
		-> const value_type&
	{
		return *m_Instance.m_data.emplace(a_hash, std::move(a_string)).first;
	}

	void data_pool::release(
		const value_type* a_ref) noexcept
	{
		if (!get_data().erase(*a_ref))
		{
			HALT("FIXME: orphaned value");
		}
	}

	template <bool _Lock>
	void data_pool::try_release(
		const value_type* a_ref) noexcept
	{
		if (a_ref->dec_refcount() != 1)
		{
			return;
		}

		if constexpr (_Lock)
		{
			IScopedLock lock(get_lock());

			if (a_ref->use_count() == 0)
			{
				release(a_ref);
			}
		}
		else
		{
			release(a_ref);
		}
	}

	auto data_pool::get_stats() noexcept
		-> stats_t
	{
		IScopedLock lock(get_lock());

		stats_t result;

		auto& data = get_data();

		result.count = data.size();

		/*for (stl::strip_type<decltype(data)>::size_type i = 0; i < data.bucket_count(); ++i) {
                std::size_t bucket_size = data.bucket_size(i);
                if (bucket_size == 0) {
                    result.map_usage++;
                }
                else {
                    result.map_usage += bucket_size;
                }
            }*/

		for (auto& entry : data)
		{
			result.key_data_usage += sizeof(stl::strip_type<decltype(entry)>);
			result.string_usage += entry.m_value.capacity() + sizeof(decltype(entry.m_value));

			auto c = entry.use_count();

			result.total_fixed_count += c;
			result.total_fixed_size += c * sizeof(stl::fixed_string);

			result.estimated_uncached_usage += (entry.m_value.capacity() + sizeof(decltype(entry.m_value))) * c;
		}

		result.total = result.map_usage +
		               result.key_data_usage +
		               result.string_usage +
		               result.total_fixed_size;

		result.ratio = static_cast<long double>(result.total) / static_cast<long double>(result.estimated_uncached_usage);

		return result;
	}

}

namespace stl
{
	using namespace string_cache;

	void fixed_string::set(const char* a_value)
	{
		if (a_value == nullptr)
		{
			clear();
		}
		else
		{
			set_impl(std::string(a_value));
		}
	}
}

#endif