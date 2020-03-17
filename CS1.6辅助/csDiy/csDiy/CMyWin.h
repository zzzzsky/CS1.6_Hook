#pragma once

#include "CGame.h"
#include <stdlib.h>

// CMyWin 对话框
#define  AFX_DESIGN_TIME
class CMyWin : public CDialogEx
{
	DECLARE_DYNAMIC(CMyWin)
	
public:
	CGame * pGame;
	CMyWin(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMyWin();
	int IsSuoXue = 0;
	bool IsWudi = false;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
	// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void RefreshPos(pstPlayerPos pPos, DWORD IDCX, DWORD IDCY, DWORD IDCZ);		//将坐标填到对应的框中
	void RefreshMemoryPos(pstPlayerPos pPos);								//将坐标填入到真正的内存当中
	
																			// 当前人物血量
	float m_dwBlood;
	afx_msg void OnBnClickedButtonBlood();
	// 改变血量
	float m_changeBlood;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	float m_posX;
	float m_posY;
	float m_posZ;
	afx_msg void OnBnClickedButtonMarkpos3();
	afx_msg void OnBnClickedButtonMarkpos2();
	afx_msg void OnBnClickedButtonMovePos1();
	afx_msg void OnBnClickedButtonMovePos2();
	afx_msg void OnBnClickedCheckSuoxue();
	afx_msg void OnBnClickedCheckWudi();
	float m_hujia;
	afx_msg void OnBnClickedCheckHujia();
	int m_numOfbullet;
	afx_msg void OnBnClickedCheckMoreBullet();
	afx_msg void OnBnClickedCheckWuhouzuo();
	afx_msg void OnBnClickedCheckSeckill();
	afx_msg void OnBnClickedInfGrenade();
	afx_msg void OnBnClickedCheckChangeview();
	afx_msg void OnBnClickedCheckBuyanywhere();
	afx_msg void OnBnClickedCheckSuodingmoney();
};
