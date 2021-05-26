#include "RayTracer.h"
#include <cassert>
#include "Utility.h"
#include "Light.h"
#include "clock.h"
#include "Mat4x4.h"
#include <thread>

#include "STB/stb_image_write.h"
#pragma warning(push)
#pragma warning(disable: 6386)
#pragma  warning(disable: 26451)

constexpr uint8_t MASK_moveLeft{ 0b0000'0001 }; // represents bit 0
constexpr uint8_t MASK_moveRight{ 0b0000'0010 }; // represents bit 1
constexpr uint8_t MASK_moveUp{ 0b0000'0100 }; // represents bit 2
constexpr uint8_t MASK_moveDown{ 0b0000'1000 }; // represents bit 3
constexpr uint8_t MASK_moveForward{ 0b0001'0000 }; // represents bit 4
constexpr uint8_t MASK_moveBack{ 0b0010'0000 }; // represents bit 5

constexpr uint8_t MASK_rotLeft{ 0b0000'0001 }; // represents bit 0
constexpr uint8_t MASK_rotRight{ 0b0000'0010 }; // represents bit 1
constexpr uint8_t MASK_rotUp{ 0b0000'0100 }; // represents bit 2
constexpr uint8_t MASK_rotDown{ 0b0000'1000 }; // represents bit 3
constexpr uint8_t MASK_rotTiltLeft{ 0b0001'0000 }; // represents bit 4
constexpr uint8_t MASK_rotTiltRight{ 0b0010'0000 }; // represents bit 5

RayTracer::RayTracer(int width, int height, const Camera& camera) :SFML(width, height), m_camera(camera), m_world(m_camera)
{
	m_camera.m_highResHeight = static_cast<float>(SFML.m_highResHeight);
	m_camera.m_highResWidth = static_cast<float>(SFML.m_highResWidth);
}
std::atomic<int32_t> jobsLeft{ -4 };
Status* threadStatuses;
Job** joblist;
Job** highResJobList;
float angle{};
void HandleJob(int threadID, sf::Uint8*& pixels, const int width, RayTracer& tracer)
{
	while (threadStatuses[threadID] != Status::EXIT)
	{
		while (jobsLeft >= 0)
		{
			threadStatuses[threadID] = Status::WORKING;
			int jobID{ jobsLeft-- };
			if (jobID < 0)
			{
				break;
			}

			Job* currentJob{  };
			if (tracer.screenShotMode == false)
				currentJob = joblist[abs(jobID)];
			else
				currentJob = highResJobList[abs(jobID)];

			assert(currentJob != nullptr);
			for (int y = currentJob->yPos; y < currentJob->yPos + currentJob->tileSize; ++y) {
				for (int x = currentJob->xPos; x < currentJob->xPos + currentJob->tileSize; ++x) {
					Color color = tracer.trace(tracer.m_camera.getRayAt(y, x, tracer.screenShotMode), 0);
					if (tracer.screenShotMode == false)
					{
						pixels[(y) * 4 * width + x * 4] = static_cast<sf::Uint8>(color.clamp(color.R) * 255);
						pixels[(y) * 4 * width + x * 4 + 1] = static_cast<sf::Uint8>(color.clamp(color.G) * 255);
						pixels[(y) * 4 * width + x * 4 + 2] = static_cast<sf::Uint8>(color.clamp(color.B) * 255);
						pixels[(y) * 4 * width + x * 4 + 3] = 255;
					}
					else
					{
						unsigned highResWidth{ tracer.SFML.m_highResWidth };
						tracer.SFML.pixelBuffer[(y) * 4 * highResWidth + x * 4] += color.clamp(color.R);
						tracer.SFML.pixelBuffer[(y) * 4 * highResWidth + x * 4 + 1] += color.clamp(color.G);
						tracer.SFML.pixelBuffer[(y) * 4 * highResWidth + x * 4 + 2] += color.clamp(color.B);
						tracer.SFML.pixelBuffer[(y) * 4 * highResWidth + x * 4 + 3] += 255;

						tracer.SFML.highResPixels[(y) * 4 * highResWidth + x * 4] = static_cast<sf::Uint8>((tracer.SFML.pixelBuffer[(y) * 4 * highResWidth + x * 4] / static_cast<float>(tracer.frameCountForPixelBuffer - 1)) * 255);
						tracer.SFML.highResPixels[(y) * 4 * highResWidth + x * 4 + 1] = static_cast<sf::Uint8>((tracer.SFML.pixelBuffer[(y) * 4 * highResWidth + x * 4 + 1] / static_cast<float>(tracer.frameCountForPixelBuffer -1)) * 255);
						tracer.SFML.highResPixels[(y) * 4 * highResWidth + x * 4 + 2] = static_cast<sf::Uint8>((tracer.SFML.pixelBuffer[(y) * 4 * highResWidth + x * 4 + 2] / static_cast<float>(tracer.frameCountForPixelBuffer - 1)) * 255);
						tracer.SFML.highResPixels[(y) * 4 * highResWidth + x * 4 + 3] = 255;
					}
				}
			}
		}
		threadStatuses[threadID] = Status::DONE;
	}
}
void RayTracer::RenderImage()
{
	int height{ SFML.m_height };
	int width{ SFML.m_width };
	numOfThreadsAvailable = std::thread::hardware_concurrency();
	threadStatuses = new Status[numOfThreadsAvailable]{ Status::WAITING };
	std::thread** threads = new std::thread * [numOfThreadsAvailable] {nullptr};
	for (unsigned i = 0; i < numOfThreadsAvailable; ++i)
	{
		threads[i] = new std::thread(HandleJob, i, std::ref(SFML.pixels), width, std::ref(*this));
	}

	//char result[10];

	int tileSize{ 8 };
	int numOfJobs{ static_cast<int>((width * height) / (tileSize * tileSize)) };
	int numOfHighResJobs{ static_cast<int>((SFML.m_highResWidth * SFML.m_highResHeight) / (tileSize * tileSize)) };
	int currentJob{ 0 };
	joblist = new Job * [numOfJobs] {nullptr};
	highResJobList = new Job * [numOfHighResJobs] {nullptr};
	for (int y = 0; y < height; y += tileSize)
	{
		for (int x = 0; x < width; x += tileSize)
		{
			joblist[currentJob] = new Job(x, y, tileSize);
			++currentJob;
		}
	}
	int currentHighResJob{ 0 };
	for (unsigned y = 0; y < SFML.m_highResHeight; y += tileSize)
	{
		for (unsigned x = 0; x < SFML.m_highResWidth; x += tileSize)
		{
			highResJobList[currentHighResJob] = new Job(x, y, tileSize);
			++currentHighResJob;
		}
	}

	//preCalculateNextFrame();
	while (exitApplication == false)
	{
		Clock timer{};
		deltaTime = clamp(deltaTime, 0.f, 1000.f / 15.f);
		//m_camera.m_cameraOrigin = { cosf(angle) * 10.f, 1.f,sinf(angle) * 10.f };
		//m_camera.updateViewMatrix();

		beginTick();
		m_world.buildBVH();

		if (brakeOnTraceBegin)
		{
			sf::Vector2i pos{ sf::Mouse::getPosition(SFML.getWindow()) };
			Ray debugRay(m_camera.getRayAt(pos.y, pos.x));
			Color debugColor = trace(debugRay, 0);
			continue;
		}
		//printf("Layers %ld \n", m_world.layers);

		for (unsigned i = 0; i < numOfThreadsAvailable; ++i)
			threadStatuses[i] = Status::GO;
		if (screenShotMode == false)
			jobsLeft = numOfJobs - 1;
		else
			jobsLeft = numOfHighResJobs - 1;

#if(PRINTINFO)
		printf("Time taken %f \n", timer.endTimer());
		printf("Average calculations per query: %llu \n", m_world.numOfTests / m_world.timesQueried);
		printf("Average number of intersection tests for early out %llu \n", m_world.earllyOutTests / m_world.timesQueriedEarlyOut);
		printf("Number of BVH layers %i \n", m_world.numOfLayers);
#endif

		while (true)
		{
			bool done{ true };
			for (unsigned i = 0; i < numOfThreadsAvailable; ++i)
			{
				if (threadStatuses[i] != Status::DONE || jobsLeft >= 0)
					done = false;
			}
			if (done)
				break;
		}

		//SFML.getWindow().setTitle(windowName + "100");
		deltaTime = static_cast<float>(timer.endTimer() * 1000.f);

		frameTimeCounter += deltaTime;
		++numberOfFrames;
		endTick();
		if (screenShotMode)
			++frameCountForPixelBuffer;
	}
	for (int i = 0; i < numOfJobs; ++i)
	{
		delete joblist[i];
		joblist[i] = nullptr;
	}
}

//Light light{ { 3.f,20.f,10.f } ,{1.f,1.f,1.f,1.f} };

Color RayTracer::trace(Ray ray, int depth)
{
	if (brakeOnTraceBegin)
	{
		brakeOnTraceBegin = false;
		__debugbreak();
	}

	Color visualize{};
	if (visualizeBVH)
		visualize = m_world.visualizeBox(ray);

	if (depth > maxDepth)
	{
		return { 0.f,0.f,0.f };
		//return { 12.f / 255.f,40.f / 255.f,120.f / 255.f };
	}

	Shape* closestShape{ nullptr };

	Vec3f intersectPoint{};

	// Get the closest object or if we hit nothing set the background color
	if (getClosestObject(ray, intersectPoint, closestShape) == false)
	{
		if (visualizeBVH)
		{
			if (visualize.R > 0.f || visualize.G > 0.f || visualize.B > 0.f)
				return  visualize;
		}

		return  _background;
	}
	Shape* workingShape{ nullptr };
	closestShape->IntersectionTest(ray, intersectPoint, workingShape);

	Vec3f normalVec{ closestShape->calculateNormal(intersectPoint) };
	normalVec.normalize();
	Color colorToPrint{};

	if (visualizeNormals == false)
	{
		colorToPrint = closestShape->getColor(intersectPoint);
		// Calculate shadows
		colorToPrint = calculateShadows(intersectPoint, normalVec, closestShape, colorToPrint, ray.m_orgin);

		// Reflection + Refraction can be a maximum of 1
		float fresnel{ 0.f };
		Color refractionColor = calculateRefraction(ray, intersectPoint, normalVec, closestShape, depth, fresnel);
		colorToPrint += refractionColor;
		if (closestShape->m_mat.reflectiveIndex > 0.f)
		{
			// Calculate reflections
			Color reflectionColor{ calulateReflection(ray.m_direction, intersectPoint, normalVec, closestShape, depth) };

			reflectionColor = reflectionColor * closestShape->m_mat.reflectiveIndex;

			colorToPrint += reflectionColor;
		}
	}
	else
		colorToPrint = { normalVec.m_x * 0.5f + 0.5f ,normalVec.m_y * 0.5f + 0.5f,normalVec.m_z < 0 ? normalVec.m_z * -1.f * 0.5f + 0.5f : normalVec.m_z * 0.5f + 0.5f };

	if (visualizeBVH)
	{
		if (depth == 0)
			colorToPrint += visualize;
	}
	// Clamp the color
	colorToPrint.clamp();

	return colorToPrint;
}

Vec3f RayTracer::reflect(const Vec3f& I, const Vec3f& N)
{
	return I - 2.f * I.dot(N)* N;
}

Color RayTracer::calculateShadows(const Vec3f& intersectPoint, const Vec3f& normalVector, const Shape* closestShape, const Color& beginColor, const Vec3f& viewLocation)
{
	assert(closestShape != nullptr);
	Color colorResult{};
	Vec3f Lookingat{ viewLocation - intersectPoint };
	Lookingat.normalize();
	for (int i = 0; i < m_world.scene->m_numberOfLights; ++i)
	{
		Vec3f lightPos{ m_world.scene->lights[i]->m_lightLocation };
		if (screenShotMode)
		{
			float offsetX = cosf(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * M_PI))))* m_world.scene->lights[i]->m_area;
			float offsetZ = sinf(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * M_PI))))* m_world.scene->lights[i]->m_area;
			lightPos.m_x += offsetX;
			lightPos.m_z += offsetZ;
		}

		Vec3f lightDirection{ lightPos - (intersectPoint) };
		float lambert{ normalVector.dot(lightDirection) };
		if (lambert > epsilon)
		{
			float result{};

			float attenuation{ lightDirection.lenght2() };
			lightDirection.normalize();
			Ray shadowRay{ lightDirection ,intersectPoint + normalVector * 1e-4f ,{} };
			bool lightVisable{ true };

			float maxDistance{ (m_world.scene->lights[i]->m_lightLocation - shadowRay.m_orgin).lenght2() };
			if (m_world.getAIntersection(shadowRay, maxDistance))
			{
				lightVisable = false;
			}

			Vec3f Half{ Lookingat + lightDirection };
			Half.normalize();
			float specular{ powf(max(0.f, (Half.dot(normalVector))),200) };
			result = (lightVisable * (m_world.scene->lights[i]->m_lightIntensity / attenuation) * lambert * closestShape->m_mat.albedo);
			colorResult += (m_world.scene->lights[i]->m_lightColor * result);
			// Specular light
			colorResult += m_world.scene->lights[i]->m_lightColor * specular * (m_world.scene->lights[i]->m_lightIntensity / attenuation) * lightVisable;
		}
	}
	// Ambient lighting
	colorResult += Color{ 1.f,1.f,1.f } *0.1f;
	return { beginColor.R * colorResult.R , beginColor.G * colorResult.G,beginColor.B * colorResult.B };
}

Color RayTracer::calulateReflection(const Vec3f& rayDirection, const Vec3f& intersectPoint, const Vec3f& normalVector, const Shape* closestShape, int depth)
{
	assert(closestShape != nullptr);

	if (closestShape->m_mat.reflectiveIndex > 0.f)
	{
		bool outside{ rayDirection.dot(normalVector) < 0 };

		Vec3f reflectRayDirection{ reflect(rayDirection,normalVector) };
		reflectRayDirection.normalize();
		Vec3f ReflectRayOrgin{ outside ? intersectPoint + normalVector * 1e-4f : intersectPoint - normalVector * 1e-4f };
		Ray reflectiveRay{ reflectRayDirection,ReflectRayOrgin,{} };
		Color test = (trace(reflectiveRay, depth + 1));
		if (closestShape->m_mat.reflectiveIndex > 0)
			test = test * closestShape->m_mat.reflectiveIndex;
		else
			test = _background;

		return test;
	}
	else return Color{ 0.f,0.f,0.f };
}

Vec3f RayTracer::refract(const Ray& ray, const Vec3f& normal, const Shape* closestShape)
{
	

	float cosi = clamp(-1.f, 1.f, ray.m_direction.dot(normal));
	float etai = 1.f, etat = closestShape->m_mat.refractionIndex;
	Vec3f n = normal;
	if (cosi < 0) { cosi = -cosi; }
	else { swap(etai, etat); n = -1.f * normal; }
	float eta = etai / etat;
	float k = 1.f - eta * eta * (1.f - cosi * cosi);
	return k < 0.f ? 0.f : eta * ray.m_direction + (eta * cosi - sqrtf(k)) * n;
}

float RayTracer::Calculatefresnel(const Ray& ray, const Vec3f& normal, const Shape* closestShape)
{
	float cosi = clamp(-1.f, 1.f, ray.m_direction.dot(normal));
	float etai = 1.f, etat = closestShape->m_mat.refractionIndex;
	if (cosi > 0) { swap(etai, etat); }
	// Compute sini using Snell's law
	float sint = etai / etat * sqrtf(std::max(0.f, 1.f - cosi * cosi));
	// Total internal reflection
	if (sint >= 1.f) {
		return 1.f;
	}
	else {
		float cost = sqrtf(std::max(0.f, 1.f - sint * sint));
		cosi = fabsf(cosi);
		float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		return  (Rs * Rs + Rp * Rp) / 2.f;
	}
}

Color RayTracer::calculateRefraction(const Ray& ray, const Vec3f& intersectPoint, const Vec3f& normalVector,
	Shape* closestShape, int depth, float& fresnel)
{
	assert(closestShape != nullptr);

	Color RefractionColor{};
	Color ReflectionColor{};
	if (closestShape->m_mat.refractionIndex > 0)
	{
		fresnel = Calculatefresnel(ray, normalVector, closestShape);
		bool outside{ ray.m_direction.dot(normalVector) < 0 };
		if (fresnel < 1.f )
		{
			// get the transmission Ray
			Vec3f TransmissionRay{ refract(ray,normalVector,closestShape) };
/*			if (TransmissionRay.m_x < epsilon && TransmissionRay.m_x > -epsilon &&
				TransmissionRay.m_y < epsilon && TransmissionRay.m_y > -epsilon &&
				TransmissionRay.m_z < epsilon && TransmissionRay.m_z >-epsilon
				)
			{
			
			}
			else*/ {
				TransmissionRay.normalize();


				// Make the actual refraction Ray
				Ray refractionRay{ TransmissionRay, outside ? intersectPoint - normalVector * 1e-4f : intersectPoint + normalVector * 1e-4f };

				RefractionColor = trace(refractionRay, depth + 1);
			}
		}
		Vec3f reflectionDirection = reflect(ray.m_direction, normalVector).normalize();
		Vec3f reflectionRayOrig{ outside ? intersectPoint - normalVector * 1e-4f : intersectPoint + normalVector * 1e-4f };
		ReflectionColor = trace(Ray{reflectionDirection,reflectionRayOrig}, depth + 1);

		return ReflectionColor * fresnel + RefractionColor * (1.f - fresnel);
	}
	return  { 0.f,0.f,0.f };
}

bool RayTracer::getClosestObject(const Ray& ray, Vec3f& intersectPoint, Shape*& closestShape)
{
	closestShape = m_world.queryIntersection(ray, intersectPoint);
	if (closestShape != nullptr)
		return true;
	return false;
}

void RayTracer::handleInput()
{
	while (SFML.getWindow().pollEvent(event))
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		{
			brakeOnTraceBegin = true;
		}

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::D:
				moveInfoMASK &= ~MASK_moveRight;
				break;
			case sf::Keyboard::A:
				moveInfoMASK &= ~MASK_moveLeft;
				break;
			case sf::Keyboard::W:
				moveInfoMASK &= ~MASK_moveForward;
				break;
			case sf::Keyboard::S:
				moveInfoMASK &= ~MASK_moveBack;
				break;
			case sf::Keyboard::Q:
				moveInfoMASK &= ~MASK_moveDown;
				break;
			case sf::Keyboard::E:
				moveInfoMASK &= ~MASK_moveUp;
				break;
			case sf::Keyboard::Numpad4:
				rotationInfoMASK &= ~MASK_rotLeft;
				break;

			case sf::Keyboard::Numpad6:
				rotationInfoMASK &= ~MASK_rotRight;
				break;
			case sf::Keyboard::Numpad8:
				rotationInfoMASK &= ~MASK_moveDown;
				break;

			case sf::Keyboard::Numpad5:
				rotationInfoMASK &= ~MASK_moveUp;
				break;
			case sf::Keyboard::Numpad7:
				rotationInfoMASK &= ~MASK_rotTiltLeft;
				break;

			case sf::Keyboard::Numpad9:
				rotationInfoMASK &= ~MASK_rotTiltRight;
				break;
			}
		}

		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::F1:
				SFML.showInfoText = !SFML.showInfoText;
				break;
			case sf::Keyboard::F2:
				SFML.showStats = !SFML.showStats;
				break;

			case sf::Keyboard::F5:
			{
				Scenes next = (static_cast<Scenes>(static_cast<int>(m_world.currentScene) - 1));
				if (next == Scenes::BEGIN)
					next = (static_cast<Scenes>(static_cast<int>(Scenes::MAX) - 1));
				m_world.loadScene(next, m_camera);
				break;
			}
			case sf::Keyboard::F6:
			{
				Scenes next = (static_cast<Scenes>(static_cast<int>(m_world.currentScene) + 1));
				if (next == Scenes::MAX)
					next = (static_cast<Scenes>(static_cast<int>(Scenes::BEGIN) + 1));
				m_world.loadScene(next, m_camera);

				break;
			}
			case sf::Keyboard::Add:
				movementSpeedModifier = clamp(movementSpeedModifier + 0.1f, 0.f, 10.f);
				break;
			case sf::Keyboard::Subtract:
				movementSpeedModifier = clamp(movementSpeedModifier - 0.1f, 0.f, 10.f);
				break;
			case sf::Keyboard::PageUp:
				m_camera.changeFOV(1);
				break;
			case sf::Keyboard::PageDown:
				m_camera.changeFOV(-1);
				break;
			case sf::Keyboard::A:
				moveInfoMASK |= MASK_moveLeft;
				break;
			case sf::Keyboard::D:
				moveInfoMASK |= MASK_moveRight;
				break;
			case sf::Keyboard::W:
				moveInfoMASK |= MASK_moveForward;
				break;

			case sf::Keyboard::S:
				moveInfoMASK |= MASK_moveBack;
				break;
			case sf::Keyboard::Q:
				moveInfoMASK |= MASK_moveDown;
				break;

			case sf::Keyboard::E:
				moveInfoMASK |= MASK_moveUp;
				break;
			case sf::Keyboard::Numpad4:
				rotationInfoMASK |= MASK_rotLeft;
				break;

			case sf::Keyboard::Numpad6:
				rotationInfoMASK |= MASK_rotRight;
				break;
			case sf::Keyboard::Numpad8:
				rotationInfoMASK |= MASK_moveDown;
				break;

			case sf::Keyboard::Numpad5:
				rotationInfoMASK |= MASK_moveUp;
				break;
			case sf::Keyboard::Numpad7:
				rotationInfoMASK |= MASK_rotTiltLeft;
				break;

			case sf::Keyboard::Numpad9:
				rotationInfoMASK |= MASK_rotTiltRight;
				break;
			case sf::Keyboard::Numpad1:
				maxDepth = max(maxDepth - 1, 0);
				break;
			case sf::Keyboard::Numpad2:
				maxDepth = maxDepth + 1;
				break;
			case sf::Keyboard::N:
				visualizeNormals = !visualizeNormals;
				break;
			case sf::Keyboard::M:
				visualizeBVH = !visualizeBVH;
				break;
			case sf::Keyboard::Up:
				m_camera.changeFocalLenght(-0.1f);
				if (screenShotMode == true)
				{
					frameCountForPixelBuffer = 1;
					memset(SFML.pixelBuffer, 0, SFML.m_highResWidth * SFML.m_highResHeight * 4 * sizeof(float));
				}
				break;
			case sf::Keyboard::Down:
				m_camera.changeFocalLenght(0.1f);
				if (screenShotMode == true)
				{
					frameCountForPixelBuffer = 1;
					memset(SFML.pixelBuffer, 0, SFML.m_highResWidth * SFML.m_highResHeight * 4 * sizeof(float));
				}
				break;
			case sf::Keyboard::Left:
				m_camera.changeAppetureSize(-0.001f);
				if (screenShotMode == true)
				{
					frameCountForPixelBuffer = 1;
					memset(SFML.pixelBuffer, 0, SFML.m_highResWidth* SFML.m_highResHeight * 4 * sizeof(float));
				}
				break;
			case sf::Keyboard::Right:
				m_camera.changeAppetureSize(0.001f);
				if (screenShotMode == true)
				{
					frameCountForPixelBuffer = 1;
					memset(SFML.pixelBuffer, 0, SFML.m_highResWidth* SFML.m_highResHeight * 4 * sizeof(float));
				}
				break;

			case sf::Keyboard::V:
				screenShotMode = !screenShotMode;
				if (screenShotMode == false)
				{
					frameCountForPixelBuffer = 1;
					memset(SFML.pixelBuffer, 0, SFML.m_highResWidth * SFML.m_highResHeight * 4 * sizeof(float));
					SFML.getWindow().setSize(sf::Vector2u{ static_cast<unsigned>(SFML.m_width),static_cast<unsigned>(SFML.m_height) });
				}
				else
				{
					SFML.getWindow().setSize({ SFML.m_highResWidth, SFML.m_highResHeight });
					SFML.getWindow().setPosition({ 0,0 });
				}

				break;

			case sf::Keyboard::F9:
				m_world.testDepth += 1;
				break;
			case sf::Keyboard::F10:
				m_world.testDepth = max(m_world.testDepth - 1, 0);
				break;
			case sf::Keyboard::Escape:
				SFML.getWindow().close();
				exitApplication = true;
				for (unsigned i = 0; i < numOfThreadsAvailable; ++i)
					threadStatuses[i] = Status::EXIT;

				break;
			case sf::Keyboard::C:
				std::string filename{ "screenshots/screenshot" + std::to_string(screenshotIndex) + ".png" };
				if(screenShotMode == false)
				stbi_write_png(filename.c_str(), SFML.m_width, SFML.m_height, 4, static_cast<uint8_t*>(SFML.pixels), SFML.m_width * 4);
				else
				stbi_write_png(filename.c_str(), SFML.m_highResWidth, SFML.m_highResHeight, 4, static_cast<uint8_t*>(SFML.highResPixels), SFML.m_highResWidth * 4);
				++screenshotIndex;
				break;
			}
		}
		if (event.type == sf::Event::Closed)
		{
			SFML.getWindow().close();
			exitApplication = true;
			for (unsigned i = 0; i < numOfThreadsAvailable; ++i)
			{
				threadStatuses[i] = Status::EXIT;
			}
		}
	}
}

void RayTracer::beginTick()
{
	if (screenShotMode == false)
	{
		if ((moveInfoMASK & MASK_moveRight) == MASK_moveRight)
			m_camera.moveRelative(Vec4f{ 1.f, 0.f, 0.f, 0.f } *(deltaTime * movementSpeed * movementSpeedModifier));
		if ((moveInfoMASK & MASK_moveLeft) == MASK_moveLeft)
			m_camera.moveRelative(Vec4f{ -1.f, 0.f, 0.f, 0.f } *(deltaTime * movementSpeed * movementSpeedModifier));
		if ((moveInfoMASK & MASK_moveForward) == MASK_moveForward)
			m_camera.moveRelative(Vec4f{ 0.f, 0.f, -1.f, 0.f } *deltaTime * movementSpeed * movementSpeedModifier);
		if ((moveInfoMASK & MASK_moveBack) == MASK_moveBack)
			m_camera.moveRelative(Vec4f{ 0.f, 0.f, 1.f, 0.f } *deltaTime * movementSpeed * movementSpeedModifier);
		if ((moveInfoMASK & MASK_moveDown) == MASK_moveDown)
			m_camera.moveRelative(Vec4f{ 0.f, -1.f, 0.f, 0.f } *deltaTime * movementSpeed * movementSpeedModifier);
		if ((moveInfoMASK & MASK_moveUp) == MASK_moveUp)
			m_camera.moveRelative(Vec4f{ 0.f, 1.f, 0.f, 0.f } *deltaTime * movementSpeed * movementSpeedModifier);

		if ((rotationInfoMASK & MASK_rotLeft) == MASK_rotLeft)
			m_camera.rotateY(1 * deltaTime * rotationSpeed * rotationSpeedModifier);
		if ((rotationInfoMASK & MASK_rotRight) == MASK_rotRight)
			m_camera.rotateY(-1 * deltaTime * rotationSpeed * rotationSpeedModifier);
		if ((rotationInfoMASK & MASK_rotDown) == MASK_rotDown)
			m_camera.rotateX(1 * deltaTime * rotationSpeed * rotationSpeedModifier);
		if ((rotationInfoMASK & MASK_rotUp) == MASK_rotUp)
			m_camera.rotateX(-1 * deltaTime * rotationSpeed * rotationSpeedModifier);
		if ((rotationInfoMASK & MASK_rotTiltLeft) == MASK_rotTiltLeft)
			m_camera.rotateZ(1 * deltaTime * rotationSpeed * rotationSpeedModifier);
		if ((rotationInfoMASK & MASK_rotTiltRight) == MASK_rotTiltRight)
			m_camera.rotateZ(-1 * deltaTime * rotationSpeed * rotationSpeedModifier);
		m_world.scene->tick(deltaTime);
	}
}

void RayTracer::endTick()
{
	m_world.scene->endTick(deltaTime);

	SFML.setStats(static_cast<float>(frameTimeCounter / numberOfFrames), m_camera, movementSpeedModifier, maxDepth, visualizeNormals, m_world.currentScene, visualizeBVH, m_world.testDepth, screenShotMode, m_camera.getFocalLenght(), m_camera.getApeture());
	if (frameTimeCounter > 1000)
	{
		frameTimeCounter = 0;
		numberOfFrames = 0;
	}
	SFML.updateImage();
	SFML.updateWindow(screenShotMode);
	m_world.clearBVH();
	//SFML.lockWindow();

	handleInput();
}

#pragma warning(pop)