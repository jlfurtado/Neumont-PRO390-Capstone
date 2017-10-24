#pragma once

#include "DynamicArray.h"
#include <DirectXMath.h>

namespace Capstone
{
	class VertexGroup
	{
	public:
		VertexGroup(); 
		~VertexGroup();

		void Clear();
		void Update(float dt);

	private:
		DynamicArray<int> m_vertexIndices;
		DirectX::XMVECTOR m_scale;
		DirectX::XMVECTOR m_translation;
		DirectX::XMVECTOR m_rotation;
		DirectX::XMVECTOR m_lowScale;
		DirectX::XMVECTOR m_lowTranslation;
		DirectX::XMVECTOR m_lowRotation;
		DirectX::XMVECTOR m_highScale;
		DirectX::XMVECTOR m_highTranslation;
		DirectX::XMVECTOR m_highRotation;
	};


}
