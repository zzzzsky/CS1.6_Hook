#include "stdafx.h"
#include "CGameHook.h"


HMODULE CGameHook::hMpdll = 0;															//保存此模块基址
DWORD CGameHook::dw_Player = 0;															//保存玩家地址
DWORD CGameHook::dw_Player7c = 0;														//保存玩家地址
//******************************秒杀HOOK相关数据******************************
BYTE CGameHook::by_SecKill_HookCode[SECKILL_PATCHLENGTH] = { 0 };						//保存JMP XXXXXX
BYTE CGameHook::by_SecKill_OriginalCode[SECKILL_PATCHLENGTH] = { 0 };					//保存JMP XXXXXX
DWORD CGameHook::dw_SecKill_HookFunc = 0;												//保存要HOOK的函数首地址
DWORD CGameHook::dw_SecKill_Attacker = 0;												//保存攻击者地址
BOOL CGameHook::Is_SecKill_Hooded = 0;													//保存是否已经HOOK过
DWORD CGameHook::dw_SecKill_RetAddr = 0;												//保存返回地址
//******************************无限手雷HOOK相关数据******************************
BYTE CGameHook::by_InfGrenade_OriginalCode[INFGRENADE_PATCHLENGTH] = {0};				//保存原来的地址,以UNHOOK
BYTE CGameHook::by_InfGrenade_HookCode[INFGRENADE_PATCHLENGTH] = {0};					//保存JMP XXXXXX
DWORD CGameHook::dw_InfGrenade_HookAddr = 0;											//保存要HOOK的地址
DWORD CGameHook::dw_InfGrenade_Attacker = 0;											//保存扔手雷的人的基址(+208)
BOOL CGameHook::Is_InfGrenade_Hooded = 0;												//保存是否已经HOOK过
DWORD CGameHook::dw_InfGrenade_RetAddr = 0;												//保存返回地址
//******************************万象天引相关数据******************************
DWORD CGameHook::dw_Draw_PeopleAddr7c = 0;												//保存准星指向人物基址的全局变量
BYTE CGameHook::by_Draw_HookCode[DRAW_PATCHLENGTH] = {0};								//保存JMP XXXXXX
DWORD CGameHook::dw_Draw_HookAddr = 0;													//保存要HOOK的地址
DWORD CGameHook::dw_Draw_RetAddr = 0;													//保存返回地址
BOOL CGameHook::Is_Draw_Hooded = 0;														//保存是否被HOOK过
//*******************************地爆天星相关数据*******************************
BYTE CGameHook::by_Bomb_OriginalCode[BOMB_PATCHLENGTH];									//保存原来的地址,以UNHOOK
BYTE CGameHook::by_Bomb_HookCode[BOMB_PATCHLENGTH];										//保存JMP XXXXXX
DWORD CGameHook::dw_Bomb_HookAddr;														//保存要HOOK的地址
BOOL CGameHook::Is_Bomb_Hooded;															//保存是否已经HOOK过
DWORD CGameHook::dw_Bomb_RetAddr;														//保存返回地址
DWORD CGameHook::dw_Bomb_Pos_x;															//保存自定义手雷爆炸的坐标x
DWORD CGameHook::dw_Bomb_Pos_y;															//保存自定义手雷爆炸的坐标y
DWORD CGameHook::dw_Bomb_Pos_z;															//保存自定义手雷爆炸的坐标z

void __declspec(naked) MySecKill()
{
	/*

	|EIP
	|Arg1 = 0C29DBFC		//攻击者基址
	|Arg2 = 0C29DBFC		//攻击者基址
	|Arg3 = 41500000		//掉的血量
	|Arg4 = 00001002		//未知


	*/
	__asm {
		pushad;
		pushfd;

		mov eax, dword ptr ss : [esp + 0X28];				//保存攻击者地址
		mov dword ptr ds : [CGameHook::dw_SecKill_Attacker], eax;
		mov ebx, dword ptr ds : [CGameHook::dw_Player];

		cmp eax, ebx;										//判断是不是自己攻击的别人
		jnz label;											//如果不是，结束			
		mov ebx, ALLBLOOD;
		mov dword ptr ss : [esp + 0X28 + 0X8], ebx;			//如果攻击者是自己的话，就将血量设置为100
	label:
		popfd;
		popad;

		sub esp, 0x3C;
		push ebx;
		push esi;

		jmp CGameHook::dw_SecKill_RetAddr;
	}
}


void __declspec(naked) MyInfGrenade()
{
	__asm {
		pushad;
		pushfd;

		mov dword ptr ds : [CGameHook::dw_InfGrenade_Attacker], ebx;
		
		mov eax, dword ptr ds : [CGameHook::dw_Player];	//保存+208后的自身地址
		add eax, 0X208;
		mov eax, dword ptr ds:[eax];	

		cmp eax, ebx;

		jz label;

		popfd;
		popad;

		dec ecx;
		mov dword ptr ds : [eax], ecx;
		mov ecx, dword ptr ds : [esi + 0xC4];
		jmp CGameHook::dw_InfGrenade_RetAddr;
label:
		popfd;
		popad;
		
		nop;
		mov dword ptr ds : [eax], ecx;
		mov ecx, dword ptr ds : [esi + 0xC4];

		jmp CGameHook::dw_InfGrenade_RetAddr;
	}
}

void __declspec(naked) MyDraw()
{
	__asm {
		pushad;			
		pushfd;

		mov eax, dword ptr ss : [esp + 0X24 - 0XD8];
		cmp eax, dword ptr ds : [CGameHook::dw_Player7c];
		jz label;

		mov dword ptr ds : [CGameHook::dw_Draw_PeopleAddr7c], eax;	//保存指向人物的基址

	label:
		popfd;
		popad;
		
		push eax;
		push 0x0;
		push ecx;
		push 0x1;
	
		jmp CGameHook::dw_Draw_RetAddr;
	}
}

void __declspec(naked) MyBomb()
{
	__asm {
		pushad;
		pushfd;

		lea eax, dword ptr ds : [eax + 0X8];
		mov ebx, dword ptr ds : [CGameHook::dw_Bomb_Pos_x];
		mov dword ptr ds:[eax], ebx
		mov ebx, dword ptr ds : [CGameHook::dw_Bomb_Pos_y];
		mov dword ptr ds : [eax + 0X4], ebx
		mov ebx, dword ptr ds : [CGameHook::dw_Bomb_Pos_z];
		mov dword ptr ds : [eax + 0X8], ebx

		popfd;
		popad;

		fld dword ptr ds : [eax + 0x8];
		fstp dword ptr ss : [esp + 0x4];
		fld dword ptr ds : [eax + 0xC];
		
		jmp CGameHook::dw_Bomb_RetAddr;
	}
}



VOID CGameHook::GetInfGrenadeInfo()
{
	dw_InfGrenade_HookAddr = (DWORD)hMpdll + DEC_NUMOFFGRENADE_OFFSET;
	memset(&by_InfGrenade_HookCode[0], 0X90, INFGRENADE_PATCHLENGTH);
	by_InfGrenade_HookCode[0] = 0XE9;
	*(DWORD *)&by_InfGrenade_HookCode[1] = (DWORD)MyInfGrenade - dw_InfGrenade_HookAddr - 5;
	dw_InfGrenade_RetAddr = dw_InfGrenade_HookAddr + INFGRENADE_PATCHLENGTH;
	if (!Is_InfGrenade_Hooded)
		ReadProcessMemory(GetCurrentProcess(), (LPVOID)dw_InfGrenade_HookAddr, by_InfGrenade_OriginalCode, INFGRENADE_PATCHLENGTH, NULL);
}

BOOL CGameHook::InfGrenadeHook(BOOL Is_Hook)
{
	BOOL Ret = FALSE;
	DWORD dwOldProtect = 0;
	if (Is_Hook)		//如果是HOOK
	{
		if (!Is_InfGrenade_Hooded)	//之前没有被HOOK
		{
			VirtualProtect((LPVOID)dw_InfGrenade_HookAddr, INFGRENADE_PATCHLENGTH, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			WriteProcessMemory(GetCurrentProcess(), (LPVOID)dw_InfGrenade_HookAddr, by_InfGrenade_HookCode, INFGRENADE_PATCHLENGTH, NULL);
			VirtualProtect((LPVOID)dw_InfGrenade_HookAddr, INFGRENADE_PATCHLENGTH, dwOldProtect, NULL);
			Ret = TRUE;
			Is_Hook = TRUE;
		}
	}
	else
	{
		if (!Is_InfGrenade_Hooded)	//之前没有被HOOK
		{
			VirtualProtect((LPVOID)dw_InfGrenade_HookAddr, INFGRENADE_PATCHLENGTH, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			WriteProcessMemory(GetCurrentProcess(), (LPVOID)dw_InfGrenade_HookAddr, by_InfGrenade_OriginalCode, INFGRENADE_PATCHLENGTH, NULL);
			VirtualProtect((LPVOID)dw_InfGrenade_HookAddr, INFGRENADE_PATCHLENGTH, dwOldProtect, NULL);
			Ret = TRUE;
			Is_Hook = FALSE;
		}
	}

	return Ret;
}

VOID CGameHook::GetDrawInfo()
{
	dw_Draw_HookAddr = (DWORD)hMpdll + DRAW_OFFSET;
	memset(&by_Draw_HookCode[0], 0X90, DRAW_PATCHLENGTH);
	by_Draw_HookCode[0] = 0XE9;
	*(DWORD *)&by_Draw_HookCode[1] = (DWORD)MyDraw - dw_Draw_HookAddr - 5;
	dw_Draw_RetAddr = dw_Draw_HookAddr + DRAW_PATCHLENGTH;
}

BOOL CGameHook::DrawHook()
{
	BOOL Ret = FALSE;
	DWORD dwOldProtect = 0;

	if (!Is_Draw_Hooded)
	{
		VirtualProtect((LPVOID)dw_Draw_HookAddr, DRAW_PATCHLENGTH, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)dw_Draw_HookAddr, by_Draw_HookCode, DRAW_PATCHLENGTH, NULL);
		VirtualProtect((LPVOID)dw_Draw_HookAddr, DRAW_PATCHLENGTH, dwOldProtect, NULL);
		Is_Draw_Hooded = TRUE;
		Ret = TRUE;
	}
	return Ret;
}

VOID CGameHook::GetBombInfo()
{
	dw_Bomb_HookAddr = (DWORD)hMpdll + BOMB_OFFSET;
	memset(&by_Bomb_HookCode[0], 0X90, BOMB_PATCHLENGTH);
	by_Bomb_HookCode[0] = 0XE9;
	*(DWORD *)&by_Bomb_HookCode[1] = (DWORD)MyBomb - dw_Bomb_HookAddr - 5;
	dw_Bomb_RetAddr = dw_Bomb_HookAddr + BOMB_PATCHLENGTH;
	if (!Is_Bomb_Hooded)
		ReadProcessMemory(GetCurrentProcess(), (LPVOID)dw_Bomb_HookAddr, by_Bomb_OriginalCode, BOMB_PATCHLENGTH, NULL);
}

BOOL CGameHook::BombHook(BOOL Is_Hook)
{
	BOOL Ret = FALSE;
	DWORD dwOldProtect = 0;
	if (Is_Hook)		//如果是HOOK
	{
		if (!Is_Bomb_Hooded)	//之前没有被HOOK
		{
			VirtualProtect((LPVOID)dw_Bomb_HookAddr, BOMB_PATCHLENGTH, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			WriteProcessMemory(GetCurrentProcess(), (LPVOID)dw_Bomb_HookAddr, by_Bomb_HookCode, BOMB_PATCHLENGTH, NULL);
			VirtualProtect((LPVOID)dw_Bomb_HookAddr, BOMB_PATCHLENGTH, dwOldProtect, NULL);
			Ret = TRUE;
			Is_Hook = TRUE;
		}
	}
	else
	{
		if (!Is_Bomb_Hooded)	//之前没有被HOOK
		{
			VirtualProtect((LPVOID)dw_Bomb_HookAddr, BOMB_PATCHLENGTH, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			WriteProcessMemory(GetCurrentProcess(), (LPVOID)dw_Bomb_HookAddr, by_Bomb_OriginalCode, BOMB_PATCHLENGTH, NULL);
			VirtualProtect((LPVOID)dw_Bomb_HookAddr, BOMB_PATCHLENGTH, dwOldProtect, NULL);
			Ret = TRUE;
			Is_Hook = FALSE;
		}
	}

	return Ret;
}

VOID CGameHook::GetMpdllAddr()
{
	hMpdll = GetModuleHandle("mp.dll");
}

CGameHook::CGameHook()
{
	
}

BOOL CGameHook::SecKillHook(BOOL Is_Hook)
{
	BOOL Ret = FALSE;
	DWORD dwOldProtect = 0;
	if (Is_Hook)		//如果是HOOK
	{
		if (!Is_SecKill_Hooded)	//之前没有被HOOK
		{
			VirtualProtect((LPVOID)dw_SecKill_HookFunc, SECKILL_PATCHLENGTH, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			WriteProcessMemory(GetCurrentProcess(), (LPVOID)dw_SecKill_HookFunc, by_SecKill_HookCode, SECKILL_PATCHLENGTH, NULL);
			VirtualProtect((LPVOID)dw_SecKill_HookFunc, SECKILL_PATCHLENGTH, dwOldProtect, NULL);
			Ret = TRUE;
			Is_Hook = TRUE;
		}
	}
	else
	{
		if (!Is_SecKill_Hooded)	//之前没有被HOOK
		{
			VirtualProtect((LPVOID)dw_SecKill_HookFunc, SECKILL_PATCHLENGTH, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			WriteProcessMemory(GetCurrentProcess(), (LPVOID)dw_SecKill_HookFunc, by_SecKill_OriginalCode, SECKILL_PATCHLENGTH, NULL);
			VirtualProtect((LPVOID)dw_SecKill_HookFunc, SECKILL_PATCHLENGTH, dwOldProtect, NULL);
			Ret = TRUE;
			Is_Hook = FALSE;
		}
	}

	return Ret;
}

VOID CGameHook::GetSecKillInfo()
{
	dw_SecKill_HookFunc = (DWORD)hMpdll + DEC_BLOODFUNC_OFFSET;
	memset(&by_SecKill_HookCode[0], 0X90, SECKILL_PATCHLENGTH);
	by_SecKill_HookCode[0] = 0XE9;
	*(DWORD *)&by_SecKill_HookCode[1] = (DWORD)MySecKill - dw_SecKill_HookFunc - SECKILL_PATCHLENGTH;
	dw_SecKill_RetAddr = dw_SecKill_HookFunc + SECKILL_PATCHLENGTH;
	if (!Is_SecKill_Hooded)
		ReadProcessMemory(GetCurrentProcess(), (LPVOID)dw_SecKill_HookFunc, by_SecKill_OriginalCode, SECKILL_PATCHLENGTH, NULL);
}
