#include "Camera.h"

#include "Utility.h"
#include <cmath>
#include "Ray.h"

Camera::Camera(int fov, float width, float height, const Vec3f& origin, const Vec3f& lookat) : m_width(width), m_height(height),
m_angle(tan((M_PI* fov / 180.f) / 2)), m_aspectRatio(width / height), m_cameraOrigin(origin), m_lookAt(lookat), m_fov{ fov }
{
	focalPlane.m_normalVector = { 0.f,0.f,1.f };
	viewMatrix =
	{
		Vec4f{1.f,0.f,0.f,0.f},
		Vec4f{0.f,1.f,0.f,1.f},
		Vec4f{0.f,0.f,1.f,0.f},
		Vec4f{0.f,0.f,0.f,1.f}
	};
	//updateViewMatrix();
}

Ray Camera::getRayAt(int y, int x, bool screenShotMode)
{
	float xpos{ static_cast<float>(x) };
	float ypos{ static_cast<float>(y) };
	float offsetX{};
	float offsetY{};
	if (screenShotMode == true)
	{
		offsetX =  cosf(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * M_PI))));
		offsetY =  sinf(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * M_PI))));
		xpos += offsetX * originJitter ;
		ypos += offsetY * originJitter;
	}

	// normalize screen coordinates and make sure the values are from the middle of the pixel
	
	float normalX{  };
	float normalY{  };
	if(screenShotMode == false)
	{
		normalX = (xpos + 0.5f) / m_width;
		normalY = (ypos + 0.5f) / m_height;
	}
	else
	{
		normalX = (xpos + 0.5f) / (m_highResWidth);
		normalY = (ypos + 0.5f) / (m_highResHeight);
		
	}
	if (screenShotMode == false)
		m_aspectRatio = m_width / m_height;
	else
		m_aspectRatio = m_highResWidth / m_highResHeight;
	
	// Calculate the
	float wX{ (2.f * normalX - 1.f) * m_aspectRatio * m_angle };
	float wY{ (1.f - 2.f * normalY) * m_angle };
	Vec3f raydir{ wX, wY, -1.f };
	
	Vec3f origin = {0.f,0.f,0.f};
	if (screenShotMode == true)
	{
		Vec3f hitToFocalPlane{};
		Shape* tempShapePointer{};
		focalPlane.IntersectionTest(Ray{ raydir,{0.f,0.f,0.f} },hitToFocalPlane,tempShapePointer);


		origin = raydir;
		origin.m_x += offsetX * aperture;
		origin.m_y += offsetY * aperture;
		raydir =   hitToFocalPlane - origin;
		
	}
	
	
	//Vec3f AdjustedOrigin{ offsetX, offsetY, 0.f };
	
	Vec4f direction{ raydir,0.f };
	raydir = (viewMatrix * direction).m_XYZ;
	raydir.normalize();

	 origin = { (viewMatrix * Vec4f{ origin,1.f }).m_XYZ };

	return Ray{ raydir,origin };
}

void Camera::updateViewMatrix()
{
	Vec3f z{ (m_cameraOrigin - m_lookAt).normalize() };
	z.m_x *= -1;
	z.m_y *= -1;
	Vec3f x{ (m_up.cross(z).normalize()) };
	Vec3f y{ z.cross(x).normalize() };
	viewMatrix =
	{
		Vec4f{x,0.f},
		Vec4f{y,0.f},
		Vec4f{z,0.f},
		Vec4f{0.f,0.f,0.f,1.f}
	};
}

void Camera::mouseMove(sf::Vector2<int> newMouse)
{
	sf::Vector2 < float > diff = static_cast<sf::Vector2<float>>(m_mousePos - newMouse);

	m_yaw += diff.x * 0.001f;
	m_pitch -= diff.y * 0.001f;

	if (m_pitch > 89.0f)
		m_pitch = 89.0f;
	if (m_pitch < -89.0f)
		m_pitch = -89.0f;
	Vec3f z{
		cosf(degreesToRads(m_yaw) * cosf(degreesToRads(m_pitch))),
		sinf(degreesToRads(m_pitch)),
		sinf(degreesToRads(m_yaw) * degreesToRads(m_pitch))
	};
	z.normalize();
	z.m_x *= -1;
	z.m_y *= -1;
	Vec3f x{ (m_up.cross(z).normalize()) };
	Vec3f y{ z.cross(x).normalize() };
	viewMatrix =
	{
		Vec4f{x,0.f},
		Vec4f{y,0.f},
		Vec4f{z,0.f},
		Vec4f{0.f,0.f,0.f,1.f}
	};
}

void Camera::rotateX(const float& degrees)
{
	Mat4x4 rotateXMat
	{
		Vec4f{1.f,0.f,0.f,0.f},
		Vec4f{0.f,cosf(degreesToRads(degrees)),-sinf(degreesToRads(degrees)),0.f},
		Vec4f{0.f,sinf(degreesToRads(degrees)),cosf(degreesToRads(degrees)),0.f},
		Vec4f{0.f,0.f,0.f,1.f}
	};

	//rotation = (rotation * rotateXMat );
	//updateMatrix();
	(viewMatrix = viewMatrix * rotateXMat);
}

void Camera::rotateY(const float& degrees)
{
	Mat4x4 rotateYMat
	{
		  Vec4f{cosf(degreesToRads(degrees)),  0.f,	sinf(degreesToRads(degrees)),		0.f},
		Vec4f{0.f,								1.f,	0.f,									0.f},
		Vec4f{-sinf(degreesToRads(degrees)), 0.f,	cosf(degreesToRads(degrees)),		0.f},
		Vec4f{0.f,								0.f,	0.f,									1.f}
	};
	//rotation = rotation * rotateYMat;
	//updateMatrix();
	viewMatrix = viewMatrix * rotateYMat;
}

void Camera::rotateZ(const float& degrees)
{
	Mat4x4 rotateZMat
	{
		Vec4f{cosf(degreesToRads(degrees)),-sinf(degreesToRads(degrees)),0.f,0.f},
		Vec4f{sinf(degreesToRads(degrees)),cosf(degreesToRads(degrees)),0.f,0.f},
		Vec4f{0.f,0.f,1.f,0.f},
		Vec4f{0.f,0.f,0.f,1.f}
	};
	//rotation = rotation * rotateZMat;
	//updateMatrix();
	viewMatrix = viewMatrix * rotateZMat;
}

void Camera::moveRelative(const Vec4f moveVector)
{
	Vec4f move = viewMatrix * moveVector;
	viewMatrix.m_X[3] += move[0] * 0.1f;
	viewMatrix.m_Y[3] += move[1] * 0.1f;
	viewMatrix.m_Z[3] += move[2] * 0.1f;
	
}

int Camera::getFOV()
{
	return m_fov;
}

void Camera::changeFOV(int offset)
{
	m_fov = clamp(m_fov + offset, 1, 179);
	m_angle = (tan((M_PI * m_fov / 180.f) / 2));
}

void Camera::changeFocalLenght(float offset)
{
	float change{ clamp( focalLenght + offset,-20.f,-2.f) };
	focalLenght = change;
	focalPlane.m_orgin.m_z = change;
}

void Camera::changeAppetureSize(float offset)
{
	float change{ clamp(aperture + offset,0.001f,0.5f) };
	aperture = change;
}
