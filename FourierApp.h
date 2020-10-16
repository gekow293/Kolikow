
// FourierApp.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// FourierApp:
// Сведения о реализации этого класса: FourierApp.cpp
//

class FourierApp : public CWinApp
{
public:
	FourierApp();

// Переопределение
public:
	virtual BOOL InitInstance();
	//afx_msg void AppMessageCommand();
	//afx_msg void ExitApp();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern FourierApp theApp;
