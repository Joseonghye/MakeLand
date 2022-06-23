#include "..\public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "AnimChannel.h"

Engine::Model::Model(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:VIBuffer(pDevice, pDeviceContext),
	m_pScene(nullptr), m_pFaceIndices(nullptr),
	m_szMeshFilePath(""), m_eMeshType(TYPE_END), m_iCurAni(0)
{
}

Engine::Model::Model(const Model & rhs)
	: VIBuffer(rhs),
	m_pFaceIndices(rhs.m_pFaceIndices),
	m_pScene(rhs.m_pScene),
	m_Materials(rhs.m_Materials),
	m_PivotMatrix(rhs.m_PivotMatrix),
	m_iCurAni(rhs.m_iCurAni),
	m_strModelName(rhs.m_strModelName)

{
	for (auto& pMeshContainer : rhs.m_MeshContainers)
		m_MeshContainers.push_back(pMeshContainer->Clone());

	for (auto& pMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(pMaterial->pMeshTexture[i]);
	}
}

_bool Engine::Model::GetCurrentAnimationEnd()
{
	return m_Animations[m_iCurAni]->GetCurrentAnimtaionEnd();
}

HRESULT Engine::Model::Ready_Prototype(const char* pMeshFilePath, const char* pMeshName, const _tchar* pShaderFilePath, _fmatrix PivotMatrix, MESHTYPE eMeshType)
{
	m_isCloned = false;
	m_eMeshType = eMeshType;
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	strcpy_s(m_szMeshFilePath, pMeshFilePath);

	m_strModelName = pMeshName;

	char szFullPath[MAX_PATH] = "";
	strcpy_s(szFullPath, pMeshFilePath);
	strcat_s(szFullPath, pMeshName);

	m_pScene = m_Importer.ReadFile(szFullPath, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	if (nullptr == m_pScene)
		return E_FAIL;

	//정점 갯수 셋팅
	if (FAILED(ReserveVIData()))
		return E_FAIL;

	//정점 정보를 읽음
	if (FAILED(CreateMeshContainer()))
		return E_FAIL;

	if (FAILED(CreateMaterial()))
		return E_FAIL;

	if (FAILED(CompileShaderFiles(pShaderFilePath)))
		return E_FAIL;
	
	//애니메이션이 없으면 여기서 끝 
	if (TYPE_NONE == m_eMeshType)
	{
		if (FAILED(CreateVIBuffer()))
			return E_FAIL;

		return S_OK;
	}

	if (FAILED(CreateHierarchyNode(m_pScene->mRootNode)))
		return E_FAIL;

	//계층구조대로 정렬
	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](HierarchyNode* pSour, HierarchyNode* pDest)
	{
		return pSour->GetDepth() < pDest->GetDepth();
	});

	if (FAILED(CreateSkinned()))
		return E_FAIL;

	if (FAILED(CreateVIBuffer()))
		return E_FAIL;

	if (FAILED(CreateAnimation()))
		return E_FAIL;


	return S_OK;
}

HRESULT Engine::Model::Ready(void * pArg)
{
	m_isCloned = true;

	if (TYPE_NONE == m_eMeshType)
		return S_OK;

	if (FAILED(CreateHierarchyNode(m_pScene->mRootNode)))
		return E_FAIL;

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](HierarchyNode* pSour, HierarchyNode* pDest)
	{
		return pSour->GetDepth() < pDest->GetDepth();
	});

	if (FAILED(CreateSkinned()))
		return E_FAIL;

	if (FAILED(CreateAnimation()))
		return E_FAIL;

	return S_OK;
}

HRESULT Engine::Model::BindBuffers()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	ID3D11Buffer*	pVertexBuffers[] = { m_pVB };
	_uint			iStrides[] = { m_iStride };
	_uint			iOffsets[] = { 0 };

	//버텍스 버퍼 바인드 
	m_pDeviceContext->IASetVertexBuffers(0, 1, pVertexBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

HRESULT Engine::Model::UpdateCombinedTransformationMatrix(_double TimeDelta)
{
	//현재 애니 재생시간에 따른 뼈들의 TransformationMatrix 갱신
	
	m_Animations[m_iCurAni]->UpdateTransformationMatrix(TimeDelta);

	//부모부터 행렬 갱신 > 렌더링에 사용 
	for (auto& pHierarchy : m_HierarchyNodes)
		pHierarchy->UpdateCombinedTransformationMatrix(m_iCurAni);

	return S_OK;
}

HRESULT Engine::Model::Render(_uint iMeshContainerIndex, _uint iPassIndex)
{
	m_pDeviceContext->IASetInputLayout(m_EffectDescs[iPassIndex]->pInputLayout);

	_matrix BoneMatrices[128];
	ZeroMemory(BoneMatrices, sizeof(_matrix) * 128);

	
	if (m_MeshContainers[iMeshContainerIndex]->isBoneEmpty() && m_MeshContainers[iMeshContainerIndex]->GetParent() != nullptr)
	{
		_matrix Matrix = m_MeshContainers[iMeshContainerIndex]->GetParent()->GetCombinedMatrix();
		for (int i = 0; i < 128; i++)
		{
			BoneMatrices[i] = Matrix;
		}
	}
	else
		m_MeshContainers[iMeshContainerIndex]->SetUpBoneMatrice(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

	if (m_strModelName.find("Amenosagiri") != string::npos &&
		strcmp(m_MeshContainers[iMeshContainerIndex]->GetParent()->GetName(), "adachi_boss_lens_0_Part")==0)
		return S_OK;
		//m_MeshContainers[iMeshContainerIndex]->GetMeshName() == "adachi_boss_lens_0_Part")
	

	if (FAILED(SetUpValueOnShader("g_BoneMatrices", BoneMatrices, sizeof(_matrix) * 128)))
		return E_FAIL;

	if (FAILED(m_EffectDescs[iPassIndex]->pPass->Apply(0, m_pDeviceContext)))
		return E_FAIL;

	m_MeshContainers[iMeshContainerIndex]->Render();

	return S_OK;
}

void Engine::Model::SetUpAnimationIndex(_uint iAniIndex)
{
	m_Animations[m_iCurAni]->Reset();
	m_iCurAni = iAniIndex;
//	m_Animations[m_iCurAni]->ResetTrackPositionAcc();
}

HRESULT Engine::Model::SetUpTextureOnShader(const char * pConstantName, _uint iMeshContainerIndex, aiTextureType eTextureType)
{
	if (FAILED(__super::SetUpTextureOnShader(pConstantName, m_Materials[m_MeshContainers[iMeshContainerIndex]->GetMaterialIndex()]->pMeshTexture[eTextureType])))
		return E_FAIL;

	/*
	_uint iMaterialIndex = m_MeshContainers[iMeshContainerIndex]->GetMaterialIndex();
	if (FAILED(__super::SetUpTextureOnShader(pConstantName, m_Materials[iMaterialIndex]->pMeshTexture[eTextureType])))
		return E_FAIL;
	*/
	return S_OK;
}

HRESULT Engine::Model::ReserveVIData()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		m_iNumVertices += m_pScene->mMeshes[i]->mNumVertices;
		m_iNumPrimitive += m_pScene->mMeshes[i]->mNumFaces;
	}

	m_pVertices = new VTXMESH[m_iNumVertices];
	m_pFaceIndices = new FACEINDICES32[m_iNumPrimitive];

	ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);
	ZeroMemory(m_pFaceIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	return S_OK;
}

HRESULT Engine::Model::CreateMeshContainer()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	_uint iVertexIndex = 0;
	_uint iFaceIndex = 0;

	//메쉬 갯수 만큼
	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		//실제 메쉬
		aiMesh* pMesh = m_pScene->mMeshes[i];

		MeshContainer::MESHDESC	MeshDesc;
		ZeroMemory(&MeshDesc, sizeof(MeshDesc));

		MeshDesc.iNumFaces = pMesh->mNumFaces;
		MeshDesc.iStartVertexIndex = iVertexIndex;
		MeshDesc.iStartFaceIndex = iFaceIndex;
		MeshDesc.iMaterialIndex = pMesh->mMaterialIndex;
		MeshDesc.strName = pMesh->mName.C_Str();

		MeshContainer*		pMeshContainer = MeshContainer::Create(m_pDevice, m_pDeviceContext, MeshDesc);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_MeshContainers.push_back(pMeshContainer);

		for (_uint j = 0; j < pMesh->mNumVertices; ++j)
		{
			_vector vPosition;
			memcpy(&vPosition, &pMesh->mVertices[j], sizeof(_float3));

			if(TYPE_NONE == m_eMeshType) //애니메이션이 없는 경우에만 정점에 피봇매트릭스 적용 
				vPosition = XMVector3TransformCoord(vPosition, XMLoadFloat4x4(&m_PivotMatrix));
	
			XMStoreFloat3(&((VTXMESH*)m_pVertices)[iVertexIndex].vPosition, vPosition);

			memcpy(&((VTXMESH*)m_pVertices)[iVertexIndex].vNormal, &pMesh->mNormals[j], sizeof(_float3));
			memcpy(&((VTXMESH*)m_pVertices)[iVertexIndex].vTexUV, &pMesh->mTextureCoords[0][j], sizeof(_float2));
			memcpy(&((VTXMESH*)m_pVertices)[iVertexIndex].vTangent, &pMesh->mTangents[j], sizeof(_float3));
			++iVertexIndex;
		}

		for (_uint j = 0; j < pMesh->mNumFaces; ++j)
		{
			m_pFaceIndices[iFaceIndex]._0 = pMesh->mFaces[j].mIndices[0];
			m_pFaceIndices[iFaceIndex]._1 = pMesh->mFaces[j].mIndices[1];
			m_pFaceIndices[iFaceIndex]._2 = pMesh->mFaces[j].mIndices[2];
			++iFaceIndex;
		}
	}
	return S_OK;
}

HRESULT Engine::Model::CreateMaterial()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	m_Materials.reserve(m_pScene->mNumMaterials);

	char		szMeshFilePath[MAX_PATH] = "";

	for (_uint i = 0; i < m_pScene->mNumMaterials; ++i)
	{
		aiMaterial*		pMaterial = m_pScene->mMaterials[i];

		MATERIAL*	pMeshMaterial = new MATERIAL;
		ZeroMemory(pMeshMaterial, sizeof(MATERIAL));

		for (_uint j = aiTextureType_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString	strFilePath;

			if (FAILED(pMaterial->GetTexture(aiTextureType(j), 0, &strFilePath)))
				continue;

			char	szFileName[MAX_PATH] = "";
			char	szExt[MAX_PATH] = "";
			_splitpath_s(strFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szMeshFilePath, m_szMeshFilePath);
			strcat_s(szMeshFilePath, szFileName);
			strcat_s(szMeshFilePath, szExt);

			_tchar		szFullName[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szMeshFilePath, strlen(szMeshFilePath), szFullName, MAX_PATH);

			pMeshMaterial->pMeshTexture[j] = Texture::Create(m_pDevice, m_pDeviceContext, szFullName);
			if (nullptr == pMeshMaterial->pMeshTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(pMeshMaterial);
	}

	return S_OK;
}

HRESULT Engine::Model::CreateVIBuffer()
{
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	// D3D11_BUFFER_DESC 
	m_iStride = sizeof(VTXMESH);
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	// D3D11_SUBRESOURCE_DATA
	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubresourceData.pSysMem = m_pVertices;

	if (FAILED(CreateVertexBuffer()))
		return E_FAIL;

	// D3D11_BUFFER_DESC 
	m_IndicesByte = sizeof(FACEINDICES32);
	m_iNumIndicesPrimitive = 3;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eFormat = DXGI_FORMAT_R32_UINT;

	m_IBDesc.ByteWidth = m_IndicesByte * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	// D3D11_SUBRESOURCE_DATA
	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubresourceData.pSysMem = m_pFaceIndices;

	if (FAILED(CreateIndexBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT Engine::Model::CompileShaderFiles(const _tchar * pShaderFilePath)
{
	D3D11_INPUT_ELEMENT_DESC		ElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	if (FAILED(__super::CompileShaderFiles(pShaderFilePath, ElementDescs, 6)))
		return E_FAIL;

	return S_OK;
}

HRESULT Engine::Model::CreateHierarchyNode(aiNode* pNode, HierarchyNode* pParent, _uint iDepth)
{
	_matrix	TransformationMatrix;
	memcpy(&TransformationMatrix, &pNode->mTransformation, sizeof(_matrix));

	HierarchyNode*	pHierarchyNode = HierarchyNode::Create(m_pDevice, m_pDeviceContext, pNode->mName.data, TransformationMatrix, iDepth, pParent);
	if (nullptr == pHierarchyNode)
		return E_FAIL;

	//
	if (pNode->mNumMeshes > 0 && m_isCloned)
	{
		for (int i = 0; i < pNode->mNumMeshes; ++i)
		{
			if (m_MeshContainers[pNode->mMeshes[i]]->GetParent() == nullptr)
			{
				m_MeshContainers[pNode->mMeshes[i]]->SetParent(pHierarchyNode);
			}
			else
			{
				if (m_MeshContainers[pNode->mMeshes[i]]->GetParent()->GetDepth() == iDepth)
				{
					MeshContainer* duplicate = new MeshContainer(*m_MeshContainers[pNode->mMeshes[i]]);
					duplicate->SetParent(pHierarchyNode);

					m_MeshContainers.push_back(duplicate);
				}
			}
		}
	}


	pHierarchyNode->ResizeChannel(m_pScene->mNumAnimations);

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
		CreateHierarchyNode(pNode->mChildren[i], pHierarchyNode, iDepth + 1);

	return S_OK;
}

HRESULT Engine::Model::CreateSkinned()
{
	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		aiMesh*	pMesh = m_pScene->mMeshes[i];
		MeshContainer* pMeshContainer = m_MeshContainers[i];

		for (_uint j = 0; j < pMesh->mNumBones; ++j)
		{
			aiBone*		pBone = pMesh->mBones[j];

			BONEDESC*	pBoneDesc = new BONEDESC;
			_matrix		OffsetMatrix;

			OffsetMatrix = XMMatrixTranspose(_matrix(pBone->mOffsetMatrix[0]));
			XMStoreFloat4x4(&pBoneDesc->OffsetMatrix, OffsetMatrix);

			pBoneDesc->pNode = FindHierarchyNode(pBone->mName.data);
			if (nullptr == pBoneDesc->pNode)
				return E_FAIL;
			
			pBoneDesc->pNode->SetOffset(OffsetMatrix);
			pMeshContainer->AddBoneDesc(pBoneDesc);

			if (m_isCloned)
				continue;

			for (_uint k = 0; k < pBone->mNumWeights; ++k)
			{
				MeshContainer::MESHDESC MeshDesc = pMeshContainer->GetMeshDesc();
				_uint index = MeshDesc.iStartVertexIndex + pBone->mWeights[k].mVertexId;

				if (0.0f == ((VTXMESH*)m_pVertices)[index].vBlendWeight.x) 
				{
					((VTXMESH*)m_pVertices)[index].vBlendIndex.x = j;
					((VTXMESH*)m_pVertices)[index].vBlendWeight.x = pBone->mWeights[k].mWeight;
				}
				else if (0.0f == ((VTXMESH*)m_pVertices)[index].vBlendWeight.y)
				{
					((VTXMESH*)m_pVertices)[index].vBlendIndex.y= j;
					((VTXMESH*)m_pVertices)[index].vBlendWeight.y = pBone->mWeights[k].mWeight;
				}
				else if (0.0f == ((VTXMESH*)m_pVertices)[index].vBlendWeight.z)
				{
					((VTXMESH*)m_pVertices)[index].vBlendIndex.z = j;
					((VTXMESH*)m_pVertices)[index].vBlendWeight.z = pBone->mWeights[k].mWeight;
				}
				else
				{
					((VTXMESH*)m_pVertices)[index].vBlendIndex.w = j;
					((VTXMESH*)m_pVertices)[index].vBlendWeight.w = pBone->mWeights[k].mWeight;
				}
			}
		}
	}

	return S_OK;
}

HRESULT Engine::Model::CreateAnimation()
{
 	for (_uint i = 0; i < m_pScene->mNumAnimations; ++i)
	{
		aiAnimation* pAiAni = m_pScene->mAnimations[i];

		Animation* pAni = Animation::Create(pAiAni->mName.data, pAiAni->mDuration, pAiAni->mTicksPerSecond);
		if (nullptr == pAni)
			return E_FAIL;

		//현재 애니에 영향을 주는 뼈들
		for (_uint j = 0; j < pAiAni->mNumChannels; ++j)
		{
			aiNodeAnim* pNodeAnim = pAiAni->mChannels[j];
			HierarchyNode*	pHierarchyNode = FindHierarchyNode(pNodeAnim->mNodeName.data);
			AnimChannel* pChannel = AnimChannel::Create(pNodeAnim->mNodeName.data);
			if (nullptr == pChannel)
				return E_FAIL;
		
			//최대 키프레임갯수 구하기
			_uint iNumKeyframes = max(pNodeAnim->mNumPositionKeys, pNodeAnim->mNumRotationKeys);
			iNumKeyframes = max(iNumKeyframes, pNodeAnim->mNumScalingKeys);

	//		if (FAILED(pChannel->ReserveKeyFrames(iNumKeyframes)))
	//			return E_FAIL;

			_vector			vScale, vRotation, vPosition;
			ZeroMemory(&vScale, sizeof(_vector));
			ZeroMemory(&vRotation, sizeof(_vector));
			ZeroMemory(&vPosition, sizeof(_vector));

			for (_uint k = 0; k < iNumKeyframes; ++k)
			{
				KEYFRAME* pKeyFrame = new KEYFRAME;
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

				if (pNodeAnim->mNumPositionKeys > k)
				{
					memcpy(&vPosition, &pNodeAnim->mPositionKeys[k].mValue, sizeof(_float3));
					pKeyFrame->Time = pNodeAnim->mPositionKeys[k].mTime;
					vPosition = XMVectorSetW(vPosition, 1.f);
				}
				if (pNodeAnim->mNumScalingKeys > k)
				{
					memcpy(&vScale, &pNodeAnim->mScalingKeys[k].mValue, sizeof(_float3));
					pKeyFrame->Time = pNodeAnim->mScalingKeys[k].mTime;
				}
				if (pNodeAnim->mNumRotationKeys > k)
				{
					vRotation = XMVectorSetX(vRotation, pNodeAnim->mRotationKeys[k].mValue.x);
					vRotation = XMVectorSetY(vRotation, pNodeAnim->mRotationKeys[k].mValue.y);
					vRotation = XMVectorSetZ(vRotation, pNodeAnim->mRotationKeys[k].mValue.z);
					vRotation = XMVectorSetW(vRotation, pNodeAnim->mRotationKeys[k].mValue.w);

					pKeyFrame->Time = pNodeAnim->mRotationKeys[k].mTime;
				}

				memcpy(&pKeyFrame->vScale, &vScale, sizeof(_float3));
				memcpy(&pKeyFrame->vRotation, &vRotation, sizeof(_float4));
				memcpy(&pKeyFrame->vPosition, &vPosition, sizeof(_float3));

				pChannel->AddKeyFrame(pKeyFrame);
			}
			pHierarchyNode->AddChannel(i, pChannel);
			pAni->AddChannel(pChannel);
		}
 		m_Animations.push_back(pAni);
	}
	return S_OK;
}

void Engine::Model::SetHierarchyNode(const aiScene * scene, aiNode * node, HierarchyNode * parents, _uint depth)
{
}

HierarchyNode * Engine::Model::FindHierarchyNode(const char * pName)
{
	auto iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](HierarchyNode* pNode)
	{
		return !strcmp(pNode->GetName(), pName);
	});

	if(iter == m_HierarchyNodes.end())
		return nullptr;

	return (*iter);
}

Model * Engine::Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char * pMeshFilePath, const char * pMeshName, const _tchar* pShaderFilePath, _fmatrix PivotMatrix, MESHTYPE eMeshType)
{
	Model* pInstance = new Model(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype(pMeshFilePath, pMeshName, pShaderFilePath,PivotMatrix,eMeshType)))
	{
		MSGBOX("Failed to Creating Model");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Component * Engine::Model::Clone(void * pArg)
{
	Model* pInstance = new Model(*this);
	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating Model Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::Model::Free()
{
	__super::Free();

	if (!m_isCloned)
	{
		Safe_Delete_Array(m_pFaceIndices);
		//m_Importer.FreeScene();
	}

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	for (auto& pNode : m_HierarchyNodes)
		Safe_Release(pNode);

	m_HierarchyNodes.clear();

	for (auto& pMeshContainer : m_MeshContainers)
		Safe_Release(pMeshContainer);
	m_MeshContainers.clear();

	for (auto& pMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(pMaterial->pMeshTexture[i]);

		if(!m_isCloned)
			Safe_Delete(pMaterial);
	}
	m_Materials.clear();
}
