#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "GameInstance.h"
#include "CameraDynamic.h"
#include "PickingMgr.h"

CLevel_GamePlay::CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MainInventory(TEXT("Layer_MainInventory"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MainInventory_back(TEXT("Layer_MainInventory_back"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MainInventory_front(TEXT("Layer_MainInventory_front"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Pont(TEXT("Layer_Pont"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Equipment_back(TEXT("Layer_Equipment_back"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Equipment_front(TEXT("Layer_Equipment_front"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_PlayerStatUI(TEXT("Layer_PlayerStatUI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_StatUIPont(TEXT("Layer_StatUIPont"))))
		return E_FAIL;

	CPickingMgr::Get_Instance()->Ready_PickingMgr(LEVEL_GAMEPLAY);

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	
	
	CPickingMgr::Get_Instance()->Picking();
	
}

void CLevel_GamePlay::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	
	if (FAILED(pGameInstance->Add_GameObjectLoad(TEXT("Prototype_GameObject_Terrain"), LEVEL_GAMEPLAY, pLayerTag,
		TEXT("Prototype_Component_VIBuffer_Terrain_Load0"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Water"), LEVEL_GAMEPLAY, pLayerTag, _float3(-10.f, -1.5f, -10.f))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pig"), LEVEL_GAMEPLAY, pLayerTag, _float3(7.f, 1.f, 5.f))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Spider"), LEVEL_GAMEPLAY, pLayerTag, _float3(10.f, 1.f, 8.f))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Object(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE		hFile = CreateFile(TEXT("../Bin/Resources/Data/Tree.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong		dwByte = 0;
	_float3 ObjectPos(0, 0, 0);
	_uint iNum = 0;

	/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Tree"), LEVEL_GAMEPLAY, pLayerTag, ObjectPos);
	}

	CloseHandle(hFile);

	hFile = CreateFile(TEXT("../Bin/Resources/Data/Grass.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	ObjectPos = _float3(0, 0, 0);
	iNum = 0;

	/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Grass"), LEVEL_GAMEPLAY, pLayerTag, ObjectPos);
	}

	CloseHandle(hFile);


	hFile = CreateFile(TEXT("../Bin/Resources/Data/Rock.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	ObjectPos = _float3(0, 0, 0);
	iNum = 0;

	/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Boulder"), LEVEL_GAMEPLAY, pLayerTag, ObjectPos);
	}

	CloseHandle(hFile);

	hFile = CreateFile(TEXT("../Bin/Resources/Data/Berry.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	ObjectPos = _float3(0, 0, 0);
	iNum = 0;

	/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Berry_Bush"), LEVEL_GAMEPLAY, pLayerTag, ObjectPos);
	}

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BackGround"), LEVEL_GAMEPLAY, pLayerTag, _float3(10.f, 1.f, 5.f))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCameraDynamic::CAMERADESC_DERIVED				CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCameraDynamic::CAMERADESC_DERIVED));

	CameraDesc.iTest = 10;

	CameraDesc.CameraDesc.vEye = _float3(0.f, 2.f, -5.f);
	CameraDesc.CameraDesc.vAt = _float3(0.f, 0.f, 0.f);

	CameraDesc.CameraDesc.fFovy = D3DXToRadian(60.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 600.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_GAMEPLAY, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_PlayerStatUI(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Playerhp"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Playerhunger"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PlayerMentality"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Mouse_item"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;



	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MainInventory(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MainInventory"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BagInventory"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_MainInventory_back(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	for (int i = 0; i < 18; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MainInventory_back"), LEVEL_GAMEPLAY, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MainInventory_front(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	for (int i = 0; i < 18; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MainInventory_front"), LEVEL_GAMEPLAY, pLayerTag, (int*)&i)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Pont(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	for (int i = 0; i < 10; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pont"), LEVEL_GAMEPLAY, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_StatUIPont(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	for (int i = 0; i < 3; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HpPont"), LEVEL_GAMEPLAY, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 3; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_hungerPont"), LEVEL_GAMEPLAY, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 3; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MentalityPont"), LEVEL_GAMEPLAY, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	Safe_Release(pGameInstance);

	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_Equipment_back(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	for (int i = 0; i < 4; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Equipment_back"), LEVEL_GAMEPLAY, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Equipment_front(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	for (int i = 0; i < 4; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Equipment_front"), LEVEL_GAMEPLAY, pLayerTag, (int*)&i)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}



CLevel_GamePlay * CLevel_GamePlay::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_GamePlay*	pInstance = new CLevel_GamePlay(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();


}
