#pragma once

#include <DirectXMath.h>

namespace Capstone
{
	class Mesh
	{
	public:
		Mesh();
		~Mesh();

		DirectX::XMMATRIX *GetMTWMatrixPtr();
		float *GetVertexPointer();
		int GetStride();
		int GetVertexCount();
		int GetFloatsPerVertex();
		void CalcMatrix();
		int GetVertexBufferSize();
		void Update(float dt);
		void SaveLow();
		void SaveHigh();
		void VaryVectorUniform();
		void VaryVectorBellApproximation();
		void VaryComponentUniform();
		void VaryComponentBellApproximation();
		void VarySmoothBellApproximation();
		void VarySmoothUniform();
		void RestoreLow();
		void RestoreHigh();
		bool LoadMesh(const char *const filePath);
		void ClearObjectLevelVariations();
		void UpdateSelectedColors();

	private:
		DirectX::XMMATRIX m_modelToWorld;
		DirectX::XMVECTOR m_scale;
		DirectX::XMVECTOR m_translation;
		DirectX::XMVECTOR m_rotation;
		DirectX::XMVECTOR m_lowScale;
		DirectX::XMVECTOR m_lowTranslation;
		DirectX::XMVECTOR m_lowRotation;
		DirectX::XMVECTOR m_highScale;
		DirectX::XMVECTOR m_highTranslation;
		DirectX::XMVECTOR m_highRotation;

		float *m_pVerts;
		float *m_pBaseVerts;
		int m_stride;
		int m_vertexCount;
		int m_floatsPerVertex;
		const char *const m_format;

		void ReleaseVerts();
		void SetColor(int idx, float r, float g, float b);
	};
}

