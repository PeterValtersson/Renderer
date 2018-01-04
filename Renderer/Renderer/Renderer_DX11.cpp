#include "Renderer_DX11.h"
#include <Profiler.h>
namespace Graphics
{

	Renderer_DX11::Renderer_DX11(const RendererInitializationInfo & ii)
		: settings(ii), running(false), initiated(false)
	{
		
	}


	Renderer_DX11::~Renderer_DX11()
	{
	}
	GRAPHICS_ERROR Renderer_DX11::Initialize()
	{
		StartProfile;
		if (initiated)
			RETURN_ERROR_C("Renderer has already been initiated");
		PASS_IF_ERROR(device.Init((HWND)settings.windowHandle, settings.windowState == WindowState::FULLSCREEN, settings.windowState == WindowState::FULLSCREEN_BORDERLESS, settings.bufferCount));
		initiated = true;
		RETURN_SUCCESS;
	}
	void Renderer_DX11::Shutdown()
	{
		StartProfile;
		if (running)
		{
			running = false;
			myThread.join();
		}
		device.Shutdown();
		initiated = false;
	}	

	float clearColor[5][4] = {
		{1, 0, 0, 1 },
		{ 1, 1, 0, 1 },
	{ 1, 0, 1, 1 },
	{ 0, 1, 0, 1 },
	{ 0, 0, 1, 1 }
	};
	int at = 0;
	void Renderer_DX11::Pause()
	{
	
		StartProfile;
		if (running)
		{
			running = false;
			myThread.join();
		}
		at = ++at % 5;
	}
	GRAPHICS_ERROR Renderer_DX11::Start()
	{
		StartProfile;
		if (running)
			RETURN_ERROR_C("Renderer already running");
		if (!initiated)
			RETURN_ERROR_C("Renderer must be initiated first");
		running = true;
		myThread = std::thread(&Renderer_DX11::Run, this);
		RETURN_SUCCESS;
	}
	GRAPHICS_ERROR Renderer_DX11::UpdateSettings(const RendererInitializationInfo & ii)
	{
		StartProfile;
		//if (settings.windowHandle != ii.windowHandle)
		//	RETURN_ERROR_C("Handle not the same");
		settings = ii;
		PASS_IF_ERROR(device.ResizeSwapChain((HWND)settings.windowHandle, settings.windowState == WindowState::FULLSCREEN, settings.windowState == WindowState::FULLSCREEN_BORDERLESS, settings.bufferCount));

		RETURN_SUCCESS;
	}
	const RendererInitializationInfo & Renderer_DX11::GetSettings() const
	{
		return settings;
	}
	PipelineHandler_Interface * Renderer_DX11::GetPipelineHandler() const
	{
		return nullptr;
	}
	void Renderer_DX11::Run()
	{
		StartProfile;
		while (running)
		{
			StartProfileC("Run_While");
			ResolveJobs();

			clearColor[at][0] =fmodf(clearColor[at][0] + 0.01f, 1.0f);

			BeginFrame();

			Frame();

			EndFrame();
		}
	}
	void Renderer_DX11::ResolveJobs()
	{
		StartProfile;
	}
	void Renderer_DX11::BeginFrame()
	{
		StartProfile;
		// clear the back buffer
	
	
		ID3D11RenderTargetView* views[] = { device.GetRTV() };
		device.GetDeviceContext()->OMSetRenderTargets(1, views, device.GetDepthStencil());


		// Clear the primary render target view using the specified color
		device.GetDeviceContext()->ClearRenderTargetView(device.GetRTV(), clearColor[at]);

		// Clear the standard depth stencil view
		device.GetDeviceContext()->ClearDepthStencilView(device.GetDepthStencil(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	}
	void Renderer_DX11::Frame()
	{
		StartProfile;
	}
	void Renderer_DX11::EndFrame()
	{
		StartProfile;
		device.Present(settings.vsync);
	}
}