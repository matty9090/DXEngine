#include "Sphere.hpp"

Sphere::Sphere() {

}

Sphere::~Sphere() {

}

bool Sphere::init(ID3D11Device *device, Shader *shader) {
	size_t size = 10;

	int v = 0, c = 0, off = 0;

	std::vector<Vertex> vertices;
	std::vector<unsigned long> indices;

	generateGrid(vertices, indices, Front, size, off);
	generateGrid(vertices, indices, Bottom, size, off);
	generateGrid(vertices, indices, Back, size, off);
	//generateGrid(vertices, indices, Left, size, off);
	//generateGrid(vertices, indices, Top, size, off);
	//generateGrid(vertices, indices, Right, size, off);

	for (size_t i = 0; i < vertices.size(); ++i)
		vertices[i].position = mapPointToSphere(vertices[i].position);

	D3DXMatrixIdentity(&m_WorldMatrix);
	D3DXMatrixTranslation(&m_MatrixMov, 0.0f, 0.0f, -8.0f);
	m_WorldMatrix = m_MatrixMov;

	return initData(device, vertices, indices);
}

D3DXVECTOR3 Sphere::mapPointToSphere(D3DXVECTOR3 p) {
	float x2 = p.x * p.x, y2 = p.y * p.y, z2 = p.z * p.z;

	return D3DXVECTOR3(p.x * sqrtf(1.0f - y2 * 0.5f - z2 * 0.5f + (y2 * z2) * 0.33333f),
					   p.y * sqrtf(1.0f - z2 * 0.5f - x2 * 0.5f + (z2 * x2) * 0.33333f),
					   p.z * sqrtf(1.0f - x2 * 0.5f - y2 * 0.5f + (x2 * y2) * 0.33333f));
}

void Sphere::generateGrid(std::vector<Vertex> &v_arr, std::vector<unsigned long> &i_arr, int face, size_t size, int &off) {
	float step = 1.0f / size;

	switch (face) {
		case Top: case Bottom:
			for (float y = 0; y < 1; y += step)
				for (float x = 0; x < 1; x += step)
					v_arr.push_back({ D3DXVECTOR3(x - 0.5f, (face == Top) ? 0.5f : -0.5f, y - 0.5f), D3DXVECTOR3(0, 0, 0), D3DXVECTOR4((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f, 1.0f) });
			
			break;

		case Right: case Left:
			for (float y = 0; y < 1; y += step)
				for (float x = 0; x < 1; x += step)
					v_arr.push_back({ D3DXVECTOR3((face == Right) ? -0.5f : 0.5f, x - 0.5f, y - 0.5f), D3DXVECTOR3(0, 0, 0), D3DXVECTOR4((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f, 1.0f) });
			
			break;

		case Front: case Back:
			for (float y = 1; y >= 0; y -= step)
				for (float x = 1; x >= 0; x -= step)
					v_arr.push_back({ D3DXVECTOR3(y - 0.5f - step, x - 0.5f - step, (face == Front) ? -0.5f : 0.5f), D3DXVECTOR3(0, 0, 0), D3DXVECTOR4((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f, 1.0f) });
			
			break;
	}

	for (size_t y = 0; y < size - 1; ++y) {
		if(y > 0)
			i_arr.push_back(y * size + off);
		
		for (size_t x = 0; x < size; ++x) {
			i_arr.push_back(y * size + x + off);
			i_arr.push_back(((y + 1) * size) + x + off);
		}

		if(y < size - 2)
			i_arr.push_back(((y + 1) * size) + (size - 1) + off);
	}

	m_VertexCount += v_arr.size();
	m_IndexCount += i_arr.size();

	off += v_arr.size();
}
