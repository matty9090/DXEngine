#pragma once

#include "Vec3.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Timer.h"
#include "Input.hpp"
#include "Graphics.hpp"
#include "Primitive.hpp"
#include "Model.hpp"
#include "Light.hpp"
#include "Mirror.hpp"

#include <D3D10.h>
#include <D3DX10.h>
#include <D3DX10math.h>

class DXEngine {
	public:
		DXEngine(Graphics *graphics);
		~DXEngine();

		void render();
		void renderMirrors();
		void setAmbientColour(Colour col) { m_Lighting.ambient = col; }
		void setActiveCamera(Camera *cam) { m_Camera = cam; }

		void    createLight(PointLight *light);
		void    createLight(SpotLight  *light);
		void	createMirror(Vec2<size_t> res, Camera *cam, Model *model, bool reflect = false);
		Cube   *createCube(DXShader shader, D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		Model  *createModel(DXShader shader, D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		Camera *createCamera(D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f));


		void RGBToHSL(int R, int G, int B, int& H, int& S, int& L);
		void HSLToRGB(float H, float S, float L, int& R, int& G, int& B);

	private:
		ID3D11Device *m_Device;
		ID3D11DeviceContext *m_Context;

		Graphics *m_Graphics;
		SceneLighting m_Lighting;

		Camera *m_Camera;

		std::vector<PointLight*> m_Lights;
		std::vector<SpotLight*>  m_sLights;
		std::vector<Primitive*>  m_Objects;
		std::vector<Mirror>      m_Mirrors;
		std::vector<Camera*>     m_Cameras;

		float _min(float f1, float f2, float f3);
		float _max(float f1, float f2, float f3);

		void renderMirrorMatrix(Mirror &mirror, D3DXMATRIX &matrix, D3DXVECTOR3 &camPos, D3DXPLANE &plane);
};