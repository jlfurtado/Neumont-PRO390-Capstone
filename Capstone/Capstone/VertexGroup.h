#pragma once

#include "DynamicArray.h"

namespace Capstone
{
	class VertexGroup
	{
	public:
		VertexGroup(); 
		~VertexGroup();

	private:
		DynamicArray<int> m_vertexIndices;
	};


}
