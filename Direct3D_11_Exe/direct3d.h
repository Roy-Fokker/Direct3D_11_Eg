#pragma once

#include <Windows.h>
#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <atlbase.h>

namespace direct3d_11_eg
{
	class direct3d
	{
	public:
		using device = CComPtr<ID3D11Device>;
		using swap_chain = CComPtr<IDXGISwapChain>;
		using context = CComPtr<ID3D11DeviceContext>;

	public:
		direct3d() = delete;
		direct3d(HWND hWnd);
		~direct3d();

		void resize_swap_chain();
		void present(bool vSync = false);

		context get_context() const;
		swap_chain get_swap_chain() const;
		device get_device() const;

	private:
		void make_device();
		void make_swap_chain();

	private:
		device d3d_device;
		swap_chain d3d_swap_chain;
		context d3d_context;

		HWND window_handle;
	};

};