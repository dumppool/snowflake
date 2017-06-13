// LostReality.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <Windows.h>

#include <vector>
#include <array>
using namespace std;

int main()
{
	int nativeArray[100];
	printf("\n native array nativeArray[100] size: %d", sizeof(nativeArray));

	vector<int> vector0;
	vector0.resize(100);
	printf("\n std vector vector<int> resize(100) size: %d", sizeof(vector0));

	array<int, 10> array0;
	printf("\n std array array<int, 10> size: %d", sizeof(array0));

	int index = 0;
	for_each(array0.begin(), array0.end(), [&](int& val) {val = index++; });
	printf("\n array: ");
	for_each(array0.begin(), array0.end(), [](int val) {printf("%d, ", val); });

	printf("\n");
	system("pause");
    return 0;
}

