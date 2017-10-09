#pragma once

#include "targetver.h"

#define MODULE_MSG_PREFIX "converter-log"
#define MODULE_WARN_PREFIX "converter-warn"
#define MODULE_ERR_PREFIX "converter-error"
#include "LostCoreIncludes.h"

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <tchar.h>

using FMatrixNodeAlias = LostCore::FMatrixNode;
using FPoseTreeAlias = LostCore::FPoseTree;
using FPoseMapAlias = LostCore::FPoseMap;
using FMeshDataAlias = LostCore::FMeshData;
using FRealCurveAlias = LostCore::FRealCurve;
using FMatrixCurveAlias = LostCore::FMatrixCurve;

// sdk includes
#include <fbxsdk.h>

#include "samples/Common/Common.h"

#include "Importer.h"
