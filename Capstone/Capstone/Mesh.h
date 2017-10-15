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

