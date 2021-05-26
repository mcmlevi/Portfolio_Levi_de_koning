#include <Thpch.h>
#include <Resources/D3D12/D3D12Resource.h>
#include <Graphics/D3D12/D3D12Device.h>
#include <Utility/D3DErrorHandler.h>

namespace Th
{

	D3D12Resource::D3D12Resource(const std::wstring& name /*= L""*/)
	{
		m_ResourceName = name;
		m_FormatSupport = {};
	}

	D3D12Resource::D3D12Resource(const D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE* clearValue, const std::wstring& name /*= L""*/)
	{
		m_ResourceName = name;

		if (clearValue != nullptr)
		{
			m_ClearValue = clearValue;
		}

		D3D_CHECK(m_Device->Get()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON,
			m_ClearValue,
			IID_PPV_ARGS(&m_InteralResource)));

		//TODO: Add to global resource tracker for transition barriers.

		SetName(name);
	}

	D3D12Resource::D3D12Resource(ComPtr<ID3D12Resource> resource, const std::wstring& name /*= L""*/) 
		: m_InteralResource(resource),
		m_FormatSupport({})
	{
		SetName(name);
	}

	D3D12Resource::D3D12Resource(const D3D12Resource& copy)
	{
		m_ResourceName = copy.m_ResourceName;
		m_InteralResource = copy.m_InteralResource;
		m_Device = copy.m_Device;
		m_FormatSupport = copy.m_FormatSupport;
		m_ClearValue = copy.m_ClearValue != nullptr ? new D3D12_CLEAR_VALUE(*copy.m_ClearValue) : nullptr;
	}

	D3D12Resource::D3D12Resource(D3D12Resource&& copy) noexcept
	{
		m_InteralResource = std::move(copy.m_InteralResource);
		m_FormatSupport = std::move(copy.m_FormatSupport);
		m_ResourceName = std::move(copy.m_ResourceName);
		m_ClearValue = std::move(copy.m_ClearValue);
		m_Device = std::move(copy.m_Device);
	}

	D3D12Resource& D3D12Resource::operator=(D3D12Resource&& other) noexcept
	{
		if (this != &other)
		{
			m_InteralResource = std::move(other.m_InteralResource);
			m_FormatSupport = other.m_FormatSupport;
			m_ResourceName = std::move(other.m_ResourceName);
			m_ClearValue = std::move(other.m_ClearValue);
			m_Device = std::move(other.m_Device);

			other.Reset();
		}

		return *this;
	}

	D3D12Resource& D3D12Resource::operator=(const D3D12Resource& other)
	{
		if (this != &other)
		{
			m_InteralResource = other.m_InteralResource;
			m_FormatSupport = other.m_FormatSupport;
			m_ResourceName = other.m_ResourceName;
			m_Device = other.m_Device;
			
			if (other.m_ClearValue != nullptr)
			{
				m_ClearValue = new D3D12_CLEAR_VALUE(*other.m_ClearValue);
			}
		}

		return *this;
	}

	void D3D12Resource::Initialize(D3D12Device* device)
	{
		m_Device = device;

		CheckFeatureSupport();
	}

	bool D3D12Resource::IsValid() const
	{
		return m_InteralResource != nullptr;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> D3D12Resource::Get() const
	{
		return m_InteralResource;
	}

	D3D12_RESOURCE_DESC D3D12Resource::GetDesc() const
	{
		return m_InteralResource != nullptr ? m_InteralResource->GetDesc() : D3D12_RESOURCE_DESC();
	}

	void D3D12Resource::Set(ComPtr<ID3D12Resource> d3d12Resource, D3D12_CLEAR_VALUE* clearValue /*= nullptr*/)
	{
		m_InteralResource = d3d12Resource;
		m_ClearValue = m_ClearValue != nullptr ? clearValue : nullptr;
		
		CheckFeatureSupport();
		SetName(m_ResourceName);
	}

	void D3D12Resource::SetName(const std::wstring& name)
	{
		m_ResourceName = name;
		if (m_InteralResource != nullptr && !m_ResourceName.empty())
		{
			m_InteralResource->SetName(m_ResourceName.c_str());
		}
	}

	void D3D12Resource::Reset()
	{
		m_InteralResource.Reset();
		m_FormatSupport = {};
		delete m_ClearValue;
		m_ClearValue = nullptr;
		m_ResourceName.clear();
		m_Device = nullptr;
	}

	void D3D12Resource::CheckFeatureSupport()
	{
		if (m_Device != nullptr && m_InteralResource != nullptr)
		{
			auto desc = m_InteralResource->GetDesc();

			m_FormatSupport.Format = desc.Format;
			D3D_CHECK(m_Device->Get()->CheckFeatureSupport(
				D3D12_FEATURE::D3D12_FEATURE_FORMAT_SUPPORT,
				&m_FormatSupport,
				sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT)));
		}
		else
		{
			m_FormatSupport.Format = {};
		}
	}

}