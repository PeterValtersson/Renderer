#include "Renderer_DX11.h"
#include <Profiler.h>
namespace Graphics
{

	Renderer_DX11::Renderer_DX11(const RendererInitializationInfo & ii)
		: settings(ii), running(false), initiated(false), pipeline(nullptr), device(nullptr)
	{
		renderJobInfoClientSide.renderGroupsWithID.resize(uint8_t(RenderGroup::FINAL_PASS) + 1);
		renderJobInfoRenderSide.renderGroupsWithID.resize(uint8_t(RenderGroup::FINAL_PASS) + 1);
		renderJobInfoRenderSide.renderGroupsWithJob.resize(uint8_t(RenderGroup::FINAL_PASS) + 1);
	}


	Renderer_DX11::~Renderer_DX11()
	{
	}
	GRAPHICS_ERROR Renderer_DX11::Initialize()
	{
		StartProfile;

		if (initiated)
			RETURN_GRAPHICS_ERROR_C("Renderer has already been initiated");

		device = new DeviceHandler();
		if (!device)
			RETURN_GRAPHICS_ERROR_C("Could not create device handler");
		PASS_IF_GRAPHICS_ERROR(device->Init((HWND)settings.windowHandle, settings.windowState == WindowState::FULLSCREEN, settings.windowState == WindowState::FULLSCREEN_BORDERLESS, settings.bufferCount));
		
		pipeline = new PipelineAssigner();
		if (!device)
			RETURN_GRAPHICS_ERROR_C("Could not create pipeline");

		PASS_IF_GRAPHICS_ERROR(pipeline->Init(device->GetDevice(), device->GetDeviceContext(), device->GetRTV(), device->GetDepthStencil()));

		initiated = true;
		RETURN_GRAPHICS_SUCCESS;
	}
	void Renderer_DX11::Shutdown()
	{
		StartProfile;
		if (running)
		{
			running = false;
			myThread.join();
		}
		if (pipeline)
		{
			pipeline->Shutdown();
			pipeline = nullptr;
		}
		if (device)
		{
			device->Shutdown();
			device = nullptr;
		}

		
		initiated = false;
	}	

	float clearColor[5][4] = {
		{1, 0, 0, 1 },
		{ 1, 1, 0, 1 },
	{ 1, 0, 1, 1 },
	{ 0, 1, 0, 1 },
	{ 0, 0, 1, 1 }
	};
	int at = 0;
	void Renderer_DX11::Pause()
	{
	
		StartProfile;
		if (running)
		{
			running = false;
			myThread.join();
		}
		at = ++at % 5;
	}
	GRAPHICS_ERROR Renderer_DX11::Start()
	{
		StartProfile;
		if (running)
			RETURN_GRAPHICS_ERROR_C("Renderer already running");
		if (!initiated)
			RETURN_GRAPHICS_ERROR_C("Renderer must be initiated first");
		running = true;
		myThread = std::thread(&Renderer_DX11::Run, this);
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR Renderer_DX11::UpdateSettings(const RendererInitializationInfo & ii)
	{
		StartProfile;
		//if (settings.windowHandle != ii.windowHandle)
		//	RETURN_GRAPHICS_ERROR_C("Handle not the same");
		settings = ii;
		PASS_IF_GRAPHICS_ERROR(device->ResizeSwapChain((HWND)settings.windowHandle, settings.windowState == WindowState::FULLSCREEN, settings.windowState == WindowState::FULLSCREEN_BORDERLESS, settings.bufferCount));

		RETURN_GRAPHICS_SUCCESS;
	}
	const RendererInitializationInfo & Renderer_DX11::GetSettings() const
	{
		return settings;
	}
	PipelineHandler_Interface * Renderer_DX11::GetPipelineHandler()
	{
		return pipeline;
	}
	GRAPHICS_ERROR Renderer_DX11::AddRenderJob(Utilz::GUID id, const RenderJob & job, RenderGroup renderGroup)
	{
		StartProfile;

		if (renderJobInfoClientSide.FindRenderJob(id, renderGroup).has_value())
			RETURN_GRAPHICS_ERROR_C("RenderJob is already registered to group");

		renderJobInfoClientSide.GetIDs(renderGroup).push_back(id);

		jobsToAdd.push({ id, renderGroup, job });


		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR Renderer_DX11::AddRenderJob(const RenderJob & job, RenderGroup renderGroup)
	{
		return AddRenderJob(job.pipeline.ID(), job, renderGroup);
	}
	void Renderer_DX11::RemoveRenderJob(Utilz::GUID id, RenderGroup renderGroup)
	{
		StartProfile;

		if (auto index = renderJobInfoClientSide.FindRenderJob(id, renderGroup); index.has_value())
		{
			uint32_t last = uint32_t(renderJobInfoClientSide.GetIDs(renderGroup).size()) - 1;
			renderJobInfoClientSide.GetIDs(renderGroup)[*index] = renderJobInfoClientSide.GetIDs(renderGroup)[last];
			renderJobInfoClientSide.GetIDs(renderGroup).pop_back();

			jobsToRemove.push({ id, renderGroup });
		}
	}
	void Renderer_DX11::RemoveRenderJob(Utilz::GUID id)
	{
		StartProfile;

		for (uint8_t i = 0; i <= uint8_t(RenderGroup::FINAL_PASS); i++)
		{
			if (auto index = renderJobInfoClientSide.FindRenderJob(id, RenderGroup(i)); index.has_value())
			{
				uint32_t last = uint32_t(renderJobInfoClientSide.renderGroupsWithID[i].size()) - 1;
				renderJobInfoClientSide.renderGroupsWithID[i][*index] = renderJobInfoClientSide.renderGroupsWithID[i][last];
				renderJobInfoClientSide.renderGroupsWithID[i].pop_back();

				jobsToRemove.push({ id, RenderGroup(i) });
			}
		}
		
	}
	uint32_t Renderer_DX11::GetNumberOfRenderJobs() const
	{
		StartProfile;

		uint32_t count = 0;
		for (auto&g : renderJobInfoClientSide.renderGroupsWithID)
			count += uint32_t(g.size());

		return count;
	}
	uint8_t Renderer_DX11::IsRenderJobRegistered(Utilz::GUID id) const
	{
		StartProfile;

		uint32_t count = 0;
		for (uint8_t i = 0; i <= uint8_t(RenderGroup::FINAL_PASS); i++)
			for (auto& j : renderJobInfoClientSide.renderGroupsWithID[i])
				if (j == id)
				{
					count++;
					continue;
				}

		return count;
	}
	void Renderer_DX11::Run()
	{
		StartProfile;
		while (running)
		{
			StartProfileC("Run_While");
			ResolveJobs();

			clearColor[at][0] =fmodf(clearColor[at][0] + 0.01f, 1.0f);

			BeginFrame();

			Frame();

			EndFrame();
		}
	}
	void Renderer_DX11::ResolveJobs()
	{
		StartProfile;
		{
			StartProfileC("Remove Jobs");
			// Remove jobs
			while (!jobsToRemove.wasEmpty())
			{
				StartProfileC("Remove Job");
				auto& job = jobsToRemove.top();
				if (auto index = renderJobInfoRenderSide.FindRenderJob(job.id, job.group); index.has_value())
				{
					uint32_t last = uint32_t(renderJobInfoRenderSide.GetIDs(job.group).size()) - 1;
					renderJobInfoRenderSide.GetIDs(job.group)[*index] = renderJobInfoRenderSide.GetIDs(job.group)[last];
					renderJobInfoRenderSide.GetJobs(job.group)[*index] = renderJobInfoRenderSide.GetJobs(job.group)[last];

					renderJobInfoRenderSide.GetIDs(job.group).pop_back();
					renderJobInfoRenderSide.GetJobs(job.group).pop_back();

				}
				jobsToRemove.pop();
			}
		}
		{
			StartProfileC("Add Jobs");
			while (!jobsToAdd.wasEmpty())
			{
				StartProfileC("Add Job");
				auto& job = jobsToAdd.top();
				if (auto index = renderJobInfoRenderSide.FindRenderJob(job.id, job.group); !index.has_value())
				{
					renderJobInfoRenderSide.GetIDs(job.group).push_back(job.id);
					renderJobInfoRenderSide.GetJobs(job.group).push_back(job.job);
				}
				jobsToAdd.pop();
			}
		}
	}
	void Renderer_DX11::BeginFrame()
	{
		StartProfile;

		// Clear the primary render target view using the specified color
		device->GetDeviceContext()->ClearRenderTargetView(device->GetRTV(), clearColor[at]);

		// Clear the standard depth stencil view
		device->GetDeviceContext()->ClearDepthStencilView(device->GetDepthStencil(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	}
	void Renderer_DX11::Frame()
	{
		StartProfile;
	}
	void Renderer_DX11::EndFrame()
	{
		StartProfile;
		device->Present(settings.vsync);
	}
	std::optional<uint32_t> Renderer_DX11::RenderJobInfoRenderSide::FindRenderJob(Utilz::GUID id, RenderGroup group)
	{
		for (uint32_t i = 0; i < renderGroupsWithID[uint8_t(group)].size(); i++)
			if (renderGroupsWithID[uint8_t(group)][i] == id)
				return i;
		return std::nullopt;
	}
	std::optional<uint32_t> Renderer_DX11::RenderJobInfoClientSide::FindRenderJob(Utilz::GUID id, RenderGroup group)
	{
		for (uint32_t i = 0; i < renderGroupsWithID[uint8_t(group)].size(); i++)
			if (renderGroupsWithID[uint8_t(group)][i] == id)
				return i;
		return std::nullopt;
	}
}