#include "stdafx.h"
#include "CppUnitTest.h"
#include <Windows.h>
#include "../Include/Graphics/Renderer_Interface.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <thread>
using namespace std::chrono_literals;

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		break;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
	}
	return 0;
}

void InitWindow(HWND& _hWnd)
{
	// Setup the windows class
	WNDCLASSEX wc;

	auto _hInst = GetModuleHandle(NULL);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _hInst;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"TEST";
	wc.cbSize = sizeof(WNDCLASSEX);

	LONG windowWidth = 300;
	LONG windowHeight = 300;


	// Register the window class.
	//Place the window in the middle of the Window.
	auto _windowPosX = (GetSystemMetrics(SM_CXSCREEN) - (int)windowWidth) / 2;
	auto _windowPosY = (GetSystemMetrics(SM_CYSCREEN) - (int)windowHeight) / 2;

	RegisterClassEx(&wc);
	RECT rc = { 0, 0, (LONG)windowWidth, (LONG)windowHeight };

	
	// Create the window with the Window settings and get the handle to it.
	_hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		L"TEST",
		L"TEST",
		0,
		_windowPosX,
		_windowPosY,
		rc.right - rc.left,
		rc.bottom - rc.top,
		NULL,
		NULL,
		_hInst,
		NULL);

	// Bring the window up on the Window and set it as main focus.
	ShowWindow(_hWnd, SW_SHOW);
	SetForegroundWindow(_hWnd);
	SetFocus(_hWnd);
}



namespace UnitTest1
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			HWND hwnd;
			InitWindow(hwnd);
			auto r = CreateRenderer(Renderer_Backend::DIRECTX11, { hwnd });
			auto res = Renderer_Initialize_C(r);
			Assert::AreEqual(0, res.errornr);
			res = Renderer_Start_C(r);
			Assert::AreEqual(0, res.errornr);
			MSG msg;
			for( int i = 0; i< 300; i++)
			{
				
				// Handle the windows messages.
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			
			
			}
			Renderer_Pause_C(r);
			res = Renderer_UpdateSettings_C(r, { hwnd });
			Assert::AreEqual(0, res.errornr);
			res = Renderer_Start_C(r);
			Assert::AreEqual(0, res.errornr);
				for (int i = 0; i< 300; i++)
				{

					// Handle the windows messages.
					while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					std::this_thread::sleep_for(10ms);

				}
		
			Renderer_Shutdown_C(r);
		}

	};
}

