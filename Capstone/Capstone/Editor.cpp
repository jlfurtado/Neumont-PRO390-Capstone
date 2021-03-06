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
		if (!MakeMeshVertexBuffer(1)) { return false; }
		if (!MakeUtilityVertexBuffer()) { return false; }
		CalcNormalsFor(m_pivotVerts, PIVOT_VERTS, PIVOT_FLOATS_PER_VERTEX, PIVOT_FLOATS_PER_VERTEX - 3);
		if (!MakePivotVertexBuffer()) { return false; }
		if (!MakeFrustumVertexBuffer()) { return false; }

		// pcnphong | pctnphongnotex
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

		ID3DBlob* VS3 = nullptr;
		ID3DBlob* PS3 = nullptr;
		if (!CompileD3DShader(L"..//data//shaders//pctnphongnotex.shader", "VertexShaderFunction", VERTEX_SHADER_STR, &VS3))
		{
			DebugConsole::Log("ERROR: FAILED TO COMPILE VERTEX SHADER!!!\n");
			return false;
		}
		if (!CompileD3DShader(L"..//data//shaders//pctnphongnotex.shader", "PixelShaderFunction", PIXEL_SHADER_STR, &PS3))
		{
			DebugConsole::Log("ERROR: FAILED TO COMPILE PIXEL SHADER!!!\n");
			return false;
		}

		// encapsulate both shaders into shader objects
		HRESULT r = m_device->CreateVertexShader(VS1->GetBufferPointer(), VS1->GetBufferSize(), NULL, &pPCNVertShader);
		r = m_device->CreatePixelShader(PS1->GetBufferPointer(), PS1->GetBufferSize(), NULL, &pPCNPixelShader);

		// encapsulate both shaders into shader objects
		r = m_device->CreateVertexShader(VS2->GetBufferPointer(), VS2->GetBufferSize(), NULL, &pPCVertShader);
		r = m_device->CreatePixelShader(PS2->GetBufferPointer(), PS2->GetBufferSize(), NULL, &pPCPixelShader);

		// encapsulate both shaders into shader objects
		r = m_device->CreateVertexShader(VS3->GetBufferPointer(), VS3->GetBufferSize(), NULL, &pPCTNVertShader);
		r = m_device->CreatePixelShader(PS3->GetBufferPointer(), PS3->GetBufferSize(), NULL, &pPCTNPixelShader);

		// create the input layout object
		D3D11_INPUT_ELEMENT_DESC iedpcn[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		r = m_device->CreateInputLayout(iedpcn, 3, VS1->GetBufferPointer(), VS1->GetBufferSize(), &pPCNLayout);


		// create the input layout object
		D3D11_INPUT_ELEMENT_DESC iedpctn[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		r = m_device->CreateInputLayout(iedpctn, 4, VS3->GetBufferPointer(), VS3->GetBufferSize(), &pPCTNLayout);

		// create the input layout object
		D3D11_INPUT_ELEMENT_DESC iedpc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		r = m_device->CreateInputLayout(iedpc, 2, VS2->GetBufferPointer(), VS2->GetBufferSize(), &pPCLayout);

		float t = 0.75f;
		m_interp = XMVectorSet(t, t, t, t);

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
		if (!m_meshUniformManager.AddUniform("Interp", sizeof(DirectX::XMVECTOR), &m_interp, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }

		if (!m_meshUniformManager.Initialize(m_device, VS3, PS3)) { DebugConsole::Log("Failed to initialize uniform manager!\n"); return false; }

		m_identity = XMMatrixIdentity();

		// send data to our shader!!!!
		if (!m_utilUniformManager.AddUniform("mtw", sizeof(DirectX::XMFLOAT4X4), &m_identity, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_utilUniformManager.AddUniform("wtv", sizeof(DirectX::XMFLOAT4X4), &m_identity, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_utilUniformManager.AddUniform("proj", sizeof(DirectX::XMFLOAT4X4), &m_identity, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_utilUniformManager.Initialize(m_device, VS2, PS2)) { DebugConsole::Log("Failed to initialize uniform manager!\n"); return false; }


		// send data to our shader!!!!
		if (!m_pivotUniformManager.AddUniform("mtw", sizeof(DirectX::XMFLOAT4X4), &m_pivotMTW, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_pivotUniformManager.AddUniform("wtv", sizeof(DirectX::XMFLOAT4X4), m_camera.GetWorldToViewMatrixPointer(), false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_pivotUniformManager.AddUniform("proj", sizeof(DirectX::XMFLOAT4X4), &m_projectionMatrix, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_pivotUniformManager.AddUniform("LightColor", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_pivotUniformManager.AddUniform("LightPos", sizeof(DirectX::XMFLOAT4), &m_lightPos, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_pivotUniformManager.AddUniform("DiffuseColor", sizeof(DirectX::XMFLOAT4), &m_diffuseColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_pivotUniformManager.AddUniform("DiffuseIntensity", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_pivotUniformManager.AddUniform("AmbientColor", sizeof(DirectX::XMFLOAT4), &m_ambientColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_pivotUniformManager.AddUniform("AmbientIntensity", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_pivotUniformManager.AddUniform("SpecularColor", sizeof(DirectX::XMFLOAT4), &m_specularColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_pivotUniformManager.AddUniform("SpecularIntensity", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_pivotUniformManager.AddUniform("SpecularPower", sizeof(DirectX::XMFLOAT4), &m_specularIntensity, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_pivotUniformManager.AddUniform("CameraPosition", sizeof(DirectX::XMFLOAT4), m_camera.GetPositionPointer(), true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_pivotUniformManager.AddUniform("ITMTW", sizeof(DirectX::XMFLOAT4X4), &m_itmtwPivot, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_pivotUniformManager.AddUniform("Interp", sizeof(DirectX::XMVECTOR), &m_interp, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }

		if (!m_pivotUniformManager.Initialize(m_device, VS1, PS1)) { DebugConsole::Log("Failed to initialize uniform manager!\n"); return false; }

		// send data to our shader!!!!
		if (!m_frustumUniformManager.AddUniform("mtw", sizeof(DirectX::XMFLOAT4X4), &m_identity, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_frustumUniformManager.AddUniform("wtv", sizeof(DirectX::XMFLOAT4X4), m_camera.GetWorldToViewMatrixPointer(), false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_frustumUniformManager.AddUniform("proj", sizeof(DirectX::XMFLOAT4X4), &m_projectionMatrix, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_frustumUniformManager.AddUniform("LightColor", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_frustumUniformManager.AddUniform("LightPos", sizeof(DirectX::XMFLOAT4), &m_lightPos, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_frustumUniformManager.AddUniform("DiffuseColor", sizeof(DirectX::XMFLOAT4), &m_diffuseColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_frustumUniformManager.AddUniform("DiffuseIntensity", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_frustumUniformManager.AddUniform("AmbientColor", sizeof(DirectX::XMFLOAT4), &m_ambientColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_frustumUniformManager.AddUniform("AmbientIntensity", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_frustumUniformManager.AddUniform("SpecularColor", sizeof(DirectX::XMFLOAT4), &m_specularColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_frustumUniformManager.AddUniform("SpecularIntensity", sizeof(DirectX::XMFLOAT4), &m_lightColor, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_frustumUniformManager.AddUniform("SpecularPower", sizeof(DirectX::XMFLOAT4), &m_specularIntensity, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_frustumUniformManager.AddUniform("CameraPosition", sizeof(DirectX::XMFLOAT4), m_camera.GetPositionPointer(), true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_frustumUniformManager.AddUniform("ITMTW", sizeof(DirectX::XMFLOAT4X4), &m_identity, false)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }
		if (!m_frustumUniformManager.AddUniform("Interp", sizeof(DirectX::XMVECTOR), &m_interp, true)) { DebugConsole::Log("Failed to AddUniform!\n"); return false; }

		if (!m_frustumUniformManager.Initialize(m_device, VS1, PS1)) { DebugConsole::Log("Failed to initialize uniform manager!\n"); return false; }


		CalculatePerspectiveMatrix();

		m_initialized = true;
		m_mesh.ClearObjectLevelVariations();
		m_mesh.ClearVertexGroups();
		
		EnableBlend();

		return true;
	}

	void Editor::UnloadContent()
	{
		m_frustumUniformManager.Shutdown();
		m_meshUniformManager.Shutdown();
		m_utilUniformManager.Shutdown();
		m_pivotUniformManager.Shutdown();

		SafeRelease(pPCNVertShader);
		SafeRelease(pPCNPixelShader);
		SafeRelease(pPCVertShader);
		SafeRelease(pPCPixelShader);
		SafeRelease(pPCTNVertShader);
		SafeRelease(pPCTNPixelShader);
		SafeRelease(pFrustumVertexBuffer);
		SafeRelease(pMeshVertexBuffer);
		SafeRelease(pUtilityVertexBuffer);
		SafeRelease(pPivotVertexBuffer);
		SafeRelease(pPCNLayout);
		SafeRelease(pPCTNLayout);
		SafeRelease(pPCLayout);

		m_initialized = false;
	}

	void Editor::Update(float dt)
	{
		m_camera.Update(dt);

		LogFPS(dt, 1.0f);

		if (!m_displayMode)
		{
			m_mesh.Update(dt);
			HandleVertexSelection();
		}

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

		m_pivotMTW = m_mesh.GetPivotTranslation() * XMMatrixScaling(0.25f, 0.25f, 0.25f);
		m_inverseTransposeModelToWorldMatrix = XMMatrixTranspose(XMMatrixInverse(NULL, *m_mesh.GetMTWMatrixPtr()));
		m_itmtwPivot = XMMatrixTranspose(XMMatrixInverse(NULL, m_pivotMTW));
		m_lightPos = *m_camera.GetPositionPointer();
	}

	void Editor::Render()
	{
		m_context->ClearRenderTargetView(m_backBufferTarget, &bgColorRGBA[0]);
		m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		RenderMesh();
		RenderPivot();
		if (m_clicked) { RenderUtils(); }
		if (m_displayFrustum) { RenderFrustum(); }

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
		if (!CheckValidMode("LoadObj")) { return false; }
		if (!m_mesh.LoadMeshOBJ(filePath)) { return false; }
		if (!MakeMeshVertexBuffer(1)) { DebugConsole::Log("Failed to LoadObj! Failed to MakeMeshVertexBuffer!\n"); return false; }
		return true;
	}

	void Editor::ReSendMeshVerticesSameBuffer()
	{
		ReSendVerticesSameBuffer(&pMeshVertexBuffer, m_mesh.GetVertexBufferSize() * m_mesh.GetNumMeshes(), m_mesh.GetVertexPointer(), m_mesh.GetStride());
	}

	bool Editor::SetPivotCamera()
	{
		if (!CheckValidMode("SetPivotCamera")) { return false; }
		return m_mesh.SetPivotCoords(XMVectorGetX(*m_camera.GetPositionPointer()), XMVectorGetY(*m_camera.GetPositionPointer()), XMVectorGetZ(*m_camera.GetPositionPointer()));
	}

	bool Editor::SetPivotXYZ(float x, float y, float z)
	{
		if (!CheckValidMode("SetPivotXYZ")) { return false; }
		return m_mesh.SetPivotCoords(x, y, z);
	}

	bool Editor::SetPivotCenter()
	{
		if (!CheckValidMode("SetPivotCenter")) { return false; }
		return m_mesh.SetPivotCenter();
	}

	bool Editor::SetVariationType(VariationType type)
	{
		if (!CheckValidMode("SetVariationType")) { return false; }
		return m_mesh.SetVariationType(type);
	}

	bool Editor::WriteMeshToFile(const char * const filePath)
	{
		return m_mesh.WriteToFile(filePath);
	}

	bool Editor::ReadMeshFromFile(const char * const filePath)
	{
		if (!CheckValidMode("ReadMeshFromFile")) { return false; }
		if (!m_mesh.ReadFromFile(filePath)) { return false; }
		if (!MakeMeshVertexBuffer(1)) { DebugConsole::Log("Failed to ReadMeshFromFile! Failed to MakeMeshVertexBuffer!\n"); return false; }
		return true;
	}

	bool Editor::SetCameraSpeed(float speed)
	{
		m_camera.SetSpeed(speed);
		return true;
	}

	bool Editor::SetCameraRotateSpeed(float speed)
	{
		m_camera.SetRotateSpeed(speed);
		return true;
	}

	unsigned Editor::GetVertexGroupCount()
	{
		return m_mesh.GetNumVertexGroups();
	}

	bool Editor::SelectVertexGroup(int idx)
	{
		if (!CheckValidMode("SelectVertexGroup")) { return false; }
		return m_mesh.SelectVertexGroup(idx);
	}

	bool Editor::GetVertexGroupInfo(int group, unsigned & outSize)
	{
		return m_mesh.GetVertexGroupInfo(group, outSize);
	}

	bool Editor::RemoveVertexGroup(int idx)
	{
		if (!CheckValidMode("RemoveVertexGroup")) { return false; }
		return m_mesh.RemoveVertexGroup(idx);
	}

	bool Editor::EnterDisplayMode2D(int displayCount1, int displayCount2, const DirectX::XMVECTOR & offset1, const DirectX::XMVECTOR & offset2)
	{
		if (displayCount1 < 1) { DebugConsole::Log("MINIMUM OF 1 DISPLAY VARIANT!!!\n"); return false; }
		if (displayCount2 < 1) { DebugConsole::Log("MINIMUM OF 1 DISPLAY VARIANT!!!\n"); return false; }

		m_clicked = false;
		m_displayMode = true;

		if (!m_mesh.PreMultiply(displayCount1 * displayCount2)) { DebugConsole::Log("Failed to Pre-Multiply Mesh!"); return false; }
		if (!MakeMeshVertexBuffer(displayCount1 *displayCount2)) { DebugConsole::Log("Failed to EnterDisplayMode2D! Failed to MakeMeshVertexBuffer!\n"); return false; }
		if (!m_mesh.Multiply2D(displayCount1, displayCount2, offset1, offset2))
		{
			DebugConsole::Log("Failed to multiply2D mesh!\n");
			ExitDisplayMode();
			return false;
		}

		return true;
	}

	bool Editor::EnterDisplayMode(int displayCount, const DirectX::XMVECTOR& offset)
	{
		if (displayCount < 1) { DebugConsole::Log("MINIMUM OF 1 DISPLAY VARIANT!!!\n"); return false; }
		m_clicked = false;
		m_displayMode = true;

		if (!m_mesh.PreMultiply(displayCount)) { DebugConsole::Log("Failed to Pre-Multiply Mesh!"); return false; }
		if (!MakeMeshVertexBuffer(displayCount)) { DebugConsole::Log("Failed to EnterDisplayMode! Failed to MakeMeshVertexBuffer!\n"); return false; }
		if (!m_mesh.Multiply(offset))
		{
			DebugConsole::Log("Failed to multiply mesh!\n");
			ExitDisplayMode();
			return false;
		}

		return true;
	}

	bool Editor::ExitDisplayMode()
	{
		if (!m_displayMode) { DebugConsole::Log("Cannot ExitDisplayMode! Not in display mode!\n"); return false; }

		m_displayMode = false;
		if (!m_mesh.Singularify()) { DebugConsole::Log("Failed to Singularify mesh!\n"); return false; }
		if (!MakeMeshVertexBuffer(1)) { DebugConsole::Log("Failed to ExitDisplayMode! Failed to MakeMeshVertexBuffer!\n"); return false; }

		return true;
	}

	bool Editor::CheckValidMode(const char *const action)
	{
		if (m_displayMode && !ExitDisplayMode()) { DebugConsole::Log("Cannot [%s]! Failed to exit display mode first!\n", action); return false; }
		return true;
	}

	bool Editor::ClearVertexGroups()
	{
		if (m_mesh.GetNumVertexGroups() <= 0) { DebugConsole::Log("Cannot ClearVertexGroups! No vertex groups exist to clear!\n"); return false; }
		m_mesh.ClearVertexGroups();
		m_mesh.ColorAll(0.0f, 0.0f, 1.0f);
		ReSendMeshVerticesSameBuffer();
		return true;
	}

	bool Editor::ExportCurrentMeshObj(const char * const filePath)
	{
		return m_mesh.ExportCurrentMeshOBJ(filePath);
	}

	const int MAX_EXPORT = 100;
	bool Editor::ExportVariedMeshesObj(int count, const char * const filePath)
	{
		if (count < 1) { DebugConsole::Log("Cannot ExportVariedMeshesObj! Minimum of one mesh to export!"); return false; }
		if (count > MAX_EXPORT) { DebugConsole::Log("Cannot ExportVariedMeshesObj! Maximum of [%d] meshes can be exported!\n", MAX_EXPORT); return false; }
		return m_mesh.ExportVariedMeshesOBJ(count, filePath);
	}

	void Editor::ReSendUtilVerticesSameBuffer()
	{
		ReSendVerticesSameBuffer(&pUtilityVertexBuffer, UTIL_FLOATS * sizeof(float), &m_utilVerts[0], sizeof(float) * UTIL_FLOATS_PER_VERTEX);
	}

	void Editor::ReSendFrustumVerticesSameBuffer()
	{
		ReSendVerticesSameBuffer(&pFrustumVertexBuffer, FRUSTUM_FLOATS * sizeof(float), &m_frustumVerts[0], FRUSTUM_FLOATS_PER_VERTEX * sizeof(float));
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
		m_context->VSSetShader(pPCTNVertShader, 0, 0);
		m_context->PSSetShader(pPCTNPixelShader, 0, 0);

		m_context->IASetInputLayout(pPCTNLayout);

		// select which vertex buffer to display
		UINT stride = m_mesh.GetStride();
		UINT offset = 0;
		m_context->IASetVertexBuffers(0, 1, &pMeshVertexBuffer, &stride, &offset);

		// select which primtive type we are using
		m_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// SEND data
		m_meshUniformManager.PassUniforms(m_context);

		// draw the vertex buffer to the back buffer
		m_context->Draw(m_mesh.GetVertexCount() * m_mesh.GetNumMeshes(), 0);
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

	void Editor::RenderPivot()
	{
		// set the shader objects
		m_context->VSSetShader(pPCNVertShader, 0, 0);
		m_context->PSSetShader(pPCNPixelShader, 0, 0);

		m_context->IASetInputLayout(pPCNLayout);

		// select which vertex buffer to display
		UINT stride = PIVOT_FLOATS_PER_VERTEX * sizeof(float);
		UINT offset = 0;
		m_context->IASetVertexBuffers(0, 1, &pPivotVertexBuffer, &stride, &offset);

		// select which primtive type we are using
		m_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// SEND data
		m_pivotUniformManager.PassUniforms(m_context);

		// draw the vertex buffer to the back buffer
		m_context->Draw(PIVOT_VERTS, 0);
	}

	void Editor::RenderFrustum()
	{
		// set the shader objects
		m_context->VSSetShader(pPCNVertShader, 0, 0);
		m_context->PSSetShader(pPCNPixelShader, 0, 0);

		m_context->IASetInputLayout(pPCNLayout);

		// select which vertex buffer to display
		UINT stride = sizeof(float) * FRUSTUM_FLOATS_PER_VERTEX;
		UINT offset = 0;
		m_context->IASetVertexBuffers(0, 1, &pFrustumVertexBuffer, &stride, &offset);

		// select which primtive type we are using
		m_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// SEND data
		m_frustumUniformManager.PassUniforms(m_context);

		// draw the vertex buffer to the back buffer
		m_context->Draw(FRUSTUM_VERTS, 0);
	}

	bool Editor::MakeFrustumVertexBuffer()
	{
		return MakeBuffer(&pFrustumVertexBuffer, FRUSTUM_FLOATS * sizeof(float), &m_frustumVerts[0], FRUSTUM_FLOATS_PER_VERTEX * sizeof(float));
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

	bool Editor::MakeMeshVertexBuffer(int count)
	{
		return MakeBuffer(&pMeshVertexBuffer, m_mesh.GetVertexBufferSize() * count, m_mesh.GetVertexPointer(), m_mesh.GetStride());
	}

	bool Editor::MakeUtilityVertexBuffer()
	{
		return MakeBuffer(&pUtilityVertexBuffer, sizeof(float) * UTIL_FLOATS, &m_utilVerts[0], sizeof(float) * UTIL_FLOATS_PER_VERTEX);
	}

	bool Editor::MakePivotVertexBuffer()
	{
		return MakeBuffer(&pPivotVertexBuffer, sizeof(float) * PIVOT_FLOATS, &m_pivotVerts[0], sizeof(float) * PIVOT_FLOATS_PER_VERTEX);
	}

	bool Editor::MakeBuffer(ID3D11Buffer **pBuffer, size_t bufferSize, float * pData, size_t byteWidth)
	{
		// create the vertex buffer
		D3D11_BUFFER_DESC bd;
		MyUtils::MyClearFunc(&bd);

		bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
		bd.ByteWidth = bufferSize;  // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

		// create the buffer
		ID3D11Buffer *pOld = *pBuffer;
		if (FAILED(m_device->CreateBuffer(&bd, NULL, pBuffer)))
		{
			//m_device->GetDeviceRemovedReason();
			DebugConsole::Log("Failed to CreateBuffer! Cannot MakeBuffer!\n");
			return false;
		}
		
		// release the old if creating the new did not fail!
		SafeRelease(pOld);

		// bind our buffer?? NEEDED or NOT?!?!
		UINT stride = byteWidth;
		UINT offset = 0;
		m_context->IASetVertexBuffers(0, 1, &pMeshVertexBuffer, &stride, &offset);

		// copy the vertices into the buffer
		D3D11_MAPPED_SUBRESOURCE ms;
		m_context->Map(*pBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // map the buffer
		memcpy(ms.pData, pData, bufferSize); // copy the data
		m_context->Unmap(*pBuffer, NULL); // unmap the buffer
		return true;
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

	void Editor::FrustumVertsFromFrustum(const Frustum & frustum)
	{
		XMVECTOR ntl = frustum.GetPoint(Frustum::NEAR_TOP_LEFT);
		XMVECTOR ntr = frustum.GetPoint(Frustum::NEAR_TOP_RIGHT);
		XMVECTOR nbr = frustum.GetPoint(Frustum::NEAR_BOTTOM_RIGHT);
		XMVECTOR nbl = frustum.GetPoint(Frustum::NEAR_BOTTOM_LEFT);
		XMVECTOR ftl = frustum.GetPoint(Frustum::FAR_TOP_LEFT);
		XMVECTOR ftr = frustum.GetPoint(Frustum::FAR_TOP_RIGHT);
		XMVECTOR fbr = frustum.GetPoint(Frustum::FAR_BOTTOM_RIGHT);
		XMVECTOR fbl = frustum.GetPoint(Frustum::FAR_BOTTOM_LEFT);

		// near
		m_frustumVerts[0 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ntl);
		m_frustumVerts[0 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ntl);
		m_frustumVerts[0 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ntl);
		m_frustumVerts[1 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(nbr);
		m_frustumVerts[1 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(nbr);
		m_frustumVerts[1 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(nbr);
		m_frustumVerts[2 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(nbl);
		m_frustumVerts[2 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(nbl);
		m_frustumVerts[2 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(nbl);
		m_frustumVerts[3 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ntl);
		m_frustumVerts[3 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ntl);
		m_frustumVerts[3 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ntl);
		m_frustumVerts[4 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ntr);
		m_frustumVerts[4 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ntr);
		m_frustumVerts[4 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ntr);
		m_frustumVerts[5 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(nbr);
		m_frustumVerts[5 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(nbr);
		m_frustumVerts[5 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(nbr);

		// far
		m_frustumVerts[6 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ftl);
		m_frustumVerts[6 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ftl);
		m_frustumVerts[6 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ftl);
		m_frustumVerts[7 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(fbl);
		m_frustumVerts[7 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(fbl);
		m_frustumVerts[7 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(fbl);
		m_frustumVerts[8 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(fbr);
		m_frustumVerts[8 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(fbr);
		m_frustumVerts[8 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(fbr);
		m_frustumVerts[9 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ftl);
		m_frustumVerts[9 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ftl);
		m_frustumVerts[9 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ftl);
		m_frustumVerts[10 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(fbr);
		m_frustumVerts[10 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(fbr);
		m_frustumVerts[10 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(fbr);
		m_frustumVerts[11 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ftr);
		m_frustumVerts[11 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ftr);
		m_frustumVerts[11 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ftr);

		// top
		m_frustumVerts[12 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ftl);
		m_frustumVerts[12 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ftl);
		m_frustumVerts[12 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ftl);
		m_frustumVerts[13 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ntr);
		m_frustumVerts[13 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ntr);
		m_frustumVerts[13 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ntr);
		m_frustumVerts[14 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ntl);
		m_frustumVerts[14 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ntl);
		m_frustumVerts[14 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ntl);
		m_frustumVerts[15 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ftl);
		m_frustumVerts[15 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ftl);
		m_frustumVerts[15 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ftl);
		m_frustumVerts[16 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ftr);
		m_frustumVerts[16 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ftr);
		m_frustumVerts[16 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ftr);
		m_frustumVerts[17 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ntr);
		m_frustumVerts[17 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ntr);
		m_frustumVerts[17 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ntr);

		// bottom
		m_frustumVerts[18 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(fbl);
		m_frustumVerts[18 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(fbl);
		m_frustumVerts[18 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(fbl);
		m_frustumVerts[19 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(nbl);
		m_frustumVerts[19 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(nbl);
		m_frustumVerts[19 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(nbl);
		m_frustumVerts[20 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(nbr);
		m_frustumVerts[20 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(nbr);
		m_frustumVerts[20 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(nbr);
		m_frustumVerts[21 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(fbl);
		m_frustumVerts[21 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(fbl);
		m_frustumVerts[21 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(fbl);
		m_frustumVerts[22 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(nbr);
		m_frustumVerts[22 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(nbr);
		m_frustumVerts[22 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(nbr);
		m_frustumVerts[23 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(fbr);
		m_frustumVerts[23 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(fbr);
		m_frustumVerts[23 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(fbr);

		// left
		m_frustumVerts[24 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ftl);
		m_frustumVerts[24 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ftl);
		m_frustumVerts[24 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ftl);
		m_frustumVerts[25 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(nbl);
		m_frustumVerts[25 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(nbl);
		m_frustumVerts[25 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(nbl);
		m_frustumVerts[26 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(fbl);
		m_frustumVerts[26 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(fbl);
		m_frustumVerts[26 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(fbl);
		m_frustumVerts[27 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ftl);
		m_frustumVerts[27 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ftl);
		m_frustumVerts[27 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ftl);
		m_frustumVerts[28 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ntl);
		m_frustumVerts[28 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ntl);
		m_frustumVerts[28 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ntl);
		m_frustumVerts[29 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(nbl);
		m_frustumVerts[29 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(nbl);
		m_frustumVerts[29 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(nbl);

		// right
		m_frustumVerts[30 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ntr);
		m_frustumVerts[30 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ntr);
		m_frustumVerts[30 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ntr);
		m_frustumVerts[31 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(fbr);
		m_frustumVerts[31 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(fbr);
		m_frustumVerts[31 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(fbr);
		m_frustumVerts[32 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(nbr);
		m_frustumVerts[32 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(nbr);
		m_frustumVerts[32 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(nbr);
		m_frustumVerts[33 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ntr);
		m_frustumVerts[33 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ntr);
		m_frustumVerts[33 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ntr);
		m_frustumVerts[34 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(ftr);
		m_frustumVerts[34 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(ftr);
		m_frustumVerts[34 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(ftr);
		m_frustumVerts[35 * FRUSTUM_FLOATS_PER_VERTEX + 0] = XMVectorGetX(fbr);
		m_frustumVerts[35 * FRUSTUM_FLOATS_PER_VERTEX + 1] = XMVectorGetY(fbr);
		m_frustumVerts[35 * FRUSTUM_FLOATS_PER_VERTEX + 2] = XMVectorGetZ(fbr);

		CalcNormalsFor(&m_frustumVerts[0], FRUSTUM_VERTS, FRUSTUM_FLOATS_PER_VERTEX, FRUSTUM_FLOATS_PER_VERTEX - 3);
		ReSendFrustumVerticesSameBuffer();
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

	const int VERTS_PER_TRIANGLE = 3;
	void Editor::CalcNormalsFor(float * pVerts, int numVerts, int stride, int normalOffset)
	{
		for (int i = 0; i < numVerts; i += VERTS_PER_TRIANGLE)
		{
			float *pVert1 = pVerts + i * stride;
			float *pNorm1 = pVert1 + normalOffset;
			float *pVert2 = pVert1 + stride;
			float *pNorm2 = pNorm1 + stride;
			float *pVert3 = pVert2 + stride;
			float *pNorm3 = pNorm2 + stride;

			XMVECTOR p1 = XMVectorSet(pVert1[0], pVert1[1], pVert1[2], 0.0f);
			XMVECTOR p2 = XMVectorSet(pVert2[0], pVert2[1], pVert2[2], 0.0f);
			XMVECTOR p3 = XMVectorSet(pVert3[0], pVert3[1], pVert3[2], 0.0f);

			XMVECTOR normal = XMVector3Normalize(XMVector3Cross(p2 - p1, p3 - p2));

			float x = XMVectorGetX(normal);
			float y = XMVectorGetY(normal);
			float z = XMVectorGetZ(normal);

			pNorm1[0] = x;
			pNorm1[1] = y;
			pNorm1[2] = z;

			pNorm2[0] = x;
			pNorm2[1] = y;
			pNorm2[2] = z;

			pNorm3[0] = x;
			pNorm3[1] = y;
			pNorm3[2] = z;
		
		}
	}

	void Editor::HandleVertexSelection()
	{
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
				
				if (Keyboard::IsKeyDown(VK_SHIFT))
				{
					m_mesh.AddVerticesInFrustum(mouseFrustum);
				}
				else if (Keyboard::IsKeyDown(VK_CONTROL))
				{
					m_mesh.RemoveVerticesInFrustum(mouseFrustum);
				}
				else
				{
					m_mesh.SelectVerticesInFrustum(mouseFrustum);
				}

				if (m_displayFrustum) { FrustumVertsFromFrustum(mouseFrustum); }
			}
		}
	}

	void Editor::ToggleDebugFrustum()
	{
		m_displayFrustum = !m_displayFrustum;
	}
}
