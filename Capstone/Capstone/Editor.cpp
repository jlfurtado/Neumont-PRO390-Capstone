#include "Editor.h"
#include "Utils.h"
#include "DebugConsole.h"
#include "EditorWindow.h"
#include "Keyboard.h"
#include "Mouse.h"

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
		bd.ByteWidth = m_mesh.GetVertexBufferSize();  // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

		m_device->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer

		// copy the vertices into the buffer
		D3D11_MAPPED_SUBRESOURCE ms;
		m_context->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
		memcpy(ms.pData, m_mesh.GetVertexPointer(), m_mesh.GetVertexBufferSize());                 // copy the data
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

		CalculatePerspectiveMatrix();

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
		m_camera.Update(dt);

		//int w, h;
		//m_pMyWindow->GetWindowSize(w, h);
		//DebugConsole::Log("Frame [%d]\n", ct);
		LogFPS(dt, 1.0f);
		m_timer += dt; if (m_timer > m_loopTime) { m_timer -= m_loopTime; }

		//if (Keyboard::IsKeyPressed('W')) { DebugConsole::Log("W pressed\n"); }
		//if (Keyboard::IsKeyReleased('W')) { DebugConsole::Log("W released\n"); }

		m_mesh.Update(dt);
		m_mesh.CalcMatrix();

		m_t = m_timer <= m_halfLoopTime ? m_timer / m_halfLoopTime : (m_loopTime - m_timer) / m_halfLoopTime;
		//DebugConsole::Log("Size: [%d, %d]\n", w, h);
	}

	void Editor::Render()
	{
		float nc[4]{ 0.0f, 0.0f, 0.0f, 0.0f };

		for (int i = 0; i < 4; ++i)
		{
			nc[i] = bgColorRGBA[i] * m_t + (1.0f - m_t) * otherColorRGBA[i];
		}

		// clear the back buffer to a deep blue
		m_context->ClearRenderTargetView(m_backBufferTarget, &nc[0]);
		m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		// select which vertex buffer to display
		UINT stride = m_mesh.GetStride();
		UINT offset = 0;
		m_context->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

		// select which primtive type we are using
		m_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		// SEND MATRICES
		m_context->UpdateSubresource(m_pModelToWorldBuffer, 0, 0, m_mesh.GetMTWMatrixPtr(), 0, 0);
		m_context->UpdateSubresource(m_pWorldToViewBuffer, 0, 0, m_camera.GetWorldToViewMatrixPointer(), 0, 0);
		m_context->UpdateSubresource(m_pProjectionBuffer, 0, 0, &m_projectionMatrix, 0, 0);
		m_context->VSSetConstantBuffers(0, 1, &m_pModelToWorldBuffer);
		m_context->VSSetConstantBuffers(1, 1, &m_pWorldToViewBuffer);
		m_context->VSSetConstantBuffers(2, 1, &m_pProjectionBuffer);

		// draw the vertex buffer to the back buffer
		m_context->Draw(m_mesh.GetVertexCount(), 0);

		// switch the back buffer and the front buffer
		m_swapChain->Present(0, 0);
	}

	void Editor::OnWindowResize()
	{
		CalculatePerspectiveMatrix();
	}

	void Editor::MouseScroll(int /*degrees*/)
	{

	}

	void Editor::OnMouseScroll(int degrees, void * pInstance)
	{
		if (pInstance)
		{
			Editor *pEditor = reinterpret_cast<Editor *>(pInstance);
			pEditor->MouseScroll(degrees);
		}
	}

	void Editor::CalculatePerspectiveMatrix()
	{
		if (m_pMyWindow)
		{
			int w = m_pMyWindow->GetWidth(), h = m_pMyWindow->GetHeight();
			m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, (float)w / (float)h, 0.01f, 100.0f);
			m_projectionMatrix = DirectX::XMMatrixTranspose(m_projectionMatrix);
		}
	}

	void Editor::LogFPS(float dt, float fpsInterval)
	{
		static unsigned int numFrames = 0;
		static float    timeAccumulator = 0.0f;

		++numFrames;
		timeAccumulator += dt;

		if (timeAccumulator >= fpsInterval)
		{
			//Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "FPS: %6.1f over %3.1f seconds.\n", numFrames / timeAccumulator, m_fpsInterval);
			//char fpsText[50];
			DebugConsole::Log("FPS: %6.1f over %3.1f seconds.\n", numFrames / timeAccumulator, fpsInterval);
			//m_fpsTextObject.SetupText(-0.9f, 0.9f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, fpsText);
			timeAccumulator = 0;
			numFrames = 0;
		}
	}
}
