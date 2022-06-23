#include "..\public\Light.h"
#include "ViewPortBuffer.h"
#include "TargetMgr.h"
#include "GameInstance.h"

Engine::Light::Light(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:m_pDevice(pDevice), m_pDeviceContext(pDeviceContext), m_pVIBuffer(nullptr)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(pDeviceContext);
}

HRESULT Engine::Light::Ready(const LIGHTDESC& LightDesc)
{
	m_LightDesc = LightDesc;

	D3D11_VIEWPORT		ViewportDesc;
	_uint		iNumViewports = 1;
	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_pVIBuffer = ViewPortBuffer::Create(m_pDevice, m_pDeviceContext, 0.f, 0.f, ViewportDesc.Width, ViewportDesc.Height, TEXT("../Bin/ShaderFiles/Shader_RectViewPort.hlsl"));
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	return S_OK;
}

HRESULT Engine::Light::Render()
{
	TargetMgr*	pTargetMgr = GET_INSTANCE(TargetMgr);
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	_uint		iPassIndex = 0;

	if (m_LightDesc.eType == tagLightDesc::TYPE_DIRECTIONAL)
	{
		iPassIndex = 1;
		m_pVIBuffer->SetUpValueOnShader("g_vLightDir", &_float4(m_LightDesc.vDirection.x, m_LightDesc.vDirection.y, m_LightDesc.vDirection.z, 0.f), sizeof(_float4));
	}
	else if (m_LightDesc.eType == tagLightDesc::TYPE_POINT)
	{
		iPassIndex = 2;
		m_pVIBuffer->SetUpValueOnShader("g_vLightPos", &_float4(m_LightDesc.vPosition.x, m_LightDesc.vPosition.y, m_LightDesc.vPosition.z, 1.f), sizeof(_float4));
		m_pVIBuffer->SetUpValueOnShader("g_fRange", &m_LightDesc.fRange, sizeof(_float));
	}

	m_pVIBuffer->SetUpTextureOnShader("g_NormalTexture", pTargetMgr->GetSRV(TEXT("Target_Normal")));
	m_pVIBuffer->SetUpTextureOnShader("g_DepthTexture", pTargetMgr->GetSRV(TEXT("Target_Depth")));


	m_pVIBuffer->SetUpValueOnShader("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4));
	m_pVIBuffer->SetUpValueOnShader("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4));
	m_pVIBuffer->SetUpValueOnShader("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4));

	_vector		vCamPosition = pGameInstance->GetCamPos();

	_matrix		ViewMatrix = pGameInstance->GetTransform(PipeLine::D3DTS_VIEW);
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
	_matrix		ProjMatrix = pGameInstance->GetTransform(PipeLine::D3DTS_PROJECTION);
	ProjMatrix = XMMatrixInverse(nullptr, ProjMatrix);

	m_pVIBuffer->SetUpValueOnShader("g_vCamPosition", &vCamPosition, sizeof(_float4));
	m_pVIBuffer->SetUpValueOnShader("g_ViewMatrixInv", &XMMatrixTranspose(ViewMatrix), sizeof(_float4x4));
	m_pVIBuffer->SetUpValueOnShader("g_ProjMatrixInv", &XMMatrixTranspose(ProjMatrix), sizeof(_float4x4));

	m_pVIBuffer->Render(iPassIndex);

	RELEASE_INSTANCE(GameInstance);
	RELEASE_INSTANCE(TargetMgr);

	return S_OK;
}

Light * Engine::Light::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC& LightDesc)
{
	Light* pInstance = new Light(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready(LightDesc)))
	{
		MSGBOX("Failed to Creating Light");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::Light::Free()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
