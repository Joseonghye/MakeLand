#include "..\public\FontMgr.h"
#include "CustomFont.h"

IMPLEMENT_SINGLETON(FontMgr)

Engine::FontMgr::FontMgr()
{
}

HRESULT Engine::FontMgr::AddFont(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pFontTag, const _tchar * pFontPath)
{
	CustomFont*	pFont = CustomFont::Create(pDevice, pDeviceContext, pFontPath);

	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.insert(FONT::value_type(pFontTag, pFont));

	return S_OK;
}

HRESULT Engine::FontMgr::Render_Font(const _tchar * pFontTag, XMFLOAT2 vPos, const _tchar * pString, _fvector vColor)
{
	auto	iter = find_if(m_Fonts.begin(), m_Fonts.end(), Tag_Finder(pFontTag));

	if (iter == m_Fonts.end())
		return E_FAIL;

	return iter->second->Render_CustomFont(vPos, pString,vColor);
}

void Engine::FontMgr::Free()
{
	for (auto& pFont : m_Fonts)
		Safe_Release(pFont.second);

	m_Fonts.clear();
}

