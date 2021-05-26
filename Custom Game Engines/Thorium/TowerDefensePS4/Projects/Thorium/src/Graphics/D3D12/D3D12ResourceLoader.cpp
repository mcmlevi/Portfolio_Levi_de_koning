#include <Thpch.h>
#include <Graphics/D3D12/D3D12ResourceLoader.h>
#include <Components/ModelComponent.h>
#include <Graphics/D3D12/D3D12CommandList.h>
#include <Graphics/ModelLoading/ObjFactoryWin.h>
#include <Graphics/ModelLoading/GLTFFactoryWin.h>
#include <Graphics/D3D12/D3D12CommandQueue.h>
#include <Resources/Mesh.h>
#include <Resources/Shader.h>
#include <Core/MessageLogger.h>

namespace Th
{
	void D3D12ResourceLoader::Initialize(D3D12Device& device)
	{
		m_Device = device;
	}

	void D3D12ResourceLoader::Destroy()
	{

	}

	Shader* D3D12ResourceLoader::LoadShaderPipeline(const std::string& vertShader, const std::string& fragShader)
	{
		Shader* shader = new Shader();
		shader->Initialize(m_Device, vertShader, fragShader);
		return shader;
	}

	Components::ModelComponent* D3D12ResourceLoader::LoadModel(const std::string& modelPath, EModelFileType modelType)
	{
		auto commandQueue = m_Device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto commandList = commandQueue->GetCommandList();

		Components::ModelComponent* model = new Components::ModelComponent();

		Th::ModelData* modelData = nullptr;
		switch (modelType)
		{
		case EModelFileType::OBJ:
		{
			ObjFactoryWin factory;
			modelData = factory.LoadModel(modelPath);

		}
		break;
		case EModelFileType::GLTF:
		{
			GLTFFactoryWin factory;
			modelData = factory.LoadModel(modelPath);		
		}
		break;
		default:
			LOGERROR("Unsupported file type for model loading!");
			return nullptr;
		}

		std::vector<RenderComponent> meshes;

		for (int i = 0; i < modelData->Vertices.size(); ++i)
		{
			meshes.push_back(RenderComponent());
			meshes[i].RenderMesh.Initialize(&m_Device, commandList, modelData->Vertices[i], modelData->Indexes[i]);
		}

		model->SetMeshes(meshes);

		auto fenceValue = commandQueue->ExecuteCommandList(commandList);
		commandQueue->WaitForFenceValue(fenceValue);
		model->SetCollider(modelData->Min, modelData->Max);
		return model;
	}
}