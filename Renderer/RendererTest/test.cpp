#include "pch.h"
#include <Windows.h>
#include <Graphics/Renderer_Interface.h>
#include <thread>
using namespace std::chrono_literals;
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

TEST(RendererTest, Create) {
	using namespace std::chrono_literals;
	HWND w = 0;
	Window::InitWindow( w );
	/*auto r = Graphics::Renderer_Interface::Create_Renderer( Graphics::Renderer_Backend::DIRECTX11, { w } );

	r->Start();
	std::this_thread::sleep_for( 1s );*/
}

TEST(RendererTest, CreateBuffers) {
	HWND w;
	InitWindow(w);
	auto r = CreateRenderer(Renderer_Backend::DIRECTX11, { w });
	auto re = Renderer_Initialize_C(r);
	EXPECT_EQ(re.hash, "Success"_hash);

	re = Renderer_Start_C(r);
	EXPECT_EQ(re.hash, "Success"_hash);
	struct TestData
	{
		int testInt = 1337;
		int pad;
		int pad1;
		int pad2;
	}testData;
	re = r->GetPipelineHandler()->CreateBuffer("ConstB", Graphics::Pipeline::Buffer::ConstantBuffer(sizeof(TestData), &testData));
	EXPECT_EQ(re.hash, "Success"_hash);
	Graphics::Pipeline::Buffer bb = Graphics::Pipeline::Buffer::ConstantBuffer(sizeof(TestData), &testData);
	bb.flags |= Graphics::Pipeline::BufferFlags::CPU_READ;
	re = r->GetPipelineHandler()->CreateBuffer("ConstBRead", Graphics::Pipeline::Buffer::ConstantBuffer(sizeof(TestData), &testData));
	EXPECT_EQ(re.hash, "Success"_hash);
	float p[3] = { 1.0f, 0.0f,0.0f };
	re = r->GetPipelineHandler()->CreateBuffer("VertexB", Graphics::Pipeline::Buffer::VertexBuffer(p, sizeof(p), 1));
	EXPECT_EQ(re.hash, "Success"_hash);

	bool called1 = false;
	bool called2= false;
	auto l = [&](Graphics::UpdateObject* buffer) {
		
		try
		{
			called2 = true;
			EXPECT_EQ(buffer->GetMapObject<TestData>().testInt, testData.testInt);
		}
		catch (UERROR err)
		{
			called1 = true;
			EXPECT_NE(err.hash, "Success"_hash);
		}
		RETURN_SUCCESS;
	};
	re = r->AddUpdateJob("TestMap", Graphics::UpdateJob::Buffer("ConstB", Graphics::UpdateFrequency::EVERY_FRAME,l), Graphics::RenderGroup::PRE_PASS_0);
	EXPECT_EQ(re.hash, "Success"_hash);
	re = r->AddUpdateJob("TestMap2", Graphics::UpdateJob::Buffer("ConstBRead", Graphics::UpdateFrequency::EVERY_FRAME, l), Graphics::RenderGroup::PRE_PASS_0);
	EXPECT_EQ(re.hash, "Success"_hash);
	std::this_thread::sleep_for(1s);
	EXPECT_TRUE(called1); 
	EXPECT_TRUE(called2);

	r->GetPipelineHandler()->DestroyBuffer("ConstB");
	r->GetPipelineHandler()->DestroyBuffer("ConstBRead");



	Renderer_Shutdown_C(r);
}


TEST(RendererTest, Get) {

	HWND w;
	InitWindow(w);
	auto r = CreateRenderer(Renderer_Backend::DIRECTX11, { w });
	auto re = Renderer_Initialize_C(r);
	EXPECT_EQ(re.hash, "Success"_hash);

	re = Renderer_Start_C(r);
	EXPECT_EQ(re.hash, "Success"_hash);

	auto nr = Graphics::Get();

}