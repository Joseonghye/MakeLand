#include "..\public\BaseUI.h"

Engine::BaseUI::BaseUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:GameObject(pDevice, pDeviceContext), m_pRectBuf(nullptr)
{
	m_eTag = GameObjectTag::UI;
}

Engine::BaseUI::BaseUI(const BaseUI & rhs)
	:GameObject(rhs)
{
}

HRESULT Engine::BaseUI::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Engine::BaseUI::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;
	return S_OK;
}

_int Engine::BaseUI::Update(_double TimeDelta)
{
	return _int();
}

_int Engine::BaseUI::LateUpdate(_double TimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderGroup(Renderer::RENDER_UI, this);
	return _int();
}

HRESULT Engine::BaseUI::Render()
{
	m_pRectBuf->SetUpValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_WorldMatrix), sizeof(_float) * 16);
	m_pRectBuf->SetUpValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pRectBuf->SetUpValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));


	m_pRectBuf->SetUpTextureOnShader("g_DiffuseTexture", m_pTexture, 0);

	m_pRectBuf->Render(0);
	return S_OK;
}

GameObject * Engine::BaseUI::Clone(void * pArg)
{
	return nullptr;
}

void Engine::BaseUI::Free()
{
	__super::Free();

	//Safe_Release(m_pTexture);
	//Safe_Release(m_pRectBuf);
	//Safe_Release(m_pRenderer);
}
