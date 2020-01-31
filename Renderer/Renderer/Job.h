#ifndef _GRAPHICS_JOB_H_
#define _GRAPHICS_JOB_H_
#pragma once
#include <vector>
#include <Utilities/GUID.h>
#include <Graphics/RenderJob.h>

namespace Renderer
{
	template<class JobType>
	struct Job {
		//**** TO ADD/REMOVE STUFF ****//
		template<class JOB>
		struct ToAdd {
			Utilities::GUID id;
			RenderGroup group;
			JobType job;
		};
		Utilities::CircularFiFo<ToAdd<JobType>> jobsToAdd;

		struct ToRemove {
			Utilities::GUID id;
			RenderGroup group;
		};
		Utilities::CircularFiFo<ToRemove> jobsToRemove;
		//****					   ****//

		void Add()noexcept
		{
			while ( !jobsToAdd.isEmpty() )
			{
				//StartProfileC( "Add Job" );
				renderSide.Add( jobsToAdd.top() );
				jobsToAdd.pop();
			}
		}
	
		void Remove()noexcept
		{
			while ( !jobsToRemove.isEmpty() )
			{
				//StartProfileC( "Remove Render Job" );
				renderSide.Remove( jobsToRemove.top() );
				jobsToRemove.pop();
			}
		}

		template<class JobType>
		struct RenderSide {
			std::vector<std::vector<JobType>> renderGroupsWithJob;
			std::vector<std::vector<Utilities::GUID>> renderGroupsWithID;
			std::optional<size_t> Find( Utilities::GUID id, RenderGroup group )noexcept
			{
				for ( size_t i = 0; i < renderGroupsWithID[uint8_t( group )].size(); i++ )
					if ( renderGroupsWithID[uint8_t( group )][i] == id )
						return i;
				return std::nullopt;
			}

			void Add( const ToAdd<JobType>& job )noexcept
			{
				if ( auto index = Find( job.id, job.group ); !index.has_value() )
				{
					GetIDs( job.group ).push_back( job.id );
					GetJobs( job.group ).push_back( job.job );
				}
			}
			void Remove( const ToRemove& job )noexcept
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

			inline std::vector<JobType>& GetJobs( RenderGroup group )noexcept
			{
				return renderGroupsWithJob[uint8_t( group )];
			}
			inline std::vector<Utilities::GUID>& GetIDs( RenderGroup group )noexcept
			{
				return renderGroupsWithID[uint8_t( group )];
			}
		};
		RenderSide<JobType> renderSide;


		struct ClientSide {
			std::vector<std::vector<Utilities::GUID>> renderGroupsWithID;
			std::optional<size_t> Find( Utilities::GUID id, RenderGroup group )noexcept
			{
				for ( size_t i = 0; i < renderGroupsWithID[uint8_t( group )].size(); i++ )
					if ( renderGroupsWithID[uint8_t( group )][i] == id )
						return i;
				return std::nullopt;
			}
			inline std::vector<Utilities::GUID>& GetIDs( RenderGroup group )noexcept
			{
				return renderGroupsWithID[uint8_t( group )];
			}

			size_t IsRegistered( Utilities::GUID id )const noexcept
			{
				size_t count = 0;
				for ( size_t i = 0; i <= uint8_t( RenderGroup::FINAL_PASS ); i++ )
					for ( auto& j : renderGroupsWithID[i] )
						if ( j == id )
							count++;
				return count;
			}
		}clientSide;

		Job()
		{
			clientSide.renderGroupsWithID.resize( uint8_t( RenderGroup::FINAL_PASS ) + 1 );
			renderSide.renderGroupsWithID.resize( uint8_t( RenderGroup::FINAL_PASS ) + 1 );
			renderSide.renderGroupsWithJob.resize( uint8_t( RenderGroup::FINAL_PASS ) + 1 );
		}
	};

}
#endif