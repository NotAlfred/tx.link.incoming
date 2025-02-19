/*
 *  Copyright (C) 2010 Alexander Chernykh 
 *
 *  This file is part of MyQuik: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MyQuik is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *  dlg.cpp
 */


#include "stdafx.h"
#include "dlg.h"
#include "MyQuik.h"

namespace my {
		
	HHOOK DialogMessageHook::m_hHook = NULL;
	THWNDCollection DialogMessageHook::m_aWindows;
	//
	// Note that windows are enumerated in top-down Z-order, so the menu 
	// window should always be the first one found.
	//   taken from code written by by Paul DiLascia,
	//   C++ Q&A, MSDN Magazine, November 2003
	//
	static BOOL CALLBACK MyEnumProc(HWND hwnd, LPARAM lParam)
	{
		TCHAR buf[16];
		::GetClassName(hwnd, buf, sizeof(buf) / sizeof(TCHAR));
		if (0 == _tcsncmp(buf, _T("#32768"), 6) == 0) {// special classname for menus
			*((HWND*)lParam) = hwnd;
			return FALSE;
		}
		return TRUE;
	}

	// Hook procedure for WH_GETMESSAGE hook type.
	//
	// This function is more or less a combination of MSDN KB articles
	// Q187988 and Q216503. See MSDN for additional details
	//
	LRESULT CALLBACK DialogMessageHook::GetMessageProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		// If this is a keystrokes message, pass it to IsDialogMessage for tab and accelerator processing
		LPMSG lpMsg = (LPMSG) lParam;

		// check if there is a menu active
		HWND hMenuWnd = NULL;
#if 0
		EnumWindows(MyEnumProc, (LPARAM)&hMenuWnd);
#endif
		if (hMenuWnd == NULL &&
			(nCode >= 0) &&
			PM_REMOVE == wParam &&
			(lpMsg->message >= WM_KEYFIRST && lpMsg->message <= WM_KEYLAST))
		{
			HWND hWnd, hActiveWindow = GetActiveWindow();
			THWNDCollection::iterator it = m_aWindows.begin();

			// check each window we manage to see if the message is meant for them
			while (it != m_aWindows.end())
			{
				hWnd = *it;

				if (::IsWindow(hWnd) && 
					::IsDialogMessage(hWnd, lpMsg))
				{
					// The value returned from this hookproc is ignored, and it cannot
					// be used to tell Windows the message has been handled. To avoid
					// further processing, convert the message to WM_NULL before returning.
					lpMsg->hwnd = NULL;
					lpMsg->message = WM_NULL;
					lpMsg->lParam = 0L;
					lpMsg->wParam = 0;

					break;
				}

				it++;
			}
		}
		// Passes the hook information to the next hook procedure in the current hook chain.
		return ::CallNextHookEx(m_hHook, nCode, wParam, lParam);
	}

	HRESULT DialogMessageHook::InstallHook(HWND hWnd)
	{
		// make sure the hook is installed
		if (m_hHook == NULL)
		{
			m_hHook = ::SetWindowsHookEx(WH_GETMESSAGE, GetMessageProc, _Module.m_hInst, ::GetCurrentThreadId());

			// is the hook set?
			if (m_hHook == NULL)
			{
				return E_UNEXPECTED;
			}
		}
		// add the window to our list of managed windows
		if (m_aWindows.find(hWnd) == m_aWindows.end())
			m_aWindows.insert(hWnd);

		return S_OK;
	}

	HRESULT DialogMessageHook::UninstallHook(HWND hWnd)
	{
		HRESULT hr = S_OK;

		// was the window found?
		if (m_aWindows.erase(hWnd) == 0)
			return E_INVALIDARG;

		// is this the last window? if so, then uninstall the hook
		if (m_aWindows.size() == 0 && m_hHook)
		{
			if (!::UnhookWindowsHookEx(m_hHook))
				hr = HRESULT_FROM_WIN32(::GetLastError());

			m_hHook = NULL;
		}

		return hr;
	}

} // namespace my

