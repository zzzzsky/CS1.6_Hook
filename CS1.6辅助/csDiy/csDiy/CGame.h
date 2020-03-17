#pragma once
#include "CGameHook.h"
#include <vector>
#include <algorithm>
#include <stdlib.h>

using namespace std;
typedef struct _stPlayerPos
{
	float x;
	float y; 
	float z;
}stPlayerPos, * pstPlayerPos;

class CGame
{
public:
	HMODULE hModule;										//保存当前exe模块基址
	DWORD * pPlayer;										//当前人物信息基址
	DWORD * pPlayer7c;										//基址+7C
	DWORD * pWeapon;										//保存武器信息基址
	DWORD dwInGame[4];										//判断是否在游戏中的四个偏移量
	stPlayerPos NowPos;										//保存当前地址
	stPlayerPos MarkPos1;									//保存标记地址1
	stPlayerPos MarkPos2;									//保存标记地址2
	float fNowBlood_suoxue = 0;								//保存要锁定的血量值
	CGameHook * pCGameHook;									//一个全局变量指向HOOK类的对象
	DWORD dwEnemyCode;										//保存敌人阵营的编号 1恐怖分子 2反恐精英
	
	vector<DWORD> pAllPeople7c;								//保存所有人物的基址 +7C
	vector<DWORD> pEnemy7c;									//保存所有敌人的基址 +7C
	vector<DWORD> pEnemy4;									//保存所有敌人的详细信息基址 +4
	vector<stPlayerPos> stRandomPos;						//保存随机坐标
	BOOL IsUseDiBaoTianXing = FALSE;						//保存是否允许使用地爆天星
	BOOL IsFirstDiBaoTianXing = FALSE;						//是否是第一次地爆天星
public:
	CGame();
	~CGame();
	DWORD * GetPlayerAddress();								//获取玩家信息地址
	DWORD * GetWeaponAddress();								//获取武器信息基址
	BOOL IsInGame();										//判断是在主界面还是游戏中
	float GetMyBlood();										//获取当前血量
	float GetMyHujia();										//获取护甲值
	int GetNumOfBullet();									//获取当前子弹数
	void GetLocalPos(pstPlayerPos pNowPos);					//获取当前坐标
	DWORD GetMyMoney();										//获取当前金钱数
	void SuoXue(float nowBlood, bool IsFirst);				//实现锁血功能,只有第一次点击锁血的时候锁定的血量永久性保存下载
	void SuperMan();										//实现无敌状态
	void SuManHujia();										//索满护甲
	void SuoDingMoney();									//锁定金币
	void WuXinBulletNum();									//无限子弹
	void NoHouzuoli();										//无后坐力
	bool HookReduceBlood(bool IsHook);						//Hook掉血函数，实现秒杀
	bool HookInfGrenade(bool IsHook);						//无限手雷
	bool Changingview(bool IsFPV);							//切换视角实现穿墙			
	bool BuyAnywhere();										//随地购物
	DWORD GetMyTeam();										//获取当前阵营 1恐怖分子 2反恐精英
	DWORD GetPointToWho();									//指向了谁 敌人 队友 墙
	BOOL CopyPos(pstPlayerPos p1, pstPlayerPos p2);			//复制坐标		传两个指向坐标的地址，直接修改
	BOOL GetAllPeople7c();									//循环遍历获取人物基址，直到满足个数退出
	DWORD FilteEnemy();										//根据找到的所有的人的基址，筛选出敌人还是队友,	返回值是敌人的个数
	BOOL EmeptyPeopleAddr();								//清空获取的人物基址
	VOID GenerateRandomPos();								//生成随机坐标
	VOID DiBaoTianXingCopyPos();							
	BOOL DiBaoTianXing666();								//地爆天星功能
	
	BOOL IsAllEnemyKilled();								//是否所有敌人阵亡

};

