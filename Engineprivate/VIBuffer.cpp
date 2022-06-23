#include "..\public\VIBuffer.h"

#include "Texture.h"

Engine::VIBuffer::VIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext* pDeviceContext)
	:Component(pDevice, pDeviceContext),
	m_pVB(nullptr), m_pIB(nullptr),
	m_iStride(0), m_iNumVertices(0),
	m_IndicesByte(0), m_iNumPrimitive(0), m_iNumIndicesPrimitive(0),
	m_pEffect(nullptr)
{
}

Engine::VIBuffer::VIBuffer(const VIBuffer & rhs)
	:Component(rhs),
	 m_pVB(rhs.m_pVB),
	 m_VBDesc(rhs.m_VBDesc),
	 m_VBSubresourceData(rhs.m_VBSubresourceData),
	 m_iStride(rhs.m_iStride),
	 m_iNumVertices(rhs.m_iNumVertices),
	 m_pVertices(rhs.m_pVertices),
	 m_pIB(rhs.m_pIB),
	 m_IBDesc(rhs.m_IBDesc),
	 m_IBSubresourceData(rhs.m_IBSubresourceData),
	 m_IndicesByte(rhs.m_IndicesByte),
	 m_iNumPrimitive(rhs.m_iNumPrimitive),
	 m_iNumIndicesPrimitive(rhs.m_iNumIndicesPrimitive),
	 m_ePrimitiveTopology(rhs.m_ePrimitiveTopology),
	 m_pEffect(rhs.m_pEffect),
	 m_EffectDescs(rhs.m_EffectDescs),
	m_eFormat(rhs.m_eFormat)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
	Safe_AddRef(m_pEffect);

	for (auto& pEffectDesc : m_EffectDescs)
	{
		Safe_AddRef(pEffectDesc->pInputLayout);
		Safe_AddRef(pEffectDesc->pPass);
	}
}

HRESULT Engine::VIBuffer::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Engine::VIBuffer::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT Engine::VIBuffer::Render(_uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	//그릴 버텍스 버퍼를 장치에 바인드 
	//버텍스 버퍼는 여러개의 버퍼 사용가능 (인덱스는 하나만가능)

	ID3D11Buffer*	pVertexBuffers[] = { m_pVB };
	_uint	iStrides[] = { m_iStride };
	_uint	iOffsets[] = { 0 };

	m_pDeviceContext->IASetVertexBuffers(0, 1, pVertexBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	m_pDeviceContext->IASetInputLayout(m_EffectDescs[iPassIndex]->pInputLayout);

	if (FAILED(m_EffectDescs[iPassIndex]->pPass->Apply(0, m_pDeviceContext)))
		return E_FAIL;

	m_pDeviceContext->DrawIndexed(m_iNumIndicesPrimitive*m_iNumPrimitive, 0, 0);

	return S_OK;
}

HRESULT Engine::VIBuffer::SetUpValueOnShader(const char* pConstantName, void* pData, _uint iSize)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	if (FAILED(pVariable->SetRawValue(pData, 0, iSize)))
		return E_FAIL;

	return S_OK;
}

HRESULT Engine::VIBuffer::SetUpTextureOnShader(const char * pConstantName, Texture * pTexture, _uint iTextureIndex)
{
	if (nullptr == pTexture)
		return E_FAIL;

	ID3D11ShaderResourceView* pShaderResourceView = pTexture->GetShaderResourceView(iTextureIndex);
	if (nullptr == pShaderResourceView)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetResource(pShaderResourceView);
}

HRESULT Engine::VIBuffer::SetUpTextureOnShader(const char * pConstantName, ID3D11ShaderResourceView * pSRV)
{
	ID3DX11EffectShaderResourceVariable* pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetResource(pSRV);
}

HRESULT Engine::VIBuffer::CreateVertexBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBDesc, &m_VBSubresourceData, &m_pVB)))
		return E_FAIL;

	return S_OK;
}

HRESULT Engine::VIBuffer::CreateIndexBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_IBDesc, &m_IBSubresourceData, &m_pIB)))
		return E_FAIL;

	return S_OK;
}

HRESULT Engine::VIBuffer::CompileShaderFiles(const _tchar * pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElementDesc, _uint iNumElements)
{
	_uint iFlag = 0;
#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG

	ID3DBlob*	pShaderByteCodes = nullptr;
	ID3DBlob*	pCompileErrorMsg = nullptr;

	if (FAILED(D3DCompileFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, nullptr, "fx_5_0", iFlag, 0, &pShaderByteCodes, &pCompileErrorMsg)))
		return E_FAIL;

	if (FAILED(D3DX11CreateEffectFromMemory(pShaderByteCodes->GetBufferPointer(), pShaderByteCodes->GetBufferSize(), 0, m_pDevice, &m_pEffect)))
		return E_FAIL;

	ID3DX11EffectTechnique*		pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC			EffectDesc;
	if (FAILED(pTechnique->GetDesc(&EffectDesc)))
		return E_FAIL;

	m_EffectDescs.reserve(EffectDesc.Passes);
	for (_uint i = 0; i < EffectDesc.Passes; ++i)
	{
		EFFECTDESC* pEffectDesc = new EFFECTDESC;
		ZeroMemory(pEffectDesc, sizeof(EFFECTDESC));

		pEffectDesc->pPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC passDesc;
		pEffectDesc->pPass->GetDesc(&passDesc);

		if (FAILED(m_pDevice->CreateInputLayout(pElementDesc, iNumElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &pEffectDesc->pInputLayout)))
			return E_FAIL;

		m_EffectDescs.push_back(pEffectDesc);
	}

	return S_OK;
}

void Engine::VIBuffer::Free()
{
	__super::Free();

	for (auto& pEffectDesc : m_EffectDescs)
	{
		Safe_Release(pEffectDesc->pInputLayout);
		//Safe_Release(pEffectDesc->pPass);
	}

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pVertices);

		for (auto& pEffectDesc : m_EffectDescs)
			Safe_Delete(pEffectDesc);

		m_EffectDescs.clear();
	}

	Safe_Release(m_pEffect);
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

}
