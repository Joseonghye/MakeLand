#include "..\public\ViewPortBuffer.h"

ViewPortBuffer::ViewPortBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: VIBuffer(pDevice, pDeviceContext)
{

}

ViewPortBuffer::ViewPortBuffer(const ViewPortBuffer & rhs)
	: VIBuffer(rhs)
{
}

HRESULT ViewPortBuffer::Ready_Prototype( _float fX, _float fY, _float fSizeX, _float fSizeY, const _tchar* pShaderFilePath)
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	/* D3D11_BUFFER_DESC */
	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 4;

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	D3D11_VIEWPORT		ViewportDesc;

	_uint		iNumViewport = 1;	

	m_pDeviceContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	
	/* D3D11_SUBRESOURCE_DATA */
	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXTEX) * m_iNumVertices);

	((VTXTEX*)m_pVertices)[0].vPosition = _float3(fX / (ViewportDesc.Width * 0.5f) - 1.f, fY / (ViewportDesc.Height * -0.5f) + 1.f, 0.f);
	((VTXTEX*)m_pVertices)[0].vTexUV = _float2(0.0f, 0.f);

	((VTXTEX*)m_pVertices)[1].vPosition = _float3((fX + fSizeX) / (ViewportDesc.Width * 0.5f) - 1.f, fY / (ViewportDesc.Height * -0.5f) + 1.f, 0.f);
	((VTXTEX*)m_pVertices)[1].vTexUV = _float2(1.0f, 0.f);

	((VTXTEX*)m_pVertices)[2].vPosition = _float3((fX + fSizeX) / (ViewportDesc.Width * 0.5f) - 1.f, (fY + fSizeY) / (ViewportDesc.Height * -0.5f) + 1.f, 0.f);
	((VTXTEX*)m_pVertices)[2].vTexUV = _float2(1.0f, 1.f);

	((VTXTEX*)m_pVertices)[3].vPosition = _float3((fX) / (ViewportDesc.Width * 0.5f) - 1.f, (fY + fSizeY) / (ViewportDesc.Height * -0.5f) + 1.f, 0.f);
	((VTXTEX*)m_pVertices)[3].vTexUV = _float2(0.0f, 1.f);

	
	m_VBSubresourceData.pSysMem = m_pVertices;

	if (FAILED(__super::CreateVertexBuffer()))
		return E_FAIL;


	/* D3D11_BUFFER_DESC */
	m_IndicesByte = sizeof(FACEINDICES16);
	m_iNumPrimitive = 2;
	m_iNumIndicesPrimitive = 3;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eFormat = DXGI_FORMAT_R16_UINT;

	m_IBDesc.ByteWidth = m_IndicesByte * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;


	/* D3D11_SUBRESOURCE_DATA */
	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	FACEINDICES16*		pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);

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

	D3D11_INPUT_ELEMENT_DESC		ElementDescs[] = 
	{		
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }		
	};

	if (FAILED(CompileShaderFiles(pShaderFilePath, ElementDescs, 2)))
		return E_FAIL;

	return S_OK;
}

HRESULT ViewPortBuffer::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	return S_OK;
}

ViewPortBuffer * ViewPortBuffer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _float fX, _float fY, _float fSizeX, _float fSizeY, const _tchar* pShaderFilePath)
{
	ViewPortBuffer*		pInstance = new ViewPortBuffer(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready_Prototype(fX, fY, fSizeX, fSizeY, pShaderFilePath)))
	{
		MSGBOX("Failed to Creating ViewPortBuffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Component * ViewPortBuffer::Clone(void * pArg)
{
	ViewPortBuffer*		pInstance = new ViewPortBuffer(*this);

	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating ViewPortBuffer Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void ViewPortBuffer::Free()
{
	__super::Free();


}
