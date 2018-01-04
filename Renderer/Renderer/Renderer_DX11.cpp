#include "Renderer_DX11.h"

namespace Graphics
{

	Renderer_DX11::Renderer_DX11(const RendererInitializationInfo & ii)
		: settings(ii)
	{
	}


	Renderer_DX11::~Renderer_DX11()
	{
	}
	Graphics_Error Renderer_DX11::Initialize()
	{
		PASS_IF_ERROR(device.Init((HWND)settings.windowHandle, settings.windowState == WindowState::FULLSCREEN, settings.windowState == WindowState::FULLSCREEN_BORDERLESS, settings.bufferCount));

		RETURN_SUCCESS;
	}
	void Renderer_DX11::Shutdown()
	{
	}
	void Renderer_DX11::Pause()
	{
	}
	Graphics_Error Renderer_DX11::Start()
	{
		RETURN_SUCCESS;
	}
	Graphics_Error Renderer_DX11::UpdateSettings(const RendererInitializationInfo & ii)
	{
		RETURN_SUCCESS;
	}
	const RendererInitializationInfo & Renderer_DX11::GetSettings() const
	{
		return settings;
	}
}