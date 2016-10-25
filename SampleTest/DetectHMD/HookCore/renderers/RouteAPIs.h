#pragma once

namespace lostvr
{
	class RouteObject
	{
	public:
		explicit RouteObject(PVOID entry);
		virtual ~RouteObject();

		virtual bool InstallRoute() = 0;
		virtual void UninstallRoute() = 0;
		virtual const CHAR* GetRouteString() const = 0;

		PVOID GetOriginalEntry()
		{
			return OriginalEntry;
		}

	protected:
		bool InstallRoute_MhookImpl();
		void UninstallRoute_MhookImpl();

	protected:
		PVOID OriginalEntry;
		PVOID RouterEntry;
	};

	class MemberFunctionRouter : public RouteObject
	{
		
	public:
		explicit MemberFunctionRouter(PVOID router, int vfoffset);

		virtual bool InstallRoute() override;
		virtual void UninstallRoute() override;

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