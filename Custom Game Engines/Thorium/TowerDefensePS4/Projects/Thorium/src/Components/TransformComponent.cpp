#include "Thpch.h"
#include <Components/TransformComponent.h>
#include <Core/MessageLogger.h>
#include <entt/single_include/entt/entt.hpp>
#include <Core/WorldManager.h>
#define GET_PARENT_TRANSFORM(transform) Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::TransformComponent>(transform)

Th::Components::TransformComponent::TransformComponent(const glm::vec3& position /*= glm::vec3(0.0f)*/, entt::entity parent /*= nullptr*/, 
                                                       const glm::vec3& eulerRotation /*= glm::vec3(0.0f)*/, const glm::vec3& scale /*= glm::vec3(1.0f)*/) :
        m_Parent(parent),
        m_WorldMatrix(1.0f),
        m_Position(position),
        m_Rotation(0.0f, 0.0f, 0.0f, 0.0f),
        m_Scale(1.0f)
{
    m_Position = position;
	glm::quat pitch = glm::angleAxis(eulerRotation.x, glm::vec3(1, 0, 0));
	glm::quat yaw = glm::angleAxis(eulerRotation.y, glm::vec3(0, 1, 0));
	glm::quat roll = glm::angleAxis(eulerRotation.z, glm::vec3(0, 0, -1));
	m_Rotation = pitch * yaw * roll;
    m_Scale = scale;
    m_EulerAngles = eulerRotation;
	CalculateMatrices();
}

Th::Components::TransformComponent::TransformComponent(const glm::vec3& position, const glm::vec3& targetPos, entt::entity parent /*= nullptr*/,
    const glm::vec3& up /*= glm::vec3(0.0f, 1.0f, 0.0f)*/, const glm::vec3& eulerRotation /*= glm::vec3(0.0f)*/, const glm::vec3& scale /*= glm::vec3(1.0f)*/) :
	m_Parent(parent),
	m_WorldMatrix(1.0f),
	m_Position(position),
	m_Rotation(0.0f, 0.0f, 0.0f, 0.0f),
	m_Scale(1.0f)
{
 
	m_Position = position;
	glm::quat pitch = glm::angleAxis(eulerRotation.x, glm::vec3(1, 0, 0));
	glm::quat yaw = glm::angleAxis(eulerRotation.y, glm::vec3(0, 1, 0));
	glm::quat roll = glm::angleAxis(eulerRotation.z, glm::vec3(0, 0, -1));
	m_Rotation = pitch * yaw * roll;
	m_Scale = scale;
    m_EulerAngles = eulerRotation;
    LookAt(targetPos, up);
}

Th::Components::TransformComponent::~TransformComponent()
{
}

// --------------------------------------------- \\
// ----- || Position | Rotation | Scale || ----- \\
// --------------------------------------------- \\

void Th::Components::TransformComponent::SetPosition(const glm::vec3& a_Position)
{
    m_Position = a_Position;
    CalculateMatrices();
}

glm::vec3 Th::Components::TransformComponent::GetPosition() const
{
    return m_Position;
}

void Th::Components::TransformComponent::SetRotation(const glm::vec3& a_Rotation)
{
    m_EulerAngles = a_Rotation;
    glm::quat pitch = glm::angleAxis(a_Rotation.x, glm::vec3(1, 0, 0));
    glm::quat yaw = glm::angleAxis(a_Rotation.y, glm::vec3(0, 1, 0));
    glm::quat roll = glm::angleAxis(a_Rotation.z, glm::vec3(0, 0, -1));
    m_Rotation = roll * yaw * pitch;
	//m_Rotation = glm::quat(a_Rotation);
    CalculateMatrices();
}

void Th::Components::TransformComponent::SetRotation(const glm::quat& a_Rotation)
{
    m_Rotation = a_Rotation;
    CalculateMatrices();
}

glm::quat Th::Components::TransformComponent::GetRotation() const
{
    return m_Rotation;
}

glm::vec3 Th::Components::TransformComponent::GetEulerAngles() const
{
    return m_EulerAngles;
	//return glm::eulerAngles(m_Rotation);
}

void Th::Components::TransformComponent::SetScale(const glm::vec3& a_Scale)
{
    m_Scale = a_Scale;
    CalculateMatrices();
}

glm::vec3 Th::Components::TransformComponent::GetScale() const
{
    return m_Scale;
}

// ------------------------ \\
// ----- || Matrix || ----- \\
// ------------------------ \\

void Th::Components::TransformComponent::CalculateMatrices()
{
    m_LocalMatrix =
        glm::translate(glm::mat4(1.0f), m_Position) *
        glm::toMat4(m_Rotation) *
        glm::scale(glm::mat4(1.0f), m_Scale);

    m_WorldMatrix = LocalToWorldMatrix() * m_LocalMatrix;
}

glm::mat4 Th::Components::TransformComponent::GetWorldMatrix() const
{
    return m_WorldMatrix;
}

glm::mat4 Th::Components::TransformComponent::GetLocalMatrix() const
{
    return m_LocalMatrix;
}

glm::mat4 Th::Components::TransformComponent::WorldToLocalMatrix()
{
    glm::mat4 mat = glm::identity<glm::mat4>();
    return m_Parent == entt::null ? mat : GET_PARENT_TRANSFORM(m_Parent).WorldToLocalMatrix() * glm::inverse(GET_PARENT_TRANSFORM(m_Parent).GetLocalMatrix());
}

glm::mat4 Th::Components::TransformComponent::LocalToWorldMatrix()
{
    glm::mat4 mat = glm::identity<glm::mat4>();
    return m_Parent == entt::null ? mat : GET_PARENT_TRANSFORM(m_Parent).LocalToWorldMatrix() * GET_PARENT_TRANSFORM(m_Parent).GetLocalMatrix();
}

// -------------------------------- \\
// ----- || Parent | Child || ----- \\
// -------------------------------- \\

void Th::Components::TransformComponent::SetParent(entt::entity a_Parent,entt::entity thisEntity)
{

    // Set new parent.
    m_Parent = a_Parent;

    if (m_Parent != entt::null)
		GET_PARENT_TRANSFORM(m_Parent).AddChild(thisEntity);
    CalculateMatrices();
}

entt::entity Th::Components::TransformComponent::GetParent() const
{
    return m_Parent;
}


void Th::Components::TransformComponent::AddChild(entt::entity a_Child)
{
    bool exist = false;

    // Flag to not add the child if it already exists.
    for (size_t i = 0; i < m_Children.size(); i++)
    {
        if (a_Child == m_Children[i])
        {
            exist = true;
        }
    }

    if (!exist)
    {
        m_Children.push_back(a_Child);
    }
}

entt::entity Th::Components::TransformComponent::GetChild(const int a_Index) const
{
    // Only try to get it when the index is between valid range.
    assert(a_Index < m_Children.size() && a_Index > -1);
    return m_Children[a_Index];
}

void Th::Components::TransformComponent::DetachChild(entt::entity child)
{
    auto it = std::find(m_Children.begin(), m_Children.end(), child);
	if(it != m_Children.end())
	{
        m_Children.erase(it);
	}
}

entt::entity Th::TransformComponent::GetRoot() const
{
    entt::entity current = entt::null;
    entt::entity parent = m_Parent;
	
    while(true)
    {
        current = parent;
        parent = Th::WorldManager::GetSingleton().GetEntityRegister().get<TransformComponent>(current).GetParent();
    	
	    if(parent == entt::null)
	    {
            return current;
	    }
    }
}

int Th::Components::TransformComponent::GetChildCount()
{
    return m_Children.size();
}

// --------------------------- \\
// ----- || Utilities || ----- \\
// --------------------------- \\

glm::vec3 Th::Components::TransformComponent::GetRight() const
{
    return glm::normalize(m_WorldMatrix[0]);
}

glm::vec3 Th::Components::TransformComponent::GetUp() const
{
    return glm::normalize(m_WorldMatrix[1]);
}

glm::vec3 Th::Components::TransformComponent::GetForward() const
{
    return glm::normalize(m_WorldMatrix[2]);
}

void Th::Components::TransformComponent::LookAt(const glm::vec3& a_Target, const glm::vec3& a_Up)
{
    m_Rotation = glm::quatLookAtLH(glm::normalize(a_Target - m_Position), a_Up);
    CalculateMatrices();
}

bool Th::Components::TransformComponent::operator==(const Th::Components::TransformComponent& rhs)
{
    return ((m_Parent == rhs.m_Parent) && (m_Children == rhs.m_Children) && (m_LocalMatrix == rhs.m_LocalMatrix));
}

bool Th::Components::TransformComponent::operator!=(const Th::Components::TransformComponent& rhs)
{
    return !(*this == rhs);
}
