#include "..\public\CubeBuffer.h"

Engine::CubeBuffer::CubeBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:VIBuffer(pDevice, pDeviceContext)
{
}

Engine::CubeBuffer::CubeBuffer(const CubeBuffer & rhs)
	:VIBuffer(rhs)
{
}
 
HRESULT Engine::CubeBuffer::Ready_Prototype(const _tchar* pShaderFilePath)
{
	if(FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	//D3D11 BUFFER DESC
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iStride = sizeof(VTXCUBETEX);
	m_iNumVertices = 8;

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE; //CPU 접근/읽기 불가 > 쉐이더 읽기/쓰기 가능
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride; //정점버퍼를 읽을때 사용 단위 

											  // D3D11 SUBRESOUREC DATA
	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	m_pVertices = new VTXCUBETEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXCUBETEX)*m_iNumVertices);

	((VTXCUBETEX*)m_pVertices)[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[0].vTexUV = ((VTXCUBETEX*)m_pVertices)[0].vPosition;

	((VTXCUBETEX*)m_pVertices)[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[1].vTexUV = ((VTXCUBETEX*)m_pVertices)[1].vPosition;

	((VTXCUBETEX*)m_pVertices)[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[2].vTexUV = ((VTXCUBETEX*)m_pVertices)[2].vPosition;

	((VTXCUBETEX*)m_pVertices)[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[3].vTexUV = ((VTXCUBETEX*)m_pVertices)[3].vPosition;

	((VTXCUBETEX*)m_pVertices)[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[4].vTexUV = ((VTXCUBETEX*)m_pVertices)[4].vPosition;

	((VTXCUBETEX*)m_pVertices)[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[5].vTexUV = ((VTXCUBETEX*)m_pVertices)[5].vPosition;

	((VTXCUBETEX*)m_pVertices)[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[6].vTexUV = ((VTXCUBETEX*)m_pVertices)[6].vPosition;

	((VTXCUBETEX*)m_pVertices)[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[7].vTexUV = ((VTXCUBETEX*)m_pVertices)[7].vPosition;

	m_VBSubresourceData.pSysMem = m_pVertices;

	if (FAILED(__super::CreateVertexBuffer()))
		return E_FAIL;

	//-----------------------------------//

	//D3D11 BUFFER DESC
	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IndicesByte = sizeof(FACEINDICES16);
	m_iNumPrimitive = 12;
	m_iNumIndicesPrimitive = 3;
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eFormat = DXGI_FORMAT_R16_UINT;

	m_IBDesc.ByteWidth = m_IndicesByte * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE; 
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	// D3D11 SUBRESOUREC DATA
	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	FACEINDICES16* pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES16)*m_iNumPrimitive);

	/* 오른쪽 */
	pIndices[0]._0 = 1; pIndices[0]._1 = 5; pIndices[0]._2 = 6;
	pIndices[1]._0 = 1; pIndices[1]._1 = 6; pIndices[1]._2 = 2;

	/* 왼쪽 */
	pIndices[2]._0 = 4; pIndices[2]._1 = 0; pIndices[2]._2 = 3;
	pIndices[3]._0 = 4; pIndices[3]._1 = 3; pIndices[3]._2 = 7;

	/* 위쪽 */
	pIndices[4]._0 = 4; pIndices[4]._1 = 5; pIndices[4]._2 = 1;
	pIndices[5]._0 = 4; pIndices[5]._1 = 1; pIndices[5]._2 = 0;

	/* 아래쪽 */
	pIndices[6]._0 = 3; pIndices[6]._1 = 2; pIndices[6]._2 = 6;
	pIndices[7]._0 = 3; pIndices[7]._1 = 6; pIndices[7]._2 = 7;

	/* 뒤쪽 */
	pIndices[8]._0 = 5; pIndices[8]._1 = 4; pIndices[8]._2 = 7;
	pIndices[9]._0 = 5; pIndices[9]._1 = 7; pIndices[9]._2 = 6;

	/* 앞쪽 */
	pIndices[10]._0 = 0; pIndices[10]._1 = 1; pIndices[10]._2 = 2;
	pIndices[11]._0 = 0; pIndices[11]._1 = 2; pIndices[11]._2 = 3;

	m_IBSubresourceData.pSysMem = pIndices;

	if (FAILED(__super::CreateIndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	//정점 구조체 멤버 구성
	D3D11_INPUT_ELEMENT_DESC ElementDesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//쉐이더 컴파일  
	if (FAILED(CompileShaderFiles(pShaderFilePath, ElementDesc, 2)))
		return E_FAIL;

	return S_OK;
}

HRESULT Engine::CubeBuffer::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	return S_OK;
}

CubeBuffer * Engine::CubeBuffer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath)
{
	CubeBuffer* pInstance = new CubeBuffer(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype(pShaderFilePath)))
	{
		MSGBOX("Failed to Creating CubeBuffer Buffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Component * Engine::CubeBuffer::Clone(void * pArg)
{
	CubeBuffer*	pInstance = new CubeBuffer(*this);
	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating CubeBuffer Buffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::CubeBuffer::Free()
{
	__super::Free();
}
