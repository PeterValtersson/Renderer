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
	Graphics_Error Renderer_DX11::Initialize()
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
	void Renderer_DX11::Pause()
	{
		StartProfile;
		if (running)
		{
			running = false;
			myThread.join();
		}
	}
	Graphics_Error Renderer_DX11::Start()
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
	Graphics_Error Renderer_DX11::UpdateSettings(const RendererInitializationInfo & ii)
	{
		StartProfile;
		PASS_IF_ERROR(device.ResizeSwapChain((HWND)settings.windowHandle, settings.windowState == WindowState::FULLSCREEN, settings.windowState == WindowState::FULLSCREEN_BORDERLESS, settings.bufferCount));

		RETURN_SUCCESS;
	}
	const RendererInitializationInfo & Renderer_DX11::GetSettings() const
	{
		return settings;
	}
	void Renderer_DX11::Run()
	{
		StartProfile;
		while (running)
		{
			StartProfileC("Run_While");
			ResolveJobs();

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
		float clearColor[] = { 0, 0, 0, 1 };

		ID3D11RenderTargetView* views[] = { device.GetRTV() };
		device.GetDeviceContext()->OMSetRenderTargets(1, views, device.GetDepthStencil());


		// Clear the primary render target view using the specified color
		device.GetDeviceContext()->ClearRenderTargetView(device.GetRTV(), clearColor);

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