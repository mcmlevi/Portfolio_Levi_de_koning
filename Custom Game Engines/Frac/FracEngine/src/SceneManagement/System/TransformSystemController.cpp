#include "pch.h"
#include "SceneManagement/System/TransformSystemController.h"

#include "Core/ECSWrapper.h"
#include "CoreRenderAPI/Components/Transform.h"

struct updateTag{};

Frac::TransformSystemController::TransformSystemController(Frac::ECSWrapper* ecs) : m_wrapper(ecs)
{

}

void Frac::TransformSystemController::Initialize()
{	

	auto e =m_wrapper->makeEntity("TransformSystemEntity");
	m_wrapper->add<updateTag>(e);
	std::function<void(EntityHandle, updateTag&)> updateTransform = [this](EntityHandle, updateTag& )
	{
		this->ApplyTransform();
	};
	m_wrapper->makeSystem<updateTag>(updateTransform, Frac::ePipelineStage::onValidate, "Update Transform System");
}



void Frac::TransformSystemController::ApplyTransform()
{
	std::function<void(Frac::It, uint64_t, TOR::Transform*)> func = [this](Frac::It it, uint64_t i, TOR::Transform* transform)
	{
		
		Frac::Column<const TOR::Transform> parent = it.column<const TOR::Transform>(2);

		if (transform[i].isDirty)
		{
			if (parent.isSet())
			{
				const glm::mat4 t = glm::translate(glm::mat4{ 1.f }, transform[i].Position);
				const glm::mat4 r = glm::toMat4(transform[i].Orientation);
				const glm::mat4 s = glm::scale(glm::mat4{ 1.f }, transform[i].Scale);
				glm::mat4 trs = (t * r * s);
				glm::mat4 p = parent->ModelTransformData;
				glm::mat4 ptrs = p * trs;
				transform[i].ModelTransformData = ptrs;
			}
			else
			{
				const glm::mat4 t = glm::translate(glm::mat4{ 1.f }, transform[i].Position);
				const glm::mat4 r = glm::toMat4(transform[i].Orientation);
				const glm::mat4 s = glm::scale(glm::mat4{ 1.f }, transform[i].Scale);
				transform[i].ModelTransformData = (t * r * s);
			}
			if (transform[i].isStationary)
			{
				transform[i].isDirty = false;
			}
		}		
	};
	static auto w = m_wrapper->makeQuery< TOR::Transform>("CASCADE:TOR.Transform");
	w.Iter(func);
}
