#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <vector>

#include "Vec3.hpp"
#include "Shader.hpp"
#include "AABB.hpp"

class Primitive {
	public:
		Primitive(ID3D11Device *device, ID3D11DeviceContext *context, DXShader shader);
		~Primitive();

		void render(ID3D11DeviceContext *deviceContext, D3DXMATRIX &viewMatrix, D3DXMATRIX &projMatrix, D3DXVECTOR3 &camPos, D3DXVECTOR4 &clip, SceneLighting lighting) const;
		void cleanup();

		bool isMirror() { return m_IsMirror; }
		bool ignoreRaycast() { return m_IgnoreRaycast; }
		bool shouldDraw() { return m_Draw; }

		void move(D3DXVECTOR3 &p);
		void rotate(D3DXVECTOR3 &r);

		void setPosition(Vec3<float> &pos);
		void setPosition(D3DXVECTOR3 &pos);
		void setColour(Vec3<float> &colour);
		void setScale(float scale);
		void setScale(Vec3<float> &scale);
		void setMirror(bool mirror) { m_IsMirror = mirror; }
		void setIgnoreRaycast(bool ignore) { m_IgnoreRaycast = ignore; }

		int getIndexCount() { return m_IndexCount; }
		int getVertexCount() { return m_VertexCount; }

		void show() { m_Draw = true;  }
		void hide() { m_Draw = false; }

		D3DXVECTOR3  &getPosition()		{ return m_Pos; }
		D3DMATRIX     getWorldMatrix()	{ return m_WorldMatrix; }
		Shader       *getShader()		{ return m_Shader; }
		ID3D11Device *getDevice()		{ return m_Device; }
		AABB         &getAABB()			{ return m_AABB; }

	protected:
		ID3D11Buffer *m_VertexBuffer, *m_IndexBuffer;
		ID3D11Device *m_Device;
		ID3D11DeviceContext *m_Context;

		D3DXMATRIX m_WorldMatrix, m_MatrixMov;
		D3DXMATRIX m_RotX, m_RotY, m_RotZ;
		D3DXMATRIX m_ScaleMatrix;

		D3D_PRIMITIVE_TOPOLOGY m_Topology;

		D3DXVECTOR3 m_Pos, m_Rot;
		Shader *m_Shader;
		AABB m_AABB;

		bool m_IgnoreRaycast;
		bool m_IsMirror;
		bool m_Draw;

		unsigned int m_VertexSize;
		int m_VertexCount, m_IndexCount;

		struct Vertex {
			D3DXVECTOR3 position;
			D3DXVECTOR3 normal;
			D3DXVECTOR4 color;
		};

		struct Vertex2D {
			D3DXVECTOR3 position;
			D3DXVECTOR2 uv;
		};

		std::vector<Vertex> m_Vertices;
		std::vector<Vertex2D> m_Vertices2D;
		std::vector<WORD> m_Indices;

		virtual void init() = 0;
		void initMatrices();
		bool initData(bool use2D = false);
};

class Cube : public Primitive {
	public:
		Cube(ID3D11Device *device, ID3D11DeviceContext *context, DXShader shader) : Primitive(device, context, shader) {
			init();
			initData();
		}

	private:
		void init();
};

class Sprite : public Primitive {
	public:
		Sprite(ID3D11Device *device, ID3D11DeviceContext *context, DXShader shader);

		void load(std::string file);

	private:
		void init();

		DXShader m_DXShader;
		ID3D11InputLayout *m_VertexLayout;
};