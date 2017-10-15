#include "Mesh.h"
#include "Keyboard.h"
#include <windows.h>

namespace Capstone
{
	using namespace DirectX;

	Mesh::Mesh()
	{
		m_scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		m_rotation = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		m_translation = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
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
		return reinterpret_cast<float*>(&m_cubeVertices[0]);
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

	}
}