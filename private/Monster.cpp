#include "stdafx.h"
#include "..\public\Monster.h"

#include "GameInstance.h"

Client::Monster::Monster(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: GameObject(pDevice, pDeviceContext),
	m_pRenderer(nullptr), m_pModel(nullptr), m_pTransform(nullptr),
	m_eCurState(STATE_IDLE), m_eNextState(STATE_IDLE)
{
}

Client::Monster::Monster(const GameObject & rhs)
	: GameObject(rhs)
{
}

HRESULT Client::Monster::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::Monster::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	if (FAILED(SetUpComponents()))
		return E_FAIL;

	m_eCurState = STATE_IDLE;
	m_eNextState = STATE_IDLE;

	return S_OK;
}

_int Client::Monster::Update(_double TimeDelta)
{
	m_pCollider->Update(m_pTransform->GetWorldMatrix());

	//상태 셋팅 
	SetState();

	return _int();
}

_int Client::Monster::LateUpdate(_double TimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderGroup(Renderer::RENDER_NONALPHA, this);

	m_pModel->UpdateCombinedTransformationMatrix(TimeDelta);

	//if(m_eCurState == STATE_ATTACK)
	//	if (m_pModel->GetCurrentAnimationEnd())
	//		m_eNextState = STATE_IDLE;

	GameInstance*		pGameInstance = GET_INSTANCE(GameInstance);
	Collider*		pPlayerCollider = (Collider*)pGameInstance->GetComponent(SCENE_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"));
	m_pCollider->Collision(pPlayerCollider);
	RELEASE_INSTANCE(GameInstance);

	return _int();
}

HRESULT Client::Monster::Render()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	m_pModel->SetUpValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransform->GetWorldMatrix()), sizeof(_matrix));
	m_pModel->SetUpValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pModel->SetUpValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_PROJECTION)), sizeof(_matrix));
	
	if (FAILED(m_pModel->BindBuffers()))
		return E_FAIL;

	for (_uint i = 0; i < m_pModel->GetNumMeshContainer(); ++i)
	{
		m_pModel->SetUpTextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModel->Render(i, 1);
	}

	RELEASE_INSTANCE(GameInstance);

#ifdef _DEBUG
	m_pCollider->Render();
#endif // DEBUG


	return S_OK;
}

HRESULT Client::Monster::SetUpComponents()
{
	Transform::TRANSFORMDESC transDesc;
	transDesc.fSpeedPerSec = 7.f;
	transDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (Component**)&m_pTransform,&transDesc)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (Component**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_GAMEPLAY, TEXT("Prototype_Component_Model_Monster"), TEXT("Com_Model"), (Component**)&m_pModel)))
		return E_FAIL;
	
	Collider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(Collider::COLLIDERDESC));
	ColliderDesc.vScale = _float3(0.8f, 1.f, 1.5f);
	ColliderDesc.vPosition = _float3(0.f, 0.5f, -0.5f);
	if (FAILED(__super::SetUpComponents(SCENE_GAMEPLAY, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (Component**)&m_pCollider, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

void Client::Monster::SetState()
{
	if (m_eNextState == m_eCurState)
		return;

	m_eCurState = m_eNextState;

	switch (m_eCurState)
	{
	case STATE_IDLE:
		m_pModel->SetUpAnimationIndex((int)M_IDLE);
		break;
	case STATE_RUN:
		m_pModel->SetUpAnimationIndex((int)M_RUN);
		break;
	case STATE_ATTACK:
		m_pModel->SetUpAnimationIndex((int)M_ATTACK);
		break;
	}

}

Monster * Client::Monster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	Monster*	pInstance = new Monster(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

GameObject * Client::Monster::Clone(void * pArg)
{
	Monster*	pInstance = new Monster(*this);

	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating Monster Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Monster::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
//	Safe_Release(m_pCollider);
	Safe_Release(m_pRenderer);
}
