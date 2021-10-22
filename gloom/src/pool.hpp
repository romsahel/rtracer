#pragma once

#include <vector>
#include <queue>

template <typename T>
class Pool
{
public:
	T& Take()
	{
		T* obj;
		if (m_free.empty())
		{
			obj = new T();
			m_values.push_back(obj);
		}
		else
		{
			obj = m_free.front();
			m_free.pop();
		}

		return *obj;
	}

	bool Release(const T& obj)
	{
		auto it = std::find(m_values.begin(), m_values.end(), &obj);
		if (it != m_values.end())
		{
			m_free.push(*it);
			return true;
		}
		return false;
	}

private:
	std::vector<T*> m_values;
	std::queue<T*> m_free;
};
