#ifndef _GRAPHICS_RENDERER_DX11_H_
#define _GRAPHICS_RENDERER_DX11_H_
#include <Graphics\Renderer_Interface.h>
#include <Graphics\PipelineHandler_Interface.h>
#include "DeviceHandler.h"
#include <thread>
#include "PipelineAssigner.h"
#include <optional>
#include "CircularFIFO.h"
#include <Profiler.h>
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

		GRAPHICS_ERROR AddRenderJob(Utilities::GUID id, const RenderJob& job, RenderGroup renderGroup) override;
		GRAPHICS_ERROR AddRenderJob(const RenderJob& job, RenderGroup renderGroup) override;
		void RemoveRenderJob(Utilities::GUID id, RenderGroup renderGroup) override;
		void RemoveRenderJob(Utilities::GUID id) override;
		uint32_t GetNumberOfRenderJobs()const override;
		uint8_t IsRenderJobRegistered(Utilities::GUID id)const override;

		GRAPHICS_ERROR AddUpdateJob(Utilities::GUID id, const UpdateJob& job, RenderGroup renderGroupToPerformUpdateBefore) override;
		GRAPHICS_ERROR AddUpdateJob(const UpdateJob& job, RenderGroup renderGroupToPerformUpdateBefore) override;
		void RemoveUpdateJob(Utilities::GUID id, RenderGroup renderGroup) override;
		void RemoveUpdateJob(Utilities::GUID id) override;
		uint32_t GetNumberOfUpdateJobs()const override;
		uint8_t IsUpdateJobRegistered(Utilities::GUID id)const override;
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
		
		template<class JOB>
		struct JobStuff
		{
			//**** TO ADD/REMOVE STUFF ****//
			template<class JOB>
			struct ToAdd
			{
				Utilities::GUID id;
				RenderGroup group;
				JOB job;
			};
			Utilities::CircularFiFo<ToAdd<JOB>> jobsToAdd;
			struct ToRemove
			{
				Utilities::GUID id;
				RenderGroup group;
			};
			Utilities::CircularFiFo<ToRemove> jobsToRemove;
			//****					   ****//

			void Add()
			{
				StartProfile;
				while (!jobsToAdd.wasEmpty())
				{
					StartProfileC("Add Job");
					renderSide.Add(jobsToAdd.top());
					jobsToAdd.pop();
				}
			}
			void Remove()
			{
				StartProfile;
				while (!jobsToRemove.wasEmpty())
				{
					StartProfileC("Remove Render Job");
					renderSide.Remove(jobsToRemove.top());
					jobsToRemove.pop();
				}
			}

			template<class JOB>
			struct RenderSide
			{
				std::vector<std::vector<JOB>> renderGroupsWithJob;
				std::vector<std::vector<Utilities::GUID>> renderGroupsWithID;
				std::optional<uint32_t> Find(Utilities::GUID id, RenderGroup group)
				{
					for (uint32_t i = 0; i < renderGroupsWithID[uint8_t(group)].size(); i++)
						if (renderGroupsWithID[uint8_t(group)][i] == id)
							return i;
					return std::nullopt;
				}

				inline void Add(const ToAdd<JOB>& job)
				{
					if (auto index = Find(job.id, job.group); !index.has_value())
					{
						GetIDs(job.group).push_back(job.id);
						GetJobs(job.group).push_back(job.job);
					}
				}
				inline void Remove(const ToRemove& job)
				{
					
					if (auto index = Find(job.id, job.group); index.has_value())
					{
						uint32_t last = uint32_t(GetIDs(job.group).size()) - 1;
						GetIDs(job.group)[*index] = GetIDs(job.group)[last];
						GetJobs(job.group)[*index] = GetJobs(job.group)[last];

						GetIDs(job.group).pop_back();
						GetJobs(job.group).pop_back();
					}

				}
	
				inline std::vector<JOB>& GetJobs(RenderGroup group)
				{
					return renderGroupsWithJob[uint8_t(group)];
				}
				inline std::vector<Utilities::GUID>& GetIDs(RenderGroup group)
				{
					return renderGroupsWithID[uint8_t(group)];
				}
			};
			RenderSide<JOB> renderSide;


			struct ClientSide
			{
				std::vector<std::vector<Utilities::GUID>> renderGroupsWithID;
				std::optional<uint32_t> Find(Utilities::GUID id, RenderGroup group)
				{
					for (uint32_t i = 0; i < renderGroupsWithID[uint8_t(group)].size(); i++)
						if (renderGroupsWithID[uint8_t(group)][i] == id)
							return i;
					return std::nullopt;
				}
				inline std::vector<Utilities::GUID>& GetIDs(RenderGroup group)
				{
					return renderGroupsWithID[uint8_t(group)];
				}

				uint32_t Registered(Utilities::GUID id)const
				{
					StartProfile;

					uint32_t count = 0;
					for (uint8_t i = 0; i <= uint8_t(RenderGroup::FINAL_PASS); i++)
						for (auto& j : renderGroupsWithID[i])
							if (j == id)
							{
								count++;
								continue;
							}

					return count;
				}
			}clientSide;

			JobStuff()
			{
				clientSide.renderGroupsWithID.resize(uint8_t(RenderGroup::FINAL_PASS) + 1);
				renderSide.renderGroupsWithID.resize(uint8_t(RenderGroup::FINAL_PASS) + 1);
				renderSide.renderGroupsWithJob.resize(uint8_t(RenderGroup::FINAL_PASS) + 1);
			}
		};
		JobStuff<RenderJob> renderJobs;
		JobStuff<UpdateJob> updateJobs;

	
	};
}
#endif