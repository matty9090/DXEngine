#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <d3dcompiler.h>

#include <string>
#include <vector>

#include "Light.hpp"

struct DXShader {
	std::wstring vertex, pixel;

	DXShader() {}
	DXShader(std::wstring v, std::wstring p) : vertex(v), pixel(p) {}
};

class Shader {
	public:
		Shader(ID3D11Device *device, ID3D11DeviceContext *context, std::wstring vertexShader, std::wstring pixelShader, bool should_init = true);
		~Shader();
		
		bool init(ID3D11Device *device, std::wstring vertexShader, std::wstring pixelShader, ID3D11InputLayout *layout = NULL);
		bool render(ID3D11DeviceContext *deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX &viewMatrix, D3DXMATRIX &projectionMatrix, D3DXVECTOR3 &camPos, D3DXVECTOR4 &clip, SceneLighting lighting);
		void cleanup();

		void setTexture(std::string tex);
		void setNormalMap(std::string tex);
		void setSpecularMap(std::string tex);
		void setParallaxMap(std::string tex);
		void setSamplerState();
		void setBlendState(int blend);
		void setRasterState(D3D11_CULL_MODE cull, bool wireframe = false);
		void setDepthState(ID3D11DepthStencilState *depth) { m_DepthState = depth; }
		void setRenderTexture(ID3D11ShaderResourceView *tex) { m_Texture = tex; }

		void addBuffer(void *data, size_t size);
		void mapBuffer(int i);
		void updateBuffer(int i, void *data);

		ID3DBlob *getVertexShaderBuffer() { return m_VertexShaderBuffer; }

		enum EBlendState { Additive, Alpha, Multi, None };

	private:
		struct MatrixBuffer {
			D3DXMATRIX  world;		// 64 bytes
			D3DXMATRIX  view;		// 64 bytes
			D3DXMATRIX  projection; // 64 bytes
			D3DXVECTOR3 camPos;		// 12 bytes
			float pad;				// 4 bytes
			D3DXVECTOR4 clipPlane;  // 16 bytes
		};

		ID3D11Device *m_Device;
		ID3D11DeviceContext *m_Context;
		ID3D11VertexShader *m_VertexShader;
		ID3D11PixelShader *m_PixelShader;
		ID3D11InputLayout *m_Layout;
		ID3D11Buffer *m_MatrixBuffer, *m_LightBuffer;
		ID3DBlob *m_VertexShaderBuffer = NULL, *m_PixelShaderBuffer = NULL;
		ID3D11ShaderResourceView *m_Texture, *m_NormalMap, *m_SpecularMap, *m_ParallaxMap;
		ID3D11SamplerState *m_Sampler;
		ID3D11RasterizerState *m_Raster;
		ID3D11BlendState *m_Blend;
		ID3D11DepthStencilState *m_DepthState;

		struct Buffer {
			ID3D11Buffer *buffer;
			void *data;
			size_t size;
		};

		std::vector<Buffer> m_Buffers;
		
		bool setParameters(ID3D11DeviceContext *deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXVECTOR3 camPos, D3DXVECTOR4 &clip, SceneLighting lighting);
		void handleErrors(ID3D10Blob *errorMessage, char *shaderFilename);
};
