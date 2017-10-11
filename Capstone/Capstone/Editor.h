#pragma once

#include "DXBase.h"
#include <DirectXMath.h>

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

			void OnWindowResize();

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

		void CalculatePerspectiveMatrix();
		void LogFPS(float dt, float interval);

		const int NUM_VERTICES = sizeof(cubeVertices) / sizeof(VERTEX);

		float bgColorRGBA[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float otherColorRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		ID3D11VertexShader *pVS = 0;    // the vertex shader
		ID3D11PixelShader *pPS = 0;     // the pixel shader
		ID3D11Buffer *pVBuffer = 0;    // vertex buffer
		ID3D11InputLayout *pLayout = 0;    // global
		const char *const VERTEX_SHADER_STR = "vs_5_0";
		const char *const PIXEL_SHADER_STR = "ps_5_0";
		ID3D11Buffer *m_pModelToWorldBuffer = 0;
		ID3D11Buffer *m_pWorldToViewBuffer = 0;
		ID3D11Buffer *m_pProjectionBuffer = 0;
		float m_t = 0.0f;
		float m_timer = 0.0f;
		const float m_loopTime = 10.0f;
		const float m_halfLoopTime = m_loopTime / 2.0f;

		DirectX::XMMATRIX m_projectionMatrix;
	};
}
