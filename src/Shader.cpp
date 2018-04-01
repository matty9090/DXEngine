#include "Shader.hpp"

#include <fstream>

Shader::Shader(ID3D11Device *device, std::wstring vertexShader, std::wstring pixelShader, bool should_init) {
	m_VertexShader = NULL;
	m_PixelShader = NULL;
	m_Layout = NULL;
	m_MatrixBuffer = NULL;
	m_LightBuffer = NULL;

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
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_MatrixBuffer))) return false;
	if (FAILED(device->CreateBuffer(&lightBufferDesc, NULL, &m_LightBuffer))) return false;

	return true;
}

void Shader::cleanup() {
	if (m_Layout) m_Layout->Release();
	if (m_PixelShader) m_PixelShader->Release();
	if (m_VertexShader) m_VertexShader->Release();
	if (m_MatrixBuffer) m_MatrixBuffer->Release();
	if (m_LightBuffer) m_LightBuffer->Release();
}

bool Shader::render(ID3D11DeviceContext *deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX &viewMatrix, D3DXMATRIX &projectionMatrix, D3DXVECTOR3 &camPos, D3DXVECTOR3 &lightPos, D3DXVECTOR3 &lightCol, D3DXVECTOR3 &ambientColour) {
	if (!setParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, camPos, lightPos, lightCol, ambientColour))
		return false;

	deviceContext->IASetInputLayout(m_Layout);
	deviceContext->VSSetShader(m_VertexShader, NULL, 0);
	deviceContext->PSSetShader(m_PixelShader, NULL, 0);
	
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return true;
}

bool Shader::setParameters(ID3D11DeviceContext *deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXVECTOR3 camPos, D3DXVECTOR3 lightPos, D3DXVECTOR3 lightCol, D3DXVECTOR3 ambientColour) {
	D3D11_MAPPED_SUBRESOURCE mapMatrix, mapLight;
	MatrixBuffer *matrixPtr;
	LightBuffer  *lightPtr;
	unsigned int bufferNumber;

	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	if (FAILED(deviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapMatrix))) return false;

	matrixPtr = (MatrixBuffer*)mapMatrix.pData;
	matrixPtr->world		= worldMatrix;
	matrixPtr->view			= viewMatrix;
	matrixPtr->projection	= projectionMatrix;

	deviceContext->Unmap(m_MatrixBuffer, 0);

	if (FAILED(deviceContext->Map(m_LightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapLight))) return false;

	lightPtr = (LightBuffer*)mapLight.pData;
	lightPtr->lightPos		= lightPos;
	lightPtr->camPos		= camPos;
	lightPtr->lightCol		= lightCol;
	lightPtr->ambientCol	= ambientColour;

	deviceContext->Unmap(m_LightBuffer, 0);

	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_LightBuffer);

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
