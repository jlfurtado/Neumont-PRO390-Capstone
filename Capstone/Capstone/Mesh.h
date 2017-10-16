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
		void CalcMatrix();
		int GetVertexBufferSize();
		void Update(float dt);
		void SaveLow();
		void SaveHigh();
		void Vary();
		void RestoreLow();
		void RestoreHigh();

	private:
		struct VERTEX
		{
			float X, Y, Z;      // position
			float r, g, b, a;   // color
		};

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

		VERTEX m_cubeVertices[36] =
		{
			/*   X      Y      Z     R     G     B     A */
			{ -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f },
			{ +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f },
			{ -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f },
			{ +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f },
			{ +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f },
			{ -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f },

			{ +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f },
			{ -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f },
			{ +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f },
			{ -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f },
			{ -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f },
			{ +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f },

			{ -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f },
			{ -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f },
			{ -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f },
			{ -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f },
			{ -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f },
			{ -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f },

			{ +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f },
			{ +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f },
			{ +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f },
			{ +1.0f, +1.0f,  1.0f, +1.0f, +1.0f,  1.0f, +1.0f },
			{ +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f },
			{ +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f },

			{ -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f },
			{ +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f },
			{ -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f },
			{ +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f },
			{ +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f },
			{ -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f },

			{ +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f },
			{ -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f },
			{ +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f },
			{ -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f },
			{ -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f },
			{ +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f }
		};

		const int NUM_VERTICES = sizeof(m_cubeVertices) / sizeof(VERTEX);
	};
}

