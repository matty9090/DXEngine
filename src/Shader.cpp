#include "Shader.hpp"

#include <fstream>

Shader::Shader(ID3D11Device *device, ID3D11DeviceContext *context, std::wstring vertexShader, std::wstring pixelShader, bool should_init) : m_Device(device), m_Context(context) {
	m_VertexShader = NULL;
	m_PixelShader = NULL;
	m_Layout = NULL;
	m_MatrixBuffer = NULL;
	m_LightBuffer = NULL;
	m_Texture = NULL;
	m_NormalMap = NULL;
	m_ParallaxMap = NULL;
	m_SpecularMap = NULL;
	m_DepthState = NULL;
	m_Blend = NULL;
	m_Sampler = NULL;

	D3DReadFileToBlob(vertexShader.c_str(), &m_VertexShaderBuffer);
	D3DReadFileToBlob(pixelShader.c_str(), &m_PixelShaderBuffer);
	device->CreateVertexShader(m_VertexShaderBuffer->GetBufferPointer(), m_VertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
	device->CreatePixelShader(m_PixelShaderBuffer->GetBufferPointer(), m_PixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader);

	if(should_init)
		init(device, vertexShader, pixelShader);
}

bool Shader::init(ID3D11Device *device, std::wstring vertexShader, std::wstring pixelShader, ID3D11InputLayout *layout) {
	ID3D10Blob *errorMessage = NULL;
	D3D11_BUFFER_DESC matrixBufferDesc, lightBufferDesc;
	
	if (!layout) {
		D3D11_INPUT_ELEMENT_DESC vertexLayout[] = {
			// Semantic   Index  Format							 Slot   Offset	Slot Class					 Instance Step
			{ "POSITION", 0,	 DXGI_FORMAT_R32G32B32_FLOAT,	 0,		0,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",	  0,	 DXGI_FORMAT_R32G32B32_FLOAT,	 0,		12,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0,	 DXGI_FORMAT_R32G32B32A32_FLOAT, 0,		24,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		unsigned int numElements = sizeof(vertexLayout) / sizeof(vertexLayout[0]);

		if (FAILED(device->CreateInputLayout(vertexLayout, numElements, m_VertexShaderBuffer->GetBufferPointer(), m_VertexShaderBuffer->GetBufferSize(), &m_Layout)))
			return false;
	} else
		m_Layout = layout;

	m_VertexShaderBuffer->Release();
	m_PixelShaderBuffer->Release();

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(SceneLighting);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_MatrixBuffer))) return false;
	if (FAILED(device->CreateBuffer(&lightBufferDesc, NULL, &m_LightBuffer))) return false;

	setSamplerState();
	setBlendState(Alpha);
	setRasterState(D3D11_CULL_BACK);

	return true;
}

void Shader::cleanup() {
	if (m_Layout) m_Layout->Release();
	if (m_PixelShader) m_PixelShader->Release();
	if (m_VertexShader) m_VertexShader->Release();
	if (m_MatrixBuffer) m_MatrixBuffer->Release();
	if (m_LightBuffer) m_LightBuffer->Release();
}

void Shader::setSamplerState() {
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	m_Device->CreateSamplerState(&samplerDesc, &m_Sampler);
}

void Shader::setBlendState(int blend) {
	D3D11_BLEND_DESC blendState;
	ZeroMemory(&blendState, sizeof(D3D11_BLEND_DESC));

	if (blend == Alpha) {
		blendState.RenderTarget[0].BlendEnable = TRUE;
		blendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	} else if (blend == Additive) {
		blendState.RenderTarget[0].BlendEnable = TRUE;
		blendState.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendState.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	} else {
		blendState.RenderTarget[0].BlendEnable = FALSE;
	}

	m_Device->CreateBlendState(&blendState, &m_Blend);
}

void Shader::setRasterState(D3D11_CULL_MODE cull, bool wireframe) {
	D3D11_RASTERIZER_DESC rasterDesc;

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = cull;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = (wireframe) ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	m_Device->CreateRasterizerState(&rasterDesc, &m_Raster);
}

void Shader::addBuffer(void *data, size_t size) {
	D3D11_BUFFER_DESC desc;

	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	Buffer buf;
	buf.size = size;
	buf.data = data;

	HRESULT r = m_Device->CreateBuffer(&desc, NULL, &buf.buffer);
	m_Buffers.push_back(buf);

	updateBuffer(m_Buffers.size() - 1, data);
}

void Shader::mapBuffer(int i) {
	D3D11_MAPPED_SUBRESOURCE sub;

	void *bufferPtr;
	
	m_Context->Map(m_Buffers[i].buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);

	bufferPtr = (void*)sub.pData;
	memcpy(bufferPtr, m_Buffers[i].data, m_Buffers[i].size);

	m_Context->Unmap(m_Buffers[i].buffer, 0);
}

void Shader::updateBuffer(int i, void *data) {
	m_Buffers[i].data = data;
}

void Shader::setTexture(std::string tex) {
	D3DX11CreateShaderResourceViewFromFileA(m_Device, tex.c_str(), NULL, NULL, &m_Texture, NULL);
}

void Shader::setNormalMap(std::string tex) {
	D3DX11CreateShaderResourceViewFromFileA(m_Device, tex.c_str(), NULL, NULL, &m_NormalMap, NULL);
}

void Shader::setSpecularMap(std::string tex) {
	D3DX11CreateShaderResourceViewFromFileA(m_Device, tex.c_str(), NULL, NULL, &m_SpecularMap, NULL);
}

void Shader::setParallaxMap(std::string tex) {
	D3DX11CreateShaderResourceViewFromFileA(m_Device, tex.c_str(), NULL, NULL, &m_ParallaxMap, NULL);
}

bool Shader::render(ID3D11DeviceContext *deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX &viewMatrix, D3DXMATRIX &projectionMatrix, D3DXVECTOR3 &camPos, SceneLighting lighting) {
	if (!setParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, camPos, lighting))
		return false;

	deviceContext->IASetInputLayout(m_Layout);
	deviceContext->VSSetShader(m_VertexShader, NULL, 0);
	deviceContext->PSSetShader(m_PixelShader, NULL, 0);
	deviceContext->PSSetSamplers(0, 1, &m_Sampler);
	deviceContext->RSSetState(m_Raster);
	deviceContext->OMSetBlendState(m_Blend, 0, 0xFFFFFFFF);
	
	if(m_DepthState)
		deviceContext->OMSetDepthStencilState(m_DepthState, 1);

	deviceContext->DrawIndexed(indexCount, 0, 0);

	return true;
}

bool Shader::setParameters(ID3D11DeviceContext *deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXVECTOR3 camPos, SceneLighting lighting) {
	D3D11_MAPPED_SUBRESOURCE mapMatrix, mapLight;
	MatrixBuffer *matrixPtr;
	SceneLighting  *lightPtr;

	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	if (FAILED(deviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapMatrix))) return false;

	matrixPtr = (MatrixBuffer*)mapMatrix.pData;
	matrixPtr->world		= worldMatrix;
	matrixPtr->view			= viewMatrix;
	matrixPtr->projection	= projectionMatrix;
	matrixPtr->camPos		= camPos;
	matrixPtr->pad			= 0;
	
	deviceContext->Unmap(m_MatrixBuffer, 0);

	if (FAILED(deviceContext->Map(m_LightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapLight))) return false;

	lightPtr = (SceneLighting*)mapLight.pData;
	lightPtr->ambient = lighting.ambient;
	lightPtr->num  = lighting.num;
	lightPtr->nums = lighting.nums;

	memcpy(lightPtr->lights,  lighting.lights,  sizeof(PointLight) * lighting.num);
	memcpy(lightPtr->slights, lighting.slights, sizeof(SpotLight)  * lighting.nums);

	deviceContext->Unmap(m_LightBuffer, 0);

	for (size_t i = 0; i < m_Buffers.size(); ++i)
		mapBuffer(i);

	if (m_Texture)		deviceContext->PSSetShaderResources(0, 1, &m_Texture);
	if (m_NormalMap)	deviceContext->PSSetShaderResources(1, 1, &m_NormalMap);
	if (m_SpecularMap)	deviceContext->PSSetShaderResources(2, 1, &m_SpecularMap);
	if (m_ParallaxMap)	deviceContext->PSSetShaderResources(3, 1, &m_ParallaxMap);

	deviceContext->VSSetConstantBuffers(0, 1, &m_MatrixBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &m_MatrixBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &m_LightBuffer);

	int startSlot = 1;

	for (size_t i = 0; i < m_Buffers.size(); ++i) {
		deviceContext->VSSetConstantBuffers(startSlot    , 1, &m_Buffers[i].buffer);
		deviceContext->PSSetConstantBuffers(startSlot + 1, 1, &m_Buffers[i].buffer);

		++startSlot;
	}

	return true;
}

void Shader::handleErrors(ID3D10Blob *errorMessage, char *shaderFilename) {
	char *compileErrors;
	unsigned long bufferSize, i;
	std::ofstream file("errors.txt");

	compileErrors = (char*)(errorMessage->GetBufferPointer());
	bufferSize = errorMessage->GetBufferSize();

	for (i = 0; i < bufferSize; i++)
		file << compileErrors[i];

	file.close();

	errorMessage->Release();
	errorMessage = 0;
}

Shader::~Shader() {

}
