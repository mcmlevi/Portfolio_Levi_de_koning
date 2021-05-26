#pragma once

#include <vector>
#include <Graphics/D3D12/D3D12Device.h>
#include <Resources/D3D12/D3D12Vertex.h>
#include <Resources/Shader.h>
#include <Components/ModelComponent.h>
#include <Resources/EModelFileType.h>

namespace Th
{
	/// <summary>
	/// The mesh loader takes care of platform specific resource loading, in this case d3d12.
	/// </summary>
	class D3D12ResourceLoader
	{
	public:
		D3D12ResourceLoader() = default;
		~D3D12ResourceLoader() = default;

		void Initialize(D3D12Device& device);
		void Destroy();

		Shader* LoadShaderPipeline(const std::string& vertShader, const std::string& fragShader);
		Components::ModelComponent* LoadModel(const std::string& modelPath, EModelFileType modelType);
	
	private:
		D3D12Device m_Device;
	};
}