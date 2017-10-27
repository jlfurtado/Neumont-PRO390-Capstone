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
		: m_format("PCN"), m_pEditor(pEditor), m_testGroups(1 << 4)
	{
		m_objectLevelVariation.Initialize(VertexGroup::DoNothingOnPurpose, this, &m_scale, &m_rotation, &m_translation); // must come before load
		m_objectLevelVariation.ClearVariations();
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
		if (m_currentVertexGroup < 0) { m_objectLevelVariation.Update(dt); CalcMatrix(); }
		else { m_testGroups[m_currentVertexGroup].Update(dt); }

		if (Keyboard::IsKeyDown(VK_SHIFT))
		{
			if (Keyboard::IsKeyPressed('B') && Keyboard::IsKeyDown('U'))
			{ 
				m_objectLevelVariation.VarySmoothUniform();
				for (size_t i = 0; i < m_testGroups.size(); ++i)
				{
					m_testGroups[i].GetVariationPointer()->VarySmoothUniform();
				}

				UpdateAllVertexGroups(this);
			}

			if (Keyboard::IsKeyPressed('B') && Keyboard::IsKeyUp('U'))
			{
				m_objectLevelVariation.VarySmoothBellApproximation();
				for (size_t i = 0; i < m_testGroups.size(); ++i)
				{
					m_testGroups[i].GetVariationPointer()->VarySmoothBellApproximation();
				}

				UpdateAllVertexGroups(this);
			}

			if (Keyboard::IsKeyPressed('V') && Keyboard::IsKeyDown('U'))
			{
				m_objectLevelVariation.VaryVectorUniform();
				for (size_t i = 0; i < m_testGroups.size(); ++i)
				{
					m_testGroups[i].GetVariationPointer()->VaryVectorUniform();
				}

				UpdateAllVertexGroups(this);
			}

			if (Keyboard::IsKeyPressed('V') && Keyboard::IsKeyUp('U'))
			{
				m_objectLevelVariation.VaryVectorBellApproximation();
				for (size_t i = 0; i < m_testGroups.size(); ++i)
				{
					m_testGroups[i].GetVariationPointer()->VaryVectorBellApproximation();
				}

				UpdateAllVertexGroups(this);
			}

			if (Keyboard::IsKeyPressed('C') && Keyboard::IsKeyDown('U'))
			{
				m_objectLevelVariation.VaryComponentUniform();
				for (size_t i = 0; i < m_testGroups.size(); ++i)
				{
					m_testGroups[i].GetVariationPointer()->VaryComponentUniform();
				}

				UpdateAllVertexGroups(this);
			}

			if (Keyboard::IsKeyPressed('C') && Keyboard::IsKeyUp('U'))
			{
				m_objectLevelVariation.VaryComponentBellApproximation();
				for (size_t i = 0; i < m_testGroups.size(); ++i)
				{
					m_testGroups[i].GetVariationPointer()->VaryComponentBellApproximation();
				}

				UpdateAllVertexGroups(this);
			}
		}
	}

	bool Mesh::LoadMesh(const char *const filePath)
	{
		float *pVerts = nullptr;
		int vertexCount = 0, stride = 0;
		if (!ObjLoader::LoadObj(filePath, m_format, &pVerts, &vertexCount, &stride)) { return false; }

		ClearObjectLevelVariations();
		ReleaseVerts();
		ClearVertexGroups();
		m_vertexCount = vertexCount;
		m_stride = stride;
		m_floatsPerVertex = m_stride / sizeof(float);

		m_pBaseVerts = pVerts;

		// copy the vertices because we're going to be working with a copy so we can revert and stuff!
		m_pVerts = new float[m_floatsPerVertex * m_vertexCount]{ 0.0f };
		std::memcpy(m_pVerts, m_pBaseVerts, m_stride * m_vertexCount);

		ColorAll(0.0f, 0.0f, 1.0f);

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

	void Mesh::ClearVertexGroups()
	{
		for (unsigned i = 0; i < m_testGroups.size(); ++i)
		{
			m_testGroups[i].Clear();
		}

		m_testGroups.clear();
		m_currentVertexGroup = -1;
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

	void Mesh::UpdateCurrentVertexGroup(void * pMesh)
	{
		Mesh *pM = reinterpret_cast<Mesh *>(pMesh);
		if (pM->m_currentVertexGroup >= 0) { UpdateVertexGroup(pMesh, pM->m_currentVertexGroup); }
	}

	void Mesh::UpdateAllVertexGroups(void * pMesh)
	{
		Mesh *pM = reinterpret_cast<Mesh *>(pMesh);

		if (pM && pM->m_pEditor)
		{
			for (size_t currentVertexGroup = 0; currentVertexGroup < pM->m_testGroups.size(); ++currentVertexGroup)
			{
				UpdateVertexGroup(pMesh, currentVertexGroup);
			}
			
			pM->m_pEditor->ReSendVerticesSameBuffer();
		}
	}

	void Mesh::UpdateVertexGroup(void * pMesh, int groupIdx)
	{
		Mesh *pM = reinterpret_cast<Mesh *>(pMesh);

		if (pM && pM->m_pEditor && pM->m_pBaseVerts && pM->m_pVerts)
		{
			const int *pIndices = pM->m_testGroups[groupIdx].GetIndices();

			// same matrix per group
			DirectX::XMMATRIX MTW = pM->m_testGroups[groupIdx].CalcMTW();
			DirectX::XMVECTOR pivot = pM->m_testGroups[groupIdx].GetPivot();
			for (int i = 0; i < pM->m_testGroups[groupIdx].Count(); ++i)
			{
				int vertIdx = *(pIndices + i);
				int floatIdx = vertIdx * pM->m_floatsPerVertex;
				float *pBase = pM->m_pBaseVerts + floatIdx;
				float *pVert = pM->m_pVerts + floatIdx;

				XMVECTOR newVertPos = XMVector4Transform(XMVectorSet(pBase[0], pBase[1], pBase[2], 1.0f) - pivot, MTW) + pivot;
				pVert[0] = XMVectorGetX(newVertPos);
				pVert[1] = XMVectorGetY(newVertPos);
				pVert[2] = XMVectorGetZ(newVertPos);
			}

			pM->m_pEditor->ReSendVerticesSameBuffer();
		}
	}

	void Mesh::SelectVerticesInFrustum(const Frustum & frustum)
	{
		// add the test group, select it, and make sure its empty
		m_testGroups.push_back(VertexGroup());
		m_currentVertexGroup = m_testGroups.size() - 1;
		m_testGroups[m_currentVertexGroup].Clear();
		
		// IN CASE RESIZED RE-HOOK UP POINTERS!!!!
		for (size_t i = 0; i < m_testGroups.size(); ++i)
		{
			m_testGroups[i].Initialize(Mesh::UpdateCurrentVertexGroup, this);
		}

		XMMATRIX mtwT = XMMatrixTranspose(m_modelToWorld);
		for (int i = 0; i < m_vertexCount; ++i)
		{
			int idx = i * m_floatsPerVertex;
			XMVECTOR modelSpacePoint = XMVectorSet(m_pVerts[idx], m_pVerts[idx + 1], m_pVerts[idx + 2], 1.0f);
			XMVECTOR worldSpacePoint = XMVector4Transform(modelSpacePoint, mtwT);
			if (frustum.PointInFrustum(worldSpacePoint))
			{
				m_testGroups[m_currentVertexGroup].Add(i);
			}
		}

		if (m_testGroups[m_currentVertexGroup].Count() > 0)
		{
			ColorAll(1.0f, 1.0f, 1.0f);

			const int *pInd = m_testGroups[m_currentVertexGroup].GetIndices();
			for (int i = 0; i < m_testGroups[m_currentVertexGroup].Count(); ++i)
			{
				SetColor(*(pInd + i), 1.0f, 0.0f, 0.0f);
			}
		}
		else
		{
			m_testGroups.pop_back();
			m_currentVertexGroup = -1;
			ColorAll(0.0f, 0.0f, 1.0f);
		}

		m_pEditor->ReSendVerticesSameBuffer();
	}
	
	bool Mesh::SetPivotCoords(float x, float y, float z)
	{
		if (m_currentVertexGroup < 0 || (unsigned)m_currentVertexGroup >= m_testGroups.size()) { DebugConsole::Log("Cannot setPivot! No selected vertex group!\n"); return false; }
		
		m_testGroups[m_currentVertexGroup].SetPivot(XMVectorSet(x, y, z, 0.0f));
		return true;
	}

	bool Mesh::SetPivotCenter()
	{
		if (m_currentVertexGroup < 0 || (unsigned)m_currentVertexGroup >= m_testGroups.size()) { DebugConsole::Log("Cannot setPivot! No selected vertex group!\n"); return false; }

		const int *pIndices = m_testGroups[m_currentVertexGroup].GetIndices();

		XMVECTOR center = XMVectorZero();

		for (int i = 0; i < m_testGroups[m_currentVertexGroup].Count(); ++i)
		{
			int idx = pIndices[i];
			int floatIdx = idx * m_floatsPerVertex;
			center += XMVectorSet(m_pVerts[floatIdx + 0], m_pVerts[floatIdx + 1], m_pVerts[floatIdx + 2], 0.0f);
		}

		center /= (float)m_testGroups[m_currentVertexGroup].Count();
		m_testGroups[m_currentVertexGroup].SetPivot(center);

		return true;
	}
}