#pragma once

namespace Moongoose {
	class ImGuiManager
	{

	public:

		void startup();
		void shutdown();
		void showDockSpace(bool* p_open);

	private:

		static void setStyle();

	};
}

