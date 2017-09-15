/*
* file UtilitiesHeader.h
*
* author luoxw
* date 2017/01/18
*
*
*/

#pragma once




//FORCEINLINE bool GetWndFromProcessID(DWORD pid, HWND hwnd[], UINT& nwnd)
//{
//	HWND temp = NULL;
//	nwnd = 0;
//	while ((temp = FindWindowEx(NULL, temp, NULL, NULL)) && sizeof(hwnd) > nwnd)
//	{
//		DWORD p;
//		GetWindowThreadProcessId(temp, &p);
//		if (pid == p)
//		{
//			hwnd[nwnd++] = temp;
//			//break;
//		}
//	}
//
//	HWND root = hwnd[nwnd - 1];
//	temp = root;
//	//nwnd = 1;
//
//	while ((temp = GetParent(temp)) != NULL)
//	{
//		DWORD p;
//		GetWindowThreadProcessId(temp, &p);
//		if (pid == p)
//		{
//			root = temp;
//		}
//		else
//		{
//			break;
//		}
//	}
//
//	LVMSG("GetWndFromProcessID", "last wnd: %x, root: %x", hwnd[nwnd - 1], root);
//	hwnd[0] = root;
//	nwnd = 1;
//	return true;
//}


