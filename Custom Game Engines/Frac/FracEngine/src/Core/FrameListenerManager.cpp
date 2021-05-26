#include <pch.h>
#include <Core/FrameListenerManager.h>
#include <Core/FrameListener.h>
#include <Core/ECSWrapper.h>
#include <Core/EngineCore.h>
struct FrameListnerComponent
{
	Frac::FrameListener* ptr_frameListener;
};


void Frac::FrameListenerManager::Initialize(ECSWrapper& wrapper)
{
	std::function<void(EntityHandle, FrameListnerComponent&)> begin = [](EntityHandle, FrameListnerComponent& listner)
	{
		listner.ptr_frameListener->FrameStarted();
	};
	wrapper.makeSystem<FrameListnerComponent>(begin, ePipelineStage::preUpdate,"beginFrameListnerSystem");

	std::function<void(EntityHandle, FrameListnerComponent&)> end = [](EntityHandle, FrameListnerComponent& listner)
	{
		listner.ptr_frameListener->FrameEnded();
	};
	wrapper.makeSystem<FrameListnerComponent>(end, ePipelineStage::postUpdate,"endFrameListnerSystem");
	
}

Frac::FrameListenerHandle Frac::FrameListenerManager::AddFrameListener(FrameListener* listener)
{
	ECSWrapper& l = listener->GetEngineConnector()->GetEngine().GetWorld();
	return l.makeEntity<FrameListnerComponent>("", FrameListnerComponent{ listener });
}
