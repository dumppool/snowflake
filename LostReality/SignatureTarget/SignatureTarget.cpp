// SignatureTarget.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>

#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

void DisplayErrorBox(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and clean up

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

DWORD SearchDirectory(const TCHAR* szDir, wofstream& fout)
{
	WIN32_FIND_DATA ffd;
	DWORD dwError = 0;
	TCHAR searchTarget[MAX_PATH];
	StringCchCopy(searchTarget, MAX_PATH, szDir);
	StringCchCat(searchTarget, MAX_PATH, TEXT("\\*"));
	HANDLE hFind = FindFirstFile(searchTarget, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		DisplayErrorBox(TEXT("FindFirstFile"));
		return dwError;
	}

	// List all the files in the directory with some info about them.

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wcscmp(ffd.cFileName, TEXT(".")) != 0 && wcscmp(ffd.cFileName, TEXT("..")) != 0)
			{
				TCHAR sub[MAX_PATH];
				StringCchCopy(sub, MAX_PATH, szDir);
				StringCchCat(sub, MAX_PATH, TEXT("\\"));
				StringCchCat(sub, MAX_PATH, ffd.cFileName);
				SearchDirectory(sub, fout);
			}
		}
		else
		{
			wstring fileName(ffd.cFileName);
			size_t p;
			if ((p = fileName.rfind('.')) != wstring::npos)
			{
				wstring fileExt(fileName.begin() + p + 1, fileName.end());
				if (fileExt.compare(TEXT("dll")) == 0 ||
					fileExt.compare(TEXT("exe")) == 0)
				{
					wstring rdir(szDir);
					size_t rpos = rdir.find(TEXT("\\\\"));
					if (rpos != wstring::npos)
					{
						rdir = wstring(rdir.begin() + rpos + 1, rdir.end());
					}

					fout << setw(80) << rdir << TEXT("\t\t") << ffd.cFileName << endl;
				}
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		DisplayErrorBox(TEXT("FindFirstFile"));
	}

	FindClose(hFind);
	return 0;
}


int main(int argc, TCHAR *argv[])
{
	TCHAR szDir[MAX_PATH];
	HANDLE hFind = INVALID_HANDLE_VALUE;

	_tprintf(TEXT("\nTarget directory is %s\n\n"), argv[1]);

	WCHAR buf[MAX_PATH];
	GetModuleFileName(NULL, buf, MAX_PATH);
	wstring root = buf;
	root.resize(root.rfind('\\') + 1);

	wofstream file;
	file.open((root + TEXT("list.txt")), ios::out);

	StringCchCopy(szDir, MAX_PATH, root.c_str());

	DWORD dwError = SearchDirectory(szDir, file);

	file.close();
	return dwError;
}

