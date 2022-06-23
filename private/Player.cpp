#include "stdafx.h"
#include "..\public\Player.h"

#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Camera.h"

Client::Player::Player(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: GameObject(pDevice, pDeviceContext),
	m_pRenderer(nullptr), m_pModel(nullptr), m_pTransform(nullptr),
	m_pWeaponNode(nullptr), m_pWeapon(nullptr),
	m_eCurState(STATE_IDLE), m_eNextState(STATE_IDLE)
{
}

Client::Player::Player(const GameObject & rhs)
	: GameObject(rhs),
	m_eCurState(STATE_IDLE), m_eNextState(STATE_IDLE)
{
}

HRESULT Client::Player::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::Player::Ready(void * pArg)
{
	//if (FAILED(__super::Ready(pArg)))
	//	return E_FAIL;

	if (nullptr != pArg)
	{
		memcpy(&m_bBattle, pArg, sizeof(_bool));
	}
	else
		m_bBattle = false;

	if (FAILED(SetUpComponents()))
		return E_FAIL;

	//m_pTransform->Scaling(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));

	//m_eCurState = STATE_IDLE;
	//m_eNextState = STATE_IDLE;

	m_pWeaponNode = m_pModel->GetBoneNode("Bip01_L_Finger2_Bone");
	m_pModel->SetUpAnimationIndex((int)P_IDLE);
	//m_pWeaponNode = m_pModel->GetBoneNode("Bip01_R_Hand_Bone");

	return S_OK;
}

_int Client::Player::Update(_double TimeDelta)
{
	if (m_bBattle)
		return _int();

	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		m_eNextState = STATE_ATTACK;
	}
	else if(m_eCurState != STATE_ATTACK)
	{
		m_eNextState = STATE_IDLE;

		_vector vUp = XMVectorSet(0, 1, 0, 0);
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		_matrix camWorld = XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_VIEW));
		_vector vCamRight = XMVector3Normalize(camWorld.r[0]);
		_vector vCamForward = XMVector3Normalize(XMVector3Cross(vCamRight, vUp));

		_float fRadian = XMVectorGetX(XMVector3Dot(XMVectorSet(0,0,1,0), vCamForward));
		_vector Cross = XMVector3Cross(XMVectorSet(0, 0, 1, 0), vCamForward);
		
		//회전 방향 확인 
		_bool negative = false;
		if (XMVectorGetX(XMVector3Dot(vUp, Cross)) < 0) negative = true;

		fRadian = acosf(fRadian);
		if (negative) fRadian *= -1;

//		_float angle = XMConvertToDegrees(fRadian);

		RELEASE_INSTANCE(GameInstance);

		if (GetKeyState('W') & 0x8000)
		{
			m_pTransform->SetUpRotation(vUp, fRadian);
			m_eNextState = STATE_RUN;
			m_pTransform->GoStright(TimeDelta);
		}
		if (GetKeyState('A') & 0x8000)
		{
			//회전
			m_pTransform->SetUpRotation(vUp, (fRadian + XMConvertToRadians(-90.f)));
			m_pTransform->GoStright(TimeDelta);
			m_eNextState = STATE_RUN;
		}
		if (GetKeyState('D') & 0x8000)
		{
			m_pTransform->SetUpRotation(vUp, (fRadian + XMConvertToRadians(90.f)));
			m_pTransform->GoStright(TimeDelta);
			m_eNextState = STATE_RUN;
		}
		if (GetKeyState('S') & 0x8000)
		{
			m_pTransform->SetUpRotation(vUp, (fRadian+ XMConvertToRadians(180)));
			m_pTransform->GoStright(TimeDelta);
			m_eNextState = STATE_RUN;
		}
	}

	m_pCollider->Update(m_pTransform->GetWorldMatrix());

	//상태 셋팅 
	SetState();

	return _int();
}

_int Client::Player::LateUpdate(_double TimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderGroup(Renderer::RENDER_NONALPHA, this);

	m_pModel->UpdateCombinedTransformationMatrix(TimeDelta);

	if(m_eCurState == STATE_ATTACK)
		if (m_pModel->GetCurrentAnimationEnd())
			m_eNextState = STATE_IDLE;

	SetUpWeapon();

	return _int();
}

HRESULT Client::Player::Render()
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

	m_pWeapon->SetUpValueOnShader("g_WorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_WeaponMat)), sizeof(_matrix));
	m_pWeapon->SetUpValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pWeapon->SetUpValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	if (FAILED(m_pWeapon->BindBuffers()))
		return E_FAIL;

	for (_uint i = 0; i < m_pWeapon->GetNumMeshContainer(); ++i)
	{
		m_pWeapon->SetUpTextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pWeapon->Render(i, 0);
	}

	RELEASE_INSTANCE(GameInstance);

#ifdef _DEBUG
	m_pCollider->Render();
#endif // DEBUG


	return S_OK;
}

HRESULT Client::Player::SetUpComponents()
{
	Transform::TRANSFORMDESC transDesc;
	transDesc.fSpeedPerSec = 7.f;
	transDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (Component**)&m_pTransform,&transDesc)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (Component**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_GAMEPLAY, TEXT("Prototype_Component_Model_Yu"), TEXT("Com_Model"), (Component**)&m_pModel)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_GAMEPLAY, TEXT("Prototype_Component_Model_W_Yu"), TEXT("Com_Weapon"), (Component**)&m_pWeapon)))
		return E_FAIL;
	
	Collider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(Collider::COLLIDERDESC));
	ColliderDesc.vScale = _float3(0.7f, 1.5f, 0.8f);
	ColliderDesc.vPosition = _float3(0.f, 0.8f, 0.2f);
	if (FAILED(__super::SetUpComponents(SCENE_GAMEPLAY, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (Component**)&m_pCollider, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::Player::SetUpWeapon()
{
	_matrix		TransMatrix = XMMatrixIdentity();
	TransMatrix = XMMatrixRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(90.f))
		*XMMatrixRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(-15.f))
		*XMMatrixTranslation(-55.f,100.f,0.f);//XMMatrixRotationX(XMConvertToRadians(50.f));
//		XMMatrixRotationZ(XMConvertToRadians(160.f));
		//XMMatrixRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(-100));

//		XMMatrixRotationAxis(XMVectorSet(0, 0, 1, 0), XMConvertToRadians(180.f));
//		XMMatrixRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(-85.f));
//		
//		XMMatrixTranslation(-100.0f, -10.f, -10.f);
	

	// m_pModel->GetPivotMatrix() *
	_matrix		BoneMatrix = TransMatrix* m_pWeaponNode->GetOffsetMatrix() * m_pWeaponNode->GetCombinedMatrix()
		* m_pModel->GetPivotMatrix() * m_pTransform->GetWorldMatrix();

	XMStoreFloat4x4(&m_WeaponMat, BoneMatrix);

	return S_OK;
}

void Client::Player::SetState()
{
	if (m_eNextState == m_eCurState)
		return;

	//m_isRot = false;
	m_eCurState = m_eNextState;

	switch (m_eCurState)
	{
	case STATE_IDLE:
		m_pModel->SetUpAnimationIndex((int)P_IDLE);
		break;
	case STATE_RUN:
		m_pModel->SetUpAnimationIndex((int)P_RUN);
		break;
	case STATE_ATTACK:
		m_pModel->SetUpAnimationIndex((int)P_ATTACK);
		break;
	}

}

Player * Client::Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	Player*	pInstance = new Player(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

GameObject * Client::Player::Clone(void * pArg)
{
	Player*	pInstance = new Player(*this);

	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating Player Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pWeapon);
//	Safe_Release(m_pCollider);
	Safe_Release(m_pRenderer);
}
