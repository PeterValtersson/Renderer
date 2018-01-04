#ifndef _GRAPHICS_RENDERER_INTERFACE_H_
#define _GRAPHICS_RENDERER_INTERFACE_H_
#include "../DLLExport.h"
#include "../Graphics_Error.h"

#include <stdint.h>

namespace Graphics
{
	enum class WindowState : uint8_t
	{
		WINDOWED,
		FULLSCREEN,
		FULLSCREEN_BORDERLESS
	};
	struct Resolution
	{
		uint32_t width;
		uint32_t height;
	};
	struct RendererInitializationInfo
	{
		void* windowHandle = nullptr;
		WindowState windowState = WindowState::WINDOWED;
		uint32_t bufferCount = 2;
		Resolution resolution = { 1280, 720 };
		uint8_t vsync = 1Ui8;
	};
	class Renderer_Interface
	{
	public:
		virtual ~Renderer_Interface() {};

		virtual Graphics_Error Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual void Pause() = 0;
		virtual Graphics_Error Start() = 0;

		virtual Graphics_Error UpdateSettings(const RendererInitializationInfo& ii) = 0;
		virtual const RendererInitializationInfo& GetSettings()const = 0;
	protected:
		Renderer_Interface() {};
	};
}
enum class Renderer_Backend
{
	DIRECTX11
};
DECLDIR_GRAPHICS_C Graphics::Renderer_Interface* CreateRenderer(Renderer_Backend backend, const Graphics::RendererInitializationInfo & ii);
DECLDIR_GRAPHICS_C Graphics::Graphics_Error Renderer_Initialize_C(Graphics::Renderer_Interface* r);
DECLDIR_GRAPHICS_C void Renderer_Shutdown_C(Graphics::Renderer_Interface* r);
DECLDIR_GRAPHICS_C void Renderer_Pause_C(Graphics::Renderer_Interface* r);
DECLDIR_GRAPHICS_C Graphics::Graphics_Error Renderer_Start_C(Graphics::Renderer_Interface* r);

DECLDIR_GRAPHICS_C Graphics::Graphics_Error Renderer_UpdateSettings_C(Graphics::Renderer_Interface* r, Graphics::RendererInitializationInfo ii);
DECLDIR_GRAPHICS_C Graphics::RendererInitializationInfo Renderer_GetSettings_C(Graphics::Renderer_Interface* r);

#endif
