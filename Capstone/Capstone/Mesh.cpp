#include "Mesh.h"
#include "Keyboard.h"
#include <windows.h>
#include "Variations.h"
#include "DebugConsole.h"
#include "ObjLoader.h"

namespace Capstone
{
	using namespace DirectX;

	Mesh::Mesh()
	{
		LoadMesh("..\\Data\\OBJS\\BetterDargon.obj");
		m_scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		m_rotation = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		m_translation = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		SaveLow();
		SaveHigh();
	}

	Mesh::~Mesh()
	{
		ReleaseVerts();
	}

	DirectX::XMMATRIX * Mesh::GetMTWMatrixPtr()
	{
		return &m_modelToWorld;
	}

	float * Mesh::GetVertexPointer()
	{
		return reinterpret_cast<float*>(m_pVerts);
	}

	int Mesh::GetStride()
	{
		return m_stride;
	}

	int Mesh::GetVertexCount()
	{
		return m_vertexCount;
	}

	int Mesh::GetFloatsPerVertex()
	{
		return m_floatsPerVertex;
	}

	void Mesh::CalcMatrix()
	{
		m_modelToWorld = XMMatrixScalingFromVector(m_scale) * XMMatrixRotationRollPitchYawFromVector(m_rotation) * XMMatrixTranslationFromVector(m_translation);
		m_modelToWorld = XMMatrixTranspose(m_modelToWorld);
	}

	int Mesh::GetVertexBufferSize()
	{
		return m_vertexCount * m_stride;
	}

	void Mesh::Update(float dt)
	{
		XMVECTOR f(dt * XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f));
		XMVECTOR r(dt * XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));
		XMVECTOR u(dt * XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));

		if (Keyboard::IsKeyDown('X') && Keyboard::IsKeyDown('T')) { m_translation += Keyboard::IsKeyUp(VK_SHIFT) ? r : -r; }
		if (Keyboard::IsKeyDown('Y') && Keyboard::IsKeyDown('T')) { m_translation += Keyboard::IsKeyUp(VK_SHIFT) ? u : -u; }
		if (Keyboard::IsKeyDown('Z') && Keyboard::IsKeyDown('T')) { m_translation += Keyboard::IsKeyUp(VK_SHIFT) ? f : -f; }

		if (Keyboard::IsKeyDown('X') && Keyboard::IsKeyDown('R')) { m_rotation += Keyboard::IsKeyUp(VK_SHIFT) ? r : -r; }
		if (Keyboard::IsKeyDown('Y') && Keyboard::IsKeyDown('R')) { m_rotation += Keyboard::IsKeyUp(VK_SHIFT) ? u : -u; }
		if (Keyboard::IsKeyDown('Z') && Keyboard::IsKeyDown('R')) { m_rotation += Keyboard::IsKeyUp(VK_SHIFT) ? f : -f; }

		if (Keyboard::IsKeyDown('X') && Keyboard::IsKeyDown('E')) { m_scale += Keyboard::IsKeyUp(VK_SHIFT) ? r : -r; }
		if (Keyboard::IsKeyDown('Y') && Keyboard::IsKeyDown('E')) { m_scale += Keyboard::IsKeyUp(VK_SHIFT) ? u : -u; }
		if (Keyboard::IsKeyDown('Z') && Keyboard::IsKeyDown('E')) { m_scale += Keyboard::IsKeyUp(VK_SHIFT) ? f : -f; }

		if (Keyboard::IsKeyPressed('L')) { if (Keyboard::IsKeyUp(VK_SHIFT)) { SaveLow(); } else { RestoreLow(); } }
		if (Keyboard::IsKeyPressed('H')) { if (Keyboard::IsKeyUp(VK_SHIFT)) { SaveHigh(); } else { RestoreHigh(); } }

		if (Keyboard::IsKeyPressed('V') && Keyboard::IsKeyDown('U')) { VaryVectorUniform(); }
		if (Keyboard::IsKeyPressed('V') && Keyboard::IsKeyUp('U')) { VaryVectorBellApproximation(); }
		if (Keyboard::IsKeyPressed('C') && Keyboard::IsKeyDown('U')) { VaryComponentUniform(); }
		if (Keyboard::IsKeyPressed('C') && Keyboard::IsKeyUp('U')) { VaryComponentBellApproximation(); }
		if (Keyboard::IsKeyPressed('B') && Keyboard::IsKeyDown('U')) { VarySmoothUniform(); }
		if (Keyboard::IsKeyPressed('B') && Keyboard::IsKeyUp('U')) { VarySmoothBellApproximation(); }
	}

	void Mesh::SaveLow()
	{
		DebugConsole::Log("SaveLow\n");
		m_lowScale = m_scale;
		m_lowTranslation = m_translation;
		m_lowRotation = m_rotation;
	}

	void Mesh::SaveHigh()
	{
		DebugConsole::Log("SaveHigh\n");
		m_highScale = m_scale;
		m_highTranslation = m_translation;
		m_highRotation = m_rotation;
	}

	void Mesh::VaryVectorUniform()
	{
		DebugConsole::Log("Vary Vector Uniform\n");
		m_rotation = Variations::VectorUniform(m_lowRotation, m_highRotation);
		m_translation = Variations::VectorUniform(m_lowTranslation, m_highTranslation);
		m_scale = Variations::VectorUniform(m_lowScale, m_highScale);
	}

	void Mesh::VaryVectorBellApproximation()
	{
		DebugConsole::Log("Vary Vector Bell\n");
		m_rotation = Variations::VectorBellApproximation(m_lowRotation, m_highRotation, 10);
		m_translation = Variations::VectorBellApproximation(m_lowTranslation, m_highTranslation, 10);
		m_scale = Variations::VectorBellApproximation(m_lowScale, m_highScale, 10);
	}

	void Mesh::VaryComponentUniform()
	{
		DebugConsole::Log("Vary Component Uniform\n");
		m_rotation = Variations::VectorComponentUniform(m_lowRotation, m_highRotation);
		m_translation = Variations::VectorComponentUniform(m_lowTranslation, m_highTranslation);
		m_scale = Variations::VectorComponentUniform(m_lowScale, m_highScale);
	}

	void Mesh::VaryComponentBellApproximation()
	{
		DebugConsole::Log("Vary Component Bell\n");
		m_rotation = Variations::VectorComponentBellApproximation(m_lowRotation, m_highRotation, 10);
		m_translation = Variations::VectorComponentBellApproximation(m_lowTranslation, m_highTranslation, 10);
		m_scale = Variations::VectorComponentBellApproximation(m_lowScale, m_highScale, 10);
	}

	void Mesh::VarySmoothBellApproximation()
	{
		DebugConsole::Log("Vary Smooth Bell");
		Variations::TripleVectorBellApproximation(m_lowRotation, m_highRotation, &m_rotation,
												  m_lowScale, m_highScale, &m_scale,
												  m_lowTranslation, m_highTranslation, &m_translation,
												  10);
	}

	void Mesh::VarySmoothUniform()
	{
		DebugConsole::Log("Vary Smooth Uniform");
		Variations::TripleVectorUniform(m_lowRotation, m_highRotation, &m_rotation,
										m_lowScale, m_highScale, &m_scale,
										m_lowTranslation, m_highTranslation, &m_translation);
	}

	void Mesh::RestoreLow()
	{
		DebugConsole::Log("RestoreLow\n");
		m_scale = m_lowScale;
		m_rotation = m_lowRotation;
		m_translation = m_lowTranslation;
	}

	void Mesh::RestoreHigh()
	{
		DebugConsole::Log("RestoreHigh\n");
		m_scale = m_highScale;
		m_rotation = m_highRotation;
		m_translation = m_highTranslation;
	}

	bool Mesh::LoadMesh(const char *const filePath)
	{
		float *pVerts = nullptr;
		int vertexCount = 0, stride = 0;
		if (!ObjLoader::LoadObj(filePath, &pVerts, &vertexCount, &stride)) { return false; }

		ClearObjectLevelVariations();
		ReleaseVerts();
		m_vertexCount = vertexCount;
		m_stride = stride;
		m_floatsPerVertex = m_stride / sizeof(float);

		m_pBaseVerts = pVerts;

		// copy the vertices because we're going to be working with a copy so we can revert and stuff!
		m_pVerts = new float[m_floatsPerVertex * m_vertexCount]{ 0.0f };
		std::memcpy(m_pVerts, m_pBaseVerts, m_stride * m_vertexCount);	

		return true;
	}

	void Mesh::ClearObjectLevelVariations()
	{
		m_scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		m_translation = XMVectorZero();
		m_rotation = XMVectorZero();
		SaveLow();
		SaveHigh();
	}

	void Mesh::ReleaseVerts()
	{
		if (m_pVerts) { delete[] m_pVerts; }
		if (m_pBaseVerts) { delete[] m_pBaseVerts; }
		m_pVerts = nullptr;
		m_pBaseVerts = nullptr;
	}
}