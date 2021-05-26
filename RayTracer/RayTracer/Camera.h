#pragma once
#include "vec3f.h"
#include "Mat4x4.h"
#include "SFML/include/SFML/System/Vector2.hpp"
#include "Plane.h"
class Ray;
class Vec3f;
class Camera
{
public:
	Camera(int fov, float width, float height, const Vec3f& origin, const Vec3f& lookat);
	Ray getRayAt(int y, int x, bool cameraMode = false);
	void updateViewMatrix();
	void mouseMove(sf::Vector2<int> newMouse);

	void rotateX(const float& degrees);
	void rotateY(const float& degrees);
	void rotateZ(const float& degrees);
	void moveRelative(const Vec4f moveVector);
	int getFOV();
	void changeFOV(int offset);
	void changeFocalLenght(float offset);
	void changeAppetureSize(float offset);
	float getFocalLenght() { return focalLenght; }
	float getApeture() { return  aperture; }
	Mat4x4 viewMatrix;
	
	Vec3f m_lookAt{ 0.f,1.f,-5.f };
	Vec3f m_cameraOrigin;
	float m_highResWidth;
	float m_highResHeight;

private:
	float m_angle;
	float m_width;
	float m_height;
	float m_aspectRatio;
	Vec3f m_up{ 0,1,0 };

	float m_yaw{};
	float m_pitch{};
	
	int m_fov{};
	sf::Vector2<int> m_mousePos{};

	Plane focalPlane{ {0.f,0.f,-4.f},{},{},{} };

	float originJitter{ 0.0025f };
	float aperture{ 0.0070f };
	float focalLenght{ -4.f };

};

