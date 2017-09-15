/*
* file StringUtils.h
*
* author luoxw
* date 2017/09/15
*
*
*/

#pragma once

FORCEINLINE wstring UTF8ToWide(const string &utf8)
{
	if (utf8.length() == 0) {
		return wstring();
	}

	// compute the length of the buffer we'll need
	int count = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);

	if (count == 0) {
		return wstring();
	}

	// convert
	wchar_t* buf = new wchar_t[count];
	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, buf, count);
	wstring result(buf);
	delete[] buf;
	return result;
}

FORCEINLINE string WideToUTF8(const wstring &wide) {
	if (wide.length() == 0) {
		return string();
	}

	// compute the length of the buffer we'll need
	int count = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1,
		NULL, 0, NULL, NULL);
	if (count == 0) {
		return string();
	}

	// convert
	char *buf = new char[count];
	WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, buf, count,
		NULL, NULL);

	string result(buf);
	delete[] buf;
	return result;
}