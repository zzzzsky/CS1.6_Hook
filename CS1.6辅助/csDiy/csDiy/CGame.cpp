#include "stdafx.h"
#include "CGame.h"


CGame::CGame()
{
	pCGameHook = new CGameHook();		//给秒杀类申请空间
	//判断是否正在游戏中的四个地址
	dwInGame[0] = 0X114B98C;
	dwInGame[1] = 0x114BB90;
	dwInGame[2] = 0X1A628EC;
	dwInGame[3] = 0X1B1FBBC;
	//获取当前exe模块的基址
	hModule = GetModuleHandle(NULL);
	if (!hModule)
	{
		OutputDebugString("获取基址失败!\n");
		return;
	}
	for (int i = 0; i < 4; i++)
	{
		dwInGame[i] = (DWORD)hModule + dwInGame[i];
	}
	
	//给坐标赋值
	ZeroMemory(&NowPos, 12);
	ZeroMemory(&MarkPos1, 12);
	ZeroMemory(&MarkPos2, 12);
}

CGame::~CGame()
{
	if (pCGameHook)
	{
		delete pCGameHook;
		pCGameHook = NULL;
	}
		
}

DWORD  * CGame::GetPlayerAddress()
{
	DWORD  * a = (DWORD *)((DWORD)hModule + 0X11069BC);
	DWORD * ptr = (DWORD *)*a;
	DWORD * ptr1 = (DWORD *)*(DWORD *)((DWORD)ptr + 0X7C);
	pPlayer7c = ptr1;
	CGameHook::dw_Player7c = (DWORD)ptr1;
	pPlayer = (DWORD *)*(DWORD *)((DWORD)ptr1 + 0X4);				//当前人物基址
	return pPlayer;
}

DWORD * CGame::GetWeaponAddress()
{
	DWORD  * a = (DWORD *)((DWORD)hModule + 0X11069BC);
	DWORD * ptr = (DWORD *)*a;
	DWORD * ptr1 = (DWORD *)*(DWORD *)((DWORD)ptr + 0X7C);
	pWeapon = (DWORD *)*(DWORD *)((DWORD)ptr1 + 0X5EC);				
	return pWeapon;
}

BOOL CGame::IsInGame()
{
	int i = 0;
	for ( i = 0; i < 4; i++)
	{
		if (!*(DWORD *)dwInGame[i])				//数组中的四个数全部是1才是正在游戏中
			break;
	}
	if (i == 4)
		return true;
	return false;
}

float CGame::GetMyBlood()
{
	float blood = *(float * )((DWORD)pPlayer + 0X160);
	return blood;
}

float CGame::GetMyHujia()
{
	float hujia = *(float *)((DWORD)pPlayer + 0X1BC);
	return hujia;
}

int CGame::GetNumOfBullet()
{
	pWeapon = GetWeaponAddress();
	int num = *(int *)((DWORD)pWeapon + 0XCC);
	return num;
}

void CGame::GetLocalPos(pstPlayerPos pNowPos)
{
	pNowPos->x = *(float *)((DWORD)pPlayer + 0X8);
	pNowPos->y = *(float *)((DWORD)pPlayer + 0XC);
	pNowPos->z = *(float *)((DWORD)pPlayer + 0X10);
}

DWORD CGame::GetMyMoney()
{
	DWORD myMoney = *(DWORD *)((DWORD)pPlayer7c + 0X1CC);
	return myMoney;
}

void CGame::SuoXue(float nowBlood, bool IsFirst)
{
	if (IsFirst == true)
	{
		fNowBlood_suoxue = nowBlood;
	}
	float * pblood = (float *)((DWORD)pPlayer + 0X160);
	*pblood = fNowBlood_suoxue;
}

void CGame::SuperMan()
{
	float * pblood = (float *)((DWORD)pPlayer + 0X16C);
	*pblood = 0;
}

void CGame::SuManHujia()
{
	float * hujia = (float *)((DWORD)pPlayer + 0X1BC);
	*hujia = 999;
}

void CGame::SuoDingMoney()
{
	*(DWORD *)((DWORD)pPlayer7c + 0X1CC) = 16000;
}

void CGame::WuXinBulletNum()
{
	int * num = (int *)((DWORD)pWeapon + 0XCC);
	*num = 100;
}						//无限子弹

void CGame::NoHouzuoli()
{
	int * num = (int *)((DWORD)pWeapon + 0X100);
	*num = 0;					//无限子弹
}

bool CGame::HookReduceBlood(bool IsHook)
{
	pCGameHook->SecKillHook(IsHook);
	return false;
}

bool CGame::HookInfGrenade(bool IsHook)
{
	pCGameHook->InfGrenadeHook(IsHook);
	return false;
}

bool CGame::Changingview(bool IsFPV)
{
	DWORD *pView = (DWORD *)(*(DWORD *)((DWORD)pPlayer + 0X208) + 0X2C4);
	if (IsFPV)
	{
		*pView = 0;
		OutputDebugString("切换为第一人称视角!");
	}
	else
	{
		*pView = 3;
		OutputDebugString("切换为自由视角，可以穿墙!");
	}
	
	return true;
}

bool CGame::BuyAnywhere()
{
	DWORD * a = (DWORD *)((DWORD)pPlayer7c + 0X3C0);
	*a = 1;
	return false;
}

DWORD CGame::GetMyTeam()
{
	DWORD a = *(DWORD *)((DWORD)pPlayer7c + 0X1C8);
	if (a == 1)
		dwEnemyCode = 2;
	if(a == 2)
		dwEnemyCode = 1;

	return a;
}

DWORD CGame::GetPointToWho()
{
	DWORD who = *(DWORD *)((DWORD)pPlayer7c + 0X710);
	return who;
}

BOOL CGame::CopyPos(pstPlayerPos p1, pstPlayerPos p2)
{
	p1->x = p2->x;
	p1->y = p2->y;
	p1->z = p2->z;
	return 0;
}

BOOL CGame::GetAllPeople7c()
{
	DWORD p7c = *(DWORD *)(*(DWORD *)((DWORD)hModule + 0X1033240) + 0X7C);
	pAllPeople7c.push_back(p7c);
	sort(pAllPeople7c.begin(), pAllPeople7c.end());
	pAllPeople7c.erase(unique(pAllPeople7c.begin(), pAllPeople7c.end()), pAllPeople7c.end());

	return pAllPeople7c.size() >= 19; 
}

DWORD CGame::FilteEnemy()
{
	for (int i = 0; i < pAllPeople7c.size(); i++)
	{
		if (*(DWORD *)((DWORD)pAllPeople7c[i] + 0X1C8) == dwEnemyCode)			//如果是敌人
		{
			pEnemy7c.push_back(pAllPeople7c[i]);
		}
	}

	for (int i = 0; i < pEnemy7c.size(); i++)
	{
		pEnemy4.push_back(*(DWORD *)((DWORD)pEnemy7c[i] + 4));
		
		char buffer[10] = { 0 };
		sprintf_s(buffer, "%#X", pEnemy4[i]);
		OutputDebugString(buffer);
	}
	char buffer[20] = { 0 };
	sprintf_s(buffer, "人数 :%d", pEnemy4.size());
	OutputDebugString(buffer);

	return pEnemy4.size();
}

BOOL CGame::EmeptyPeopleAddr()
{
	pEnemy4.clear();
	pEnemy7c.clear();
	pAllPeople7c.clear();
	stRandomPos.clear();
	return 0;
}

VOID CGame::GenerateRandomPos()
{
	stPlayerPos tempPos = {0,0,0};

	for (int i = 0; i < pEnemy4.size(); i++)
	{
		tempPos.x = NowPos.x + rand() % 100 - 50;
		tempPos.y = NowPos.y + rand() % 100 - 50;
		tempPos.z = NowPos.z + rand() % 70 - 25;

		stRandomPos.push_back(tempPos);						//生成随机坐标
	}

	return VOID();
}

VOID CGame::DiBaoTianXingCopyPos()
{
	try
	{
		stPlayerPos tempPos = { 0,0,0 };

		for (int i = 0; i < pEnemy4.size(); i++)
		{
			tempPos.x = stRandomPos[i].x;
			tempPos.y = stRandomPos[i].y;
			tempPos.z = stRandomPos[i].z;

			pCGameHook->dw_Bomb_Pos_x = *(DWORD *)&tempPos.x;
			pCGameHook->dw_Bomb_Pos_y = *(DWORD *)&tempPos.y;
			pCGameHook->dw_Bomb_Pos_z = *(DWORD *)&tempPos.z;

			CopyPos(pstPlayerPos((DWORD)pEnemy4[i] + 8), &tempPos);
		}
	}
	catch (...)
	{
		OutputDebugString("给坐标赋值出现异常!");
	}

}

BOOL CGame::DiBaoTianXing666()
{
	if (!IsUseDiBaoTianXing)
	{
		OutputDebugString("目前不允许使用地爆天星功能");
		return false;
	}
	if (!IsFirstDiBaoTianXing)
	{
		OutputDebugString("地爆天星功能每局限用一次");
		return false;
	}
	try
	{

		OutputDebugString("正在使用地爆天星功能");
		GenerateRandomPos();
		Sleep(1500);
		DiBaoTianXingCopyPos();
		pCGameHook->BombHook(TRUE);
		IsFirstDiBaoTianXing = FALSE;
	}
	catch (...)
	{
		OutputDebugString("地爆天星功能异常!\n");
	}


	return true;
}

BOOL CGame::IsAllEnemyKilled()
{
	BOOL ret = TRUE;
	for (int i = 0; i < pEnemy4.size(); i++)
	{
		float x = *(float *)((DWORD)pEnemy4[i] + 0X160);
		if (x > 1.1)
		{
			ret = FALSE;
			break;
		}
	}
	return ret;
}
