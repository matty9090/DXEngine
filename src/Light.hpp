#pragma once

#include <D3DX10.h>

typedef D3DXVECTOR3 Colour;

struct PointLight {
	D3DXVECTOR3 pos;	// 12 bytes
	float power;		// 4  bytes
	Colour colour;		// 12 bytes
	float pad1;			// 4  bytes
};

struct SpotLight {
	D3DXVECTOR3 pos;	// 12 bytes
	float range;		// 4  bytes
	D3DXVECTOR3 dir;	// 12  bytes
	float cone;			// 4  bytes
	D3DXVECTOR3 att;	// 12  bytes
	float pad;			// 4  bytes
};

struct SceneLighting {
	PointLight  lights[8];	// 256 bytes
	SpotLight  slights[4];	// 256 bytes
	Colour ambient;			// 12  bytes
	size_t num;				// 4   bytes
};