#ifndef _GRAPHICS_RENDERER_DX11_H_
#define _GRAPHICS_RENDERER_DX11_H_
#include <Graphics\Renderer_Interface.h>
#include <Graphics\PipelineHandler_Interface.h>
#include "DeviceHandler.h"
#include <thread>
#include "PipelineAssigner.h"
#include <optional>
#include <Utilities/CircularFIFO.h>
#include "Job.h"

namespace Graphics
{

	class Renderer_DX11 : public Renderer_Interface{
	public:
		Renderer_DX11( const RendererInitializationInfo& ii );
		~Renderer_DX11()noexcept;

		virtual void Pause()noexcept override;
		virtual void Start()noexcept override;

		virtual void UpdateSettings( const RendererInitializationInfo& ii ) override;
		virtual const RendererInitializationInfo& GetSettings()const noexcept override;

		virtual void UsePipelineHandler( const std::function<void( PipelineHandler_Interface & pipeline_handler )>& callback ) override;

		virtual void AddRenderJob( Utilities::GUID id, const RenderJob& job, RenderGroup renderGroup ) override;
		// Will use the id of the pipeline
		virtual void AddRenderJob( const RenderJob& job, RenderGroup renderGroup ) override;
		virtual void RemoveRenderJob( Utilities::GUID id, RenderGroup renderGroup )noexcept override;
		virtual void RemoveRenderJob( Utilities::GUID id )noexcept override;
		virtual size_t GetNumberOfRenderJobs()const noexcept override;
		// Returns the number of groups this job is registered under.
		virtual size_t IsRenderJobRegistered( Utilities::GUID id )const noexcept override;

		virtual void AddUpdateJob( Utilities::GUID id, const UpdateJob& job, RenderGroup renderGroupToPerformUpdateBefore ) override;
		// Will use the id of the object
		virtual void AddUpdateJob( const UpdateJob& job, RenderGroup renderGroup ) override;
		virtual void RemoveUpdateJob( Utilities::GUID id, RenderGroup renderGroupToPerformUpdateBefore )noexcept override;
		virtual void RemoveUpdateJob( Utilities::GUID id )noexcept override;
		virtual size_t GetNumberOfUpdateJobs()const noexcept override;
		// Returns the number of groups this job is registered under.
		virtual size_t IsUpdateJobRegistered( Utilities::GUID id )const noexcept override;
	private:
		void PerformRenderJob( const RenderJob& job )noexcept;
	private:
		RendererInitializationInfo settings;
		std::unique_ptr<DeviceHandler> device_handler;
		std::unique_ptr<PipelineAssigner> pipeline;

		//***** Threading stuff and safety ******//
		std::thread myThread;
		bool running;

		void Run()noexcept;
		void ResolveJobs()noexcept;
		void BeginFrame()noexcept;
		void Frame()noexcept;
		void EndFrame()noexcept;

		//***** Renderjob stuff ******//
		Job<RenderJob> renderJobs;
		Job<UpdateJob> updateJobs;
	};
}
#endif