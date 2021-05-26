#include <Thpch.h>
#include <Core/WorldManager.h>
#include <Components/NameComponent.h>
#include <Events/EventMessenger.h>
#include <unordered_map>

Th::WorldManager::WorldManager()
{
	
}

Th::WorldManager::~WorldManager()
{

}

void Th::WorldManager::Initialize()
{
	m_Systems = std::vector<Systems*>(0);
	Th::TransformComponent rootT{ glm::vec3{0.f,0.f,0.f},entt::null,glm::vec3{} };
	m_WorldRoot = CreateEntity("WorldRoot", rootT);
	m_Register.get<Th::TransformComponent>(m_WorldRoot).SetParent(entt::null, m_WorldRoot);
}

Th::WorldManager& Th::WorldManager::GetSingleton()
{
	static Th::WorldManager singleton;
	return singleton;
}

void Th::WorldManager::RegisterSystem(Systems* a_System)
{
	m_Systems.push_back(a_System);
}

entt::registry& Th::WorldManager::GetEntityRegister()
{
	return m_Register;
}

entt::entity Th::WorldManager::CreateEntity(const std::string& name, TransformComponent& transform)
{
		static std::unordered_map<std::string, unsigned int> sNameTracker{};

		entt::entity entity = GetEntityRegister().create();
		if(sNameTracker.find(name) == sNameTracker.end())
		{
			m_Register.emplace<NameComponent>(entity, name);
			sNameTracker.insert({name,0 });
		}
		else
		{
			int ID = ++sNameTracker[name];
			std::string editedName{name + std::to_string(ID)};
			m_Register.emplace<NameComponent>(entity, editedName);
		}
		m_Register.emplace<TransformComponent>(entity, transform);
		// This weird line is here beacuse emplace creates a new transform so when entt creates it I need to do the actual parenting manually
		Th::Components::TransformComponent& transformRef = m_Register.get<Th::TransformComponent>(entity);
		if (transform.GetParent() != entt::null)
			transformRef.SetParent(transform.GetParent(), entity);
		else
			transformRef.SetParent(m_WorldRoot, entity);
		return entity;
}

void Th::WorldManager::DeleteEntity(entt::entity a_Entity)
{
	Th::TransformComponent& entityTransRef = m_Register.get<Th::TransformComponent>(a_Entity);
	Th::TransformComponent& parentRef = m_Register.get<Th::TransformComponent>(entityTransRef.GetParent());
	parentRef.DetachChild(a_Entity);
	m_Register.destroy(a_Entity);
}

void Th::WorldManager::RemoveAllComponents(entt::entity a_Entity)
{
	m_Register.remove_all(a_Entity);
}

entt::entity Th::WorldManager::GetWorldRoot()
{
	return m_WorldRoot;
}

glm::vec3 Th::WorldManager::Unproject(glm::vec3& v, entt::entity Camera)
{
	Th::Components::TransformComponent& CamTransform = m_Register.get<Th::Components::TransformComponent>(Camera);
	Th::Components::CameraComponent& CamCmp = m_Register.get<Th::CameraComponent>(Camera);
	glm::vec4 vec(v.x, v.y, v.z, 1);
	glm::mat4 vpinv = CamCmp.GetProjection() * glm::inverse(CamTransform.GetWorldMatrix());
	vpinv = glm::inverse(vpinv);

	glm::vec4 unproj = glm::normalize(vpinv * vec);

	return glm::vec3(unproj.x, unproj.y, unproj.z);
}

void Th::WorldManager::Update(float a_DeltaTime)
{
	// Physics system.

	// Gameplay systems.
	for (size_t i = 0; i < m_Systems.size(); i++)
	{
		//printf("Tick");
		m_Systems[i]->OnUpdate(a_DeltaTime);
	}

	// Rendering system.
}
