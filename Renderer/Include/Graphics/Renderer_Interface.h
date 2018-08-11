#ifndef _GRAPHICS_RENDERER_INTERFACE_H_
#define _GRAPHICS_RENDERER_INTERFACE_H_
#include "../DLLExport.h"
#include <Error.h>
#include "PipelineHandler_Interface.h"
#include <stdint.h>
#include "RenderJob.h"
#include "UpdateJob.h"

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

		virtual UERROR Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual void Pause() = 0;
		virtual UERROR Start() = 0;

		virtual UERROR UpdateSettings(const RendererInitializationInfo& ii) = 0;
		virtual const RendererInitializationInfo& GetSettings()const = 0;

		virtual PipelineHandler_Interface* GetPipelineHandler() = 0;

		virtual UERROR AddRenderJob(Utilities::GUID id, const RenderJob& job, RenderGroup renderGroup) = 0;
		// Will use the id of the pipeline
		virtual UERROR AddRenderJob(const RenderJob& job, RenderGroup renderGroup) = 0;
		virtual void RemoveRenderJob(Utilities::GUID id, RenderGroup renderGroup) = 0;
		virtual void RemoveRenderJob(Utilities::GUID id) = 0;
		virtual uint32_t GetNumberOfRenderJobs()const = 0;
		virtual uint8_t IsRenderJobRegistered(Utilities::GUID id)const = 0;

		virtual UERROR AddUpdateJob(Utilities::GUID id, const UpdateJob& job, RenderGroup renderGroupToPerformUpdateBefore) = 0;
		// Will use the id of the object
		virtual UERROR AddUpdateJob(const UpdateJob& job, RenderGroup renderGroup) = 0;
		virtual void RemoveUpdateJob(Utilities::GUID id, RenderGroup renderGroupToPerformUpdateBefore) = 0;
		virtual void RemoveUpdateJob(Utilities::GUID id) = 0;
		virtual uint32_t GetNumberOfUpdateJobs()const = 0;
		virtual uint8_t IsUpdateJobRegistered(Utilities::GUID id)const = 0;
	protected:
		Renderer_Interface() {};
	};

	DECLDIR_GRAPHICS Renderer_Interface* Get();
}
enum class Renderer_Backend
{
	DIRECTX11
};
DECLDIR_GRAPHICS_C Graphics::Renderer_Interface* CreateRenderer(Renderer_Backend backend, const Graphics::RendererInitializationInfo & ii);
DECLDIR_GRAPHICS_C Utilities::Error Renderer_Initialize_C(Graphics::Renderer_Interface* r);
DECLDIR_GRAPHICS_C void Renderer_Shutdown_C(Graphics::Renderer_Interface* r);
DECLDIR_GRAPHICS_C void Renderer_Pause_C(Graphics::Renderer_Interface* r);
DECLDIR_GRAPHICS_C Utilities::Error Renderer_Start_C(Graphics::Renderer_Interface* r);

DECLDIR_GRAPHICS_C Utilities::Error Renderer_UpdateSettings_C(Graphics::Renderer_Interface* r, Graphics::RendererInitializationInfo ii);
DECLDIR_GRAPHICS_C Graphics::RendererInitializationInfo Renderer_GetSettings_C(Graphics::Renderer_Interface* r);

#endif
