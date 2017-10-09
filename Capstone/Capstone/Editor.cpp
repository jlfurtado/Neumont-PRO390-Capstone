#include "Editor.h"
#include "Utils.h"
#include "DebugConsole.h"
#include <DirectXMath.h>

namespace Capstone
{
	Editor::Editor()
	{
	}

	Editor::~Editor()
	{
	}

	bool Editor::LoadContent()
	{
		// create the vertex buffer
		D3D11_BUFFER_DESC bd;
		MyUtils::MyClearFunc(&bd);

		bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
		bd.ByteWidth = sizeof(VERTEX) * NUM_VERTICES;  // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

		m_device->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer

		// copy the vertices into the buffer
		D3D11_MAPPED_SUBRESOURCE ms;
		m_context->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
		memcpy(ms.pData, cubeVertices, sizeof(cubeVertices));                 // copy the data
		m_context->Unmap(pVBuffer, NULL);    // unmap the buffer

		// load and compile the two shaders
		ID3DBlob* VS = nullptr;
		ID3DBlob* PS = nullptr;
		if (!CompileD3DShader(L"..//data//shaders//shaders.shader", "VertexShaderFunction", VERTEX_SHADER_STR, &VS))
		{
			DebugConsole::Log("ERROR: FAILED TO COMPILE VERTEX SHADER!!!\n");
			return false;
		}
		if (!CompileD3DShader(L"..//data//shaders//shaders.shader", "PixelShaderFunction", PIXEL_SHADER_STR, &PS))
		{
			DebugConsole::Log("ERROR: FAILED TO COMPILE PIXEL SHADER!!!\n");
			return false;
		}

		// encapsulate both shaders into shader objects
		m_device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
		m_device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

		// set the shader objects
		m_context->VSSetShader(pVS, 0, 0);
		m_context->PSSetShader(pPS, 0, 0);

		// create the input layout object
		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		m_device->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
		m_context->IASetInputLayout(pLayout);

		// make something to describe a matrix buffer, we'll use this three times coming up
		D3D11_BUFFER_DESC constDesc;
		ZeroMemory(&constDesc, sizeof(constDesc));
		constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
		constDesc.Usage = D3D11_USAGE_DEFAULT;

		// try to create buffer for sending the model to world matrices
		if (FAILED(m_device->CreateBuffer(&constDesc, 0, &m_pModelToWorldBuffer)))
		{
			DebugConsole::Log("ERROR: FAILED TO CREATE MTW BUFFER!!!\n");
			return false;
		}

		// try to create buffer for sending world to view matrices
		if (FAILED(m_device->CreateBuffer(&constDesc, 0, &m_pWorldToViewBuffer)))
		{
			DebugConsole::Log("ERROR: FAILED TO CREATE WTV BUFFER!!!\n");
			return false;
		}

		 // try to create buffer for sending projection matrices
		if (FAILED(m_device->CreateBuffer(&constDesc, 0, &m_pProjectionBuffer)))
		{
			DebugConsole::Log("ERROR: FAILED TO CREATE PROJ BUFFER!!!\n");
			return false;
		}

		return true;
	}

	void Editor::UnloadContent()
	{
		if (pVS) pVS->Release();
		if (pPS) pPS->Release();
		if (pVBuffer) pVBuffer->Release();
		if (pLayout) pLayout->Release();
		if (m_pModelToWorldBuffer) m_pModelToWorldBuffer->Release();
		if (m_pWorldToViewBuffer) m_pWorldToViewBuffer->Release();
		if (m_pProjectionBuffer) m_pProjectionBuffer->Release();

		m_pModelToWorldBuffer = 0;
		m_pWorldToViewBuffer = 0;
		m_pProjectionBuffer = 0;
		pVS = 0;
		pPS = 0;
		pVBuffer = 0;
		pLayout = 0;
	}

	void Editor::Update(float dt)
	{
		DebugConsole::Log("Frame [%d]\n", ct);
	}

	void Editor::Render()
	{
		float nc[4]{ 0.0f, 0.0f, 0.0f, 0.0f };

		++ct %= loop;

		float t = ct <= halfLoop ? ct / halfLoopF : (loopF - ct) / halfLoopF;

		for (int i = 0; i < 4; ++i)
		{
			nc[i] = bgColorRGBA[i] * t + (1.0f - t) * otherColorRGBA[i];
		}

		// clear the back buffer to a deep blue
		m_context->ClearRenderTargetView(m_backBufferTarget, &nc[0]);
		m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		// select which vertex buffer to display
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		m_context->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

		// select which primtive type we are using
		m_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// make the mtw matrix
		DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();
		worldMat = DirectX::XMMatrixTranspose(worldMat);

		// make the wtv matrix
		DirectX::XMFLOAT3 eye(2.0f - 10.0f * t, 5.0f - 10.0f * t, 10.0f * (t - 0.5f));
		DirectX::XMFLOAT3 target(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 up(0.0f, 1.0f, 0.0f);
		DirectX::XMMATRIX viewMat = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&eye),
															  DirectX::XMLoadFloat3(&target),
															  DirectX::XMLoadFloat3(&up));
		viewMat = DirectX::XMMatrixTranspose(viewMat);

		DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 800.0f / 600.0f, 0.01f, 100.0f);
		projMatrix = DirectX::XMMatrixTranspose(projMatrix);

		// SEND MATRICES
		m_context->UpdateSubresource(m_pModelToWorldBuffer, 0, 0, &worldMat, 0, 0);
		m_context->UpdateSubresource(m_pWorldToViewBuffer, 0, 0, &viewMat, 0, 0);
		m_context->UpdateSubresource(m_pProjectionBuffer, 0, 0, &projMatrix, 0, 0);
		m_context->VSSetConstantBuffers(0, 1, &m_pModelToWorldBuffer);
		m_context->VSSetConstantBuffers(1, 1, &m_pWorldToViewBuffer);
		m_context->VSSetConstantBuffers(2, 1, &m_pProjectionBuffer);

		// draw the vertex buffer to the back buffer
		m_context->Draw(NUM_VERTICES, 0);

		// switch the back buffer and the front buffer
		m_swapChain->Present(0, 0);
	}
}
