#pragma once

#include <D3DX11.h>
#include <d3dx9math.h>

#include "Camera.hpp"
#include "Model.hpp"

struct Mirror {
	Model *model;
	Camera *cam;
	
	bool reflect;

	Vec2<size_t> res;
	D3D11_VIEWPORT viewport;

	ID3D11Texture2D *tex;
	ID3D11Texture2D *stencil;

	ID3D11RenderTargetView	 *target;
	ID3D11DepthStencilView   *depthStencil;
	ID3D11ShaderResourceView *map;
};