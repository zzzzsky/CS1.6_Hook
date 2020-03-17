// CMyWin.cpp: 实现文件
//

#include "stdafx.h"
#include "csDiy.h"
#include "CMyWin.h"
#include "afxdialogex.h"
#include <string>
#include "CGameHook.h"
using namespace std;
// CMyWin 对话框

IMPLEMENT_DYNAMIC(CMyWin, CDialogEx)

CMyWin::CMyWin(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN, pParent)
	, m_dwBlood(0)
	, m_changeBlood(0)
	, m_posX(0)
	, m_posY(0)
	, m_posZ(0)
	, m_hujia(0)
	, m_numOfbullet(0)
{
	pGame = new CGame();
}

CMyWin::~CMyWin()
{
}

void CMyWin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_BLOOD, m_dwBlood);
	DDX_Text(pDX, IDC_EDIT_CHANGEBLOOD, m_changeBlood);
	DDX_Text(pDX, IDC_EDIT_POSX, m_posX);
	DDX_Text(pDX, IDC_EDIT_POSY, m_posY);
	DDX_Text(pDX, IDC_EDIT_POSZ, m_posZ);
	DDX_Text(pDX, IDC_EDIT_HUJIA, m_hujia);
}


BEGIN_MESSAGE_MAP(CMyWin, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_BLOOD, &CMyWin::OnBnClickedButtonBlood)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_MARKPOS3, &CMyWin::OnBnClickedButtonMarkpos3)
	ON_BN_CLICKED(IDC_BUTTON_MARKPOS2, &CMyWin::OnBnClickedButtonMarkpos2)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_POS1, &CMyWin::OnBnClickedButtonMovePos1)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_POS2, &CMyWin::OnBnClickedButtonMovePos2)
	ON_BN_CLICKED(IDC_CHECK_SUOXUE, &CMyWin::OnBnClickedCheckSuoxue)
	ON_BN_CLICKED(IDC_CHECK_WUDI, &CMyWin::OnBnClickedCheckWudi)
	ON_BN_CLICKED(IDC_CHECK_HUJIA, &CMyWin::OnBnClickedCheckHujia)
	ON_BN_CLICKED(IDC_CHECK_MORE_BULLET, &CMyWin::OnBnClickedCheckMoreBullet)
	ON_BN_CLICKED(IDC_CHECK_WUHOUZUO, &CMyWin::OnBnClickedCheckWuhouzuo)

	ON_BN_CLICKED(IDC_CHECK_SECKILL, &CMyWin::OnBnClickedCheckSeckill)
	ON_BN_CLICKED(IDC_INF_GRENADE, &CMyWin::OnBnClickedInfGrenade)
	ON_BN_CLICKED(IDC_CHECK_CHANGEVIEW, &CMyWin::OnBnClickedCheckChangeview)
	ON_BN_CLICKED(IDC_CHECK_BUYANYWHERE, &CMyWin::OnBnClickedCheckBuyanywhere)
	ON_BN_CLICKED(IDC_CHECK_SUODINGMONEY, &CMyWin::OnBnClickedCheckSuodingmoney)
END_MESSAGE_MAP()


void CMyWin::OnClose()
{
	::MessageBox(NULL, "请用辅助注入器卸载辅助", "提示", MB_OKCANCEL);
}

int CMyWin::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	//一直检测是否处在游戏当中
	SetTimer(0, 1000, NULL);					//检测是否正在游戏，如果是则开启辅助，如果不是，那么隐藏辅助
	

	return 0;
}

void CMyWin::OnTimer(UINT_PTR nIDEvent)
{
	static char buffer[100] = { 0 };
	static long flag = 0;	
	switch (nIDEvent)
	{
	case 0:					//不断检测是否在游戏中	
		try
		{
			if (pGame->IsInGame())						//如果正在游戏
			{
				flag++;
				if (flag == 1)							//每一次进入游戏所有基址要重新获取
				{
					srand((size_t)time(NULL));			//下随机种子，以便后面地爆天星功能
					pGame->GetWeaponAddress();
					CGameHook::dw_Player = (DWORD)pGame->GetPlayerAddress();
					pGame->GetMyTeam();					//返回值是我方阵营,在函数内部获取地方阵营
					//显示辅助窗口
					ShowWindow(SW_SHOW);
					//获取HOOK相关的信息
					pGame->pCGameHook->GetMpdllAddr();
					pGame->pCGameHook->GetSecKillInfo();
					pGame->pCGameHook->GetInfGrenadeInfo();
					pGame->pCGameHook->GetBombInfo();
					pGame->IsFirstDiBaoTianXing = TRUE;

					if (!pGame->pCGameHook->Is_Draw_Hooded)		//一开启游戏立马 万象天引HOOK
					{
						pGame->pCGameHook->GetDrawInfo();
						pGame->pCGameHook->DrawHook();
					}


					SetTimer(1, 200, NULL);				//刷新界面，比如血量，后坐力，坐标什么的
					SetTimer(3, 100, NULL);				//循环找所有人的基址，如果找到了，杀死计时器
				}
			}
			else
			{
				if (flag > 0)							//如果已经设置过计时器
				{
					KillTimer(1);
					KillTimer(3);
					pGame->EmeptyPeopleAddr();
					pGame->IsUseDiBaoTianXing = FALSE;
					GetDlgItem(IDC_STATIC_DIBAOTIANXING1)->ShowWindow(SW_HIDE);
					GetDlgItem(IDC_STATIC_DIBAOTIANXING2)->ShowWindow(SW_HIDE);
					flag = 0;
				}

				ShowWindow(SW_HIDE);
			}
		}
		catch(...)
		{

		}
		break;
	case 1:
		if (!pGame->IsInGame())
			break;
		try
		{
			//刷新血量
			m_dwBlood = (pGame->GetMyBlood());						//获取当前血量
			sprintf_s(buffer, "%.0f", m_dwBlood);
			GetDlgItem(IDC_EDIT_BLOOD)->SetWindowText(buffer);		//将当前的血量打印到窗口中

			//刷新护甲
			m_hujia = (pGame->GetMyHujia());						//获取当前血量
			sprintf_s(buffer, "%.0f", m_hujia);
			GetDlgItem(IDC_EDIT_HUJIA)->SetWindowText(buffer);		//将当前的护甲打印到窗口中

			//刷新金钱
			DWORD myMoney = (pGame->GetMyMoney());						//获取当前血量
			sprintf_s(buffer, "%d", myMoney);
			GetDlgItem(IDC_EDIT_MONEY)->SetWindowText(buffer);		//将当前的护甲打印到窗口中

			//刷新地址
			pGame->GetLocalPos(&pGame->NowPos);

			m_posX = pGame->NowPos.x;
			m_posY = pGame->NowPos.y;
			m_posZ = pGame->NowPos.z;
			RefreshPos(&pGame->NowPos, IDC_EDIT_POSX, IDC_EDIT_POSY, IDC_EDIT_POSZ);

			//刷新子弹数														
			m_numOfbullet = (pGame->GetNumOfBullet());				//获取子弹数
			sprintf_s(buffer, "%d", m_numOfbullet);
			GetDlgItem(IDC_EDIT_BULLETNUM)->SetWindowText(buffer);	//将当前的子弹数打印到窗口中	
		}
		catch (...)
		{
			OutputDebugString("刷新血量或护甲或地址或子弹数异常");
		}
	

		try
		{
			//根据判断实现锁血
			if (IsSuoXue)
			{
				bool flag1 = false;
				if (pGame->fNowBlood_suoxue > -1 && pGame->fNowBlood_suoxue < 1)		//如果是0的话,说明是第一次
					flag1 = true;
				pGame->SuoXue(m_dwBlood, flag1);
			}
			else
			{
				pGame->fNowBlood_suoxue = 0;
			}
		
		}
		catch (...)
		{
			OutputDebugString("锁血异常");
		}
		try
		{
			//实现无敌
			OnBnClickedCheckWudi();
			//护甲
			OnBnClickedCheckHujia();
			//子弹
			OnBnClickedCheckMoreBullet();
			//后坐力
			OnBnClickedCheckWuhouzuo();
			//随地购物
			OnBnClickedCheckBuyanywhere();
			//无限金币
			pGame->SuoDingMoney();
		}
		catch (...)
		{
			OutputDebugString("无敌 护甲 子弹 后坐力 随机购物异常");
		}

		break;
	case 3:
		try {
			if (pGame->GetAllPeople7c())
			{
				OutputDebugString("基址获取完毕!");
				GetDlgItem(IDC_STATIC_DIBAOTIANXING1)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_STATIC_DIBAOTIANXING2)->ShowWindow(SW_SHOW);

				pGame->FilteEnemy();								//获取到了所有敌人的详细信息
				pGame->IsUseDiBaoTianXing = TRUE;
				KillTimer(3);
			}
		}
		catch (...)
		{

		}
		break;
	case 5:
		try
		{
			if (pGame->IsAllEnemyKilled())						//如果说所有的敌人都杀死了
			{
				OutputDebugString("所有敌人都被杀死了!");
				pGame->pCGameHook->BombHook(FALSE);
				pGame->stRandomPos.clear();
				pGame->IsFirstDiBaoTianXing = TRUE;

				GetDlgItem(IDC_STATIC_DIBAOTIANXING1)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_STATIC_DIBAOTIANXING2)->ShowWindow(SW_SHOW);
				KillTimer(5);						
			}
			pGame->DiBaoTianXingCopyPos();
		}
		catch (...)
		{
			KillTimer(5);
		}

		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CMyWin::RefreshPos(pstPlayerPos pPos, DWORD IDCX, DWORD IDCY, DWORD IDCZ)
{
	char buffer[50];
	sprintf_s(buffer, "%.1f", pPos->x);
	GetDlgItem(IDCX)->SetWindowText(buffer);		//将当前的x打印到窗口中
	sprintf_s(buffer, "%.1f", pPos->y);
	GetDlgItem(IDCY)->SetWindowText(buffer);		//将当前的y打印到窗口中
	sprintf_s(buffer, "%.1f", pPos->z);
	GetDlgItem(IDCZ)->SetWindowText(buffer);		//将当前的z打印到窗口中
}

void CMyWin::RefreshMemoryPos(pstPlayerPos pPos)
{
	//如果说没有标记
	if (!(pPos->x || pPos->y || pPos->z))
	{
		return;
	}

	// TODO: 在此添加控件通知处理程序代码
	if (pGame->IsInGame())
	{
		float * x = (float *)((DWORD)pGame->pPlayer + 0X8);
		float * y = (float *)((DWORD)pGame->pPlayer + 0XC);
		float * z = (float *)((DWORD)pGame->pPlayer + 0X10);
		*x = pPos->x;
		*y = pPos->y;
		*z = pPos->z;
	}
}

//修改血量
void CMyWin::OnBnClickedButtonBlood()
{
	//先获取输出框中的字符串
	CString csText;
	GetDlgItem(IDC_EDIT_CHANGEBLOOD)->GetWindowText(csText);
	m_changeBlood = (float)atoi(csText);
	
	// TODO: 在此添加控件通知处理程序代码
	if (pGame->IsInGame())
	{
		float * a =  (float *)((DWORD)pGame->pPlayer + 0X160);
		*a = m_changeBlood;
	}
}

//标记位置1
void CMyWin::OnBnClickedButtonMarkpos3()
{
	pGame->MarkPos1.x = pGame->NowPos.x;
	pGame->MarkPos1.y = pGame->NowPos.y;
	pGame->MarkPos1.z = pGame->NowPos.z;
	// TODO: 在此添加控件通知处理程序代码
	RefreshPos(&pGame->MarkPos1, IDC_EDIT_POSX_1, IDC_EDIT_POSY_1, IDC_EDIT_POSZ_1);
}

//标记位置2
void CMyWin::OnBnClickedButtonMarkpos2()
{
	pGame->MarkPos2.x = pGame->NowPos.x;
	pGame->MarkPos2.y = pGame->NowPos.y;
	pGame->MarkPos2.z = pGame->NowPos.z;
	// TODO: 在此添加控件通知处理程序代码
	RefreshPos(&pGame->MarkPos2, IDC_EDIT_POSX_2, IDC_EDIT_POSY_2, IDC_EDIT_POSZ_2);
}

//瞬移位置1
void CMyWin::OnBnClickedButtonMovePos1()
{
	RefreshMemoryPos(&pGame->MarkPos1);
}

//瞬移位置2
void CMyWin::OnBnClickedButtonMovePos2()
{
	RefreshMemoryPos(&pGame->MarkPos2);
}

//锁血
void CMyWin::OnBnClickedCheckSuoxue()
{
	//检查锁血checkBox的
	int state = ((CButton *)GetDlgItem(IDC_CHECK_SUOXUE))->GetCheck();
	IsSuoXue = state;
	if (state)
		OutputDebugString("CS->实现锁血\n");
	else
		OutputDebugString("CS->取消锁血\n");
}

//改变视野线程
DWORD WINAPI ChangeViewThread(LPVOID lParam)
{
	CMyWin * _this = (CMyWin *)lParam;
	_this->OnBnClickedCheckChangeview();
	return 0;
}

//判断地址是不是人物基址
BOOL IsPeopleAddr(DWORD a)
{
	return !(a & 0XF0000000);
}

//万象天引技能线程
DWORD WINAPI WanXiangTianYinThread(LPVOID lParam)
{
	CMyWin * _this = (CMyWin *)lParam;
	int who = _this->pGame->GetPointToWho();		//此时指向的谁  
	if (who == 0)									//如果指向的是墙
		return 0;
	
	bool IsPeople = IsPeopleAddr(_this->pGame->pCGameHook->dw_Draw_PeopleAddr7c);
	if (!IsPeople)									//如果不是人就退出
		return 0;

	pstPlayerPos pMyPos = (pstPlayerPos)(*(DWORD *)((DWORD)_this->pGame->pPlayer7c + 4) + 8);
	pstPlayerPos pHisPos = (pstPlayerPos)(*(DWORD *)((DWORD)_this->pGame->pCGameHook->dw_Draw_PeopleAddr7c + 4) + 8);
	float * pdwHp = (float *)(*(DWORD *)((DWORD)_this->pGame->pCGameHook->dw_Draw_PeopleAddr7c + 4) + 0X160);
	float * pdwMyHp = (float *)(*(DWORD *)((DWORD)_this->pGame->pPlayer7c + 4) + 0X160);

	switch (who)
	{
	case 1:											//指向的是队友
		_this->pGame->CopyPos(pMyPos, pHisPos);		//p1  <-  p2
		*pdwHp = 100;			//给队友加血
		*pdwMyHp = 100;				
		break;
	case 2:											//指向的是敌人
		_this->pGame->CopyPos(pHisPos, pMyPos);
		*pdwHp = 1;
		break;	
	}
	return 1;
}

//地爆天星技能线程
DWORD WINAPI DiBaoTianXingThread(LPVOID lParam)
{
	CMyWin * _this = (CMyWin *)lParam;

	if (!_this->pGame->DiBaoTianXing666())
	{
		return 0;
	}
	_this->GetDlgItem(IDC_STATIC_DIBAOTIANXING1)->ShowWindow(SW_HIDE);
	_this->GetDlgItem(IDC_STATIC_DIBAOTIANXING2)->ShowWindow(SW_HIDE);
	SetTimer(_this->m_hWnd, 5, 100, NULL);



	return 0;
}
BOOL CMyWin::PreTranslateMessage(MSG* pMsg)
{
	if (GetAsyncKeyState(VK_F9))		//标记位置1
	{
		OnBnClickedButtonMarkpos3();
	}
	else if (GetAsyncKeyState(VK_F10))
	{
		OnBnClickedButtonMovePos1();
	}
	else if (GetAsyncKeyState(VK_F11))
	{
		OnBnClickedButtonMarkpos2();
	}
	else if (GetAsyncKeyState(VK_F12))
	{
		OnBnClickedButtonMovePos2();
	}
	else
	{

	}

	if (GetAsyncKeyState(VK_MENU))
	{
		CreateThread(NULL, 0, ChangeViewThread, this, 0, NULL);
	}
	if (GetAsyncKeyState('E'))
	{
		CreateThread(NULL, 0, WanXiangTianYinThread, this, 0, NULL);
	}
	if (GetAsyncKeyState('T'))
	{
		CreateThread(NULL, 0, DiBaoTianXingThread, this, 0, NULL);
	}

	return 	TranslateMessage(pMsg);
}

//无敌
void CMyWin::OnBnClickedCheckWudi()
{
	int state = ((CButton *)GetDlgItem(IDC_CHECK_WUDI))->GetCheck();
	if (state)
	{
		pGame->SuperMan();
		return;
	}
}

//索满护甲
void CMyWin::OnBnClickedCheckHujia()
{
	int state = ((CButton *)GetDlgItem(IDC_CHECK_HUJIA))->GetCheck();
	if (state)
	{
		pGame->SuManHujia();
		return;
	}
}


void CMyWin::OnBnClickedCheckMoreBullet()
{
	int state = ((CButton *)GetDlgItem(IDC_CHECK_MORE_BULLET))->GetCheck();
	if (state)
	{
		pGame->WuXinBulletNum();
		return;
	}
}


void CMyWin::OnBnClickedCheckWuhouzuo()
{
	int state = ((CButton *)GetDlgItem(IDC_CHECK_WUHOUZUO))->GetCheck();
	if (state)
	{
		pGame->NoHouzuoli();
		return;
	}
}


void CMyWin::OnBnClickedCheckSeckill()
{
	int state = ((CButton *)GetDlgItem(IDC_CHECK_SECKILL))->GetCheck();
	if (state)
	{
		OutputDebugString("开启秒杀\n");
		pGame->HookReduceBlood(true);
		return;
	}
	else
	{
		OutputDebugString("关闭秒杀\n");
		pGame->HookReduceBlood(false);
	}
}


void CMyWin::OnBnClickedInfGrenade()
{
	int state = ((CButton *)GetDlgItem(IDC_INF_GRENADE))->GetCheck();
	if (state)
	{
		OutputDebugString("开启无限手雷");
		pGame->HookInfGrenade(true);
		return;
	}
	else
	{
		OutputDebugString("关闭无限手雷");
		pGame->HookInfGrenade(false);
	}
}

void CMyWin::OnBnClickedCheckChangeview()
{
	int state = ((CButton *)GetDlgItem(IDC_CHECK_CHANGEVIEW))->GetCheck();
	
	if (state)
		state = 0;
	else
		state = 1;
	((CButton *)GetDlgItem(IDC_CHECK_CHANGEVIEW))->SetCheck(state);
		
	if (state)
	{
		pGame->Changingview(false);
	}
	else
	{
		pGame->Changingview(true);
	}
}


void CMyWin::OnBnClickedCheckBuyanywhere()
{
	int state = ((CButton *)GetDlgItem(IDC_CHECK_BUYANYWHERE))->GetCheck();
	if (state)
	{
		pGame->BuyAnywhere();
		return;
	}
}


void CMyWin::OnBnClickedCheckSuodingmoney()
{
	int state = ((CButton *)GetDlgItem(IDC_CHECK_SUODINGMONEY))->GetCheck();
	if (state)
	{
		pGame->SuoDingMoney();
		return;
	}
}
