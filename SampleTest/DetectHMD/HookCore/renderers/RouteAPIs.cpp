/*
 * file RouteAPIs.cpp
 *
 * author luoxw
 * date 2016/11/10
 *
 * 
 */
#include "HookCorePCH.h"
#include "RouteAPIs.h"
#include "Direct3D11Renderer.h"
#include "mhook-lib/mhook.h"

using namespace lostvr;

RouteObject::RouteObject(PVOID entry) : OriginalEntry(nullptr)
, RouterEntry(entry)
{
	RouteModule::Get()->Register(this);
}

RouteObject::~RouteObject()
{
	OriginalEntry = nullptr;
	RouterEntry = nullptr;
}

bool RouteObject::InstallRoute()
{
	return InstallRoute_MhookImpl();
}

bool RouteObject::UninstallRoute()
{
	return UninstallRoute_MhookImpl();
}

bool RouteObject::InstallRoute_MhookImpl()
{
	const CHAR* head = "RouteObject::InstallRoute_MhookImpl";
	if (OriginalEntry == nullptr)
	{
		LVMSG(head, "null original entry");
		return false;
	}

	if (RouterEntry == nullptr)
	{
		LVMSG(head, "null router entry");
		return false;
	}

	if (Mhook_SetHook((PVOID*)&OriginalEntry, RouterEntry))
	{
		LVMSG(head, "installed %s", GetRouteString());
		return true;
	}
	else
	{
		LVMSG(head, "failed to install %s", GetRouteString());
		return false;
	}
}

bool RouteObject::UninstallRoute_MhookImpl()
{
	const CHAR* head = "RouteObject::UninstallRoute_MhookImpl";
	if (Mhook_Unhook((PVOID*)&OriginalEntry))
	{
		LVMSG(head, "uninstalled %s", GetRouteString());
		return true;
	}
	else
	{
		LVMSG(head, "failed to uninstall %s", GetRouteString());
		return false;
	}
}

RouteModule::RouteModule()
{
}

RouteModule::~RouteModule()
{
	for (auto router : RouteObjects)
	{
		if (router != nullptr)
		{
			delete router;
			router = nullptr;
		}
	}

	RouteObjects.clear();
}

void RouteModule::Register(RouteObject * obj)
{
	RouteObjects.push_back(obj);
}

bool RouteModule::InstallRouters()
{
	const CHAR* head = "RouteModule::InstallRouters";
	bool bEverSuccess = false;
	for (auto router : RouteObjects)
	{
		if (router != nullptr)
		{
			bool bSuccess = router->InstallRoute();
			bEverSuccess |= bSuccess;
			LVMSG(head, "install route %s %s", router->GetRouteString(), bSuccess ? "successfully" : "failed");
		}
	}

	return bEverSuccess;
}

void RouteModule::UninstallRouters()
{
	for (auto router : RouteObjects)
	{
		if (router != nullptr)
		{
			router->UninstallRoute();
		}
	}
}

MemberFunctionRouter::MemberFunctionRouter(PVOID entry, int vfoffset) : RouteObject(entry)
, Offset(vfoffset)
{
}

bool MemberFunctionRouter::InstallRoute()
{
	const CHAR* head = "MemberFunctionRouter::InstallRoute";
	void* target = GetThisObject();
	if (target == nullptr)
	{
		return false;
	}

	OriginalEntry = (void*)*((__int3264*)*(__int3264*)target + Offset);

	return InstallRoute_MhookImpl();
}

bool MemberFunctionRouter::UninstallRoute()
{
	return UninstallRoute_MhookImpl();
}
