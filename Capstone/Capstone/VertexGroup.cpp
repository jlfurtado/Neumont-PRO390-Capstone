#include "VertexGroup.h"
#include "Mesh.h"
#include "Utils.h"

namespace Capstone
{
	using namespace DirectX;

	VertexGroup::VertexGroup(VariationController::VariationChangedCallback cb, Mesh *pMesh)
		: m_vertexIndices(1 << 12)
	{
		m_variation.Initialize(cb, pMesh, &m_scale, &m_rotation, &m_translation);
	}

	VertexGroup::~VertexGroup()
	{
	}

	void VertexGroup::Clear()
	{
		m_vertexIndices.Clear(0); // must come before!
		m_variation.ClearVariations();
	}

	int VertexGroup::Count()
	{
		return m_vertexIndices.GetCount();
	}

	void VertexGroup::Update(float dt)
	{
		m_variation.Update(dt);
	}

	void VertexGroup::Add(int idx)
	{
		if (!m_vertexIndices.Contains(idx))
		{
			m_vertexIndices.Add(idx);
		}
	}

	const int * VertexGroup::GetIndices()
	{
		return m_vertexIndices.GetDataPtr();
	}

	DirectX::XMMATRIX VertexGroup::CalcMTW()
	{
		return MyUtils::MTWFromSRT(&m_scale, &m_rotation, &m_translation);
	}


}