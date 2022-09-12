#include "stdafx.h"
#include "Bearger.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"
#include "Item.h"
#include "Carrot.h"
#include "CameraManager.h"

CBearger::CBearger(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CBearger::CBearger(const CBearger & rhs)
	: CMonster(rhs)
{
}

HRESULT CBearger::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBearger::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(4.f, 4.f, 1.f);

	m_tInfo.iMaxHp = 100;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

	m_fAggroRadius = 4.f;
	m_fAttackRadius = 2.f; 
	m_fEatRadius = 2.f;
	m_fCameraShakeRadius = 7.f;

	// Set this as final patrol position
	vPatrolPosition = _float3(40.f, 1.f, 27.f);
	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
	return S_OK;
}

int CBearger::Tick(_float fTimeDelta)
{
	if (__super::Tick(fTimeDelta))
		return OBJ_DEAD;

	// A.I.
	AI_Behaviour(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CBearger::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	Change_Motion();
	Change_Frame(fTimeDelta);

	// Reset Eat Animation
	if (m_eState == STATE::EAT)
	{
		fEatTimer += fTimeDelta;
		if (fEatTimer > 1.5f)
		{
			m_eState = STATE::POST_EAT;
			fEatTimer = 0.f;
		}
	}

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

}

HRESULT CBearger::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	m_pColliderCom->Render_ColliderBox();
#endif // _DEBUG

	return S_OK;
}

HRESULT CBearger::SetUp_Components(void* pArg)
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
	CollRectDesc.fRadiusY = 0.25f;
	CollRectDesc.fRadiusX = 0.25f;
	CollRectDesc.fRadiusZ = 0.5f;
	CollRectDesc.fOffSetX = 0.0f;
	CollRectDesc.fOffSetY = -1.4f;
	CollRectDesc.fOffsetZ = 0.0f;

	/* For.Com_Collider_Rect*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;


	SetUp_DebugComponents(pArg);

	return S_OK;
}

HRESULT CBearger::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 60;

	TextureDesc.m_iEndTex = 58;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_DOWN"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Attack_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 58;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_SIDE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Attack_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 58;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_UP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Attack_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 16;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_CHARGE_DOWN"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Charge_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 16;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_CHARGE_SIDE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Charge_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 16;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_CHARGE_UP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Charge_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 58;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_DEATH"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Death"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 32;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_EAT"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Eat"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 16;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_EAT_POST"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Eat_Post"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 23;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_EAT_PRE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Eat_Pre"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 37;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_GROUND_POUND_DOWN"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Ground_Pound_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 35;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_GROUND_POUND_SIDE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Ground_Pound_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 34;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_GROUND_POUND_UP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Ground_Pound_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 17;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_HIT"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Hit"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 37;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_AGGRO_DOWN"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Idle_Aggro_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 37;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_AGGRO_SIDE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Idle_Aggro_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 37;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_AGGRO_UP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Idle_Aggro_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 37;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_DOWN"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Idle_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 37;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_SIDE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Idle_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 37;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_UP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Idle_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 32;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_RUN_DOWN"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Run_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 32;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_RUN_SIDE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Run_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 32;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_RUN_UP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Run_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 56;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_WALK_DOWN"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Walk_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 56;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_WALK_SIDE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Walk_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 57;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_WALK_UP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Walk_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CBearger::Change_Frame(_float fTimeDelta)
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
	case STATE::IDLE_AGGRO:
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

		Check_CameraShake(m_eState);
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case STATE::RUN:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-4.f, 4.f, 1.f);
		else
			m_pTransformCom->Set_Scale(4.f, 4.f, 1.f);

		Check_CameraShake(m_eState);
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case STATE::CHARGE:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-4.f, 4.f, 1.f);
		else
			m_pTransformCom->Set_Scale(4.f, 4.f, 1.f);

		Check_CameraShake(m_eState);
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case STATE::PRE_EAT:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATE::EAT;
		else if (m_pTextureCom->Get_Frame().m_iCurrentTex == 13)
			if (m_pTarget)
				m_pTarget->Set_Dead(true);
		break;
	case STATE::POST_EAT:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
		{
			m_eState = STATE::IDLE;
			m_dwIdleTime = GetTickCount();
			m_eDir = Get_Processed_Dir(DIR_STATE::DIR_DOWN);
			m_bIsEating = false;
		}
		break;
	case STATE::EAT:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case STATE::ATTACK:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-4.f, 4.f, 1.f);
		else
			m_pTransformCom->Set_Scale(4.f, 4.f, 1.f);

		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
		{
			m_bIsAttacking = false;
			m_dwAttackTime = GetTickCount();
		}
		else
			Attack();
			
		break;
	case STATE::POUND_GROUND:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-4.f, 4.f, 1.f);
		else
			m_pTransformCom->Set_Scale(4.f, 4.f, 1.f);

		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
		{
			m_bIsAttacking = false;
			m_dwAttackTime = GetTickCount();
		}
		else 
			Attack(true);
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
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 49)
			Drop_Items();

		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	}
}

void CBearger::Change_Motion()
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
		case STATE::IDLE_AGGRO:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_IDLE_AGGRO_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_IDLE_AGGRO_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_IDLE_AGGRO_SIDE"));
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
		case STATE::RUN:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_RUN_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_RUN_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_RUN_SIDE"));
				break;
			}

			if (m_eDir != m_ePreDir)
				m_ePreDir = m_eDir;
			break;
		case STATE::CHARGE:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_CHARGE_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_CHARGE_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_CHARGE_SIDE"));
				break;
			}

			if (m_eDir != m_ePreDir)
				m_ePreDir = m_eDir;
			break;
		case STATE::PRE_EAT:
			Change_Texture(TEXT("Com_Texture_EAT_PRE"));
			break;
		case STATE::EAT:
			Change_Texture(TEXT("Com_Texture_EAT"));
			break;
		case STATE::POST_EAT:
			Change_Texture(TEXT("Com_Texture_EAT_POST"));
			break;
		case STATE::ATTACK:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_ATTACK_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_ATTACK_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_ATTACK_SIDE"));
				break;
			}

			if (m_eDir != m_ePreDir)
				m_ePreDir = m_eDir;
			break;
		case STATE::POUND_GROUND:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_GROUND_POUND_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_GROUND_POUND_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_GROUND_POUND_SIDE"));
				break;
			}

			if (m_eDir != m_ePreDir)
				m_ePreDir = m_eDir;
			break;
		case STATE::HIT:
			Change_Texture(TEXT("Com_Texture_HIT"));
			break;
		case STATE::DIE:
			Change_Texture(TEXT("Com_Texture_DEATH"));
			break;
		}

		if (m_eState != m_ePreState)
			m_ePreState = m_eState;
	}
}

void CBearger::AI_Behaviour(_float fTimeDelta)
{
	if (m_bHit)
		m_eState = STATE::HIT;

	// Check for Target, AggroRadius
	if (m_bAggro)
	{
		Find_Target(); // Find Player
		if (m_pTarget)
		{
			// If in AttackRadius: Attack
			if (m_fDistanceToTarget < m_fAttackRadius)
			{
				if (!m_bIsAttacking && GetTickCount() > m_dwAttackTime + 1500)
				{
					m_eState = rand() % 2 ? STATE::ATTACK : STATE::POUND_GROUND;
					m_bIsAttacking = true;
				}
				else if (!m_bIsAttacking)
					m_eState = STATE::IDLE_AGGRO;
			}
			// If NOT in AttackRadius: Follow Target
			else
				Follow_Target(fTimeDelta);
		}
	}
	else
	{
		if (!m_bIsEating)
			Find_Target(); // Find Food
		
		if (m_pTarget)
		{
			// If in EatRadius: Eat
			if (m_fDistanceToTarget < m_fEatRadius && !m_bIsEating)
			{
				m_eState = STATE::PRE_EAT;
				m_bIsEating = true;
			}
			else if (!m_bIsEating)
				Follow_Target(fTimeDelta, true);
		}
		else
			Patrol(fTimeDelta);
	}

	if (D3DXVec3Length(&(Get_Position() - vPatrolPosition)) < 0.1f && !m_bAggro)
		m_bAggro = true;
}

void CBearger::Patrol(_float fTimeDelta)
{
	// Switch between Idle and Walk (based on time)
	if (m_eState == STATE::IDLE)
	{
		if (GetTickCount() > m_dwIdleTime + 3000)
		{
			m_eState = STATE::WALK;
			m_dwWalkTime = GetTickCount();
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
		CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
		if (!pVIBuffer_Terrain)
			return;
		CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
		if (!pTransform_Terrain)
			return;

		_float3 vScale = m_pTransformCom->Get_Scale();
		vPatrolPosition.y = pVIBuffer_Terrain->Compute_Height(vPatrolPosition, pTransform_Terrain->Get_WorldMatrix(), (1 * vScale.y / 2));

		// Change Direction
		_float fX = vPatrolPosition.x - Get_Position().x;
		_float fZ = vPatrolPosition.z - Get_Position().z;

		// Move Horizontally
		if (abs(fX) > abs(fZ))
			if (fX > 0)
				m_eDir = Get_Processed_Dir(DIR_STATE::DIR_RIGHT);
			else
				m_eDir = Get_Processed_Dir(DIR_STATE::DIR_LEFT);
		// Move Vertically
		else
			if (fZ > 0)
				m_eDir = Get_Processed_Dir(DIR_STATE::DIR_UP);
			else
				m_eDir = Get_Processed_Dir(DIR_STATE::DIR_DOWN);

		m_pTransformCom->Go_PosTarget(fTimeDelta * .25f, _float3(vPatrolPosition.x, vPatrolPosition.y, vPatrolPosition.z), _float3{ 0.f, 0.f, 0.f });
	}
}

void CBearger::Find_Target()
{
	if (!m_bIsAttacking && !m_bHit && !m_bDead)
	{
		// Look for Player
		if (m_bAggro)
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

			CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

			Safe_Release(pGameInstance);

			if (pTarget)
			{
				_float3 vTargetPos = pTarget->Get_Position();
				m_fDistanceToTarget = D3DXVec3Length(&(Get_Position() - vTargetPos));
				m_pTarget = pTarget;
			}
			else
				m_pTarget = nullptr;
		}
		// Look for Food (Carrot)
		else
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

			list<CGameObject*>* pObjects = pGameInstance->Get_ObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Object")); // Get Objects in GAMEPLAY_LEVEL
			Safe_Release(pGameInstance);
			for (auto iter = pObjects->begin(); iter != pObjects->end(); ++iter)
			{
				if (!(*iter))
					continue;

				_float3 vObjPos = (*iter)->Get_Position();

				// Check if Object is in Eat Radius
				m_fDistanceToTarget = D3DXVec3Length(&(Get_Position() - vObjPos));
				if (m_fDistanceToTarget < m_fAggroRadius)
				{
					// If Object is a Carrot set as Target
					CCarrot* pCarrot = dynamic_cast<CCarrot*>(*iter);
					if (pCarrot)
					{
						m_pTarget = pCarrot;
						break;
					}
					else
						m_pTarget = nullptr;
				}
				else
					m_pTarget = nullptr;
			}
		}
	}
	else
		m_pTarget = nullptr;
}

void CBearger::Follow_Target(_float fTimeDelta, _bool bIsFood)
{
	// Set State 
	if (bIsFood)
		m_eState = STATE::WALK;
	else
		m_eState = m_tInfo.iCurrentHp < (m_tInfo.iMaxHp / 2) ? STATE::CHARGE : STATE::RUN;

	_float3 fTargetPos = m_pTarget->Get_Position();

	// Set Direction
	_float fX = fTargetPos.x - Get_Position().x;
	_float fZ = fTargetPos.z - Get_Position().z;

	// Move Horizontally
	if (abs(fX) > abs(fZ))
		if (fX > 0)
			m_eDir = Get_Processed_Dir(DIR_STATE::DIR_RIGHT);
		else
			m_eDir = Get_Processed_Dir(DIR_STATE::DIR_LEFT);
	// Move Vertically
	else
		if (fZ > 0)
			m_eDir = Get_Processed_Dir(DIR_STATE::DIR_UP);
		else
			m_eDir = Get_Processed_Dir(DIR_STATE::DIR_DOWN);

	m_pTransformCom->Go_PosTarget(fTimeDelta * .25f, fTargetPos, _float3(0, 0, 0));
	m_bIsAttacking = false;
}

void CBearger::Attack(_bool bIsSpecial)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	// Normal Attack
	if (!bIsSpecial && m_pTextureCom->Get_Frame().m_iCurrentTex == 33)
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
	}
	// Special Attack
	else if (bIsSpecial)
	{
		BULLETDATA BulletData;
		ZeroMemory(&BulletData, sizeof(BulletData));

		BulletData.bIsPlayerBullet = false;
		BulletData.eWeaponType = WEAPON_TYPE::BEARGER_SPECIAL;
		BulletData.eDirState = Get_Unprocessed_Dir(m_eDir);
		D3DXVec3Normalize(&BulletData.vLook, &m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		D3DXVec3Normalize(&BulletData.vRight, &m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

		// Bullet Spawn Location
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		if (!pGameInstance)
			return;
		CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
		if (!pVIBuffer_Terrain)
			return;
		CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
		if (!pTransform_Terrain)
			return;

		_float3 vSpawnPos = (_float3)m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3];
		vSpawnPos.y = pVIBuffer_Terrain->Compute_Height(vSpawnPos, pTransform_Terrain->Get_WorldMatrix());

		// Create Ring and First Wave of Rocks
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 21)
		{
			CCameraDynamic* pCamera = dynamic_cast<CCameraDynamic*>(CCameraManager::Get_Instance()->Get_CurrentCamera());
			if (pCamera)
				pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.7f, 0.2f, 0.01f);

			// Ring
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Attack_Special"), LEVEL_STATIC, TEXT("Layer_Attack"), &vSpawnPos)))
				return;

			// Rocks
			BulletData.vPosition = vSpawnPos + BulletData.vLook;
			BulletData.vPosition.y = pVIBuffer_Terrain->Compute_Height(BulletData.vPosition, pTransform_Terrain->Get_WorldMatrix());
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;
			BulletData.vPosition = vSpawnPos + BulletData.vRight;
			BulletData.vPosition.y = pVIBuffer_Terrain->Compute_Height(BulletData.vPosition, pTransform_Terrain->Get_WorldMatrix());
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;
			BulletData.vPosition = vSpawnPos - BulletData.vLook;
			BulletData.vPosition.y = pVIBuffer_Terrain->Compute_Height(BulletData.vPosition, pTransform_Terrain->Get_WorldMatrix());
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;
			BulletData.vPosition = vSpawnPos - BulletData.vRight;
			BulletData.vPosition.y = pVIBuffer_Terrain->Compute_Height(BulletData.vPosition, pTransform_Terrain->Get_WorldMatrix());
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;
		}
		// Create Second Wave of Rocks
		else if (m_pTextureCom->Get_Frame().m_iCurrentTex == 33)
		{
			// Rocks
			BulletData.vPosition = vSpawnPos + BulletData.vLook * 2;
			BulletData.vPosition.y = pVIBuffer_Terrain->Compute_Height(BulletData.vPosition, pTransform_Terrain->Get_WorldMatrix());
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;
			BulletData.vPosition = vSpawnPos + BulletData.vRight * 2;
			BulletData.vPosition.y = pVIBuffer_Terrain->Compute_Height(BulletData.vPosition, pTransform_Terrain->Get_WorldMatrix());
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;
			BulletData.vPosition = vSpawnPos - BulletData.vLook * 2;
			BulletData.vPosition.y = pVIBuffer_Terrain->Compute_Height(BulletData.vPosition, pTransform_Terrain->Get_WorldMatrix());
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;
			BulletData.vPosition = vSpawnPos - BulletData.vRight * 2;
			BulletData.vPosition.y = pVIBuffer_Terrain->Compute_Height(BulletData.vPosition, pTransform_Terrain->Get_WorldMatrix());
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;
		}
	}
}

void CBearger::Check_CameraShake(STATE eState)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	if (pPlayer)
	{
		_float fDistance = D3DXVec3Length(&(Get_Position() - pPlayer->Get_Position()));
		if (fDistance < m_fCameraShakeRadius)
		{
			CCameraDynamic* pCamera = dynamic_cast<CCameraDynamic*>(CCameraManager::Get_Instance()->Get_CurrentCamera());
			if (pCamera)
			{
				switch (eState)
				{
				case STATE::WALK:
					switch (m_eDir)
					{
					case DIR_STATE::DIR_DOWN:
						if (m_pTextureCom->Get_Frame().m_iCurrentTex == 8 || m_pTextureCom->Get_Frame().m_iCurrentTex == 37)
							pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.1f, 0.1f, 0.01f);
						break;
					case DIR_STATE::DIR_UP:
						if (m_pTextureCom->Get_Frame().m_iCurrentTex == 7 || m_pTextureCom->Get_Frame().m_iCurrentTex == 44)
							pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.1f, 0.1f, 0.01f);
						break;
					case DIR_STATE::DIR_LEFT:
					case DIR_STATE::DIR_RIGHT:
						if (m_pTextureCom->Get_Frame().m_iCurrentTex == 8 || m_pTextureCom->Get_Frame().m_iCurrentTex == 29)
							pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.1f, 0.1f, 0.01f);
						break;
					}
					break;
				case STATE::RUN:
					switch (m_eDir)
					{
					case DIR_STATE::DIR_DOWN:
						if (m_pTextureCom->Get_Frame().m_iCurrentTex == 2|| m_pTextureCom->Get_Frame().m_iCurrentTex == 16)
							pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.1f, 0.1f, 0.01f);
						break;
					case DIR_STATE::DIR_UP:
						if (m_pTextureCom->Get_Frame().m_iCurrentTex == 8 || m_pTextureCom->Get_Frame().m_iCurrentTex == 22)
							pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.1f, 0.1f, 0.01f);
						break;
					case DIR_STATE::DIR_LEFT:
					case DIR_STATE::DIR_RIGHT:
						if (m_pTextureCom->Get_Frame().m_iCurrentTex == 1 || m_pTextureCom->Get_Frame().m_iCurrentTex == 15)
							pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.1f, 0.1f, 0.01f);
						break;
					}
					break;
				case STATE::CHARGE:
					switch (m_eDir)
					{
					case DIR_STATE::DIR_DOWN:
						if (m_pTextureCom->Get_Frame().m_iCurrentTex == 2 || m_pTextureCom->Get_Frame().m_iCurrentTex == 10)
							pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.1f, 0.1f, 0.01f);
						break;
					case DIR_STATE::DIR_UP:
						if (m_pTextureCom->Get_Frame().m_iCurrentTex == 4 || m_pTextureCom->Get_Frame().m_iCurrentTex == 11)
							pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.1f, 0.1f, 0.01f);
						break;
					case DIR_STATE::DIR_LEFT:
					case DIR_STATE::DIR_RIGHT:
						if (m_pTextureCom->Get_Frame().m_iCurrentTex == 2 || m_pTextureCom->Get_Frame().m_iCurrentTex == 10)
							pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.1f, 0.1f, 0.01f);
						break;
					}
					break;
				}
			}
		}
	}	
}

_float CBearger::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	_float fDmg = __super::Take_Damage(fDamage, DamageType, DamageCauser);

	if (fDmg > 0)
	{
		if (!m_bDead)
			m_bHit = true;

		m_bAggro = true;
		m_bIsAttacking = false;
		m_dwAttackTime = GetTickCount();
	}

	return fDmg;
}

HRESULT CBearger::Drop_Items()
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

_bool CBearger::IsDead()
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

CBearger* CBearger::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBearger* pInstance = new CBearger(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBearger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBearger::Clone(void* pArg)
{
	CBearger* pInstance = new CBearger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBearger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBearger::Free()
{
	__super::Free();
}