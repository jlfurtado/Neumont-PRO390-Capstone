#include "VariationController.h"
#include "Keyboard.h"
#include "Variations.h"
#include <windows.h>
#include "DebugConsole.h"

namespace Capstone
{
	using namespace DirectX;
	
	const int ASSUME_SINGLE_INSTANCE = 0;

	float VariationController::s_variationSpeed = 1.0f;
	VariationController::VaryCallback VariationController::s_varyFuncs[VARIATION_CALLBACKS] = {
		VariationController::VaryComponentUniform,
		VariationController::VaryVectorUniform,
		VariationController::VarySmoothUniform,
		VariationController::VaryComponentBellApproximation,
		VariationController::VaryVectorBellApproximation,
		VariationController::VarySmoothBellApproximation
	};

	void VariationController::Initialize(VariationChangedCallback onModified, void *pInstance, DirectX::XMVECTOR * pScale, DirectX::XMVECTOR * pRotation, DirectX::XMVECTOR * pTranslation)
	{
		m_pScale = pScale;
		m_pRotation = pRotation;
		m_pTranslation = pTranslation;
		m_onVariationChanged = onModified;
		m_pInstance = pInstance;
	}

	void VariationController::Update(float dt)
	{
		float move = dt * s_variationSpeed;
		XMVECTOR f(move * XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f));
		XMVECTOR r(move * XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));
		XMVECTOR u(move * XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
		
		if (Keyboard::IsKeyDown('X') && Keyboard::IsKeyDown('T')) { *m_pTranslation += Keyboard::IsKeyUp(VK_SHIFT) ? r : -r; CallChanged(ASSUME_SINGLE_INSTANCE); }
		if (Keyboard::IsKeyDown('Y') && Keyboard::IsKeyDown('T')) { *m_pTranslation += Keyboard::IsKeyUp(VK_SHIFT) ? u : -u; CallChanged(ASSUME_SINGLE_INSTANCE); }
		if (Keyboard::IsKeyDown('Z') && Keyboard::IsKeyDown('T')) { *m_pTranslation += Keyboard::IsKeyUp(VK_SHIFT) ? f : -f; CallChanged(ASSUME_SINGLE_INSTANCE); }

		if (Keyboard::IsKeyDown('X') && Keyboard::IsKeyDown('R')) { *m_pRotation += Keyboard::IsKeyUp(VK_SHIFT) ? r : -r; CallChanged(ASSUME_SINGLE_INSTANCE); }
		if (Keyboard::IsKeyDown('Y') && Keyboard::IsKeyDown('R')) { *m_pRotation += Keyboard::IsKeyUp(VK_SHIFT) ? u : -u; CallChanged(ASSUME_SINGLE_INSTANCE); }
		if (Keyboard::IsKeyDown('Z') && Keyboard::IsKeyDown('R')) { *m_pRotation += Keyboard::IsKeyUp(VK_SHIFT) ? f : -f; CallChanged(ASSUME_SINGLE_INSTANCE); }

		if (Keyboard::IsKeyDown('X') && Keyboard::IsKeyDown('E')) { *m_pScale += Keyboard::IsKeyUp(VK_SHIFT) ? r : -r; CallChanged(ASSUME_SINGLE_INSTANCE); }
		if (Keyboard::IsKeyDown('Y') && Keyboard::IsKeyDown('E')) { *m_pScale += Keyboard::IsKeyUp(VK_SHIFT) ? u : -u; CallChanged(ASSUME_SINGLE_INSTANCE); }
		if (Keyboard::IsKeyDown('Z') && Keyboard::IsKeyDown('E')) { *m_pScale += Keyboard::IsKeyUp(VK_SHIFT) ? f : -f; CallChanged(ASSUME_SINGLE_INSTANCE); }

		if (Keyboard::IsKeyPressed('L')) { if (Keyboard::IsKeyUp(VK_SHIFT)) { SaveLow(); } else { RestoreLow(); } }
		if (Keyboard::IsKeyPressed('H')) { if (Keyboard::IsKeyUp(VK_SHIFT)) { SaveHigh(); } else { RestoreHigh(); } }
		
		if (Keyboard::IsKeyUp(VK_SHIFT))
		{
			if (Keyboard::IsKeyPressed('V')) { Vary(); CallChanged(ASSUME_SINGLE_INSTANCE); }
		}
	}

	void VariationController::SaveLow()
	{
		m_lowScale = *m_pScale;
		m_lowTranslation = *m_pTranslation;
		m_lowRotation = *m_pRotation;
	}

	void VariationController::SaveHigh()
	{
		m_highScale = *m_pScale;
		m_highTranslation = *m_pTranslation;
		m_highRotation = *m_pRotation;
	}

	void VariationController::VaryVectorUniform()
	{
		*m_pRotation = Variations::VectorUniform(m_lowRotation, m_highRotation);
		*m_pTranslation = Variations::VectorUniform(m_lowTranslation, m_highTranslation);
		*m_pScale = Variations::VectorUniform(m_lowScale, m_highScale);
	}

	void VariationController::VaryVectorBellApproximation()
	{
		*m_pRotation = Variations::VectorBellApproximation(m_lowRotation, m_highRotation, 10);
		*m_pTranslation = Variations::VectorBellApproximation(m_lowTranslation, m_highTranslation, 10);
		*m_pScale = Variations::VectorBellApproximation(m_lowScale, m_highScale, 10);
	}

	void VariationController::VaryComponentUniform()
	{
		*m_pRotation = Variations::VectorComponentUniform(m_lowRotation, m_highRotation);
		*m_pTranslation = Variations::VectorComponentUniform(m_lowTranslation, m_highTranslation);
		*m_pScale = Variations::VectorComponentUniform(m_lowScale, m_highScale);
	}

	void VariationController::VaryComponentBellApproximation()
	{
		*m_pRotation = Variations::VectorComponentBellApproximation(m_lowRotation, m_highRotation, 10);
		*m_pTranslation = Variations::VectorComponentBellApproximation(m_lowTranslation, m_highTranslation, 10);
		*m_pScale = Variations::VectorComponentBellApproximation(m_lowScale, m_highScale, 10);
	}

	void VariationController::VarySmoothBellApproximation()
	{
		Variations::TripleVectorBellApproximation(m_lowRotation, m_highRotation, m_pRotation,
												  m_lowScale, m_highScale, m_pScale,
												  m_lowTranslation, m_highTranslation, m_pTranslation,
												  10);
	}

	void VariationController::VarySmoothUniform()
	{
		Variations::TripleVectorUniform(m_lowRotation, m_highRotation, m_pRotation,
										m_lowScale, m_highScale, m_pScale,
										m_lowTranslation, m_highTranslation, m_pTranslation);
	}

	void VariationController::RestoreLow()
	{
		*m_pScale = m_lowScale;
		*m_pRotation = m_lowRotation;
		*m_pTranslation = m_lowTranslation;
		CallChanged(ASSUME_SINGLE_INSTANCE);
	}

	void VariationController::RestoreHigh()
	{
		*m_pScale = m_highScale;
		*m_pRotation = m_highRotation;
		*m_pTranslation = m_highTranslation;
		CallChanged(ASSUME_SINGLE_INSTANCE);
	}

	void VariationController::CallChanged(int instanceIdx)
	{
		m_onVariationChanged(m_pInstance, instanceIdx);
	}

	void VariationController::ClearVariations()
	{
		*m_pScale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		*m_pRotation = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		*m_pTranslation = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		SaveLow();
		SaveHigh();
		CallChanged(ASSUME_SINGLE_INSTANCE);
	}

	void VariationController::Vary()
	{
		s_varyFuncs[static_cast<int>(m_variationType)](this);
	}

	void VariationController::SetVariationType(VariationType type)
	{
		m_variationType = type;
	}

	bool VariationController::SetVariationSpeed(float speed)
	{
		if (speed < 0.0f) { return false; }

		s_variationSpeed = speed;
		return true;
	}

	void VariationController::VaryVectorUniform(VariationController * pController)
	{
		pController->VaryVectorUniform();
	}

	void VariationController::VaryVectorBellApproximation(VariationController * pController)
	{
		pController->VaryVectorBellApproximation();
	}

	void VariationController::VaryComponentUniform(VariationController * pController)
	{
		pController->VaryComponentUniform();
	}

	void VariationController::VaryComponentBellApproximation(VariationController * pController)
	{
		pController->VaryComponentBellApproximation();
	}

	void VariationController::VarySmoothBellApproximation(VariationController * pController)
	{
		pController->VarySmoothBellApproximation();
	}

	void VariationController::VarySmoothUniform(VariationController * pController)
	{
		pController->VarySmoothUniform();
	}
}