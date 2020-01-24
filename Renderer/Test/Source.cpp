#include <Windows.h>
#include <Graphics/Renderer_Interface.h>
#include <thread>
#include <Utilities/StringUtilities.h>

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

		LONG windowWidth = 1280;
		LONG windowHeight = 720;


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


int main()
{
	static const char* vs = R"(

		struct VertexInputType
		{
			float3 position : POSITION;
			float3 color : COLOR;
		};

		struct PixelInputType
		{
			float4 position : SV_POSITION;
			float4 color : COLOR;
		};	

		PixelInputType ColorVertexShader(VertexInputType input)
		{
			PixelInputType output;
    

			// Change the position vector to be 4 units for proper matrix calculations.
			output.position.w = 1.0f;

			output.position.xyz = input.position;

			// Store the input color for the pixel shader to use.
			output.color.rgb = input.color;
			output.color.a = 1.0f;
			return output;
		}


		)";
	static const char* ps = R"(

		struct PixelInputType
		{
			float4 position : SV_POSITION;
			float4 color : COLOR;
		};


		////////////////////////////////////////////////////////////////////////////////
		// Pixel Shader
		////////////////////////////////////////////////////////////////////////////////
		float4 ColorPixelShader(PixelInputType input) : SV_TARGET
		{
			return input.color;
		}


		)";

	using namespace std::chrono_literals;
	HWND w;
	Window::InitWindow( w );
	{
		auto r = Graphics::Renderer_Interface::Create_Renderer( Graphics::Renderer_Backend::DIRECTX11, { w ,Graphics::WindowState::FULLSCREEN } );
		struct Vertex{
			float x, y, z;
			float r, g, b;
		};
		Vertex triangleData[] = {
			{-0.5f, -0.5f, 0, 1.0f, 0.0f, 0.0f},
			{0, 0.5f, 0, 1.0f, 0.0f, 0.0f},
			{0.5f, -0.5f, 0, 1.0f, 0.0f, 0.0f}
		};
		r->UsePipelineHandler( [&]( Graphics::PipelineHandler_Interface& ph )
		{
			ph.CreateBuffer( "Triangle", Graphics::Pipeline::Buffer::VertexBuffer( triangleData, sizeof( Vertex ), 3 ) );
			ph.CreateShader( "VertexShader", Graphics::Pipeline::ShaderType::VERTEX, vs, strlen( vs ), "ColorVertexShader", "vs_5_0" );
			ph.CreateShader( "PixelShader", Graphics::Pipeline::ShaderType::PIXEL, ps, strlen( ps ), "ColorPixelShader", "ps_5_0" );
			Graphics::Pipeline::RasterizerState rs;
			rs.cullMode = Graphics::Pipeline::CullMode::CULL_BACK;
			rs.fillMode = Graphics::Pipeline::FillMode::FILL_SOLID;
			rs.windingOrder = Graphics::Pipeline::WindingOrder::CLOCKWISE;
			ph.CreateRasterizerState( "Rasterizer", rs );
		} );

		Graphics::RenderJob job;
		job.pipeline.Edit( []( Graphics::Pipeline::Pipeline_Mutable pl )
		{
			pl.IAStage.vertexBuffer = "Triangle";
			pl.IAStage.topology = Graphics::Pipeline::PrimitiveTopology::TRIANGLE_LIST;
			pl.VSStage.shader = "VertexShader";
			pl.RStage.viewport = Graphics::Default_Viewport;
			pl.RStage.rasterizerState = "Rasterizer";
			pl.PSStage.shader = "PixelShader";
			pl.OMStage.clearTargets = true;
			pl.OMStage.renderTargetCount = 1;
			pl.OMStage.renderTargets[0] = Graphics::Default_RenderTarget;
		} );
		job.vertexCount = 3;

		r->AddRenderJob( "HelloTriangle", job, Graphics::RenderGroup::RENDER_PASS_0 );

		r->Start();
		std::this_thread::sleep_for( 1s );
	}
	DestroyWindow( w );
}