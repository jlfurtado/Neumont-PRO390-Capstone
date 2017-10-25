#include "Mesh.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <windows.h>
#include "DebugConsole.h"
#include "ObjLoader.h"
#include "Utils.h"
#include "Editor.h"

namespace Capstone
{
	using namespace DirectX;

	Mesh::Mesh(Editor * pEditor)
		: m_format("PCN"), m_testGroup(Mesh::UpdateTestGroup, this), m_pEditor(pEditor)
	{
		m_objectLevelVariation.Initialize(Mesh::DoNothing, this, &m_scale, &m_rotation, &m_translation); // must come before load
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
		m_modelToWorld = DirectX::XMMatrixTranspose(MyUtils::MTWFromSRT(&m_scale, &m_rotation, &m_translation));
	}

	int Mesh::GetVertexBufferSize()
	{
		return m_vertexCount * m_stride;
	}

	void Mesh::Update(float dt)
	{
		//m_objectLevelVariation.Update(dt);
		m_testGroup.Update(dt);

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
			DebugConsole::Log("(%d, %d, %d, %d)\n", m_lastMouseX, m_lastMouseY, mouseX, mouseY);
		}
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

	void Mesh::ColorAll(float r, float g, float b)
	{
		for (int i = 0; i < m_vertexCount; ++i)
		{
			SetColor(i, r, g, b);
		}
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

	void Mesh::DoNothing(void *)
	{
	}

	void Mesh::UpdateTestGroup(void * pMesh)
	{
		Mesh *pM = reinterpret_cast<Mesh *>(pMesh);

		if (pM && pM->m_pEditor)
		{
			const int *pIndices = pM->m_testGroup.GetIndices();

			// same matrix per group
			DirectX::XMMATRIX MTW = pM->m_testGroup.CalcMTW();
			for (int i = 0; i < pM->m_testGroup.Count(); ++i)
			{
				int vertIdx = *(pIndices + i);
				int floatIdx = vertIdx * pM->m_floatsPerVertex;
				float *pBase = pM->m_pBaseVerts + floatIdx;
				float *pVert = pM->m_pVerts + floatIdx;

				XMVECTOR newVertPos = XMVector4Transform(XMVectorSet(pBase[0], pBase[1], pBase[2], 1.0f), MTW);
				pVert[0] = XMVectorGetX(newVertPos);
				pVert[1] = XMVectorGetY(newVertPos);
				pVert[2] = XMVectorGetZ(newVertPos);
			}

			pM->m_pEditor->ReSendVerticesSameBuffer();
		}
	}

	void Mesh::InitTestGroup()
	{
		m_testGroup.Clear();

		for (int i = 0; i < m_vertexCount * 1 / 3; ++i)
		{
			m_testGroup.Add(i);
		}

		ColorAll(1.0f, 1.0f, 1.0f);

		const int *pInd = m_testGroup.GetIndices();
		for (int i = 0; i < m_testGroup.Count(); ++i)
		{
			SetColor(*(pInd + i), 1.0f, 0.0f, 0.0f);
		}
		
		m_pEditor->ReSendVerticesSameBuffer();
	}
}