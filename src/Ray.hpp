#pragma once

#include <D3DX10math.h>

#include "Model.hpp"

struct Ray {
	D3DXVECTOR3 pos;
	D3DXVECTOR3 dir;
};

struct RayHit {
	Model *model;
};