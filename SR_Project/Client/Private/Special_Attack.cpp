#include "stdafx.h"
#include "Special_Attack.h"
#include "GameInstance.h"
#include "PickingMgr.h"

CSpecial_Attack::CSpecial_Attack(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CSpecial_Attack::CSpecial_Attack(const CSpecial_Attack & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSpecial_Attack::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpecial_Attack::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	WalkingTerrain();

	return S_OK;
}


int CSpecial_Attack::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bDead)
		return OBJ_DEAD;

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CSpecial_Attack::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	Change_Motion();
	Change_Frame();
}

HRESULT CSpecial_Attack::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_pTextureCom->Get_Frame().m_iCurrentTex)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}


HRESULT CSpecial_Attack::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//m_TimerTag = TEXT("Timer_Attack_Ring");
	//if (FAILED(pGameInstance->Add_Timer(m_TimerTag)))
	//return E_FAIL;

	Safe_Release(pGameInstance);

	/* For.Com_Texture */
	Texture_Clone();

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(0.f);
	TransformDesc.InitPos = *(_float3*)pArg;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpecial_Attack::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CSpecial_Attack::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CSpecial_Attack::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);  // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

void CSpecial_Attack::WalkingTerrain()
{
	_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (!pGameInstance)
		return;
	CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
	if (!pVIBuffer_Terrain)
		return;
	CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
	if (!pTransform_Terrain)
		return;

	_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vScale = m_pTransformCom->Get_Scale();
	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), (1 * vScale.y *0.5f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

HRESULT CSpecial_Attack::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

HRESULT CSpecial_Attack::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 60;

	TextureDesc.m_iEndTex = 17;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Attack_Ring"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Attack_Ring"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CSpecial_Attack::Change_Frame()
{
	if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
		m_bDead = true;
}

void CSpecial_Attack::Change_Motion()
{
}

CSpecial_Attack* CSpecial_Attack::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSpecial_Attack* pInstance = new CSpecial_Attack(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CSpecial_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpecial_Attack::Clone(void* pArg)
{
	CSpecial_Attack* pInstance = new CSpecial_Attack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CSpecial_Attack "));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CSpecial_Attack::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}