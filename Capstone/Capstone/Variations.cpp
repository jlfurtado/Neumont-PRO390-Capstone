#include "Variations.h"
#include "MathUtility.h"

namespace Capstone
{
	using namespace DirectX;

	float Variations::ScalarUniform(float v1, float v2)
	{
		return MathUtility::Rand(MathUtility::Min(v1, v2), MathUtility::Max(v1, v2));
	}

	XMVECTOR Variations::VectorUniform(const XMVECTOR & v1, const XMVECTOR & v2)
	{
		XMFLOAT4 f1, f2;
		XMStoreFloat4(&f1, v1);
		XMStoreFloat4(&f2, v2);
		return XMVectorSet(ScalarUniform(f1.x, f2.x),
						   ScalarUniform(f1.y, f2.y),
						   ScalarUniform(f1.z, f2.z),
						   ScalarUniform(f1.w, f2.w));
	}
}