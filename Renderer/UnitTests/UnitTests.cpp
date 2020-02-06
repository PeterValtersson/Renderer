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
	TEST_CLASS( UnitTests ){
public:
	TEST_METHOD( Create )
	{
		using namespace std::chrono_literals;
		HWND w;
		Window::InitWindow( w );
		{
			auto r = Renderer::Renderer_Interface::Create_Renderer( Renderer::Renderer_Backend::DIRECTX11, { w } );

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
			auto r = Renderer::Renderer_Interface::Create_Renderer( Renderer::Renderer_Backend::DIRECTX11, { w } );

			r->Start();
			struct CBD{
				int a = 1337;
			};
			r->UsePipelineHandler( []( Renderer::PipelineHandler_Interface& ph )
			{
				CBD d;
				Assert::ExpectException<Renderer::Could_Not_Create_Buffer>( [&]
				{
					ph.CreateBuffer( "Buffer", Renderer::Pipeline::Buffer::ConstantBuffer( d ) );
				} );
			} );

		}
		catch ( Renderer::Graphics_Exception & e )
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
			auto r = Renderer::Renderer_Interface::Create_Renderer( Renderer::Renderer_Backend::DIRECTX11, { w } );

			r->Start();
			struct CBD{
				int a = 1337;
				int pad[3];
			};
			r->UsePipelineHandler( []( Renderer::PipelineHandler_Interface& ph )
			{
				CBD d;
				ph.CreateBuffer( "Buffer", Renderer::Pipeline::Buffer::ConstantBuffer( d ) );

			} );

			int a = 1337;
			r->AddUpdateJob( Renderer::UpdateJob::UpdateBuffer( "Buffer", Renderer::UpdateFrequency::EVERY_FRAME, [&]( Renderer::UpdateObjectRef& obj )
			{
				obj.Map<CBD>( [&]( CBD& d )
				{
					d.a = a++;
				}, Renderer::AccessFlag::WRITE );

			} ), Renderer::RenderGroup::PRE_PASS_0 );

			while ( a < 1400 );
		}
		catch ( Renderer::Graphics_Exception & e )
		{
			DestroyWindow( w );
			Assert::IsTrue( false, Utilities::String::utf8_2_utf16( e.what() ).c_str() );
		}
		DestroyWindow( w );
	}

	TEST_METHOD( RenderTriangle )
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
			auto r = Renderer::Renderer_Interface::Create_Renderer( Renderer::Renderer_Backend::DIRECTX11, { w } );
			struct Vertex{
				float x, y, z;
				float r, g, b;
			};
			Vertex triangleData[] = {
				{-0.5f, -0.5f, 0, 1.0f, 0.0f, 0.0f},
				{0, 0.5f, 0, 1.0f, 0.0f, 0.0f},
				{0.5f, -0.5f, 0, 1.0f, 0.0f, 0.0f}
			};
			r->UsePipelineHandler( [&]( Renderer::PipelineHandler_Interface& ph )
			{
				ph.CreateBuffer( "Triangle", Renderer::Pipeline::Buffer::VertexBuffer( triangleData, sizeof( Vertex ), 3 ) );
				ph.CreateShader( "VertexShader", Renderer::Pipeline::ShaderType::VERTEX, vs, strlen( vs ), "ColorVertexShader", "vs_5_0" );
				ph.CreateShader( "PixelShader", Renderer::Pipeline::ShaderType::PIXEL, ps, strlen( ps ), "ColorPixelShader", "ps_5_0" );
				Renderer::Pipeline::RasterizerState rs;
				rs.cullMode = Renderer::Pipeline::CullMode::CULL_BACK;
				rs.fillMode = Renderer::Pipeline::FillMode::FILL_SOLID;
				rs.windingOrder = Renderer::Pipeline::WindingOrder::CLOCKWISE;
				ph.CreateRasterizerState( "Rasterizer", rs );
			} );

			Renderer::RenderJob job;
			job.pipeline.Edit( []( Renderer::Pipeline::Pipeline_Mutable pl )
			{
				pl.IAStage.vertexBuffer = "Triangle";
				pl.IAStage.topology = Renderer::Pipeline::PrimitiveTopology::TRIANGLE_LIST;
				pl.VSStage.shader = "VertexShader";
				pl.RStage.viewport = Renderer::Default_Viewport;
				pl.RStage.rasterizerState = "Rasterizer";
				pl.PSStage.shader = "PixelShader";
				pl.OMStage.clearTargets = true;
				pl.OMStage.renderTargetCount = 1;
				pl.OMStage.renderTargets[0] = Renderer::Default_RenderTarget;
			} );
			job.vertexCount = 3;
	
			r->AddRenderJob( "HelloTriangle", job, Renderer::RenderGroup::RENDER_PASS_0 );

			r->Start();
			std::this_thread::sleep_for( 1s );
		}
		DestroyWindow( w );
	}


	TEST_METHOD( MovingTriangle )
	{
		static const char* vs = R"(

		cbuffer ExtraPosBuffer
		{
			float extra_pos;
		};

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
			output.position.x += extra_pos;

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
			auto r = Renderer::Renderer_Interface::Create_Renderer( Renderer::Renderer_Backend::DIRECTX11, { w } );
			struct Vertex {
				float x, y, z;
				float r, g, b;
			};
			Vertex triangleData[] = {
				{-0.5f, -0.5f, 0, 1.0f, 0.0f, 0.0f},
				{0, 0.5f, 0, 1.0f, 0.0f, 0.0f},
				{0.5f, -0.5f, 0, 1.0f, 0.0f, 0.0f}
			};
			r->UsePipelineHandler( [&]( Renderer::PipelineHandler_Interface& ph )
			{
				ph.CreateBuffer( "Triangle", Renderer::Pipeline::Buffer::VertexBuffer( triangleData, sizeof( Vertex ), 3 ) );
				ph.CreateShader( "VertexShader", Renderer::Pipeline::ShaderType::VERTEX, vs, strlen( vs ), "ColorVertexShader", "vs_5_0" );
				ph.CreateShader( "PixelShader", Renderer::Pipeline::ShaderType::PIXEL, ps, strlen( ps ), "ColorPixelShader", "ps_5_0" );
				Renderer::Pipeline::RasterizerState rs;
				rs.cullMode = Renderer::Pipeline::CullMode::CULL_NONE;
				rs.fillMode = Renderer::Pipeline::FillMode::FILL_SOLID;
				rs.windingOrder = Renderer::Pipeline::WindingOrder::CLOCKWISE;
				ph.CreateRasterizerState( "Rasterizer", rs );
			} );

			Renderer::RenderJob job;
			job.pipeline.Edit( []( Renderer::Pipeline::Pipeline_Mutable pl )
			{
				pl.IAStage.vertexBuffer = "Triangle";
				pl.IAStage.topology = Renderer::Pipeline::PrimitiveTopology::TRIANGLE_LIST;
				pl.VSStage.shader = "VertexShader";
				pl.RStage.viewport = Renderer::Default_Viewport;
				pl.RStage.rasterizerState = "Rasterizer";
				pl.PSStage.shader = "PixelShader";
				pl.OMStage.clearTargets = true;
				pl.OMStage.renderTargetCount = 1;
				pl.OMStage.renderTargets[0] = Renderer::Default_RenderTarget;
			} );
			job.vertexCount = 3;

			r->AddRenderJob( "HelloTriangle", job, Renderer::RenderGroup::RENDER_PASS_0 );
			float pos = 0.0f;
			r->AddUpdateJob( Renderer::UpdateJob::UpdateBuffer( "ExtraPosBuffer", Renderer::UpdateFrequency::EVERY_FRAME, [&]( Renderer::UpdateObjectRef& obj ) 
			{
				obj.WriteTo( &pos, sizeof( pos ) );
				pos += 0.01f;
			} ), Renderer::RenderGroup::PRE_PASS_0);
			

			r->Start();
			while ( pos < 1.0f );
		}
		DestroyWindow( w );
	}
	};
}
