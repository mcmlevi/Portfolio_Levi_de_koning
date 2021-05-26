
#pragma once
#include "SceneManagement/Components/QuaternionRotationComponent.h"
#include "SceneManagement/Components/ScaleComponent.h"
#include "SceneManagement/Components/PositionComponent.h"
#include "SceneManagement/Components/ModelMatrixComponent.h"

namespace Frac 
{
	struct TransformComponent
	{
	public:
		PositionComponent Position;
		ScaleComponent Scale;
		QuaternionRotationComponent Rotation;

		bool isDirty = true;
		ModelMatrixComponent ModelMatrix;
	};
}