#pragma once
#include "Tools/PC/Window/IGUIWindow.h"

namespace Frac
{
	class ECSWrapper;
	class EntityList;
	class ComponentInspector : public IGUIWindow
	{
	public:
		ComponentInspector(ECSWrapper& a_flecsWorld, EntityList& a_entityList);
		~ComponentInspector() = default;

		void Update() override;

	private:
		ECSWrapper& m_flecsWorld;
		EntityList& m_entityList;
	};

} // namespace Frac