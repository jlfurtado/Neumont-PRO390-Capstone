#pragma once

#include <DirectXMath.h>

namespace Capstone
{

	class VariationController
	{
	public:
		typedef void(*VariationChangedCallback)();

		void Initialize(VariationChangedCallback onModified, DirectX::XMVECTOR *pScale, DirectX::XMVECTOR *pTranslation, DirectX::XMVECTOR *pRotation);
		void Update(float dt);
		void ClearVariations();


	private:
		void SaveLow();
		void SaveHigh();
		void VaryVectorUniform();
		void VaryVectorBellApproximation();
		void VaryComponentUniform();
		void VaryComponentBellApproximation();
		void VarySmoothBellApproximation();
		void VarySmoothUniform();
		void RestoreLow();
		void RestoreHigh();

		DirectX::XMVECTOR m_lowScale;
		DirectX::XMVECTOR m_lowTranslation;
		DirectX::XMVECTOR m_lowRotation;
		DirectX::XMVECTOR m_highScale;
		DirectX::XMVECTOR m_highTranslation;
		DirectX::XMVECTOR m_highRotation;
		DirectX::XMVECTOR *m_pScale{ nullptr };
		DirectX::XMVECTOR *m_pTranslation{ nullptr };
		DirectX::XMVECTOR *m_pRotation{ nullptr };
		VariationChangedCallback m_onVariationChanged{ nullptr };
	};
}


