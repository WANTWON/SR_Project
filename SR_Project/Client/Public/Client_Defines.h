#pragma once

#include "Engine_Defines.h"
#include <process.h>

class Item;

extern HWND	g_hWnd;
extern HINSTANCE g_hInst;

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
	enum OBJID { OBJ_PLAYER, OBJ_MOUSE, OBJ_BUTTON, OBJ_MONSTER, OBJ_BULLET, OBJ_ITEM, OBJ_NPC, OBJ_PORTAL, OBJ_END };
	enum DIR_STATE { DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT, DIR_END };
	enum WEAPON_TYPE { WEAPON_HAND, WEAPON_SWORD, WEAPON_STAFF, WEAPON_DART, WEAPON_BOMB, WEAPON_SMOKE, WEAPON_END };
	enum TOOLTYPE { TOOL_GATHERING, TOOL_WEAPON, TOOL_TEST1, TOOL_TEST2, TOOL_END };
	enum ITEMID { ITEM_FOOD, ITEM_EQUIPMENT, ITEM_MATERIAL, ITEM_BAG, ITEM_ARMOR, ITEM_HAND, ITEM_HAT, ITEM_END };
	enum INTERACTOBJ_ID { TREE, GRASS, BOULDER, BERRYBUSH, ITEMS, CARROT, ID_END };
	enum ITEMNAME {
		ITEMNAME_ARMOR, ITEMNAME_AXE, ITEMNAME_BAG, ITEMNAME_BERRY, ITEMNAME_SHOTTER, ITEMNAME_WOOD, ITEMNAME_CARROT, ITEMNAME_COAL,
		ITEMNAME_STAFF, ITEMNAME_GRASS, ITEMNAME_ROCK, ITEMNAME_HELMET, ITEMNAME_GOLD, ITEMNAME_HAMBAT, ITEMNAME_WOOD2, ITEMNAME_MEAT,
		ITEMNAME_MONSTERMEAT, ITEMNAME_PICK, ITEMNAME_PIGTAIL, ITEMNAME_ROCK2, ITEMNAME_ROPE, ITEMNAME_WEB, ITEMNAME_TORCH, ITEMNAME_END
	};

	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	typedef struct ObjectInfo
	{
		float iMaxHp;
		float iCurrentHp;
		//vector<Item*> vItemDrops;
	}OBJINFO;

	typedef struct tagBulletData {
		_bool bIsPlayerBullet;
		WEAPON_TYPE eWeaponType;
		_float3 vPosition;
		DIR_STATE eDirState;
		_float3 vLook;
		_float3	vTargetPos;
	}BULLETDATA;
}

using namespace Client;