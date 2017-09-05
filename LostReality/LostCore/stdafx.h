// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>

/******************************************************
* Include UtilitiesHeader.h 
*/
#define MODULE_MSG_PREFIX "lostcore-log"
#define MODULE_WARN_PREFIX "lostcore-warn"
#define MODULE_ERR_PREFIX "lostcore-error"

#ifdef GET_MODULE
#undef GET_MODULE
#endif
#define GET_MODULE LostCore::GetModule_LostCore

#ifdef EXPORT_API
#undef EXPORT_API
#endif
#define EXPORT_API LOSTCORE_API

#ifdef GLOBAL_HANDLER_PTR
#undef GLOBAL_HANDLER_PTR
#endif
#define GLOBAL_HANDLER_PTR (LostCore::FGlobalHandler::Get())

#include "UtilitiesHeader.h"
/*******************************************************/

#include "LostCore.h"

#include "LostCore-D3D11.h"