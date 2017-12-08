#pragma once

#include<d3d11.h>
namespace Capstone
{
	class EditorWindow;
	class DXBase
	{
	public:
		DXBase();
		virtual ~DXBase();

		bool Initialize(HINSTANCE intanceHandle, HWND windowHandle, EditorWindow *pWindow);
		void Shutdown();

		bool CompileD3DShader(LPCWSTR filePath, const char *const entry, const char *const shaderModel, ID3DBlob** buffer);

		virtual bool LoadContent();
		virtual void UnloadContent();

		virtual void Update(float dt) = 0;
		virtual void Render() = 0;

		void EnableBlend();
		void DisableBlend();

		void EnableCulling();
		void DisableCulling();

		template<typename T>
		static void SafeRelease(T *&pUnknown)
		{
			static_assert(std::is_base_of<IUnknown, T>::value, "CUSTOM ERROR: Can't safe release anything that is not IUnknown.");
			if (pUnknown)
			{
				pUnknown->Release();
				pUnknown = nullptr;
			}
		}

	protected:
		HINSTANCE m_instanceHandle;
		HWND m_windowHandle;

		D3D_DRIVER_TYPE m_driverType;
		D3D_FEATURE_LEVEL m_featureLevel;

		ID3D11Device* m_device = 0;
		ID3D11DeviceContext* m_context = 0;
		IDXGISwapChain* m_swapChain = 0;
		ID3D11RenderTargetView* m_backBufferTarget = 0;

		ID3D11Texture2D* m_depthTexture = 0;
		ID3D11DepthStencilView* m_depthStencilView = 0;
		EditorWindow *m_pMyWindow = 0;

		ID3D11BlendState* m_alphaEnableBlendingState = 0;
		ID3D11BlendState* m_alphaDisableBlendingState = 0;

		ID3D11RasterizerState* m_cullOnState = 0;
		ID3D11RasterizerState* m_cullOffState = 0;
	};
}
