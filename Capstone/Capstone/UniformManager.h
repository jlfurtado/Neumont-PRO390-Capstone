#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

namespace Capstone
{
	class UniformManager
	{
	public:
		bool Initialize(ID3D11Device* pDevice, ID3DBlob *pVertBuff, ID3DBlob *pPixelBuff);
		bool Shutdown();
		bool AddUniform(const char *const name, int size, void *pData, bool pixelShader);

		void PassUniforms(ID3D11DeviceContext* pContext);

		static int GetLocation(const char *const name, ID3DBlob* pBuff);

	private:
		struct Uniform
		{
			const char *m_name{ nullptr };
			ID3D11Buffer *m_pBuffer{ nullptr };
			int m_size{ 0 };
			int m_location{ 0 };
			void *m_pData{ nullptr };
			bool m_pixelShader{ false };
		};

		static const int MAX_UNIFORMS = 20;
		Uniform m_uniforms[MAX_UNIFORMS];
		int m_nextUniform = 0;
	};


}
