#pragma once

#include "DXBase.h"
#include <DirectXMath.h>
#include "Camera.h"
#include "Mesh.h"

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

			static void OnMouseScroll(int degrees, void *pInstance);
			void MouseScroll(int degrees);
			void LoadObj(const char *const filePath);

	private:
		void MakeVertexBuffer();
		void CalculatePerspectiveMatrix();
		void LogFPS(float dt, float interval);

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

		Camera m_camera;
		Mesh m_mesh;
		DirectX::XMMATRIX m_projectionMatrix;
	};
}
