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
		MakeMeshVertexBuffer();
		MakeUtilityVertexBuffer();

		// load and compile the two shaders
		ID3DBlob* VS1 = nullptr;
		ID3DBlob* PS1 = nullptr;
		if (!CompileD3DShader(L"..//data//shaders//pcnphong.shader", "VertexShaderFunction", VERTEX_SHADER_STR, &VS1))
		{
			DebugConsole::Log("ERROR: FAILED TO COMPILE VERTEX SHADER!!!\n");
			return false;
		}
		if (!CompileD3DShader(L"..//data//shaders//pcnphong.shader", "PixelShaderFunction", PIXEL_SHADER_STR, &PS1))
		{
			DebugConsole::Log("ERROR: FAILED TO COMPILE PIXEL SHADER!!!\n");
			return false;
		}

		ID3DBlob* VS2 = nullptr;
		ID3DBlob* PS2 = nullptr;
		if (!CompileD3DShader(L"..//data//shaders//pc.shader", "VertexShaderFunction", VERTEX_SHADER_STR, &VS2))
		{
			DebugConsole::Log("ERROR: FAILED TO COMPILE VERTEX SHADER!!!\n");
			return false;
		}
		if (!CompileD3DShader(L"..//data//shaders//pc.shader", "PixelShaderFunction", PIXEL_SHADER_STR, &PS2))
		{
			DebugConsole::Log("ERROR: FAILED TO COMPILE PIXEL SHADER!!!\n");
			return false;
		}

		// encapsulate both shaders into shader objects
		m_device->CreateVertexShader(VS1->GetBufferPointer(), VS1->GetBufferSize(), NULL, &pPCNVertShader);
		m_device->CreatePixelShader(PS1->GetBufferPointer(), PS1->GetBufferSize(), NULL, &pPCNPixelShader);

		// encapsulate both shaders into shader objects
		m_device->CreateVertexShader(VS2->GetBufferPointer(), VS2->GetBufferSize(), NULL, &pPCVertShader);
		m_device->CreatePixelShader(PS2->GetBufferPointer(), PS2->GetBufferSize(), NULL, &pPCPixelShader);

		// create the input layout object
		D3D11_INPUT_ELEMENT_DESC iedpcn[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		m_device->CreateInputLayout(iedpcn, 3, VS1->GetBufferPointer(), VS1->GetBufferSize(), &pPCNLayout);

		// create the input layout object
		D3D11_INPUT_ELEMENT_DESC iedpc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		m_device->CreateInputLayout(iedpc, 2, VS2->GetBufferPointer(), VS2->GetBufferSize(), &pPCLayout);


		// send data to our shader!!!!
		if (!m_meshUniformManager.AddUniform("mtw", sizeof(DirectX::XMFLOAT4X4), m_mesh.GetMTWMatrixPtr(), false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_meshUniformManager.AddUniform("wtv", sizeof(DirectX::XMFLOAT4X4), m_camera.GetWorldToViewMatrixPointer(), false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_meshUniformManager.AddUniform("proj", sizeof(DirectX::XMFLOAT4X4), &m_projectionMatrix, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_meshUniformManager.AddUniform("LightColor", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_meshUniformManager.AddUniform("LightPos", sizeof(DirectX::XMFLOAT4), &m_lightPos, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_meshUniformManager.AddUniform("DiffuseColor", sizeof(DirectX::XMFLOAT4), &m_diffuseColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_meshUniformManager.AddUniform("DiffuseIntensity", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_meshUniformManager.AddUniform("AmbientColor", sizeof(DirectX::XMFLOAT4), &m_ambientColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_meshUniformManager.AddUniform("AmbientIntensity", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_meshUniformManager.AddUniform("SpecularColor", sizeof(DirectX::XMFLOAT4), &m_specularColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_meshUniformManager.AddUniform("SpecularIntensity", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_meshUniformManager.AddUniform("SpecularPower", sizeof(DirectX::XMFLOAT4), &m_specularIntensity, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_meshUniformManager.AddUniform("CameraPosition", sizeof(DirectX::XMFLOAT4), m_camera.GetPositionPointer(), true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_meshUniformManager.AddUniform("ITMTW", sizeof(DirectX::XMFLOAT4X4), &m_inverseTransposeModelToWorldMatrix, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		
		float t = 0.75f;
		m_interp = XMVectorSet(t, t, t, t);
		if (!m_meshUniformManager.AddUniform("Interp", sizeof(DirectX::XMVECTOR), &m_interp, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }

		if (!m_meshUniformManager.Initialize(m_device, VS1, PS1)) { DebugConsole::Log("Failed to initialize uniform manager!\n"); return false; }

		m_identity = XMMatrixIdentity();

		// send data to our shader!!!!
		if (!m_utilUniformManager.AddUniform("mtw", sizeof(DirectX::XMFLOAT4X4), &m_identity, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_utilUniformManager.AddUniform("wtv", sizeof(DirectX::XMFLOAT4X4), &m_identity, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_utilUniformManager.AddUniform("proj", sizeof(DirectX::XMFLOAT4X4), &m_identity, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_utilUniformManager.Initialize(m_device, VS2, PS2)) { DebugConsole::Log("Failed to initialize uniform manager!\n"); return false; }

		CalculatePerspectiveMatrix();

		m_initialized = true;
		m_mesh.ClearObjectLevelVariations();
		m_mesh.ClearVertexGroups();
		
		EnableBlend();

		return true;
	}

	void Editor::UnloadContent()
	{
		m_meshUniformManager.Shutdown();
		m_utilUniformManager.Shutdown();

		if (pPCNVertShader) pPCNVertShader->Release();
		if (pPCNPixelShader) pPCNPixelShader->Release();
		if (pPCVertShader) pPCVertShader->Release();
		if (pPCPixelShader) pPCPixelShader->Release();
		if (pMeshVertexBuffer) pMeshVertexBuffer->Release();
		if (pUtilityVertexBuffer) pUtilityVertexBuffer->Release();
		if (pPCNLayout) pPCNLayout->Release();
		if (pPCLayout) pPCLayout->Release();

		pPCNVertShader = 0;
		pPCNPixelShader = 0;
		pPCVertShader = 0;
		pPCPixelShader = 0;
		pMeshVertexBuffer = 0;
		pUtilityVertexBuffer = 0;
		pPCLayout = 0;
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
		else if (m_clicked)
		{
			int mouseX = Mouse::GetMouseX();
			int mouseY = Mouse::GetMouseY();

			float lastXPercent = m_lastMouseX / (float)m_pMyWindow->GetWidth();
			float currentXPercent = mouseX / (float)m_pMyWindow->GetWidth();
			float lastYPercent = m_lastMouseY / (float)m_pMyWindow->GetHeight();
			float currentYPercent = mouseY / (float)m_pMyWindow->GetHeight();

			float lowXPerc = MathUtility::Min(lastXPercent, currentXPercent);
			float highXPerc = MathUtility::Max(lastXPercent, currentXPercent);
			float lowYPerc = MathUtility::Min(lastYPercent, currentYPercent);
			float highYPerc = MathUtility::Max(lastYPercent, currentYPercent);

			UtilFromMousePercents(lowXPerc, lowYPerc, highXPerc, highYPerc);

			if (Mouse::LeftMouseReleased())
			{
				m_clicked = false;
				Frustum cameraFrustum = Frustum(*m_camera.GetPositionPointer(), *m_camera.GetViewDirPointer(), *m_camera.GetUpPointer(),
												 m_fovy, m_nearClip, (float)m_pMyWindow->GetWidth(), (float)m_pMyWindow->GetHeight(), m_farClip);

				Frustum mouseFrustum = Frustum::GetSubFrustum(cameraFrustum, lowXPerc, lowYPerc, highXPerc, highYPerc);
				m_mesh.SelectVerticesInFrustum(mouseFrustum);
			}
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

		RenderMesh();
		if (m_clicked) { RenderUtils(); }

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
		
		MakeMeshVertexBuffer();

		return true;
	}

	void Editor::ReSendMeshVerticesSameBuffer()
	{
		ReSendVerticesSameBuffer(&pMeshVertexBuffer, m_mesh.GetVertexBufferSize(), m_mesh.GetVertexPointer(), m_mesh.GetStride());
	}

	bool Editor::SetPivotCamera()
	{
		return m_mesh.SetPivotCoords(XMVectorGetX(*m_camera.GetPositionPointer()), XMVectorGetY(*m_camera.GetPositionPointer()), XMVectorGetZ(*m_camera.GetPositionPointer()));
	}

	bool Editor::SetPivotXYZ(float x, float y, float z)
	{
		return m_mesh.SetPivotCoords(x, y, z);
	}

	bool Editor::SetPivotCenter()
	{
		return m_mesh.SetPivotCenter();
	}

	bool Editor::SetVariationType(VariationType type)
	{
		return m_mesh.SetVariationType(type);
	}

	bool Editor::WriteMeshToFile(const char * const filePath)
	{
		return m_mesh.WriteToFile(filePath);
	}

	bool Editor::ReadMeshFromFile(const char * const filePath)
	{
		if (!m_mesh.ReadFromFile(filePath)) { return false; }

		MakeMeshVertexBuffer();

		return true;
	}

	void Editor::ReSendUtilVerticesSameBuffer()
	{
		ReSendVerticesSameBuffer(&pUtilityVertexBuffer, UTIL_FLOATS * sizeof(float), &m_utilVerts[0], sizeof(float) * UTIL_FLOATS_PER_VERTEX);
	}

	void Editor::ReSendVerticesSameBuffer(ID3D11Buffer ** pBuffer, size_t bufferSize, float * pData, size_t inStride)
	{
		if (m_initialized)
		{
			UINT stride = inStride;
			UINT offset = 0;
			m_context->IASetVertexBuffers(0, 1, pBuffer, &stride, &offset);

			// copy the vertices into the buffer
			D3D11_MAPPED_SUBRESOURCE ms;
			m_context->Map(*pBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
			memcpy(ms.pData, pData, bufferSize);  // copy the mesh data
			m_context->Unmap(*pBuffer, NULL);    // unmap the buffer
		}
	}

	void Editor::RenderMesh()
	{
		// set the shader objects
		m_context->VSSetShader(pPCNVertShader, 0, 0);
		m_context->PSSetShader(pPCNPixelShader, 0, 0);

		m_context->IASetInputLayout(pPCNLayout);

		// select which vertex buffer to display
		UINT stride = m_mesh.GetStride();
		UINT offset = 0;
		m_context->IASetVertexBuffers(0, 1, &pMeshVertexBuffer, &stride, &offset);

		// select which primtive type we are using
		m_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// SEND data
		m_meshUniformManager.PassUniforms(m_context);

		// draw the vertex buffer to the back buffer
		m_context->Draw(m_mesh.GetVertexCount(), 0);
	}

	void Editor::RenderUtils()
	{
		// set the shader objects
		m_context->VSSetShader(pPCVertShader, 0, 0);
		m_context->PSSetShader(pPCPixelShader, 0, 0);

		m_context->IASetInputLayout(pPCLayout);

		// select which vertex buffer to display
		UINT stride = sizeof(float) * UTIL_FLOATS_PER_VERTEX;
		UINT offset = 0;
		m_context->IASetVertexBuffers(0, 1, &pUtilityVertexBuffer, &stride, &offset);

		// select which primtive type we are using
		m_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// SEND data
		m_utilUniformManager.PassUniforms(m_context);

		// draw the vertex buffer to the back buffer
		m_context->Draw(UTIL_VERTS, 0);
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

	void Editor::MakeMeshVertexBuffer()
	{
		MakeBuffer(&pMeshVertexBuffer, m_mesh.GetVertexBufferSize(), m_mesh.GetVertexPointer(), m_mesh.GetStride());
	}

	void Editor::MakeUtilityVertexBuffer()
	{
		MakeBuffer(&pUtilityVertexBuffer, sizeof(float) * UTIL_FLOATS, &m_utilVerts[0], sizeof(float) * UTIL_FLOATS_PER_VERTEX);
	}

	void Editor::MakeBuffer(ID3D11Buffer **pBuffer, size_t bufferSize, float * pData, size_t byteWidth)
	{
		// create the vertex buffer
		D3D11_BUFFER_DESC bd;
		MyUtils::MyClearFunc(&bd);

		bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
		bd.ByteWidth = bufferSize;  // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

		if (*pBuffer)
		{
			(*pBuffer)->Release();
			*pBuffer = nullptr;
		}

		// TODO ERROR CHECK HERE!!!
		m_device->CreateBuffer(&bd, NULL, pBuffer);       // create the buffer
		//HRESULT r2 = m_device->GetDeviceRemovedReason();

		// bind our buffer?? NEEDED or NOT?!?!
		UINT stride = byteWidth;
		UINT offset = 0;
		m_context->IASetVertexBuffers(0, 1, &pMeshVertexBuffer, &stride, &offset);

		// copy the vertices into the buffer
		D3D11_MAPPED_SUBRESOURCE ms;
		m_context->Map(*pBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // map the buffer
		memcpy(ms.pData, pData, bufferSize); // copy the data
		m_context->Unmap(*pBuffer, NULL); // unmap the buffer
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

	void Editor::UtilFromMousePercents(float lowX, float lowY, float highX, float highY)
	{
		// lerp -1 to 1 simplified
		float ndclx = 2.0f * lowX - 1.0f;
		float ndchx = 2.0f * highX - 1.0f;

		// lerp 1 to -1 simplified
		float ndcly = 1.0f -2.0f * highY;
		float ndchy = 1.0f -2.0f * lowY;
		m_utilVerts[0 * UTIL_FLOATS_PER_VERTEX + 0] = ndclx;
		m_utilVerts[0 * UTIL_FLOATS_PER_VERTEX + 1] = ndchy;
		m_utilVerts[1 * UTIL_FLOATS_PER_VERTEX + 0] = ndchx;
		m_utilVerts[1 * UTIL_FLOATS_PER_VERTEX + 1] = ndcly;
		m_utilVerts[2 * UTIL_FLOATS_PER_VERTEX + 0] = ndclx;
		m_utilVerts[2 * UTIL_FLOATS_PER_VERTEX + 1] = ndcly;
		m_utilVerts[3 * UTIL_FLOATS_PER_VERTEX + 0] = ndchx;
		m_utilVerts[3 * UTIL_FLOATS_PER_VERTEX + 1] = ndchy;
		m_utilVerts[4 * UTIL_FLOATS_PER_VERTEX + 0] = ndchx;
		m_utilVerts[4 * UTIL_FLOATS_PER_VERTEX + 1] = ndcly;
		m_utilVerts[5 * UTIL_FLOATS_PER_VERTEX + 0] = ndclx;
		m_utilVerts[5 * UTIL_FLOATS_PER_VERTEX + 1] = ndchy;
		ReSendUtilVerticesSameBuffer();
	}
}
