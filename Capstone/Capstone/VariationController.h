#pragma once

#include <DirectXMath.h>
#include "VariationType.h"

namespace Capstone
{
	class VariationController
	{
		typedef void(*VaryCallback)(VariationController *pController);

	public:
		typedef void(*VariationChangedCallback)(void *pInstance);
		void Initialize(VariationChangedCallback onModified, void *m_pInstance, DirectX::XMVECTOR *pScale, DirectX::XMVECTOR *pRotation, DirectX::XMVECTOR *pTranslation);
		void Update(float dt);
		void ClearVariations();
		void Vary();
		void SetVariationType(VariationType type);

	private:
		static void VaryVectorUniform(VariationController *pController);
		static void VaryVectorBellApproximation(VariationController *pController);
		static void VaryComponentUniform(VariationController *pController);
		static void VaryComponentBellApproximation(VariationController *pController);
		static void VarySmoothBellApproximation(VariationController *pController);
		static void VarySmoothUniform(VariationController *pController);

		void VaryVectorUniform();
		void VaryVectorBellApproximation();
		void VaryComponentUniform();
		void VaryComponentBellApproximation();
		void VarySmoothBellApproximation();
		void VarySmoothUniform();

		void SaveLow();
		void SaveHigh();

		void RestoreLow();
		void RestoreHigh();
		void CallChanged();

		DirectX::XMVECTOR m_lowScale;
		DirectX::XMVECTOR m_lowTranslation;
		DirectX::XMVECTOR m_lowRotation;
		DirectX::XMVECTOR m_highScale;
		DirectX::XMVECTOR m_highTranslation;
		DirectX::XMVECTOR m_highRotation;
		DirectX::XMVECTOR *m_pScale{ nullptr };
		DirectX::XMVECTOR *m_pTranslation{ nullptr };
		DirectX::XMVECTOR *m_pRotation{ nullptr };
		VariationType m_variationType{ VariationType::SMOOTH_UNIFORM };

		void *m_pInstance{ nullptr };
		VariationChangedCallback m_onVariationChanged{ nullptr };

		static const int VARIATION_CALLBACKS = 6;
		static VaryCallback s_varyFuncs[VARIATION_CALLBACKS];
	};
}


