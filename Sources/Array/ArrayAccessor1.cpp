/*************************************************************************
> File Name: ArrayAccessor1.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: 1-D array accessor class.
> Created Time: 2017/01/28
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#include <Array/ArrayAccessor1.h>

#include <algorithm>
#include <cassert>

namespace CubbyFlow
{
	template <typename T>
	ArrayAccessor<T, 1>::ArrayAccessor() :
		m_size(0), m_data(nullptr)
	{

	}

	template <typename T>
	ArrayAccessor<T, 1>::ArrayAccessor(size_t size, T* const data)
	{
		Reset(size, data);
	}

	template <typename T>
	ArrayAccessor<T, 1>::ArrayAccessor(const ArrayAccessor& other)
	{
		Set(other);
	}

	template <typename T>
	void ArrayAccessor<T, 1>::Set(const ArrayAccessor& other)
	{
		Reset(other.m_size, other.m_data);
	}

	template <typename T>
	void ArrayAccessor<T, 1>::Reset(size_t size, T* const data)
	{
		m_size = size;
		m_data = data;
	}

	template <typename T>
	T& ArrayAccessor<T, 1>::At(size_t i)
	{
		assert(i < m_size);
		return m_data[i];
	}

	template <typename T>
	const T& ArrayAccessor<T, 1>::At(size_t i) const
	{
		assert(i < m_size);
		return m_data[i];
	}

	template <typename T>
	T* const ArrayAccessor<T, 1>::Begin() const
	{
		return m_data;
	}
	
	template <typename T>
	T* const ArrayAccessor<T, 1>::End() const
	{
		return m_data + m_size;
	}
	
	template <typename T>
	T* ArrayAccessor<T, 1>::Begin()
	{
		return m_data;
	}

	template <typename T>
	T* ArrayAccessor<T, 1>::End()
	{
		return m_data + m_size;
	}

	template <typename T>
	size_t ArrayAccessor<T, 1>::Size() const
	{
		return m_size;
	}

	template <typename T>
	T* const ArrayAccessor<T, 1>::Data() const
	{
		return m_data;
	}

	template <typename T>
	void ArrayAccessor<T, 1>::Swap(ArrayAccessor& other)
	{
		std::swap(other.m_data, m_data);
		std::swap(other.m_size, m_size);
	}

	template <typename T>
	template <typename Callback>
	void ArrayAccessor<T, 1>::ForEach(Callback func) const
	{
		for (size_t i = 0; i < Size(); ++i)
		{
			func(At(i));
		}
	}

	template <typename T>
	template <typename Callback>
	void ArrayAccessor<T, 1>::ForEachIndex(Callback func) const
	{
		for (size_t i = 0; i < Size(); ++i)
		{
			func(i);
		}
	}

	template <typename T>
	template <typename Callback>
	void ArrayAccessor<T, 1>::ParallelForEach(Callback func)
	{
		ParallelFor(ZERO_SIZE, Size(), [&](size_t i)
		{
			func(At(i));
		});
	}

	template <typename T>
	template <typename Callback>
	void ArrayAccessor<T, 1>::ParallelForEachIndex(Callback func) const
	{
		ParallelFor(ZERO_SIZE, Size(), func);
	}

	template <typename T>
	T& ArrayAccessor<T, 1>::operator[](size_t i)
	{
		return m_data[i];
	}

	template <typename T>
	const T& ArrayAccessor<T, 1>::operator[](size_t i) const
	{
		return m_data[i];
	}

	template <typename T>
	ArrayAccessor<T, 1>& ArrayAccessor<T, 1>::operator=(const ArrayAccessor& other)
	{
		Set(other);
		return *this;
	}

	template <typename T>
	ArrayAccessor<T, 1>::operator ConstArrayAccessor<T, 1>() const
	{
		return ConstArrayAccessor<T, 1>(*this);
	}
}