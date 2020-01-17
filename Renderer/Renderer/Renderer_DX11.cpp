#include "Renderer_DX11.h"
#include <Utilities/Profiler/Profiler.h>
namespace Graphics
{

	Renderer_DX11::Renderer_DX11(const RendererInitializationInfo & ii)
		: settings(ii), running(false)
	{
		device_handler = std::make_unique<DeviceHandler>( ( HWND )settings.windowHandle, settings.windowState == WindowState::FULLSCREEN, settings.windowState == WindowState::FULLSCREEN_BORDERLESS, settings.bufferCount );

		pipeline = std::make_unique<PipelineAssigner>( device_handler->GetDevice(), device_handler->GetDeviceContext(),
													   device_handler->GetRTV(), device_handler->GetSRV(),
													   device_handler->GetDepthStencil(), device_handler->GetDepthSRV(),
													   device_handler->GetViewport() );

	}


	Renderer_DX11::~Renderer_DX11()noexcept
	{
		if ( running )
		{
			running = false;
			myThread.join();
		}
		if ( pipeline )
		{
			pipeline->Shutdown();
		}
		if ( device_handler )
		{
			device_handler->Shutdown();
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
		if (running)
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
		myThread = std::thread(&Renderer_DX11::Run, this);
	}
	void Renderer_DX11::UpdateSettings(const RendererInitializationInfo & ii)
	{
		PROFILE;
		settings = ii;
		device_handler->ResizeSwapChain((HWND)settings.windowHandle, settings.windowState == WindowState::FULLSCREEN, settings.windowState == WindowState::FULLSCREEN_BORDERLESS, settings.bufferCount);
	}
	const RendererInitializationInfo & Renderer_DX11::GetSettings() const noexcept
	{
		return settings;
	}

	void Renderer_DX11::UsePipelineHandler( const std::function<void( PipelineHandler_Interface & pipeline_handler )>& callback ) noexcept
	{
		callback( *pipeline );
	}

	void Renderer_DX11::AddRenderJob(Utilities::GUID id, const RenderJob & job, RenderGroup renderGroup)
	{
		PROFILE;

		if ( renderJobs.clientSide.Find( id, renderGroup ).has_value() )
			throw RenderJob_Exisits( id );

		renderJobs.clientSide.GetIDs(renderGroup).push_back(id);

		renderJobs.jobsToAdd.push({ id, renderGroup, job });
	}
	void Renderer_DX11::AddRenderJob(const RenderJob & job, RenderGroup renderGroup)
	{
		return AddRenderJob(job.pipeline.ID(), job, renderGroup);
	}
	void Renderer_DX11::RemoveRenderJob(Utilities::GUID id, RenderGroup renderGroup)noexcept
	{
		PROFILE;

		if (auto index = renderJobs.clientSide.Find(id, renderGroup); index.has_value())
		{
			uint32_t last = uint32_t(renderJobs.clientSide.GetIDs(renderGroup).size()) - 1;
			renderJobs.clientSide.GetIDs(renderGroup)[*index] = renderJobs.clientSide.GetIDs(renderGroup)[last];
			renderJobs.clientSide.GetIDs(renderGroup).pop_back();

			renderJobs.jobsToRemove.push({ id, renderGroup });
		}
	}
	void Renderer_DX11::RemoveRenderJob(Utilities::GUID id)noexcept
	{
		PROFILE;

		for (uint8_t i = 0; i <= uint8_t(RenderGroup::FINAL_PASS); i++)
		{
			if (auto index = renderJobs.clientSide.Find(id, RenderGroup(i)); index.has_value())
			{
				uint32_t last = uint32_t(renderJobs.clientSide.renderGroupsWithID[i].size()) - 1;
				renderJobs.clientSide.renderGroupsWithID[i][*index] = renderJobs.clientSide.renderGroupsWithID[i][last];
				renderJobs.clientSide.renderGroupsWithID[i].pop_back();

				renderJobs.jobsToRemove.push({ id, RenderGroup(i) });
			}
		}
		
	}
	size_t Renderer_DX11::GetNumberOfRenderJobs() const noexcept
	{
		PROFILE;

		size_t count = 0;
		for (auto&g : renderJobs.clientSide.renderGroupsWithID)
			count += g.size();

		return count;
	}
	size_t Renderer_DX11::IsRenderJobRegistered(Utilities::GUID id) const noexcept
	{
		return renderJobs.clientSide.IsRegistered( id );
	}
	void Renderer_DX11::AddUpdateJob(Utilities::GUID id, const UpdateJob & job, RenderGroup renderGroupToPerformUpdateBefore)
	{
		PROFILE;
		if ( auto find = updateJobs.clientSide.Find( id, renderGroupToPerformUpdateBefore ); find.has_value() )
			throw UpdateJob_Exisits( id );

		if(job.frequency != UpdateFrequency::ONCE)
			updateJobs.clientSide.GetIDs(renderGroupToPerformUpdateBefore).push_back(id);

		updateJobs.jobsToAdd.push({ id, renderGroupToPerformUpdateBefore, job });

	}
	void Renderer_DX11::AddUpdateJob(const UpdateJob & job, RenderGroup renderGroupToPerformUpdateBefore)
	{
		return AddUpdateJob(job.objectToMap, job, renderGroupToPerformUpdateBefore);
	}
	void Renderer_DX11::RemoveUpdateJob(Utilities::GUID id, RenderGroup renderGroup)noexcept
	{
		PROFILE;

		if (auto index = renderJobs.clientSide.Find(id, renderGroup); index.has_value())
		{
			uint32_t last = uint32_t(updateJobs.clientSide.GetIDs(renderGroup).size()) - 1;
			updateJobs.clientSide.GetIDs(renderGroup)[*index] = updateJobs.clientSide.GetIDs(renderGroup)[last];
			updateJobs.clientSide.GetIDs(renderGroup).pop_back();

			updateJobs.jobsToRemove.push({ id, renderGroup });
		}
	}
	void Renderer_DX11::RemoveUpdateJob(Utilities::GUID id)noexcept
	{
		PROFILE;

		for (uint8_t i = 0; i <= uint8_t(RenderGroup::FINAL_PASS); i++)
		{
			if (auto index = updateJobs.clientSide.Find(id, RenderGroup(i)); index.has_value())
			{
				uint32_t last = uint32_t(updateJobs.clientSide.renderGroupsWithID[i].size()) - 1;
				updateJobs.clientSide.renderGroupsWithID[i][*index] = updateJobs.clientSide.renderGroupsWithID[i][last];
				updateJobs.clientSide.renderGroupsWithID[i].pop_back();

				updateJobs.jobsToRemove.push({ id, RenderGroup(i) });
			}
		}
	}
	size_t Renderer_DX11::GetNumberOfUpdateJobs() const noexcept
	{
		PROFILE;

		size_t count = 0;
		for (auto&g : updateJobs.clientSide.renderGroupsWithID)
			count += g.size();

		return count;
	}
	size_t Renderer_DX11::IsUpdateJobRegistered(Utilities::GUID id) const noexcept
	{
		return updateJobs.clientSide.IsRegistered(id);
	}
	void Renderer_DX11::Run()noexcept
	{
		PROFILE;
		while (running)
		{
			PROFILE_N("Run_While");
			ResolveJobs();

			clearColor[at][0] =fmodf(clearColor[at][0] + 0.01f, 1.0f);

			BeginFrame();

			Frame();

			EndFrame();
		}
	}
	void Renderer_DX11::ResolveJobs()noexcept
	{
		PROFILE;
		{
			pipeline->UpdatePipelineObjects();

			{
				PROFILE_N("Remove Jobs");
				updateJobs.Remove();
				renderJobs.Remove();
			}
			{
				PROFILE_N("Add Jobs");
				updateJobs.Add();
				renderJobs.Add();
			}
		}
	}
	void Renderer_DX11::BeginFrame()noexcept
	{
		PROFILE;
		ID3D11RenderTargetView* views[] = { device_handler->GetRTV() };
		device_handler->GetDeviceContext()->OMSetRenderTargets(1, views, device_handler->GetDepthStencil());

		// Clear the primary render target view using the specified color
		device_handler->GetDeviceContext()->ClearRenderTargetView(device_handler->GetRTV(), clearColor[at]);

		// Clear the standard depth stencil view
		device_handler->GetDeviceContext()->ClearDepthStencilView(device_handler->GetDepthStencil(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	}
	void Renderer_DX11::Frame()noexcept
	{
		PROFILE;
		static std::vector<JobStuff<UpdateJob>::ToRemove> updateJobsToRemove;
		for (uint8_t i = 0; i <= uint8_t(RenderGroup::FINAL_PASS); i++)
		{
			auto& uj = updateJobs.renderSide.GetJobs(RenderGroup(i));
			for (auto& job : uj)
			{
				job.updateCallback(pipeline->GetUpdateObject(job.objectToMap, job.type));
				if (job.frequency == UpdateFrequency::ONCE)
					updateJobsToRemove.push_back({ job.objectToMap,RenderGroup(i) });
			}

		
		}

		for (auto toRemove : updateJobsToRemove)
			updateJobs.renderSide.Remove(toRemove);
	}
	void Renderer_DX11::EndFrame()noexcept
	{
		PROFILE;
		device_handler->Present(settings.vsync);
	}

}