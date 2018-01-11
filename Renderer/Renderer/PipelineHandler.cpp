#include "PipelineHandler.h"
#include <Profiler.h>
#define INSERT_TYPE_R(type, id) objects_RenderSide[type][id] = type##_
#define INSERT_TYPE_C(type, id) objects_ClientSide[type].emplace(id)
namespace Graphics
{
	PipelineHandler::PipelineHandler()
	{
		objects_ClientSide;
	}


	PipelineHandler::~PipelineHandler()
	{
	}

	GRAPHICS_ERROR PipelineHandler::Init(
		ID3D11Device * device, ID3D11DeviceContext * context, 
		ID3D11RenderTargetView * backbuffer, ID3D11DepthStencilView * dsv,
		const D3D11_VIEWPORT& vp)
	{
		StartProfile;

	
		this->device = device;
		this->context = context;
		INSERT_TYPE_R(PipelineObjects::RenderTarget, Default_RenderTarget) { backbuffer,{ 0.0f, 0.0f,1.0f,0.0f } };
		INSERT_TYPE_R(PipelineObjects::DepthStencilView, Default_DepthStencil) { dsv };
		INSERT_TYPE_C(PipelineObjects::RenderTarget, Default_RenderTarget);
		INSERT_TYPE_C(PipelineObjects::DepthStencilView, Default_DepthStencil);


		INSERT_TYPE_R(PipelineObjects::VertexBuffer, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::IndexBuffer, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::ConstantBuffer, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::InputLayout, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::VertexShader, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::GeomtryShader, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::PixelShader, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::ComputeShader, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::RenderTarget, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::UnorderedAccessView, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::ShaderResourceView, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::DepthStencilView, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::SamplerState, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::BlendState, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::RasterizerState, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::DepthStencilState, Default_RenderTarget) { nullptr };
		INSERT_TYPE_R(PipelineObjects::Viewport, Default_RenderTarget) { vp };
		//pipelineObjects.renderTargetViews[Default_RenderTarget] = { backbuffer,{ 0.0f, 0.0f,1.0f,0.0f } };
		//pipelineObjects.depthStencilViews[Default_DepthStencil] = dsv;
		//
		////Create nullptrs for IDs that are not assigned;
		//pipelineObjects.vertexBuffers[Utilz::GUID()].buffer = nullptr;
		//pipelineObjects.vertexBuffers[Utilz::GUID()].stride = 0;
		//pipelineObjects.indexBuffers[Utilz::GUID()].buffer = nullptr;
		//pipelineObjects.indexBuffers[Utilz::GUID()].stride = 0;
		//pipelineObjects.inputLayouts[Utilz::GUID()] = nullptr;
		//pipelineObjects.vertexShaders[Utilz::GUID()] = { nullptr };
		//pipelineObjects.geometryShaders[Utilz::GUID()] = { nullptr };
		//pipelineObjects.pixelShaders[Utilz::GUID()] = { nullptr };
		//pipelineObjects.computeShaders[Utilz::GUID()] = { nullptr };
		//pipelineObjects.constantBuffers[Utilz::GUID()] = nullptr;
		//pipelineObjects.shaderResourceViews[Utilz::GUID()] = nullptr;
		//pipelineObjects.renderTargetViews[Utilz::GUID()] = { nullptr };
		//pipelineObjects.samplerStates[Utilz::GUID()] = nullptr;
		//pipelineObjects.blendStates[Utilz::GUID()] = nullptr;
		//pipelineObjects.rasterizerStates[Utilz::GUID()] = nullptr;
		//pipelineObjects.depthStencilStates[Utilz::GUID()] = nullptr;
		//pipelineObjects.unorderedAccessViews[Utilz::GUID()] = { nullptr };
		RETURN_GRAPHICS_SUCCESS;
	}

	void PipelineHandler::Shutdown()
	{
	}

	GRAPHICS_ERROR PipelineHandler::CreateBuffer(Utilz::GUID id, const Pipeline::Buffer & buffer)
	{
	/*	if (buffer.flags & BufferFlags::BIND_VERTEX)
		{
			if(auto find = vertexbuffer)
		}*/

		RETURN_GRAPHICS_SUCCESS;
	}

	GRAPHICS_ERROR PipelineHandler::UpdateBuffer(Utilz::GUID id, void * data, size_t size)
	{
		RETURN_GRAPHICS_SUCCESS;
	}

	GRAPHICS_ERROR PipelineHandler::UpdateBuffer(Utilz::GUID id, const std::function<void(void*mappedResource)>& mapCallback)
	{
		RETURN_GRAPHICS_SUCCESS;
	}

	GRAPHICS_ERROR PipelineHandler::DestroyBuffer(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	
	GRAPHICS_ERROR PipelineHandler::CreateViewport(Utilz::GUID id, const Pipeline::Viewport & viewport)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateShader(Utilz::GUID id, Pipeline::ShaderType type, const char * sourceCode, const char * entryPoint, const char * shaderModel)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateShader(Utilz::GUID id, Pipeline::ShaderType type, void * data, size_t size)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyShader(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateTexture(Utilz::GUID id, void * data, size_t width, size_t height)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyTexture(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateRasterizerState(Utilz::GUID id, const Pipeline::RasterizerState & state)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyRasterizerState(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateBlendState(Utilz::GUID id, const Pipeline::BlendState & state)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyBlendState(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateDepthStencilState(Utilz::GUID id, const Pipeline::DepthStencilState & state)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyDepthStencilState(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateSamplerState(Utilz::GUID id, const Pipeline::SamplerState & state)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroySamplerState(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateRenderTarget(Utilz::GUID id, const Pipeline::RenderTarget & target)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyRenderTarget(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateDepthStencilView(Utilz::GUID id, const Pipeline::DepthStencilView & view)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyDepthStencilView(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateUnorderedAccessView(Utilz::GUID id, const Pipeline::UnorderedAccessView & view)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyUnorderedAccessView(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
}