#include "Camera.hpp"

Camera::Camera() : m_Pitch(0), m_Yaw(0) {
	
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

D3DXVECTOR3 Camera::getDirection() {
	return m_Forward;
}

void Camera::render() {
	D3DXMatrixRotationX(&m_RotX, -m_Pitch);
	D3DXMatrixRotationY(&m_RotY, -m_Yaw);
	D3DXMatrixRotationZ(&m_RotZ, 0.0f);

	D3DXMatrixTranslation(&m_Trans, -m_Pos.x, -m_Pos.y, -m_Pos.z);

	m_ViewMatrix = m_Trans * m_RotY * m_RotX * m_RotZ;

	m_Right   = D3DXVECTOR3(m_ViewMatrix._11, m_ViewMatrix._21, m_ViewMatrix._31);
	m_Forward = D3DXVECTOR3(m_ViewMatrix._13, m_ViewMatrix._23, m_ViewMatrix._33);
}
