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
		using device_t = CComPtr<ID3D11Device>;
		using swap_chain_t = CComPtr<IDXGISwapChain>;
		using context_t = CComPtr<ID3D11DeviceContext>;

		using render_target_view_t = CComPtr<ID3D11RenderTargetView>;
		using depth_stencil_view_t = CComPtr<ID3D11DepthStencilView>;
		using texture_2d_t = CComPtr<ID3D11Texture2D>;

		using blend_state_t = CComPtr<ID3D11BlendState>;
		using depth_stencil_state_t = CComPtr<ID3D11DepthStencilState>;
		using rasterizer_state_t = CComPtr<ID3D11RasterizerState>;
		using sampler_state_t = CComPtr<ID3D11SamplerState>;
	};

	class direct3d
	{
	public:
		direct3d() = delete;
		direct3d(HWND hWnd);
		~direct3d();

		void resize_swap_chain();
		void present(bool vSync = false);

		direct3d_types::context_t get_context() const;
		direct3d_types::swap_chain_t get_swap_chain() const;
		direct3d_types::device_t get_device() const;

	private:
		void make_device();
		void make_swap_chain();

	private:
		direct3d_types::device_t device;
		direct3d_types::swap_chain_t swap_chain;
		direct3d_types::context_t context;

		HWND window_handle;
	};

	class render_target
	{
	public:
		render_target() = delete;
		render_target(direct3d_types::device_t device, direct3d_types::swap_chain_t swap_chain);
		~render_target();

		void activate(direct3d_types::context_t context);
		void clear_views(direct3d_types::context_t context, const std::array<float, 4> &clear_color);

	private:
		void make_target_view(direct3d_types::device_t device, direct3d_types::swap_chain_t swap_chain);
		void make_stencil_view(direct3d_types::device_t device, const std::array<uint16_t, 2> &buffer_size);

	private:
		direct3d_types::render_target_view_t render_view;
		direct3d_types::texture_2d_t depth_buffer;
		direct3d_types::depth_stencil_view_t depth_view;
		D3D11_VIEWPORT viewport;
	};

	class pipeline_state
	{
	public:
		struct description
		{
			struct blend_d
			{
				D3D11_BLEND src;
				D3D11_BLEND src_alpha;
				D3D11_BLEND dst;
				D3D11_BLEND dst_alpha;
				D3D11_BLEND_OP op;
				D3D11_BLEND_OP op_alpha;
			} blend{};

			struct depth_stencil_d
			{
				bool depth_enable;
				bool write_enable;
			} depth_stencil{};

			struct rasterizer_d
			{
				D3D11_CULL_MODE cull_mode;
				D3D11_FILL_MODE fill_mode;
			} rasterizer{};

			struct sampler_d
			{
				D3D11_FILTER filter;
				D3D11_TEXTURE_ADDRESS_MODE texture_address_mode;
				uint32_t max_anisotropy;
			} sampler{};
		};

	public:
		pipeline_state() = delete;
		pipeline_state(direct3d_types::device_t device, const description &state_description);
		~pipeline_state();

		void activate(direct3d_types::context_t context);

	private:
		void make_blend_state(direct3d_types::device_t device, const description::blend_d &blend_desc);
		void make_depth_stencil_state(direct3d_types::device_t device, const description::depth_stencil_d &depth_stencil_desc);
		void make_rasterizer_state(direct3d_types::device_t device, const description::rasterizer_d &rasterizer_desc);
		void make_sampler_state(direct3d_types::device_t device, const description::sampler_d &sample_desc);

	private:
		direct3d_types::blend_state_t blend_state;
		direct3d_types::depth_stencil_state_t depth_stencil_state;
		direct3d_types::rasterizer_state_t rasterizer_state;
		direct3d_types::sampler_state_t sampler_state;
	};
};