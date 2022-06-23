#include "..\public\Texture.h"

Engine::Texture::Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Component(pDevice, pDeviceContext)
{
}

Engine::Texture::Texture(const Texture & rhs)
	: Component(rhs),
	m_Textures(rhs.m_Textures)
{
	for (auto& pShaderResourceView : m_Textures)
		Safe_AddRef(pShaderResourceView);
}

HRESULT Engine::Texture::Ready_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	_tchar szFullPath[MAX_PATH] = TEXT("");

	DirectX::ScratchImage ScratchImg;

	HRESULT hr = 0;
	for (_uint i = 0; i < iNumTextures; ++i)
	{
		_tchar szExt[MAX_PATH] = TEXT("");
		_wsplitpath_s(pTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, 256);

		wsprintf(szFullPath, pTextureFilePath, i);

		if (0 == lstrcmp(szExt, TEXT(".dds")))
			hr = DirectX::LoadFromDDSFile(szFullPath, DirectX::CP_FLAGS_NONE, nullptr, ScratchImg);

		else if (0 == lstrcmp(szExt, TEXT(".tga")))
			hr = DirectX::LoadFromTGAFile(szFullPath, nullptr, ScratchImg);

		else
			hr = DirectX::LoadFromWICFile(szFullPath, DirectX::CP_FLAGS_NONE, nullptr, ScratchImg);
		
		if (FAILED(hr))
			return E_FAIL;

		ID3D11Resource* pTextureResource = nullptr;
		if (FAILED(DirectX::CreateTexture(m_pDevice, ScratchImg.GetImages(), ScratchImg.GetImageCount(), ScratchImg.GetMetadata(), &pTextureResource)))
			return E_FAIL;

		ID3D11ShaderResourceView*		pShaderResourceView = nullptr;
		if (FAILED(m_pDevice->CreateShaderResourceView(pTextureResource, nullptr, &pShaderResourceView)))
			return E_FAIL;

		m_Textures.push_back(pShaderResourceView);

		Safe_Release(pTextureResource);
		ScratchImg.Release();
	}

	return S_OK;
}

HRESULT Engine::Texture::Ready(void * pArg)
{
	return S_OK;
}

Texture * Engine::Texture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pTextureFilePath, _uint iNumTextures)
{
	Texture* pInstance = new Texture(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype(pTextureFilePath,iNumTextures)))
	{
		MSGBOX("Failed to Creating Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Component * Engine::Texture::Clone(void * pArg)
{
	Texture* pInstance = new Texture(*this);
	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::Texture::Free()
{
	__super::Free();

	for (auto& pShaderResourceView : m_Textures)
		Safe_Release(pShaderResourceView);

}
