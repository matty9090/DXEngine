#include "Model.hpp"
#include "Import/CImportXFile.h"

#include <DxErr.h>
#pragma comment(lib, "dxerr.lib")

#include <iostream>

Model::Model(ID3D11Device *device, DXShader shader) : Primitive(device, DXShader()) {
	m_VertexShader = shader.vertex;
	m_PixelShader  = shader.pixel;

	m_Shader = new Shader(device, m_VertexShader, m_PixelShader, false);
}

bool Model::load(std::string file, bool tangents) {
	cleanup();

	m_Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	gen::CImportXFile mesh;
	gen::SSubMesh subMesh;

	if (mesh.ImportFile(file.c_str()) != gen::kSuccess)
		return false;

	if (mesh.GetSubMesh(0, &subMesh, tangents) != gen::kSuccess)
		return false;

	m_VertexCount = subMesh.numVertices;
	m_IndexCount = subMesh.numFaces * 3;

	unsigned int numElts = 0;
	unsigned int offset = 0;

	m_VertexElts[numElts].SemanticName = "POSITION";   // Semantic in HLSL (what is this data for)
	m_VertexElts[numElts].SemanticIndex = 0;           // Index to add to semantic (a count for this kind of data, when using multiple of the same type, e.g. TEXCOORD0, TEXCOORD1)
	m_VertexElts[numElts].Format = DXGI_FORMAT_R32G32B32_FLOAT; // Type of data - this one will be a float3 in the shader. Most data communicated as though it were colours
	m_VertexElts[numElts].AlignedByteOffset = offset;  // Offset of element from start of vertex data (e.g. if we have position (float3), uv (float2) then normal, the normal's offset is 5 floats = 5*4 = 20)
	m_VertexElts[numElts].InputSlot = 0;               // For when using multiple vertex buffers (e.g. instancing - an advanced topic)
	m_VertexElts[numElts].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; // Use this value for most cases (only changed for instancing)
	m_VertexElts[numElts].InstanceDataStepRate = 0;

	offset += 12;
	++numElts;

	if (subMesh.hasNormals) {
		m_VertexElts[numElts].SemanticName = "NORMAL";
		m_VertexElts[numElts].SemanticIndex = 0;
		m_VertexElts[numElts].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		m_VertexElts[numElts].AlignedByteOffset = offset;
		m_VertexElts[numElts].InputSlot = 0;
		m_VertexElts[numElts].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		m_VertexElts[numElts].InstanceDataStepRate = 0;
		offset += 12;
		++numElts;
	}

	if (subMesh.hasTangents) {
		m_VertexElts[numElts].SemanticName = "TANGENT";
		m_VertexElts[numElts].SemanticIndex = 0;
		m_VertexElts[numElts].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		m_VertexElts[numElts].AlignedByteOffset = offset;
		m_VertexElts[numElts].InputSlot = 0;
		m_VertexElts[numElts].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		m_VertexElts[numElts].InstanceDataStepRate = 0;
		offset += 12;
		++numElts;
	}

	if (subMesh.hasTextureCoords) {
		m_VertexElts[numElts].SemanticName = "TEXCOORD";
		m_VertexElts[numElts].SemanticIndex = 0;
		m_VertexElts[numElts].Format = DXGI_FORMAT_R32G32_FLOAT;
		m_VertexElts[numElts].AlignedByteOffset = offset;
		m_VertexElts[numElts].InputSlot = 0;
		m_VertexElts[numElts].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		m_VertexElts[numElts].InstanceDataStepRate = 0;
		offset += 8;
		++numElts;
	}

	if (subMesh.hasVertexColours) {
		m_VertexElts[numElts].SemanticName = "COLOR";
		m_VertexElts[numElts].SemanticIndex = 0;
		m_VertexElts[numElts].Format = DXGI_FORMAT_R8G8B8A8_UNORM; // A RGBA colour with 1 byte (0-255) per component
		m_VertexElts[numElts].AlignedByteOffset = offset;
		m_VertexElts[numElts].InputSlot = 0;
		m_VertexElts[numElts].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		m_VertexElts[numElts].InstanceDataStepRate = 0;
		offset += 4;
		++numElts;
	}

	m_VertexSize = offset;

	ID3DBlob *shader = m_Shader->getVertexShaderBuffer();

	m_Device->CreateInputLayout(m_VertexElts, numElts, shader->GetBufferPointer(), shader->GetBufferSize(), &m_VertexLayout);
	m_Shader->init(m_Device, m_VertexShader, m_PixelShader, m_VertexLayout);

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = m_VertexSize * m_VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = subMesh.vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(m_Device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer)))
		return false;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(WORD) * m_IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = subMesh.faces;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(m_Device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer)))
		return false;

	gen::SMeshMaterial mat;
	mesh.GetMaterial(0, &mat);

	m_Shader->setTexture("res/" + mat.textureFileNames[0]);

	return true;
}

Model::~Model() {

}
