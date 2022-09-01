#pragma once

class Item;

namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	typedef struct ObjectInfo
	{
		float iMaxHp;
		float iCurrentHp;
		//vector<Item*> vItemDrops;
	}OBJINFO;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
}

#include <process.h>

// Client_Defines.h

extern HWND		g_hWnd;
extern HINSTANCE g_hInst;

using namespace Client;


/* Enum */

enum OBJID { OBJ_PLAYER, OBJ_MOUSE, OBJ_BUTTON, OBJ_MONSTER, OBJ_BULLET, OBJ_ITEM, OBJ_NPC, OBJ_PORTAL, OBJ_END };

enum ITEMID { ITEM_FOOD, ITEM_EQUIPMENT, ITEM_MATERIAL, ITEM_BAG, ITEM_ARMOR, ITEM_HAND, ITEM_HAT, ITEM_END };

enum INTERACTOBJ_ID { TREE, GRASS, BOULDER, BERRYBUSH, ITEMS, CARROT, ID_END };

enum class DIR_STATE { DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT, DIR_END };

enum class WEAPON_TYPE { WEAPON_HAND, WEAPON_SWORD, WEAPON_STAFF, WEAPON_DART, WEAPON_END };

enum ITEMNAME { ITEMNAME_ARMOR, ITEMNAME_AXE, ITEMNAME_BAG, ITEMNAME_BERRY, ITEMNAME_SHOTTER, ITEMNAME_WOOD , ITEMNAME_CARROT, ITEMNAME_COAL,
	ITEMNAME_STAFF, ITEMNAME_GRASS,ITEMNAME_ROCK, ITEMNAME_HELMET, ITEMNAME_GOLD, ITEMNAME_HAMBAT, ITEMNAME_WOOD2, ITEMNAME_MEAT, ITEMNAME_MONSTERMEAT,
	ITEMNAME_PICK, ITEMNAME_PIGTAIL, ITEMNAME_ROCK2, ITEMNAME_ROPE, ITEMNAME_WEB, ITEMNAME_TORCH,ITEMNAME_END
};

enum TOOLTYPE { TOOL_GATHERING, TOOL_WEAPON, TOOL_TEST1, TOOL_TEST2, TOOL_END };

enum MAKEWHAT {MAKE_AXE, MAKE_PICK , MAKE_HAMBAT , MAKE_SHOTTER , MAKE_STAFF , MAKE_ARMOR , MAKE_HELMET , MAKE_END};

