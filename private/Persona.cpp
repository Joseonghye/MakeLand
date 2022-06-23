#include "stdafx.h"
#include "..\public\Persona.h"
#include "GameInstance.h"

Client::Persona::Persona(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:GameObject(pDevice,pDeviceContext)
{
}

Client::Persona::Persona(Persona & rhs)
	: GameObject(rhs), m_Visible(false)
{
}

vector<SKILLDATA> Client::Persona::GetPersonaSkills()
{
	vector<SKILLDATA> vecSkills;
	for (int i = 0; i < m_pPersonaData->iSkillCnt; ++i)
	{
		vecSkills.push_back(m_pPersonaData->Skills[i]);
	}

	return vecSkills;
}

void Client::Persona::SetPosition(_vector vPos,_vector vLook)
{
	m_pTransform->SetState(Transform::STATE_POSITION, vPos);

	m_pTransform->SetState(Transform::STATE_LOOK, vLook);

	_vector	vRight = XMVector3Cross(XMVectorSet(0, 1, 0, 0), vLook);
	vRight = XMVector3Normalize(vRight);
	_vector	vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector3Normalize(vUp);

	m_pTransform->SetState(Transform::STATE_RIGHT, vRight);
	m_pTransform->SetState(Transform::STATE_UP, vUp);
}

HRESULT Client::Persona::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::Persona::Ready(void * pArg)
{
	if (pArg != nullptr)
	{
		m_pPersonaData = (PERSONADATA*)pArg;

		if (FAILED(SetUpComponents()))
			return E_FAIL;
	}
	else
		return E_FAIL;


	return S_OK;
}

_int Client::Persona::Update(_double TimeDelta)
{
	return _int();
}

_int Client::Persona::LateUpdate(_double TimeDelta)
{
	if (!m_Visible)
		return _int();

	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderGroup(Renderer::RENDER_NONALPHA, this);

	m_pModel->UpdateCombinedTransformationMatrix(TimeDelta);

	//현재 애니메이션이 끝나면 다시 안보이도록 
	if (m_pModel->GetCurrentAnimationEnd())
		m_Visible = false;

	return _int();
}

HRESULT Client::Persona::Render()
{
	//평상시엔 출력 x 
	if(!m_Visible)
		return S_OK;

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
	return S_OK;
}

HRESULT Client::Persona::SetUpComponents()
{
	Transform::TRANSFORMDESC transDesc;
	transDesc.fSpeedPerSec = 7.f;
	transDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (Component**)&m_pTransform, &transDesc)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (Component**)&m_pRenderer)))
		return E_FAIL;

	//string to tchar
	const char* szModel = m_pPersonaData->strModelName.c_str();
	int len = 1 + strlen(szModel);
	size_t cn;
	_tchar* ModelName = new _tchar[len];
	mbstowcs_s(&cn, ModelName, len, szModel, len);

	if (FAILED(__super::SetUpComponents(SCENE_BATTLE, ModelName, TEXT("Com_Model"), (Component**)&m_pModel)))
		return E_FAIL;

	m_strName = m_pPersonaData->strName;


	return S_OK;
}

Persona * Client::Persona::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	Persona*	pInstance = new Persona(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating Persona");
		Safe_Release(pInstance);
	}
	return pInstance;
}

Persona * Client::Persona::Clone(void * pArg)
{
	Persona*	pInstance = new Persona(*this);
	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating Persona Clone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Client::Persona::Free()
{
	__super::Free();
	//Safe_Release(m_pTransform);
	//Safe_Release(m_pModel);
	//Safe_Release(m_pRenderer);
}
