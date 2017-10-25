#pragma once
#include <DirectXMath.h>

namespace Capstone
{
	class Camera
	{
	public:
		Camera();
		~Camera();

		DirectX::XMMATRIX *GetWorldToViewMatrixPointer();
		DirectX::XMMATRIX GetWorldToViewMatrix();
		DirectX::XMVECTOR *GetPositionPointer();
		DirectX::XMVECTOR *GetViewDirPointer();
		DirectX::XMVECTOR *GetUpPointer();

		void Update(float dt);

	private:
		float m_speed;
		float m_rotateSpeed;
		float m_pitch;
		float m_yaw;
		DirectX::XMMATRIX m_worldToViewMatrix;
		DirectX::XMVECTOR m_position;
		DirectX::XMVECTOR m_up;
		DirectX::XMVECTOR m_forward;
	};
}
