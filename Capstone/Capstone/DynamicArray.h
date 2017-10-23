#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

// Justin Furtado
// 6/30/2017
// DynamicArray.h
// Defines a templated array class that resizes 

#include "DebugConsole.h"

namespace Capstone
{
	template<class T>
	class DynamicArray
	{
	public:
		// construct a default-sized dynamic array
		DynamicArray() : m_size(DEFAULT_SIZE), m_next(0), m_pData(new T[DEFAULT_SIZE]) {}

		// construct a dynamic array with a new starting size
		DynamicArray(unsigned size) : m_size(size), m_next(0), m_pData(new T[size]) {}

		// clean up any leftover memory
		~DynamicArray() { if (m_pData) { delete[] m_pData; } }

		// allow array indexing into array
		T& operator[](unsigned index)
		{
			if (index >= m_size) { DebugConsole::Log("Tried to access index [%d] of DynamicArray of size [%d]\n", index, m_size); }
			return m_pData[index];
		}

		// add a piece of data to the array
		void Add(T data)
		{
			// handle resize
			if (m_next >= m_size)
			{
				// allocate new data, update size (2* == bitshift)
				T* pData = new T[m_size <<= 1];

				// copy data over (old size of data)
				std::memcpy(pData, m_pData, sizeof(T) * (m_size >> 1));

				// release old data
				delete[] m_pData;

				// point to new array
				m_pData = pData;
			}

			// add element to new array (or untouched one)
			m_pData[m_next++] = data;
		}

		// see if element in
		bool Contains(T data) const
		{
			// loop through
			for (unsigned i = 0; i < m_next; ++i)
			{
				// if same its in here
				if (data == m_pData[i]) { return true; }
			}

			// not found, not inside
			return false;
		}

		// get rid of first instance of data
		void RemoveFirst(T data)
		{
			// loop through elements
			for (unsigned i = 0; i < m_next; ++i)
			{
				// if its there, shift other stuff over
				if (data == m_pData[i]) 
				{
					// don't forget to reduce count by one
					std::memcpy(m_pData + i, m_pData + i + 1, (--m_next - i) * sizeof(T));
					break;
				}
			}
		}

		// returns then number of elements in the array
		unsigned GetCount()
		{
			return m_next;
		}

		// start adding at the beginning again
		void Clear()
		{
			m_next = 0;
		}

		// set all the values to value then start adding at the beginning again
		void Clear(T value)
		{
			for (unsigned i = 0; i < m_next; ++i)
			{
				m_pData[i] = value;
			}

			m_next = 0;
		}

	private:
		static const int DEFAULT_SIZE = 256;
		T *m_pData;
		unsigned m_size;
		unsigned m_next;
	};
}

#endif // ifndef DYNAMICARRAY_H