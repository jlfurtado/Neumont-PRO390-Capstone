#include "Mesh.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <windows.h>
#include "DebugConsole.h"
#include "ObjLoader.h"
#include "Utils.h"
#include "Editor.h"
#include "StringFuncs.h"
#include "CustomIO.h"

namespace Capstone
{
	using namespace DirectX;

	Mesh::Mesh(Editor * pEditor)
		: m_format("PCN"), m_pEditor(pEditor), m_testGroups(1 << 4)
	{
		InitObjectLevelVariaitions(); // must come before 
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
			if (Keyboard::IsKeyPressed('V'))
			{ 
				m_objectLevelVariation.Vary();
				for (size_t i = 0; i < m_testGroups.size(); ++i)
				{
					m_testGroups[i].GetVariationPointer()->Vary();
				}

				UpdateAllVertexGroups(this, 0);
			}
		}
	}

	bool Mesh::PreMultiply(int count)
	{
		m_numMeshes = count;
		ReleaseCurrentVerts();
		m_pVerts = new float[m_floatsPerVertex * m_vertexCount * count]{ 0.0f };
		return m_pVerts != nullptr;
	}

	bool Mesh::Multiply(const XMVECTOR& offset)
	{
		float xOffset = XMVectorGetX(offset);
		float yOffset = XMVectorGetY(offset);
		float zOffset = XMVectorGetZ(offset);

		for (int instanceIndex = 0; instanceIndex < m_numMeshes; ++instanceIndex)
		{
			int instanceOffset = m_floatsPerVertex * m_vertexCount * instanceIndex;

			// do the varying -- this no work?
			m_objectLevelVariation.Vary();
			CalcMatrix();
			XMMATRIX mtw = m_modelToWorld;
			for (size_t i = 0; i < m_testGroups.size(); ++i)
			{
				m_testGroups[i].GetVariationPointer()->Vary();
			}

			std::memcpy(m_pVerts + instanceOffset, m_pBaseVerts, m_stride * m_vertexCount);

			UpdateAllVertexGroups(this, instanceIndex);

			// offset the verts -- ugly would do better with fancy render engine
			for (int i = 0; i < m_vertexCount; ++i)
			{
				int floatIdx = m_floatsPerVertex * i;
				XMVECTOR v = XMVector4Transform(XMVectorSet(m_pVerts[instanceOffset + floatIdx + 0], m_pVerts[instanceOffset + floatIdx + 1], m_pVerts[instanceOffset + floatIdx + 2], 1.0f), mtw);
				m_pVerts[instanceOffset + floatIdx + 0] = XMVectorGetX(v) + instanceIndex * xOffset;
				m_pVerts[instanceOffset + floatIdx + 1] = XMVectorGetY(v) + instanceIndex * yOffset;
				m_pVerts[instanceOffset + floatIdx + 2] = XMVectorGetZ(v) + instanceIndex * zOffset;
			}

		}

		m_modelToWorld = XMMatrixIdentity();
		m_pEditor->ReSendMeshVerticesSameBuffer();
		return true;
	}

	bool Mesh::Singularify()
	{
		m_numMeshes = 1;
		ReleaseCurrentVerts();
		m_pVerts = new float[m_floatsPerVertex * m_vertexCount]{ 0.0f };
		std::memcpy(m_pVerts, m_pBaseVerts, m_stride * m_vertexCount);
		UpdateAllVertexGroups(this, 0);
		ColorAll(0.0f, 0.0f, 1.0f);
		return true;
	}

	int Mesh::GetNumMeshes()
	{
		return m_numMeshes;
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
		ReleaseCurrentVerts();
		ReleaseBaseVerts();
	}

	void Mesh::ReleaseCurrentVerts()
	{
		if (m_pVerts)
		{
			delete[] m_pVerts;
			m_pVerts = nullptr;
		}
	}

	void Mesh::ReleaseBaseVerts()
	{
		if (m_pBaseVerts) 
		{
			delete[] m_pBaseVerts;
			m_pBaseVerts = nullptr;
		}
	}

	const int POSITION_FLOATS = 3;
	void Mesh::SetColor(int idx, float r, float g, float b)
	{
		float *pColor = m_pVerts + (idx * m_floatsPerVertex) + POSITION_FLOATS;
		pColor[0] = r;
		pColor[1] = g;
		pColor[2] = b;
	}

	void Mesh::UpdateCurrentVertexGroup(void * pMesh, int instanceIdx)
	{
		Mesh *pM = reinterpret_cast<Mesh *>(pMesh);
		if (pM->m_currentVertexGroup >= 0) { UpdateVertexGroup(pMesh, pM->m_currentVertexGroup, instanceIdx); } // TODO CHECK MAGIC #!!!
	}

	void Mesh::UpdateAllVertexGroups(void * pMesh, int instanceIdx)
	{
		Mesh *pM = reinterpret_cast<Mesh *>(pMesh);

		if (pM && pM->m_pEditor)
		{
			for (size_t currentVertexGroup = 0; currentVertexGroup < pM->m_testGroups.size(); ++currentVertexGroup)
			{
				UpdateVertexGroup(pMesh, currentVertexGroup, instanceIdx);
			}
			
			pM->m_pEditor->ReSendMeshVerticesSameBuffer();
		}
	}

	const int COLOR_FLOATS = 4;
	const int TEXTURE_FLOATS = 2;
	void Mesh::UpdateVertexGroup(void * pMesh, int groupIdx, int instanceIndex)
	{
		Mesh *pM = reinterpret_cast<Mesh *>(pMesh);

		if (pM && pM->m_pEditor && pM->m_pBaseVerts && pM->m_pVerts)
		{
			if (instanceIndex < 0 || instanceIndex >= pM->m_numMeshes) { DebugConsole::Log("ERROR: Cannot UpdateVertexGroup! Invalid instanceIndex"); return; }
			const int *pIndices = pM->m_testGroups[groupIdx].GetIndices();

			// same matrix per group
			DirectX::XMMATRIX objMTW = pM->m_modelToWorld;
			//DirectX::XMMATRIX invObjMTW = XMMatrixInverse(nullptr, pM->m_modelToWorld);
			DirectX::XMMATRIX MTW = pM->m_testGroups[groupIdx].CalcMTW();
			DirectX::XMVECTOR pivot = pM->m_testGroups[groupIdx].GetPivot();
			//DirectX::XMVECTOR modelSpacePivot = XMVector4Transform(pivot, invObjMTW);
			int floatOffset = instanceIndex * pM->m_floatsPerVertex * pM->m_vertexCount;
			for (int i = 0; i < pM->m_testGroups[groupIdx].Count(); ++i)
			{
				int vertIdx = *(pIndices + i);
				int floatIdx = vertIdx * pM->m_floatsPerVertex;
				float *pBase = pM->m_pBaseVerts + floatIdx;
				float *pVert = pM->m_pVerts + floatIdx + floatOffset;

				XMVECTOR newVertPos = XMVector4Transform(XMVectorSet(pBase[0], pBase[1], pBase[2], 1.0f) - pivot, MTW) + pivot;
				pVert[0] = XMVectorGetX(newVertPos);
				pVert[1] = XMVectorGetY(newVertPos);
				pVert[2] = XMVectorGetZ(newVertPos);
			}

			int normalPos = StringFuncs::FindSubString(pM->m_format, "N");
			if (normalPos >= 0)
			{
				// handle all fomats
				int normalIdx = normalPos == 1 ? POSITION_FLOATS : (normalPos == 2 ? (POSITION_FLOATS + COLOR_FLOATS) : (POSITION_FLOATS + COLOR_FLOATS + TEXTURE_FLOATS));
				DirectX::XMMATRIX inverseTranspose = XMMatrixInverse(nullptr, XMMatrixTranspose(MTW));
				for (int i = 0; i < pM->m_testGroups[groupIdx].Count(); ++i)
				{
					int vertIdx = *(pIndices + i);
					int floatIdx = vertIdx * pM->m_floatsPerVertex + normalIdx;
					float *pBase = pM->m_pBaseVerts + floatIdx;
					float *pVert = pM->m_pVerts + floatIdx + floatOffset;

					XMVECTOR newVertPos = XMVector4Transform(XMVectorSet(pBase[0], pBase[1], pBase[2], 0.0f), inverseTranspose);
					pVert[0] = XMVectorGetX(newVertPos);
					pVert[1] = XMVectorGetY(newVertPos);	
					pVert[2] = XMVectorGetZ(newVertPos);
				}
			}

			pM->m_pEditor->ReSendMeshVerticesSameBuffer();
		}
	}

	bool Mesh::InitVariations()
	{
		return InitObjectLevelVariaitions() && InitVertexGroupVariations();
	}

	bool Mesh::InitObjectLevelVariaitions()
	{
		m_objectLevelVariation.Initialize(VertexGroup::DoNothingOnPurpose, this, &m_scale, &m_rotation, &m_translation); // must come before load
		return true;
	}

	bool Mesh::InitVertexGroupVariations()
	{
		for (size_t i = 0; i < m_testGroups.size(); ++i)
		{
			m_testGroups[i].Initialize(Mesh::UpdateCurrentVertexGroup, this);
		}

		return true;
	}

	void Mesh::SelectVerticesInFrustum(const Frustum & frustum)
	{
		// add the test group, select it, and make sure its empty
		m_testGroups.push_back(VertexGroup());
		m_currentVertexGroup = m_testGroups.size() - 1;
		m_testGroups[m_currentVertexGroup].Clear();
		
		// IN CASE RESIZED RE-HOOK UP POINTERS!!!!
		InitVertexGroupVariations();

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
			ColorCurrentGroup();
		}
		else
		{
			m_testGroups.pop_back();
			m_currentVertexGroup = -1;
			ColorAll(0.0f, 0.0f, 1.0f);
		}

		m_pEditor->ReSendMeshVerticesSameBuffer();
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
			center += XMVectorSet(m_pVerts[floatIdx + 0], m_pVerts[floatIdx + 1], m_pVerts[floatIdx + 2], 1.0f);
		}

		center /= (float)m_testGroups[m_currentVertexGroup].Count();
		center = XMVectorSetW(center, 0.0f);
		m_testGroups[m_currentVertexGroup].SetPivot(center);

		return true;
	}

	bool Mesh::SetVariationType(VariationType type)
	{
		return (m_currentVertexGroup >= 0 && (unsigned)m_currentVertexGroup < m_testGroups.size()) ? SetVariationTypeForCurrentGroup(type) : SetObjectLevelVariationType(type);
	}

	bool Mesh::WriteToFile(const char * const filePath)
	{
		return CustomIO::WriteMeshToFile(filePath, m_pBaseVerts, m_vertexCount, m_floatsPerVertex, &m_testGroups, &m_objectLevelVariation);
	}

	bool Mesh::ReadFromFile(const char * const filePath)
	{
		float *pVerts = nullptr;
		int vertexCount = 0, floatsPerVertex = 0;
		std::vector<VertexGroup> outGroups;
		VariationController outVariation;
		if (!CustomIO::ReadMeshFromFile(filePath, &pVerts, &vertexCount, &floatsPerVertex, &outGroups, &outVariation)) { return false; }

		ClearObjectLevelVariations();
		ReleaseVerts();
		ClearVertexGroups();
		m_vertexCount = vertexCount;
		m_floatsPerVertex = floatsPerVertex;
		m_stride = floatsPerVertex * sizeof(float);

		m_pBaseVerts = pVerts;

		// copy the vertices because we're going to be working with a copy so we can revert and stuff!
		m_pVerts = new float[m_floatsPerVertex * m_vertexCount]{ 0.0f };
		std::memcpy(m_pVerts, m_pBaseVerts, m_stride * m_vertexCount);

		m_testGroups = outGroups;
		m_objectLevelVariation = outVariation;
		InitVariations();

		ColorAll(0.0f, 0.0f, 1.0f);
		return true;
	}

	XMMATRIX Mesh::GetPivotTranslation()
	{
		if (m_currentVertexGroup < 0 || (unsigned)m_currentVertexGroup >= m_testGroups.size()) { return XMMatrixIdentity(); }
		return XMMatrixTranspose(XMMatrixTranslationFromVector(XMVector4Transform(m_testGroups[m_currentVertexGroup].GetPivot(), XMMatrixTranspose(m_modelToWorld))));
	}

	unsigned Mesh::GetNumVertexGroups()
	{
		return m_testGroups.size();
	}

	bool Mesh::SelectVertexGroup(int idx)
	{
		if (idx < 0 || (unsigned)idx > m_testGroups.size()) { return false; }

		m_currentVertexGroup = idx;
		ColorMesh();
		m_pEditor->ReSendMeshVerticesSameBuffer();
		return true;
	}

	bool Mesh::GetVertexGroupInfo(int group, unsigned & outNumVerts)
	{
		if (group < 0 || (unsigned)group >= m_testGroups.size()) { DebugConsole::Log("Cannot GetCurrentVertexGroupInfo! Invalid group [%d]!\n", group); return false; }

		outNumVerts = m_testGroups[group].GetNumVertices();
		return true;
	}

	bool Mesh::RemoveVertexGroup(int idx)
	{
		if (idx < 0 || (unsigned)idx >= m_testGroups.size()) { DebugConsole::Log("Cannot RemoveVertexGroup! Invalid group [%d]!\n", idx); return false; }

		m_currentVertexGroup = idx; // HAX
		m_testGroups[idx].Clear();
		m_currentVertexGroup = -1;
		m_testGroups.erase(m_testGroups.begin() + idx, m_testGroups.begin() + idx + 1);
		// IN CASE RESIZED RE-HOOK UP POINTERS!!!!
		InitVertexGroupVariations();
		ColorMesh();
		m_pEditor->ReSendMeshVerticesSameBuffer();

		return true;
	}

	void Mesh::ColorCurrentGroup()
	{
		ColorAll(1.0f, 1.0f, 1.0f);

		const int *pInd = m_testGroups[m_currentVertexGroup].GetIndices();
		for (int i = 0; i < m_testGroups[m_currentVertexGroup].Count(); ++i)
		{
			SetColor(*(pInd + i), 1.0f, 0.0f, 0.0f);
		}
	}

	void Mesh::ColorMesh()
	{
		if (m_currentVertexGroup >= 0) { ColorCurrentGroup(); }
		else { ColorWholeMesh(); }
	}

	void Mesh::ColorWholeMesh()
	{
		ColorAll(0.0f, 0.0f, 1.0f);
	}

	bool Mesh::SetVariationTypeForCurrentGroup(VariationType type)
	{
		if (m_currentVertexGroup < 0 || (unsigned)m_currentVertexGroup >= m_testGroups.size()) { DebugConsole::Log("Cannot SetVariationType! No selected vertex group!\n"); return false; }

		m_testGroups[m_currentVertexGroup].SetVariationType(type);
		return true;
	}

	bool Mesh::SetObjectLevelVariationType(VariationType type)
	{
		m_objectLevelVariation.SetVariationType(type);
		return true;
	}
}