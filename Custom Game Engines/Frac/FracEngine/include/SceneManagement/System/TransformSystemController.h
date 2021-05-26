#pragma once
#include <Core/Query.h>

namespace TOR {
	struct Transform;
}

namespace Frac
{
	class ECSWrapper;
	
	class TransformSystemController
	{
	public:
		TransformSystemController(Frac::ECSWrapper* ecs);
	
		void Initialize();	

	private:
		void ApplyTransform();
		ECSWrapper* m_wrapper;
	};

}//namespace Frac
