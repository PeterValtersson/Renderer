#ifndef _GRAPHICS_RENDERER_DX11_H_
#define _GRAPHICS_RENDERER_DX11_H_
#include <Graphics\Renderer_Interface.h>
#include "DeviceHandler.h"
#include <thread>
namespace Graphics
{
	class Renderer_DX11 : public Renderer_Interface
	{
	public:
		Renderer_DX11(const RendererInitializationInfo & ii);
		~Renderer_DX11();

		GRAPHICS_ERROR Initialize() override;
		void Shutdown()  override;

		void Pause()  override;
		GRAPHICS_ERROR Start()  override;

		GRAPHICS_ERROR UpdateSettings(const RendererInitializationInfo& ii)  override;
		const RendererInitializationInfo& GetSettings()const  override;

		PipelineHandler_Interface* GetPipelineHandler()const override;
	private:
		RendererInitializationInfo settings;
		DeviceHandler device;
		std::thread myThread;
		bool running;
		bool initiated;


		void Run();
		void ResolveJobs();
		void BeginFrame();
		void Frame();
		void EndFrame();
	};
}
#endif