#pragma once
#include <string>
#include <glm/fwd.hpp>
#include <Graphics/D3D12/D3D12Device.h>
#include <Graphics/D3D12/D3D12CommandList.h>

namespace Th
{
	/// <summary>
	/// Specific shader implementation for D3D12.
	/// </summary>
	class D3D12Shader
	{
	public:
		D3D12Shader() = default;
		~D3D12Shader() = default;

		void Initialize(D3D12Device& device, const std::string& vertShader, const std::string& fragShader);
		void Destroy();

		void Bind(D3D12CommandList* commandList);
		void SetRootParams(D3D12CommandList* commandList, const glm::mat4& mvp);

	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
	};
}