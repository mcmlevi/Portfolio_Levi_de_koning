
#include "../RayTracer/RayTracer.h"


int main()
{
	
	
	//float width = 852;
	//float height = 480;
	float width = 800;
	float height = 480;
	int fov = 70;
	Vec3f cameraOrigin{ 0.f,0.f,0.f };
	Vec3f lookAt{ 0.f,1.f,0.f };
	Camera camera{ fov,width,height,cameraOrigin,lookAt };
	RayTracer tracer{static_cast<int>(width),static_cast<int>(height),camera};


	tracer.RenderImage();
	

}
