#pragma once
#include <memory>

namespace Capstone
{
	class MyUtils
	{
	public:
		template<typename T>
		static void MyClearFunc(T *const data)
		{
			memset(data, NULL, sizeof(*data));
		}

		template<typename T>
		static void MyClearFunc(T *const data, int count)
		{
			memset(data, NULL, sizeof(*data) * count);
		}
	};
}
