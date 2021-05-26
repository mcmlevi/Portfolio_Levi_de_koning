#pragma once
#include <string>
#include <wrl.h>
#include <d3d12.h>
namespace Th
{
	class D3D12Device;

	/// <summary>
	/// A d3d12 specific resource base class.
	/// </summary>
	class D3D12Resource
	{
	public:
		D3D12Resource(const std::wstring& name = L"");
		D3D12Resource(const D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE* clearValue, const std::wstring& name = L"");
		D3D12Resource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const std::wstring& name = L"");

		D3D12Resource(const D3D12Resource& copy);
		D3D12Resource(D3D12Resource&& copy) noexcept;

		D3D12Resource& operator=(const D3D12Resource& other);
		D3D12Resource& operator=(D3D12Resource&& other) noexcept;

		virtual ~D3D12Resource() = default;

		virtual void Initialize(D3D12Device* device);

		/// <summary>
		/// Checks whether the resource is still valid.
		/// </summary>
		/// <returns>Returns the state of the resource.</returns>
		bool IsValid() const;

		/// <summary>
		/// Returns the resource.
		/// </summary>
		/// <returns>Returns a Comptr to the resource.</returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> Get() const;

		/// <summary>
		/// Retrieve the description of the resource.
		/// </summary>
		/// <returns>Returns the description of the resource.</returns>
		D3D12_RESOURCE_DESC GetDesc() const;

		/// <summary>
		/// Assigns another resource.
		/// </summary>
		/// <param name="d3d12Resource">The new resource.</param>
		/// <param name="clearValue">Optional: the clear value in case the resource needs it.</param>
		void Set(Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource, D3D12_CLEAR_VALUE* clearValue = nullptr);

		/// <summary>
		/// Sets the resource name.
		/// </summary>
		/// <param name="name">The name.</param>
		void SetName(const std::wstring& name);

		/// <summary>
		/// Deletes all the contents of the resource and sets it to a null state.
		/// </summary>
		virtual void Reset();

	protected:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_InteralResource;
		D3D12_FEATURE_DATA_FORMAT_SUPPORT m_FormatSupport;
		D3D12_CLEAR_VALUE* m_ClearValue;
		std::wstring m_ResourceName;

	private:
		D3D12Device* m_Device;

		/// <summary>
		/// Checks whether the format feature is supported on this GPU.
		/// </summary>
		void CheckFeatureSupport();
	};
}