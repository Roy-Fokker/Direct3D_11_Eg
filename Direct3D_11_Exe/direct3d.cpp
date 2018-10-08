#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#ifdef _DEBUG
#pragma comment(lib, "dxguid.lib")
#endif // DEBUG


#include "direct3d.h"

#include <cassert>
#include <cstdint>
#include <array>
#include <vector>
#include <DirectXColors.h>

using namespace direct3d_11_eg;
using namespace direct3d_11_eg::direct3d_types;

namespace
{
	constexpr DXGI_FORMAT swap_chain_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	constexpr uint16_t msaa_quality_level = 4U;
	constexpr uint32_t max_anisotropy = 16U;

	template<uint16_t SIZE>
	using element_desc = std::array<D3D11_INPUT_ELEMENT_DESC, SIZE>;
	constexpr D3D11_INPUT_ELEMENT_DESC position = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	constexpr D3D11_INPUT_ELEMENT_DESC normal = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	constexpr D3D11_INPUT_ELEMENT_DESC texcoord = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	const std::array<uint16_t, 2> get_window_size(HWND window_handle)
	{
		RECT rect{};
		GetClientRect(window_handle, &rect);

		return {
			static_cast<uint16_t>(rect.right - rect.left),
			static_cast<uint16_t>(rect.bottom - rect.top)
		};
	}

	const std::array<uint16_t, 2> get_swap_chain_size(swap_chain_t swap_chain)
	{
		DXGI_SWAP_CHAIN_DESC sd{};
		auto hr = swap_chain->GetDesc(&sd);
		assert(hr == S_OK);

		return {
			static_cast<uint16_t>(sd.BufferDesc.Width),
			static_cast<uint16_t>(sd.BufferDesc.Height)
		};
	}

	const DXGI_RATIONAL get_refresh_rate(CComPtr<IDXGIAdapter> dxgiAdapter, HWND window_handle, bool vSync = true)
	{
		DXGI_RATIONAL refresh_rate{ 0, 1 };

		if (vSync)
		{
			HRESULT hr{};

			CComPtr<IDXGIOutput> adapter_output;
			hr = dxgiAdapter->EnumOutputs(0, &adapter_output);
			assert(hr == S_OK);

			uint32_t display_modes_count{ 0 };
			hr = adapter_output->GetDisplayModeList(swap_chain_format,
			                                        DXGI_ENUM_MODES_INTERLACED,
			                                        &display_modes_count,
			                                        nullptr);
			assert(hr == S_OK);


			std::vector<DXGI_MODE_DESC> display_modes(display_modes_count);
			hr = adapter_output->GetDisplayModeList(swap_chain_format,
			                                        DXGI_ENUM_MODES_INTERLACED,
			                                        &display_modes_count,
			                                        &display_modes[0]);
			assert(hr == S_OK);

			auto[width, height] = get_window_size(window_handle);
			
			for (auto &mode : display_modes)
			{
				if (mode.Width == width && mode.Height == height)
				{
					refresh_rate = mode.RefreshRate;
				}
			}
		}

		return refresh_rate;
	}

	const DXGI_SAMPLE_DESC get_msaa_level(device_t device)
	{
		DXGI_SAMPLE_DESC sd{ 1, 0 };

#ifdef _DEBUG
		return sd;
#endif

		UINT msaa_level{ 0 };

		auto hr = device->CheckMultisampleQualityLevels(swap_chain_format, msaa_quality_level, &msaa_level);
		assert(hr == S_OK);
		
		if (msaa_level > 0)
		{
			sd.Count = msaa_quality_level;
			sd.Quality = msaa_level - 1;
		}

		return sd;
	}

	template<uint16_t SIZE>
	input_layout_t get_input_layout(device_t device, const std::array<D3D11_INPUT_ELEMENT_DESC, SIZE> &elements, const std::vector<byte> &vso)
	{
		input_layout_t input_layout;
		auto hr = device->CreateInputLayout(elements.data(),
											static_cast<uint32_t>(elements.size()),
											vso.data(),
											static_cast<uint32_t>(vso.size()),
											&input_layout);
		assert(hr == S_OK);

		return input_layout;
	}
}

#pragma region "Device, Context and Swap Chain"

direct3d::direct3d(HWND hWnd) :
	window_handle(hWnd)
{
	make_device();
	make_swap_chain();
}

direct3d::~direct3d()
{}

void direct3d::resize_swap_chain()
{
	auto hr = swap_chain->ResizeBuffers(NULL, NULL, NULL, DXGI_FORMAT_UNKNOWN, NULL);
	assert(hr == S_OK);
}

void direct3d::present(bool vSync)
{
	swap_chain->Present((vSync ? TRUE : FALSE), NULL);
}

context_t direct3d::get_context() const
{
	return context;
}

swap_chain_t direct3d::get_swap_chain() const
{
	return swap_chain;
}

device_t direct3d::get_device() const
{
	return device;
}

void direct3d::make_device()
{
	uint32_t flags{};
	flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT; // Needed for Direct 2D;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	std::array<D3D_FEATURE_LEVEL, 1> feature_levels{
		D3D_FEATURE_LEVEL_11_0
	};

	auto hr = D3D11CreateDevice(nullptr,
								D3D_DRIVER_TYPE_HARDWARE,
								nullptr,
								flags,
								feature_levels.data(),
								static_cast<uint32_t>(feature_levels.size()),
								D3D11_SDK_VERSION,
								&device,
								nullptr,
								&context);
	assert(hr == S_OK);
}

void direct3d::make_swap_chain()
{
	HRESULT hr{};

	CComPtr<IDXGIDevice> dxgi_device{};
	hr = device->QueryInterface<IDXGIDevice>(&dxgi_device);
	assert(hr == S_OK);

	CComPtr<IDXGIAdapter> dxgi_adapter{};
	hr = dxgi_device->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void **>(&dxgi_adapter));
	assert(hr == S_OK);

	CComPtr<IDXGIFactory> dxgi_factory{};
	hr = dxgi_adapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void **>(&dxgi_factory));
	assert(hr == S_OK);

	
	auto [width, height] = get_window_size(window_handle);

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = swap_chain_format;
	sd.BufferDesc.RefreshRate = get_refresh_rate(dxgi_adapter, window_handle);
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = window_handle;
	sd.SampleDesc = get_msaa_level(device);
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.Windowed = TRUE;

	hr = dxgi_factory->CreateSwapChain(device, &sd, &swap_chain);
	assert(hr == S_OK);

	dxgi_factory->MakeWindowAssociation(window_handle, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
}

#pragma endregion

#pragma region "Render Target"

render_target::render_target(direct3d_types::device_t device, direct3d_types::swap_chain_t swap_chain)
{
	auto [width, height] = get_swap_chain_size(swap_chain);

	make_target_view(device, swap_chain);
	make_stencil_view(device, {width, height});

	viewport = {};
	viewport.Width = width;
	viewport.Height = height;
	viewport.MaxDepth = 1.0f;
}

render_target::~render_target()
{}

void render_target::activate(context_t context)
{
	context->OMSetRenderTargets(1, &render_view.p, depth_view);
	context->RSSetViewports(1, &viewport);
}

void render_target::clear_views(context_t context, const std::array<float, 4> &clear_color)
{
	context->ClearRenderTargetView(render_view, &clear_color[0]);
	context->ClearDepthStencilView(depth_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void render_target::make_target_view(device_t device, swap_chain_t swap_chain)
{
	texture_2d_t buffer = nullptr;
	auto hr = swap_chain->GetBuffer(0,
	                                    __uuidof(ID3D11Texture2D),
	                                    reinterpret_cast<void **>(&buffer.p));
	assert(hr == S_OK);

	hr = device->CreateRenderTargetView(buffer,
	                                    0,
	                                    &render_view);
	assert(hr == S_OK);
}

void render_target::make_stencil_view(device_t device, const std::array<uint16_t, 2> &buffer_size)
{
	auto [width, height] = buffer_size;

	D3D11_TEXTURE2D_DESC td{};
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.SampleDesc = get_msaa_level(device);

	auto hr = device->CreateTexture2D(&td,
	                                      0,
	                                      &depth_buffer);
	assert(hr == S_OK);

	hr = device->CreateDepthStencilView(depth_buffer,
	                                        0,
	                                        &depth_view);
	assert(hr == S_OK);
}

#pragma endregion

#pragma region "Pipeline State"

pipeline_state::pipeline_state(device_t device, const description &state_description)
{
	make_blend_state(device, state_description.blend);
	make_depth_stencil_state(device, state_description.depth_stencil);
	make_rasterizer_state(device, state_description.rasterizer);
	make_sampler_state(device, state_description.sampler);

	make_input_layout(device, state_description.input_layout, state_description.vertex_shader_file);
	make_vertex_shader(device, state_description.vertex_shader_file);
	make_pixel_shader(device, state_description.pixel_shader_file);
}

pipeline_state::~pipeline_state()
{}

void pipeline_state::activate(context_t context)
{
	context->OMSetBlendState(blend_state,
	                         DirectX::Colors::Transparent,
	                         0xffff'ffff);
	context->OMSetDepthStencilState(depth_stencil_state,
	                                NULL);
	context->RSSetState(rasterizer_state);
	context->PSSetSamplers(0,
	                       1,
	                       &sampler_state.p);


	context->IASetPrimitiveTopology(primitive_topology);
	context->IASetInputLayout(input_layout);

	context->VSSetShader(vertex_shader, nullptr, 0);
	context->PSSetShader(pixel_shader, nullptr, 0);
}

void pipeline_state::make_blend_state(device_t device, blend_e blend)
{
	D3D11_BLEND src, dst;
	D3D11_BLEND_OP op{ D3D11_BLEND_OP_ADD };

	switch (blend)
	{
		case blend_e::Opaque:
			src = D3D11_BLEND_ONE;
			dst = D3D11_BLEND_ZERO;
			break;
		case blend_e::Alpha:
			src = D3D11_BLEND_ONE;
			dst = D3D11_BLEND_INV_SRC_ALPHA;
			break;
		case blend_e::Additive:
			src = D3D11_BLEND_SRC_ALPHA;
			dst = D3D11_BLEND_ONE;
			break;
		case blend_e::NonPremultipled:
			src = D3D11_BLEND_SRC_ALPHA;
			dst = D3D11_BLEND_INV_SRC_ALPHA;
			break;
	}

	D3D11_BLEND_DESC bd{};

	bd.RenderTarget[0].BlendEnable = ((src != D3D11_BLEND_ONE) || (dst != D3D11_BLEND_ONE));

	bd.RenderTarget[0].SrcBlend = src;
	bd.RenderTarget[0].BlendOp = op;
	bd.RenderTarget[0].DestBlend = dst;

	bd.RenderTarget[0].SrcBlendAlpha = src;
	bd.RenderTarget[0].BlendOpAlpha = op;
	bd.RenderTarget[0].DestBlendAlpha = dst;

	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	auto hr = device->CreateBlendState(&bd, &blend_state);
	assert(hr == S_OK);
}

void pipeline_state::make_depth_stencil_state(device_t device, depth_stencil_e depth_stencil)
{
	bool depth_enable, write_enable;

	switch (depth_stencil)
	{
		case depth_stencil_e::None:
			depth_enable = false;
			write_enable = false;
			break;
		case depth_stencil_e::ReadWrite:
			depth_enable = true;
			write_enable = true;
			break;
		case depth_stencil_e::ReadOnly:
			depth_enable = true;
			write_enable = false;
			break;
	}

	D3D11_DEPTH_STENCIL_DESC dsd{};

	dsd.DepthEnable = depth_enable;
	dsd.DepthWriteMask = write_enable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	dsd.StencilEnable = false;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	dsd.BackFace = dsd.FrontFace;

	auto hr = device->CreateDepthStencilState(&dsd, &depth_stencil_state);
	assert(hr == S_OK);
}

void pipeline_state::make_rasterizer_state(device_t device, rasterizer_e rasterizer)
{
	D3D11_CULL_MODE cull_mode;
	D3D11_FILL_MODE fill_mode;

	switch (rasterizer)
	{
		case rasterizer_e::CullNone:
			cull_mode = D3D11_CULL_NONE;
			fill_mode = D3D11_FILL_SOLID;
			break;
		case rasterizer_e::CullClockwise:
			cull_mode = D3D11_CULL_FRONT;
			fill_mode = D3D11_FILL_SOLID;
			break;
		case rasterizer_e::CullAntiClockwise:
			cull_mode = D3D11_CULL_BACK;
			fill_mode = D3D11_FILL_SOLID;
			break;
		case rasterizer_e::Wireframe:
			cull_mode = D3D11_CULL_BACK;
			fill_mode = D3D11_FILL_WIREFRAME;
			break;
	}

	D3D11_RASTERIZER_DESC rd{};

	rd.CullMode = cull_mode;
	rd.FillMode = fill_mode;
	rd.DepthClipEnable = true;
	rd.MultisampleEnable = true;

	
	auto hr = device->CreateRasterizerState(&rd, &rasterizer_state);
	assert(hr == S_OK);
}

void pipeline_state::make_sampler_state(device_t device, sampler_e sampler)
{
	D3D11_FILTER filter;
	D3D11_TEXTURE_ADDRESS_MODE texture_address_mode;

	switch (sampler)
	{
		case sampler_e::PointWrap:
			filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			texture_address_mode = D3D11_TEXTURE_ADDRESS_WRAP;
			break;
		case sampler_e::PointClamp:
			filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			texture_address_mode = D3D11_TEXTURE_ADDRESS_CLAMP;
			break;
		case sampler_e::LinearWrap:
			filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			texture_address_mode = D3D11_TEXTURE_ADDRESS_WRAP;
			break;
		case sampler_e::LinearClamp:
			filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			texture_address_mode = D3D11_TEXTURE_ADDRESS_CLAMP;
			break;
		case sampler_e::AnisotropicWrap:
			filter = D3D11_FILTER_ANISOTROPIC;
			texture_address_mode = D3D11_TEXTURE_ADDRESS_WRAP;
			break;
		case sampler_e::AnisotropicClamp:
			filter = D3D11_FILTER_ANISOTROPIC;
			texture_address_mode = D3D11_TEXTURE_ADDRESS_CLAMP;
			break;
	}

	D3D11_SAMPLER_DESC sd{ };

	sd.Filter = filter;

	sd.AddressU = texture_address_mode;
	sd.AddressV = texture_address_mode;
	sd.AddressW = texture_address_mode;

	sd.MaxAnisotropy = max_anisotropy;

	sd.MaxLOD = FLT_MAX;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;

	auto hr = device->CreateSamplerState(&sd, &sampler_state);
	assert(hr == S_OK);
}

void pipeline_state::make_input_layout(device_t device, input_layout_e layout, const std::vector<byte> &vso)
{
	/* TODO:
	I don't like how this works. 
	Ideally, i want to return just the 'element_desc' from the 'switch'
	and pass that to layout maker outside of 'switch'.
	*/
	switch (layout)
	{
		case input_layout_e::position:
			input_layout = get_input_layout(device, 
											element_desc<1>{ position }, 
											vso);
			break;
		case input_layout_e::position_texcoord:
			input_layout = get_input_layout(device, 
											element_desc<2>{ position, texcoord }, 
											vso);
			break;
	}
}

void pipeline_state::make_vertex_shader(device_t device, const std::vector<byte> &vso)
{
	auto hr = device->CreateVertexShader(vso.data(),
										 vso.size(),
										 NULL,
										 &vertex_shader);
	assert(hr == S_OK);
}

void pipeline_state::make_pixel_shader(device_t device, const std::vector<byte> &pso)
{
	auto hr = device->CreatePixelShader(pso.data(),
										pso.size(),
										NULL,
										&pixel_shader);
	assert(hr == S_OK);
}

#pragma endregion
