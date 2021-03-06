#include "DXBase.h"
#include <stdio.h>
#include <d3dcompiler.h>
#include "EditorWindow.h"
#include "Utils.h"

namespace Capstone
{
	DXBase::DXBase() : m_driverType(D3D_DRIVER_TYPE_NULL), m_featureLevel(D3D_FEATURE_LEVEL_11_0),
		m_device(0), m_context(0), m_swapChain(0), m_backBufferTarget(0),
		m_depthTexture(0), m_depthStencilView(0)
	{
	}

	DXBase::~DXBase()
	{
		Shutdown();
	}

	bool DXBase::Initialize(HINSTANCE instanceHandle, HWND windowHandle, EditorWindow *pWindow)
	{
		m_pMyWindow = pWindow;
		m_instanceHandle = instanceHandle;
		m_windowHandle = windowHandle;

		RECT dimensions;
		GetClientRect(windowHandle, &dimensions);

		unsigned int width = dimensions.right - dimensions.left;
		unsigned int height = dimensions.bottom - dimensions.top;

		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE, D3D_DRIVER_TYPE_SOFTWARE
		};

		unsigned int totalDriverTypes = ARRAYSIZE(driverTypes);

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0
		};

		unsigned int totalFeatureLevels = ARRAYSIZE(featureLevels);

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = width;
		swapChainDesc.BufferDesc.Height = height;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = windowHandle;
		swapChainDesc.Windowed = true;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;


		unsigned int creationFlags = 0;

#ifdef _DEBUG
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result;
		unsigned int driver = 0;

		for (driver = 0; driver < totalDriverTypes; ++driver)
		{
			result = D3D11CreateDeviceAndSwapChain(0, driverTypes[driver], 0, creationFlags,
				featureLevels, totalFeatureLevels,
				D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain,
				&m_device, &m_featureLevel, &m_context);

			if (SUCCEEDED(result))
			{
				m_driverType = driverTypes[driver];
				break;
			}
		}

		if (FAILED(result))
		{
			DebugConsole::Log("Failed to create the Direct3D device!");
			return false;
		}

		ID3D11Texture2D* backBufferTexture;

		result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTexture);

		if (FAILED(result))
		{
			DebugConsole::Log("Failed to get the swap chain back buffer!");
			return false;
		}

		result = m_device->CreateRenderTargetView(backBufferTexture, 0, &m_backBufferTarget);

		if (backBufferTexture)
			backBufferTexture->Release();

		if (FAILED(result))
		{
			DebugConsole::Log("Failed to create the render target view!");
			return false;
		}

		D3D11_TEXTURE2D_DESC depthTexDesc;
		ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
		depthTexDesc.Width = width;
		depthTexDesc.Height = height;
		depthTexDesc.MipLevels = 1;
		depthTexDesc.ArraySize = 1;
		depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthTexDesc.SampleDesc.Count = 1;
		depthTexDesc.SampleDesc.Quality = 0;
		depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthTexDesc.CPUAccessFlags = 0;
		depthTexDesc.MiscFlags = 0;
		
		result = m_device->CreateTexture2D(&depthTexDesc, NULL, &m_depthTexture);

		if (FAILED(result))
		{
			DebugConsole::Log("Failed to create the depth texture!");
			return false;
		}

		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = depthTexDesc.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		
		result = m_device->CreateDepthStencilView(m_depthTexture, &descDSV, &m_depthStencilView);

		if (FAILED(result))
		{
			DebugConsole::Log("Failed to create the depth stencil target view!");
			return false;
		}

		m_context->OMSetRenderTargets(1, &m_backBufferTarget, m_depthStencilView);

		D3D11_BLEND_DESC blendStateDescription;
		MyUtils::MyClearFunc(&blendStateDescription);

		// Create an alpha enabled blend state description.
		blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
		blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

		// Create the blend state using the description.
		result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
		if (FAILED(result))
		{
			DebugConsole::Log("Failed to create the alpha enabled blend state!");
			return false;
		}

		// Modify the description to create an alpha disabled blend state description.
		blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

		// Create the blend state using the description.
		result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
		if (FAILED(result))
		{
			DebugConsole::Log("Failed to create the alpha disabled blend state!");
			return false;
		}

		// rasterizer state
		D3D11_RASTERIZER_DESC rasterizerStateDescription;
		MyUtils::MyClearFunc(&rasterizerStateDescription);

		rasterizerStateDescription.FillMode = D3D11_FILL_SOLID;
		rasterizerStateDescription.CullMode = D3D11_CULL_FRONT;
		rasterizerStateDescription.FrontCounterClockwise = true;
		rasterizerStateDescription.DepthBias = false;
		rasterizerStateDescription.DepthBiasClamp = 0;
		rasterizerStateDescription.SlopeScaledDepthBias = 0;
		rasterizerStateDescription.DepthClipEnable = true;
		rasterizerStateDescription.ScissorEnable = true;
		rasterizerStateDescription.MultisampleEnable = false;
		rasterizerStateDescription.AntialiasedLineEnable = false;
		result = m_device->CreateRasterizerState(&rasterizerStateDescription, &m_cullOnState);
		if (FAILED(result))
		{
			DebugConsole::Log("Failed to create the cull on rasterizer state!");
			return false;
		}

		rasterizerStateDescription.CullMode = D3D11_CULL_NONE;
		result = m_device->CreateRasterizerState(&rasterizerStateDescription, &m_cullOffState);
		if (FAILED(result))
		{
			DebugConsole::Log("Failed to create the cull off rasterizerstate!");
			return false;
		}

		m_context->RSSetState(m_cullOffState); // on

		D3D11_VIEWPORT viewport;
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		m_context->RSSetViewports(1, &viewport);

		return LoadContent();
	}


	void DXBase::EnableBlend()
	{
		float blendFactor[4]{ 0.0f, 0.0f, 0.0f, 0.0f };

		// Turn on the alpha blending.
		m_context->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);
	}


	void DXBase::DisableBlend()
	{
		float blendFactor[4]{ 0.0f, 0.0f, 0.0f, 0.0f };

		// Turn off the alpha blending.
		m_context->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
	}

	void DXBase::EnableCulling()
	{
		m_context->RSSetState(m_cullOnState);
	}

	void DXBase::DisableCulling()
	{
		m_context->RSSetState(m_cullOffState);
	}

	void DXBase::Shutdown()
	{
		if (m_swapChain) { m_swapChain->SetFullscreenState(FALSE, NULL); }

		UnloadContent();

		SafeRelease(m_cullOffState);
		SafeRelease(m_cullOnState);
		SafeRelease(m_alphaDisableBlendingState);
		SafeRelease(m_alphaEnableBlendingState);
		SafeRelease(m_depthTexture);
		SafeRelease(m_depthStencilView);
		SafeRelease(m_backBufferTarget);
		SafeRelease(m_swapChain);
		SafeRelease(m_context);
		SafeRelease(m_device);
	}

	bool DXBase::CompileD3DShader(LPCWSTR filePath, const char *const entry, const char *const shaderModel, ID3DBlob ** buffer)
	{
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3DCOMPILE_DEBUG;
#endif

		ID3DBlob* errorBuffer = 0;
		HRESULT result;

		result = D3DCompileFromFile(filePath, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, shaderModel,
			shaderFlags, 0, buffer, &errorBuffer);

		if (FAILED(result))
		{
			if (errorBuffer != 0)
			{
				//OutputDebugStringA((char*)errorBuffer->GetBufferPointer());
				DebugConsole::Log((char*)errorBuffer->GetBufferPointer());
				errorBuffer->Release();
			}

			return false;
		}

		if (errorBuffer != 0)
			errorBuffer->Release();

		return true;
	}

	bool DXBase::LoadContent()
	{
		return true;
	}

	void DXBase::UnloadContent()
	{
	}

}
