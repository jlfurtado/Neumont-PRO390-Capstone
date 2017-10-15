#ifndef MATHUTILITY_H
#define MATHUTILITY_H

// Justin Furtado
// 10/13/2016
// MathUtility.h
// Degrees to radians, random, etc

namespace Capstone
{
	class MathUtility
	{
	public:
		static float ToRadians(float degrees);
		static float ToDegrees(float radians);
		static const float PI;
		static float Clamp(float value, float min, float max);
		static int Clamp(int value, int min, int max);
		static int Rand(int min, int max);
		static float Rand(float minValue, float maxValue);
		static float Min(float v1, float v2);
		static float Max(float v1, float v2);
	};
}

#endif // ifndef MATHUTILITY_H