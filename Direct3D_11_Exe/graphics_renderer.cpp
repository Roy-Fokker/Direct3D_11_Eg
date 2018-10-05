#include "graphics_renderer.h"


using namespace direct3d_11_eg;

graphics_renderer::graphics_renderer(HWND hWnd)
{
	d3d = std::make_unique<direct3d>(hWnd);
}

graphics_renderer::~graphics_renderer()
{}

void graphics_renderer::draw_frame()
{
	d3d->present();
}

void graphics_renderer::resize_frame()
{
	d3d->resize_swap_chain();
}
