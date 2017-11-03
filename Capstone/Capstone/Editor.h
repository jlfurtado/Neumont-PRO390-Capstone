#pragma once

#include "DXBase.h"
#include <DirectXMath.h>
#include "Camera.h"
#include "Mesh.h"
#include "UniformManager.h"
#include "Frustum.h"

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
			bool LoadObj(const char *const filePath);
			void ReSendMeshVerticesSameBuffer();
			bool SetPivotCamera();
			bool SetPivotXYZ(float x, float y, float z);
			bool SetPivotCenter();
			bool SetVariationType(VariationType type);
			bool WriteMeshToFile(const char *const filePath);
			bool ReadMeshFromFile(const char *const filePath);
			bool SetCameraSpeed(float speed);
			bool SetCameraRotateSpeed(float speed);

	private:
		void ReSendUtilVerticesSameBuffer();
		void ReSendVerticesSameBuffer(ID3D11Buffer **pBuffer, size_t bufferSize, float *pData, size_t stride);
		void RenderMesh();
		void RenderUtils();
		void RenderPivot();
		void ExitFullScreen();
		void MakeMeshVertexBuffer();
		void MakeUtilityVertexBuffer();
		void MakePivotVertexBuffer();
		void MakeBuffer(ID3D11Buffer **pBuffer, size_t bufferSize, float *pData, size_t byteWidth);
		void CalculatePerspectiveMatrix();
		void LogFPS(float dt, float interval);
		void UtilFromMousePercents(float lowX, float lowY, float highX, float highY);
		void CalcNormalsFor(float *pVerts, int numVerts, int stride, int normalOffset);

		float bgColorRGBA[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float otherColorRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		ID3D11VertexShader *pPCNVertShader = 0;    // the vertex shader
		ID3D11PixelShader *pPCNPixelShader = 0;     // the pixel shader
		ID3D11VertexShader *pPCVertShader = 0;    // the vertex shader
		ID3D11PixelShader *pPCPixelShader = 0;     // the pixel shader
		ID3D11Buffer *pMeshVertexBuffer = 0;    // mesh vertex buffer
		ID3D11Buffer *pUtilityVertexBuffer = 0;    // utility vertex buffer
		ID3D11Buffer *pPivotVertexBuffer = 0;    // pivot vertex buffer

		ID3D11InputLayout *pPCNLayout = 0;
		ID3D11InputLayout *pPCLayout = 0; 

		const char *const VERTEX_SHADER_STR = "vs_5_0";
		const char *const PIXEL_SHADER_STR = "ps_5_0";

		float m_t = 0.0f;
		float m_timer = 0.0f;
		const float m_loopTime = 10.0f;
		const float m_halfLoopTime = m_loopTime / 2.0f;

		UniformManager m_meshUniformManager;
		UniformManager m_utilUniformManager;
		UniformManager m_pivotUniformManager;

		Camera m_camera;
		Mesh m_mesh;
		DirectX::XMMATRIX m_projectionMatrix;
		DirectX::XMMATRIX m_inverseTransposeModelToWorldMatrix;
		DirectX::XMVECTOR m_lightColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMVECTOR m_lightPos{ 0.0f, 50.0f, 0.0f, 1.0f };
		DirectX::XMVECTOR m_diffuseColor{ 0.7f, 0.7f, 0.7f, 1.0f };
		DirectX::XMVECTOR m_ambientColor{ 0.1f, 0.1f, 0.1f, 1.0f };
		DirectX::XMVECTOR m_specularColor{ 0.1f, 0.1f, 0.1f, 1.0f };
		DirectX::XMVECTOR m_specularIntensity{ 32.0f, 0.0f, 0.0f, 0.0f };
		DirectX::XMVECTOR m_interp{ 0.75f };
		DirectX::XMMATRIX m_identity;
		DirectX::XMMATRIX m_pivotMTW;

		bool m_initialized{ false };
		float m_fovy{ DirectX::XM_PIDIV4 };
		float m_nearClip{ 0.01f };
		float m_farClip{ 100.0f };

		int m_lastMouseX;
		int m_lastMouseY;
		bool m_clicked;

		static const int UTIL_VERTS = 6;
		static const int UTIL_FLOATS_PER_VERTEX = 10;
		static const int UTIL_FLOATS = UTIL_VERTS * UTIL_FLOATS_PER_VERTEX;
		float m_utilVerts[UTIL_FLOATS] = {
			/* X      Y      Z      R      G      B      A      X      Y      Z*/
			-1.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.3f, +0.0f, +0.0f, -1.0f,
			+1.0f, -1.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.3f, +0.0f, +0.0f, -1.0f,
			-1.0f, -1.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.3f, +0.0f, +0.0f, -1.0f,
			+1.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.3f, +0.0f, +0.0f, -1.0f,
			+1.0f, -1.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.3f, +0.0f, +0.0f, -1.0f,
			-1.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.3f, +0.0f, +0.0f, -1.0f
		};

		static const int PIVOT_VERTS = 12;
		static const int PIVOT_FLOATS_PER_VERTEX = 10;
		static const int PIVOT_FLOATS = PIVOT_VERTS * PIVOT_FLOATS_PER_VERTEX;
		float m_pivotVerts[PIVOT_FLOATS] = {
			// 0 1 3
			-1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +0.0f, +1.0f, +0.0f, +0.0f, +0.0f, // Front left
			+1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +0.0f, +1.0f, +0.0f, +0.0f, +0.0f, // Front right
			+0.0f, +1.0f, +0.0f, +1.0f, +1.0f, +0.0f, +1.0f, +0.0f, +0.0f, +0.0f, // Top middle
															 
			// 1 2 3										 
			+1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +0.0f, +1.0f, +0.0f, +0.0f, +0.0f, // Front right
			+0.0f, -1.0f, -1.0f, +1.0f, +1.0f, +0.0f, +1.0f, +0.0f, +0.0f, +0.0f, // Back middle
			+0.0f, +1.0f, +0.0f, +1.0f, +1.0f, +0.0f, +1.0f, +0.0f, +0.0f, +0.0f, // Top middle
															
			// 2 0 3										
			+0.0f, -1.0f, -1.0f, +1.0f, +1.0f, +0.0f, +1.0f, +0.0f, +0.0f, +0.0f, // Back middle
			-1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +0.0f, +1.0f, +0.0f, +0.0f, +0.0f, // Front left
			+0.0f, +1.0f, +0.0f, +1.0f, +1.0f, +0.0f, +1.0f, +0.0f, +0.0f, +0.0f, // Top middle
															
			// 0 2 1										
			-1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +0.0f, +1.0f, +0.0f, +0.0f, +0.0f, // Front left
			+0.0f, -1.0f, -1.0f, +1.0f, +1.0f, +0.0f, +1.0f, +0.0f, +0.0f, +0.0f, // Back middle
			+1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +0.0f, +1.0f, +0.0f, +0.0f, +0.0f  // Front right
		};
	};
}