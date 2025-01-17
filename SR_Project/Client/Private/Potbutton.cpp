#include "stdafx.h"
#include "..\Public\Potbutton.h"
#include "GameInstance.h"
#include "Inventory.h"
#include "Mouse.h"
#include "KeyMgr.h"
#include "Player.h"




CPotbutton::CPotbutton(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPotbutton::CPotbutton(const CPotbutton & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPotbutton::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;



	return S_OK;
}

HRESULT CPotbutton::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);


	m_fSizeX = 45.f;
	m_fSizeY = 20.f;
	m_fX = 740.f;
	m_fY = 485.f;



	/*m_firstx = m_fX;
	m_firsty = m_fY;*/

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));


	//CInventory_Manager::Get_Instance()->Get_Inven_list()->front.push_back(this);
	//CInventory_Manager::Get_Instance()->Get_Inven_list().push_back(this);
	//INVENLIST
	//	m_tooltype = (TOOLTYPE)iNum;

	return S_OK;
}

int CPotbutton::Tick(_float fTimeDelta)
{

	if (m_bcheck_pot == true)
	{
		__super::Tick(fTimeDelta);
		CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
		//Safe_AddRef(pinv);

		auto potfront = pinv->Get_Potfront_list();

		/*if (m_makewhat == MAKE_AXE || m_makewhat == MAKE_HAMBAT || m_makewhat == MAKE_FENCE)
		m_fY = 300.f;
		else if (m_makewhat == MAKE_PICK || m_makewhat == MAKE_SHOTTER || m_makewhat == MAKE_POT)
		m_fY = 350.f;
		else if (m_makewhat == MAKE_STAFF || m_makewhat == MAKE_TENT)
		m_fY = 400.f;
		else if (m_makewhat == MAKE_ARMOR)
		m_fY = 450.f;
		else if (m_makewhat == MAKE_HELMET)
		m_fY = 500.f;*/
		texnum = 1;
		for (auto k : *potfront)
		{
			if (k->get_texnum() == ITEMNAME_END)
			{
				texnum = 0;
				break;
			}
		}
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

		/*if (m_fY <= 300.f)
		Open_Craft(fTimeDelta);*/


		RECT		rcRect;
		SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);


		if (PtInRect(&rcRect, ptMouse))
		{
			m_fSizeX = 45.f;
			m_fSizeY = 30.f;
			m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

			if (CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON) && texnum == 1)
			{
				/*craft(m_makewhat);
				pinv->update_craftpont();
				Safe_Release(pinv);*/
				_uint random = rand() % 2 + 1;
				m_pCookPot->Start_Cooking();
				if (!m_bCookStart)
				{
					m_dwTime = GetTickCount();
					m_bCookStart = true;
				}

				CGameInstance* pInstance = CGameInstance::Get_Instance();
				if (random == 1)
					pInstance->PlaySounds(TEXT("cook1.wav"), SOUND_UI, 0.9f);
				else
					pInstance->PlaySounds(TEXT("cook2.wav"), SOUND_UI, 0.9f);

				makewhat = craftready();
				// Timer 2/3 seconds OVER 

				pinv->Off_pot(m_pCookPot);

			}
		}
		else
		{
			m_fSizeX = 45.f;
			m_fSizeY = 20.f;

			m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
		}

		
	}
	if (GetTickCount() > m_dwTime + 4000 && m_bCookStart)
	{
		m_dwTime = GetTickCount();
		m_pCookPot->End_Cooking();
		foreffect		effectdesc;
		ZeroMemory(&effectdesc, sizeof(foreffect));
		m_bCookStart = false;
		effectdesc.pos = m_pCookPot->Get_Position();
		effectdesc.pos.y += 1.25f;
		effectdesc.itemname = craft(makewhat);


		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Poteffect"), LEVEL_GAMEPLAY, TEXT("Layer_Poteffect"), &effectdesc)))
			return OBJ_NOEVENT;
		//pinv->Off_pot();
	}
	
	
	
	return OBJ_NOEVENT;
}

void CPotbutton::Late_Tick(_float fTimeDelta)
{

	if (m_bcheck_pot == true)
	{
		__super::Late_Tick(fTimeDelta);

	/*	CMouse*			pMouse = CMouse::Get_Instance();
		Safe_AddRef(pMouse);

		RECT		rcRect;
		SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
		Safe_AddRef(pinv);

		auto mouse = pinv->Get_Mouse_item_list()->begin();*/


		






		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

		/*Safe_Release(pMouse);
		Safe_Release(pinv);*/
	}

	
	
}

HRESULT CPotbutton::Render()
{
	if (m_bcheck_pot == true)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
			return E_FAIL;

		_float4x4		ViewMatrix;
		D3DXMatrixIdentity(&ViewMatrix);

		m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
		m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(texnum)))
			return E_FAIL;

		if (FAILED(SetUp_RenderState()))
			return E_FAIL;

		m_pVIBufferCom->Render();

		if (FAILED(Release_RenderState()))
			return E_FAIL;

	}

	





	return S_OK;
}

HRESULT CPotbutton::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Potbutton"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPotbutton::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CPotbutton::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CPotbutton * CPotbutton::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPotbutton*	pInstance = new CPotbutton(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPotbutton"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPotbutton::Clone(void* pArg)
{
	CPotbutton*	pInstance = new CPotbutton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPotbutton"));
		Safe_Release(pInstance);
	}
	CInventory_Manager::Get_Instance()->Get_Potbutton_list()->push_back(pInstance);
	return pInstance;

}
ITEMNAME CPotbutton::craftready()
{
	CInventory_Manager*         pInventory_Manager = CInventory_Manager::Get_Instance();

	auto potfront = pInventory_Manager->Get_Potfront_list();

	auto pinven = pInventory_Manager->Get_Inven_list();



	bool stop1 = false;
	bool stop2 = false;


	_uint meat = 0;

	_uint pigtail = 0;
	_uint berry = 0;
	_uint carrot = 0;
	_uint spidermeat = 0;

	for (auto k = potfront->begin(); k != potfront->end(); ++k)
	{

		if ((*k)->get_texnum() == ITEMNAME_MEAT)
			++meat;

		else if ((*k)->get_texnum() == ITEMNAME_PIGTAIL)
			++pigtail;
		else if ((*k)->get_texnum() == ITEMNAME_BERRY)
			++berry;
		else if ((*k)->get_texnum() == ITEMNAME_CARROT)
			++carrot;
		else if ((*k)->get_texnum() == ITEMNAME_SPIDERMEAT)
			++spidermeat;
	}


	if (pigtail == 2 && berry == 2)
	return ITEMNAME_MEATBALL;

	else if (meat == 2 && carrot == 2)
		return ITEMNAME_NUGGET;
		
	else if (berry == 4)
	return ITEMNAME_BERRYPIE;

	else if (spidermeat == 4)
	return ITEMNAME_LASAGNA;

	else	
    return ITEMNAME_TRASH1;
		
	

	
}

ITEMNAME CPotbutton::craft(ITEMNAME what)
{

	CInventory_Manager*         pInventory_Manager = CInventory_Manager::Get_Instance();
	
	auto potfront = pInventory_Manager->Get_Potfront_list();

	auto pinven = pInventory_Manager->Get_Inven_list();


	
	bool stop1 = false;
	bool stop2 = false;

	
	_uint meat = 0;
	
	_uint pigtail = 0;
	_uint berry = 0;
	_uint carrot = 0;
	_uint spidermeat = 0;
	
	for (auto k = potfront->begin(); k != potfront->end(); ++k)
	{
		
		if ((*k)->get_texnum() == ITEMNAME_MEAT)
			++meat;
		
		else if ((*k)->get_texnum() == ITEMNAME_PIGTAIL)
			++pigtail;
		else if ((*k)->get_texnum() == ITEMNAME_BERRY)
			++berry;
		else if ((*k)->get_texnum() == ITEMNAME_CARROT)
			++carrot;
		else if ((*k)->get_texnum() == ITEMNAME_SPIDERMEAT)
			++spidermeat;
	}

	if (what == ITEMNAME_MEATBALL)
	{
		
		for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
		{
			if ((*iter)->get_texnum() == (ITEMNAME_MEATBALL) && (*iter)->get_check() == true)
			{
				(*iter)->plus_itemcount();   //먹은 아이템이 인벤토리에 이미 존재할때 카운트 증가 증가시켰다면 리턴으로 탈출! 못찾았으면? 탐색해야지 새로
											 //check = true;
				return ITEMNAME_MEATBALL;
			}
		}

		for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
		{
			if ((*iter)->get_check() == false || (*iter)->get_texnum() == ITEMNAME_END)
			{
				(*iter)->set_texnum(ITEMNAME_MEATBALL);
				(*iter)->plus_itemcount();

				(*iter)->set_check(true);

				return ITEMNAME_MEATBALL;
			}

		}

	}
	else if (what == ITEMNAME_NUGGET)
	{
		
		for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
		{
			if ((*iter)->get_texnum() == (ITEMNAME_NUGGET) && (*iter)->get_check() == true)
			{
				(*iter)->plus_itemcount();   //먹은 아이템이 인벤토리에 이미 존재할때 카운트 증가 증가시켰다면 리턴으로 탈출! 못찾았으면? 탐색해야지 새로
											 //check = true;
				return ITEMNAME_NUGGET;
			}
		}

		for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
		{
			if ((*iter)->get_check() == false || (*iter)->get_texnum() == ITEMNAME_END)
			{
				(*iter)->set_texnum(ITEMNAME_NUGGET);
				(*iter)->plus_itemcount();

				(*iter)->set_check(true);

				return ITEMNAME_NUGGET;
			}

		}

	
	}
	else if (what == ITEMNAME_BERRYPIE)
	{
		
		for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
		{
			if ((*iter)->get_texnum() == (ITEMNAME_BERRYPIE) && (*iter)->get_check() == true)
			{
				(*iter)->plus_itemcount();   //먹은 아이템이 인벤토리에 이미 존재할때 카운트 증가 증가시켰다면 리턴으로 탈출! 못찾았으면? 탐색해야지 새로
											 //check = true;
				return ITEMNAME_BERRYPIE;
			}
		}

		for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
		{
			if ((*iter)->get_check() == false || (*iter)->get_texnum() == ITEMNAME_END)
			{
				(*iter)->set_texnum(ITEMNAME_BERRYPIE);
				(*iter)->plus_itemcount();

				(*iter)->set_check(true);

				return ITEMNAME_BERRYPIE;
			}

		}

	
	}
	else if (what == ITEMNAME_LASAGNA)
	{
	
		for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
		{
			if ((*iter)->get_texnum() == (ITEMNAME_LASAGNA) && (*iter)->get_check() == true)
			{
				(*iter)->plus_itemcount();   //먹은 아이템이 인벤토리에 이미 존재할때 카운트 증가 증가시켰다면 리턴으로 탈출! 못찾았으면? 탐색해야지 새로
											 //check = true;
				return ITEMNAME_LASAGNA;
			}
		}

		for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
		{
			if ((*iter)->get_check() == false || (*iter)->get_texnum() == ITEMNAME_END)
			{
				(*iter)->set_texnum(ITEMNAME_LASAGNA);
				(*iter)->plus_itemcount();

				(*iter)->set_check(true);

				return ITEMNAME_LASAGNA;
			}

		}

	
    }
	else
	{
		for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
		{
			if ((*iter)->get_texnum() == (ITEMNAME_TRASH1) && (*iter)->get_check() == true)
			{
				(*iter)->plus_itemcount();  
				return ITEMNAME_TRASH1;
			}
		}

		for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
		{
			if ((*iter)->get_check() == false || (*iter)->get_texnum() == ITEMNAME_END)
			{
				(*iter)->set_texnum(ITEMNAME_TRASH1);
				(*iter)->plus_itemcount();

				(*iter)->set_check(true);

				return ITEMNAME_TRASH1;
			}

		}
	}

	
	
}




void CPotbutton::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

