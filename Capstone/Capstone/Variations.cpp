#include "Variations.h"
#include "MathUtility.h"

namespace Capstone
{
	float Variations::ScalarUniform(float min, float max)
	{
		return MathUtility::Rand(min, max);
	}
}