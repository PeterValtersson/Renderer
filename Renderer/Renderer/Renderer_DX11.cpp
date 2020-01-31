#include "Renderer_DX11.h"
#include <Utilities/Profiler/Profiler.h>
#undef min
#include <algorithm>
namespace Renderer
{

	Renderer_DX11::Renderer_DX11( const RendererInitializationInfo& ii ) : 
		device_handler( ii ), 
		pipeline( device_handler.GetDevice(), device_handler.GetDeviceContext() ),
		settings( ii ), running( false )
	{
		if ( !settings.windowHandle )
			return;


		pipeline.AddTexture( Default_RenderTarget, device_handler.GetRTV() );
		pipeline.AddTexture( Default_RenderTarget, device_handler.GetSRV() );
		pipeline.AddTexture( Default_DepthStencil, device_handler.GetDepthSRV() );
		pipeline.AddDepthStencilView( Default_DepthStencil, device_handler.GetDepthStencil() );

		Pipeline::Viewport vp;
		vp.width = static_cast< float >( settings.resolution.width );
		vp.height = static_cast< float >( settings.resolution.height );
		vp.maxDepth = 1.0f;

		pipeline.CreateViewport( Default_Viewport, vp );
	}


	Renderer_DX11::~Renderer_DX11()noexcept
	{
		if ( running )
		{
			running = false;
			myThread.join();
		}
	}

	float clearColor[5][4] = {
		{1, 0, 0, 1 },
		{ 1, 1, 0, 1 },
	{ 1, 0, 1, 1 },
	{ 0, 1, 0, 1 },
	{ 0, 0, 1, 1 }
	};
	int at = 0;
	void Renderer_DX11::Pause()noexcept
	{

		PROFILE;
		if ( running )
		{
			running = false;
			myThread.join();
		}
		at = ++at % 5;
	}
	void Renderer_DX11::Start()noexcept
	{
		PROFILE;
		if ( running )
			return;

		running = true;
		myThread = std::thread( &Renderer_DX11::Run, this );
	}
	void Renderer_DX11::UpdateSettings( const RendererInitializationInfo& ii )
	{
		PROFILE;
		settings = ii;

		if ( !settings.windowHandle )
			return;

		device_handler.ResizeSwapChain( ii );

		pipeline.DestroyTexture( Default_RenderTarget );
		pipeline.DestroyDepthStencilState( Default_DepthStencil );

		pipeline.AddTexture( Default_RenderTarget, device_handler.GetRTV() );
		pipeline.AddTexture( Default_RenderTarget, device_handler.GetSRV() );
		pipeline.AddTexture( Default_DepthStencil, device_handler.GetDepthSRV() );
		pipeline.AddDepthStencilView( Default_DepthStencil, device_handler.GetDepthStencil() );

		Pipeline::Viewport vp;
		vp.width = static_cast< float >( settings.resolution.width );
		vp.height = static_cast< float >( settings.resolution.height );
		vp.maxDepth = 1.0f;

		pipeline.CreateViewport( Default_Viewport, vp );
	}
	const RendererInitializationInfo& Renderer_DX11::GetSettings() const noexcept
	{
		return settings;
	}

	void Renderer_DX11::UsePipelineHandler( const std::function<void( PipelineHandler_Interface & pipeline_handler )>& callback )
	{
		callback( pipeline );
	}

	void Renderer_DX11::AddRenderJob( Utilities::GUID id, const RenderJob& job, RenderGroup renderGroup )
	{
		PROFILE;

		if ( renderJobs.clientSide.Find( id, renderGroup ).has_value() )
			throw RenderJob_Exisits( id );

		renderJobs.clientSide.GetIDs( renderGroup ).push_back( id );

		renderJobs.jobsToAdd.push( { id, renderGroup, job } );
	}
	void Renderer_DX11::AddRenderJob( const RenderJob& job, RenderGroup renderGroup )
	{
		return AddRenderJob( job.pipeline.ID(), job, renderGroup );
	}
	void Renderer_DX11::RemoveRenderJob( Utilities::GUID id, RenderGroup renderGroup )noexcept
	{
		PROFILE;

		if ( auto index = renderJobs.clientSide.Find( id, renderGroup ); index.has_value() )
		{
			uint32_t last = uint32_t( renderJobs.clientSide.GetIDs( renderGroup ).size() ) - 1;
			renderJobs.clientSide.GetIDs( renderGroup )[*index] = renderJobs.clientSide.GetIDs( renderGroup )[last];
			renderJobs.clientSide.GetIDs( renderGroup ).pop_back();

			renderJobs.jobsToRemove.push( { id, renderGroup } );
		}
	}
	void Renderer_DX11::RemoveRenderJob( Utilities::GUID id )noexcept
	{
		PROFILE;

		for ( uint8_t i = 0; i <= uint8_t( RenderGroup::FINAL_PASS ); i++ )
		{
			if ( auto index = renderJobs.clientSide.Find( id, RenderGroup( i ) ); index.has_value() )
			{
				uint32_t last = uint32_t( renderJobs.clientSide.renderGroupsWithID[i].size() ) - 1;
				renderJobs.clientSide.renderGroupsWithID[i][*index] = renderJobs.clientSide.renderGroupsWithID[i][last];
				renderJobs.clientSide.renderGroupsWithID[i].pop_back();

				renderJobs.jobsToRemove.push( { id, RenderGroup( i ) } );
			}
		}

	}
	size_t Renderer_DX11::GetNumberOfRenderJobs() const noexcept
	{
		PROFILE;

		size_t count = 0;
		for ( auto& g : renderJobs.clientSide.renderGroupsWithID )
			count += g.size();

		return count;
	}
	size_t Renderer_DX11::IsRenderJobRegistered( Utilities::GUID id ) const noexcept
	{
		return renderJobs.clientSide.IsRegistered( id );
	}
	void Renderer_DX11::AddUpdateJob( Utilities::GUID id, const UpdateJob& job, RenderGroup renderGroupToPerformUpdateBefore )
	{
		PROFILE;
		if ( auto find = updateJobs.clientSide.Find( id, renderGroupToPerformUpdateBefore ); find.has_value() )
			throw UpdateJob_Exisits( id );

		if ( job.frequency != UpdateFrequency::ONCE )
			updateJobs.clientSide.GetIDs( renderGroupToPerformUpdateBefore ).push_back( id );

		updateJobs.jobsToAdd.push( { id, renderGroupToPerformUpdateBefore, job } );

	}
	void Renderer_DX11::AddUpdateJob( const UpdateJob& job, RenderGroup renderGroupToPerformUpdateBefore )
	{
		return AddUpdateJob( job.objectToMap, job, renderGroupToPerformUpdateBefore );
	}
	void Renderer_DX11::RemoveUpdateJob( Utilities::GUID id, RenderGroup renderGroup )noexcept
	{
		PROFILE;

		if ( auto index = renderJobs.clientSide.Find( id, renderGroup ); index.has_value() )
		{
			uint32_t last = uint32_t( updateJobs.clientSide.GetIDs( renderGroup ).size() ) - 1;
			updateJobs.clientSide.GetIDs( renderGroup )[*index] = updateJobs.clientSide.GetIDs( renderGroup )[last];
			updateJobs.clientSide.GetIDs( renderGroup ).pop_back();

			updateJobs.jobsToRemove.push( { id, renderGroup } );
		}
	}
	void Renderer_DX11::RemoveUpdateJob( Utilities::GUID id )noexcept
	{
		PROFILE;

		for ( uint8_t i = 0; i <= uint8_t( RenderGroup::FINAL_PASS ); i++ )
		{
			if ( auto index = updateJobs.clientSide.Find( id, RenderGroup( i ) ); index.has_value() )
			{
				uint32_t last = uint32_t( updateJobs.clientSide.renderGroupsWithID[i].size() ) - 1;
				updateJobs.clientSide.renderGroupsWithID[i][*index] = updateJobs.clientSide.renderGroupsWithID[i][last];
				updateJobs.clientSide.renderGroupsWithID[i].pop_back();

				updateJobs.jobsToRemove.push( { id, RenderGroup( i ) } );
			}
		}
	}
	size_t Renderer_DX11::GetNumberOfUpdateJobs() const noexcept
	{
		PROFILE;

		size_t count = 0;
		for ( auto& g : updateJobs.clientSide.renderGroupsWithID )
			count += g.size();

		return count;
	}
	size_t Renderer_DX11::IsUpdateJobRegistered( Utilities::GUID id ) const noexcept
	{
		return updateJobs.clientSide.IsRegistered( id );
	}
	void Renderer_DX11::PerformRenderJob( const RenderJob& job ) noexcept
	{
		pipeline.Set_Pipeline( job.pipeline );

		if ( job.indexCount == 0 && job.instanceCount == 0 && job.vertexCount != 0 )
		{
			for ( auto& mf : job.mappingFunctions )
				mf( 0, 0 );
			device_handler.GetDeviceContext()->Draw( job.vertexCount, job.vertexOffset );
		}
		else if ( job.indexCount != 0 && job.instanceCount == 0 )
		{
			for ( auto& mf : job.mappingFunctions )
				mf( 0, 0 );
			device_handler.GetDeviceContext()->DrawIndexed( job.indexCount, job.indexOffset, job.vertexOffset );
		}
		else if ( job.indexCount == 0 && job.instanceCount != 0 )
		{
			uint32_t drawn = 0;
			while ( drawn < job.instanceCount )
			{
				const uint32_t toDraw = std::min( job.maxInstances, job.instanceCount - drawn );
				for ( auto& mf : job.mappingFunctions )
					mf( toDraw, drawn );
				device_handler.GetDeviceContext()->DrawInstanced( job.vertexCount, toDraw, job.vertexOffset, job.instanceOffset );
				drawn += toDraw;
			}
		}
		else if ( job.indexCount != 0 && job.instanceCount != 0 )
		{
			uint32_t drawn = 0;
			while ( drawn < job.instanceCount )
			{
				const uint32_t toDraw = std::min( job.maxInstances, job.instanceCount - drawn );
				for ( auto& mf : job.mappingFunctions )
					mf( toDraw, drawn );
				device_handler.GetDeviceContext()->DrawIndexedInstanced( job.indexCount, toDraw, job.indexOffset, job.vertexOffset, job.instanceOffset );
				drawn += toDraw;
			}
		}
		else if ( job.ThreadGroupCountX != 0 || job.ThreadGroupCountY != 0 || job.ThreadGroupCountZ != 0 )
		{
			device_handler.GetDeviceContext()->Dispatch( job.ThreadGroupCountX, job.ThreadGroupCountY, job.ThreadGroupCountZ );
		}
		else if ( job.vertexCount == 0 )
		{
			for ( auto& mf : job.mappingFunctions )
				mf( 0, 0 );
			device_handler.GetDeviceContext()->DrawAuto();
		}
	}

	void Renderer_DX11::Run()noexcept
	{
		PROFILE;
		while ( running )
		{
			PROFILE_N( "Run_While" );
			ResolveJobs();

		//	clearColor[at][0] = fmodf( clearColor[at][0] + 0.01f, 1.0f );

			BeginFrame();

			Frame();

			EndFrame();
		}
	}
	void Renderer_DX11::ResolveJobs()noexcept
	{
		PROFILE;
		{
			pipeline.UpdatePipelineObjects();

			{
				PROFILE_N( "Remove Jobs" );
				updateJobs.Remove();
				renderJobs.Remove();
			}
			{
				PROFILE_N( "Add Jobs" );
				updateJobs.Add();
				renderJobs.Add();
			}
		}
	}
	void Renderer_DX11::BeginFrame()noexcept
	{
		PROFILE;
		//ID3D11RenderTargetView* views[] = { device_handler.GetRTV().Get() };
		//device_handler.GetDeviceContext()->OMSetRenderTargets( 1, views, device_handler.GetDepthStencil().Get() );

		//// Clear the primary render target view using the specified color
		//device_handler.GetDeviceContext()->ClearRenderTargetView( device_handler.GetRTV().Get(), clearColor[at] );

		//// Clear the standard depth stencil view
		//device_handler.GetDeviceContext()->ClearDepthStencilView( device_handler.GetDepthStencil().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );

	}
	void Renderer_DX11::Frame()noexcept
	{
		PROFILE;
		std::vector<Job<UpdateJob>::ToRemove> updateJobsToRemove;
		for ( uint8_t i = 0; i <= uint8_t( RenderGroup::FINAL_PASS ); i++ )
		{
			auto& uj = updateJobs.renderSide.GetJobs( RenderGroup( i ) );
			for ( auto& job : uj )
			{
				try
				{
					pipeline.UpdateObject( job.objectToMap, job.type, job.updateCallback );
				}
				catch ( ... )
				{
					// Do something with that.
				}
				if ( job.frequency == UpdateFrequency::ONCE )
					updateJobsToRemove.push_back( { job.objectToMap,RenderGroup( i ) } );
			}

			auto& rj = renderJobs.renderSide.GetJobs( RenderGroup( i ) );
			for ( auto& job : rj )
			{
				PerformRenderJob( job );
			}


		}

		for ( auto& toRemove : updateJobsToRemove )
			updateJobs.renderSide.Remove( toRemove );
	}
	void Renderer_DX11::EndFrame()noexcept
	{
		PROFILE;
		device_handler.Present( settings.vsync );
		pipeline.Clear_Pipeline(); // Present unbinds all instances of backbuffer. So for now we just force setting of pipeline for the first job.
	}

}