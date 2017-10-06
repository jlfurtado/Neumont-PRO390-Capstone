#include "EditorWindow.h"
#include "Utils.h"

namespace Capstone
{
	bool EditorWindow::MakeWindow(HINSTANCE instanceHandle, int nCmdShow, const char *const windowName, int screenWidth, int screenHeight)
	{
		if (!m_console.Initialize()) { return false; }

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
		if (!RegisterClassEx(&wc)) { printf("Failed to register window class\n"); return false; };

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

		if (!m_windowHandle) { printf("Failed to create window!\n"); return false; }

		// display the window on the screen
		ShowWindow(m_windowHandle, nCmdShow);

		m_instanceHandle = instanceHandle;
		printf("Made window!!!\n");
		return true;
	}

	int EditorWindow::Run()
	{
		m_editor.Initialize(m_instanceHandle, m_windowHandle);

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
			m_editor.Update(0.0f);
			m_editor.Render();
		}

		// free resources
		m_editor.Shutdown();
		m_console.Shutdown();

		// return this part of the WM_QUIT message to Windows
		return static_cast<int>(msg.wParam);
	}

	LRESULT CALLBACK EditorWindow::WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT paintStruct;
		HDC hdc;

		// sort through and find what code to run for the message given
		switch (message)
		{
			case WM_PAINT:
			{
				hdc = BeginPaint(windowHandle, &paintStruct);
				EndPaint(windowHandle, &paintStruct);
			} break;

			// this message is read when the window is closed
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