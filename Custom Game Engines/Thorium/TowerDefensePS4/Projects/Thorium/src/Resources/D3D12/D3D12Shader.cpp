#include <Thpch.h>
#include <Resources/D3D12/D3D12Shader.h>
#include <Utility/D3DErrorHandler.h>
#include <Graphics/D3D12/D3D12CommandQueue.h>
#include <Graphics/D3D12/D3D12CommandList.h>
#include <Resources/D3D12/D3D12Vertex.h>

namespace Th
{
	void D3D12Shader::Initialize(D3D12Device& device, const std::string& vertShader, const std::string& fragShader)
	{
		//LOAD SHADERS.
		ComPtr<ID3DBlob> vertexShaderBlob;
		ComPtr<ID3DBlob> fragShaderBlob;

		//User interface has been implemented.
		D3D_CHECK(D3DReadFileToBlob(L"res/shaders/TriangleShader.vert.cso", &vertexShaderBlob));
		D3D_CHECK(D3DReadFileToBlob(L"res/shaders/TriangleShader.frag.cso", &fragShaderBlob));


		//ROOT SIGNATURE.
		//Version 1.1 is preferred, though not a must, hence checking for support.
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = { D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1_1 };
		if ((device.Get()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))) < 0)
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		// Allow input layout and deny unnecessary access to certain pipeline stages.
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		//Root parameters, in this case only the MVP in the vertex shader.
		CD3DX12_ROOT_PARAMETER1 rootParams[1] = {};
		rootParams[0].InitAsConstants(sizeof(glm::mat4) / 4, 0, 0, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParams), rootParams, 0, nullptr, rootSignatureFlags);

		ComPtr<ID3DBlob> rootSignatureBlob;
		ComPtr<ID3DBlob> rootSignatureError;

		D3D_CHECK(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &rootSignatureBlob, &rootSignatureError));
		D3D_CHECK(device.Get()->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));

		//PSO
		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS FS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} pipelineStateStream;

		D3D12_RT_FORMAT_ARRAY rtvFormats =
		{
			{ DXGI_FORMAT_R8G8B8A8_UNORM },
			1
		};

		pipelineStateStream.RootSignature = m_RootSignature.Get();
		pipelineStateStream.InputLayout = { D3D12Vertex::InputElements, D3D12Vertex::InputElementCount };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
		pipelineStateStream.FS = CD3DX12_SHADER_BYTECODE(fragShaderBlob.Get());
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateStream.RTVFormats = rtvFormats;

		D3D12_PIPELINE_STATE_STREAM_DESC psoDesc =
		{
			sizeof(PipelineStateStream),
			&pipelineStateStream
		};

		D3D_CHECK(device.Get()->CreatePipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));
	}

	void D3D12Shader::Destroy()
	{

	}

	void D3D12Shader::Bind(D3D12CommandList* commandList)
	{
		commandList->GetGraphicsCommandList()->SetPipelineState(m_PipelineState.Get());
		commandList->GetGraphicsCommandList()->SetGraphicsRootSignature(m_RootSignature.Get());
	}

	void D3D12Shader::SetRootParams(D3D12CommandList* commandList, const glm::mat4& mvp)
	{
		commandList->GetGraphicsCommandList()->SetGraphicsRoot32BitConstants(0, sizeof(glm::mat4) / 4, &mvp, 0);
	}
}