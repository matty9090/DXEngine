#include "DXEngine.hpp"

DXEngine::DXEngine(Graphics *graphics) : m_Device(graphics->getDevice()), m_Context(graphics->getDeviceContext()), m_Graphics(graphics) {
	m_Lighting.ambient = Colour(0.05f, 0.05f, 0.05f);
}

DXEngine::~DXEngine() {
	for (auto &obj   : m_Objects) delete obj;
	for (auto &light : m_Lights)  delete light;

	delete m_Camera;

	m_Objects.clear();
	m_Lights.clear();
}

void DXEngine::render() {
	for (size_t i = 0; i < m_Lights.size(); ++i)
		m_Lighting.lights[i] = *m_Lights[i];

	D3DXMATRIX viewMatrix;
	m_Camera->render();
	m_Camera->getViewMatrix(viewMatrix);

	for (auto &obj : m_Objects)
		obj->render(m_Context, viewMatrix, m_Graphics->getProjectionMatrix(), m_Camera->getDxPosition(), m_Lighting);
}

void DXEngine::createLight(Light *light) {
	m_Lights.push_back(light);
	m_Lighting.lights[m_Lights.size() - 1] = *light;
}

Cube *DXEngine::createCube(DXShader shader, D3DXVECTOR3 position) {
	Cube *cube = new Cube(m_Device, m_Context, shader);
	cube->setPosition(position);

	m_Objects.push_back(cube);

	return cube;
}

Model *DXEngine::createModel(DXShader shader, D3DXVECTOR3 position) {
	Model *model = new Model(m_Device, m_Context, shader);
	model->setPosition(position);
	model->setDepth(m_Graphics->getDepthStateOn());

	m_Objects.push_back(model);

	return model;
}

Camera *DXEngine::createCamera(D3DXVECTOR3 position) {
	m_Camera = new Camera();
	m_Camera->setPosition(position);

	return m_Camera;
}