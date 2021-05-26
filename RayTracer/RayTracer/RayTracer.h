#pragma once
#include "vec3f.h"
#include "Ray.h"
#include "SFMLWrapper.h"
#include "shapes.h"
#include "Light.h"
#include "Camera.h"
#include "World.h"
#include "atomic"
#include "Job.h"

class RayTracer
{
public:
	RayTracer(int width, int height, const Camera& camera);

	void RenderImage();

	
	Color trace(Ray ray, int depth);
	SFMLWrapper SFML;
	Vec3f m_cameraLocation{0.f,1.f,0.f};
	Camera m_camera;
	bool screenShotMode{ false };
	int frameCountForPixelBuffer{1};
private:
	
	static Vec3f reflect(const Vec3f& I, const Vec3f& N);
	static Vec3f refract(const Ray& ray, const Vec3f& normal, const Shape* closestShape);
	static float Calculatefresnel(const Ray& ray, const Vec3f& normal, const Shape* closestShape);
	Color calculateShadows(const Vec3f& intersectPoint, const Vec3f& normalVector, const Shape* closestShape, const Color& beginColor, const Vec3f& viewLocation);
	Color calulateReflection(const Vec3f& rayDirection, const Vec3f& intersectPoint, const Vec3f& normalVector, const Shape* closestShape, int depth);
	Color calculateRefraction(const Ray& ray, const Vec3f& intersectPoint, const Vec3f& normalVector,  Shape* closestShape, int depth, float& fresnel);
	bool getClosestObject(const Ray& ray, Vec3f& intersectPoint, Shape* &closestShape);

	void handleInput();

	void beginTick();
	void endTick();



	


	sf::String windowName{ "Levi's ray tracer         Percent done: " };
	World m_world;
	bool exitApplication{false};
	unsigned numOfThreadsAvailable{};


	double frameTimeCounter{};
	double numberOfFrames{};

	float deltaTime{ 1 / 30.f };
	float movementSpeedModifier{ 1.f };
	float rotationSpeedModifier{ 1.f };
	float movementSpeed{ 0.025f };
	float rotationSpeed{ 0.025f };
	bool visualizeNormals{ false };
	bool visualizeBVH{false};
	
	int maxDepth{ 3 };
	int screenshotIndex{};
	sf::Event event{};
	

	bool brakeOnTraceBegin{};
	uint8_t moveInfoMASK{};
	uint8_t rotationInfoMASK{};
	Color _background{ 12.f / 255.f,40.f / 255.f,120.f / 255.f };
	
};


