#ifndef _GRAPHICS_RENDERER_DX11_H_
#define _GRAPHICS_RENDERER_DX11_H_
#include <Graphics\Renderer_Interface.h>
#include <Graphics\PipelineHandler_Interface.h>
#include "DeviceHandler.h"
#include <thread>
#include "PipelineAssigner.h"
#include <optional>
#include "CircularFIFO.h"

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

		PipelineHandler_Interface* GetPipelineHandler() override;

		GRAPHICS_ERROR AddRenderJob(Utilz::GUID id, const RenderJob& job, RenderGroup renderGroup) override;
		GRAPHICS_ERROR AddRenderJob(const RenderJob& job, RenderGroup renderGroup) override;
		void RemoveRenderJob(Utilz::GUID id, RenderGroup renderGroup) override;
		void RemoveRenderJob(Utilz::GUID id) override;
		uint32_t GetNumberOfRenderJobs()const override;
		uint8_t IsRenderJobRegistered(Utilz::GUID id)const override;
	private:
		RendererInitializationInfo settings;
		DeviceHandler* device;
		PipelineAssigner* pipeline;
		
		//***** Threading stuff and safety ******//
		std::thread myThread;
		bool running;
		bool initiated;

		void Run();
		void ResolveJobs();
		void BeginFrame();
		void Frame();
		void EndFrame();


		//***** Renderjob stuff ******//
		struct RenderJobInfoRenderSide
		{
			std::vector<std::vector<RenderJob>> renderGroupsWithJob;
			std::vector<std::vector<Utilz::GUID>> renderGroupsWithID;
			std::optional<uint32_t> FindRenderJob(Utilz::GUID id, RenderGroup group);
			inline std::vector<RenderJob>& GetJobs(RenderGroup group)
			{
				return renderGroupsWithJob[uint8_t(group)];
			}
			inline std::vector<Utilz::GUID>& GetIDs(RenderGroup group)
			{
				return renderGroupsWithID[uint8_t(group)];
			}
		}renderJobInfoRenderSide;

			//**** TO ADD/REMOVE STUFF ****//
			struct ToAdd
			{
				Utilz::GUID id;
				RenderGroup group;
				RenderJob job;
			};
			Utilz::CircularFiFo<ToAdd> jobsToAdd;
			struct ToRemove
			{
				Utilz::GUID id;
				RenderGroup group;
			};
			Utilz::CircularFiFo<ToRemove> jobsToRemove;
			//****					   ****//

		struct RenderJobInfoClientSide
		{
			std::vector<std::vector<Utilz::GUID>> renderGroupsWithID;
			std::optional<uint32_t> FindRenderJob(Utilz::GUID id, RenderGroup group);
			inline std::vector<Utilz::GUID>& GetIDs(RenderGroup group)
			{
				return renderGroupsWithID[uint8_t(group)];
			}

			
		}renderJobInfoClientSide;
	

	};
}
#endif