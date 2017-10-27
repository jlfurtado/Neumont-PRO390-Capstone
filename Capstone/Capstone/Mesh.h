#pragma once

#include <DirectXMath.h>
#include "VariationController.h"
#include "VertexGroup.h"
#include "Frustum.h"
#include <vector>

namespace Capstone
{
	class Editor;
	class Mesh
	{
	public:
		Mesh(Editor *pEditor);
		~Mesh();

		DirectX::XMMATRIX *GetMTWMatrixPtr();
		float *GetVertexPointer();
		int GetStride();
		int GetVertexCount();
		int GetFloatsPerVertex();
		void CalcMatrix();
		int GetVertexBufferSize();
		void Update(float dt);
		
		bool LoadMesh(const char *const filePath);
		void ClearObjectLevelVariations();
		void ColorAll(float r, float g, float b);
		void ClearVertexGroups();

		void SelectVerticesInFrustum(const Frustum& frustum);
		bool SetPivotCoords(float x, float y, float z);
		bool SetPivotCenter();

	private:
		DirectX::XMMATRIX m_modelToWorld;
		DirectX::XMVECTOR m_scale;
		DirectX::XMVECTOR m_translation;
		DirectX::XMVECTOR m_rotation;
		VariationController m_objectLevelVariation;

		float *m_pVerts;
		float *m_pBaseVerts;
		int m_stride;
		int m_vertexCount;
		int m_floatsPerVertex;
		const char *const m_format;

		void ReleaseVerts();
		void SetColor(int idx, float r, float g, float b);

		static void UpdateCurrentVertexGroup(void *pMesh);
		static void UpdateAllVertexGroups(void *pMesh);
		static void UpdateVertexGroup(void *pMesh, int groupIdx);

		std::vector<VertexGroup> m_testGroups;
		int m_currentVertexGroup{ -1 };
		Editor *m_pEditor;
	};
}

