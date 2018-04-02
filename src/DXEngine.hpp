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

#include <D3D10.h>
#include <D3DX10.h>
#include <D3DX10math.h>

class DXEngine {
	public:
		DXEngine(Graphics *graphics);
		~DXEngine();

		void render();
		void setAmbientColour(Colour col) { m_Ambient = col; }

		void    createLight(Light *light);
		Cube   *createCube(DXShader shader, D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		Model  *createModel(DXShader shader, D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		Camera *createCamera(D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	private:
		ID3D11Device *m_Device;
		ID3D11DeviceContext *m_Context;

		Graphics *m_Graphics;
		Camera *m_Camera;
		Colour m_Ambient;

		std::vector<Light*> m_Lights;
		std::vector<Primitive*> m_Objects;
};