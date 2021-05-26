#include <Thpch.h>
#include <Graphics/D3D12/D3D12Device.h>
#include <Utility/D3DErrorHandler.h>
#include <Graphics/D3D12/D3D12CommandQueue.h>
#include <Graphics/D3D12/D3D12CommandList.h>

namespace Th
{
	void D3D12Device::Initialize(bool useWarp)
	{
		m_UseWarp = useWarp;
		m_SelectedAdapter = SelectCompatibleAdapter(useWarp);
		m_D3D12Device = CreateD3DDevice();

		m_DirectCommandQueue = new D3D12CommandQueue();
		m_ComputeCommandQueue = new D3D12CommandQueue();
		m_CopyCommandQueue = new D3D12CommandQueue();

		m_DirectCommandQueue->Initialize(this, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_ComputeCommandQueue->Initialize(this, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE);
		m_CopyCommandQueue->Initialize(this, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY);
	}

	void D3D12Device::Destroy()
	{
		m_DirectCommandQueue->Destroy();
		m_ComputeCommandQueue->Destroy();
		m_CopyCommandQueue->Destroy();

		delete m_DirectCommandQueue;
		delete m_ComputeCommandQueue;
		delete m_CopyCommandQueue;
	}

	Microsoft::WRL::ComPtr<ID3D12Device2> D3D12Device::Get() const
	{
		return m_D3D12Device;
	}

	bool D3D12Device::DoesUseWarp() const
	{
		return m_UseWarp;
	}

	Th::D3D12CommandQueue* D3D12Device::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const
	{
		D3D12CommandQueue* commandQueue = nullptr;
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			commandQueue = m_DirectCommandQueue;
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			commandQueue = m_ComputeCommandQueue;
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			commandQueue = m_CopyCommandQueue;
			break;
		default:
			assert(false && "Invalid command queue type.");
		}

		return commandQueue;
	}

	void D3D12Device::Flush()
	{
		m_DirectCommandQueue->Flush();
		m_ComputeCommandQueue->Flush();
		m_CopyCommandQueue->Flush();
	}

	Microsoft::WRL::ComPtr<ID3D12Device2> D3D12Device::CreateD3DDevice()
	{
		Microsoft::WRL::ComPtr<ID3D12Device2> d3d12Device2;
		D3D_CHECK(D3D12CreateDevice(m_SelectedAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device2)));

#if defined(_DEBUG)
		Microsoft::WRL::ComPtr<ID3D12InfoQueue> pInfoQueue;
		if (SUCCEEDED(d3d12Device2.As(&pInfoQueue)))
		{
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

			// Suppress whole categories of messages
			//D3D12_MESSAGE_CATEGORY Categories[] = {};

			// Suppress messages based on their severity level
			D3D12_MESSAGE_SEVERITY Severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};

			// Suppress individual messages by their ID
			D3D12_MESSAGE_ID DenyIds[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
			};

			D3D12_INFO_QUEUE_FILTER NewFilter = {};
			//NewFilter.DenyList.NumCategories = _countof(Categories);
			//NewFilter.DenyList.pCategoryList = Categories;
			NewFilter.DenyList.NumSeverities = _countof(Severities);
			NewFilter.DenyList.pSeverityList = Severities;
			NewFilter.DenyList.NumIDs = _countof(DenyIds);
			NewFilter.DenyList.pIDList = DenyIds;

			D3D_CHECK(pInfoQueue->PushStorageFilter(&NewFilter));
		}
#endif

		return d3d12Device2;
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter4> D3D12Device::SelectCompatibleAdapter(bool useWARP)
	{
		ComPtr<IDXGIFactory4> dxgiFactory;
		unsigned int dxgiFactoryFlags = 0;

#ifdef _DEBUG
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		D3D_CHECK(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

		ComPtr<IDXGIAdapter4> dxgiAdapter4;
		ComPtr<IDXGIAdapter1> dxgiAdapter1;

		if (m_UseWarp)
		{
			D3D_CHECK(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
			D3D_CHECK(dxgiAdapter1.As(&dxgiAdapter4));
		}
		else
		{
			SIZE_T maxDedicatedVideoMemory = 0;
			for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
				dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

				if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
					SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
					dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
				{
					maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
					D3D_CHECK(dxgiAdapter1.As(&dxgiAdapter4));
				}
			}
		}

		return dxgiAdapter4;
	}
}