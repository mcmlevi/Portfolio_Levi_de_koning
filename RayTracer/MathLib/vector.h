#pragma once

#include <cassert>
#include <stdlib.h>
#include <initializer_list>
namespace ldk
{
	template <class T>
	class Vector
	{
	public:
		Vector();

		Vector(const std::initializer_list<T>& list);

		~Vector();
		/**
		 * \brief Adds an element to the back and auto reserves memory if needed.
		 * \param T& element to push.
		 */
		void push_back(const T& elem);
		/**
		 * \brief removes last element
		 */
		void pop();
		/**
		 * \brief reserves memory for the array but doesn't initialize it.
		 * \param size
		 */
		void reserve(const int size);
		/**
		 * \brief Reseves memory and default initializes it
		 * \param size
		 */
		void resize(const int size);
		/**
		 * \brief returns max size.
		 */
		unsigned size();
		/**
		 * \brief returns reserved memory size for the array.
		 */
		unsigned capacity();
		T& front();
		T& back();
		bool empty();
		T& operator[](int index);

	private:
		int _capacity{};
		int _current{};
		T* array;
	};

	template <class T>
	Vector<T>::Vector()
	{
		reserve(2);
		_current = 0;
	}
	template <class T>
	Vector<T>::Vector(const std::initializer_list<T>& list)
	{
		reserve(list.size());
		for (const T& element : list)
		{
			push_back(element);
		}
	}

	template <class T>
	Vector<T>::~Vector()
	{
		for (int i = 0; i < _current; ++i)
		{
			array[i].~T();
		}
		free(array);
		array = nullptr;
		_capacity = 0;
		_current = 0;
	}

	template <class T>
	void Vector<T>::push_back(const T& elem)
	{
		if (_current >= _capacity)
			reserve(_capacity * 2);

		new (array + _current) T{ elem };
		++_current;
	}

	template <class T>
	void Vector<T>::pop()
	{
		assert(_current > 0);
		delete& array[_current--];
	}

	template <class T>
	void Vector<T>::reserve(const int size)
	{
		if (size < _capacity)
			return;

		T* tempArray = static_cast<T*>(malloc(size * sizeof(T)));
		int oldCapacity{ _capacity };
		_capacity = size;
		if (array && tempArray)
			memcpy(tempArray, array, sizeof(T) * _current);

		for (int i = 0; i < oldCapacity; ++i)
		{
			if (array)
				array[i].~T();
		}
		free(array);

		array = tempArray;
	}

	template <class T>
	void Vector<T>::resize(const int size)
	{
		if (size < _capacity)
			return;

		reserve(size);

		for (int current = _current; current < _capacity; ++current)
		{
			array[current] = T{};
		}
		_current = size;
	}

	template <class T>
	unsigned Vector<T>::size()
	{
		return _current;
	}

	template <class T>
	unsigned Vector<T>::capacity()
	{
		return _capacity;
	}

	template <class T>
	T& Vector<T>::front()
	{
		return array[0];
	}

	template <class T>
	T& Vector<T>::back()
	{
		//assert(_current > 1);
		return array[_current - 1];
	}

	template <class T>
	bool Vector<T>::empty()
	{
		return !_current;
	}

	template <class T>
	T& Vector<T>::operator[](int index)
	{
		assert((index < _current || _current == 0) && "OUT OF BOUNDS");
		return array[index];
	}
}
