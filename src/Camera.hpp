#pragma once

#include <d3dx10math.h>

#include "Vec3.hpp"

class Camera {
	public:
		Camera();
		~Camera();

		void setPosition(D3DXVECTOR3 pos);

		void moveX(float v);
		void moveZ(float v);
		void rotate(D3DXVECTOR3 r);

		Vec3<float> getPosition();
		D3DXVECTOR3 getDxPosition();

		void render();
		void getViewMatrix(D3DXMATRIX &m) { m = m_ViewMatrix; }

	private:
		float m_Pitch, m_Yaw;

		D3DXVECTOR3 m_Pos, m_Forward, m_Right;
		D3DXMATRIX m_ViewMatrix, m_Trans, m_RotX, m_RotY, m_RotZ;
};
