#include "..\public\RenderTarget.h"
#include "ViewPortBuffer.h"

RenderTarget::RenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

}

HRESULT RenderTarget::Ready(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor)
{
	if (nullptr == m_pDevice)
		return E_FAIL;	

	D3D11_TEXTURE2D_DESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = eFormat;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture)))
		return E_FAIL;

	D3D11_RENDER_TARGET_VIEW_DESC		RTVDesc;
	ZeroMemory(&RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	RTVDesc.Format = eFormat;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture, &RTVDesc, &m_pRTV)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC		SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	SRVDesc.Format = eFormat;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pSRV)))
		return E_FAIL;

	m_vClearColor = vClearColor;

	return S_OK;
}

HRESULT RenderTarget::Clear()
{
	if (nullptr == m_pRTV || 
		nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);

	return S_OK;
}

#ifdef _DEBUG
HRESULT RenderTarget::Ready_DebugBuffer(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	m_pDebugBuffer = ViewPortBuffer::Create(m_pDevice, m_pDeviceContext, fX, fY, fSizeX, fSizeY, TEXT("../Bin/ShaderFiles/Shader_RectViewPort.hlsl"));
	if (nullptr == m_pDebugBuffer)
		return E_FAIL;

	Clear();

	return S_OK;
}

HRESULT RenderTarget::Render_DebugBuffer()
{
	if (FAILED(m_pDebugBuffer->SetUpTextureOnShader("g_DiffuseTexture", m_pSRV)))
		return E_FAIL;

	m_pDebugBuffer->Render(0);

	return S_OK;
}

#endif // _DEBUG


RenderTarget * RenderTarget::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor)
{
	RenderTarget*		pInstance = new RenderTarget(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready(iWidth, iHeight, eFormat, vClearColor)))
	{
		MSGBOX("Failed to Creating CRenderTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void RenderTarget::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pDebugBuffer);
#endif // _DEBUG

	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);
	Safe_Release(m_pTexture);

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
