#pragma once

#include <DirectXMath.h>
#include "VariationController.h"
#include "VertexGroup.h"
#include "Frustum.h"
#include <vector>
#include "VariationType.h"

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
		bool PreMultiply(int count);
		bool Multiply(const DirectX::XMVECTOR& offset);
		bool Multiply2D(int count1, int count2, const DirectX::XMVECTOR& offset1, const DirectX::XMVECTOR& offset2);
		bool Singularify();
		int GetNumMeshes();

		bool LoadMeshOBJ(const char *const filePath);
		bool ExportCurrentMeshOBJ(const char *const filePath) const;
		bool ExportVariedMeshesOBJ(int count, const char *const filePath);
		void ClearObjectLevelVariations();
		void ColorAll(float r, float g, float b);
		void ClearVertexGroups();

		void SelectVerticesInFrustum(const Frustum& frustum);
		void AddVerticesInFrustum(const Frustum& frustum);
		void RemoveVerticesInFrustum(const Frustum& frustum);

		bool SetPivotCoords(float x, float y, float z);
		bool SetPivotCenter();

		bool SetVariationType(VariationType type);
		bool WriteToFile(const char *const filePath);
		bool ReadFromFile(const char *const filePath);
		DirectX::XMMATRIX GetPivotTranslation();
		unsigned GetNumVertexGroups();
		bool SelectVertexGroup(int idx);
		bool GetVertexGroupInfo(int group, unsigned &outNumVerts);
		bool RemoveVertexGroup(int idx);
		bool Vary();
		bool CopyVerticesTo(float *pArray);
		bool OffsetTransformVertsIntoArray(const DirectX::XMVECTOR& offset, const DirectX::XMMATRIX& transform, float *pArray, int instanceIndex) const;

	private:
		void ColorCurrentGroup();
		void ColorMesh();
		void ColorWholeMesh();

		DirectX::XMMATRIX m_modelToWorld;
		DirectX::XMVECTOR m_scale;
		DirectX::XMVECTOR m_translation;
		DirectX::XMVECTOR m_rotation;
		VariationController m_objectLevelVariation;

		float *m_pVerts;
		float *m_pBaseVerts;
		int m_numMeshes{ 1 };
		int m_stride;
		int m_vertexCount;
		int m_floatsPerVertex;
		const char *const m_format;

		void ReleaseVerts();
		void ReleaseCurrentVerts();
		void ReleaseBaseVerts();
		void SetColor(int idx, float r, float g, float b);

		bool SetVariationTypeForCurrentGroup(VariationType type);
		bool SetObjectLevelVariationType(VariationType type);

		static void UpdateCurrentVertexGroup(void *pMesh, int instanceIdx);
		static void UpdateAllVertexGroups(void *pMesh, int instanceIdx);
		static void UpdateVertexGroup(void *pMesh, int groupIdx, int instanceIndex);

		bool InitVariations();
		bool InitObjectLevelVariaitions();
		bool InitVertexGroupVariations();

		std::vector<VertexGroup> m_testGroups;
		int m_currentVertexGroup{ -1 };
		Editor *m_pEditor;
	};
}

