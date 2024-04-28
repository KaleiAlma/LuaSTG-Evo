#include "Core/Type.hpp"
#include "Core/Object.hpp"
#include <cstdint>

namespace Core
{
	template<typename I>
	inline bool is_pow_of_2(I const n)
	{
		return (n > 0) && (0 == (n & (n - 1)));
	}

	inline bool check_size_and_align(size_t const size, size_t const align)
	{
		assert(size > 0);
		if (size == 0)
		{
			spdlog::error("[core] [Core::Data::Data] size is 0");
			return false;
		}

	#if (SIZE_MAX == UINT64_MAX)
		assert(size <= INT64_MAX);
		if (size > INT64_MAX)
		{
			spdlog::error("[core] [Core::Data::Data] size is larger than INT64_MAX");
			return false;
		}
	#elif (SIZE_MAX == UINT32_MAX)
		assert(size <= INT32_MAX);
		if (size > INT32_MAX)
		{
			spdlog::error("[core] [Core::Data::Data] size is larger than INT32_MAX");
			return false;
		}
	#else
		static_assert(false, "unsupported size_t");
	#endif

		assert(align > 0);
		if (align == 0)
		{
			spdlog::error("[core] [Core::Data::Data] invalid alignment 0");
			return false;
		}

		assert(is_pow_of_2(align));
		if (!is_pow_of_2(align))
		{
			spdlog::error("[core] [Core::Data::Data] invalid alignment {}, required pow of 2", align);
			return false;
		}

		return true;
	}

	class DataObject : public Object<IData>
	{
	private:
		uint8_t* m_data;
		size_t m_aligned : 1;
	#if (SIZE_MAX == UINT64_MAX)
		size_t m_size : 63;
	#elif (SIZE_MAX == UINT32_MAX)
		size_t m_size : 31;
	#else
		static_assert(false, "unsupported size_t");
	#endif
	public:
		void* data() { return m_data; }
		size_t size() { return m_size; }
	public:
		DataObject(size_t size)
			: m_data(nullptr)
			, m_aligned(0)
			, m_size(0)
		{
			if (!check_size_and_align(size, sizeof(void*))) return;
			m_data = (uint8_t*)std::malloc(size);
			if (m_data) m_size = size;
		}
		DataObject(size_t size, size_t align)
			: m_data(nullptr)
			, m_aligned(1)
			, m_size(0)
		{
			if (!check_size_and_align(size, align)) return;
			assert(align > sizeof(std::max_align_t)); // 如果你遇到了这个断言，说明你在做没必要的对齐分配
#ifdef WIN32
			m_data = (uint8_t*)_aligned_malloc(size, align);
#else
			m_data = (uint8_t*)aligned_alloc(size, align);
#endif
			if (m_data) m_size = size;
		}
		virtual ~DataObject()
		{
			if (m_data)
			{
				// if (m_aligned)
				// 	_aligned_free(m_data);
				// else
					std::free(m_data);
			}
			m_data = nullptr;
			m_size = 0;
		}
	};

	bool IData::create(size_t size, IData** pp_data)
	{
		ScopeObject<DataObject> p_data;
		try
		{
			p_data.attach(new DataObject(size));
			if (!p_data->data()) return false;
		}
		catch (...)
		{
			return false;
		}
		*pp_data = p_data.detach();
		return true;
	}
	bool IData::create(size_t size, size_t align, IData** pp_data)
	{
		ScopeObject<DataObject> p_data;
		try
		{
			p_data.attach(new DataObject(size, align));
			if (!p_data->data()) return false;
		}
		catch (...)
		{
			return false;
		}
		*pp_data = p_data.detach();
		return true;
	}

	// 虚函数表 + 引用计数 + 数据指针 + 数据大小
	static_assert(sizeof(DataObject) == sizeof(size_t[4]));
}
