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
		void VaryUniform();
		void VaryBellApproximation();
		void VaryComponentUniform();
		void VaryComponentBellApproximation();
		void RestoreLow();
		void RestoreHigh();
		void RandomizeColors();
		void LoadMesh(const char *const filePath);

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

		float *pVerts;
		int m_stride;
		int m_vertexCount;
		int m_floatsPerVertex;
	};
}

