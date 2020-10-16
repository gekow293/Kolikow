// Это часть библиотеки Microsoft Foundation Classes C ++.
// Copyright (C) 1992-1998 Microsoft Corporation
// Все права защищены.
//
// Этот исходный код предназначен только в качестве дополнения к
// Справочник классов Microsoft Foundation и связанные с ним
// электронная документация, предоставляемая библиотекой.
// Смотрите эти источники для получения подробной информации о
// Microsoft Foundation Classes product.

#include "stdafx.h"
#ifndef _AFX_NO_OCC_SUPPORT
#include "occimpl.h"
#endif

#pragma warning (отключить: 4706)
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>
#pragma warning (по умолчанию: 4706)

#ifdef AFX_CORE1_SEG
#pragma code_seg (AFX_CORE1_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
статический символ THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

////////////////////////////////////////////////// ///////////////////////////
// Глобалы

const UINT CWnd::m_nMsgDragList = ::RegisterWindowMessage(DRAGLISTMSGSTRING);

// CWnds для установки z-порядка с параметром SetWindowPos pWndInsertAfter
const AFX_DATADEF CWnd CWnd::wndTop(HWND_TOP);
const AFX_DATADEF CWnd CWnd::wndBottom(HWND_BOTTOM);
const AFX_DATADEF CWnd CWnd::wndTopMost(HWND_TOPMOST);
const AFX_DATADEF CWnd CWnd::wndNoTopMost(HWND_NOTOPMOST);

const TCHAR _afxWnd[] = AFX_WND;
const TCHAR _afxWndControlBar[] = AFX_WNDCONTROLBAR;
const TCHAR _afxWndMDIFrame[] = AFX_WNDMDIFRAME;
const TCHAR _afxWndFrameOrView[] = AFX_WNDFRAMEORVIEW;
const TCHAR _afxWndOleControl[] = AFX_WNDOLECONTROL;

////////////////////////////////////////////////// ///////////////////////////
// CWnd строительство

CW::CW()
{
	AFX_ZERO_INIT_OBJECT(CCmdTarget);
}

CWnd::CWnd(HWND hWnd)
{
	AFX_ZERO_INIT_OBJECT(CCmdTarget);
	m_hWnd = hWnd;
}

// Изменить стиль окна

AFX_STATIC BOOL AFXAPI _AfxModifyStyle(HWND hWnd, int nStyleOffset,
	DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	ASSERT(hWnd!= NULL);
	DWORD dwStyle = ::GetWindowLong(hWnd, nStyleOffset);
	DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
	if (dwStyle == dwNewStyle)
		вернуть ЛОЖЬ;

	::SetWindowLong(hWnd, nStyleOffset, dwNewStyle);
	if (nFlags!= 0)
	{
		::SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
	}
	вернуть ИСТИНА;
}

BOOL PASCAL
CWnd::ModifyStyle(HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	return _AfxModifyStyle(hWnd, GWL_STYLE, dwRemove, dwAdd, nFlags);
}

BOOL PASCAL
CWnd::ModifyStyleEx(HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	return _AfxModifyStyle(hWnd, GWL_EXSTYLE, dwRemove, dwAdd, nFlags);
}

////////////////////////////////////////////////// ///////////////////////////
// Специальные помощники для определенных сообщений Windows

AFX_STATIC void AFXAPI _AfxPreInitDialog(
	CWnd* pWnd, LPRECT lpRectOld, DWORD* pdwStyleOld)
{
	ASSERT(lpRectOld!= NULL);
	ASSERT(pdwStyleOld!= NULL);

	pWnd->GetWindowRect(lpRectOld);
	*pdwStyleOld = pWnd->GetStyle();
}

AFX_STATIC void AFXAPI _AfxPostInitDialog(
	CWnd* pWnd, const RECT& rectOld, DWORD dwStyleOld)
{
	// должно быть скрыто для начала
	if (dwStyleOld & WS_VISIBLE)
		возвращение;

	// не должен быть виден после WM_INITDIALOG
	if (pWnd->GetStyle() & (WS_VISIBLE | WS_CHILD))
		возвращение;

	// не должен двигаться во время WM_INITDIALOG
	CRect rect;
	pWnd->GetWindowRect(прямоугольник);
	if (rectOld.left!= rect.left || rectOld.top!= rect.top)
		возвращение;

	// должен быть неизвестным или владелец отключен
	CWnd* pParent = pWnd->GetWindow(GW_OWNER);
	if (pParent!= NULL && pParent->IsWindowEnabled())
		возвращение;

	if (!pWnd->CheckAutoCenter())
		возвращение;

	// центрируем модальные диалоговые окна / окна сообщений
	pWnd->CenterWindow();
}

AFX_STATIC void AFXAPI
_AfxHandleActivate(CWnd* pWnd, WPARAM nState, CWnd* pWndOther)
{
	ASSERT(pWnd!= NULL);

	// отправляем WM_ACTIVATETOPLEVEL при смене родителей верхнего уровня
	CWnd* pTopLevel;
	if (!(pWnd->GetStyle() & WS_CHILD) &&
		(pTopLevel = pWnd->GetTopLevelParent())!= pWndOther->GetTopLevelParent())
	{
		// lParam указывает на окно получения сообщения WM_ACTIVATE и
		// hWndOther от WM_ACTIVATE.
		HWND hWnd2[2];
		hWnd2[0] = pWnd->m_hWnd;
		hWnd2[1] = pWndOther->GetSafeHwnd();
		// Отправь это...
		pTopLevel->SendMessage(WM_ACTIVATETOPLEVEL, nState, (LPARAM)&hWnd2[0]);
	}
}

AFX_STATIC BOOL AFXAPI
_AfxHandleSetCursor(CWnd* pWnd, UINT nHitTest, UINT nMsg)
{
	if (nHitTest == HTERROR &&
		(nMsg == WM_LBUTTONDOWN || nMsg == WM_MBUTTONDOWN ||
			nMsg == WM_RBUTTONDOWN))
	{
		// активировать последнее активное окно, если оно не активно
		CWnd* pLastActive = pWnd->GetTopLevelParent();
		if (pLastActive!= NULL)
			pLastActive = pLastActive->GetLastActivePopup();
		if (pLastActive!= NULL &&
			pLastActive!= CWnd::GetForegroundWindow() &&
			pLastActive->IsWindowEnabled())
		{
			pLastActive->SetForegroundWindow();
			вернуть ИСТИНА;
		}
	}
	вернуть ЛОЖЬ;
}

////////////////////////////////////////////////// ///////////////////////////
// Официальный способ отправки сообщения на CWnd

LRESULT AFXAPI AfxCallWndProc(CWnd* pWnd, HWND hWnd, UINT nMsg,
	WPARAM wParam = 0, LPARAM lParam = 0)
{
	_AFX_THREAD_STATE* pThreadState = _afxThreadState.GetData();
	MSG oldState = pThreadState->m_lastSentMsg; // сохранить для вложения
	pThreadState->m_lastSentMsg.hwnd = hWnd;
	pThreadState->m_lastSentMsg.message = nMsg;
	pThreadState->m_lastSentMsg.wParam = wParam;
	pThreadState->m_lastSentMsg.lParam = lParam;

#ifdef _DEBUG
	if (afxTraceFlags & traceWinMsg)
		_AfxTraceMsg(_T("WndProc"), &pThreadState->m_lastSentMsg);
#endif

	// Перехват исключений, выходящих за рамки обратного вызова
	// в отладочных сборках и предупреждаем пользователя.
	LRESULT lResult;
	ПЫТАТЬСЯ
	{
#ifndef _AFX_NO_OCC_SUPPORT
		// особый случай для WM_DESTROY
		if ((nMsg == WM_DESTROY) && (pWnd->m_pCtrlCont!= NULL))
			pWnd->m_pCtrlCont->OnUIActivate(NULL);
#endif

	// особый случай для WM_INITDIALOG
	CRect rectOld;
	DWORD dwStyle = 0;
	если(nMsg == WM_INITDIALOG)
		_AfxPreInitDialog(pWnd, &rectOld, &dwStyle);

	// делегировать объекту WindowProc
	lResult = pWnd->WindowProc(nMsg, wParam, lParam);

	// более особый случай для WM_INITDIALOG
	если(nMsg == WM_INITDIALOG)
		_AfxPostInitDialog(pWnd, rectOld, dwStyle);
	}
		CATCH_ALL(е)
	{
		CWinThread* pWinThread = AfxGetThread();
		если(pWinThread!= NULL)
		{
			lResult = pWinThread->ProcessWndProcException(e, &pThreadState->m_lastSentMsg);
			TRACE1(«Предупреждение: необработанное исключение в WindowProc(возвращает % ld). \ N»,
				LRESULT);
		}
		еще
		{
			TRACE0(«Предупреждение: необработанное исключение в WindowProc. \ N»);
			lResult = 0;
		}
		DELETE_EXCEPTION(е);
	}
	END_CATCH_ALL

		pThreadState->m_lastSentMsg = oldState;
	вернуть lResult;
}

const MSG* PASCAL CWnd::GetCurrentMessage()
{
	// заполняем время и позицию при запросе
	_AFX_THREAD_STATE* pThreadState = _afxThreadState.GetData();
	pThreadState->m_lastSentMsg.time = ::GetMessageTime();
	pThreadState->m_lastSentMsg.pt = CPoint(::GetMessagePos());
	return &pThreadState->m_lastSentMsg;
}

LRESULT CWnd::Default()
{
	// вызываем DefWindowProc с последним сообщением
	_AFX_THREAD_STATE* pThreadState = _afxThreadState.GetData();
	вернуть DefWindowProc(pThreadState->m_lastSentMsg.message,
		pThreadState->m_lastSentMsg.wParam, pThreadState->m_lastSentMsg.lParam);
}

////////////////////////////////////////////////// ///////////////////////////
// Карта из HWND в CWnd *

#include "fixalloc.h"

Класс CTempWnd : общедоступный CWnd
{
	DECLARE_DYNCREATE(CTempWnd)
	DECLARE_FIXED_ALLOC(CTempWnd);
};

CHandleMap* PASCAL afxMapHWND(BOOL bCreate)
{
	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();
	if (pState->m_pmapHWND == NULL && bCreate)
	{
		BOOL bEnable = AfxEnableMemoryTracking(FALSE);
#ifndef _AFX_PORTABLE
		_PNH pnhOldHandler = AfxSetNewHandler(&AfxCriticalNewHandler);
#endif
		pState->m_pmapHWND = new CHandleMap(RUNTIME_CLASS(CTempWnd),
			offsetof(CWnd, m_hWnd));

#ifndef _AFX_PORTABLE
		AfxSetNewHandler(pnhOldHandler);
#endif
		AfxEnableMemoryTracking(bEnable);
	}
	return pState->m_pmapHWND;
}

CWnd* PASCAL CWnd::FromHandle(HWND hWnd)
{
	CHandleMap* pMap = afxMapHWND(TRUE); // создать карту, если не существует
	ASSERT(pMap!= NULL);
	CWnd* pWnd = (CWnd*)pMap->FromHandle(hWnd);

#ifndef _AFX_NO_OCC_SUPPORT
	pWnd->AttachControlSite(ртар);
#endif

	ASSERT(pWnd == NULL || pWnd->m_hWnd == hWnd);
	вернуть pWnd;
}

CWnd* PASCAL CWnd::FromHandlePermanent(HWND hWnd)
{
	CHandleMap* pMap = afxMapHWND();
	CWnd* pWnd = NULL;
	if (pMap!= NULL)
	{
		// только смотреть на постоянной карте - не выделяет
		pWnd = (CWnd*)pMap->LookupPermanent(hWnd);
		ASSERT(pWnd == NULL || pWnd->m_hWnd == hWnd);
	}
	вернуть pWnd;
}

BOOL CWnd::Attach(HWND hWndNew)
{
	ASSERT(m_hWnd == NULL); // только один раз прикрепить, отсоединить при уничтожении
	ASSERT(FromHandlePermanent(hWndNew) == NULL);
	// уже не должно быть в постоянной карте

	if (hWndNew == NULL)
		вернуть ЛОЖЬ;

	CHandleMap* pMap = afxMapHWND(TRUE); // создать карту, если не существует
	ASSERT(pMap!= NULL);

	pMap->SetPermanent(m_hWnd = hWndNew, this);

#ifndef _AFX_NO_OCC_SUPPORT
	AttachControlSite(ртар);
#endif

	вернуть ИСТИНА;
}

HWND CWnd::Detach()
{
	HWND hWnd = m_hWnd;
	if (hWnd!= NULL)
	{
		CHandleMap* pMap = afxMapHWND(); // не создавать, если не существует
		if (pMap!= NULL)
			pMap->RemoveHandle(m_hWnd);
		m_hWnd = NULL;
	}

#ifndef _AFX_NO_OCC_SUPPORT
	m_pCtrlSite = NULL;
#endif

	возврат hWnd;
}

void CWnd::PreSubclassWindow()
{
	// нет обработки по умолчанию
}

////////////////////////////////////////////////// ///////////////////////////
// WndProc для всех CWnd и производных классов

LRESULT CALLBACK
AfxWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	// специальное сообщение, которое идентифицирует окно как использующее AfxWndProc
	если(nMsg == WM_QUERYAFXWNDPROC)
		возврат 1;

	// все остальные сообщения проходят через карту сообщений
	CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);
	ASSERT(pWnd!= NULL);
	ASSERT(pWnd->m_hWnd == hWnd);
	если(pWnd == NULL || pWnd->m_hWnd!= hWnd)
		return ::DefWindowProc(hWnd, nMsg, wParam, lParam);
	return AfxCallWndProc(pWnd, hWnd, nMsg, wParam, lParam);
}

// всегда косвенный доступ через AfxGetAfxWndProc
WNDPROC AFXAPI AfxGetAfxWndProc()
{
#ifdef _AFXDLL
	return AfxGetModuleState()->m_pfnAfxWndProc;
#else
	return &AfxWndProc;
#endif
}

////////////////////////////////////////////////// ///////////////////////////
// Специальные WndProcs (обработка активации и серые диалоги)

AFX_STATIC_DATA const TCHAR _afxOldWndProc[] = _T("AfxOldWndProc423");

LRESULT CALLBACK
_AfxActivationWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC oldWndProc = (WNDPROC) ::GetProp(hWnd, _afxOldWndProc);
	ASSERT(oldWndProc!= NULL);

	LRESULT lResult = 0;
	ПЫТАТЬСЯ
	{
		BOOL bCallDefault = TRUE;
		переключатель(нМсг)
		{
		case WM_INITDIALOG:
			{
				DWORD dwStyle;
				CRect rectOld;
				CWnd* pWnd = CWnd::FromHandle(hWnd);
				_AfxPreInitDialog(pWnd, &rectOld, &dwStyle);
				bCallDefault = FALSE;
				lResult = CallWindowProc(oldWndProc, hWnd, nMsg, wParam, lParam);
				_AfxPostInitDialog(pWnd, rectOld, dwStyle);
			}
			перемена;

		дело WM_ACTIVATE :
			_AfxHandleActivate(CWnd::FromHandle(hWnd), wParam,
				CWnd::FromHandle((HWND)LPARAM));
			перемена;

		case WM_SETCURSOR:
			bCallDefault = !_AfxHandleSetCursor(CWnd::FromHandle(hWnd),
				(кратко)LOWORD(lParam), HIWORD(lParam));
			перемена;

		case WM_NCDESTROY:
			SetWindowLong(hWnd, GWL_WNDPROC, (DWORD)oldWndProc);
			RemoveProp(hWnd, _afxOldWndProc);
			GlobalDeleteAtom(GlobalFindAtom(_afxOldWndProc));
			перемена;
		}

		// вызываем оригинальный wndproc для обработки по умолчанию
		если(bCallDefault)
			lResult = CallWindowProc(oldWndProc, hWnd, nMsg, wParam, lParam);
	}
		CATCH_ALL(е)
	{
		// обработать исключение
		MSG MSG;
		msg.hwnd = hWnd;
		msg.message = nMsg;
		msg.wParam = wParam;
		msg.lParam = lParam;
		lResult = AfxGetThread()->ProcessWndProcException(e, &msg);
		TRACE1(«Предупреждение: необработанное исключение в _AfxActivationWndProc(возвращает % ld). \ N»,
			LRESULT);
		DELETE_EXCEPTION(е);
	}
	END_CATCH_ALL

		вернуть lResult;
}

#ifndef _AFX_NO_GRAYDLG_SUPPORT
LRESULT CALLBACK
_AfxGrayBackgroundWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	// обрабатывать стандартные серые фоны, если они включены
	_AFX_WIN_STATE* pWinState = _afxWinState;
	if (pWinState->m_hDlgBkBrush!= NULL &&
		(nMsg == WM_CTLCOLORBTN || nMsg == WM_CTLCOLORDLG ||
			nMsg == WM_CTLCOLORSTATIC || nMsg == WM_CTLCOLORSCROLLBAR ||
			nMsg == WM_CTLCOLORLISTBOX) &&
		CWnd::GrayCtlColor((HDC)wParam, (HWND)lParam,
		(UINT)(nMsg - WM_CTLCOLORMSGBOX),
			pWinState->m_hDlgBkBrush, pWinState->m_crDlgTextClr))
	{
		return (LRESULT)pWinState->m_hDlgBkBrush;
	}

	// также выполняем стандартные действия, связанные с активацией
	return _AfxActivationWndProc(hWnd, nMsg, wParam, lParam);
}
#else //! _ AFX_NO_GRAYDLG_SUPPORT
#define _AfxGrayBackgroundWndProc _AfxActivationWndProc
#endif // _ AFX_NO_GRAYDLG_SUPPORT

////////////////////////////////////////////////// ///////////////////////////
// Окно создания хуков

LRESULT CALLBACK
_AfxCbtFilterHook(int код, WPARAM wParam, LPARAM lParam)
{
	_AFX_THREAD_STATE* pThreadState = _afxThreadState.GetData();
	если(код!= HCBT_CREATEWND)
	{
		// ждем HCBT_CREATEWND, просто передадим другим ...
		вернуть CallNextHookEx(pThreadState->m_hHookOldCbtFilter, код,
			wParam, lParam);
	}

	ASSERT(lParam!= NULL);
	LPCREATESTRUCT lpcs = ((LPCBT_CREATEWND)lParam)->lpcs;
	ASSERT(lpcs!= NULL);

	CWnd* pWndInit = pThreadState->m_pWndInit;
	BOOL bContextIsDLL = afxContextIsDLL;
	if (pWndInit!= NULL || (!(lpcs->style & WS_CHILD) && !bContextIsDLL))
	{
		// Примечание: специальная проверка, чтобы избежать подкласса окна IME
		если(_afxDBCS)
		{
			// сначала проверяем дешевый стиль CS_IME ...
			if (GetClassLong((HWND)wParam, GCL_STYLE) & CS_IME)
				goto lCallNextHook;

			// получаем имя класса создаваемого окна
			LPCTSTR pszClassName;
			TCHAR szClassName[_countof("ime") + 1];
			if (HIWORD(lpcs->lpszClass))
			{
				pszClassName = lpcs->lpszClass;
			}
			еще
			{
				szClassName[0] = '\ 0';
				GlobalGetAtomName((ATOM)lpcs->lpszClass, szClassName, _countof(szClassName));
				pszClassName = szClassName;
			}

				// немного дороже, чтобы проверить таким образом, но необходимо ...
				if (lstrcmpi(pszClassName, _T("ime")) == 0)
					goto lCallNextHook;
		}

		ASSERT(wParam!= NULL); // должен быть ненулевым HWND
		HWND hWnd = (HWND)wParam;
		WNDPROC oldWndProc;
		if (pWndInit!= NULL)
		{
#ifdef _AFXDLL
			AFX_MANAGE_STATE(pWndInit->m_pModuleState);
#endif

			// окно не должно быть в постоянной карте в это время
			ASSERT(CWnd::FromHandlePermanent(hWnd) == NULL);

			// подключаем HWND к pWndInit ...
			pWndInit->Attach(HWND);
			// разрешаем другим подклассам сначала
			pWndInit->PreSubclassWindow();

			WNDPROC* pOldWndProc = pWndInit->GetSuperWndProcAddr();
			ASSERT(pOldWndProc!= NULL);

#ifndef _AFX_NO_CTL3D_SUPPORT
			_AFX_CTL3D_STATE* pCtl3dState;
			DWORD dwFlags;
			if (!afxData.bWin4 && !bContextIsDLL &&
				(pCtl3dState = _afxCtl3dState.GetDataNA())!= NULL &&
				pCtl3dState->m_pfnSubclassDlgEx!= NULL &&
				(dwFlags = AfxCallWndProc(pWndInit, hWnd, WM_QUERY3DCONTROLS))!= 0)
			{
				// был ли класс зарегистрирован в AfxWndProc?
				WNDPROC afxWndProc = AfxGetAfxWndProc();
				BOOL bAfxWndProc = ((WNDPROC)
					GetWindowLong(hWnd, GWL_WNDPROC) == afxWndProc);

				pCtl3dState->m_pfnSubclassDlgEx(hWnd, dwFlags);

				// подкласс окна, если он еще не подключен к AfxWndProc
				if (!bAfxWndProc)
				{
					// подкласс окна со стандартным AfxWndProc
					oldWndProc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC,
						(DWORD)afxWndProc);
					ASSERT(oldWndProc!= NULL);
					*pOldWndProc = oldWndProc;
				}
			}
			еще
#endif
			{
				// подкласс окна со стандартным AfxWndProc
				WNDPROC afxWndProc = AfxGetAfxWndProc();
				oldWndProc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC,
					(DWORD)afxWndProc);
				ASSERT(oldWndProc!= NULL);
				if (oldWndProc!= afxWndProc)
					*pOldWndProc = oldWndProc;
			}
			pThreadState->m_pWndInit = NULL;
		}
		еще
		{
			ASSERT(bContextIsDLL!); // никогда не должен попасть сюда

			// создаем подкласс для окна с процедурой, которая делает серый фон
			oldWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
			if (oldWndProc!= NULL && GetProp(hWnd, _afxOldWndProc) == NULL)
			{
				SetProp(hWnd, _afxOldWndProc, oldWndProc);
				if ((WNDPROC)GetProp(hWnd, _afxOldWndProc) == oldWndProc)
				{
					GlobalAddAtom(_afxOldWndProc);
					SetWindowLong(hWnd, GWL_WNDPROC,
						(DWORD)(pThreadState->m_bDlgCreate ?
							_AfxGrayBackgroundWndProc : _AfxActivationWndProc));
					ASSERT(oldWndProc!= NULL);
				}
			}
		}
	}

lCallNextHook:
	LRESULT lResult = CallNextHookEx(pThreadState->m_hHookOldCbtFilter, код,
		wParam, lParam);

#ifndef _AFXDLL
	if (bContextIsDLL)
	{
		::UnhookWindowsHookEx(pThreadState->m_hHookOldCbtFilter);
		pThreadState->m_hHookOldCbtFilter = NULL;
	}
#endif
	вернуть lResult;
}

void AFXAPI AfxHookWindowCreate(CWnd* pWnd)
{
	_AFX_THREAD_STATE* pThreadState = _afxThreadState.GetData();
	if (pThreadState->m_pWndInit == pWnd)
		возвращение;

	if (pThreadState->m_hHookOldCbtFilter == NULL)
	{
		pThreadState->m_hHookOldCbtFilter = ::SetWindowsHookEx(WH_CBT,
			_AfxCbtFilterHook, NULL, ::GetCurrentThreadId());
		if (pThreadState->m_hHookOldCbtFilter == NULL)
			AfxThrowMemoryException();
	}
	ASSERT(pThreadState->m_hHookOldCbtFilter!= NULL);
	ASSERT(pWnd!= NULL);
	ASSERT(pWnd->m_hWnd == NULL); // делать только один раз

	ASSERT(pThreadState->m_pWndInit == NULL); // ловушка еще не выполняется
	pThreadState->m_pWndInit = pWnd;
}

BOOL AFXAPI AfxUnhookWindowCreate()
{
	_AFX_THREAD_STATE* pThreadState = _afxThreadState.GetData();
#ifndef _AFXDLL
	if (afxContextIsDLL&& pThreadState->m_hHookOldCbtFilter!= NULL)
	{
		::UnhookWindowsHookEx(pThreadState->m_hHookOldCbtFilter);
		pThreadState->m_hHookOldCbtFilter = NULL;
	}
#endif
	if (pThreadState->m_pWndInit!= NULL)
	{
		pThreadState->m_pWndInit = NULL;
		вернуть ЛОЖЬ; // не был успешно подключен
	}
	вернуть ИСТИНА;
}

////////////////////////////////////////////////// ///////////////////////////
// CWnd создание

BOOL CWnd::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName,
	LPCTSTR lpszWindowName, DWORD dwStyle,
	const RECT& rect, CWnd* pParentWnd, UINT nID,
	LPVOID lpParam / *= NULL * / )
{
	вернуть CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		pParentWnd->GetSafeHwnd(), (HMENU)nID, lpParam);
}

BOOL CWnd::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName,
	LPCTSTR lpszWindowName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight,
	HWND hWndParent, HMENU nIDorHMenu, LPVOID lpParam)
{
	// разрешить изменение нескольких общих параметров создания
	CREATESTRUCT cs;
	cs.dwExStyle = dwExStyle;
	cs.lpszClass = lpszClassName;
	cs.lpszName = lpszWindowName;
	cs.style = dwStyle;
	cs.x = x;
	cs.y = y;
	cs.cx = nWidth;
	cs.cy = nHeight;
	cs.hwndParent = hWndParent;
	cs.hMenu = nIDorHMenu;
	cs.hInstance = AfxGetInstanceHandle();
	cs.lpCreateParams = lpParam;

	if (!PreCreateWindow(cs))
	{
		PostNcDestroy();
		вернуть ЛОЖЬ;
	}

	AfxHookWindowCreate(это);
	HWND hWnd = ::CreateWindowEx(cs.dwExStyle, cs.lpszClass,
		cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy,
		cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

#ifdef _DEBUG
	если(hWnd == NULL)
	{
		TRACE1(«Предупреждение: сбой при создании окна : GetLastError возвращает 0x % 8.8X \ n»,
			GetLastError());
	}
#endif

	if (!AfxUnhookWindowCreate())
		PostNcDestroy(); // очистка, если CreateWindowEx завершается неудачно слишком рано

	если(hWnd == NULL)
		вернуть ЛОЖЬ;
	ASSERT(hWnd == m_hWnd); // должен был быть установлен в пересылке msg
	вернуть ИСТИНА;
}

// для дочерних окон
BOOL CWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	if (cs.lpszClass == NULL)
	{
		// убедитесь, что класс окна по умолчанию зарегистрирован
		VERIFY(AfxDeferRegisterClass(AFX_WND_REG));

		// WNDCLASS не предоставлен - используйте дочернее окно по умолчанию
		ASSERT(cs.style & WS_CHILD);
		cs.lpszClass = _afxWnd;
	}
	вернуть ИСТИНА;
}

BOOL CWnd::Create(LPCTSTR lpszClassName,
	LPCTSTR lpszWindowName, DWORD dwStyle,
	const RECT& rect,
	CWnd* pParentWnd, UINT nID,
	CCreateContext* pContext)
{
	// нельзя использовать для рабочего стола или всплывающих окон (вместо этого используйте CreateEx)
	ASSERT(pParentWnd!= NULL);
	ASSERT((dwStyle & WS_POPUP) == 0);

	return CreateEx(0, lpszClassName, lpszWindowName,
		dwStyle | WS_CHILD,
		rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top,
		pParentWnd->GetSafeHwnd(), (HMENU)nID, (LPVOID)pContext);
}

CWnd::~CWnd()
{
	if (m_hWnd != NULL &&
		this != (CWnd*)&wndTop && this != (CWnd*)&wndBottom &&
		this != (CWnd*)&wndTopMost && this != (CWnd*)&wndNoTopMost)
	{
		TRACE(_T("Warning: calling DestroyWindow in CWnd::~CWnd; ")
			_T("OnDestroy or PostNcDestroy in derived class will not be called.\n"));
		DestroyWindow();
	}

#ifndef _AFX_NO_OCC_SUPPORT
	// cleanup control container,
	// including destroying controls

	delete m_pCtrlCont;

	// cleanup control site
	if (m_pCtrlSite != NULL && m_pCtrlSite->m_pWndCtrl == this)
		m_pCtrlSite->m_pWndCtrl = NULL;
#endif
}

void CWnd::OnDestroy()
{
#ifndef _AFX_NO_OCC_SUPPORT
	// cleanup control container
	delete m_pCtrlCont;
	m_pCtrlCont = NULL;
#endif
	Default();
}

// WM_NCDESTROY is the absolute LAST message sent.
void CWnd::OnNcDestroy()
{
	// cleanup main and active windows
	CWinThread* pThread = AfxGetThread();
	if (pThread != NULL)
	{
		if (pThread->m_pMainWnd == this)
		{
			if (!afxContextIsDLL)
			{
				// shut down current thread if possible
				if (pThread != AfxGetApp() || AfxOleCanExitApp())
					AfxPostQuitMessage(0);
			}
			pThread->m_pMainWnd = NULL;
		}
		if (pThread->m_pActiveWnd == this)
			pThread->m_pActiveWnd = NULL;
	}

#ifndef _AFX_NO_OLE_SUPPORT
	// cleanup OLE drop target interface
	if (m_pDropTarget != NULL)
	{
		m_pDropTarget->Revoke();
		m_pDropTarget = NULL;
	}
#endif

#ifndef _AFX_NO_OCC_SUPPORT
	// cleanup control container
	delete m_pCtrlCont;
	m_pCtrlCont = NULL;
#endif

	// cleanup tooltip support
	if (m_nFlags & WF_TOOLTIPS)
	{
		CToolTipCtrl* pToolTip = AfxGetThreadState()->m_pToolTip;
		if (pToolTip->GetSafeHwnd() != NULL)
		{
			TOOLINFO ti; memset(&ti, 0, sizeof(TOOLINFO));
			ti.cbSize = sizeof(AFX_OLDTOOLINFO);
			ti.uFlags = TTF_IDISHWND;
			ti.hwnd = m_hWnd;
			ti.uId = (UINT)m_hWnd;
			pToolTip->SendMessage(TTM_DELTOOL, 0, (LPARAM)&ti);
		}
	}

	// call default, unsubclass, and detach from the map
	LONG pfnWndProc = GetWindowLong(m_hWnd, GWL_WNDPROC);
	Default();
	if (GetWindowLong(m_hWnd, GWL_WNDPROC) == pfnWndProc)
	{
		WNDPROC pfnSuper = *GetSuperWndProcAddr();
		if (pfnSuper != NULL)
			SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)pfnSuper);
	}
	Detach();
	ASSERT(m_hWnd == NULL);

	// call special post-cleanup routine
	PostNcDestroy();
}

void CWnd::PostNcDestroy()
{
	// default to nothing
}

void CWnd::OnFinalRelease()
{
	if (m_hWnd != NULL)
		DestroyWindow();    // will call PostNcDestroy
	else
		PostNcDestroy();
}

#ifdef _DEBUG
void CWnd::AssertValid() const
{
	if (m_hWnd == NULL)
		return;     // null (unattached) windows are valid

	// check for special wnd??? values
	ASSERT(HWND_TOP == NULL);       // same as desktop
	if (m_hWnd == HWND_BOTTOM)
		ASSERT(this == &CWnd::wndBottom);
	else if (m_hWnd == HWND_TOPMOST)
		ASSERT(this == &CWnd::wndTopMost);
	else if (m_hWnd == HWND_NOTOPMOST)
		ASSERT(this == &CWnd::wndNoTopMost);
	else
	{
		// should be a normal window
		ASSERT(::IsWindow(m_hWnd));

		// should also be in the permanent or temporary handle map
		CHandleMap* pMap = afxMapHWND();
		ASSERT(pMap != NULL);

		CObject* p;
		ASSERT((p = pMap->LookupPermanent(m_hWnd)) != NULL ||
			(p = pMap->LookupTemporary(m_hWnd)) != NULL);
		ASSERT((CWnd*)p == this);   // must be us

		// Note: if either of the above asserts fire and you are
		// writing a multithreaded application, it is likely that
		// you have passed a C++ object from one thread to another
		// and have used that object in a way that was not intended.
		// (only simple inline wrapper functions should be used)
		//
		// In general, CWnd objects should be passed by HWND from
		// one thread to another.  The receiving thread can wrap
		// the HWND with a CWnd object by using CWnd::FromHandle.
		//
		// It is dangerous to pass C++ objects from one thread to
		// another, unless the objects are designed to be used in
		// such a manner.
	}
}

void CWnd::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	dc << "\nm_hWnd = " << (UINT)m_hWnd;

	if (m_hWnd == NULL || m_hWnd == HWND_BOTTOM ||
		m_hWnd == HWND_TOPMOST || m_hWnd == HWND_NOTOPMOST)
	{
		// not a normal window - nothing more to dump
		return;
	}

	if (!::IsWindow(m_hWnd))
	{
		// not a valid window
		dc << " (illegal HWND)";
		return; // don't do anything more
	}

	CWnd* pWnd = CWnd::FromHandlePermanent(m_hWnd);
	if (pWnd != this)
		dc << " (Detached or temporary window)";
	else
		dc << " (permanent window)";

	// dump out window specific statistics
	TCHAR szBuf[64];
	if (!::SendMessage(m_hWnd, WM_QUERYAFXWNDPROC, 0, 0) && pWnd == this)
		GetWindowText(szBuf, _countof(szBuf));
	else
		::DefWindowProc(m_hWnd, WM_GETTEXT, _countof(szBuf), (LPARAM)&szBuf[0]);
	dc << "\ncaption = \"" << szBuf << "\"";

	::GetClassName(m_hWnd, szBuf, _countof(szBuf));
	dc << "\nclass name = \"" << szBuf << "\"";

	CRect rect;
	GetWindowRect(&rect);
	dc << "\nrect = " << rect;
	dc << "\nparent CWnd* = " << (void*)GetParent();

	dc << "\nstyle = " << (void*)::GetWindowLong(m_hWnd, GWL_STYLE);
	if (::GetWindowLong(m_hWnd, GWL_STYLE) & WS_CHILD)
		dc << "\nid = " << _AfxGetDlgCtrlID(m_hWnd);

	dc << "\n";
}
#endif

BOOL CWnd::DestroyWindow()
{
	if (m_hWnd == NULL)
		return FALSE;

	CHandleMap* pMap = afxMapHWND();
	ASSERT(pMap != NULL);
	CWnd* pWnd = (CWnd*)pMap->LookupPermanent(m_hWnd);
#ifdef _DEBUG
	HWND hWndOrig = m_hWnd;
#endif

#ifdef _AFX_NO_OCC_SUPPORT
	BOOL bResult = ::DestroyWindow(m_hWnd);
#else //_AFX_NO_OCC_SUPPORT
	BOOL bResult;
	if (m_pCtrlSite == NULL)
		bResult = ::DestroyWindow(m_hWnd);
	else
		bResult = m_pCtrlSite->DestroyControl();
#endif //_AFX_NO_OCC_SUPPORT

	// Note that 'this' may have been deleted at this point,
	//  (but only if pWnd != NULL)
	if (pWnd != NULL)
	{
		// Should have been detached by OnNcDestroy
#ifdef _DEBUG
		ASSERT(pMap->LookupPermanent(hWndOrig) == NULL);
#endif
	}
	else
	{
#ifdef _DEBUG
		ASSERT(m_hWnd == hWndOrig);
#endif
		// Detach after DestroyWindow called just in case
		Detach();
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// Default CWnd implementation

LRESULT CWnd::DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_pfnSuper != NULL)
		return ::CallWindowProc(m_pfnSuper, m_hWnd, nMsg, wParam, lParam);

	WNDPROC pfnWndProc;
	if ((pfnWndProc = *GetSuperWndProcAddr()) == NULL)
		return ::DefWindowProc(m_hWnd, nMsg, wParam, lParam);
	else
		return ::CallWindowProc(pfnWndProc, m_hWnd, nMsg, wParam, lParam);
}

WNDPROC* CWnd::GetSuperWndProcAddr()
{
	// Note: it is no longer necessary to override GetSuperWndProcAddr
	//  for each control class with a different WNDCLASS.
	//  This implementation now uses instance data, such that the previous
	//  WNDPROC can be anything.

	return &m_pfnSuper;
}

BOOL CWnd::PreTranslateMessage(MSG* pMsg)
{
	// handle tooltip messages (some messages cancel, some may cause it to popup)
	AFX_MODULE_STATE* pModuleState = _AFX_CMDTARGET_GETSTATE();
	if (pModuleState->m_pfnFilterToolTipMessage != NULL)
		(*pModuleState->m_pfnFilterToolTipMessage)(pMsg, this);

	// no default processing
	return FALSE;
}

void PASCAL CWnd::CancelToolTips(BOOL bKeys)
{
	// check for active tooltip
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	CToolTipCtrl* pToolTip = pThreadState->m_pToolTip;
	if (pToolTip->GetSafeHwnd() != NULL)
		pToolTip->SendMessage(TTM_ACTIVATE, FALSE);

	// check for active control bar fly-by status
	CControlBar* pLastStatus = pThreadState->m_pLastStatus;
	if (bKeys && pLastStatus != NULL && GetKeyState(VK_LBUTTON) >= 0)
		pLastStatus->SetStatusText(-1);
}

int CWnd::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	// find child window which hits the point
	// (don't use WindowFromPoint, because it ignores disabled windows)
	HWND hWndChild = _AfxChildWindowFromPoint(m_hWnd, point);
	if (hWndChild != NULL)
	{
		// return positive hit if control ID isn't -1
		int nHit = _AfxGetDlgCtrlID(hWndChild);

		// hits against child windows always center the tip
		if (pTI != NULL && pTI->cbSize >= sizeof(AFX_OLDTOOLINFO))
		{
			// setup the TOOLINFO structure
			pTI->hwnd = m_hWnd;
			pTI->uId = (UINT)hWndChild;
			pTI->uFlags |= TTF_IDISHWND;
			pTI->lpszText = LPSTR_TEXTCALLBACK;

			// set TTF_NOTBUTTON and TTF_CENTERTIP if it isn't a button
			if (!(::SendMessage(hWndChild, WM_GETDLGCODE, 0, 0) & DLGC_BUTTON))
				pTI->uFlags |= TTF_NOTBUTTON | TTF_CENTERTIP;
		}
		return nHit;
	}
	return -1;  // not found
}

void CWnd::GetWindowText(CString& rString) const
{
	ASSERT(::IsWindow(m_hWnd));

#ifndef _AFX_NO_OCC_SUPPORT
	if (m_pCtrlSite == NULL)
	{
#endif
		int nLen = ::GetWindowTextLength(m_hWnd);
		::GetWindowText(m_hWnd, rString.GetBufferSetLength(nLen), nLen + 1);
		rString.ReleaseBuffer();

#ifndef _AFX_NO_OCC_SUPPORT
	}
	else
	{
		m_pCtrlSite->GetWindowText(rString);
	}
#endif
}

int CWnd::GetDlgItemText(int nID, CString& rString) const
{
	ASSERT(::IsWindow(m_hWnd));
	rString = &afxChNil;    // empty without deallocating

#ifndef _AFX_NO_OCC_SUPPORT
	if (m_pCtrlCont == NULL)
	{
#endif
		HWND hWnd = ::GetDlgItem(m_hWnd, nID);
		if (hWnd != NULL)
		{
			int nLen = ::GetWindowTextLength(hWnd);
			::GetWindowText(hWnd, rString.GetBufferSetLength(nLen), nLen + 1);
			rString.ReleaseBuffer();
		}

#ifndef _AFX_NO_OCC_SUPPORT
	}
	else
	{
		CWnd* pWnd = GetDlgItem(nID);
		if (pWnd != NULL)
			pWnd->GetWindowText(rString);
	}
#endif

	return rString.GetLength();
}

BOOL CWnd::GetWindowPlacement(WINDOWPLACEMENT* lpwndpl) const
{
	ASSERT(::IsWindow(m_hWnd));
	lpwndpl->length = sizeof(WINDOWPLACEMENT);
	return ::GetWindowPlacement(m_hWnd, lpwndpl);
}

BOOL CWnd::SetWindowPlacement(const WINDOWPLACEMENT* lpwndpl)
{
	ASSERT(::IsWindow(m_hWnd));
	((WINDOWPLACEMENT*)lpwndpl)->length = sizeof(WINDOWPLACEMENT);
	return ::SetWindowPlacement(m_hWnd, lpwndpl);
}

/////////////////////////////////////////////////////////////////////////////
// CWnd will delegate owner draw messages to self drawing controls

// Drawing: for all 4 control types
void CWnd::OnDrawItem(int /*nIDCtl*/, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->CtlType == ODT_MENU)
	{
		CMenu* pMenu = CMenu::FromHandlePermanent(
			(HMENU)lpDrawItemStruct->hwndItem);
		if (pMenu != NULL)
		{
			pMenu->DrawItem(lpDrawItemStruct);
			return; // eat it
		}
	}
	else
	{
		// reflect notification to child window control
		if (ReflectLastMsg(lpDrawItemStruct->hwndItem))
			return;     // eat it
	}
	// not handled - do default
	Default();
}

// Drawing: for all 4 control types
int CWnd::OnCompareItem(int /*nIDCtl*/, LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
	// reflect notification to child window control
	LRESULT lResult;
	if (ReflectLastMsg(lpCompareItemStruct->hwndItem, &lResult))
		return (int)lResult;        // eat it

	// not handled - do default
	return (int)Default();
}

void CWnd::OnDeleteItem(int /*nIDCtl*/, LPDELETEITEMSTRUCT lpDeleteItemStruct)
{
	// reflect notification to child window control
	if (ReflectLastMsg(lpDeleteItemStruct->hwndItem))
		return;     // eat it
	// not handled - do default
	Default();
}

int CWnd::OnCharToItem(UINT, CListBox* pWnd, UINT)
{
	if (pWnd != NULL)
	{
		LRESULT lResult;
		if (pWnd->SendChildNotifyLastMsg(&lResult))
			return (int)lResult;     // eat it
	}
	// not handled - do default
	return (int)Default();
}

int CWnd::OnVKeyToItem(UINT, CListBox* pWnd, UINT)
{
	if (pWnd != NULL)
	{
		LRESULT lResult;
		if (pWnd->SendChildNotifyLastMsg(&lResult))
			return (int)lResult;     // eat it
	}
	// not handled - do default
	return (int)Default();
}

/////////////////////////////////////////////////////////////////////////////
// Self drawing menus are a little trickier

BOOL CMenu::TrackPopupMenu(UINT nFlags, int x, int y,
	CWnd* pWnd, LPCRECT lpRect)
{
	ASSERT(m_hMenu != NULL);

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	HWND hWndOld = pThreadState->m_hTrackingWindow;
	HMENU hMenuOld = pThreadState->m_hTrackingMenu;
	pThreadState->m_hTrackingWindow = pWnd->GetSafeHwnd();
	pThreadState->m_hTrackingMenu = m_hMenu;
	BOOL bOK = ::TrackPopupMenu(m_hMenu, nFlags, x, y, 0,
		pThreadState->m_hTrackingWindow, lpRect);
	pThreadState->m_hTrackingWindow = hWndOld;
	pThreadState->m_hTrackingMenu = hMenuOld;

	return bOK;
}

AFX_STATIC CMenu* AFXAPI _AfxFindPopupMenuFromID(CMenu* pMenu, UINT nID)
{
	ASSERT_VALID(pMenu);
	// walk through all items, looking for ID match
	UINT nItems = pMenu->GetMenuItemCount();
	for (int iItem = 0; iItem < (int)nItems; iItem++)
	{
		CMenu* pPopup = pMenu->GetSubMenu(iItem);
		if (pPopup != NULL)
		{
			// recurse to child popup
			pPopup = _AfxFindPopupMenuFromID(pPopup, nID);
			// check popups on this popup
			if (pPopup != NULL)
				return pPopup;
		}
		else if (pMenu->GetMenuItemID(iItem) == nID)
		{
			// it is a normal item inside our popup
			pMenu = CMenu::FromHandlePermanent(pMenu->m_hMenu);
			return pMenu;
		}
	}
	// not found
	return NULL;
}

// Measure item implementation relies on unique control/menu IDs
void CWnd::OnMeasureItem(int /*nIDCtl*/, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (lpMeasureItemStruct->CtlType == ODT_MENU)
	{
		ASSERT(lpMeasureItemStruct->CtlID == 0);
		CMenu* pMenu;

		_AFX_THREAD_STATE* pThreadState = _afxThreadState.GetData();
		if (pThreadState->m_hTrackingWindow == m_hWnd)
		{
			// start from popup
			pMenu = CMenu::FromHandle(pThreadState->m_hTrackingMenu);
		}
		else
		{
			// start from menubar
			pMenu = GetMenu();
		}

		pMenu = _AfxFindPopupMenuFromID(pMenu, lpMeasureItemStruct->itemID);
		if (pMenu != NULL)
			pMenu->MeasureItem(lpMeasureItemStruct);
		else
			TRACE1("Warning: unknown WM_MEASUREITEM for menu item 0x%04X.\n",
				lpMeasureItemStruct->itemID);
	}
	else
	{
		CWnd* pChild = GetDescendantWindow(lpMeasureItemStruct->CtlID, TRUE);
		if (pChild != NULL && pChild->SendChildNotifyLastMsg())
			return;     // eaten by child
	}
	// not handled - do default
	Default();
}

/////////////////////////////////////////////////////////////////////////////
// Additional helpers for WNDCLASS init

// like RegisterClass, except will automatically call UnregisterClass
BOOL AFXAPI AfxRegisterClass(WNDCLASS* lpWndClass)
{
	WNDCLASS wndcls;
	if (GetClassInfo(lpWndClass->hInstance, lpWndClass->lpszClassName,
		&wndcls))
	{
		// class already registered
		return TRUE;
	}

	if (!::RegisterClass(lpWndClass))
	{
		TRACE1("Can't register window class named %s\n",
			lpWndClass->lpszClassName);
		return FALSE;
	}

	if (afxContextIsDLL)
	{
		AfxLockGlobals(CRIT_REGCLASSLIST);
		TRY
		{
			// class registered successfully, add to registered list
			AFX_MODULE_STATE * pModuleState = AfxGetModuleState();
			LPTSTR lpszUnregisterList = pModuleState->m_szUnregisterList;
			// the buffer is of fixed size -- ensure that it does not overflow
			ASSERT(lstrlen(lpszUnregisterList) + 1 +
				lstrlen(lpWndClass->lpszClassName) + 1 <
				_countof(pModuleState->m_szUnregisterList));
			// append classname + newline to m_szUnregisterList
			lstrcat(lpszUnregisterList, lpWndClass->lpszClassName);
			TCHAR szTemp[2];
			szTemp[0] = '\n';
			szTemp[1] = '\0';
			lstrcat(lpszUnregisterList, szTemp);
		}
			CATCH_ALL(e)
		{
			AfxUnlockGlobals(CRIT_REGCLASSLIST);
			THROW_LAST();
			// Note: DELETE_EXCEPTION not required.
		}
		END_CATCH_ALL
			AfxUnlockGlobals(CRIT_REGCLASSLIST);
	}

	return TRUE;
}

LPCTSTR AFXAPI AfxRegisterWndClass(UINT nClassStyle,
	HCURSOR hCursor, HBRUSH hbrBackground, HICON hIcon)
{
	// Returns a temporary string name for the class
	//  Save in a CString if you want to use it for a long time
	LPTSTR lpszName = AfxGetThreadState()->m_szTempClassName;

	// generate a synthetic name for this class
	HINSTANCE hInst = AfxGetInstanceHandle();
	if (hCursor == NULL && hbrBackground == NULL && hIcon == NULL)
		wsprintf(lpszName, _T("Afx:%x:%x"), (UINT)hInst, nClassStyle);
	else
		wsprintf(lpszName, _T("Afx:%x:%x:%x:%x:%x"), (UINT)hInst, nClassStyle,
		(UINT)hCursor, (UINT)hbrBackground, (UINT)hIcon);

	// see if the class already exists
	WNDCLASS wndcls;
	if (::GetClassInfo(hInst, lpszName, &wndcls))
	{
		// already registered, assert everything is good
		ASSERT(wndcls.style == nClassStyle);

		// NOTE: We have to trust that the hIcon, hbrBackground, and the
		//  hCursor are semantically the same, because sometimes Windows does
		//  some internal translation or copying of those handles before
		//  storing them in the internal WNDCLASS retrieved by GetClassInfo.
		return lpszName;
	}

	// otherwise we need to register a new class
	wndcls.style = nClassStyle;
	wndcls.lpfnWndProc = DefWindowProc;
	wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
	wndcls.hInstance = hInst;
	wndcls.hIcon = hIcon;
	wndcls.hCursor = hCursor;
	wndcls.hbrBackground = hbrBackground;
	wndcls.lpszMenuName = NULL;
	wndcls.lpszClassName = lpszName;
	if (!AfxRegisterClass(&wndcls))
		AfxThrowResourceException();

	// return thread-local pointer
	return lpszName;
}

struct AFX_CTLCOLOR
{
	HWND hWnd;
	HDC hDC;
	UINT nCtlType;
};

LRESULT CWnd::OnNTCtlColor(WPARAM wParam, LPARAM lParam)
{
	// fill in special struct for compatiblity with 16-bit WM_CTLCOLOR
	AFX_CTLCOLOR ctl;
	ctl.hDC = (HDC)wParam;
	ctl.hWnd = (HWND)lParam;
	_AFX_THREAD_STATE* pThreadState = _afxThreadState.GetData();
	ctl.nCtlType = pThreadState->m_lastSentMsg.message - WM_CTLCOLORMSGBOX;
	ASSERT(ctl.nCtlType >= CTLCOLOR_MSGBOX);
	ASSERT(ctl.nCtlType <= CTLCOLOR_STATIC);

	// Note: We call the virtual WindowProc for this window directly,
	//  instead of calling AfxCallWindowProc, so that Default()
	//  will still work (it will call the Default window proc with
	//  the original Win32 WM_CTLCOLOR message).
	return WindowProc(WM_CTLCOLOR, 0, (LPARAM)&ctl);
}

/////////////////////////////////////////////////////////////////////////////
// CWnd extensions for help support

void CWnd::WinHelp(DWORD dwData, UINT nCmd)
{
	CWinApp* pApp = AfxGetApp();
	ASSERT_VALID(pApp);
	ASSERT(pApp->m_pszHelpFilePath != NULL);

	CWaitCursor wait;
	if (IsFrameWnd())
	{
		// CFrameWnd windows should be allowed to exit help mode first
		CFrameWnd* pFrameWnd = (CFrameWnd*)this;
		pFrameWnd->ExitHelpMode();
	}

	// cancel any tracking modes
	SendMessage(WM_CANCELMODE);
	SendMessageToDescendants(WM_CANCELMODE, 0, 0, TRUE, TRUE);

	// need to use top level parent (for the case where m_hWnd is in DLL)
	CWnd* pWnd = GetTopLevelParent();
	pWnd->SendMessage(WM_CANCELMODE);
	pWnd->SendMessageToDescendants(WM_CANCELMODE, 0, 0, TRUE, TRUE);

	// attempt to cancel capture
	HWND hWndCapture = ::GetCapture();
	if (hWndCapture != NULL)
		::SendMessage(hWndCapture, WM_CANCELMODE, 0, 0);

	TRACE3("WinHelp: pszHelpFile = '%s', dwData: $%lx, fuCommand: %d.\n",
		pApp->m_pszHelpFilePath, dwData, nCmd);

	// finally, run the Windows Help engine
	if (!::WinHelp(pWnd->m_hWnd, pApp->m_pszHelpFilePath, nCmd, dwData))
		AfxMessageBox(AFX_IDP_FAILED_TO_LAUNCH_HELP);
}

/////////////////////////////////////////////////////////////////////////////
// Message table implementation

BEGIN_MESSAGE_MAP(CWnd, CCmdTarget)
	ON_MESSAGE(WM_CTLCOLORSTATIC, OnNTCtlColor)
	ON_MESSAGE(WM_CTLCOLOREDIT, OnNTCtlColor)
	ON_MESSAGE(WM_CTLCOLORBTN, OnNTCtlColor)
	ON_MESSAGE(WM_CTLCOLORLISTBOX, OnNTCtlColor)
	ON_MESSAGE(WM_CTLCOLORDLG, OnNTCtlColor)
	ON_MESSAGE(WM_CTLCOLORMSGBOX, OnNTCtlColor)
	ON_MESSAGE(WM_CTLCOLORSCROLLBAR, OnNTCtlColor)
	//{{AFX_MSG_MAP(CWnd)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_WM_CTLCOLOR()
	ON_WM_COMPAREITEM()
	ON_WM_ENTERIDLE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_DELETEITEM()
	ON_WM_CHARTOITEM()
	ON_WM_VKEYTOITEM()
	ON_WM_NCDESTROY()
	ON_WM_PARENTNOTIFY()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_DEVMODECHANGE()
	ON_WM_HELPINFO()
	ON_WM_SETTINGCHANGE()
	//}}AFX_MSG_MAP
#ifndef _AFX_NO_OCC_SUPPORT
	ON_WM_DESTROY()
#endif
	ON_MESSAGE(WM_ACTIVATETOPLEVEL, OnActivateTopLevel)
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_REGISTERED_MESSAGE(CWnd::m_nMsgDragList, OnDragList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Routines for fast search of message maps

const AFX_MSGMAP_ENTRY* AFXAPI
AfxFindMessageEntry(const AFX_MSGMAP_ENTRY* lpEntry,
	UINT nMsg, UINT nCode, UINT nID)
{
#if defined(_M_IX86) && !defined(_AFX_PORTABLE)
	// 32-bit Intel 386/486 version.

	ASSERT(offsetof(AFX_MSGMAP_ENTRY, nMessage) == 0);
	ASSERT(offsetof(AFX_MSGMAP_ENTRY, nCode) == 4);
	ASSERT(offsetof(AFX_MSGMAP_ENTRY, nID) == 8);
	ASSERT(offsetof(AFX_MSGMAP_ENTRY, nLastID) == 12);
	ASSERT(offsetof(AFX_MSGMAP_ENTRY, nSig) == 16);

	_asm
	{
		MOV     EBX, lpEntry
		MOV     EAX, nMsg
		MOV     EDX, nCode
		MOV     ECX, nID
		__loop :
		CMP     DWORD PTR[EBX + 16], 0; nSig(0 = > end)
			JZ      __failed
			CMP     EAX, DWORD PTR[EBX]; nMessage
			JE      __found_message
			__next :
		ADD     EBX, SIZE AFX_MSGMAP_ENTRY
			JMP     short __loop
			__found_message :
		CMP     EDX, DWORD PTR[EBX + 4]; nCode
			JNE     __next
			// message and code good so far
			// check the ID
			CMP     ECX, DWORD PTR[EBX + 8]; nID
			JB      __next
			CMP     ECX, DWORD PTR[EBX + 12]; nLastID
			JA      __next
			// found a match
			MOV     lpEntry, EBX; return EBX
			JMP     short __end
			__failed :
		XOR     EAX, EAX; return NULL
			MOV     lpEntry, EAX
			__end :
	}
	return lpEntry;
#else  // _AFX_PORTABLE
	// C version of search routine
	while (lpEntry->nSig != AfxSig_end)
	{
		if (lpEntry->nMessage == nMsg && lpEntry->nCode == nCode &&
			nID >= lpEntry->nID && nID <= lpEntry->nLastID)
		{
			return lpEntry;
		}
		lpEntry++;
	}
	return NULL;    // not found
#endif  // _AFX_PORTABLE
}

/////////////////////////////////////////////////////////////////////////////
// Cache of most recently sent messages

#ifndef iHashMax
// iHashMax must be a power of two
#define iHashMax 512
#endif

struct AFX_MSG_CACHE
{
	UINT nMsg;
	const AFX_MSGMAP_ENTRY* lpEntry;
	const AFX_MSGMAP* pMessageMap;
};

AFX_MSG_CACHE _afxMsgCache[iHashMax];

void AFXAPI AfxResetMsgCache()
{
	memset(_afxMsgCache, 0, sizeof(_afxMsgCache));
}

/////////////////////////////////////////////////////////////////////////////
// main WindowProc implementation

LRESULT CWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// OnWndMsg does most of the work, except for DefWindowProc call
	LRESULT lResult = 0;
	if (!OnWndMsg(message, wParam, lParam, &lResult))
		lResult = DefWindowProc(message, wParam, lParam);
	return lResult;
}

BOOL CWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	LRESULT lResult = 0;

	// special case for commands
	if (message == WM_COMMAND)
	{
		if (OnCommand(wParam, lParam))
		{
			lResult = 1;
			goto LReturnTrue;
		}
		return FALSE;
	}

	// special case for notifies
	if (message == WM_NOTIFY)
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if (pNMHDR->hwndFrom != NULL && OnNotify(wParam, lParam, &lResult))
			goto LReturnTrue;
		return FALSE;
	}

	// special case for activation
	if (message == WM_ACTIVATE)
		_AfxHandleActivate(this, wParam, CWnd::FromHandle((HWND)lParam));

	// special case for set cursor HTERROR
	if (message == WM_SETCURSOR &&
		_AfxHandleSetCursor(this, (short)LOWORD(lParam), HIWORD(lParam)))
	{
		lResult = 1;
		goto LReturnTrue;
	}

	const AFX_MSGMAP* pMessageMap; pMessageMap = GetMessageMap();
	UINT iHash; iHash = (LOWORD((DWORD)pMessageMap) ^ message) & (iHashMax - 1);
	AfxLockGlobals(CRIT_WINMSGCACHE);
	AFX_MSG_CACHE* pMsgCache; pMsgCache = &_afxMsgCache[iHash];
	const AFX_MSGMAP_ENTRY* lpEntry;
	if (message == pMsgCache->nMsg && pMessageMap == pMsgCache->pMessageMap)
	{
		// cache hit
		lpEntry = pMsgCache->lpEntry;
		AfxUnlockGlobals(CRIT_WINMSGCACHE);
		if (lpEntry == NULL)
			return FALSE;

		// cache hit, and it needs to be handled
		if (message < 0xC000)
			goto LDispatch;
		else
			goto LDispatchRegistered;
	}
	else
	{
		// not in cache, look for it
		pMsgCache->nMsg = message;
		pMsgCache->pMessageMap = pMessageMap;

#ifdef _AFXDLL
		for (/* pMessageMap already init'ed */; pMessageMap != NULL;
			pMessageMap = (*pMessageMap->pfnGetBaseMap)())
#else
		for (/* pMessageMap already init'ed */; pMessageMap != NULL;
			pMessageMap = pMessageMap->pBaseMap)
#endif
		{
			// Note: catch not so common but fatal mistake!!
			//      BEGIN_MESSAGE_MAP(CMyWnd, CMyWnd)
#ifdef _AFXDLL
			ASSERT(pMessageMap != (*pMessageMap->pfnGetBaseMap)());
#else
			ASSERT(pMessageMap != pMessageMap->pBaseMap);
#endif

			if (message < 0xC000)
			{
				// constant window message
				if ((lpEntry = AfxFindMessageEntry(pMessageMap->lpEntries,
					message, 0, 0)) != NULL)
				{
					pMsgCache->lpEntry = lpEntry;
					AfxUnlockGlobals(CRIT_WINMSGCACHE);
					goto LDispatch;
				}
			}
			else
			{
				// registered windows message
				lpEntry = pMessageMap->lpEntries;
				while ((lpEntry = AfxFindMessageEntry(lpEntry, 0xC000, 0, 0)) != NULL)
				{
					UINT* pnID = (UINT*)(lpEntry->nSig);
					ASSERT(*pnID >= 0xC000 || *pnID == 0);
					// must be successfully registered
					if (*pnID == message)
					{
						pMsgCache->lpEntry = lpEntry;
						AfxUnlockGlobals(CRIT_WINMSGCACHE);
						goto LDispatchRegistered;
					}
					lpEntry++;      // keep looking past this one
				}
			}
		}

		pMsgCache->lpEntry = NULL;
		AfxUnlockGlobals(CRIT_WINMSGCACHE);
		return FALSE;
	}
	ASSERT(FALSE);      // not reached

LDispatch:
	ASSERT(message < 0xC000);
	union MessageMapFunctions mmf;
	mmf.pfn = lpEntry->pfn;

	// if we've got WM_SETTINGCHANGE / WM_WININICHANGE, we need to
	// decide if we're going to call OnWinIniChange() or OnSettingChange()

	int nSig;
	nSig = lpEntry->nSig;
	if (lpEntry->nID == WM_SETTINGCHANGE)
	{
		DWORD dwVersion = GetVersion();
		if (LOBYTE(LOWORD(dwVersion)) >= 4)
			nSig = AfxSig_vws;
		else
			nSig = AfxSig_vs;
	}

	switch (nSig)
	{
	default:
		ASSERT(FALSE);
		break;

	case AfxSig_bD:
		lResult = (this->*mmf.pfn_bD)(CDC::FromHandle((HDC)wParam));
		break;

	case AfxSig_bb:     // AfxSig_bb, AfxSig_bw, AfxSig_bh
		lResult = (this->*mmf.pfn_bb)((BOOL)wParam);
		break;

	case AfxSig_bWww:   // really AfxSig_bWiw
		lResult = (this->*mmf.pfn_bWww)(CWnd::FromHandle((HWND)wParam),
			(short)LOWORD(lParam), HIWORD(lParam));
		break;

	case AfxSig_bWCDS:
		lResult = (this->*mmf.pfn_bWCDS)(CWnd::FromHandle((HWND)wParam),
			(COPYDATASTRUCT*)lParam);
		break;

	case AfxSig_bHELPINFO:
		lResult = (this->*mmf.pfn_bHELPINFO)((HELPINFO*)lParam);
		break;

	case AfxSig_hDWw:
	{
		// special case for OnCtlColor to avoid too many temporary objects
		ASSERT(message == WM_CTLCOLOR);
		AFX_CTLCOLOR* pCtl = (AFX_CTLCOLOR*)lParam;
		CDC dcTemp; dcTemp.m_hDC = pCtl->hDC;
		CWnd wndTemp; wndTemp.m_hWnd = pCtl->hWnd;
		UINT nCtlType = pCtl->nCtlType;
		// if not coming from a permanent window, use stack temporary
		CWnd* pWnd = CWnd::FromHandlePermanent(wndTemp.m_hWnd);
		if (pWnd == NULL)
		{
#ifndef _AFX_NO_OCC_SUPPORT
			// determine the site of the OLE control if it is one
			COleControlSite* pSite;
			if (m_pCtrlCont != NULL && (pSite = (COleControlSite*)
				m_pCtrlCont->m_siteMap.GetValueAt(wndTemp.m_hWnd)) != NULL)
			{
				wndTemp.m_pCtrlSite = pSite;
			}
#endif
			pWnd = &wndTemp;
		}
		HBRUSH hbr = (this->*mmf.pfn_hDWw)(&dcTemp, pWnd, nCtlType);
		// fast detach of temporary objects
		dcTemp.m_hDC = NULL;
		wndTemp.m_hWnd = NULL;
		lResult = (LRESULT)hbr;
	}
	break;

	case AfxSig_hDw:
	{
		// special case for CtlColor to avoid too many temporary objects
		ASSERT(message == WM_REFLECT_BASE + WM_CTLCOLOR);
		AFX_CTLCOLOR* pCtl = (AFX_CTLCOLOR*)lParam;
		CDC dcTemp; dcTemp.m_hDC = pCtl->hDC;
		UINT nCtlType = pCtl->nCtlType;
		HBRUSH hbr = (this->*mmf.pfn_hDw)(&dcTemp, nCtlType);
		// fast detach of temporary objects
		dcTemp.m_hDC = NULL;
		lResult = (LRESULT)hbr;
	}
	break;

	case AfxSig_iwWw:
		lResult = (this->*mmf.pfn_iwWw)(LOWORD(wParam),
			CWnd::FromHandle((HWND)lParam), HIWORD(wParam));
		break;

	case AfxSig_iww:
		lResult = (this->*mmf.pfn_iww)(LOWORD(wParam), HIWORD(wParam));
		break;

	case AfxSig_iWww:   // really AfxSig_iWiw
		lResult = (this->*mmf.pfn_iWww)(CWnd::FromHandle((HWND)wParam),
			(short)LOWORD(lParam), HIWORD(lParam));
		break;

	case AfxSig_is:
		lResult = (this->*mmf.pfn_is)((LPTSTR)lParam);
		break;

	case AfxSig_lwl:
		lResult = (this->*mmf.pfn_lwl)(wParam, lParam);
		break;

	case AfxSig_lwwM:
		lResult = (this->*mmf.pfn_lwwM)((UINT)LOWORD(wParam),
			(UINT)HIWORD(wParam), (CMenu*)CMenu::FromHandle((HMENU)lParam));
		break;

	case AfxSig_vv:
		(this->*mmf.pfn_vv)();
		break;

	case AfxSig_vw: // AfxSig_vb, AfxSig_vh
		(this->*mmf.pfn_vw)(wParam);
		break;

	case AfxSig_vww:
		(this->*mmf.pfn_vww)((UINT)wParam, (UINT)lParam);
		break;

	case AfxSig_vvii:
		(this->*mmf.pfn_vvii)((short)LOWORD(lParam), (short)HIWORD(lParam));
		break;

	case AfxSig_vwww:
		(this->*mmf.pfn_vwww)(wParam, LOWORD(lParam), HIWORD(lParam));
		break;

	case AfxSig_vwii:
		(this->*mmf.pfn_vwii)(wParam, LOWORD(lParam), HIWORD(lParam));
		break;

	case AfxSig_vwl:
		(this->*mmf.pfn_vwl)(wParam, lParam);
		break;

	case AfxSig_vbWW:
		(this->*mmf.pfn_vbWW)(m_hWnd == (HWND)lParam,
			CWnd::FromHandle((HWND)lParam),
			CWnd::FromHandle((HWND)wParam));
		break;

	case AfxSig_vD:
		(this->*mmf.pfn_vD)(CDC::FromHandle((HDC)wParam));
		break;

	case AfxSig_vM:
		(this->*mmf.pfn_vM)(CMenu::FromHandle((HMENU)wParam));
		break;

	case AfxSig_vMwb:
		(this->*mmf.pfn_vMwb)(CMenu::FromHandle((HMENU)wParam),
			LOWORD(lParam), (BOOL)HIWORD(lParam));
		break;

	case AfxSig_vW:
		(this->*mmf.pfn_vW)(CWnd::FromHandle((HWND)wParam));
		break;

	case AfxSig_vW2:
		(this->*mmf.pfn_vW)(CWnd::FromHandle((HWND)lParam));
		break;

	case AfxSig_vWww:
		(this->*mmf.pfn_vWww)(CWnd::FromHandle((HWND)wParam), LOWORD(lParam),
			HIWORD(lParam));
		break;

	case AfxSig_vWp:
	{
		CPoint point((DWORD)lParam);
		(this->*mmf.pfn_vWp)(CWnd::FromHandle((HWND)wParam), point);
	}
	break;

	case AfxSig_vWh:
		(this->*mmf.pfn_vWh)(CWnd::FromHandle((HWND)wParam),
			(HANDLE)lParam);
		break;

	case AfxSig_vwW:
		(this->*mmf.pfn_vwW)(wParam, CWnd::FromHandle((HWND)lParam));
		break;

	case AfxSig_vwWb:
		(this->*mmf.pfn_vwWb)((UINT)(LOWORD(wParam)),
			CWnd::FromHandle((HWND)lParam), (BOOL)HIWORD(wParam));
		break;

	case AfxSig_vwwW:
	case AfxSig_vwwx:
	{
		// special case for WM_VSCROLL and WM_HSCROLL
		ASSERT(message == WM_VSCROLL || message == WM_HSCROLL ||
			message == WM_VSCROLL + WM_REFLECT_BASE || message == WM_HSCROLL + WM_REFLECT_BASE);
		int nScrollCode = (short)LOWORD(wParam);
		int nPos = (short)HIWORD(wParam);
		if (lpEntry->nSig == AfxSig_vwwW)
			(this->*mmf.pfn_vwwW)(nScrollCode, nPos,
				CWnd::FromHandle((HWND)lParam));
		else
			(this->*mmf.pfn_vwwx)(nScrollCode, nPos);
	}
	break;

	case AfxSig_vs:
		(this->*mmf.pfn_vs)((LPTSTR)lParam);
		break;

	case AfxSig_vws:
		(this->*mmf.pfn_vws)((UINT)wParam, (LPCTSTR)lParam);
		break;

	case AfxSig_vOWNER:
		(this->*mmf.pfn_vOWNER)((int)wParam, (LPTSTR)lParam);
		lResult = TRUE;
		break;

	case AfxSig_iis:
		lResult = (this->*mmf.pfn_iis)((int)wParam, (LPTSTR)lParam);
		break;

	case AfxSig_wp:
	{
		CPoint point((DWORD)lParam);
		lResult = (this->*mmf.pfn_wp)(point);
	}
	break;

	case AfxSig_wv: // AfxSig_bv, AfxSig_wv
		lResult = (this->*mmf.pfn_wv)();
		break;

	case AfxSig_vCALC:
		(this->*mmf.pfn_vCALC)((BOOL)wParam, (NCCALCSIZE_PARAMS*)lParam);
		break;

	case AfxSig_vPOS:
		(this->*mmf.pfn_vPOS)((WINDOWPOS*)lParam);
		break;

	case AfxSig_vwwh:
		(this->*mmf.pfn_vwwh)(LOWORD(wParam), HIWORD(wParam), (HANDLE)lParam);
		break;

	case AfxSig_vwp:
	{
		CPoint point((DWORD)lParam);
		(this->*mmf.pfn_vwp)(wParam, point);
		break;
	}
	case AfxSig_vwSIZING:
		(this->*mmf.pfn_vwl)(wParam, lParam);
		lResult = TRUE;
		break;

	case AfxSig_bwsp:
		lResult = (this->*mmf.pfn_bwsp)(LOWORD(wParam), (short)HIWORD(wParam),
			CPoint(LOWORD(lParam), HIWORD(lParam)));
		if (!lResult)
			return FALSE;
	}
	goto LReturnTrue;

LDispatchRegistered:    // for registered windows messages
	ASSERT(message >= 0xC000);
	mmf.pfn = lpEntry->pfn;
	lResult = (this->*mmf.pfn_lwl)(wParam, lParam);

LReturnTrue:
	if (pResult != NULL)
		*pResult = lResult;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTestCmdUI - used to test for disabled commands before dispatching

class CTestCmdUI : public CCmdUI
{
public:
	CTestCmdUI();

public: // re-implementations only
	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetRadio(BOOL bOn);
	virtual void SetText(LPCTSTR);

	BOOL m_bEnabled;
};

CTestCmdUI::CTestCmdUI()
{
	m_bEnabled = TRUE;  // assume it is enabled
}

void CTestCmdUI::Enable(BOOL bOn)
{
	m_bEnabled = bOn;
	m_bEnableChanged = TRUE;
}

void CTestCmdUI::SetCheck(int)
{
	// do nothing -- just want to know about calls to Enable
}

void CTestCmdUI::SetRadio(BOOL)
{
	// do nothing -- just want to know about calls to Enable
}

void CTestCmdUI::SetText(LPCTSTR)
{
	// do nothing -- just want to know about calls to Enable
}

/////////////////////////////////////////////////////////////////////////////
// CWnd command handling

BOOL CWnd::OnCommand(WPARAM wParam, LPARAM lParam)
// return TRUE if command invocation was attempted
{
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

	// default routing for command messages (through closure table)

	if (hWndCtrl == NULL)
	{
		// zero IDs for normal commands are not allowed
		if (nID == 0)
			return FALSE;

		// make sure command has not become disabled before routing
		CTestCmdUI state;
		state.m_nID = nID;
		OnCmdMsg(nID, CN_UPDATE_COMMAND_UI, &state, NULL);
		if (!state.m_bEnabled)
		{
			TRACE1("Warning: not executing disabled command %d\n", nID);
			return TRUE;
		}

		// menu or accelerator
		nCode = CN_COMMAND;
	}
	else
	{
		// control notification
		ASSERT(nID == 0 || ::IsWindow(hWndCtrl));

		if (_afxThreadState->m_hLockoutNotifyWindow == m_hWnd)
			return TRUE;        // locked out - ignore control notification

		// reflect notification to child window control
		if (ReflectLastMsg(hWndCtrl))
			return TRUE;    // eaten by child

		// zero IDs for normal commands are not allowed
		if (nID == 0)
			return FALSE;
	}

#ifdef _DEBUG
	if (nCode < 0 && nCode != (int)0x8000)
		TRACE1("Implementation Warning: control notification = $%X.\n",
			nCode);
#endif

	return OnCmdMsg(nID, nCode, NULL, NULL);
}

BOOL CWnd::OnNotify(WPARAM, LPARAM lParam, LRESULT* pResult)
{
	ASSERT(pResult != NULL);
	NMHDR* pNMHDR = (NMHDR*)lParam;
	HWND hWndCtrl = pNMHDR->hwndFrom;

	// get the child ID from the window itself
	UINT nID = _AfxGetDlgCtrlID(hWndCtrl);
	int nCode = pNMHDR->code;

	ASSERT(hWndCtrl != NULL);
	ASSERT(::IsWindow(hWndCtrl));

	if (_afxThreadState->m_hLockoutNotifyWindow == m_hWnd)
		return TRUE;        // locked out - ignore control notification

	// reflect notification to child window control
	if (ReflectLastMsg(hWndCtrl, pResult))
		return TRUE;        // eaten by child

	AFX_NOTIFY notify;
	notify.pResult = pResult;
	notify.pNMHDR = pNMHDR;
	return OnCmdMsg(nID, MAKELONG(nCode, WM_NOTIFY), &notify, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CWnd extensions

CFrameWnd* CWnd::GetParentFrame() const
{
	if (GetSafeHwnd() == NULL) // no Window attached
		return NULL;

	ASSERT_VALID(this);

	CWnd* pParentWnd = GetParent();  // start with one parent up
	while (pParentWnd != NULL)
	{
		if (pParentWnd->IsFrameWnd())
			return (CFrameWnd*)pParentWnd;
		pParentWnd = pParentWnd->GetParent();
	}
	return NULL;
}

HWND AFXAPI AfxGetParentOwner(HWND hWnd)
{
	// check for permanent-owned window first
	CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);
	if (pWnd != NULL)
		return pWnd->GetOwner()->GetSafeHwnd();

	// otherwise, return parent in the Windows sense
	return (::GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) ?
		::GetParent(hWnd) : ::GetWindow(hWnd, GW_OWNER);
}

CWnd* CWnd::GetTopLevelParent() const
{
	if (GetSafeHwnd() == NULL) // no Window attached
		return NULL;

	ASSERT_VALID(this);

	HWND hWndParent = m_hWnd;
	HWND hWndT;
	while ((hWndT = AfxGetParentOwner(hWndParent)) != NULL)
		hWndParent = hWndT;

	return CWnd::FromHandle(hWndParent);
}

CWnd* CWnd::GetTopLevelOwner() const
{
	if (GetSafeHwnd() == NULL) // no Window attached
		return NULL;

	ASSERT_VALID(this);

	HWND hWndOwner = m_hWnd;
	HWND hWndT;
	while ((hWndT = ::GetWindow(hWndOwner, GW_OWNER)) != NULL)
		hWndOwner = hWndT;

	return CWnd::FromHandle(hWndOwner);
}

CWnd* CWnd::GetParentOwner() const
{
	if (GetSafeHwnd() == NULL) // no Window attached
		return NULL;

	ASSERT_VALID(this);

	HWND hWndParent = m_hWnd;
	HWND hWndT;
	while ((::GetWindowLong(hWndParent, GWL_STYLE) & WS_CHILD) &&
		(hWndT = ::GetParent(hWndParent)) != NULL)
	{
		hWndParent = hWndT;
	}

	return CWnd::FromHandle(hWndParent);
}

BOOL CWnd::IsTopParentActive() const
{
	ASSERT(m_hWnd != NULL);
	ASSERT_VALID(this);

	return CWnd::GetForegroundWindow() ==
		GetTopLevelParent()->GetLastActivePopup();
}

void CWnd::ActivateTopParent()
{
	// special activate logic for floating toolbars and palettes
	CWnd* pTopLevel = GetTopLevelParent();
	CWnd* pActiveWnd = GetForegroundWindow();
	if (pActiveWnd == NULL ||
		!(pActiveWnd->m_hWnd == m_hWnd || ::IsChild(pActiveWnd->m_hWnd, m_hWnd)))
	{
		// clicking on floating frame when it does not have
		// focus itself -- activate the toplevel frame instead.
		pTopLevel->SetForegroundWindow();
	}
}

CFrameWnd* CWnd::GetTopLevelFrame() const
{
	if (GetSafeHwnd() == NULL) // no Window attached
		return NULL;

	ASSERT_VALID(this);

	CFrameWnd* pFrameWnd = (CFrameWnd*)this;
	if (!IsFrameWnd())
		pFrameWnd = GetParentFrame();

	if (pFrameWnd != NULL)
	{
		CFrameWnd* pTemp;
		while ((pTemp = pFrameWnd->GetParentFrame()) != NULL)
			pFrameWnd = pTemp;
	}
	return pFrameWnd;
}

CWnd* PASCAL CWnd::GetSafeOwner(CWnd* pParent, HWND* pWndTop)
{
	HWND hWnd = GetSafeOwner_(pParent->GetSafeHwnd(), pWndTop);
	return CWnd::FromHandle(hWnd);
}

int CWnd::MessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType)
{
	if (lpszCaption == NULL)
		lpszCaption = AfxGetAppName();
	int nResult = ::MessageBox(GetSafeHwnd(), lpszText, lpszCaption, nType);
	return nResult;
}

CWnd* PASCAL CWnd::GetDescendantWindow(HWND hWnd, int nID, BOOL bOnlyPerm)
{
	// GetDlgItem recursive (return first found)
	// breadth-first for 1 level, then depth-first for next level

	// use GetDlgItem since it is a fast USER function
	HWND hWndChild;
	CWnd* pWndChild;
	if ((hWndChild = ::GetDlgItem(hWnd, nID)) != NULL)
	{
		if (::GetTopWindow(hWndChild) != NULL)
		{
			// children with the same ID as their parent have priority
			pWndChild = GetDescendantWindow(hWndChild, nID, bOnlyPerm);
			if (pWndChild != NULL)
				return pWndChild;
		}
		// return temporary handle if allowed
		if (!bOnlyPerm)
			return CWnd::FromHandle(hWndChild);

		// return only permanent handle
		pWndChild = CWnd::FromHandlePermanent(hWndChild);
		if (pWndChild != NULL)
			return pWndChild;
	}

	// walk each child
	for (hWndChild = ::GetTopWindow(hWnd); hWndChild != NULL;
		hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
	{
		pWndChild = GetDescendantWindow(hWndChild, nID, bOnlyPerm);
		if (pWndChild != NULL)
			return pWndChild;
	}
	return NULL;    // not found
}

void PASCAL CWnd::SendMessageToDescendants(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam, BOOL bDeep, BOOL bOnlyPerm)
{
	// walk through HWNDs to avoid creating temporary CWnd objects
	// unless we need to call this function recursively
	for (HWND hWndChild = ::GetTopWindow(hWnd); hWndChild != NULL;
		hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
	{
		// if bOnlyPerm is TRUE, don't send to non-permanent windows
		if (bOnlyPerm)
		{
			CWnd* pWnd = CWnd::FromHandlePermanent(hWndChild);
			if (pWnd != NULL)
			{
				// call window proc directly since it is a C++ window
				AfxCallWndProc(pWnd, pWnd->m_hWnd, message, wParam, lParam);
			}
		}
		else
		{
			// send message with Windows SendMessage API
			::SendMessage(hWndChild, message, wParam, lParam);
		}
		if (bDeep && ::GetTopWindow(hWndChild) != NULL)
		{
			// send to child windows after parent
			SendMessageToDescendants(hWndChild, message, wParam, lParam,
				bDeep, bOnlyPerm);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Scroll bar helpers
//  hook for CWnd functions
//    only works for derived class (eg: CView) that override 'GetScrollBarCtrl'
// if the window doesn't have a _visible_ windows scrollbar - then
//   look for a sibling with the appropriate ID

CScrollBar* CWnd::GetScrollBarCtrl(int) const
{
	return NULL;        // no special scrollers supported
}

int CWnd::SetScrollPos(int nBar, int nPos, BOOL bRedraw)
{
	CScrollBar* pScrollBar;
	if ((pScrollBar = GetScrollBarCtrl(nBar)) != NULL)
		return pScrollBar->SetScrollPos(nPos, bRedraw);
	else
		return ::SetScrollPos(m_hWnd, nBar, nPos, bRedraw);
}

int CWnd::GetScrollPos(int nBar) const
{
	CScrollBar* pScrollBar;
	if ((pScrollBar = GetScrollBarCtrl(nBar)) != NULL)
		return pScrollBar->GetScrollPos();
	else
		return ::GetScrollPos(m_hWnd, nBar);
}

void CWnd::SetScrollRange(int nBar, int nMinPos, int nMaxPos, BOOL bRedraw)
{
	CScrollBar* pScrollBar;
	if ((pScrollBar = GetScrollBarCtrl(nBar)) != NULL)
		pScrollBar->SetScrollRange(nMinPos, nMaxPos, bRedraw);
	else
		::SetScrollRange(m_hWnd, nBar, nMinPos, nMaxPos, bRedraw);
}

void CWnd::GetScrollRange(int nBar, LPINT lpMinPos, LPINT lpMaxPos) const
{
	CScrollBar* pScrollBar;
	if ((pScrollBar = GetScrollBarCtrl(nBar)) != NULL)
		pScrollBar->GetScrollRange(lpMinPos, lpMaxPos);
	else
		::GetScrollRange(m_hWnd, nBar, lpMinPos, lpMaxPos);
}

// Turn on/off non-control scrollbars
//   for WS_?SCROLL scrollbars - show/hide them
//   for control scrollbar - enable/disable them
void CWnd::EnableScrollBarCtrl(int nBar, BOOL bEnable)
{
	CScrollBar* pScrollBar;
	if (nBar == SB_BOTH)
	{
		EnableScrollBarCtrl(SB_HORZ, bEnable);
		EnableScrollBarCtrl(SB_VERT, bEnable);
	}
	else if ((pScrollBar = GetScrollBarCtrl(nBar)) != NULL)
	{
		// control scrollbar - enable or disable
		pScrollBar->EnableWindow(bEnable);
	}
	else
	{
		// WS_?SCROLL scrollbar - show or hide
		ShowScrollBar(nBar, bEnable);
	}
}

BOOL CWnd::SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw)
{
	ASSERT(lpScrollInfo != NULL);
	if (afxData.nWinVer < 0x333)
		return FALSE;

	HWND hWnd = m_hWnd;
	CScrollBar* pScrollBar;
	if (nBar != SB_CTL && (pScrollBar = GetScrollBarCtrl(nBar)) != NULL)
	{
		hWnd = pScrollBar->m_hWnd;
		nBar = SB_CTL;
	}
	lpScrollInfo->cbSize = sizeof(*lpScrollInfo);
	::SetScrollInfo(hWnd, nBar, lpScrollInfo, bRedraw);
	return TRUE;
}

BOOL CWnd::GetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, UINT nMask)
{
	ASSERT(lpScrollInfo != NULL);
	if (afxData.nWinVer < 0x333)
		return FALSE;

	HWND hWnd = m_hWnd;
	CScrollBar* pScrollBar;
	if (nBar != SB_CTL && (pScrollBar = GetScrollBarCtrl(nBar)) != NULL)
	{
		hWnd = pScrol