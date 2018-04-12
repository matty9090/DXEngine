#pragma once

#include <D3DX10math.h>

class Primitive;

struct Ray {
	D3DXVECTOR3 origin;
	D3DXVECTOR3 direction;
	D3DXVECTOR4 inv_direction;

	int sign[3];

	Ray() {}

	Ray(D3DXVECTOR3 o, D3DXVECTOR3 d) {
		origin = o;
		direction = d;
		inv_direction = D3DXVECTOR4(1.0f / direction.x, 1.0f / direction.y, 1.0f / direction.z, 1.0f);

		sign[0] = (inv_direction.x < 0) ? 1 : 0;
		sign[1] = (inv_direction.y < 0) ? 1 : 0;
		sign[2] = (inv_direction.z < 0) ? 1 : 0;
	}
};

struct RayHit {
	Primitive *model;
	Ray ray;
};