#include "..\public\CustomFont.h"

Engine::CustomFont::CustomFont(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice), m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT Engine::CustomFont::Ready_CustomFont(const _tchar * pFontPath)
{
	m_pSpriteBatch = new DirectX::SpriteBatch(m_pDeviceContext);
	if (nullptr == m_pSpriteBatch)
		return E_FAIL;

	m_pSpriteFont = new DirectX::SpriteFont(m_pDevice, pFontPath);
	if (nullptr == m_pSpriteFont)
		return E_FAIL;

	return S_OK;
}

HRESULT Engine::CustomFont::Render_CustomFont(XMFLOAT2 vPos, const _tchar* pString, _fvector vColor)
{
	if (nullptr == m_pSpriteBatch || nullptr == m_pSpriteFont)
		return E_FAIL;

	m_pSpriteBatch->Begin();

	m_pSpriteFont->DrawString(m_pSpriteBatch, pString, vPos, vColor, 0.0f, _float2(0.f, 0.f), _float2(1.f, 1.f));

	m_pSpriteBatch->End();

	return S_OK;
}

CustomFont * Engine::CustomFont::Create(ID3D11Device * pGraphic_Device, ID3D11DeviceContext * pGraphic_Device_Context, const _tchar * pFontPath)
{
	CustomFont*	pInstance = new CustomFont(pGraphic_Device, pGraphic_Device_Context);

	if (FAILED(pInstance->Ready_CustomFont(pFontPath)))
	{
		MSGBOX("Failed To Create CustomFont");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Engine::CustomFont::Free()
{
	Safe_Delete(m_pSpriteFont);
	Safe_Delete(m_pSpriteBatch);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
