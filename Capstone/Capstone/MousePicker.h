#ifndef MOUSEPICKER_H
#define MOUSEPICKER_H

#include <DirectXMath.h>

namespace Capstone
{
	class  MousePicker
	{
	public:
		void static SetCameraInfo(const DirectX::XMVECTOR& cameraPosition, const DirectX::XMVECTOR& cameraViewDir, const DirectX::XMVECTOR& cameraUp);
		void static SetPerspectiveInfo(float fovyRadians, float nearClipDist, float screenWidth, float screenHeight, float farClipDist);
		DirectX::XMVECTOR static GetOrigin(int pixelX, int pixelY);
		DirectX::XMVECTOR static GetDirection(int pixelX, int pixelY);
		
	private:
		static DirectX::XMVECTOR s_cameraPosition;
		static DirectX::XMVECTOR s_viewDirection;
		static DirectX::XMVECTOR s_horizontalDirection;
		static DirectX::XMVECTOR s_verticalDirection;
		static float s_nearHorizontalLength;
		static float s_nearVerticalLength;
		static float s_farHorizontalLength;
		static float s_farVerticalLength;
		static float s_nearClipDistance;
		static float s_screenWidth;
		static float s_screenHeight;
	};
}

#endif // ifndef MOUSEPICKER_H