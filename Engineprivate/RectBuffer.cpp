#include "..\public\RectBuffer.h"

Engine::RectBuffer::RectBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:VIBuffer(pDevice, pDeviceContext)
{
}

Engine::RectBuffer::RectBuffer(const RectBuffer & rhs)
	:VIBuffer(rhs)
{
}
 
HRESULT Engine::RectBuffer::Ready_Prototype(const _tchar* pShaderFilePath)
{
	if(FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	//D3D11 BUFFER DESC
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 4;

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE; //CPU 접근/읽기 불가 > 쉐이더 읽기/쓰기 가능
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride; //정점버퍼를 읽을때 사용 단위 

											  // D3D11 SUBRESOUREC DATA
	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	m_pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXTEX)*m_iNumVertices);

	((VTXTEX*)m_pVertices)[0].vPosition = _float3(-0.5f, 0.5f, 0);
	((VTXTEX*)m_pVertices)[0].vTexUV = _float2(0.0f, 0.f);

	((VTXTEX*)m_pVertices)[1].vPosition = _float3(0.5f, 0.5f, 0);
	((VTXTEX*)m_pVertices)[1].vTexUV = _float2(1.0f, 0.f);

	((VTXTEX*)m_pVertices)[2].vPosition = _float3(0.5f, -0.5f, 0);
	((VTXTEX*)m_pVertices)[2].vTexUV = _float2(1.0f, 1.f);

	((VTXTEX*)m_pVertices)[3].vPosition = _float3(-0.5f, -0.5f, 0);
	((VTXTEX*)m_pVertices)[3].vTexUV = _float2(0.0f, 1.f);

	m_VBSubresourceData.pSysMem = m_pVertices;
	//단순 주소 복사가 아닌 사이즈 할당 및 memcopy > m_pVertices를 지워도 되지만 추후 정점 참고를 할 수 있으니 놔둠

	if (FAILED(__super::CreateVertexBuffer()))
		return E_FAIL;

	//-----------------------------------//

	//D3D11 BUFFER DESC
	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IndicesByte = sizeof(FACEINDICES16);
	m_iNumPrimitive = 2;
	m_iNumIndicesPrimitive = 3;
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eFormat = DXGI_FORMAT_R16_UINT;

	m_IBDesc.ByteWidth = m_IndicesByte * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE; //CPU 접근/읽기 불가 > 쉐이더 읽기/쓰기 가능
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	// D3D11 SUBRESOUREC DATA
	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	FACEINDICES16* pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES16)*m_iNumPrimitive);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_IBSubresourceData.pSysMem = pIndices;

	if (FAILED(__super::CreateIndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	//정점 구조체 멤버 구성
	D3D11_INPUT_ELEMENT_DESC ElementDesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	//쉐이더 컴파일  
	if (FAILED(CompileShaderFiles(pShaderFilePath, ElementDesc, 2)))
		return E_FAIL;

	return S_OK;
}

HRESULT Engine::RectBuffer::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	return S_OK;
}

RectBuffer * Engine::RectBuffer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath)
{
	RectBuffer* pInstance = new RectBuffer(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype(pShaderFilePath)))
	{
		MSGBOX("Failed to Creating RectBuffer Buffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Component * Engine::RectBuffer::Clone(void * pArg)
{
	RectBuffer*	pInstance = new RectBuffer(*this);
	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating RectBuffer Buffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::RectBuffer::Free()
{
	__super::Free();
}
