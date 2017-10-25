#pragma once

#include <DirectXMath.h>
#include "VariationController.h"
#include "VertexGroup.h"

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
		void InitTestGroup();

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
		static void DoNothing(void *);

		static void UpdateTestGroup(void *pMesh);
		VertexGroup m_testGroup;
		Editor *m_pEditor;

		int m_lastMouseX;
		int m_lastMouseY;
		bool m_clicked;
	};
}

