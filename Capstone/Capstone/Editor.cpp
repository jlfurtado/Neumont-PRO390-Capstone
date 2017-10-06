#include "Editor.h"
#include "Utils.h"
#include "DebugConsole.h"

namespace Capstone
{
	struct VERTEX
	{
		FLOAT X, Y, Z;      // position
		FLOAT r, g, b, a;   // color
	};

	VERTEX OurVertices[] =
	{
		/*   X      Y      Z     R     G     B     A */
		{ +0.0f, +0.5f, +0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ +0.5f, -0.5f, +0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ -0.5f, -0.5f, +0.0f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ +0.5f, +1.0f, +0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ +1.0f, +0.0f, +0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ +0.0f, +0.0f, +0.0f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ -0.5f, +0.0f, +0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ +0.0f, -1.0f, +0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ -1.0f, -1.0f, +0.0f, 0.0f, 0.0f, 1.0f, 1.0f }
	};

	const int NUM_VERTICES = sizeof(OurVertices) / sizeof(VERTEX);

	float bgColorRGBA[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float otherColorRGBA[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	int ct = 0;

	const int loop = 10000;
	const float loopF = (float)loop;
	const int halfLoop = loop / 2;
	const float halfLoopF = loopF / 2.0f;
	ID3D11VertexShader *pVS;    // the vertex shader
	ID3D11PixelShader *pPS;     // the pixel shader
	ID3D11Buffer *pVBuffer;    // vertex buffer
	ID3D11InputLayout *pLayout;    // global
	const char *const VERTEX_SHADER_STR = "vs_5_0";
	const char *const PIXEL_SHADER_STR = "ps_5_0";

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
		memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
		m_context->Unmap(pVBuffer, NULL);    // unmap the buffer

		// load and compile the two shaders
		ID3DBlob* VS = nullptr;
		ID3DBlob* PS = nullptr;
		if (!CompileD3DShader(L"..//data//shaders//shaders.shader", "VertexShaderFunction", VERTEX_SHADER_STR, &VS))
		{
			return false;
		}
		if (!CompileD3DShader(L"..//data//shaders//shaders.shader", "PixelShaderFunction", PIXEL_SHADER_STR, &PS))
		{
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
		return true;
	}

	void Editor::UnloadContent()
	{
		pVS->Release();
		pPS->Release();
	}

	void Editor::Update(float dt)
	{
	}

	void Editor::Render()
	{
		float nc[4]{ 0.0f, 0.0f, 0.0f, 0.0f };

		++ct %= loop;
		DebugConsole::Log("Frame\n");

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

		// draw the vertex buffer to the back buffer
		m_context->Draw(NUM_VERTICES, 0);

		// switch the back buffer and the front buffer
		m_swapChain->Present(0, 0);
	}
}
