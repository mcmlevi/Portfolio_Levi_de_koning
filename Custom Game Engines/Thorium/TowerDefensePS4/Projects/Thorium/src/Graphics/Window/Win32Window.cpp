#include <Thpch.h>
#include <Graphics/Window/Win32Window.h>
#include "Events/EventArgs.h"
#include <Events/InputHandler.h>
#include <Events/EventMessenger.h>
#include <Graphics/ImGui/imgui_impl_win32.h>
// Forward declare message handler from imgui_impl_win32.cpp
#if defined CreateWindow
#undef CreateWindow
#endif
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Th
{
	void Win32Window::Initialize(UINT width, UINT height, bool showWindow)
	{
		HINSTANCE appModule = (HINSTANCE)GetModuleHandle(NULL);

		s_WindowWidth = width;
		s_WindowHeight = height;

		RegisterWindow(appModule);
		s_WindowHandle = CreateWindow(appModule, width, height);
		
        EventMessenger::GetInstance().AddMessenger("OnWindowCloseRequest");
        EventMessenger::GetInstance().AddMessenger<ResizeEventArgs&>("OnWindowResizeRequest");

		if(showWindow)
			Show();
	}

	void Win32Window::Destroy()
	{
		DestroyWindow(s_WindowHandle);
	}

	void Win32Window::PollEvents()
	{
		MSG msg;
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
                EventMessenger::GetInstance().EvokeMessenger("OnWindowCloseRequest");
                return;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}
	}

	HWND Win32Window::GetWindowHandle() const
	{
		return s_WindowHandle;
	}

	UINT Win32Window::GetWindowWidth() const
	{
		return s_WindowWidth;
	}

	UINT Win32Window::GetWindowHeight() const
	{
		return s_WindowHeight;
	}

	const std::wstring& Win32Window::GetWindowTitle() const
	{
		return s_WindowTitle;
	}

	void Win32Window::Show()
	{
		ShowWindow(s_WindowHandle, SW_SHOW);
	}

	void Win32Window::Hide()
	{
		ShowWindow(s_WindowHandle, SW_HIDE);
	}

    LRESULT CALLBACK Win32Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        static MouseMotionEventArgs sOldMousePos{ false,false,false,false,false,0,0 };
		InputHandler& inputHandler{ InputHandler::getInstance() };
        if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
            return true;
        switch (message)
        {
            
        case WM_CREATE:
        {
            LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
        }
        return 0;


        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(hWnd, &ps);

            //TODO: Pass the Update/Tick event here.
            //TODO: Pass the render call event here.
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        {
            MSG charMsg;
            // Get the Unicode character (UTF-16)
            unsigned int c = 0;
            // For printable characters, the next message will be WM_CHAR.
            // This message contains the character code we need to send the KeyPressed event.
            // Inspired by the SDL 1.2 implementation.
            if (PeekMessage(&charMsg, s_WindowHandle, 0, 0, PM_NOREMOVE) && charMsg.message == WM_CHAR)
            {
                GetMessage(&charMsg, s_WindowHandle, 0, 0);
                c = static_cast<unsigned int>(charMsg.wParam);
            }
            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
            Th::KeyCode key = (Th::KeyCode)wParam;
            unsigned int scanCode = (lParam & 0x00FF0000) >> 16;
            KeyEventArgs keyEventArgs(key, c, KeyEventArgs::KeyState::Pressed, shift, control, alt);
            inputHandler.InvokeKeyPressedEvent(keyEventArgs);
        }
        break;
        case WM_SYSKEYUP:
        case WM_KEYUP:
        {
            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
            KeyCode key = (KeyCode)wParam;
            unsigned int c = 0;
            unsigned int scanCode = (lParam & 0x00FF0000) >> 16;

            // Determine which key was released by converting the key code and the scan code
            // to a printable character (if possible).
            // Inspired by the SDL 1.2 implementation.
            unsigned char keyboardState[256];
            GetKeyboardState(keyboardState);
            wchar_t translatedCharacters[4];
            if (int result = ToUnicodeEx(static_cast<UINT>(wParam), scanCode, keyboardState, translatedCharacters, 4, 0, NULL) > 0)
            {
                c = translatedCharacters[0];
            }

            KeyEventArgs keyEventArgs(key, c, KeyEventArgs::KeyState::Released, shift, control, alt);
            inputHandler.InvokeKeyReleasedEvent(keyEventArgs);
        }
        break;
        // The default window procedure will play a system notification sound 
        // when pressing the Alt+Enter keyboard combination if this message is 
        // not handled.
        case WM_SYSCHAR:
            break;
        case WM_MOUSEMOVE:
        {
            bool lButton = (wParam & MK_LBUTTON) != 0;
            bool rButton = (wParam & MK_RBUTTON) != 0;
            bool mButton = (wParam & MK_MBUTTON) != 0;
            bool shift = (wParam & MK_SHIFT) != 0;
            bool control = (wParam & MK_CONTROL) != 0;

            int x = ((int)(short)LOWORD(lParam));
            int y = ((int)(short)HIWORD(lParam));

            MouseMotionEventArgs mouseMotionEventArgs(lButton, mButton, rButton, control, shift, x, y);
            mouseMotionEventArgs.RelX = mouseMotionEventArgs.X - sOldMousePos.X;
            mouseMotionEventArgs.RelY = mouseMotionEventArgs.Y - sOldMousePos.Y;
            sOldMousePos = mouseMotionEventArgs;
        	inputHandler.InvokeMouseMotionEvent(mouseMotionEventArgs);
        }
        break;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        {
            bool lButton = (wParam & MK_LBUTTON) != 0;
            bool rButton = (wParam & MK_RBUTTON) != 0;
            bool mButton = (wParam & MK_MBUTTON) != 0;
            bool shift = (wParam & MK_SHIFT) != 0;
            bool control = (wParam & MK_CONTROL) != 0;

            int x = ((int)(short)LOWORD(lParam));
            int y = ((int)(short)HIWORD(lParam));

            MouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(message), MouseButtonEventArgs::ButtonState::Pressed, lButton, mButton, rButton, control, shift, x, y);
            inputHandler.InvokeMouseButtonPressedEvent(mouseButtonEventArgs);
        }
        break;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        {
            bool lButton = (wParam & MK_LBUTTON) != 0;
            bool rButton = (wParam & MK_RBUTTON) != 0;
            bool mButton = (wParam & MK_MBUTTON) != 0;
            bool shift = (wParam & MK_SHIFT) != 0;
            bool control = (wParam & MK_CONTROL) != 0;

            int x = ((int)(short)LOWORD(lParam));
            int y = ((int)(short)HIWORD(lParam));

            MouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(message), MouseButtonEventArgs::ButtonState::Released, lButton, mButton, rButton, control, shift, x, y);
            inputHandler.InvokeMouseButtonReleasedEvent(mouseButtonEventArgs);
        }
        break;
        case WM_MOUSEWHEEL:
        {
            // The distance the mouse wheel is rotated.
            // A positive value indicates the wheel was rotated to the right.
            // A negative value indicates the wheel was rotated to the left.
            float zDelta = ((int)(short)HIWORD(wParam)) / (float)WHEEL_DELTA;
            short keyStates = (short)LOWORD(wParam);

            bool lButton = (keyStates & MK_LBUTTON) != 0;
            bool rButton = (keyStates & MK_RBUTTON) != 0;
            bool mButton = (keyStates & MK_MBUTTON) != 0;
            bool shift = (keyStates & MK_SHIFT) != 0;
            bool control = (keyStates & MK_CONTROL) != 0;

            int x = ((int)(short)LOWORD(lParam));
            int y = ((int)(short)HIWORD(lParam));

            // Convert the screen coordinates to client coordinates.
            POINT clientToScreenPoint;
            clientToScreenPoint.x = x;
            clientToScreenPoint.y = y;
            ScreenToClient(s_WindowHandle, &clientToScreenPoint);

            MouseWheelEventArgs mouseWheelEventArgs(zDelta, lButton, mButton, rButton, control, shift, (int)clientToScreenPoint.x, (int)clientToScreenPoint.y);
            inputHandler.InvokeMouseWheelEvent(mouseWheelEventArgs);
        }
        break;
        case WM_SIZE:
        {
			int width = ((int)(short)LOWORD(lParam));
			int height = ((int)(short)HIWORD(lParam));

			ResizeEventArgs resizeEventArgs(width, height);
            EventMessenger::GetInstance().EvokeMessenger("OnWindowResizeRequest", resizeEventArgs);
        }
        break;

        default:
            // Handle any messages the switch statement didn't.
            return DefWindowProc(hWnd, message, wParam, lParam);

        }
    }

	Th::MouseButtonEventArgs::MouseButton Win32Window::DecodeMouseButton(UINT messageID)
	{
        MouseButtonEventArgs::MouseButton mouseButton = MouseButtonEventArgs::MouseButton::None;
        switch (messageID)
        {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        {
            mouseButton = MouseButtonEventArgs::MouseButton::Left;
        }
        break;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        {
            mouseButton = MouseButtonEventArgs::MouseButton::Right;
        }
        break;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        {
            mouseButton = MouseButtonEventArgs::MouseButton::Middel;
        }
        break;
        }

        return mouseButton;
	}

	

	void Win32Window::RegisterWindow(HINSTANCE appModule)
	{
		WNDCLASSEXW windowClass =
		{
			sizeof(WNDCLASSEXW),
			CS_HREDRAW | CS_VREDRAW,
			&Win32Window::WindowProc,
			0,
			0,
			appModule,
			LoadIcon(appModule, MAKEINTRESOURCE(101)),		//101 = the default window icon
			LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)(COLOR_WINDOW + 1),
			NULL,
			s_WindowClassName.c_str(),
			LoadIcon(appModule, MAKEINTRESOURCE(101))		//101 = the default window icon
		};

		static ATOM atom = RegisterClassExW(&windowClass);
		assert(atom > 0);
	}

	HWND Win32Window::CreateWindow(HINSTANCE appModule, UINT width, UINT height)
	{
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		int windowWidth = windowRect.right - windowRect.left;
		int windowHeight = windowRect.bottom - windowRect.top;

		int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
		int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

		HWND windowHandle = CreateWindowExW(
			NULL,
			s_WindowClassName.c_str(),
			s_WindowTitle.c_str(),
			WS_OVERLAPPEDWINDOW,
			windowX,
			windowY,
			windowWidth,
			windowHeight,
			NULL,
			NULL,
			appModule,
			nullptr
		);

		assert(windowHandle && "Failed to create render window!");
		return windowHandle;
	}

	HWND Win32Window::s_WindowHandle = nullptr;
	UINT Win32Window::s_WindowWidth = 0;
	UINT Win32Window::s_WindowHeight = 0;
	std::wstring Win32Window::s_WindowTitle = L"";
	const std::wstring Win32Window::s_WindowClassName = L"D3DApplicationWindow";
}