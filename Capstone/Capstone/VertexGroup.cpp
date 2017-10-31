#include "VertexGroup.h"
#include "Mesh.h"
#include "Utils.h"
#include "DebugConsole.h"

namespace Capstone
{
	using namespace DirectX;

	VertexGroup::VertexGroup()
		: m_vertexIndices(1 << 12)
	{
		m_variation.Initialize(VertexGroup::DoNothingOnPurpose, nullptr, &m_scale, &m_rotation, &m_translation);
		m_pivot = XMVectorZero();
		Clear();
	}

	VertexGroup::~VertexGroup()
	{
	}

	void VertexGroup::Initialize(VariationController::VariationChangedCallback cb, Mesh * pMesh)
	{
		m_variation.Initialize(cb, pMesh, &m_scale, &m_rotation, &m_translation);
	}

	void VertexGroup::Clear()
	{
		m_vertexIndices.clear(); // must come before!
		m_variation.ClearVariations();
	}

	int VertexGroup::Count()
	{
		return m_vertexIndices.size();
	}

	void VertexGroup::Update(float dt)
	{
		m_variation.Update(dt);
	}

	void VertexGroup::Add(int idx)
	{
		//if (!Contains(idx))
		//{
			m_vertexIndices.push_back(idx);
		//}
	}

	const int * VertexGroup::GetIndices()
	{
		return m_vertexIndices.data();
	}

	DirectX::XMMATRIX VertexGroup::CalcMTW()
	{
		return MyUtils::MTWFromSRT(&m_scale, &m_rotation, &m_translation);
	}

	void VertexGroup::DoNothingOnPurpose(void *)
	{
	}

	VariationController * VertexGroup::GetVariationPointer()
	{
		return &m_variation;
	}

	DirectX::XMVECTOR VertexGroup::GetPivot()
	{
		return m_pivot;
	}

	void VertexGroup::SetPivot(const DirectX::XMVECTOR & pivot)
	{
		m_pivot = pivot;
	}

	void VertexGroup::SetVariationType(VariationType type)
	{
		m_variation.SetVariationType(type);
	}

	bool VertexGroup::Contains(int idx)
	{
		for (size_t i = 0; i < m_vertexIndices.size(); ++i)
		{
			if (m_vertexIndices[i] == idx) { return true; }
		}

		return false;
	}

	void VertexGroup::LogNotSet(void *)
	{
		DebugConsole::Log("Vertex Group has unset variation callback!\n");
	}


}