#include "VertexGroup.h"

namespace Capstone
{
	using namespace DirectX;

	VertexGroup::VertexGroup()
		: m_vertexIndices(1 << 16)
	{
	}

	VertexGroup::~VertexGroup()
	{
	}

	void VertexGroup::Clear()
	{
		m_vertexIndices.Clear(0);
	}

	void VertexGroup::Update(float dt)
	{
	}


}