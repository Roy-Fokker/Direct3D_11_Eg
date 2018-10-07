#pragma once

#include <Windows.h>
#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <atlbase.h>
#include <array>

namespace direct3d_11_eg
{
	namespace direct3d_types
	{
		using device = CComPtr<ID3D11Device>;
		using swap_chain = CComPtr<IDXGISwapChain>;
		using context = CComPtr<ID3D11DeviceContext>;

		using render_target_view = CComPtr<ID3D11RenderTargetView>;
		using depth_stencil_view = CComPtr<ID3D11DepthStencilView>;
		using texture_2d = CComPtr<ID3D11Texture2D>;
	};

	class direct3d
	{
	public:
		direct3d() = delete;
		direct3d(HWND hWnd);
		~direct3d();

		void resize_swap_chain();
		void present(bool vSync = false);

		direct3d_types::context get_context() const;
		direct3d_types::swap_chain get_swap_chain() const;
		direct3d_types::device get_device() const;

	private:
		void make_device();
		void make_swap_chain();

	private:
		direct3d_types::device d3d_device;
		direct3d_types::swap_chain d3d_swap_chain;
		direct3d_types::context d3d_context;

		HWND window_handle;
	};

	class render_target
	{
	public:
		render_target() = delete;
		render_target(direct3d_types::device d3d_device, direct3d_types::swap_chain d3d_swap_chain);
		~render_target();

		void activate(direct3d_types::context d3d_context);
		void clear_views(direct3d_types::context d3d_context, const std::array<float, 4> &clear_color);

	private:
		void make_target_view(direct3d_types::device d3d_device, direct3d_types::swap_chain d3d_swap_chain);
		void make_stencil_view(direct3d_types::device d3d_device, const std::array<uint16_t, 2> &buffer_size);

	private:
		direct3d_types::render_target_view d3d_render_target_view;
		direct3d_types::texture_2d d3d_depth_stencil_buffer;
		direct3d_types::depth_stencil_view d3d_depth_stencil_view;
		D3D11_VIEWPORT viewport;
	};
};