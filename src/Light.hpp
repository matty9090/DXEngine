#pragma once

#include <D3DX10.h>

typedef D3DXVECTOR3 Colour;

struct Light {
	D3DXVECTOR3 pos;
	float power;
	Colour colour;
	float pad1;
};

struct SceneLighting {
	Light lights[2];
	Colour ambient;
	float pad;
};