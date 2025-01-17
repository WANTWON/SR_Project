﻿#include "stdafx.h"
#include "..\Public\Level_Hunt.h"
#include "GameInstance.h"
#include "PickingMgr.h"
#include "House.h"
#include "Player.h"
#include "CameraManager.h"
#include "Level_Loading.h"
#include "Portal.h"
#include "WoodWall.h"
#include "DecoObject.h"
#include "DayCycle.h"

CLevel_Hunt::CLevel_Hunt(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
}

HRESULT CLevel_Hunt::Initialize()
{
	
	CGameInstance::Get_Instance()->StopSound(SOUND_BGM);
	CGameInstance::Get_Instance()->StopAll();
	CDayCycle::Get_Instance()->AllRemoveObserver();
	if (FAILED(__super::Initialize()))
		return E_FAIL;
	

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
		return E_FAIL;

	

	CPickingMgr::Get_Instance()->Clear_PickingMgr();
	CPickingMgr::Get_Instance()->Ready_PickingMgr(LEVEL::LEVEL_HUNT);

	CCameraManager::Get_Instance()->Ready_Camera(LEVEL::LEVEL_HUNT);
	m_dwTime = GetTickCount();

	CGameInstance::Get_Instance()->PlaySounds(TEXT("waterlogged_amb_spring_day_LP_DST.wav"), SOUND_GROUND ,0.5f);
	CGameInstance::Get_Instance()->PlaySounds(TEXT("Chaplinesque2_vinyl_mastered.wav"),  SOUND_BGM , 0.3f);
	CDayCycle::Get_Instance()->RegisterObserver(this, CDayCycle::CYCLE_NONSTATIC);
	m_eDayState = CDayCycle::Get_Instance()->Get_DayState();
	m_fMusicTime = GetTickCount();
	return S_OK;
}

void CLevel_Hunt::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Play_Music();
	CDayCycle::Get_Instance()->DayCycleTick();
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	if (m_bNextLevel)
	{
		LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_DestinationLevelIndex();
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, iLevel))))
			return;
	}

	if (!m_bNextLevel)
	{
		CPickingMgr::Get_Instance()->Picking();
	}

	Start_Camera_Motion();
	//Test_QuestComplete_Camera_Motion();

	//CPickingMgr::Get_Instance()->Picking();
	Safe_Release(pGameInstance);
}

void CLevel_Hunt::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("사냥레벨입니다."));
}

HRESULT CLevel_Hunt::Ready_Layer_BackGround(const _tchar * pLayerTag)
{

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	int iTextNum = 0;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_HUNT, TEXT("Layer_Sky"), &iTextNum)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectLoad(TEXT("Prototype_GameObject_Terrain"), LEVEL_HUNT, pLayerTag,
		TEXT("Prototype_Component_VIBuffer_Terrain_Load0"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Water"), LEVEL_HUNT, pLayerTag, _float3(-20.f, -0.01f, -20.f))))
		return E_FAIL;


	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	pPlayer->Set_Position(_float3(7.5, 1, 20));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Hunt::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE		hFile = CreateFile(TEXT("../Bin/Resources/Data/Spider_Stage2.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong		dwByte = 0;
	_float3 ObjectPos(0, 0, 0);
	_uint iNum = 0;

	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Spider"), LEVEL_HUNT, pLayerTag, ObjectPos);
	}

	CloseHandle(hFile);


	
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Hunt::Ready_Layer_Object(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPortal::PORTALDESC PortalDesc;
	PortalDesc.m_eType = CPortal::PORTAL_GAMEPLAY;
	PortalDesc.vPosition = _float3(55.5f, 2.f, 20.f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_HUNT, pLayerTag, &PortalDesc)))
		return E_FAIL;

	/* Load Tree */
	HANDLE		hFile = CreateFile(TEXT("../Bin/Resources/Data/Tree_Stage2.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong		dwByte = 0;
	_float3 ObjectPos(0, 0, 0);
	_uint iNum = 0;

	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Tree"), LEVEL_HUNT, pLayerTag, ObjectPos);
	}
	CloseHandle(hFile);

	/* Load Grass */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/Grass_Stage2.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	ObjectPos = _float3(0, 0, 0);
	iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Grass"), LEVEL_HUNT, pLayerTag, ObjectPos);
	}
	CloseHandle(hFile);

	/* Load Rock */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/Rock_Stage2.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	ObjectPos = _float3(0, 0, 0);
	iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Boulder"), LEVEL_HUNT, pLayerTag, ObjectPos);
	}
	CloseHandle(hFile);

	/* Load Berry */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/Berry_Stage2.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	ObjectPos = _float3(0, 0, 0);
	iNum = 0;

	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Berry_Bush"), LEVEL_HUNT, pLayerTag, ObjectPos);
	}
	CloseHandle(hFile);

	/* Load House */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/House_Stage2.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	CHouse::HOUSEDECS HouseDesc;
	iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(HouseDesc), sizeof(CHouse::HOUSEDECS), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House"), LEVEL_HUNT, TEXT("Layer_House"), &HouseDesc);
	}
	CloseHandle(hFile);

	/* Load Collision Wall */

	hFile = CreateFile(TEXT("../Bin/Resources/Data/Collision_Wall_Stage2.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;
	dwByte = 0;
	CWoodWall::WALLDESC WallDesc;
	iNum = 0;


	/* 타일의 개수 받아오기 */
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(WallDesc), sizeof(CWoodWall::WALLDESC), &dwByte, nullptr);
		WallDesc.etype = CWoodWall::WALL_END;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WoodWall"), LEVEL_HUNT, TEXT("Layer_Wall"), &WallDesc);
	}
	CloseHandle(hFile);


	hFile = CreateFile(TEXT("../Bin/Resources/Data/Deco_Stage2.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	CDecoObject::DECODECS DecoDesc;
	iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(DecoDesc), sizeof(CDecoObject::DECODECS), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_HUNT, TEXT("Layer_Deco"), &DecoDesc);
	}
	CloseHandle(hFile);


	/*PortalDesc.m_eType = CPortal::PORTAL_BOSS;
	PortalDesc.vPosition = _float3(102.f, 2.f, 24.f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_HUNT, pLayerTag, &PortalDesc)))
		return E_FAIL;*/

	Safe_Release(pGameInstance);
	return S_OK;
}


HRESULT CLevel_Hunt::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCameraDynamic::CAMERADESC_DERIVED				CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCameraDynamic::CAMERADESC_DERIVED));

	CameraDesc.vDistance = _float3(0, 8, -6);

	CameraDesc.CameraDesc.vEye = _float3(0, 8, -6);
	CameraDesc.CameraDesc.vAt = _float3(0.f, 0.f, 0.f);

	CameraDesc.CameraDesc.fFovy = D3DXToRadian(30.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 600.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_HUNT, pLayerTag, &CameraDesc)))
		return E_FAIL;

	CameraDesc.CameraDesc.fFovy = D3DXToRadian(45.0f);


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_FPS"), LEVEL_HUNT, pLayerTag, &CameraDesc)))
		return E_FAIL;

	CameraDesc.CameraDesc.fFovy = D3DXToRadian(30.0f);
	CameraDesc.CameraDesc.vEye = _float3(0.f, 6.f, -7.f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Target"), LEVEL_HUNT, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Hunt::Start_Camera_Motion()
{
	if (!m_bTargetCam && m_dwTime + 1000 < GetTickCount())
	{
		CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_TARGET);
		CCameraTarget* pCamera = (CCameraTarget*)CCameraManager::Get_Instance()->Get_CurrentCamera();
		CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_HUNT, TEXT("Layer_Object"));
		pCamera->Set_Target(pGameObject);
		pCamera->Set_GoingMode(true);
		m_bFirst = true;
		m_bTargetCam = true;
		m_dwTime = GetTickCount();
	}

	if (m_dwTime + 5000 < GetTickCount() && m_bFirst)
	{
		CCameraTarget* pCamera = (CCameraTarget*)CCameraManager::Get_Instance()->Get_CurrentCamera();
		pCamera->Set_GoingMode(false);
		m_bFirst = false;
	}
}


void CLevel_Hunt::Play_Music()
{
	
		if (m_fMusicTime + 9000 < GetTickCount())
		{
			if (!m_bMusicStart)
			{
				
					switch (m_eDayState)
					{
					case Client::DAY_MORNING:
						CGameInstance::Get_Instance()->PlaySounds(TEXT("Chaplinesque2_vinyl_mastered.wav"), SOUND_BGM,  0.3f);
						break;
					case Client::DAY_DINNER:
						CGameInstance::Get_Instance()->PlaySounds(TEXT("Chaplinesque2_vinyl_mastered.wav"), SOUND_BGM, 0.3f);
						break;
					case Client::DAY_NIGHT:
						CGameInstance::Get_Instance()->PlaySounds(TEXT("DSS_marsh_mild_NIGHT_LP.wav"), SOUND_BGM,  0.2f);
						break;
					}


				m_bMusicStart = true;
			}
			m_fMusicTime = GetTickCount();
		}
}

void CLevel_Hunt::Update(_uint eDayState)
{
	m_eDayState = DAY_STATE(eDayState);
	m_fMusicTime = GetTickCount();
	CGameInstance::Get_Instance()->StopSound(SOUND_BGM);
	m_bMusicStart = false;
}



CLevel_Hunt * CLevel_Hunt::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Hunt*	pInstance = new CLevel_Hunt(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_Hunt"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Hunt::Free()
{
	__super::Free();


}
