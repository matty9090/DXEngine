#pragma once

#include <D3DX10math.h>

struct Ray;

class AABB {
	public:
		AABB(D3DXVECTOR3 &p, D3DXVECTOR3 min = D3DXVECTOR3(-0.2f, -0.2f, -0.2f), D3DXVECTOR3 max = D3DXVECTOR3(0.2f, 0.2f, 0.2f)) : pos(p) {
			bounds[0] = min;
			bounds[1] = max;
		}

		void setBounds(D3DXVECTOR3 min, D3DXVECTOR3 max) { bounds[0] = min, bounds[1] = max; }
		bool intersects(Ray ray, float t0, float t1);

	private:
		D3DXVECTOR3 &pos;
		D3DXVECTOR3 bounds[2];
};