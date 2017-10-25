/*
* file TypeDefs.h
*
* author luoxw
* date 2017/09/15
*
*
*/

#pragma once

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

using namespace std;

typedef vector<uint8> FBuf;

// FBufFast用reserve/capacity,在容器的拷贝赋值过程中会丢失数据,慎用
typedef vector<uint8> FBufFast;