#include "UniformManager.h"
#include "DebugConsole.h"
#include "Utils.h"
#include <D3D11Shader.h>
#include <d3dcompiler.h>

namespace Capstone
{
	bool UniformManager::Initialize(ID3D11Device * pDevice, ID3DBlob *pVertBuff, ID3DBlob *pPixelBuff)
	{
		for (int i = 0; i < m_nextUniform; ++i)
		{
			D3D11_BUFFER_DESC constDesc;
			ZeroMemory(&constDesc, sizeof(constDesc));
			constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constDesc.ByteWidth = m_uniforms[i].m_size;
			constDesc.Usage = D3D11_USAGE_DEFAULT;

			HRESULT r = pDevice->CreateBuffer(&constDesc, 0, &m_uniforms[i].m_pBuffer);
			if (FAILED(r))
			{
				DebugConsole::Log("Failed to Initialize Unfirom Manager! Uniform [%d] [%s] failed to make buffer!\n", i, m_uniforms[i].m_name);
				return false;
			}

			m_uniforms[i].m_location = GetLocation(m_uniforms[i].m_name, m_uniforms[i].m_pixelShader ? pPixelBuff : pVertBuff);
		}

		return true;
	}

	bool UniformManager::Shutdown()
	{
		for (int i = 0; i < m_nextUniform; ++i)
		{
			if (m_uniforms[i].m_pBuffer) { m_uniforms[i].m_pBuffer->Release(); }
			m_uniforms[i].m_pBuffer = nullptr;
		}

		return true;
	}

	bool UniformManager::AddUniform(const char * const name, int size, void * pData, bool pixelShader)
	{
		if (m_nextUniform >= MAX_UNIFORMS) { DebugConsole::Log("ERROR! Cannot add uniform! Max uniforms of [%d] exceeded!\n", MAX_UNIFORMS); return false; }

		m_uniforms[m_nextUniform].m_pBuffer = nullptr;
		m_uniforms[m_nextUniform].m_name = name;
		m_uniforms[m_nextUniform].m_size = size;
		m_uniforms[m_nextUniform].m_pixelShader = pixelShader;
		m_uniforms[m_nextUniform].m_pData = pData;
		m_nextUniform++;

		return true;
	}

	void UniformManager::PassUniforms(ID3D11DeviceContext* pContext)
	{
		for (int i = 0; i < m_nextUniform; ++i)
		{
			pContext->UpdateSubresource(m_uniforms[i].m_pBuffer, 0, 0, m_uniforms[i].m_pData, 0, 0);

			if (m_uniforms[i].m_pixelShader)
			{
				pContext->PSSetConstantBuffers(m_uniforms[i].m_location, 1, &m_uniforms[i].m_pBuffer);
			}
			else 
			{
				pContext->VSSetConstantBuffers(m_uniforms[i].m_location, 1, &m_uniforms[i].m_pBuffer);
			}
		}
	}

	int UniformManager::GetLocation(const char * const name, ID3DBlob* pBuff)
	{
		ID3D11ShaderReflection* pReflector = NULL;
		HRESULT r1 = D3DReflect(pBuff->GetBufferPointer(), pBuff->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector);
		if (FAILED(r1)) { return -1; }

		D3D11_SHADER_INPUT_BIND_DESC desc;
		MyUtils::MyClearFunc(&desc);
		HRESULT r2 = pReflector->GetResourceBindingDescByName(name, &desc);
		if (FAILED(r2)) { return -1; }

		return desc.BindPoint;
	}
}
