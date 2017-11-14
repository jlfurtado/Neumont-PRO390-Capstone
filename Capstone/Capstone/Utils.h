#pragma once
#include <memory>
#include <DirectXMath.h>

namespace Capstone
{
	class MyUtils
	{
	public:
		template<typename T>
		static void MyClearFunc(T *const data)
		{
			memset(data, NULL, sizeof(*data));
		}

		template<typename T>
		static void MyClearFunc(T *const data, int count)
		{
			memset(data, NULL, sizeof(*data) * count);
		}

		template<typename T>
		static void SafeDeleteSingle(T *&pData)
		{
			if (pData)
			{
				delete pData;
				pData = nullptr;
			}
		}

		template <typename T>
		static void SafeDeleteArray(T *&pArray)
		{
			if (pArray)
			{
				delete[] pArray;
				pArray = nullptr;
			}
		}

		static DirectX::XMMATRIX MTWFromSRT(const DirectX::XMVECTOR * pScale, const DirectX::XMVECTOR * pRotation, const DirectX::XMVECTOR * pTranslation)
		{
			return DirectX::XMMatrixScalingFromVector(*pScale) * DirectX::XMMatrixRotationRollPitchYawFromVector(*pRotation) * DirectX::XMMatrixTranslationFromVector(*pTranslation);
		}
	};
}
