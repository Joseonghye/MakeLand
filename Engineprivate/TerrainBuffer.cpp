#include "..\public\TerrainBuffer.h"

Engine::TerrainBuffer::TerrainBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:VIBuffer(pDevice, pDeviceContext),
	m_iNumVerticesX(0), m_iNumVerticesZ(0)
{
}

Engine::TerrainBuffer::TerrainBuffer(const TerrainBuffer & rhs)
	: VIBuffer(rhs),
	m_iNumVerticesX(rhs.m_iNumVerticesX), 
	m_iNumVerticesZ(rhs.m_iNumVerticesZ)
{
}
 
HRESULT Engine::TerrainBuffer::Ready_Prototype(const _tchar* pShaderFilePath, const _tchar*pHeightmapPath)
{
	if(FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	HANDLE	hFile = CreateFile(pHeightmapPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;
	_ulong dwByte = 0;

	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);

	_ulong*	pPixel = new _ulong[ih.biWidth * ih.biHeight]; //ÇÈ¼¿ °¹¼ö
	//ÇÈ¼¿ °ª
	ReadFile(hFile, pPixel, sizeof(_ulong) * ih.biWidth * ih.biHeight, &dwByte, nullptr);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	CloseHandle(hFile);

	//D3D11 BUFFER DESC
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	// D3D11 SUBRESOUREC DATA
	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	m_pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			((VTXNORTEX*)m_pVertices)[iIndex].vPosition = _float3(j, (pPixel[iIndex] & 0x000000ff) / 5.0f, i);
			((VTXNORTEX*)m_pVertices)[iIndex].vNormal = _float3(0.f,0.f,0.f);
			((VTXNORTEX*)m_pVertices)[iIndex].vTexUV = _float2(j / _float((m_iNumVerticesX - 1)), i / _float((m_iNumVerticesZ - 1)));
		}
	}

	//-----------------------------------//

	//D3D11 BUFFER DESC
	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IndicesByte = sizeof(FACEINDICES32);
	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iNumIndicesPrimitive = 3;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eFormat = DXGI_FORMAT_R32_UINT;

	m_IBDesc.ByteWidth = m_IndicesByte * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	// D3D11 SUBRESOUREC DATA
	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint	iNumPrimitive = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint	iIndex = i * m_iNumVerticesX + j;

			_uint	iIndices[] = {
				{ iIndex + m_iNumVerticesX },
				{ iIndex + m_iNumVerticesX + 1 },
				{ iIndex + 1 },
				{ iIndex }
			};

			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[1];
			pIndices[iNumPrimitive]._2 = iIndices[2];

			_vector vSour, vDest, vNormal;
			vSour = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[1]].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[0]].vPosition);

			vDest = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[2]].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[1]].vPosition);

			vNormal = XMVector3Cross(vSour, vDest);
			vNormal = XMVector3Normalize(vNormal);

			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[0]].vNormal) + vNormal));
			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[1]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[1]].vNormal) + vNormal));
			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[2]].vNormal) + vNormal));

			++iNumPrimitive;

			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[2];
			pIndices[iNumPrimitive]._2 = iIndices[3];

			vSour = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[2]].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[0]].vPosition);

			vDest = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[3]].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[2]].vPosition);

			vNormal = XMVector3Cross(vSour, vDest);
			vNormal = XMVector3Normalize(vNormal);

			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[0]].vNormal) + vNormal));
			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[2]].vNormal) + vNormal));
			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[3]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[iIndices[3]].vNormal) + vNormal));

			++iNumPrimitive;
		}
	}

	m_VBSubresourceData.pSysMem = m_pVertices;
	m_IBSubresourceData.pSysMem = pIndices;

	if (FAILED(__super::CreateVertexBuffer()))
		return E_FAIL;

	if (FAILED(__super::CreateIndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	//Á¤Á¡ ±¸Á¶Ã¼ ¸â¹ö ±¸¼º
	D3D11_INPUT_ELEMENT_DESC ElementDesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	//½¦ÀÌ´õ ÄÄÆÄÀÏ  
	if (FAILED(CompileShaderFiles(pShaderFilePath, ElementDesc, 3)))
		return E_FAIL;

	Safe_Delete_Array(pPixel);

	return S_OK;
}

HRESULT Engine::TerrainBuffer::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	return S_OK;
}

TerrainBuffer * Engine::TerrainBuffer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath, const _tchar*pHeightmapPath)
{
	TerrainBuffer* pInstance = new TerrainBuffer(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype(pShaderFilePath, pHeightmapPath)))
	{
		MSGBOX("Failed to Creating TerrainBuffer Buffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Component * Engine::TerrainBuffer::Clone(void * pArg)
{
	TerrainBuffer*	pInstance = new TerrainBuffer(*this);
	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating TerrainBuffer Buffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::TerrainBuffer::Free()
{
	__super::Free();
}
