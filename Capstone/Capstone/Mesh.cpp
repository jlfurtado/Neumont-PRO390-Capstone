#include "Mesh.h"
#include "Keyboard.h"
#include <windows.h>
#include "DebugConsole.h"
#include "ObjLoader.h"

namespace Capstone
{
	using namespace DirectX;

	Mesh::Mesh()
		: m_format("PCN")
	{
		m_objectLevelVariation.Initialize(Mesh::DoNothing, &m_scale, &m_translation, &m_rotation); // must come before load
		LoadMesh("..\\Data\\OBJS\\Sphere.obj");
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
		m_objectLevelVariation.Update(dt);
	}

	bool Mesh::LoadMesh(const char *const filePath)
	{
		float *pVerts = nullptr;
		int vertexCount = 0, stride = 0;
		if (!ObjLoader::LoadObj(filePath, m_format, &pVerts, &vertexCount, &stride)) { return false; }

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
		m_objectLevelVariation.ClearVariations();
	}

	void Mesh::ReleaseVerts()
	{
		if (m_pVerts) { delete[] m_pVerts; }
		if (m_pBaseVerts) { delete[] m_pBaseVerts; }
		m_pVerts = nullptr;
		m_pBaseVerts = nullptr;
	}

	const int POSITION_FLOATS = 3;
	void Mesh::SetColor(int idx, float r, float g, float b)
	{
		float *pColor = m_pVerts + (idx * m_floatsPerVertex) + POSITION_FLOATS;
		pColor[0] = r;
		pColor[1] = g;
		pColor[2] = b;
	}

	void Mesh::DoNothing()
	{
	}

	void Mesh::UpdateSelectedColors()
	{
		for (int i = 0; i < m_vertexCount; ++i)
		{
			if (/*selected*/false) { SetColor(i, 1.0f, 0.0f, 0.0f); }
			else { SetColor(i, 1.0f, 1.0f, 1.0f); }
		}
	}
}