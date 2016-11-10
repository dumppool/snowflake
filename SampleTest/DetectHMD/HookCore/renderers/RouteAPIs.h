/*
 * file RouteAPIs.h
 *
 * author luoxw
 * date 2016/11/10
 *
 * 
 */
#pragma once

namespace lostvr
{
	class RouteObject
	{
	public:
		explicit RouteObject(PVOID entry);
		virtual ~RouteObject();

		virtual bool InstallRoute();
		virtual bool UninstallRoute();
		virtual const CHAR* GetRouteString() const = 0;

		PVOID GetOriginalEntry()
		{
			return OriginalEntry;
		}

	protected:
		bool InstallRoute_MhookImpl();
		bool UninstallRoute_MhookImpl();

	protected:
		PVOID OriginalEntry;
		PVOID RouterEntry;
	};

	class MemberFunctionRouter : public RouteObject
	{
		
	public:
		explicit MemberFunctionRouter(PVOID router, int vfoffset);

		virtual bool InstallRoute() override;
		virtual bool UninstallRoute() override;

		virtual PVOID GetThisObject() = 0;

	protected:
		int Offset;
	};

	class RouteModule
	{
		std::vector<RouteObject*> RouteObjects;

	public:
		static RouteModule* Get()
		{
			static RouteModule Inst;
			return &Inst;
		}

		RouteModule();
		~RouteModule();

		void Register(RouteObject* obj);
		
		bool InstallRouters(); 
		void UninstallRouters();
	};
}