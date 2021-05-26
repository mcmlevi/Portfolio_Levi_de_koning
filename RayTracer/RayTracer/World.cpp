#include "World.h"
#include <cassert>
#include "Utility.h"
#include "testScene.h"
#include "week5Scene.h"
#include "CornelBoxScene.h"
#include "Camera.h"
#include "JBVHnode.h"
#include "fakeScene.h"
#include "week8Reference.h"
#include "meshTestScene.h"


#pragma warning(push)
#pragma warning(disable: 26451)
World::World(Camera& camera)
{
	srand(static_cast<unsigned>(time(NULL)));

	loadScene(Scenes::testScene, camera);
}

World::~World()
{
	delete scene;
	scene = nullptr;
}

Shape* World::queryIntersection(const Ray& ray, Vec3f& hitPoint)
{
#if (PRINTINFO)
	timesQueried += 1;
#endif

	Shape* closestShape{ nullptr };
	float distance{ INFINITY };
	float distance2{ INFINITY };
	if (root != nullptr)
		closestShape = GetClosestShape(ray, *root, distance, distance2);

	//		for (int i = 0; i < scene->numOfShapes; ++i)
	//		{
	//			Vec3f hit{};
	//#if (PRINTINFO)
	//			numOfTests++;
	//#endif
	//
	//			if (scene->shapes[i]->IntersectionTest(ray, hit))
	//			{
	//				float lenght{ (ray.m_orgin - hit).lenght2() };
	//				if (lenght < distance)
	//				{
	//					distance = lenght;
	//					closestShape = scene->shapes[i];
	//					hitPoint = hit;
	//				}
	//			}
	//float distanceToObject{ INFINITY };
	//if (closestShape)
	//{
	//	closestShape->IntersectionTest(ray, hitPoint);
	//	distanceToObject = (ray.m_orgin - hitPoint).lenght2();
	//}

	for (int i = 0; i < numOfNonBVHShapes; ++i)
	{
		Vec3f tempHit{};
		Shape* workingShape{ nullptr };
		if (nonBVHShapes[i]->IntersectionTest(ray, tempHit,workingShape))
		{
			if ((ray.m_orgin - tempHit).lenght2() < distance2 )
			{
				closestShape = workingShape;
				hitPoint = tempHit;
			}
		}
	}

	return  closestShape;
}

bool World::getAIntersection(const Ray& ray, const float& maxDistance)
{
#if (PRINTINFO)
	timesQueriedEarlyOut++;
#endif

	{
		float distance{ INFINITY };
		if (numOfBVHShapes > 0)
			traverseWithEarlyOut(ray, *root, distance, maxDistance);

		if (distance != INFINITY)
			return  true;

		Vec3f hit{};
		for (int i = 0; i < numOfNonBVHShapes; ++i)
		{
#if (PRINTINFO)
			earllyOutTests++;
#endif
			Shape* workingShape{ nullptr };
			if (nonBVHShapes[i]->IntersectionTest(ray, hit,workingShape))
			{
				if ((ray.m_orgin - hit).lenght2() < maxDistance && workingShape->m_mat.refractionIndex < epsilon)
					return true;
			}
	}
		return false;
}
}

Color World::visualizeBox(const Ray& ray)
{
	boxDepth = 0;
	if (scene->numOfShapes > minObjectForBVH || FORCEBVH)
	{
		float distance{ INFINITY };

		int depth{};

		getEdgeIntersection(ray, *root, distance, depth, 1);

		float scale{ static_cast<float>(depth) / static_cast<float>(numOfBVHShapes * 2.f + 1.f) };
		//scale += 0.5f;
		Color color{ 1.f - scale,scale,scale };
		color.clamp();

		if (distance != INFINITY)
		{
			return 	color;
		}
		else
			return { 0.f,0.f,0.f };
	}
	return { 0.f,0.f,0.f };
}

Shape* World::GetClosestShape(const Ray& ray, JBVNode& currentNode, float& boxDistance, float& objectDistance)
{
	float temp{};
	if (currentNode.boundingBox.BoundingVolumeTest(ray, temp) == false)
		return nullptr;

	if (currentNode.count != 0)
	{
		Shape* closest{ nullptr };
		for (int i = 0; i < currentNode.count; ++i)
		{
			int index = i + currentNode.left;
			if (currentNode.left > numOfBVHShapes)
				return closest;
			
			Vec3f hit{};
			Shape* workingShape{ nullptr };
		if (sortedObjects[index]->IntersectionTest(ray, hit,workingShape))
			{
				float distance{ (ray.m_orgin - hit).lenght2() };
				if (distance < objectDistance)
				{
					objectDistance = distance;
					closest = workingShape;
				}
			}
		}
		return closest;
	}
	else
	{
		float distance1{ INFINITY };
		float distance2{ INFINITY };
		Shape* shape1{};
		Shape* shape2{};

		if (pool[currentNode.left]->boundingBox.BoundingVolumeTest(ray, distance1) == false)
			shape1 = nullptr;
		if (pool[currentNode.left + 1]->boundingBox.BoundingVolumeTest(ray, distance2) == false)
			shape2 = nullptr;

		if (distance1 == INFINITY && distance2 == INFINITY)
			return nullptr;

		if (distanceBetween(distance1, distance2) <= epsilon || pool[currentNode.left]->boundingBox.overlap(pool[currentNode.left + 1]->boundingBox))
		{
		shape1 = GetClosestShape(ray, *pool[currentNode.left], boxDistance, objectDistance);

		shape2 = GetClosestShape(ray, *pool[currentNode.left + 1], boxDistance, objectDistance);
		}
		else
		{
			if(distance1 < 0 && distance2 < 0)
			{
				distance1 *= -1;
				distance2 *= -1;
			}
			if (distance1 < distance2)
			{
				shape1 = GetClosestShape(ray, *pool[currentNode.left], boxDistance, objectDistance);

				if (shape1 == nullptr)
					shape2 = GetClosestShape(ray, *pool[currentNode.left + 1], boxDistance, objectDistance);
			}
			else if (distance2 <= distance1)
			{
				shape1 = GetClosestShape(ray, *pool[currentNode.left +1], boxDistance, objectDistance);

				if (shape1 == nullptr)
					shape2 = GetClosestShape(ray, *pool[currentNode.left], boxDistance, objectDistance);
			}
		
		}

		if (shape1 == nullptr && shape2 != nullptr)
		{
			swap(shape1, shape2);
		}

		if (shape1 != nullptr && shape2 != nullptr)
		{
			Vec3f hit1{ INFINITY };
			Vec3f hit2{ INFINITY };
			Shape* workingShape0{ nullptr };
			Shape* workingShape1{ nullptr };
			shape1->IntersectionTest(ray, hit1,workingShape0);
			shape2->IntersectionTest(ray, hit2,workingShape1);
			float distanceToPoint1{ (ray.m_orgin - hit1).lenght2() };
			float distanceToPoint2{ (ray.m_orgin - hit2).lenght2() };
			if (distanceToPoint2 < distanceToPoint1)
			{
				swap(shape1, shape2);
			}
		}

		return shape1;
	}
}

void World::traverseWithEarlyOut(const Ray& ray, JBVNode& currentNode, float& objectDistance, const float maxDistance)
{
	//assert(currentNode.left <= numOfBVHShapes);
	if(currentNode.left > numOfBVHShapes)
		return;
	
	float temp{};
	if (currentNode.boundingBox.BoundingVolumeTest(ray, temp) == false)
		return;

	if (currentNode.count != 0)
	{
		for (int i = 0; i < currentNode.count; ++i)
		{
			int index = i + currentNode.left;
			Vec3f hit{};
			Shape* workingShape{ nullptr };
			if (sortedObjects[index]->IntersectionTest(ray, hit,workingShape))
			{
				if ((ray.m_orgin - hit).lenght2() < maxDistance && workingShape->m_mat.refractionIndex < epsilon)
				{
					objectDistance = 0;
					return;
				}
			}
		}
	}
	else
	{
		traverseWithEarlyOut(ray, *pool[currentNode.left], objectDistance, maxDistance);

		if (objectDistance != INFINITY)
			return;

		traverseWithEarlyOut(ray, *pool[currentNode.left + 1], objectDistance, maxDistance);
	}
}

void World::getEdgeIntersection(const Ray& ray, JBVNode& currentNode, float& objectDistance, int& depthCounter, int depth)
{
	if (objectDistance != INFINITY)
		return;
	if (depth > testDepth)
		return;
	if (depth == testDepth)
	{
		if (currentNode.boundingBox.GetEdges(ray))
		{
			depthCounter = depth;
			objectDistance = 0;
			return;
		}
	}
	if (currentNode.count != 0)
		return;

	getEdgeIntersection(ray, *pool[currentNode.left], objectDistance, depthCounter, depth + 1);
	if (objectDistance != INFINITY)
		return;
	getEdgeIntersection(ray, *pool[currentNode.left + 1], objectDistance, depthCounter, depth + 1);
}


AABBox World::getBounds(int first, int count)
{
	float pos[6]
	{
		INFINITY,
		INFINITY,
		-INFINITY,
		-INFINITY,
		-INFINITY,
		INFINITY
	};
	for (int i = 0; i < count; ++i)
	{
		int index = i + first;
		AABBox* box{ boxes[index] };
		if (box->bounds[0] < pos[0])
			pos[0] = box->bounds[0];

		if (box->bounds[1] < pos[1])
			pos[1] = box->bounds[1];

		if (box->bounds[2] > pos[2])
			pos[2] = box->bounds[2];

		if (box->bounds[3] > pos[3])
			pos[3] = box->bounds[3];

		if (box->bounds[4] > pos[4])
			pos[4] = box->bounds[4];

		if (box->bounds[5] < pos[5])
			pos[5] = box->bounds[5];
	}
	return AABBox{ {pos[0],pos[1],pos[2]},{pos[3],pos[4],pos[5]} };
}
bool horizontal{ true };
void World::Subdivide(JBVNode& currentNode, int first)
{
	if (currentNode.count < maxObjectsInLeaf)
	{
		currentNode.left = first;
		return;
	}
	layers++;
	currentNode.left = poolptr ++;
	poolptr++;
	// Order the objects based on the split plane
	bool succes{ true };
	int pivotIndex{ Partition(first, currentNode.count,succes) };

	if (succes == false)
	{
		return;
	}

	// set left node info
	JBVNode* left{ pool[currentNode.left] };
	int leftFirst = first;
	left->count = pivotIndex;
	left->boundingBox = getBounds(leftFirst, left->count);
	// set right node info
	JBVNode* right{ pool[currentNode.left + 1] };
	int rightFirst = first + pivotIndex;
	right->count = currentNode.count - pivotIndex;
	right->boundingBox = getBounds(rightFirst, right->count);
	assert(left->left <= numOfBVHShapes && right->left <= numOfBVHShapes);
	Subdivide(*pool[currentNode.left], leftFirst);
	Subdivide(*pool[currentNode.left + 1], rightFirst);
	
	currentNode.count = 0;
}

enum class axis { X, Y, Z };
int World::Partition(int first, int count, bool& succes)
{
	Shape** objects = new Shape * [count];
	AABBox** boxesPartition = new AABBox * [count];
	float minX{ INFINITY };
	float maxX{ -INFINITY };
	float minY{ INFINITY };
	float maxY{ -INFINITY };
	float minZ{ -INFINITY };
	float maxZ{ INFINITY };
	Vec3f averagePoint{};
	for (int i = 0; i < count; ++i)
	{
		const int index{ first + i };
		Vec3f centerPoint{ boxes[index]->getCenter() };
		if (centerPoint.m_x < minX)
			minX = centerPoint.m_x;
		else if (centerPoint.m_x > maxX)
			maxX = centerPoint.m_x;
		if (centerPoint.m_y < minY)
			minY = centerPoint.m_y;
		else if (centerPoint.m_y > maxY)
			maxY = centerPoint.m_y;
		if (centerPoint.m_z > minZ)
			minZ = centerPoint.m_z;
		else if (centerPoint.m_z < maxZ)
			maxZ = centerPoint.m_z;

		averagePoint += centerPoint;
	}
	averagePoint /= static_cast<const float>(count);

	float x{ averagePoint.m_x };
	float y{ averagePoint.m_y };
	float z{ averagePoint.m_z };
	int left{};
	int right{ count - 1 };
	float distance{ -INFINITY };
	axis sortingAxis{};
	if (distanceBetween(minX, maxX) > distance)
	{
		sortingAxis = axis::X;
		distance = distanceBetween(minX, maxX);
	}
	if (distanceBetween(minY, maxY) > distance)
	{
		sortingAxis = axis::Y;
		distance = distanceBetween(minY, maxY);
	}
	if (distanceBetween(minZ, maxZ) > distance)
	{
		sortingAxis = axis::Z;
		distance = distanceBetween(minZ, maxZ);
	}
	if (distance < epsilon)
	{
		succes = false;
		return left;
	}
	for (int i = 0; i < count; ++i)
	{
		const int index = i + first;
		if (sortingAxis == axis::Y)
		{
			if (boxes[index]->getCenter().m_y < y)
			{
				objects[left] = sortedObjects[index];
				boxesPartition[left] = boxes[index];
				left++;
			}
			else
			{
				objects[right] = sortedObjects[index];
				boxesPartition[right] = boxes[index];
				right--;
			}
		}
		if (sortingAxis == axis::X)
		{
			if (boxes[index]->getCenter().m_x < x)
			{
				objects[left] = sortedObjects[index];
				boxesPartition[left] = boxes[index];
				left++;
			}
			else
			{
				objects[right] = sortedObjects[index];
				boxesPartition[right] = boxes[index];
				right--;
			}
		}
		if (sortingAxis == axis::Z)
		{
			if (boxes[index]->getCenter().m_z < z)
			{
				objects[left] = sortedObjects[index];
				boxesPartition[left] = boxes[index];
				left++;
			}
			else
			{
				objects[right] = sortedObjects[index];
				boxesPartition[right] = boxes[index];
				right--;
			}
		}
	}


	for (int i = 0; i < count; ++i)
	{
		sortedObjects[i + first] = objects[i];
		boxes[i + first] = boxesPartition[i];
	}

	delete[] objects;
	delete[] boxesPartition;

	return left;
}

void World::buildBVH()
{
	if (scene->numOfShapes > minObjectForBVH)
	{
		sortedObjects = new Shape * [scene->numOfShapes];
		nonBVHShapes = new Shape * [scene->numOfShapes];
		boxes = new AABBox * [scene->numOfShapes];

		int currentSorted{};
		int currentUnsorted{};
		for (int i = 0; i < scene->numOfShapes; ++i)
		{
			AABBox* box = scene->shapes[i]->getBoundingVolume();
			if (box != nullptr)
			{
				boxes[currentSorted] = box;
				sortedObjects[currentSorted++] = scene->shapes[i];
			}

			if (box == nullptr)
				nonBVHShapes[currentUnsorted++] = scene->shapes[i];
		}
		numOfBVHShapes = (currentSorted);
		numOfNonBVHShapes = (currentUnsorted);

		if (numOfBVHShapes)
		{
			pool = new JBVNode * [numOfBVHShapes * 2 + 1]{};
			for (int i = 0; i < numOfBVHShapes * 2 + 1; ++i)
			{
				pool[i] = new JBVNode;
			}
			root = pool[0];

			root->count = numOfBVHShapes;
			root->boundingBox = getBounds(0, root->count);
			++poolptr;
			Subdivide(*root, 0);
		}
	}
}

void World::clearBVH()
{
	if (numOfBVHShapes > 0)
	{
		if (pool != nullptr)
		{
			for (int i = 0; i < numOfBVHShapes * 2 + 1; ++i)
			{
				delete pool[i];
				pool[i] = nullptr;
			}
			delete[] pool;
		}
		if (boxes != nullptr) {
			for (int i = 0; i < numOfBVHShapes; ++i)
			{
				delete boxes[i];
				boxes[i] = nullptr;
			}
			delete[] boxes;
		}
	}
	delete[] sortedObjects;
	delete[] nonBVHShapes;

	root = nullptr;
	numOfBVHShapes = 0;
	numOfNonBVHShapes = 0;
	poolptr = 0;
	layers = 0;
}

void World::loadScene(Scenes sceneToLoad, Camera& camera)
{
	if (currentScene == sceneToLoad)
		return;

	delete scene;
	scene = nullptr;
	switch (sceneToLoad)
	{
	case Scenes::testScene:
		scene = new testScene{};
		break;
	case Scenes::week5Scene:
		scene = new week5Scene{};
		break;
	case Scenes::CornellBox:
		scene = new CornelBoxScene{};
		break;
	case  Scenes::fakeScene:
		scene = new fakeScene{};
		currentScene = Scenes::fakeScene;
		break;
	case  Scenes::week8Scene:
		scene = new week8Reference{};
		currentScene = Scenes::week8Scene;
		break;
	case Scenes::meshScene:
		scene = new meshTestScene{};
		currentScene = Scenes::meshScene;
		break;
	default:

		scene = new testScene{};
		currentScene = Scenes::testScene;
		assert(false && " Not a valid Scene");
	}

	currentScene = sceneToLoad;
	camera.viewMatrix = scene->cameraLocation;

	assert(scene != nullptr && "Something went wrong while loading the scene");
}

#pragma  warning(pop)