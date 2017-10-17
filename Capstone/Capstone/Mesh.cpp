#include "Mesh.h"
#include "Keyboard.h"
#include <windows.h>
#include "Variations.h"
#include "DebugConsole.h"

namespace Capstone
{
	using namespace DirectX;

	Mesh::Mesh()
	{
		m_scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		m_rotation = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		m_translation = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		SaveLow();
		SaveHigh();
	}

	Mesh::~Mesh()
	{
	}

	DirectX::XMMATRIX * Mesh::GetMTWMatrixPtr()
	{
		return &m_modelToWorld;
	}

	float * Mesh::GetVertexPointer()
	{
		return reinterpret_cast<float*>(pVerts);
	}

	int Mesh::GetStride()
	{
		return sizeof(VERTEX);
	}

	int Mesh::GetVertexCount()
	{
		return NUM_VERTICES;
	}

	void Mesh::CalcMatrix()
	{
		m_modelToWorld = XMMatrixScalingFromVector(m_scale) * XMMatrixRotationRollPitchYawFromVector(m_rotation) * XMMatrixTranslationFromVector(m_translation);
		m_modelToWorld = XMMatrixTranspose(m_modelToWorld);
	}

	int Mesh::GetVertexBufferSize()
	{
		return sizeof(VERTEX) * NUM_VERTICES;
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

		if (Keyboard::IsKeyPressed('V') && Keyboard::IsKeyDown('U')) { VaryUniform(); }
		if (Keyboard::IsKeyPressed('V') && Keyboard::IsKeyUp('U')) { VaryBellApproximation(); }
		if (Keyboard::IsKeyPressed('C') && Keyboard::IsKeyDown('U')) { VaryComponentUniform(); }
		if (Keyboard::IsKeyPressed('C') && Keyboard::IsKeyUp('U')) { VaryComponentBellApproximation(); }

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

	void Mesh::VaryUniform()
	{
		DebugConsole::Log("Vary Vector Uniform\n");
		m_rotation = Variations::VectorUniform(m_lowRotation, m_highRotation);
		m_translation = Variations::VectorUniform(m_lowTranslation, m_highTranslation);
		m_scale = Variations::VectorUniform(m_lowScale, m_highScale);
	}

	void Mesh::VaryBellApproximation()
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

	void Mesh::RandomizeColors()
	{
		for (int i = 0; i < NUM_VERTICES; ++i)
		{
			float *pColors = reinterpret_cast<float*>(pVerts + i) + 3; // move i verts and 3 floats in
			pColors[0] = Variations::ScalarUniform(0.0f, 1.0f);
			pColors[1] = Variations::ScalarUniform(0.0f, 1.0f);
			pColors[2] = Variations::ScalarUniform(0.0f, 1.0f);
		}
	}
}