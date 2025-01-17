#include "stdafx.h"
#include "..\Public\Woodie.h"
#include "GameInstance.h"
#include "Player.h"
#include "BerryBush.h"
#include "Pig.h"
#include "Bullet.h"
#include "Skill.h"
#include "Inventory.h"
CWoodie::CWoodie(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CNPC(pGraphic_Device)
{
}

CWoodie::CWoodie(const CWoodie & rhs)
	: CNPC(rhs)
{
}

HRESULT CWoodie::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWoodie::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	//Test
	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
	m_eObjID = OBJID::OBJ_NPC;
	m_eNPCID = NPCID::NPC_WOODIE;

	m_fAtk_Max_CoolTime = 2.f;
	m_fAtk_Cur_CoolTime = m_fAtk_Max_CoolTime;

	m_fSkill_Max_CoolTime = 5.f;
	m_fSkill_Cur_CoolTime = m_fSkill_Max_CoolTime;

	m_fSkillRange = 2.f;
	m_fAtkRange = 2.f;
	m_fOwnerRadius = 10.f;
	m_fMinRange = 5.f;
	m_fDetectRange = 15.f;
	//Init BehavirvTree
	BehaviorTree = new CBT_NPC(this);

	BehaviorTree->Initialize();

	m_tInfo.iMaxHp = 300;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;
	m_tInfo.fDamage = 20.f;

	Change_Texture(TEXT("Com_Texture_Idle_Down"));
	return S_OK;
}

int CWoodie::Tick(_float fTimeDelta)
{
	m_iCurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	
	if ((LEVEL)m_iCurrentLevelndex == LEVEL_GAMEPLAY && m_bDead && m_fReviveTime > 10.f)
	{
		m_bCanTalk = true;
		m_bDead = false;
		Clear_Activated();
		Reset_Target();
	}

	if (m_iCurrentLevelndex != m_iPreLevelIndex)
	{
		if (m_bOwner)
		{
			m_bCanTalk = true;
			_float3 Owner_Pos = static_cast<CPlayer*>(m_pOwner)->Get_Pos();
			Owner_Pos.x -= 3.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, Owner_Pos);

			MINIMAP		minidesc;
			ZeroMemory(&minidesc, sizeof(MINIMAP));
			minidesc.name = MIN_WOODIE;
			minidesc.pointer = this;
			LEVEL CurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MiniMap_Icon"), CurrentLevelndex, TEXT("MiniMap_Icon"), &minidesc);

		}
		else
		{
			m_bCanTalk = true;
			Clear_Activated();
			Reset_Target();
			if (m_iCurrentLevelndex == LEVEL_GAMEPLAY)
			{
				MINIMAP		minidesc;
				ZeroMemory(&minidesc, sizeof(MINIMAP));
				minidesc.name = MIN_WOODIE;
				minidesc.pointer = this;
				LEVEL CurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MiniMap_Icon"), CurrentLevelndex, TEXT("MiniMap_Icon"), &minidesc);
			}
			
		}
		

		m_iPreLevelIndex = m_iCurrentLevelndex;
	}

	if (m_iCurrentLevelndex == LEVEL_LOADING)
		return OBJ_NOEVENT;

	if (m_iCurrentLevelndex != LEVEL_GAMEPLAY && !m_bOwner)
	{
		m_bCanTalk = false;
		return OBJ_NOEVENT;
	}


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_PLAYER, this);

	__super::Tick(fTimeDelta);


	BehaviorTree->Tick(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CWoodie::Late_Tick(_float fTimeDelta)
{
	if (m_iCurrentLevelndex == LEVEL_LOADING)
		return;

	if (m_iCurrentLevelndex != LEVEL_GAMEPLAY && !m_bOwner)
		return;


	__super::Late_Tick(fTimeDelta);

	Change_Frame(fTimeDelta);
	if (m_bDead)
	{
		return;
	}

	if (m_eCur_Dir == DIR_STATE::DIR_LEFT)
	{
		m_pTransformCom->Set_Scale(-1.f, 1.f, 1.f);
	}
}

HRESULT CWoodie::Render()
{
	if (m_iCurrentLevelndex == LEVEL_LOADING)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;


#ifdef _DEBUG

	//m_pColliderCom->Render_ColliderBox();
#endif // _DEBUG

	return S_OK;
}

HRESULT CWoodie::SetUp_Components(void * pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//m_TimerTag = TEXT("Timer_Grass");
	//if (FAILED(pGameInstance->Add_Timer(m_TimerTag)))
	//return E_FAIL;



	/* For.Com_Texture */
	Texture_Clone();
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));
	CollRectDesc.fRadiusY = 0.4f;
	CollRectDesc.fRadiusX = 0.3f;
	CollRectDesc.fRadiusZ = 0.3f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = -0.25f;
	CollRectDesc.fOffsetZ = 0.f;

	/* For.Com_Collider_Rect*/
	/*if (FAILED(__super::Add_Components(TEXT("Com_Collider_Rect"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Rect"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
	return E_FAIL;*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 3.f / 1.13f;
	TransformDesc.fRotationPerSec = D3DXToRadian(0.f);
	TransformDesc.InitPos = _float3(45.f, 2.f, 30.f);;



	Safe_Release(pGameInstance);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWoodie::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	/*Idle*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 29;
	TextureDesc.m_fSpeed = 20;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Idle_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Idle_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Idle_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	/*Run*/
	TextureDesc.m_iEndTex = 24;
	TextureDesc.m_fSpeed = 30;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Run_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Run_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Run_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Run_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Run_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Run_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	/*Build*/
	TextureDesc.m_iEndTex = 16;
	TextureDesc.m_fSpeed = 20;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Build_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Build_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Build_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Build_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Build_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Build_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Pickup*/
	TextureDesc.m_iEndTex = 6;
	TextureDesc.m_fSpeed = 30;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Pickup_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Pickup_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Pickup_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Pickup_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Pickup_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Pickup_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Give*/
	TextureDesc.m_iEndTex = 17;
	TextureDesc.m_fSpeed = 30;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Give_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Give_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Give_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Give_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Give_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Give_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Dance*/
	TextureDesc.m_iEndTex = 39;
	TextureDesc.m_fSpeed = 30;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dance"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Dance"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Talk*/
	TextureDesc.m_iEndTex = 51;
	TextureDesc.m_fSpeed = 30;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Talk"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Talk"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Attack*/
	TextureDesc.m_iEndTex = 18;
	TextureDesc.m_fSpeed = 30;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Attack_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Attack_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Attack_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Attack_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Attack_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Attack_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Dead*/
	TextureDesc.m_iEndTex = 55;
	TextureDesc.m_fSpeed = 30;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dead"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Dead"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Hit*/
	TextureDesc.m_iEndTex = 22;
	TextureDesc.m_fSpeed = 30;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Hit"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Woodie_Hit"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CWoodie::Change_Frame(_float fTimeDelta)
{
	if (m_eState == NPC_STATE::DEAD)
	{
		m_pTextureCom->MoveFrame(m_TimerTag, false);
	}
	else
	{
		m_pTextureCom->MoveFrame(m_TimerTag);
	}

}

void CWoodie::Change_Motion()
{
}

void CWoodie::Interact(_uint Damage)
{
	if (m_iTalkCnt == 2 && Damage == 1)
	{
		m_bAccept = true;
		m_iTalkCnt++;
	}
	else if (m_iTalkCnt == 2 && Damage == 2)
	{
		m_bAccept = false;
		m_iTalkCnt++;
	}
	else if (m_iTalkCnt != 2)
	{
		m_iTalkCnt++;
	}

}

HRESULT CWoodie::Drop_Items()
{
	return S_OK;
}

void CWoodie::Make_Interrupt(CPawn * pCauser, _uint _InterruptNum)
{
	switch (_InterruptNum)
	{
	case 0: // Talk
		Reset_Target();
		m_pTarget = pCauser;
		Safe_AddRef(m_pTarget);
		m_iInterruptNum = _InterruptNum;
		m_bInterrupted = true;
		break;
	case 1://FightMode
		if (!m_bFightMode && m_bOwner)
		{
			m_iInterruptNum = _InterruptNum;
			m_bInterrupted = true;
		}
		break;
	}


}

_float CWoodie::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (m_bInvincibleMode || m_bFirstCall)
		return 0.f;

	if (!m_bDead && m_tInfo.iCurrentHp <= (_int)fDamage)
	{
		m_fReviveTime = 0.f;
		m_bDead = true;
		m_bCanTalk = false;
	}
	else if(!m_bDead &&!m_bHited)
	{
		m_tInfo.iCurrentHp -= (_int)fDamage;
		cout << "WoodieHP: " << m_tInfo.iCurrentHp << endl;
		m_bHited = true;
	}
	return fDamage;
}

void CWoodie::Move(_float _fTimeDelta)
{
	_float3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_eCur_Dir = Check_Direction();

	m_eState = CNPC::MOVE;

	if (m_ePreState != m_eState
		|| m_ePre_Dir != m_eCur_Dir)
	{
		m_eCur_Dir = Check_Direction();
		switch (m_eCur_Dir)
		{
		case DIR_UP:
			Change_Texture(TEXT("Com_Texture_Run_Up"));
			break;

		case DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Run_Down"));
			break;

		case DIR_LEFT:
			Change_Texture(TEXT("Com_Texture_Run_Side"));
			break;

		case DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Run_Side"));
			break;
		}
		m_ePreState = m_eState;
		m_ePre_Dir = m_eCur_Dir;
	}
	Play_Sound(_fTimeDelta);
	m_pTransformCom->Go_PosTarget(_fTimeDelta, m_vTargetPos, _float3{ 0.f, 0.f, 0.f });

	SetUp_BillBoard();
}

void CWoodie::Idle(_float _fTimeDelta)
{
	m_eState = CNPC::IDLE;

	if (m_ePreState != m_eState)
	{
		switch (m_eCur_Dir)
		{
		case DIR_UP:
			Change_Texture(TEXT("Com_Texture_Idle_Up"));
			break;

		case DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Idle_Down"));
			break;

		case DIR_LEFT:
		case DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Idle_Side"));
			break;

		}
		m_ePreState = m_eState;
	}
}

void CWoodie::Interaction(_float _fTimedelta)
{
	//Test Only Berry
	if (m_pTarget == nullptr)
		return;
	//나중에 분간.

	Revive_Berry(_fTimedelta);

}
void CWoodie::Talk(_float _fTimeDelta)
{
	if (static_cast<CPawn*>(m_pTarget)->Get_ObjID() == OBJID::OBJ_PLAYER)
	{
		Talk_Player(_fTimeDelta);
	}
	else
	{//Monster
		Talk_Friend(_fTimeDelta);
	}
}

void CWoodie::Dance(_float _fTimeDelta)
{
	m_fInteractTIme += _fTimeDelta;

	m_eState = CNPC::DANCE;

	if (m_ePreState != m_eState)
	{
		m_bInteract = true;
		Change_Texture(TEXT("Com_Texture_Dance"));
		m_ePreState = m_eState;
	}
	if (2.f < m_fInteractTIme)
	{
		m_fInteractTIme = 0.f;
		m_bInteract = false;
	}
}

void CWoodie::Attack(_float _fTimeDelta)
{
	//m_fInteractTIme += _fTimeDelta;

	m_eState = CNPC::ATTACK;

	if (m_ePreState != m_eState)
	{
		Play_Sound(_fTimeDelta);
		m_bInteract = true;
		switch (m_eCur_Dir)
		{
		case DIR_STATE::DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Attack_Down"));
			break;
		case DIR_STATE::DIR_UP:
			Change_Texture(TEXT("Com_Texture_Attack_Up"));
			break;

		case DIR_STATE::DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Attack_Side"));
			break;
		case DIR_STATE::DIR_LEFT:
			Change_Texture(TEXT("Com_Texture_Attack_Side"));
			break;
		}
		m_ePreState = m_eState;
	}
	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 2)
	{
		Create_Bullet(_fTimeDelta);
		m_bInteract = false;
		m_bCanAttack = false;
	}


}

void CWoodie::Interrupted(_float _fTimeDelta)
{//for Test. Only TalkInterrupt
	if (m_bInterrupted)
	{

		switch (m_iInterruptNum)
		{
		case 0://TalkMode
			Clear_Activated();
			m_bFirstCall = true;
			m_bInteract = true;
			m_bInterrupted = false;
			m_bSelectAct = false;
			m_bFinishInteract = false;
			break;
		case 1: // attackMode
			Clear_Activated();
			m_bFightMode = true;
			m_bInteract = true;
			m_bInterrupted = false;
			m_bSelectAct = false;
			m_bFinishInteract = false;
			break;
		}

	}

}

void CWoodie::Skill(_float _fTimeDelta)
{
	m_eState = CNPC::SKILL;

	if (m_ePreState != m_eState)
	{
		Play_Sound(_fTimeDelta);
		m_bInteract = true;
		switch (m_eCur_Dir)
		{
		case DIR_STATE::DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Attack_Down"));
			break;
		case DIR_STATE::DIR_UP:
			Change_Texture(TEXT("Com_Texture_Attack_Up"));
			break;
		case DIR_STATE::DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Attack_Side"));
			break;
		case DIR_STATE::DIR_LEFT:
			Change_Texture(TEXT("Com_Texture_Attack_Side"));
			break;
		}
		m_ePreState = m_eState;
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		CSkill::SKILL_DESC SkillDesc;

		SkillDesc.eDirState = DIR_END;
		SkillDesc.eSkill = CSkill::SKILL_TYPE::CRACKLE;
		SkillDesc.vTargetPos = m_pOwner->Get_Position();
		SkillDesc.vPosition = Get_Position();
		SkillDesc.vScale = _float3(1.f, 1.f, 1.f);
		SkillDesc.pTarget = m_pOwner;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Skill"), m_iCurrentLevelndex, TEXT("Skill"), &SkillDesc)))
			return;
		m_bSkillUsing = true;
	}


	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 2)
	{
		m_bSkillUsing = false;
		Create_Heal(_fTimeDelta);
		m_bInteract = false;
		m_bCanSkill = false;
	}
}

_bool CWoodie::Hit(_float _fTimeDelta)
{
	m_fInteractTIme += _fTimeDelta;

	m_eState = CNPC::HIT;

	if (m_ePreState != m_eState)
	{
		Play_Sound(_fTimeDelta);
		m_bInteract = true;
		Change_Texture(TEXT("Com_Texture_Hit"));
		m_ePreState = m_eState;
	}
	
	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex -1)
	{
		Clear_Activated();
		m_bHited = false;
	}
	return true;
}

_bool CWoodie::Dead(_float _fTimeDelta)
{
	m_eState = CNPC::DEAD;

	if (m_ePreState != m_eState)
	{
		Play_Sound(_fTimeDelta);
		//m_bInteract = true;
		Change_Texture(TEXT("Com_Texture_Dead"));
		m_ePreState = m_eState;
	}

	return true;
}

void CWoodie::Select_Target(_float _fTimeDelta)
{
	if (m_iCurrentLevelndex == LEVEL_LOADING)
		return;

	Find_Priority();
	m_bInteract = true;
}

void CWoodie::Set_RandPos(_float _fTimeDelta)
{// Find Random Patroling Position
	_float fOffsetX = ((_float)rand() / (float)(RAND_MAX)) * m_fPatrolRadius;
	_int bSignX = rand() % 2;
	_float fOffsetZ = ((_float)rand() / (float)(RAND_MAX)) * m_fPatrolRadius;
	_int bSignZ = rand() % 2;
	m_fPatrolPosX = bSignX ? (m_pTransformCom->Get_TransformDesc().InitPos.x + fOffsetX) : (m_pTransformCom->Get_TransformDesc().InitPos.x - fOffsetX);
	m_fPatrolPosZ = bSignZ ? (m_pTransformCom->Get_TransformDesc().InitPos.z + fOffsetZ) : (m_pTransformCom->Get_TransformDesc().InitPos.z - fOffsetZ);

	m_vTargetPos = _float3(m_fPatrolPosX, 0.5f, m_fPatrolPosZ);
	m_bSelectAct = false;
}

_bool CWoodie::Get_Target_Moved(_float _fTimeDelta, _uint _iTarget)
{
	if (m_bFightMode&&m_bSkillUsing)
		return false;
	else if (!m_bFightMode && m_bSkillUsing)
	{
		m_bSkillUsing = false;
		Clear_Activated();
	}
	_float fMinRange = 0.f;
	_float fMiddleRange = 0.f;
	_float Compare_Range = 0.f;
	switch (_iTarget)
	{
	case 0: //Target == Owner
		fMinRange = m_fMinRange;
		fMiddleRange = m_fOwnerRadius;
		m_vTargetPos = static_cast<CPlayer*>(m_pOwner)->Set_PartyPostion(this);
		break;
	case 1:// Basic AttackRange
		fMiddleRange = fMinRange = m_fAtkRange;
		m_vTargetPos = m_pTarget->Get_Position();
		break;
	case 2://SkillRange
		fMiddleRange = fMinRange = m_fSkillRange;

		m_vTargetPos = m_pTarget->Get_Position();
		break;
	default:
		if (m_pTarget)
		{
			m_vTargetPos = m_pTarget->Get_Position();
		}
		fMiddleRange = fMinRange = 0.4f;
		break;
	}


	Compare_Range = (m_vTargetPos.x - Get_Pos().x)*(m_vTargetPos.x - Get_Pos().x)
		+ (m_vTargetPos.z - Get_Pos().z)*(m_vTargetPos.z - Get_Pos().z);

	if (fMinRange > Compare_Range)
	{
		m_bArrive = true;
		Clear_Activated();
		return false;
	}

	if (m_bFightMode)
	{
		if (fMiddleRange < Compare_Range)
		{
			m_bArrive = false;
			Clear_Activated();
		}
	}
	else
	{	
		if (fMinRange < Compare_Range)
		{
			m_bArrive = false;
			Clear_Activated();
		}
	}
	return true;
}

_bool CWoodie::Detect_Enemy()
{
	Find_Enemy();

	if (m_pTarget == nullptr)
		return false;

	m_bInteract = true;
	return true;
}

void CWoodie::Play_Sound(_float _fTimeDelta)
{
	if (CGameInstance::Get_Instance()->Is_In_Frustum(Get_Position(), m_fRadius) == false)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_int iNum = rand() % 4;
	_tchar	szFullPath[MAX_PATH];
	_float fVolume = 0.5f;
	switch (m_eState)
	{
	case CNPC::IDLE:
		break;
	case CNPC::ATTACK:
	case CNPC::SKILL:
		wcscpy_s(szFullPath, TEXT("Weapon_Swing.wav"));
		fVolume = 1.f;
		break;
	case CNPC::MOVE:
		fVolume = 0.3f;
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 14
			|| m_pTextureCom->Get_Frame().m_iCurrentTex == 2)
		{
			iNum = rand() % 4;
			if ((LEVEL)m_iCurrentLevelndex == LEVEL_GAMEPLAY)
			{
				wcscpy_s(szFullPath, TEXT("footstep_grass_%d.wav"));
			}
			else
			{
				wcscpy_s(szFullPath, TEXT("DSS_woodlegs_footstep_wood_%d.wav"));
			}
			wsprintf(szFullPath, szFullPath, iNum);
		}
		break;
	case CNPC::HIT:
		//Player Hit
		fVolume = 0.8f;
		wcscpy_s(szFullPath, TEXT("Woodie_hurt_%d.wav"));
		wsprintf(szFullPath, szFullPath, iNum);
		break;
	case CNPC::DEAD:
		fVolume = 0.9f;
		wcscpy_s(szFullPath, TEXT("Woodie_death.wav"));
		break;
	case CNPC::TALK:
		/*fVolume = 0.8f;
		wcscpy_s(szFullPath, TEXT("Woodie_generic_%d.wav"));
		wsprintf(szFullPath, szFullPath, iNum);*/
		break;
	}

	pGameInstance->PlaySounds(szFullPath, SOUND_WOODIE, fVolume);

	Safe_Release(pGameInstance);
}

void CWoodie::Revive_Berry(_float _fTimeDelta)
{
	m_eState = CNPC::INTERACT;

	if (m_ePreState != m_eState)
	{
		switch (m_eCur_Dir)
		{
		case DIR_UP:
			Change_Texture(TEXT("Com_Texture_Build_Up"));
			break;

		case DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Build_Down"));
			break;

		case DIR_LEFT:
		case DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Build_Side"));
			break;

		}
		m_ePreState = m_eState;
	}

	m_fInteractTIme += _fTimeDelta;

	if (2.f < m_fInteractTIme)
	{
		m_fInteractTIme = 0.f;

		m_bInteract = false;
		m_bSelectAct = true;
		Reset_Target();
	}


}

void CWoodie::Talk_Player(_float _fTimeDelta)
{
	CInventory_Manager::Get_Instance()->Get_Talk_list()->front()->Set_WoodyTalk(true);

	CInventory_Manager* pinven = CInventory_Manager::Get_Instance();
	pinven->Get_Talk_list()->front()->setcheck(true);

	if (!m_bFirstCall)
	{
		m_bNextAct = false;
		m_bInteract = false;
		return;
	}

	m_eState = CNPC::TALK;
	static_cast<CPlayer*>(m_pTarget)->Set_TalkMode(true);
	static_cast<CPlayer*>(m_pTarget)->Set_bOnlyActionKey(true);

	if (m_ePreState != m_eState)
	{
		m_fInteractTIme = 0.f;
		m_bInteract = true;
		Change_Texture(TEXT("Com_Texture_Talk"));
		m_ePreState = m_eState;

	}


	if (m_iPreTalkCnt != m_iTalkCnt)
	{
		Play_Sound(_fTimeDelta);
		if (!m_bOwner)
		{//IsPartyed
			switch (m_iTalkCnt)
			{
			case 1:
				pinven->Get_Talk_list()->front()->Set_Texnum1(0);
				break;
			case 2:
				pinven->Get_Talk_list()->front()->Set_Texnum1(1);
				break;
			case 3:
				if (m_bAccept)
				{
					pinven->Get_Talk_list()->front()->Set_Texnum1(2);
					m_bNextAct = true;

				}
				else
				{
					pinven->Get_Talk_list()->front()->Set_Texnum1(3);
					m_bNextAct = false;
				}
				break;
			case 4:
				m_bSelectAct = true;
				Clear_Activated();
				static_cast<CPlayer*>(m_pTarget)->Set_TalkMode(false);
				static_cast<CPlayer*>(m_pTarget)->Set_bOnlyActionKey(false);
				m_iTalkCnt = 0;
				m_bInteract = false;
				m_bFirstCall = false;
				if (m_bNextAct)
				{
					m_bOwner = true;
					m_pOwner = static_cast<CPawn*>(m_pTarget);
					static_cast<CPlayer*>(m_pTarget)->Add_Party(TEXT("Woodie"), this);
					CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
					auto k = pinv->Get_Party_list();
					for (auto iter : *k)
					{
						if (iter->Get_check() == false)
						{
							iter->Set_check(true);
							iter->Set_texnum(2);
							break;
						}
					}
				}
				else
				{
					Reset_Target();
					m_pTarget = nullptr;
				}
				pinven->Get_Talk_list()->front()->setcheck(false);
				CInventory_Manager::Get_Instance()->Get_Talk_list()->front()->Set_WoodyTalk(false);
				break;
			}
		}
		else
		{//Solo
			switch (m_iTalkCnt)
			{
			case 1:
				pinven->Get_Talk_list()->front()->Set_Texnum1(4);
				break;
			case 2:
				pinven->Get_Talk_list()->front()->Set_Texnum1(5);
				break;
			case 3:
				if (m_bAccept)
				{
					pinven->Get_Talk_list()->front()->Set_Texnum1(6);
					m_bNextAct = false;
				}
				else
				{
					pinven->Get_Talk_list()->front()->Set_Texnum1(8);
					m_bNextAct = true;
				}
				break;
			case 4:
				m_bSelectAct = true;
				Clear_Activated();
				static_cast<CPlayer*>(m_pTarget)->Set_TalkMode(false);
				static_cast<CPlayer*>(m_pTarget)->Set_bOnlyActionKey(false);
				if (!m_bNextAct)
				{
					static_cast<CPlayer*>(m_pTarget)->Release_Party(TEXT("Woodie"));

					Reset_Target();
					m_bOwner = false;
					m_pOwner = nullptr;

					CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
					auto k = pinv->Get_Party_list();
					for (auto iter : *k)
					{
						if (iter->Get_check() == true && iter->Get_texnum() == 2)
						{
							iter->Set_check(false);
							iter->Set_texnum(3);
							break;
						}
					}
				}
				m_iTalkCnt = 0;
				m_bInteract = false;
				m_bFirstCall = false;
				pinven->Get_Talk_list()->front()->setcheck(false);
				CInventory_Manager::Get_Instance()->Get_Talk_list()->front()->Set_WoodyTalk(false);
				break;
			}
		}
		m_iPreTalkCnt = m_iTalkCnt;
	}



}

void CWoodie::Talk_Friend(_float _fTimeDelta)
{

	m_fInteractTIme += _fTimeDelta;

	m_eState = CNPC::TALK;

	if (m_ePreState != m_eState)
	{
		m_fInteractTIme = 0.f;
		m_bInteract = true;
		Change_Texture(TEXT("Com_Texture_Talk"));
		m_ePreState = m_eState;
		static_cast<CPig*>(m_pTarget)->Interact(_fTimeDelta, 0);
	}

	if (2.f < m_fInteractTIme)
	{
		m_fInteractTIme = 0.f;
		m_bInteract = false;

		_int iRandNum = rand() % 2;
		if (iRandNum == 1)
		{
			static_cast<CPig*>(m_pTarget)->Interact(_fTimeDelta, 2);
			m_bNextAct = false;
		}
		else
		{
			static_cast<CPig*>(m_pTarget)->Interact(_fTimeDelta, 1);
			m_bNextAct = true;
		}
	}


}

void CWoodie::Create_Bullet(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	BULLETDATA BulletData;
	ZeroMemory(&BulletData, sizeof(BulletData));
	BulletData.bIsPlayerBullet = true;
	BulletData.eDirState = m_eCur_Dir;
	BulletData.eWeaponType = WEAPON_SWORD;
	BulletData.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	BulletData.vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	D3DXVec3Normalize(&BulletData.vLook, &BulletData.vLook);
	D3DXVec3Normalize(&BulletData.vRight, &BulletData.vRight);
	switch (m_eCur_Dir)
	{
	case DIR_UP:
		BulletData.vPosition = Get_Position() + BulletData.vLook*1.f;
		break;
	case DIR_DOWN:
		BulletData.vPosition = Get_Position() - BulletData.vLook*1.f;
		break;
	case DIR_LEFT:
		BulletData.vPosition = Get_Position() + BulletData.vRight*1.f;
		break;
	case DIR_RIGHT:
		BulletData.vPosition = Get_Position() + BulletData.vRight*1.f;
		break;
	}

	if(FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
		return;
}

void CWoodie::Create_Heal(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CSkill::SKILL_DESC SkillDesc;

	SkillDesc.eDirState = DIR_END;
	SkillDesc.eSkill = CSkill::SKILL_TYPE::CRACKLE_HIT;
	SkillDesc.vTargetPos = m_pOwner->Get_Position();
	SkillDesc.vPosition = m_pOwner->Get_Position();
	SkillDesc.vScale = _float3(2.f, 3.f, 1.f);
	SkillDesc.pTarget = m_pOwner;

	_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float3 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	D3DXVec3Normalize(&vLook, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);
	switch (m_eCur_Dir)
	{
	case DIR_UP:
		SkillDesc.vPosition = Get_Position() + vLook*1.f;
		break;
	case DIR_DOWN:
		SkillDesc.vPosition = Get_Position() - vLook*1.f;
		break;
	case DIR_LEFT:
		SkillDesc.vPosition = Get_Position() + vRight*1.f;
		break;
	case DIR_RIGHT:
		SkillDesc.vPosition = Get_Position() + vRight*1.f;
		break;
	}

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Skill"), m_iCurrentLevelndex, TEXT("Skill"), &SkillDesc)))
		return;
}

void CWoodie::MoveWithOwner(_float _fTimeDelta)
{
	_float3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (static_cast<CPawn*>(m_pTarget)->Get_ObjID() == OBJ_MONSTER && m_bFightMode)
	{
		m_pTransformCom->Go_PosTarget(_fTimeDelta, m_vTargetPos, _float3{ 0.f, 0.f, 0.f });
		if ((abs(vMyPos.x - m_vTargetPos.x) < 0.3 &&
			abs(vMyPos.z - m_vTargetPos.z) < 0.3))
		{
			m_bDirChanged = false;
		}
	}
	else if (m_pTarget == m_pOwner)
	{
		m_pTransformCom->Go_PosTarget(_fTimeDelta, _float3(m_fPatrolPosX, Get_Position().y, m_fPatrolPosZ), _float3{ 0.f, 0.f, 0.f });
		if ((abs(vMyPos.x - m_fPatrolPosX) < 0.3 &&
			abs(vMyPos.z - m_fPatrolPosZ) < 0.3))
		{
			m_bDirChanged = false;
		}
	}
}

void CWoodie::MoveWithoutOwner(_float _fTimeDelta)
{
	_float3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (m_pTarget)
	{
		m_pTransformCom->Go_PosTarget(_fTimeDelta, m_vTargetPos, _float3{ 0.f, 0.f, 0.f });
		if ((abs(vMyPos.x - m_vTargetPos.x) < 0.1f &&
			abs(vMyPos.z - m_vTargetPos.z) < 0.1f))
		{
			m_bDirChanged = false;
		}
	}
	else
	{
		m_pTransformCom->Go_PosTarget(_fTimeDelta, _float3(m_fPatrolPosX, Get_Position().y, m_fPatrolPosZ), _float3{ 0.f, 0.f, 0.f });
		if ((abs(vMyPos.x - m_fPatrolPosX) < 0.3 &&
			abs(vMyPos.z - m_fPatrolPosZ) < 0.3))
		{
			m_bDirChanged = false;
		}
	}
}

DIR_STATE CWoodie::Check_Direction(void)
{
	//내 자신의 Look에서 3번 변환한 값
	_float3 vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float4x4 OriginMat = m_pTransformCom->Get_WorldMatrix();
	_float4x4 RotateMat = OriginMat;
	_float3 vRot45 = *(_float3*)&OriginMat.m[2][0];
	_float3 vRot135 = *(_float3*)&OriginMat.m[2][0];


	//Turn
	D3DXMatrixRotationAxis(&RotateMat, &_float3(0.f, 1.f, 0.f), D3DXToRadian(90.f));
	D3DXVec3TransformNormal(&vRot45, &vRot45, &RotateMat);
	//vRot45 = *(_float3*)&RotateMat.m[2][0];
	D3DXMatrixRotationAxis(&RotateMat, &_float3(0.f, 1.f, 0.f), D3DXToRadian(135.f));
	D3DXVec3TransformNormal(&vRot135, &vRot135, &RotateMat);
	//vRot135 = *(_float3*)&RotateMat.m[2][0];;
	///////
	_float3 vTargetLook = m_vTargetPos - Get_Pos();

	D3DXVec3Normalize(&vRot45, &vRot45);
	D3DXVec3Normalize(&vRot135, &vRot135);
	D3DXVec3Normalize(&vMyLook, &vMyLook);
	D3DXVec3Normalize(&vTargetLook, &vTargetLook);

	//각 내적값들 구하기
	_float fTargetDot = D3DXVec3Dot(&vMyLook, &vRot45);
	fTargetDot = acos(fTargetDot);
	_float fDegree45 = D3DXToDegree(fTargetDot);

	fTargetDot = D3DXVec3Dot(&vMyLook, &vRot135);
	fTargetDot = acos(fTargetDot);
	_float fDegree135 = D3DXToDegree(fTargetDot);

	fTargetDot = D3DXVec3Dot(&vMyLook, &vTargetLook);
	fTargetDot = acos(fTargetDot);
	_float fDegreeTarget = D3DXToDegree(fTargetDot);

	//외적하기
	_float3 vCrossOut;
	//D3DXVec3Cross(&vCrossOut, &vMyLook, &vTargetLook);
	D3DXVec3Cross(&vCrossOut, &vMyLook, &vTargetLook);


	DIR_STATE eDir = DIR_END;
	//Check Left Or Right
	if (vCrossOut.y > 0.f)//Right
	{
		if (fDegreeTarget > 0.f && fDegreeTarget <= 60.f)
		{
			//cout << "y+, UP, Degree: " << fDegreeTarget << endl;
			return DIR_UP;
		}
		else if (fDegreeTarget > 60.f && fDegreeTarget <= 135.f)
		{
			//cout << "y+, Right, Degree: " << fDegreeTarget << endl;
			return DIR_RIGHT;
		}
		else if (fDegreeTarget > 135.f && fDegreeTarget <= 180.f)
		{
			//cout << "y+, Down, Degree: " << fDegreeTarget << endl;
			return DIR_DOWN;
		}
	}
	else//Left
	{
		if (fDegreeTarget > 0.f && fDegreeTarget <= 60.f)
		{
			//cout << "y-, UP, Degree: " << fDegreeTarget << endl;
			return DIR_UP;
		}
		else if (fDegreeTarget > 60.f && fDegreeTarget <= 135.f)
		{
			//cout << "y-, Left, Degree: " << fDegreeTarget << endl;
			return DIR_LEFT;
		}
		else if (fDegreeTarget > 135.f && fDegreeTarget <= 180.f)
		{
			//cout << "y-, Down, Degree: " << fDegreeTarget << endl;
			return DIR_DOWN;
		}
	}

	//cout << "Error " << fDegreeTarget << endl;
	return DIR_DOWN;

}

void CWoodie::Find_Priority()
{
	int i = rand() % 5;
	switch (i)
	{
	case 1:
		Find_Friend();
		break;
	case 2:
		Find_Berry();
		break;
	case 0:
	case 3:
	case 4:
		Set_RandPos(0.f);
		break;
	}
}

void CWoodie::Find_Friend()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* list_Obj = pGameInstance->Get_ObjectList(m_iCurrentLevelndex, TEXT("Layer_Monster"));

	if (list_Obj == nullptr)
		return;

	_uint iIndex = 0;
	Reset_Target();
	m_pTarget = nullptr;
	for (auto& iter_Obj = list_Obj->begin(); iter_Obj != list_Obj->end();)
	{
		if ((*iter_Obj) != nullptr && !dynamic_cast<CMonster*>(*iter_Obj)->Get_Aggro()
			&& dynamic_cast<CMonster*>(*iter_Obj)->Get_MonsterID() == CMonster::MONSTER_ID::PIG)
		{

			if (m_pTarget == nullptr)
			{
				m_pTarget = *iter_Obj;
				m_bSelectAct = false;
			}

			_float fCmpDir = (Get_Pos().x - (*iter_Obj)->Get_Position().x)*(Get_Pos().x - (*iter_Obj)->Get_Position().x)
				/*+ (Get_Pos().y - (*iter_Obj)->Get_Position().y)*(Get_Pos().y - (*iter_Obj)->Get_Position().y)*/
				+ (Get_Pos().z - (*iter_Obj)->Get_Position().z)*(Get_Pos().z - (*iter_Obj)->Get_Position().z);

			_float fTargetDir = (Get_Pos().x - (m_pTarget)->Get_Position().x)*(Get_Pos().x - (m_pTarget)->Get_Position().x)
				/*+ (Get_Pos().y - (m_pTarget)->Get_Position().y)*(Get_Pos().y - (m_pTarget)->Get_Position().y)*/
				+ (Get_Pos().z - (m_pTarget)->Get_Position().z)*(Get_Pos().z - (m_pTarget)->Get_Position().z);

			if (fCmpDir < fTargetDir)
			{
				m_pTarget = *iter_Obj;
			}

			++iIndex;
			iter_Obj++;
		}
		else
		{
			++iIndex;
			iter_Obj++;
			continue;
		}
	}
	Safe_AddRef(m_pTarget);
	Safe_Release(pGameInstance);
}

void CWoodie::Find_Enemy()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* list_Obj = pGameInstance->Get_ObjectList(m_iCurrentLevelndex, TEXT("Layer_Monster"));

	if (list_Obj == nullptr)
		return;

	_uint iIndex = 0;

	Reset_Target();
	m_pTarget = nullptr;
	for (auto& iter_Obj = list_Obj->begin(); iter_Obj != list_Obj->end();)
	{
		if ((*iter_Obj) != nullptr && dynamic_cast<CMonster*>(*iter_Obj)->Get_Aggro())
		{
			_float fCmpDir = (Get_Pos().x - (*iter_Obj)->Get_Position().x)*(Get_Pos().x - (*iter_Obj)->Get_Position().x)
				/*+ (Get_Pos().y - (*iter_Obj)->Get_Position().y)*(Get_Pos().y - (*iter_Obj)->Get_Position().y)*/
				+ (Get_Pos().z - (*iter_Obj)->Get_Position().z)*(Get_Pos().z - (*iter_Obj)->Get_Position().z);

			_float fOwnerDistance = 0.f;

			if (fCmpDir > m_fOwnerRadius)
			{
				++iIndex;
				iter_Obj++;
				continue;
			}

			if (m_pTarget == nullptr)
			{
				m_pTarget = *iter_Obj;
				m_bSelectAct = false;
			}

			_float fTargetDir = (Get_Pos().x - (m_pTarget)->Get_Position().x)*(Get_Pos().x - (m_pTarget)->Get_Position().x)
				/*+ (Get_Pos().y - (m_pTarget)->Get_Position().y)*(Get_Pos().y - (m_pTarget)->Get_Position().y)*/
				+ (Get_Pos().z - (m_pTarget)->Get_Position().z)*(Get_Pos().z - (m_pTarget)->Get_Position().z);

			if (fCmpDir < fTargetDir)
			{
				m_pTarget = *iter_Obj;
			}

			++iIndex;
			iter_Obj++;
		}
		else
		{
			++iIndex;
			iter_Obj++;
			continue;
		}
	}

	if (m_pTarget == nullptr)
	{
		m_bFightMode = false;
	}

	Safe_AddRef(m_pTarget);

	Safe_Release(pGameInstance);
}

void CWoodie::Find_Berry()
{
	//후에 Primary_queue로 각 레이어들중에서 가장 가까운 객체를 m_pTarget
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* list_Obj = pGameInstance->Get_ObjectList(m_iCurrentLevelndex, TEXT("Layer_Object"));

	if (list_Obj == nullptr)
		return;

	_uint iIndex = 0;
	Reset_Target();
	m_pTarget = nullptr;
	
	Safe_AddRef(m_pTarget);
	Safe_Release(pGameInstance);

}

void CWoodie::Find_Player()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	Reset_Target();

	m_pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	Safe_AddRef(m_pTarget);

	Safe_Release(pGameInstance);
}

CWoodie * CWoodie::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWoodie* pInstance = new CWoodie(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CWoodie"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWoodie::Clone(void * pArg)
{
	CWoodie* pInstance = new CWoodie(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CWoodie"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWoodie::Free()
{
	__super::Free();

	Safe_Release(BehaviorTree);
}
