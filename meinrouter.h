
// meinrouter.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CmeinrouterApp:
// �йش����ʵ�֣������ meinrouter.cpp
//

class CmeinrouterApp : public CWinApp
{
public:
	CmeinrouterApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CmeinrouterApp theApp;
