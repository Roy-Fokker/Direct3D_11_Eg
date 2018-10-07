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

using namespace direct3d_11_eg;
using namespace direct3d_11_eg::direct3d_types;

namespace
{
	constexpr DXGI_FORMAT swap_chain_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	constexpr uint16_t msaa_quality_level = 4U;

	const std::array<uint16_t, 2> get_window_size(HWND window_handle)
	{
		RECT rect{};
		GetClientRect(window_handle, &rect);

		return {
			static_cast<uint16_t>(rect.right - rect.left),
			static_cast<uint16_t>(rect.bottom - rect.top)
		};
	}

	const std::array<uint16_t, 2> get_swap_chain_size(swap_chain d3d_swap_chain)
	{
		DXGI_SWAP_CHAIN_DESC sd{};
		auto hr = d3d_swap_chain->GetDesc(&sd);
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

	const DXGI_SAMPLE_DESC get_msaa_level(device device)
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
}

#pragma region "Direct 3D - Device, Context and Swap Chain"

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
	auto hr = d3d_swap_chain->ResizeBuffers(NULL, NULL, NULL, DXGI_FORMAT_UNKNOWN, NULL);
	assert(hr == S_OK);
}

void direct3d::present(bool vSync)
{
	d3d_swap_chain->Present((vSync ? TRUE : FALSE), NULL);
}

context direct3d::get_context() const
{
	return d3d_context;
}

swap_chain direct3d::get_swap_chain() const
{
	return d3d_swap_chain;
}

device direct3d::get_device() const
{
	return d3d_device;
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
								&d3d_device,
								nullptr,
								&d3d_context);
	assert(hr == S_OK);
}

void direct3d::make_swap_chain()
{
	HRESULT hr{};

	CComPtr<IDXGIDevice> dxgi_device{};
	hr = d3d_device->QueryInterface<IDXGIDevice>(&dxgi_device);
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
	sd.SampleDesc = get_msaa_level(d3d_device);
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.Windowed = TRUE;

	hr = dxgi_factory->CreateSwapChain(d3d_device, &sd, &d3d_swap_chain);
	assert(hr == S_OK);

	dxgi_factory->MakeWindowAssociation(window_handle, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
}

#pragma endregion

#pragma region "Direct 3D - Render Target"

render_target::render_target(direct3d_types::device d3d_device, direct3d_types::swap_chain d3d_swap_chain)
{
	auto [width, height] = get_swap_chain_size(d3d_swap_chain);

	make_target_view(d3d_device, d3d_swap_chain);
	make_stencil_view(d3d_device, {width, height});

	viewport = {};
	viewport.Width = width;
	viewport.Height = height;
	viewport.MaxDepth = 1.0f;
}

render_target::~render_target()
{}

void render_target::activate(direct3d_types::context d3d_context)
{
	d3d_context->OMSetRenderTargets(1, &d3d_render_target_view.p, d3d_depth_stencil_view);
	d3d_context->RSSetViewports(1, &viewport);
}

void render_target::clear_views(context d3d_context, const std::array<float, 4> &clear_color)
{
	d3d_context->ClearRenderTargetView(d3d_render_target_view, &clear_color[0]);
	d3d_context->ClearDepthStencilView(d3d_depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void render_target::make_target_view(device d3d_device, swap_chain d3d_swap_chain)
{
	texture_2d buffer = nullptr;
	auto hr = d3d_swap_chain->GetBuffer(0,
	                                    __uuidof(ID3D11Texture2D),
	                                    reinterpret_cast<void **>(&buffer.p));
	assert(hr == S_OK);

	hr = d3d_device->CreateRenderTargetView(buffer,
	                                        0,
	                                        &d3d_render_target_view);
	assert(hr == S_OK);
}

void render_target::make_stencil_view(device d3d_device, const std::array<uint16_t, 2> &buffer_size)
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
	td.SampleDesc = get_msaa_level(d3d_device);

	auto hr = d3d_device->CreateTexture2D(&td,
	                                      0,
	                                      &d3d_depth_stencil_buffer);
	assert(hr == S_OK);

	hr = d3d_device->CreateDepthStencilView(d3d_depth_stencil_buffer,
	                                        0,
	                                        &d3d_depth_stencil_view);
	assert(hr == S_OK);
}

#pragma endregion