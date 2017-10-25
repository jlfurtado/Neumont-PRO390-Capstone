#pragma once

#include "DynamicArray.h"
#include "VariationController.h"
#include <DirectXMath.h>

namespace Capstone
{
	class Mesh;
	class VertexGroup
	{
	public:
		VertexGroup(VariationController::VariationChangedCallback cb, Mesh *pMesh);
		~VertexGroup();

		void Clear();
		int Count();
		void Update(float dt);
		void Add(int idx);
		const int *GetIndices();
		DirectX::XMMATRIX CalcMTW();

	private:
		DirectX::XMVECTOR m_scale;
		DirectX::XMVECTOR m_translation;
		DirectX::XMVECTOR m_rotation;
		DynamicArray<int> m_vertexIndices;
		VariationController m_variation;

	};


}
