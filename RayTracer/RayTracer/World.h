#pragma once
#include "Ray.h"
#include "shapes.h"
#include "Light.h"
#include "Scene.h"
#include "JBVHnode.h"

#define PRINTINFO 0
#define VISUALIZE 0
#define FORCEBVH 0
#include "e_scenes.h"
class Camera;

class World
{
public:
	World(Camera& camera);
	~World();
	Shape* queryIntersection(const Ray& ray, Vec3f& hitPoint);
	bool getAIntersection(const Ray& ray, const float& maxDistance);
	Color visualizeBox(const Ray& ray);

	Scene* scene;
	Scenes currentScene;
	int testDepth{ 1 };
#if(PRINTINFO)
	unsigned long long timesQueried{ };
	unsigned long long numOfTests{};
	unsigned long long earllyOutTests{};
	unsigned long long timesQueriedEarlyOut{};
	int numOfLayers{};
#endif

	long layers{};

	static const int minObjectForBVH{ 0 };

	void buildBVH();
	void clearBVH();
	void loadScene(Scenes sceneToLoad, Camera& camera);
private:
	Shape* GetClosestShape(const Ray& ray, JBVNode& currentNode, float& boxDistance, float& objectDistance);
	void traverseWithEarlyOut(const Ray& ray, JBVNode& currentNode, float& objectDistance, const float maxDistance);
	void getEdgeIntersection(const Ray& ray, JBVNode& currentNode, float& objectDistance, int& depthcounter, int currentDepth);



	JBVNode* root;

	JBVNode** pool;
	int numOfNonBVHShapes{};
	int numOfBVHShapes{};
	Shape** nonBVHShapes{nullptr};
	Shape** sortedObjects{nullptr};
	AABBox** boxes{nullptr};
	int maxObjectsInLeaf{ 20 };
	int poolptr{};
	AABBox getBounds(int first, int count);
	void Subdivide(JBVNode & currentNode, int first);
	int Partition(int first, int count, bool& succes);
	int boxDepth{};


	
};
