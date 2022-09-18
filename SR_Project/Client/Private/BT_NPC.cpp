#include "stdafx.h"
#include "..\Public\BT_NPC.h"

CBT_NPC::CBT_NPC(CNPC * _Actor)
	:Actor(_Actor)
{
	//Safe_AddRef(Actor);
}

void CBT_NPC::Initialize()
{
	//Init_Map();
	Init_Nodes();
}

void CBT_NPC::Init_Nodes()
{
	//Init Root 
	Root = new CSequenceNode;

	//Init Selector
	Selector_OnTarget = new CSelectorNode;
	Selector_Main = new CSelectorNode;

	Selector_OnOwner = new CSelectorNode;
	Selector_NonFight = new CSelectorNode;
	Selector_Attack = new CSelectorNode;
	//Selector_Skill = new CSelectorNode;
	//Init Sequence
	Sequence_NonTarget = new CSequenceNode;
	Sequence_Interact_Actor = new CSequenceNode;
	Sequence_OnTarget = new CSequenceNode;
	Sequence_Attack = new CSequenceNode;
	Sequence_MoveToTarget = new CSequenceNode; // TODO: Add TargetMove, Move, IsArrive
	Sequence_Fight = new CSequenceNode;
	Sequence_Skill = new CSequenceNode;
	//Init Decorator
	BTTask_HasTarget = new CBTTask_HasTarget;
	BTTask_HasEnemy = new CBTTask_HasTarget;
	BTTask_IsActor = new CBTTask_IsActor;
	BTTask_HasOwner = new CBTTask_HasOwner;
	BTTask_IsFirstCall = new CBTTask_IsFirstCall;
	BTTask_IsFightMode = new CBTTask_IsFightMode;
	BTTask_IsTargetMove = new CBTTask_TargetMoved;

	BTTask_IsOwner_Closed = new CBTTask_TargetMoved;

	BTTask_IsSkillRange = new CBTTask_TargetSkillRange;
	BTTask_IsAtkRange = new CBTTask_TargetAtkRange;
	//Init Leaf
	BTTask_SetRandPos = new CBTTask_SetRandPos;
	BTTask_Idle = new CBTTask_Idle;
	BTTask_Move = new CBTTask_Move;
	BTTask_Interact = new CBTTask_Interact;
	BTTask_SelectTarget = new CBTTask_SelectTarget;
	BTTask_Talk = new CBTTask_Talk;
	BTTask_Dance = new CBTTask_Dance;
	BTTask_Arrive = new CBTTask_IsArrive;
	BTTask_Interrupt = new CBTTask_Interrupt;
	
	BTTask_SetGoalPos = new CBTTask_SetGoalPos;
	BTTask_Attack = new CBTTask_Attack;
	BTTask_GetCanAttack = new CBTTask_GetCanAttack;
	BTTask_TargetIsDead = new CBTTask_TargetIsDead;
	BTTask_TargetHited = new CBTTask_Target_Hited;
	BTTask_Fail = new CBTTask_Fail;
	BTTask_GetCanSkill = new CBTTask_GetCanSkill;
	BTTask_Skill = new CBTTask_Skill;
	//Add in Root
	Root->Add_Node(Selector_Main);
	//Select_Main
	Selector_Main->Add_Node(BTTask_Interrupt);
	Selector_Main->Add_Node(BTTask_HasOwner);
	// HasOwner
	BTTask_HasOwner->Set_DecoratorNodes(Selector_OnOwner, BTTask_HasTarget);
	//Add in HasTarget_Deco
	BTTask_HasTarget->Set_DecoratorNodes(Selector_OnTarget, Sequence_NonTarget);
	BTTask_HasEnemy->Set_DecoratorNodes(Selector_Attack, BTTask_Fail);
	//BTTask_IsActor->Set_DecoratorNodes(Sequence_Interact_Actor, BTTask_Interact);
	//Add in TargetMoved_Deco
	BTTask_IsTargetMove->Set_DecoratorNodes(Sequence_MoveToTarget, BTTask_Idle);
	
	//Firstcall_Deco
	BTTask_IsFirstCall->Set_DecoratorNodes(Sequence_Interact_Actor, nullptr);
	BTTask_IsFightMode->Set_DecoratorNodes(BTTask_IsOwner_Closed, Selector_NonFight);
	//Owner_Close_Deco
	BTTask_IsOwner_Closed->Set_DecoratorNodes( BTTask_IsTargetMove, Sequence_Fight);
	//IsTargetAttakRange_Deco
	BTTask_IsAtkRange->Set_DecoratorNodes(Sequence_Attack, BTTask_Move);
	//IsSkillRange
	BTTask_IsSkillRange->Set_DecoratorNodes(BTTask_Skill, BTTask_Move);
	//Add InSequence_NoneTarget 
	Sequence_NonTarget->Add_Node(BTTask_Idle);
	Sequence_NonTarget->Add_Node(BTTask_Move);
	Sequence_NonTarget->Add_Node(BTTask_SetRandPos);
	Sequence_NonTarget->Add_Node(BTTask_SelectTarget);
	//------- OnTargeting-Selector---------------
	Selector_OnTarget->Add_Node(BTTask_TargetIsDead);
	Selector_OnTarget->Add_Node(BTTask_TargetHited);
	Selector_OnTarget->Add_Node(Sequence_OnTarget);
	Selector_OnTarget->Add_Node(BTTask_Move);
	//------Attack_Selector
	Selector_Attack->Add_Node(Sequence_Skill);
	Selector_Attack->Add_Node(BTTask_IsAtkRange);
	Selector_Attack->Add_Node(BTTask_Idle);

	//------Attack_Selector
	//Selector_Skill->Add_Node(BTTask_TargetSkillRange);
	//Selector_Skill->Add_Node(BTTask_Idle);
	//-------Ontargeting_Sequence
	Sequence_OnTarget->Add_Node(BTTask_Arrive);
	Sequence_OnTarget->Add_Node(BTTask_IsActor);
	//----------Sequence_OnActor-------------
	Sequence_Interact_Actor->Add_Node(BTTask_Talk);
	Sequence_Interact_Actor->Add_Node(BTTask_Dance);
	//---------BTTask_IsActor---
	BTTask_IsActor->Add_Node(Sequence_Fight);
	BTTask_IsActor->Add_Node(Sequence_Interact_Actor);
	BTTask_IsActor->Add_Node(BTTask_Interact);
	//--Selector_OnOwner--
	Selector_OnOwner->Add_Node(BTTask_IsFightMode);
	
	//-----Selector_NonFight
	Selector_NonFight->Add_Node(BTTask_IsFirstCall);
	Selector_NonFight->Add_Node(BTTask_IsTargetMove);
	//--Sequence_Attack---
	Sequence_Attack->Add_Node(BTTask_GetCanAttack);
	Sequence_Attack->Add_Node(BTTask_Attack);
	//------Sequence_MotveToTarget
	Sequence_MoveToTarget->Add_Node(BTTask_SetGoalPos);
	Sequence_MoveToTarget->Add_Node(BTTask_Move);
	Sequence_MoveToTarget->Add_Node(BTTask_Arrive);
	//----Sequence_Fight
	Sequence_Fight->Add_Node(BTTask_SelectTarget);
	Sequence_Fight->Add_Node(BTTask_HasEnemy);

	//------Sequence_Skill
	Sequence_Skill->Add_Node(BTTask_GetCanSkill);
	Sequence_Skill->Add_Node(BTTask_IsSkillRange);
	//-----Sequence_FollowOwne------
	//Sequence_FollowOwner->Add_Node(BTTask_SelectTarget);
	//Sequence_FollowOwner->Add_Node(BTTask_Move);
	//Test
	////Add in Root
	//Root->Add_Node(BTTask_HasTarget);
	////Add in HasTarget_Deco
	//BTTask_HasTarget->Set_DecoratorNodes(Selector_OnTarget, Sequence_NonTarget);
	//BTTask_IsActor->Set_DecoratorNodes(BTTask_Talk, BTTask_Interact);
	////Add InSequence_NoneTarget 
	//Sequence_NonTarget->Add_Node(BTTask_Idle);
	//Sequence_NonTarget->Add_Node(BTTask_Move);
	//Sequence_NonTarget->Add_Node(BTTask_Dance);
	//Sequence_NonTarget->Add_Node(BTTask_SetRandPos);
	////Sequence_NonTarget->Add_Node(BTTask_SelectTarget);
	////------- OnTargeting-Selector---------------
	//Selector_OnTarget->Add_Node(Sequence_OnTarget);
	//Selector_OnTarget->Add_Node(BTTask_Move);
	////-------Ontargeting_Sequence
	//Sequence_OnTarget->Add_Node(BTTask_Arrive);
	//Sequence_OnTarget->Add_Node(BTTask_IsActor);

}

//void CBT_NPC::Init_Map()
//{
//	for (_uint i = 0; i < (_uint)CNPC::NPC_STATE::STATE_END; ++i)
//	{
//		m_mapNPCStatus.emplace((CNPC::NPC_STATE)i, STATUS::NOT_STARTED);
//	}
//}

void CBT_NPC::Tick(_float _fTimeDelta)
{
	if(Root->Excute(Actor, _fTimeDelta) == STATUS::SUCCESS)
	{
		return;
	}
}

//CNPC::NPC_STATE CBT_NPC::Find_Running()
//{
//	for(auto& iter : m_mapNPCStatus)
//	{
//		if (STATUS::RUNNING == iter.second)
//			return iter.first;
//	}
//
//	return CNPC::NPC_STATE::STATE_END;
//}
//
//void CBT_NPC::Clear_Status(void)
//{
//	for (auto& iter : m_mapNPCStatus)
//	{
//		iter.second = STATUS::NOT_STARTED;
//	}
//
//}

void CBT_NPC::Free()
{
	Safe_Release(Root);
	//Safe_Release(Actor);
}
