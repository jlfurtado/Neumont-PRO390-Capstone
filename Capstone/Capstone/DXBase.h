#pragma once

#include<d3d11.h>

class DXBase
{
public:
	DXBase();
	virtual ~DXBase();

	bool Initialize(HINSTANCE intanceHandle, HWND windowHandle);
	void Shutdown();

	bool CompileD3DShader(LPCWSTR filePath, const char *const entry, const char *const shaderModel, ID3DBlob** buffer);

	virtual bool LoadContent();
	virtual void UnloadContent();

	virtual void Update(float dt) = 0;
	virtual void Render() = 0;

protected:
	HINSTANCE m_instanceHandle;
	HWND m_windowHandle;

	D3D_DRIVER_TYPE m_driverType;
	D3D_FEATURE_LEVEL m_featureLevel;

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
	IDXGISwapChain* m_swapChain;
	ID3D11RenderTargetView* m_backBufferTarget;

	ID3D11Texture2D* m_depthTexture;
	ID3D11DepthStencilView* m_depthStencilView;
};
