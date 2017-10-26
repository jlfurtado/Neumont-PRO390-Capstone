#pragma once

#include <vector>
#include "VariationController.h"
#include <DirectXMath.h>
#include <malloc.h>

namespace Capstone
{
	class Mesh;
	class VertexGroup
	{
	public:
		VertexGroup();
		~VertexGroup();

		void Initialize(VariationController::VariationChangedCallback cb, Mesh *pMesh);
		void Clear();
		int Count();
		void Update(float dt);
		void Add(int idx);
		const int *GetIndices();
		DirectX::XMMATRIX CalcMTW();
		static void DoNothingOnPurpose(void *);
		VariationController *GetVariationPointer();
		float TX() { return DirectX::XMVectorGetX(m_translation); }
		float TY() { return DirectX::XMVectorGetY(m_translation); }
		float TZ() { return DirectX::XMVectorGetZ(m_translation); }

	private:
		DirectX::XMVECTOR m_scale;
		DirectX::XMVECTOR m_translation;
		DirectX::XMVECTOR m_rotation;
		VariationController m_variation;
		std::vector<int> m_vertexIndices;
		bool Contains(int idx);

		static void LogNotSet(void *);
	};
}