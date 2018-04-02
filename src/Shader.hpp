#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <d3dcompiler.h>

#include <string>

struct DXShader {
	std::wstring vertex, pixel;

	DXShader() {}
	DXShader(std::wstring v, std::wstring p) : vertex(v), pixel(p) {}
};

class Shader {
	public:
		Shader(ID3D11Device *device, std::wstring vertexShader, std::wstring pixelShader, bool should_init = true);
		~Shader();
		
		bool init(ID3D11Device *device, std::wstring vertexShader, std::wstring pixelShader, ID3D11InputLayout *layout = NULL);
		bool render(ID3D11DeviceContext *deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX &viewMatrix, D3DXMATRIX &projectionMatrix, D3DXVECTOR3 &camPos, D3DXVECTOR3 &lightPos, D3DXVECTOR3 &lightCol, D3DXVECTOR3 &ambientColour);
		void setTexture(std::string tex);
		void cleanup();

		ID3DBlob *getVertexShaderBuffer() { return m_VertexShaderBuffer; }

	private:
		struct MatrixBuffer {
			D3DXMATRIX  world;		// 64 bytes
			D3DXMATRIX  view;		// 64 bytes
			D3DXMATRIX  projection; // 64 bytes
			D3DXVECTOR3 camPos;		// 12 bytes

			float pad0;
		};

		struct LightBuffer {
			D3DXVECTOR3 lightPos;	// 12 bytes
			float pad0;
			D3DXVECTOR3 lightCol;   // 12 bytes
			float pad1;
			D3DXVECTOR3 ambientCol;	// 12 bytes
			float pad2;
		};

		ID3D11Device *m_Device;
		ID3D11VertexShader *m_VertexShader;
		ID3D11PixelShader *m_PixelShader;
		ID3D11InputLayout *m_Layout;
		ID3D11Buffer *m_MatrixBuffer, *m_LightBuffer;
		ID3DBlob *m_VertexShaderBuffer = NULL, *m_PixelShaderBuffer = NULL;
		ID3D11ShaderResourceView *m_Texture;
		ID3D11SamplerState *m_Sampler;
		
		bool setParameters(ID3D11DeviceContext *deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXVECTOR3 camPos, D3DXVECTOR3 lightPos, D3DXVECTOR3 lightCol, D3DXVECTOR3 ambientColour);
		void handleErrors(ID3D10Blob *errorMessage, char *shaderFilename);
};
