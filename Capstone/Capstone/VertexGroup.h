#pragma once

#include <vector>
#include "VariationController.h"
#include <DirectXMath.h>
#include <malloc.h>
#include "VariationType.h"

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
		void RemoveFirst(int idx);
		const int *GetIndices();
		DirectX::XMMATRIX CalcMTW();
		static void DoNothingOnPurpose(void *, int);
		VariationController *GetVariationPointer();
		DirectX::XMVECTOR GetPivot();
		void SetPivot(const DirectX::XMVECTOR& pivot);
		void SetVariationType(VariationType type);
		unsigned GetNumVertices();
		bool Contains(int idx);
		
	private:
		DirectX::XMVECTOR m_scale;
		DirectX::XMVECTOR m_translation;
		DirectX::XMVECTOR m_rotation;
		DirectX::XMVECTOR m_pivot;
		VariationController m_variation;
		std::vector<int> m_vertexIndices;


		static void LogNotSet(void *);

		friend class CustomIO;
	};
}