#pragma once
#include "Events/EventArgs.h"
#include <Windows.h>			//The WinAPI

#if defined CreateWindow
#undef CreateWindow
#endif

namespace Th
{
	/// <summary>
	/// Window specifically created for the Windows OS.
	/// </summary>
	class Win32Window
	{
	public:
		Win32Window() = default;
		~Win32Window() = default;

		void Initialize(UINT width, UINT height, bool showWindow = true);
		void Destroy();

		void PollEvents();

		HWND GetWindowHandle() const;
		UINT GetWindowWidth() const;
		UINT GetWindowHeight() const;
		const std::wstring& GetWindowTitle() const;

		void Show();
		void Hide();
	
	protected:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		void RegisterWindow(HINSTANCE appModule);
		HWND CreateWindow(HINSTANCE appModule, UINT width, UINT height);

	private:
		static Th::MouseButtonEventArgs::MouseButton DecodeMouseButton(UINT messageID);
		static HWND s_WindowHandle;
		static UINT s_WindowWidth;
		static UINT s_WindowHeight;
		static std::wstring s_WindowTitle;
		const static std::wstring s_WindowClassName;
	};
}