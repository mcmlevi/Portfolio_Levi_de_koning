#pragma once
#include <d3d12.h>
#include <wrl.h>


#if _DEBUG
/// <summary>
/// The D3D_CHECK validates the D3D12 API method call return values..
/// </summary>
#define D3D_CHECK(method) D3DErrorHandler::CheckD3DError(method, #method, __FILE__, __LINE__);
#else
#define D3D_CHECK(stmt) stmt
#endif

namespace Th
{
	/// <summary>
	/// Specifically handles the D3D12 errors on the API methods that are called using the D3D_CHECK method.
	/// This class needs an instance somewhere to exist whenever the D3D12 renderer is used - therefor the D3D12 renderer has ownership of this class.
	/// </summary>
	class D3DErrorHandler
	{
	public:
		D3DErrorHandler();
		~D3DErrorHandler() = default;

		/// <summary>
		/// Used by the D3D_CHECK macro in order to check the validity of the D3D methods used by the D3D12 API.
		/// Never call this, unless the user needs to know something really specific.
		/// </summary>
		/// <param name="hr">The return value of the method.</param>
		/// <param name="method">The name of the current method.</param>
		/// <param name="file">The name of the file including the file path.</param>
		/// <param name="lineNumber">The line number the error occurs at.</param>
		static void CheckD3DError(long hr, const char* method, const std::string& file, int lineNumber);
	
	private:
		Microsoft::WRL::ComPtr<ID3D12Debug> m_DebugInterface;
	};
}