#pragma once

#include <Windows.h>
#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <atlbase.h>
#include <array>
#include <vector>

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

		using vertex_shader_t = CComPtr<ID3D11VertexShader>;
		using pixel_shader_t = CComPtr<ID3D11PixelShader>;
		using input_layout_t = CComPtr<ID3D11InputLayout>;
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
		enum class blend_e
		{
			Opaque,
			Alpha,
			Additive,
			NonPremultipled
		};

		enum class depth_stencil_e
		{
			None,
			ReadWrite,
			ReadOnly
		};

		enum class rasterizer_e
		{
			CullNone,
			CullClockwise,
			CullAntiClockwise,
			Wireframe
		};

		enum class sampler_e
		{
			PointWrap,
			PointClamp,
			LinearWrap,
			LinearClamp,
			AnisotropicWrap,
			AnisotropicClamp
		};

		enum class input_layout_e
		{
			position,
			position_texcoord
		};

		struct description
		{
			blend_e blend;
			depth_stencil_e depth_stencil;
			rasterizer_e rasterizer;
			sampler_e sampler;

			input_layout_e input_layout;
			D3D11_PRIMITIVE_TOPOLOGY primitive_topology;
			std::vector<byte> &vertex_shader_file;
			std::vector<byte> &pixel_shader_file;
		};

	public:
		pipeline_state() = delete;
		pipeline_state(direct3d_types::device_t device, const description &state_description);
		~pipeline_state();

		void activate(direct3d_types::context_t context);

	private:
		void make_blend_state(direct3d_types::device_t device, blend_e blend);
		void make_depth_stencil_state(direct3d_types::device_t device, depth_stencil_e depth_stencil);
		void make_rasterizer_state(direct3d_types::device_t device, rasterizer_e rasterizer);
		void make_sampler_state(direct3d_types::device_t device, sampler_e sampler);

		void make_input_layout(direct3d_types::device_t device, input_layout_e input_layout, const std::vector<byte> &vso);
		void make_vertex_shader(direct3d_types::device_t device, const std::vector<byte> &vso);
		void make_pixel_shader(direct3d_types::device_t device, const std::vector<byte> &pso);

	private:
		direct3d_types::blend_state_t blend_state;
		direct3d_types::depth_stencil_state_t depth_stencil_state;
		direct3d_types::rasterizer_state_t rasterizer_state;
		direct3d_types::sampler_state_t sampler_state;

		direct3d_types::input_layout_t input_layout;
		D3D11_PRIMITIVE_TOPOLOGY primitive_topology;
		direct3d_types::vertex_shader_t vertex_shader;
		direct3d_types::pixel_shader_t pixel_shader;
	};
};