#pragma once

#include "DXBase.h"

namespace Capstone
{
	class Editor : public DXBase
	{
		public:
			Editor();
			virtual ~Editor();

			bool LoadContent();
			void UnloadContent();

			void Update(float dt);
			void Render();

	private:
		struct VERTEX
		{
			FLOAT X, Y, Z;      // position
			FLOAT r, g, b, a;   // color
		};

		VERTEX cubeVertices[36] =
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

		const int NUM_VERTICES = sizeof(cubeVertices) / sizeof(VERTEX);

		float bgColorRGBA[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float otherColorRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		int ct = 0;

		const int loop = 10000;
		const float loopF = (float)loop;
		const int halfLoop = loop / 2;
		const float halfLoopF = loopF / 2.0f;
		ID3D11VertexShader *pVS = 0;    // the vertex shader
		ID3D11PixelShader *pPS = 0;     // the pixel shader
		ID3D11Buffer *pVBuffer = 0;    // vertex buffer
		ID3D11InputLayout *pLayout = 0;    // global
		const char *const VERTEX_SHADER_STR = "vs_5_0";
		const char *const PIXEL_SHADER_STR = "ps_5_0";
		ID3D11Buffer *m_pModelToWorldBuffer = 0;
		ID3D11Buffer *m_pWorldToViewBuffer = 0;
		ID3D11Buffer *m_pProjectionBuffer = 0;

	};
}
