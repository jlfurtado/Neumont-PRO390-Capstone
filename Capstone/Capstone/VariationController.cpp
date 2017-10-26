#include "VariationController.h"
#include "Keyboard.h"
#include "Variations.h"
#include <windows.h>
#include "DebugConsole.h"

namespace Capstone
{
	using namespace DirectX;

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
		XMVECTOR f(dt * XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f));
		XMVECTOR r(dt * XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));
		XMVECTOR u(dt * XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
		
		if (Keyboard::IsKeyDown('X') && Keyboard::IsKeyDown('T')) { *m_pTranslation += Keyboard::IsKeyUp(VK_SHIFT) ? r : -r; CallChanged(); }
		if (Keyboard::IsKeyDown('Y') && Keyboard::IsKeyDown('T')) { *m_pTranslation += Keyboard::IsKeyUp(VK_SHIFT) ? u : -u; CallChanged(); }
		if (Keyboard::IsKeyDown('Z') && Keyboard::IsKeyDown('T')) { *m_pTranslation += Keyboard::IsKeyUp(VK_SHIFT) ? f : -f; CallChanged(); }

		if (Keyboard::IsKeyDown('X') && Keyboard::IsKeyDown('R')) { *m_pRotation += Keyboard::IsKeyUp(VK_SHIFT) ? r : -r; CallChanged(); }
		if (Keyboard::IsKeyDown('Y') && Keyboard::IsKeyDown('R')) { *m_pRotation += Keyboard::IsKeyUp(VK_SHIFT) ? u : -u; CallChanged(); }
		if (Keyboard::IsKeyDown('Z') && Keyboard::IsKeyDown('R')) { *m_pRotation += Keyboard::IsKeyUp(VK_SHIFT) ? f : -f; CallChanged(); }

		if (Keyboard::IsKeyDown('X') && Keyboard::IsKeyDown('E')) { *m_pScale += Keyboard::IsKeyUp(VK_SHIFT) ? r : -r; CallChanged(); }
		if (Keyboard::IsKeyDown('Y') && Keyboard::IsKeyDown('E')) { *m_pScale += Keyboard::IsKeyUp(VK_SHIFT) ? u : -u; CallChanged(); }
		if (Keyboard::IsKeyDown('Z') && Keyboard::IsKeyDown('E')) { *m_pScale += Keyboard::IsKeyUp(VK_SHIFT) ? f : -f; CallChanged(); }

		if (Keyboard::IsKeyPressed('L')) { if (Keyboard::IsKeyUp(VK_SHIFT)) { SaveLow(); } else { RestoreLow(); } }
		if (Keyboard::IsKeyPressed('H')) { if (Keyboard::IsKeyUp(VK_SHIFT)) { SaveHigh(); } else { RestoreHigh(); } }
		
		if (Keyboard::IsKeyUp(VK_SHIFT))
		{
			if (Keyboard::IsKeyPressed('V') && Keyboard::IsKeyDown('U')) { VaryVectorUniform(); CallChanged(); }
			if (Keyboard::IsKeyPressed('V') && Keyboard::IsKeyUp('U')) { VaryVectorBellApproximation(); CallChanged();}
			if (Keyboard::IsKeyPressed('C') && Keyboard::IsKeyDown('U')) { VaryComponentUniform(); CallChanged(); }
			if (Keyboard::IsKeyPressed('C') && Keyboard::IsKeyUp('U')) { VaryComponentBellApproximation(); CallChanged(); }
			if (Keyboard::IsKeyPressed('B') && Keyboard::IsKeyDown('U')) { VarySmoothUniform(); CallChanged(); }
			if (Keyboard::IsKeyPressed('B') && Keyboard::IsKeyUp('U')) { VarySmoothBellApproximation(); CallChanged(); }
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
		CallChanged();
	}

	void VariationController::RestoreHigh()
	{
		*m_pScale = m_highScale;
		*m_pRotation = m_highRotation;
		*m_pTranslation = m_highTranslation;
		CallChanged();
	}

	void VariationController::CallChanged()
	{
		m_onVariationChanged(m_pInstance);
	}

	void VariationController::ClearVariations()
	{
		*m_pScale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		*m_pRotation = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		*m_pTranslation = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		SaveLow();
		SaveHigh();
		CallChanged();
	}
}