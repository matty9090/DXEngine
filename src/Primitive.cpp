#include "Primitive.hpp"

Primitive::Primitive(ID3D11Device *device, ID3D11DeviceContext *context, DXShader shader)
	: m_VertexCount(0), m_IndexCount(0), m_Device(device), m_Context(context), m_AABB(m_Pos), m_IgnoreRaycast(false), m_Draw(true)
{
	m_VertexBuffer = NULL;
	m_IndexBuffer = NULL;

	if(!shader.vertex.empty())
		m_Shader = new Shader(device, context, shader.vertex, shader.pixel);

	initMatrices();
}

Primitive::~Primitive() {

}

void Primitive::initMatrices() {
	D3DXMatrixIdentity(&m_WorldMatrix);
	D3DXMatrixTranslation(&m_MatrixMov, 0, 0, 0);
	D3DXMatrixScaling(&m_ScaleMatrix, 1.0f, 1.0f, 1.0f);

	rotate(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}

void Primitive::render(ID3D11DeviceContext *deviceContext, D3DXMATRIX &viewMatrix, D3DXMATRIX &projMatrix, D3DXVECTOR3 &camPos, D3DXVECTOR4 &clip, SceneLighting lighting) const {
	UINT offset = 0;
	
	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &m_VertexSize, &offset);
	deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	deviceContext->IASetPrimitiveTopology(m_Topology);

	m_Shader->render(deviceContext, m_IndexCount, m_WorldMatrix, viewMatrix, projMatrix, camPos, clip, lighting);
}

void Primitive::cleanup() {
	if (m_IndexBuffer)  m_IndexBuffer->Release();
	if (m_VertexBuffer) m_VertexBuffer->Release();
}

void Primitive::setPosition(Vec3<float> &pos) {
	m_Pos = D3DXVECTOR3(pos.x, pos.y, pos.z);
	D3DXMatrixTranslation(&m_MatrixMov, m_Pos.x, m_Pos.y, m_Pos.z);
	m_WorldMatrix = m_ScaleMatrix * m_MatrixMov * m_RotZ * m_RotX * m_RotY;
}

void Primitive::setPosition(D3DXVECTOR3 &pos) {
	m_Pos = pos;
	D3DXMatrixTranslation(&m_MatrixMov, m_Pos.x, m_Pos.y, m_Pos.z);
	m_WorldMatrix = m_ScaleMatrix * m_MatrixMov * m_RotZ * m_RotX * m_RotY;
}

void Primitive::setColour(Vec3<float> &colour) {
	for (auto &vertex : m_Vertices)
		vertex.color = D3DXVECTOR4(colour.x, colour.y, colour.z, 1.0f);

	cleanup();
	initData();
}

void Primitive::setScale(float scale) {
	D3DXMatrixScaling(&m_ScaleMatrix, scale, scale, scale);
	m_WorldMatrix = m_ScaleMatrix * m_MatrixMov * m_RotZ * m_RotX * m_RotY;
	m_AABB.setBounds(D3DXVECTOR3(-10.0f, -10.0f, -10.0f) * scale, D3DXVECTOR3(10.0f, 10.0f, 10.0f) * scale);
}

void Primitive::setScale(Vec3<float>& scale) {
	D3DXMatrixScaling(&m_ScaleMatrix, scale.x, scale.y, scale.z);
	m_WorldMatrix = m_ScaleMatrix * m_MatrixMov * m_RotZ * m_RotX * m_RotY;
	m_AABB.setBounds(D3DXVECTOR3(-scale.x, -scale.y, -scale.z), D3DXVECTOR3(scale.x, scale.y, scale.z));
}

void Primitive::move(D3DXVECTOR3 &p) {
	m_Pos += p;

	D3DXMatrixTranslation(&m_MatrixMov, m_Pos.x, m_Pos.y, m_Pos.z);

	m_WorldMatrix = m_ScaleMatrix * m_MatrixMov * m_RotZ * m_RotX * m_RotY;
}

void Primitive::rotate(D3DXVECTOR3 &r) {
	m_Rot += r;

	D3DXMatrixRotationX(&m_RotX, m_Rot.x);
	D3DXMatrixRotationY(&m_RotY, m_Rot.y);
	D3DXMatrixRotationZ(&m_RotZ, m_Rot.z);

	m_WorldMatrix = m_ScaleMatrix * m_MatrixMov * m_RotZ * m_RotX * m_RotY;
}

bool Primitive::initData(bool use2D) {
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = m_VertexSize * m_VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	if (use2D)
		vertexData.pSysMem = &m_Vertices2D[0];
	else
		vertexData.pSysMem = &m_Vertices[0];

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

	indexData.pSysMem = &m_Indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(m_Device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer)))
		return false;

	return true;
}

void Cube::init() {
	m_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_Vertices = {
		{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(-0, -0, -1), D3DXVECTOR4(0.0f, 0.3f, 0.3f, 0.0f) },
		{ D3DXVECTOR3(-1.0f,  1.0f, -1.0f), D3DXVECTOR3(-0, -0, -1),  D3DXVECTOR4(0.0f, 0.5f, 0.5f, 0.0f) },
		{ D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(-0, -0, -1),  D3DXVECTOR4(0.0f, 0.6f, 0.6f, 0.0f) },
		{ D3DXVECTOR3(1.0f,  1.0f, -1.0f), D3DXVECTOR3(-1, -0, -0),  D3DXVECTOR4(0.0f, 0.8f, 0.8f, 0.0f) },
		{ D3DXVECTOR3(1.0f,  -1.0f, 1.0f), D3DXVECTOR3(-1, -0, -0),  D3DXVECTOR4(0.0f, 0.5f, 0.5f, 0.0f) },
		{ D3DXVECTOR3(1.0f,  1.0f, 1.0f), D3DXVECTOR3(-1, -0, -0),  D3DXVECTOR4(0.0f, 0.3f, 0.3f, 0.0f) },
		{ D3DXVECTOR3(-1.0f,  -1.0f, 1.0f), D3DXVECTOR3(-1, -0, -0),  D3DXVECTOR4(0.0f, 0.5f, 0.5f, 0.0f) },
		{ D3DXVECTOR3(-1.0f,  1.0f, 1.0f), D3DXVECTOR3(-1, -0, -0),  D3DXVECTOR4(0.0f, 0.3f, 0.3f, 0.0f) }
	};

	m_Indices = {
		1, 3, 0,
		2, 4, 3,
		5, 1, 7,
		0, 6, 4,
		7, 5
	};

	m_VertexCount = m_Vertices.size();
	m_IndexCount = m_Indices.size();
	m_VertexSize = sizeof(Vertex);
}

Sprite::Sprite(ID3D11Device *device, ID3D11DeviceContext *context, DXShader shader) : Primitive(device, context, DXShader()), m_DXShader(shader) {
	m_Shader = new Shader(device, context, shader.vertex, shader.pixel, false);
	
	init();
	initData(true);
}

void Sprite::load(std::string file) {
	m_Shader->setTexture(file);
}

void Sprite::init() {
	m_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_Vertices2D = {
		{ D3DXVECTOR3(-0.5f,  0.5f, 0.0f), D3DXVECTOR2(0.0f, 0.0f) },
		{ D3DXVECTOR3( 0.5f,  0.5f, 0.0f), D3DXVECTOR2(1.0f, 0.0f) },
		{ D3DXVECTOR3(-0.5f, -0.5f, 0.0f), D3DXVECTOR2(0.0f, 1.0f) },
		{ D3DXVECTOR3( 0.5f, -0.5f, 0.0f), D3DXVECTOR2(1.0f, 1.0f) }
	};

	m_Indices = {
		0, 1, 2,
		2, 1, 3
	};

	m_VertexCount = m_Vertices2D.size();
	m_IndexCount = m_Indices.size();
	m_VertexSize = sizeof(Vertex2D);

	D3D11_INPUT_ELEMENT_DESC vertexLayout[] = {
		// Semantic   Index  Format							 Slot   Offset	Slot Class					 Instance Step
		{ "POSITION", 0,	 DXGI_FORMAT_R32G32B32_FLOAT,	 0,		0,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0,	 DXGI_FORMAT_R32G32_FLOAT,		 0,		12,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	unsigned int numElements = sizeof(vertexLayout) / sizeof(vertexLayout[0]);
	m_Device->CreateInputLayout(vertexLayout, numElements, m_Shader->getVertexShaderBuffer()->GetBufferPointer(), m_Shader->getVertexShaderBuffer()->GetBufferSize(), &m_VertexLayout);

	m_Shader->init(m_Device, m_DXShader.vertex, m_DXShader.pixel, m_VertexLayout);
}
