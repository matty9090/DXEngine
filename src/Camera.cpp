#include "Camera.hpp"

Camera::Camera() : m_Pitch(0), m_Yaw(0) {
	D3DXMatrixIdentity(&m_ViewMatrix);
}

Camera::~Camera() {

}

void Camera::setPosition(D3DXVECTOR3 pos) {
	m_Pos = pos;
}

void Camera::moveX(float v) {
	m_Pos += m_Right * v;
}

void Camera::moveZ(float v) {
	m_Pos += m_Forward * v;
}

void Camera::rotate(D3DXVECTOR3 r) {
	m_Pitch += r.x;
	m_Yaw += r.y;
}

Vec3<float> Camera::getPosition() {
	return Vec3<float>(m_Pos.x, m_Pos.y, m_Pos.z);
}

D3DXVECTOR3 Camera::getDxPosition() {
	return m_Pos;
}

void Camera::render() {
	/*float cosPitch = cosf(m_Pitch);
	float sinPitch = sinf(m_Pitch);
	float cosYaw = cosf(m_Yaw);
	float sinYaw = sinf(m_Yaw);

	D3DXVECTOR3 xaxis = { cosYaw, 0, -sinYaw };
	D3DXVECTOR3 yaxis = { sinYaw * sinPitch, cosPitch, cosYaw * sinPitch };
	D3DXVECTOR3 zaxis = { sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw };

	float xdot = D3DXVec3Dot(&xaxis, &m_Pos);
	float ydot = D3DXVec3Dot(&yaxis, &m_Pos);
	float zdot = D3DXVec3Dot(&zaxis, &m_Pos);

	m_ViewMatrix = {
		xaxis.x, yaxis.x, zaxis.x, 0,
		xaxis.y, yaxis.y, zaxis.y, 0,
		xaxis.z, yaxis.z, zaxis.z, 0,
		-xdot  , -ydot  , -zdot  , 1
	};

	m_Forward = zaxis, m_Right = xaxis;*/

	D3DXMatrixRotationX(&m_RotX, -m_Pitch);
	D3DXMatrixRotationY(&m_RotY, -m_Yaw);
	D3DXMatrixRotationZ(&m_RotZ, 0.0f);

	D3DXMatrixTranslation(&m_Trans, -m_Pos.x, -m_Pos.y, -m_Pos.z);

	m_ViewMatrix = m_Trans * m_RotY * m_RotX * m_RotZ;

	m_Right   = D3DXVECTOR3(m_ViewMatrix._11, m_ViewMatrix._21, m_ViewMatrix._31);
	m_Forward = D3DXVECTOR3(m_ViewMatrix._13, m_ViewMatrix._23, m_ViewMatrix._33);
}