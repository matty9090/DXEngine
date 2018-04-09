#pragma once

#include <D3DX10.h>

typedef D3DXVECTOR3 Colour;

struct Light {
	D3DXVECTOR3 pos;	// 12 bytes
	float power;		// 4  bytes
	Colour colour;		// 12 bytes
	float pad1;			// 4  bytes
};

struct SceneLighting {
	Light lights[2];	// 32 bytes
	Colour ambient;		// 12 bytes
	float pad;			// 4  bytes
};