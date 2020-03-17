#pragma once
#include <windows.h>
#include <stdio.h>

//*********************************************秒杀HOOK*********************************************
#define SECKILL_PATCHLENGTH 5						//秒杀HOOK打补丁的长度
#define DEC_BLOODFUNC_OFFSET 0x66CC0				//掉血函数内部离基址的偏移
#define ALLBLOOD	0X42C80000						//100血量
//0AEC6CC0    83EC 3C         sub esp,0x3C
//0AEC6CC3    53              push ebx
//0AEC6CC4    56              push esi
//*********************************************无限手雷HOOK*********************************************
#define INFGRENADE_PATCHLENGTH 9					//秒杀HOOK打补丁的长度
#define DEC_NUMOFFGRENADE_OFFSET	0X753A					//减少手雷数的地址偏移
//1F56753A    49                                  dec ecx; 减少手雷
//1F56753B    8908                                mov dword ptr ds : [eax], ecx
//1F56753D    8B8E C4000000                       mov ecx, dword ptr ds : [esi + 0xC4]
//*********************************************万象天引HOOK*********************************************
#define DRAW_PATCHLENGTH	6
#define DRAW_OFFSET 0X9C0C9
//0AD2C0C9    50              push eax
//0AD2C0CA    6A 00           push 0x0
//0AD2C0CC    51              push ecx
//0AD2C0CD    6A 01           push 0x1
//*********************************************地爆天星手雷爆炸HOOK******************************************
#define BOMB_PATCHLENGTH	7
#define BOMB_OFFSET 0X77C79
//1FAF7C79    D940 08         fld dword ptr ds : [eax + 0x8]
//1FAF7C7C    D95C24 04       fstp dword ptr ss : [esp + 0x4]
//1FAF7C80    D940 0C         fld dword ptr ds : [eax + 0xC]













void  MySecKill();
void MyInfGrenade();
void MyDraw();		
void MyBomb();

class CGameHook
{
public:
	//******************************MP.DLL模块基址******************************
	static HMODULE hMpdll;	
	static DWORD dw_Player;													//保存玩家地址
	static DWORD dw_Player7c;
	//******************************秒杀HOOK相关数据******************************
	static BYTE by_SecKill_OriginalCode[SECKILL_PATCHLENGTH];				//保存原来的地址,以UNHOOK
	static BYTE by_SecKill_HookCode[SECKILL_PATCHLENGTH];					//保存JMP XXXXXX
	static DWORD dw_SecKill_HookFunc;										//保存要HOOK的地址
	static DWORD dw_SecKill_Attacker;										//保存攻击者地址
	static BOOL Is_SecKill_Hooded;											//保存是否已经HOOK过
	static DWORD dw_SecKill_RetAddr;										//保存返回地址
	BOOL SecKillHook(BOOL Is_Hook);
	VOID GetSecKillInfo();
	//******************************无限手雷HOOK相关数据******************************
	static BYTE by_InfGrenade_OriginalCode[INFGRENADE_PATCHLENGTH];			//保存原来的地址,以UNHOOK
	static BYTE by_InfGrenade_HookCode[INFGRENADE_PATCHLENGTH];				//保存JMP XXXXXX
	static DWORD dw_InfGrenade_HookAddr;									//保存要HOOK的地址
	static DWORD dw_InfGrenade_Attacker;									//保存扔手雷的人的基址(+208)
	static BOOL Is_InfGrenade_Hooded;										//保存是否已经HOOK过
	static DWORD dw_InfGrenade_RetAddr;										//保存返回地址
	VOID GetInfGrenadeInfo();	
	BOOL InfGrenadeHook(BOOL Is_Hook);
	//******************************万象天引HOOK相关数据******************************
	static DWORD dw_Draw_PeopleAddr7c;										//保存准星指向人物基址的全局变量
	static BYTE by_Draw_HookCode[DRAW_PATCHLENGTH];							//保存JMP XXXXXX
	static DWORD dw_Draw_HookAddr;											//保存要HOOK的地址
	static DWORD dw_Draw_RetAddr;											//保存返回地址
	static BOOL Is_Draw_Hooded;												//保存是否已经HOOK过
	VOID GetDrawInfo();														//获取相关信息
	BOOL DrawHook();
	//******************************地爆天星HOOK手雷爆炸地点******************************
	static BYTE by_Bomb_OriginalCode[BOMB_PATCHLENGTH];						//保存原来的地址,以UNHOOK
	static BYTE by_Bomb_HookCode[BOMB_PATCHLENGTH];							//保存JMP XXXXXX
	static DWORD dw_Bomb_HookAddr;											//保存要HOOK的地址
	static BOOL Is_Bomb_Hooded;												//保存是否已经HOOK过
	static DWORD dw_Bomb_RetAddr;											//保存返回地址
	static DWORD dw_Bomb_Pos_x;												//保存自定义手雷爆炸的坐标x
	static DWORD dw_Bomb_Pos_y;												//保存自定义手雷爆炸的坐标y
	static DWORD dw_Bomb_Pos_z;												//保存自定义手雷爆炸的坐标z
	VOID GetBombInfo();														//获取相关信息
	BOOL BombHook(BOOL Is_Hook);

	//得到模块基址
	VOID GetMpdllAddr(); 

	CGameHook();
};
