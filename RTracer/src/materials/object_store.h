#pragma once

template <typename object_t>
class object_store
{
public:
	template <typename T, class... Args>
	T& add(Args&&... args)
	{
		auto* added = new T(std::forward<Args>(args)...);
		m_list.push_back(added);
		return *added;
	}

	virtual ~object_store()
	{
		for (object_t* obj : m_list)
		{
			delete obj;
		}
	}

	constexpr auto begin() const noexcept
	{
		return m_list.begin();
	}

	constexpr auto end() const noexcept
	{
		return m_list.end();
	}

	auto size() const noexcept
	{
		return m_list.size();
	}

	const auto& operator[](size_t index) const
	{
		return m_list[index];
	}

	auto& operator[](size_t index)
	{
		return m_list[index];
	}

private:
	std::vector<object_t*> m_list;
};
