#include "stdafx.h"
#include "Boarrior.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"
#include "Item.h"
#include "CameraManager.h"

CBoarrior::CBoarrior(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CBoarrior::CBoarrior(const CBoarrior & rhs)
	: CMonster(rhs)
{
}

HRESULT CBoarrior::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoarrior::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(4.f, 4.f, 1.f);

	m_tInfo.iMaxHp = 1000;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;
	m_tInfo.fDamage = 20.f;

	m_fAggroRadius = 6.f;
	m_fAttackRadius = 2.f;
	m_fSpecialAttackRadius = 4.f;

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
	return S_OK;
}

int CBoarrior::Tick(_float fTimeDelta)
{
	if (__super::Tick(fTimeDelta) && m_bDeadAnimExpired)
		return OBJ_DEAD;

	if (m_bShouldSpawnBullet)
		Spawn_Bullet(fTimeDelta);

	// A.I.
	AI_Behaviour(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CBoarrior::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Change_Motion();
	Change_Frame(fTimeDelta);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
}

HRESULT CBoarrior::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	m_pColliderCom->Render_ColliderBox();
#endif // _DEBUG

	return S_OK;
}

HRESULT CBoarrior::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

	/* For.Com_Texture */
	Texture_Clone();

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.InitPos = *(_float3*)pArg;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider*/
	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));
	
	CollRectDesc.fRadiusY = .2f;
	CollRectDesc.fRadiusX = .2f;
	CollRectDesc.fRadiusZ = .8f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = -1.f;
	CollRectDesc.fOffsetZ = 0.f;

	/* For.Com_Collider_Rect*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;

	SetUp_DebugComponents(pArg);

	return S_OK;
}

HRESULT CBoarrior::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 30;
	
	TextureDesc.m_iEndTex = 25;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_1_DOWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Attack_1_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 25;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_1_SIDE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Attack_1_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 25;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_1_UP"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Attack_1_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 44;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_2"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Attack_2"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 57;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_3_DOWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Attack_3_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 57;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_3_SIDE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Attack_3_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 57;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_3_UP"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Attack_3_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 15;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_DASH_DOWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Dash_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 15;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_DASH_SIDE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Dash_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 15;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_DASH_UP"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Dash_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 92;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_DEATH"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Death"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 16;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_HIT_DOWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Hit_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 16;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_HIT_SIDE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Hit_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 16;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_HIT_UP"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Hit_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 31;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_DOWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Idle_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 31;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_SIDE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Idle_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 31;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_UP"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Idle_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 6;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_STUN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Stun"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 36;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_WALK_DOWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Walk_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 36;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_WALK_SIDE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Walk_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 36;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_WALK_UP"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Walk_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CBoarrior::Change_Frame(_float fTimeDelta)
{
	switch (m_eState)
	{
	case STATE::IDLE:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-4.f, 4.f, 1.f);
		else
			m_pTransformCom->Set_Scale(4.f, 4.f, 1.f);

		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case STATE::WALK:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-4.f, 4.f, 1.f);
		else
			m_pTransformCom->Set_Scale(4.f, 4.f, 1.f);

		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case STATE::DASH:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-4.f, 4.f, 1.f);
		else
			m_pTransformCom->Set_Scale(4.f, 4.f, 1.f);

		if ((m_pTextureCom->MoveFrame(m_TimerTag)) == true)
			m_eState = IDLE;
		break;
	case STATE::ATTACK_1:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-4.f, 4.f, 1.f);
		else
			m_pTransformCom->Set_Scale(4.f, 4.f, 1.f);

		Attack(fTimeDelta);
		break;
	case STATE::ATTACK_2:
		Attack(fTimeDelta, m_eState);
		break;
	case STATE::ATTACK_3:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-4.f, 4.f, 1.f);
		else
			m_pTransformCom->Set_Scale(4.f, 4.f, 1.f);

		Attack(fTimeDelta, m_eState);
		break;
	case STATE::STUN:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATE::IDLE;

		break;
	case STATE::HIT:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-4.f, 4.f, 1.f);
		else
			m_pTransformCom->Set_Scale(4.f, 4.f, 1.f);

		if ((m_pTextureCom->MoveFrame(m_TimerTag, false) == true))
			m_bHit = false;
		break;
	case STATE::DIE:
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 74)
			Drop_Items();
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 92)
			m_bDeadAnimExpired = true;

		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	}
}

void CBoarrior::Change_Motion()
{
	if (m_eState != m_ePreState || m_eDir != m_ePreDir)
	{
		switch (m_eState)
		{
		case STATE::IDLE:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_IDLE_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_IDLE_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_IDLE_SIDE"));
				break;
			}

			if (m_eDir != m_ePreDir)
				m_ePreDir = m_eDir;
			break;
		case STATE::WALK:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_WALK_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_WALK_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_WALK_SIDE"));
				break;
			}

			if (m_eDir != m_ePreDir)
				m_ePreDir = m_eDir;
			break;
		case STATE::DASH:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_DASH_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_DASH_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_DASH_SIDE"));
				break;
			}

			if (m_eDir != m_ePreDir)
				m_ePreDir = m_eDir;
			break;
		case STATE::ATTACK_1:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_ATTACK_1_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_ATTACK_1_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_ATTACK_1_SIDE"));
				break;
			}

			if (m_eDir != m_ePreDir)
				m_ePreDir = m_eDir;
			break;
		case STATE::ATTACK_2:
			Change_Texture(TEXT("Com_Texture_ATTACK_2"));
			break;
		case STATE::ATTACK_3:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_ATTACK_3_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_ATTACK_3_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_ATTACK_3_SIDE"));
				break;
			}

			if (m_eDir != m_ePreDir)
				m_ePreDir = m_eDir;
			break;
		case STATE::STUN:
			Change_Texture(TEXT("Com_Texture_STUN"));
			break;
		case STATE::HIT:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_HIT_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_HIT_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_HIT_SIDE"));
				break;
			}

			if (m_eDir != m_ePreDir)
				m_ePreDir = m_eDir;
			break;
		case STATE::DIE:
			Change_Texture(TEXT("Com_Texture_DEATH"));
			break;
		}

		if (m_eState != m_ePreState)
			m_ePreState = m_eState;
	}
}

void CBoarrior::AI_Behaviour(_float fTimeDelta)
{
	if (m_bDead || m_bHit || m_bIsAttacking)
	{
		if (m_bHit)
			m_eState = STATE::HIT;

		return;
	}

	Find_Target();  // Check: pTarget, bAggro, fDistanceToTarget

	if (m_pTarget && m_bAggro)
	{
		// Randomly choose a Pattern (if not already chosen)
		if (!m_iPattern)
			m_iPattern = rand() % 3 + 1;

		// Set Attack Radius based on Pattern chosen before
		_float fRadius;
		switch (m_iPattern)
		{
		case 1:
			fRadius = m_fAttackRadius;
			break;
		case 2:
		case 3:
			fRadius = m_fSpecialAttackRadius;
			break;
		default:
			fRadius = m_fAttackRadius;
		}

		Calculate_Direction(m_pTarget->Get_Position());

		// If in AttackRadius: Attack
		if (m_fDistanceToTarget < fRadius)
		{
			if (GetTickCount() > m_dwAttackTime + 1500)
			{
				if (m_iPattern == 1 || m_iPattern == 2 || m_iPattern == 3)
				{
					m_eState = m_iPattern == 1 ? STATE::ATTACK_1 : m_iPattern == 2 ? STATE::ATTACK_2 : STATE::ATTACK_3;
					m_bIsAttacking = true;
					m_fFollowTime = 0.f;
				}
			}
			else
				m_eState = STATE::IDLE;
		}
		// If NOT in AttackRadius: Follow Target
		else
			Follow_Target(fTimeDelta);
	}
	else
		Patrol(fTimeDelta);
}

void CBoarrior::Patrol(_float fTimeDelta)
{
	// Switch between Idle and Walk (based on time)
	if (m_eState == STATE::IDLE)
	{
		if (GetTickCount() > m_dwIdleTime + 3000)
		{
			m_eState = STATE::WALK;
			m_dwWalkTime = GetTickCount();

			// Find Random Patroling Position
			_float fOffsetX = ((_float)rand() / (float)(RAND_MAX)) * m_fPatrolRadius;
			_int bSignX = rand() % 2;
			_float fOffsetZ = ((_float)rand() / (float)(RAND_MAX)) * m_fPatrolRadius;
			_int bSignZ = rand() % 2;
			m_fPatrolPosX = bSignX ? (m_pTransformCom->Get_TransformDesc().InitPos.x + fOffsetX) : (m_pTransformCom->Get_TransformDesc().InitPos.x - fOffsetX);
			m_fPatrolPosZ = bSignZ ? (m_pTransformCom->Get_TransformDesc().InitPos.z + fOffsetZ) : (m_pTransformCom->Get_TransformDesc().InitPos.z - fOffsetZ);
		}
	}
	else if (m_eState == STATE::WALK)
	{
		if (GetTickCount() > m_dwWalkTime + 1500)
		{
			m_eState = STATE::IDLE;
			m_dwIdleTime = GetTickCount();
		}
	}

	// Movement
	if (m_eState == STATE::WALK)
	{
		// Adjust PatrolPosition Y
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		if (!pGameInstance)
			return;
		CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_BOSS, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
		if (!pVIBuffer_Terrain)
			return;
		CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(LEVEL_BOSS, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
		if (!pTransform_Terrain)
			return;

		_float3 vPatrolPosition = { m_fPatrolPosX, Get_Position().y, m_fPatrolPosZ };
		_float3 vScale = m_pTransformCom->Get_Scale();

		vPatrolPosition.y = pVIBuffer_Terrain->Compute_Height(vPatrolPosition, pTransform_Terrain->Get_WorldMatrix(), (1 * vScale.y / 2));

		Calculate_Direction(vPatrolPosition);

		m_pTransformCom->Go_PosTarget(fTimeDelta * .1f, _float3(m_fPatrolPosX, Get_Position().y, m_fPatrolPosZ), _float3{ 0.f, 0.f, 0.f });
	}
}

void CBoarrior::Find_Target()
{
	if (m_bDead)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pTarget);

	if (pPlayer)
	{
		if (pPlayer->Get_Dead())
		{
			if (m_bAggro)
			{
				m_pTarget = nullptr;
				m_eState = STATE::IDLE;
				m_bAggro = false;
			}
			return;
		}
			
		if (pTarget)
		{
			_float3 vTargetPos = pTarget->Get_Position();
			m_fDistanceToTarget = D3DXVec3Length(&(vTargetPos - Get_Position()));

			if (m_fDistanceToTarget < m_fAggroRadius || m_bAggro)
			{
				// Set Target and Aggro
				m_pTarget = pTarget;
				m_bAggro = true;
			}
		}
	}
}

void CBoarrior::Follow_Target(_float fTimeDelta)
{
	if (m_fFollowTime < 3.f)
	{
		m_fFollowTime += fTimeDelta;
	
		// Set State 
		m_eState = STATE::WALK;

		_float3 fTargetPos = m_pTarget->Get_Position();
		m_pTransformCom->Go_PosTarget(fTimeDelta * .1f, fTargetPos, _float3(0, 0, 0));

		m_bIsAttacking = false;
	}
	else
	{
		m_iPattern = rand() % 2 + 2;
		m_fFollowTime = 0.f;
	}
}

void CBoarrior::Attack(_float fTimeDelta, STATE eAttack)
{
	if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
	{
		// Resetting Variables
		m_bIsAttacking = false;
		m_dwAttackTime = GetTickCount();
		m_bDidDamage = false;
		m_iPattern = 0; 
		m_vAttackPos = _float3(0.f, 0.f, 0.f);
	}
	else 
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

		switch (eAttack)
		{
		case STATE::ATTACK_1:
		{
			// Normal Attack
			if (m_pTextureCom->Get_Frame().m_iCurrentTex == 5 && !m_bDidDamage)
			{
				// Create Standard Bullet
				BULLETDATA BulletData;
				ZeroMemory(&BulletData, sizeof(BulletData));

				BulletData.bIsPlayerBullet = false;
				BulletData.vPosition = m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3];
				BulletData.eDirState = Get_Unprocessed_Dir(m_eDir);
				BulletData.fOffsetSide = 1.f;
				BulletData.fOffsetUp = 1.f;
				BulletData.fOffsetDown = 1.f;

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
					return;

				m_bDidDamage = true;
			}
		}
		break;
		case STATE::ATTACK_2:
		{
			// Whirling Attack

			// Move towards Player
			if (m_vAttackPos == _float3(0.f, 0.f, 0.f))
				m_vAttackPos = m_pTarget->Get_Position();
			
			m_pTransformCom->Go_PosTarget(fTimeDelta, m_vAttackPos, _float3(0, 0, 0));

			// If did NOT Apply Damage yet
			if (!m_bDidDamage)
			{
				// If Spider is Colliding with Player 
				vector<CGameObject*> vecDamagedActor;
				if (pGameInstance->Collision_Check_Group_Multi(CCollider_Manager::COLLISION_PLAYER, vecDamagedActor, this, CCollider_Manager::COLLSIION_BOX))
				{
					CGameInstance::Apply_Damage_Multi(m_tInfo.fDamage, vecDamagedActor, this, nullptr);
					m_bDidDamage = true;
				}
			}
		}
		break;
		case STATE::ATTACK_3:
		{
			if (m_pTextureCom->Get_Frame().m_iCurrentTex == 38 && !m_bShouldSpawnBullet)
			{
				m_bShouldSpawnBullet = true;

				CCameraDynamic* pCamera = dynamic_cast<CCameraDynamic*>(CCameraManager::Get_Instance()->Get_CurrentCamera());
				if (pCamera)
					pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.7f, 0.2f, 0.01f);
			}

			break;
		}
		}
	}
}

void CBoarrior::Spawn_Bullet(_float fTimeDelta)
{
	if (m_fBulletAliveTime < .05f)
		m_fBulletAliveTime += fTimeDelta;
	else
	{
		m_fBulletAliveTime = 0.f;
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

		// Special Attack
		BULLETDATA BulletData1;
		BULLETDATA BulletData2;
		ZeroMemory(&BulletData1, sizeof(BulletData1));
		ZeroMemory(&BulletData2, sizeof(BulletData2));

		BulletData1.bIsPlayerBullet = false;
		BulletData2.bIsPlayerBullet = false;
		BulletData1.eWeaponType = WEAPON_TYPE::BOARRIOR_SPECIAL;
		BulletData2.eWeaponType = WEAPON_TYPE::BOARRIOR_SPECIAL;
		BulletData1.vPosition = (_float3)m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3];
		BulletData2.vPosition = (_float3)m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3];

		_float fForwardOffset = 0.75f;
		_float fSideOffset = 0.25f;

		switch (Get_Unprocessed_Dir(m_eDir))
		{
		case DIR_STATE::DIR_DOWN:
			BulletData1.vPosition -= _float3(fSideOffset, 0.f, 1.f + fForwardOffset * m_iBulletCount);
			BulletData2.vPosition -= _float3(-fSideOffset, 0.f, 1.f + fForwardOffset * m_iBulletCount);
			break;
		case DIR_STATE::DIR_RIGHT:
			BulletData1.vPosition += _float3(1.f + fForwardOffset * m_iBulletCount, 0.f, -fSideOffset);
			BulletData2.vPosition += _float3(1.f + fForwardOffset * m_iBulletCount, 0.f, fSideOffset);
			break;
		case DIR_STATE::DIR_UP:
			BulletData1.vPosition += _float3(-fSideOffset, 0.f, 1.f + fForwardOffset * m_iBulletCount);
			BulletData2.vPosition += _float3(fSideOffset, 0.f, 1.f + fForwardOffset * m_iBulletCount);
			break;
		case DIR_STATE::DIR_LEFT:
			BulletData1.vPosition -= _float3(1.f + fForwardOffset * m_iBulletCount, 0.f, fSideOffset);
			BulletData2.vPosition -= _float3(1.f + fForwardOffset * m_iBulletCount, 0.f, -fSideOffset);
			break;
		}

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData1)))
			return;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData2)))
			return;

		m_iBulletCount += 1;

		if (m_iBulletCount == 5)
		{
			m_bShouldSpawnBullet = false;
			m_iBulletCount = 0;
		}	
	}
}

_float CBoarrior::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	_float fDmg = __super::Take_Damage(fDamage, DamageType, DamageCauser);

	if (fDmg > 0)
	{
		if (!m_bDead)
		{
			if (m_fStaggerDamage > m_fStaggerDamageLimit)
			{
				m_bHit = true;
				
				m_bIsAttacking = false;
				m_bAggro = true;
				m_vAttackPos = _float3(0.f, 0.f, 0.f);
				m_dwAttackTime = GetTickCount();

				m_fStaggerDamage = 0.f;
			}
			else
				m_fStaggerDamage += fDamage;
		}
	}

	return fDmg;
}

HRESULT CBoarrior::Drop_Items()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CItem::ITEMDESC ItemDesc;
	ZeroMemory(&ItemDesc, sizeof(CItem::ITEMDESC));

	// Random Position Drop based on Object Position
	_float fOffsetX = ((_float)rand() / (float)(RAND_MAX)) * .5f;
	_int bSignX = rand() % 2;
	_float fOffsetZ = ((_float)rand() / (float)(RAND_MAX)) * .5f;
	_int bSignZ = rand() % 2;
	_float fPosX = bSignX ? (Get_Position().x + fOffsetX) : (Get_Position().x - fOffsetX);
	_float fPosZ = bSignZ ? (Get_Position().z + fOffsetZ) : (Get_Position().z - fOffsetZ);

	ItemDesc.fPosition = _float3(fPosX, Get_Position().y, fPosZ);
	ItemDesc.pTextureComponent = TEXT("Com_Texture_Spider_Meat");
	ItemDesc.pTexturePrototype = TEXT("Prototype_Component_Texture_Equipment_front");
	ItemDesc.eItemName = ITEMNAME::ITEMNAME_SPIDERMEAT;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Item"), LEVEL_GAMEPLAY, TEXT("Layer_Object"), &ItemDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

_bool CBoarrior::IsDead()
{
	if (m_bDead && m_eState == STATE::DIE && GetTickCount() > m_dwDeathTime + 1500)
		return true;
	else if (m_bDead && m_eState != STATE::DIE)
	{
		m_dwDeathTime = GetTickCount();
		m_eState = STATE::DIE;
	}

	return false;
}

CBoarrior* CBoarrior::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBoarrior* pInstance = new CBoarrior(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBoarrior"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoarrior::Clone(void* pArg)
{
	CBoarrior* pInstance = new CBoarrior(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBoarrior"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoarrior::Free()
{
	__super::Free();
}