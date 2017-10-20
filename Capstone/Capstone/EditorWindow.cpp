#include "EditorWindow.h"
#include "Utils.h"
#include "ElapsedTime.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "CommandProcessor.h"

namespace Capstone
{
	EditorWindow *EditorWindow::s_pFirst = nullptr;

	bool EditorWindow::MakeWindow(HINSTANCE instanceHandle, int nCmdShow, const char *const windowName, int screenWidth, int screenHeight)
	{
		if (s_pFirst == nullptr) { s_pFirst = this; }
		if (!m_console.Initialize(true)) { return false; }
		
		m_width = screenWidth;
		m_height = screenHeight;

		// this struct holds information for the window class
		WNDCLASSEX wc;
		MyUtils::MyClearFunc(&wc); // zero-out all the memory 

		// fill in the struct with the needed information
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = EditorWindow::WindowProc;
		wc.hInstance = instanceHandle;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		//wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.lpszClassName = WINDOW_CLASS_NAME;

		// register the window class
		if (!RegisterClassEx(&wc)) { DebugConsole::Log("code %d\n", GetLastError()); DebugConsole::Log("Failed to register window class\n"); return false; };

		RECT windowRect = { 0, 0, screenWidth, screenHeight }; // set size and not position
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE); // adjust the size

		// create the window and use the result as the handle
		m_windowHandle = CreateWindowEx(NULL,
			WINDOW_CLASS_NAME,    // name of the window class
			windowName,   // title of the window
			WS_OVERLAPPEDWINDOW,    // window style
			300,    // x-position of the window
			300,    // y-position of the window
			windowRect.right - windowRect.left,    // width of the window
			windowRect.bottom - windowRect.top,    // height of the window
			NULL,    // we have no parent window, NULL
			NULL,    // we aren't using menus, NULL
			instanceHandle,    // application handle
			NULL);    // used with multiple windows, NULL

		if (!m_windowHandle) { DebugConsole::Log("Failed to create window!\n"); return false; }

		// display the window on the screen
		ShowWindow(m_windowHandle, nCmdShow);

		m_instanceHandle = instanceHandle;
		DebugConsole::Log("Made window!!!\n");
		return true;
	}

	int EditorWindow::Run()
	{
		CommandProcessor::Initialize(this);
		ElapsedTime::Initialize();
		m_editor.Initialize(m_instanceHandle, m_windowHandle, this);
		Keyboard::Initialize();
		Mouse::Initialize(Editor::OnMouseScroll, &m_editor);

		// this struct holds Windows event messages
		MSG msg = { 0 };

		// wait for the next message in the queue, store the result in 'msg'
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				// translate keystroke messages into the right format
				TranslateMessage(&msg);

				// send the message to the WindowProc function
				DispatchMessage(&msg);
			}

			// update 'n draw
			Keyboard::Update();
			POINT cursorPos;
			GetCursorPos(&cursorPos);
			ScreenToClient(m_windowHandle, &cursorPos);
			Mouse::Update(cursorPos.x, cursorPos.y);
			m_editor.Update(ElapsedTime::GetLastFrameTime());
			m_editor.Render();
		}

		// free resources
		m_editor.Shutdown();
		m_console.Shutdown();
		CommandProcessor::Shutdown();

		// return this part of the WM_QUIT message to Windows
		return static_cast<int>(msg.wParam);
	}

	int EditorWindow::GetWidth()
	{
		return m_width;
	}

	int EditorWindow::GetHeight()
	{
		return m_height;
	}

	void EditorWindow::CloseWindow()
	{
		PostMessage(m_windowHandle, WM_CLOSE, NULL, NULL);
	}

	LRESULT CALLBACK EditorWindow::WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT paintStruct;
		HDC hdc;

		// sort through and find what code to run for the message given
		switch (message)
		{
			// draw the application
			case WM_PAINT:
			{
				hdc = BeginPaint(windowHandle, &paintStruct);
				EndPaint(windowHandle, &paintStruct);
			} break;

			// keyboard events
			case WM_KEYDOWN: { Keyboard::KeyHit(wParam); } break;
			case WM_KEYUP:   { Keyboard::KeyRelease(wParam); } break;

			// lose keyboard focus
			case WM_KILLFOCUS: { Keyboard::ClearAll(); Mouse::ClearAll(); } break;

			// mouse click events
			case WM_LBUTTONDOWN: { Mouse::Clicked(Mouse::LEFT_MOUSE_INDEX);	} break;
			case WM_LBUTTONUP: { Mouse::Released(Mouse::LEFT_MOUSE_INDEX); } break;
			case WM_MBUTTONDOWN: { Mouse::Clicked(Mouse::MIDDLE_MOUSE_INDEX); } break;
			case WM_MBUTTONUP: { Mouse::Released(Mouse::MIDDLE_MOUSE_INDEX); } break;
			case WM_RBUTTONDOWN: { Mouse::Clicked(Mouse::RIGHT_MOUSE_INDEX); } break;
			case WM_RBUTTONUP: { Mouse::Released(Mouse::RIGHT_MOUSE_INDEX);	} break;

			// mouse scroll event
			case WM_MOUSEWHEEL:	{ Mouse::Scroll(GET_WHEEL_DELTA_WPARAM(wParam)); } break;

			// window resize
			case WM_SIZE:
			{
				if (s_pFirst)
				{
					RECT rect;
					GetClientRect(windowHandle, &rect);
					s_pFirst->m_width = rect.right - rect.left;
					s_pFirst->m_height = rect.bottom - rect.top;
					s_pFirst->m_editor.OnWindowResize();
				}
			} break;

			// Exit window
			case WM_DESTROY:
			{
				// close the application entirely
				PostQuitMessage(0);
				return 0;
			} break;

			default:
				// Handle any messages the switch statement didn't
				return DefWindowProc(windowHandle, message, wParam, lParam);
		}

		return 0;
	}
}