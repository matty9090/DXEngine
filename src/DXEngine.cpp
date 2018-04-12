#include "DXEngine.hpp"

DXEngine::DXEngine(Graphics *graphics) : m_Device(graphics->getDevice()), m_Context(graphics->getDeviceContext()), m_Graphics(graphics) {
	m_Lighting.ambient = Colour(0.05f, 0.05f, 0.05f);
}

DXEngine::~DXEngine() {
	for (auto &obj    : m_Objects)  delete obj;
	for (auto &light  : m_Lights)   delete light;
	for (auto &camera : m_Cameras)  delete camera;

	m_Objects.clear();
	m_Lights.clear();
}

void DXEngine::render() {
	for (size_t i = 0; i < m_Lights.size(); ++i)
		m_Lighting.lights[i] = *m_Lights[i];

	for (size_t i = 0; i < m_sLights.size(); ++i)
		m_Lighting.slights[i] = *m_sLights[i];

	D3DXMATRIX viewMatrix;

	m_Camera->render();
	m_Camera->getViewMatrix(viewMatrix);

	for (auto &obj : m_Objects)
		obj->render(m_Context, viewMatrix, m_Graphics->getProjectionMatrix(), m_Camera->getDxPosition(), D3DXVECTOR4(0, 0, 0, 0), m_Lighting);
}

void DXEngine::renderMirrors() {
	D3DXMATRIX viewMatrix;
	D3DXVECTOR3 camPos;
	D3DXPLANE plane;

	for (size_t i = 0; i < m_Mirrors.size(); ++i) {
		m_Graphics->setRenderTargets(m_Mirrors[i].target, m_Mirrors[i].depthStencil, m_Mirrors[i].viewport);
		
		m_Mirrors[i].cam->render();

		if (m_Mirrors[i].reflect) {
			renderMirrorMatrix(m_Mirrors[i], viewMatrix, camPos, plane);
		} else {
			m_Mirrors[i].cam->getViewMatrix(viewMatrix);
			camPos = m_Mirrors[i].cam->getDxPosition();
			plane = D3DXPLANE(0, 0, 0, 0);
		}

		for (auto &obj : m_Objects)
			obj->render(m_Context, viewMatrix, m_Graphics->getProjectionMatrix(), camPos, (D3DXVECTOR4)plane, m_Lighting);
	}
}

void DXEngine::createLight(PointLight *light) {
	m_Lights.push_back(light);
	m_Lighting.lights[m_Lights.size() - 1] = *light;
	m_Lighting.num = m_Lights.size();
}

void DXEngine::createLight(SpotLight *light) {
	m_sLights.push_back(light);
	m_Lighting.slights[m_sLights.size() - 1] = *light;
	m_Lighting.nums = m_sLights.size();
}

void DXEngine::createMirror(Vec2<size_t> res, Camera *cam, Model *model, bool reflect) {
	Mirror mirror;

	mirror.cam		= cam;
	mirror.res		= res;
	mirror.model	= model;
	mirror.reflect	= reflect;

	m_Graphics->createMirror(mirror);
	model->getShader()->setRenderTexture(mirror.map);

	m_Mirrors.push_back(mirror);
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
	Camera *cam = new Camera();
	cam->setPosition(position);

	m_Cameras.push_back(cam);

	return cam;
}

void DXEngine::RGBToHSL(int R, int G, int B, int& H, int& S, int& L) {
	float fr = (float)R / 255.0f;
	float fg = (float)G / 255.0f;
	float fb = (float)B / 255.0f;

	float min = _min(fr, fg, fb);
	float max = _max(fr, fg, fb);

	L = 50 * (int)(max + min);

	if (min == max) {
		S = H = 0;
		return;
	}

	if (L < 50)
		S = 100 * (int)((max - min) / (max + min));
	else
		S = 100 * (int)((max - min) / (2 - max - min));

	if (max == fr) H = 60 * (int)((fg - fb) / (max - min));
	if (max == fg) H = 60 * (int)((fb - fr) / (max - min)) + 120;
	if (max == fb) H = 60 * (int)((fr - fg) / (max - min)) + 240;

	if (H < 0) H += 360;
}

void DXEngine::HSLToRGB(float H, float S, float L, int& R, int& G, int& B) {
	S /= 100.f, L /= 100.f;

	float hh = H / 60.0f;
	float C = (1.0f - fabs(2.0f * L - 1.0f)) * S;
	float X = C * (1.0f - fabs(fmodf(hh, 2.0f) - 1.0f));
	float fR = 0.0f, fG = 0.0f, fB = 0.0f;

	if (hh >= 0 && hh < 1) { fR = C; fG = X; }
	else if (hh >= 1 && hh < 2) { fR = X; fG = C; }
	else if (hh >= 2 && hh < 3) { fG = C; fB = X; }
	else if (hh >= 3 && hh < 4) { fG = X; fB = C; }
	else if (hh >= 4 && hh < 5) { fR = X; fB = C; }
	else { fR = C; fB = X; }

	float m = L - C / 2.0f;

	R = (int)((fR + m) * 255);
	G = (int)((fG + m) * 255);
	B = (int)((fB + m) * 255);
}

float DXEngine::_min(float f1, float f2, float f3) {
	float fMin = f1;

	if (f2 < fMin)
		fMin = f2;

	if (f3 < fMin)
		fMin = f3;

	return fMin;
}

float DXEngine::_max(float f1, float f2, float f3) {
	float fMax = f1;

	if (f2 > fMax)
		fMax = f2;

	if (f3 > fMax)
		fMax = f3;

	return fMax;
}

void DXEngine::renderMirrorMatrix(Mirror &mirror, D3DXMATRIX &matrix, D3DXVECTOR3 &camPos, D3DXPLANE &plane) {
	D3DXMATRIXA16 mirrorMatrix = mirror.model->getWorldMatrix();

	D3DXVECTOR3 mirrorPoint  = D3DXVECTOR3(mirrorMatrix(3, 0), mirrorMatrix(3, 1), mirrorMatrix(3, 2));
	D3DXVECTOR3 mirrorNormal = D3DXVECTOR3(mirrorMatrix(2, 0), mirrorMatrix(2, 1), mirrorMatrix(2, 2));

	D3DXPlaneFromPointNormal(&plane, &mirrorPoint, &mirrorNormal);

	D3DXMATRIX viewMatrix;
	mirror.cam->getViewMatrix(viewMatrix);

	D3DXMATRIXA16 reflectMatrix;
	D3DXMatrixReflect(&reflectMatrix, &plane);
	matrix = reflectMatrix * viewMatrix;

	D3DXVECTOR3 cameraPos = mirror.cam->getDxPosition();
	D3DXVECTOR4 reflectCameraPos4;
	D3DXVec3Transform(&reflectCameraPos4, &cameraPos, &reflectMatrix);
	camPos = D3DXVECTOR3((float*)reflectCameraPos4);
}
