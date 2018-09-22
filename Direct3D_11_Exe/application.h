#pragma once

#include "window.h"

#include <memory>


namespace direct3d_11_eg
{
	class application
	{
	public:
		application();
		~application();

		int run();

	private:
		bool keypress_callback(uintptr_t key_code, uintptr_t extension);

	private:
		bool exit_application = false;
		std::unique_ptr<window> app_window = nullptr;
	};

};