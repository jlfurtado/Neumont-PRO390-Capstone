#include "MathUtility.h"
#include "MathUtility.h"
#include <random>

// Justin Furtado
// 10/13/2016
// MathUtility.h
// Degrees to radians, random, etc

namespace Capstone
{
	const float MathUtility::PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148f;

	float MathUtility::ToRadians(float degrees)
	{
		return degrees / 180.0f * PI;
	}

	float MathUtility::ToDegrees(float radians)
	{
		return radians / PI * 180.0f;
	}

	float MathUtility::Clamp(float value, float min, float max)
	{
		float result = value;
		if (result < min) { result = min; }
		else if (result > max) { result = max; }
		return result;
	}

	int MathUtility::Clamp(int value, int min, int max)
	{
		int result = value;
		if (result < min) { result = min; }
		else if (result > max) { result = max; }
		return result;
	}

	int MathUtility::Rand(int min, int max)
	{
		return (rand() % (max - min)) + min;
	}

	float MathUtility::Rand(float minValue, float maxValue)
	{
		return (rand() * (maxValue - minValue) / RAND_MAX)  + minValue;
	}

	float MathUtility::Min(float v1, float v2)
	{
		return v1 < v2 ? v1 : v2;
	}

	float MathUtility::Max(float v1, float v2)
	{
		return v1 > v2 ? v1 : v2;
	}
	

}