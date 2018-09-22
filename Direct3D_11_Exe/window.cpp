#include "window.h"

#include "window_implementation.inl"

using namespace direct3d_11_eg;


window::window(const size &window_size, std::wstring_view title, uint16_t res_icon_id)
{
	window_impl = std::make_unique<window_implementation>();

	DWORD window_style = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
		window_style_ex = WS_EX_OVERLAPPEDWINDOW;

	RECT window_rectangle{ 0,
							0,
							window_size.width,
							window_size.height };

	AdjustWindowRectEx(&window_rectangle, window_style, NULL, window_style_ex);

	window_impl->Create(nullptr,
						window_rectangle,
						title.data(),
						window_style,
						window_style_ex);

	if (res_icon_id)
	{
		auto icon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(res_icon_id));
		window_impl->SetIcon(icon);
	}

	window_impl->CenterWindow();
}

window::~window()
{}

void window::set_message_callback(message_type msg, const callback_method &callback)
{
	uint16_t message_index = static_cast<uint16_t>(msg);
	window_impl->callback_methods[message_index] = callback;
}

void window::show()
{
	window_impl->ShowWindow(SW_SHOWNORMAL);
	window_impl->SetFocus();
}

void window::process_messages()
{
	BOOL has_more_messages = TRUE;
	while (has_more_messages)
	{
		MSG msg{};

		// Parameter two here has to be nullptr, putting hWnd here will
		// not retrive WM_QUIT messages, as those are posted to the thread
		// and not the window
		has_more_messages = PeekMessage(&msg, nullptr, NULL, NULL, PM_REMOVE);
		if (msg.message == WM_QUIT)
		{
			return;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

HWND window::handle() const
{
	return window_impl->m_hWnd;
}

