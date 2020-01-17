#ifndef _GRAPHICS_RENDERER_DX11_H_
#define _GRAPHICS_RENDERER_DX11_H_
#include <Graphics\Renderer_Interface.h>
#include <Graphics\PipelineHandler_Interface.h>
#include "DeviceHandler.h"
#include <thread>
#include "PipelineAssigner.h"
#include <optional>
#include <Utilities/CircularFIFO.h>
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

		virtual void UsePipelineHandler( const std::function<void( PipelineHandler_Interface & pipeline_handler )>& callback )noexcept override;

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
		RendererInitializationInfo settings;
		std::unique_ptr<DeviceHandler> device_handler;
		std::unique_ptr<PipelineAssigner> pipeline;

		//***** Threading stuff and safety ******//
		std::thread myThread;
		bool running;

		void Run();
		void ResolveJobs();
		void BeginFrame();
		void Frame();
		void EndFrame();

		//***** Renderjob stuff ******//

		template<class JOB>
		struct JobStuff{
			//**** TO ADD/REMOVE STUFF ****//
			template<class JOB>
			struct ToAdd{
				Utilities::GUID id;
				RenderGroup group;
				JOB job;
			};
			Utilities::CircularFiFo<ToAdd<JOB>> jobsToAdd;
			struct ToRemove{
				Utilities::GUID id;
				RenderGroup group;
			};
			Utilities::CircularFiFo<ToRemove> jobsToRemove;
			//****					   ****//

			void Add()
			{
				StartProfile;
				while ( !jobsToAdd.wasEmpty() )
				{
					StartProfileC( "Add Job" );
					renderSide.Add( jobsToAdd.top() );
					jobsToAdd.pop();
				}
			}
			void Remove()
			{
				StartProfile;
				while ( !jobsToRemove.wasEmpty() )
				{
					StartProfileC( "Remove Render Job" );
					renderSide.Remove( jobsToRemove.top() );
					jobsToRemove.pop();
				}
			}

			template<class JOB>
			struct RenderSide{
				std::vector<std::vector<JOB>> renderGroupsWithJob;
				std::vector<std::vector<Utilities::GUID>> renderGroupsWithID;
				std::optional<size_t> Find( Utilities::GUID id, RenderGroup group )
				{
					for ( size_t i = 0; i < renderGroupsWithID[uint8_t( group )].size(); i++ )
						if ( renderGroupsWithID[uint8_t( group )][i] == id )
							return i;
					return std::nullopt;
				}

				inline void Add( const ToAdd<JOB>& job )
				{
					if ( auto index = Find( job.id, job.group ); !index.has_value() )
					{
						GetIDs( job.group ).push_back( job.id );
						GetJobs( job.group ).push_back( job.job );
					}
				}
				inline void Remove( const ToRemove& job )
				{

					if ( auto index = Find( job.id, job.group ); index.has_value() )
					{
						uint32_t last = uint32_t( GetIDs( job.group ).size() ) - 1;
						GetIDs( job.group )[*index] = GetIDs( job.group )[last];
						GetJobs( job.group )[*index] = GetJobs( job.group )[last];

						GetIDs( job.group ).pop_back();
						GetJobs( job.group ).pop_back();
					}

				}

				inline std::vector<JOB>& GetJobs( RenderGroup group )
				{
					return renderGroupsWithJob[uint8_t( group )];
				}
				inline std::vector<Utilities::GUID>& GetIDs( RenderGroup group )
				{
					return renderGroupsWithID[uint8_t( group )];
				}
			};
			RenderSide<JOB> renderSide;


			struct ClientSide{
				std::vector<std::vector<Utilities::GUID>> renderGroupsWithID;
				std::optional<size_t> Find( Utilities::GUID id, RenderGroup group )
				{
					for ( size_t i = 0; i < renderGroupsWithID[uint8_t( group )].size(); i++ )
						if ( renderGroupsWithID[uint8_t( group )][i] == id )
							return i;
					return std::nullopt;
				}
				inline std::vector<Utilities::GUID>& GetIDs( RenderGroup group )
				{
					return renderGroupsWithID[uint8_t( group )];
				}

				size_t IsRegistered( Utilities::GUID id )const
				{
					size_t count = 0;
					for ( size_t i = 0; i <= uint8_t( RenderGroup::FINAL_PASS ); i++ )
						for ( auto& j : renderGroupsWithID[i] )
							if ( j == id )
								count++;
					return count;
				}
			}clientSide;

			JobStuff()
			{
				clientSide.renderGroupsWithID.resize( uint8_t( RenderGroup::FINAL_PASS ) + 1 );
				renderSide.renderGroupsWithID.resize( uint8_t( RenderGroup::FINAL_PASS ) + 1 );
				renderSide.renderGroupsWithJob.resize( uint8_t( RenderGroup::FINAL_PASS ) + 1 );
			}
		};
		JobStuff<RenderJob> renderJobs;
		JobStuff<UpdateJob> updateJobs;


	};
}
#endif