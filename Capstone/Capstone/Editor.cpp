#include "Editor.h"
#include "Utils.h"
#include "DebugConsole.h"
#include "EditorWindow.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "MathUtility.h"

namespace Capstone
{
	using namespace DirectX;

	Editor::Editor()
		: m_mesh(this)
	{
	}

	Editor::~Editor()
	{
	}

	bool Editor::LoadContent()
	{
		MakeVertexBuffer();

		// load and compile the two shaders
		ID3DBlob* VS = nullptr;
		ID3DBlob* PS = nullptr;
		if (!CompileD3DShader(L"..//data//shaders//pcnphong.shader", "VertexShaderFunction", VERTEX_SHADER_STR, &VS))
		{
			DebugConsole::Log("ERROR: FAILED TO COMPILE VERTEX SHADER!!!\n");
			return false;
		}
		if (!CompileD3DShader(L"..//data//shaders//pcnphong.shader", "PixelShaderFunction", PIXEL_SHADER_STR, &PS))
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
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		m_device->CreateInputLayout(ied, 3, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
		m_context->IASetInputLayout(pLayout);

		// send data to our shader!!!!
		if (!m_uniformManager.AddUniform("mtw", sizeof(DirectX::XMFLOAT4X4), m_mesh.GetMTWMatrixPtr(), false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_uniformManager.AddUniform("wtv", sizeof(DirectX::XMFLOAT4X4), m_camera.GetWorldToViewMatrixPointer(), false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_uniformManager.AddUniform("proj", sizeof(DirectX::XMFLOAT4X4), &m_projectionMatrix, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_uniformManager.AddUniform("LightColor", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_uniformManager.AddUniform("LightPos", sizeof(DirectX::XMFLOAT4), &m_lightPos, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_uniformManager.AddUniform("DiffuseColor", sizeof(DirectX::XMFLOAT4), &m_diffuseColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_uniformManager.AddUniform("DiffuseIntensity", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_uniformManager.AddUniform("AmbientColor", sizeof(DirectX::XMFLOAT4), &m_ambientColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_uniformManager.AddUniform("AmbientIntensity", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_uniformManager.AddUniform("SpecularColor", sizeof(DirectX::XMFLOAT4), &m_specularColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_uniformManager.AddUniform("SpecularIntensity", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_uniformManager.AddUniform("SpecularPower", sizeof(DirectX::XMFLOAT4), &m_specularIntensity, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_uniformManager.AddUniform("CameraPosition", sizeof(DirectX::XMFLOAT4), m_camera.GetPositionPointer(), true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_uniformManager.AddUniform("ITMTW", sizeof(DirectX::XMFLOAT4X4), &m_inverseTransposeModelToWorldMatrix, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }

		float t = 0.75f;
		m_interp = XMVectorSet(t, t, t, t);
		if (!m_uniformManager.AddUniform("Interp", sizeof(DirectX::XMVECTOR), &m_interp, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }

		if (!m_uniformManager.Initialize(m_device, VS, PS)) { DebugConsole::Log("Failed to initialize uniform manager!\n"); return false; }

		CalculatePerspectiveMatrix();

		m_initialized = true;
		m_mesh.InitTestGroup();

		return true;
	}

	void Editor::UnloadContent()
	{
		m_uniformManager.Shutdown();

		if (pVS) pVS->Release();
		if (pPS) pPS->Release();
		if (pVBuffer) pVBuffer->Release();
		if (pLayout) pLayout->Release();

		pVS = 0;
		pPS = 0;
		pVBuffer = 0;
		pLayout = 0;
		m_initialized = false;
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

		if (Keyboard::IsKeyPressed(VK_RETURN))
		{
			ExitFullScreen(); // UGLY BUG FIX
			DebugConsole::GetCommand();
		}

		if (Keyboard::IsKeyPressed('M'))
		{
			ExitFullScreen(); // UGLY BUG FIX
			DebugConsole::ToggleKeep();
		}


		if (Keyboard::IsKeyPressed(VK_ESCAPE))
		{
			m_pMyWindow->CloseWindow();
		}

		if (Mouse::LeftMouseClicked())
		{
			m_clicked = true;
			m_lastMouseX = Mouse::GetMouseX();
			m_lastMouseY = Mouse::GetMouseY();
		}
		else if (m_clicked && Mouse::LeftMouseReleased())
		{
			m_clicked = false;
			int mouseX = Mouse::GetMouseX();
			int mouseY = Mouse::GetMouseY();
			Frustum cameraFrustum = Frustum(*m_camera.GetPositionPointer(), *m_camera.GetViewDirPointer(), *m_camera.GetUpPointer(),
										    m_fovy, m_nearClip, (float)m_pMyWindow->GetWidth(), (float)m_pMyWindow->GetHeight(), m_farClip);

			float lastXPercent = m_lastMouseX / (float)m_pMyWindow->GetWidth();
			float currentXPercent = mouseX / (float)m_pMyWindow->GetWidth();
			float lastYPercent = m_lastMouseY / (float)m_pMyWindow->GetHeight();
			float currentYPercent = mouseY / (float)m_pMyWindow->GetHeight();

			float lowXPerc = MathUtility::Min(lastXPercent, currentXPercent);
			float highXPerc = MathUtility::Max(lastXPercent, currentXPercent);
			float lowYPerc = MathUtility::Min(lastYPercent, currentYPercent);
			float highYPerc = MathUtility::Max(lastYPercent, currentYPercent);

			Frustum mouseFrustum = Frustum::GetSubFrustum(cameraFrustum, lowXPerc, lowYPerc, highXPerc, highYPerc);
			m_mesh.SelectVerticesInFrustum(mouseFrustum);
		}

		m_inverseTransposeModelToWorldMatrix = XMMatrixTranspose(XMMatrixInverse(NULL, *m_mesh.GetMTWMatrixPtr()));
		m_lightPos = *m_camera.GetPositionPointer();
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
		m_uniformManager.PassUniforms(m_context);

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

	bool Editor::LoadObj(const char * const filePath)
	{
		if (!m_mesh.LoadMesh(filePath)) { return false; }
		
		MakeVertexBuffer();
		m_mesh.InitTestGroup();

		return true;
	}

	void Editor::ReSendVerticesSameBuffer()
	{
		if (m_initialized)
		{
			// copy the vertices into the buffer
			D3D11_MAPPED_SUBRESOURCE ms;
			m_context->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
			memcpy(ms.pData, m_mesh.GetVertexPointer(), m_mesh.GetVertexBufferSize());                 // copy the data
			m_context->Unmap(pVBuffer, NULL);    // unmap the buffer
		}
	}

	void Editor::ExitFullScreen()
	{
		if (m_swapChain)
		{
			BOOL isFullScreen;
			m_swapChain->GetFullscreenState(&isFullScreen, NULL);
			if (isFullScreen) { m_swapChain->SetFullscreenState(FALSE, NULL); }
		}
	}

	void Editor::MakeVertexBuffer()
	{
		// create the vertex buffer
		D3D11_BUFFER_DESC bd;
		MyUtils::MyClearFunc(&bd);

		bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
		bd.ByteWidth = m_mesh.GetVertexBufferSize();  // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

		if (pVBuffer) 
		{
			pVBuffer->Release();
			pVBuffer = nullptr;
		}

		// TODO ERROR CHECK HERE!!!
		m_device->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer
		//HRESULT r2 = m_device->GetDeviceRemovedReason();

		// copy the vertices into the buffer
		D3D11_MAPPED_SUBRESOURCE ms;
		m_context->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
		memcpy(ms.pData, m_mesh.GetVertexPointer(), m_mesh.GetVertexBufferSize());                 // copy the data
		m_context->Unmap(pVBuffer, NULL);    // unmap the buffer
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
			m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(m_fovy, (float)w / (float)h, m_nearClip, m_farClip);
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
