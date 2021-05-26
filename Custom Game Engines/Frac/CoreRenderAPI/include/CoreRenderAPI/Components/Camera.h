#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace TOR 
{
	/// <summary>
	/// Contains the different types of camera the renderer can provide for the renderer.
	/// </summary>
	enum class ECameraType
	{
		Perspective,
		Orthographic
	};

	/// <summary>
	/// Camera handles the project matrix and the settings that apply to it.
	/// </summary>
	struct Camera
	{
	public:
		glm::mat4 ProjectionMat;
		
		float FovInDeg;
		float ZNear;
		float ZFar;
		float AspectRatio;

		int ScreenWidth;
		int ScreenHeight;

		ECameraType CameraType;

		bool IsActive;
		bool IsDirty;
	};
}