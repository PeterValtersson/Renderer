#include "pch.h"
#include "CppUnitTest.h"
#include <Windows.h>
#include <Graphics/Renderer_Interface.h>
#include <thread>
#include <Utilities/StringUtilities.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	namespace Window
	{

		LRESULT CALLBACK WndProc( HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam )
		{
			switch ( umessage )
			{
				// Check if the window is being destroyed.
			case WM_DESTROY:
			{
				PostQuitMessage( 0 );
				break;
			}

			// Check if the window is being closed.
			case WM_CLOSE:
			{
				PostQuitMessage( 0 );
				break;
			}

			// All other messages pass to the message handler in the system class.
			default:
			{
				return DefWindowProc( hwnd, umessage, wparam, lparam );
			}
			}
			return 0;
		}

		void InitWindow( HWND& _hWnd )
		{
			// Setup the windows class
			WNDCLASSEX wc;

			auto _hInst = GetModuleHandle( NULL );
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = WndProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = _hInst;
			wc.hIcon = LoadIcon( NULL, IDI_WINLOGO );
			wc.hIconSm = wc.hIcon;
			wc.hCursor = LoadCursor( NULL, IDC_ARROW );
			wc.hbrBackground = ( HBRUSH )GetStockObject( BLACK_BRUSH );
			wc.lpszMenuName = NULL;
			wc.lpszClassName = L"TEST";
			wc.cbSize = sizeof( WNDCLASSEX );

			LONG windowWidth = 300;
			LONG windowHeight = 300;


			// Register the window class.
			//Place the window in the middle of the Window.
			auto _windowPosX = ( GetSystemMetrics( SM_CXSCREEN ) - ( int )windowWidth ) / 2;
			auto _windowPosY = ( GetSystemMetrics( SM_CYSCREEN ) - ( int )windowHeight ) / 2;

			RegisterClassEx( &wc );
			RECT rc = { 0, 0, ( LONG )windowWidth, ( LONG )windowHeight };


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
				NULL );

			// Bring the window up on the Window and set it as main focus.
			ShowWindow( _hWnd, SW_SHOW );
			SetForegroundWindow( _hWnd );
			SetFocus( _hWnd );
		}
	}
	TEST_CLASS( UnitTests ){
public:

	TEST_METHOD( Create )
	{
		using namespace std::chrono_literals;
		HWND w;
		Window::InitWindow( w );
		{
			auto r = Graphics::Renderer_Interface::Create_Renderer( Graphics::Renderer_Backend::DIRECTX11, { w } );

			r->Start();
			std::this_thread::sleep_for( 1s );
		}
		DestroyWindow( w );
	}
	TEST_METHOD( CreateBuffer_16Byte_Mod )
	{
		using namespace std::chrono_literals;
		HWND w;
		Window::InitWindow( w );
		try
		{
			auto r = Graphics::Renderer_Interface::Create_Renderer( Graphics::Renderer_Backend::DIRECTX11, { w } );

			r->Start();
			struct CBD{
				int a = 1337;
			};
			r->UsePipelineHandler( []( Graphics::PipelineHandler_Interface& ph )
			{
				CBD d;
				Assert::ExpectException<Graphics::Could_Not_Create_Buffer>( [&]
				{
					ph.CreateBuffer( "Buffer", Graphics::Pipeline::Buffer::ConstantBuffer( d ) );
				} );
			} );

		}
		catch ( Graphics::Graphics_Exception & e )
		{
			DestroyWindow( w );
			Assert::IsTrue( false, Utilities::String::utf8_2_utf16( e.what() ).c_str() );
		}
		DestroyWindow( w );
	}
	TEST_METHOD( CreateBuffer )
	{
		using namespace std::chrono_literals;
		HWND w;
		Window::InitWindow( w );
		try
		{
			auto r = Graphics::Renderer_Interface::Create_Renderer( Graphics::Renderer_Backend::DIRECTX11, { w } );

			r->Start();
			struct CBD{
				int a = 1337;
				int pad[3];
			};
			r->UsePipelineHandler( []( Graphics::PipelineHandler_Interface& ph )
			{
				CBD d;
				ph.CreateBuffer( "Buffer", Graphics::Pipeline::Buffer::ConstantBuffer( d ) );

			} );

			int a = 1337;
			r->AddUpdateJob( Graphics::UpdateJob::UpdateBuffer( "Buffer", Graphics::UpdateFrequency::EVERY_FRAME, [&]( Graphics::UpdateObjectRef& obj )
			{
				obj.Map<CBD>( [&]( CBD& d )
				{
					d.a = a++;
				}, Graphics::AccessFlag::WRITE );

			} ), Graphics::RenderGroup::PRE_PASS_0 );
			
			while ( a < 1400 );
		}
		catch ( Graphics::Graphics_Exception & e )
		{
			DestroyWindow( w );
			Assert::IsTrue( false, Utilities::String::utf8_2_utf16( e.what() ).c_str() );
		}
		DestroyWindow( w );
	}
	};
}