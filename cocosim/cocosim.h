
// cocosim.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CcocosimApp: 
// �йش����ʵ�֣������ cocosim.cpp
//

class CcocosimApp : public CWinApp
{
public:
	CcocosimApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CcocosimApp theApp;
