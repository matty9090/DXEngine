#pragma once

#include "Primitive.hpp"

#include <D3dx9shape.h>
#include <vector>

class Sphere : public Primitive {
	public:
		Sphere();
		~Sphere();

		bool init(ID3D11Device *device, Shader *shader);

	private:
		enum Faces { Top, Bottom, Left, Right, Front, Back };
		
		D3DXVECTOR3 mapPointToSphere(D3DXVECTOR3 point);
		void generateGrid(std::vector<Vertex> &v_arr, std::vector<unsigned long> &i_arr, int face, size_t size, int &off);
};
