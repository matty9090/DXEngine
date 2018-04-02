#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <vector>

#include "Vec3.hpp"
#include "Shader.hpp"

class Primitive {
	public:
		Primitive(ID3D11Device *device, DXShader shader);
		~Primitive();

		void render(ID3D11DeviceContext *deviceContext, D3DXMATRIX &viewMatrix, D3DXMATRIX &projMatrix, D3DXVECTOR3 &camPos, D3DXVECTOR3 &lightPos, D3DXVECTOR3 &lightCol, D3DXVECTOR3 &ambientColour) const;
		void cleanup();

		void move(Vec3<float> &p);
		void rotate(Vec3<float> &r);
		void setPosition(Vec3<float> &pos);
		void setPosition(D3DXVECTOR3 &pos);
		void setColour(Vec3<float> &colour);
		void setScale(float scale);
		void setScale(Vec3<float> &scale);

		int getIndexCount() { return m_IndexCount; }
		int getVertexCount() { return m_VertexCount; }

		D3DMATRIX getWorldMatrix() { return m_WorldMatrix; }
		Shader *getShader() { return m_Shader; }
		ID3D11Device *getDevice() { return m_Device; }

	protected:
		ID3D11Buffer *m_VertexBuffer, *m_IndexBuffer;
		ID3D11Device *m_Device;

		D3DXMATRIX m_WorldMatrix, m_MatrixMov;
		D3DXMATRIX m_RotX, m_RotY, m_RotZ;
		D3DXMATRIX m_ScaleMatrix;

		D3D_PRIMITIVE_TOPOLOGY m_Topology;

		Vec3<float> m_Pos, m_Rot;
		Shader *m_Shader;

		unsigned int m_VertexSize;
		int m_VertexCount, m_IndexCount;

		struct Vertex {
			D3DXVECTOR3 position;
			D3DXVECTOR3 normal;
			D3DXVECTOR4 color;
		};

		std::vector<Vertex> m_Vertices;
		std::vector<WORD> m_Indices;

		virtual void init() = 0;
		void initMatrices();
		bool initData();
};

class Cube : public Primitive {
	public:
		Cube(ID3D11Device *device, DXShader shader) : Primitive(device, shader) {
			init();
			initData();
		}

	private:
		void init();
};
