#pragma once

namespace lostvr {

	class MainApp
	{
	public:
		MainApp();

		bool Init(HINSTANCE hinst, LPWSTR title);
		void Run();

		bool bInitialized;
	};
};