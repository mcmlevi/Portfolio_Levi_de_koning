#include "Thpch.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include <Core/WorldManager.h>

class TestSystem : public Th::Systems
{
public:
	TestSystem() {}
	~TestSystem() {}

	virtual void OnUpdate(float) override
	{
		
	}
};

namespace  UnitTest
{
	namespace Thorium
	{
		TEST_CASE("Systems")
		{
			SUBCASE("Registry")
			{
				TestSystem testSystem;
				Th::TransformComponent transform;
				size_t enttCount = testSystem.GetEntityCount();
				CHECK(enttCount == 0);
				entt::entity _entity = testSystem.CreateEntity("Test", transform);
				enttCount = testSystem.GetEntityCount();
				CHECK(enttCount == 1);
				//testSystem.DeleteEntity(_entity);
				enttCount = testSystem.GetEntityCount();
				CHECK(enttCount == 1);
			}

			SUBCASE("Components")
			{
				
			}
		}

		TEST_CASE("Components")
		{
			SUBCASE("TransformComponent")
			{
				Th::WorldManager& wm = Th::WorldManager::GetSingleton();
				
				Th::TransformComponent parent;
				Th::TransformComponent child1;
				Th::TransformComponent child2;
				Th::TransformComponent child3;
				
				entt::entity eParent = wm.CreateEntity("Parent", parent);
				entt::entity eChild1 = wm.CreateEntity("Child1", child1);
				entt::entity eChild2 = wm.CreateEntity("Child2", child2);
				entt::entity eChild3 = wm.CreateEntity("Child3", child3);
				
				CHECK(wm.GetEntityRegister().get<Th::TransformComponent>(eParent).GetPosition().x == 0);
				CHECK(wm.GetEntityRegister().get<Th::TransformComponent>(eParent).GetRotation().x == 0);
				CHECK(wm.GetEntityRegister().get<Th::TransformComponent>(eParent).GetScale().x == 1);

				wm.GetEntityRegister().get<Th::TransformComponent>(eParent).AddChild(eChild1);
				wm.GetEntityRegister().get<Th::TransformComponent>(eParent).AddChild(eChild2);

				CHECK(wm.GetEntityRegister().get<Th::TransformComponent>(eParent).GetChildCount() == 2);

				wm.GetEntityRegister().get<Th::TransformComponent>(eChild2).SetPosition(glm::vec3(0,1200,0));

				CHECK(wm.GetEntityRegister().get<Th::TransformComponent>(wm.GetEntityRegister().get<Th::TransformComponent>(eParent).GetChild(1)).GetPosition().y == 1200);

				wm.GetEntityRegister().get<Th::TransformComponent>(eParent).DetachChild(eChild2);

				CHECK(wm.GetEntityRegister().get<Th::TransformComponent>(eParent).GetChildCount() == 1);

				wm.GetEntityRegister().get<Th::TransformComponent>(wm.GetEntityRegister().get<Th::TransformComponent>(eParent).GetChild(0)).AddChild(eChild2);
				wm.GetEntityRegister().get<Th::TransformComponent>(wm.GetEntityRegister().get<Th::TransformComponent>(wm.GetEntityRegister().get<Th::TransformComponent>(eParent).GetChild(0)).GetChild(0)).AddChild(eChild3);
				
				//CHECK(test == true);
				
				// This is horrifying there should be a helper method to narrow this syntax.
				// I could say add a child on object child2, but you don't always know all child transforms.
				// new method: ( wm.GetTransform(wm.GetTransform(wm.GetTransform(eParent).GetChild(0)).GetChild(0)).AddChild(eChild3); )
				// I still don't like, would love the syntax to be entity.GetTransform().GetChild(0).GetChild(0).AddChild(0);
				// But for this I need to make some additions to the entity itself, and I can't be bothered to tangle to much in that class itself.


				
				//wm.GetEntityRegister().get<Th::TransformComponent>(eParent).AddChild(eChild1);
				//wm.GetEntityRegister().get<Th::TransformComponent>(eParent). (Detach all)??????

				
				
				//CHECK();
			}

			/*SUBCASE("CameraComponent")
			{

			}

			SUBCASE("TransformComponent")
			{

			}*/
		}
	}
	
	namespace TowerDefence
	{
		TEST_CASE("Enemy")
		{
			SUBCASE("Damage")
			{
				const int v0 = 0;
				CHECK(v0 == 0);
			}
		}
	}
}

