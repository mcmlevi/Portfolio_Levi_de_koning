#pragma once
#include "Tools/PC/Window/IGUIWindow.h"
#include "Core/ECSWrapper.h"

namespace Frac
{
	class EntityList : public IGUIWindow
	{
	public:
		EntityList(ECSWrapper& a_flecsWorld);
		~EntityList() = default;

		void Update() override {};
		EntityHandle GetSelectedEntity() const;

	private: 
		ECSWrapper* m_flecsWorld;

		int m_entityCount;
		int m_selectedItem;
		float m_selectableWidgetXOffset;
		EntityHandle m_selectedEntity;

		std::vector<EntityHandle> m_entitiesWithTransformComponent;
	};
} // namespace Frac