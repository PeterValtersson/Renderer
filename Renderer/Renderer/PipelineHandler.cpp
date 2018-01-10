#include "PipelineHandler.h"

namespace Graphics
{
	PipelineHandler::PipelineHandler()
	{
	}


	PipelineHandler::~PipelineHandler()
	{
	}

	GRAPHICS_ERROR PipelineHandler::Init(ID3D11Device * device, ID3D11DeviceContext * context, ID3D11RenderTargetView * backbuffer, ID3D11DepthStencilView * dsv)
	{
		this->device = device;
		this->context = context;

		pipelineObjects.renderTargetViews[Default_RenderTarget] = { backbuffer,{ 0.0f, 0.0f,1.0f,0.0f } };
		pipelineObjects.depthStencilViews[Default_DepthStencil] = dsv;
		
		//Create nullptrs for IDs that are not assigned;
		pipelineObjects.vertexBuffers[Utilz::GUID()].buffer = nullptr;
		pipelineObjects.vertexBuffers[Utilz::GUID()].stride = 0;
		pipelineObjects.indexBuffers[Utilz::GUID()].buffer = nullptr;
		pipelineObjects.indexBuffers[Utilz::GUID()].stride = 0;
		pipelineObjects.inputLayouts[Utilz::GUID()] = nullptr;
		pipelineObjects.vertexShaders[Utilz::GUID()] = { nullptr };
		pipelineObjects.geometryShaders[Utilz::GUID()] = { nullptr };
		pipelineObjects.pixelShaders[Utilz::GUID()] = { nullptr };
		pipelineObjects.computeShaders[Utilz::GUID()] = { nullptr };
		pipelineObjects.constantBuffers[Utilz::GUID()] = nullptr;
		pipelineObjects.shaderResourceViews[Utilz::GUID()] = nullptr;
		pipelineObjects.renderTargetViews[Utilz::GUID()] = { nullptr };
		pipelineObjects.samplerStates[Utilz::GUID()] = nullptr;
		pipelineObjects.blendStates[Utilz::GUID()] = nullptr;
		pipelineObjects.rasterizerStates[Utilz::GUID()] = nullptr;
		pipelineObjects.depthStencilStates[Utilz::GUID()] = nullptr;
		pipelineObjects.unorderedAccessViews[Utilz::GUID()] = { nullptr };
		RETURN_GRAPHICS_SUCCESS;
	}

	void PipelineHandler::Shutdown()
	{
	}

	GRAPHICS_ERROR PipelineHandler::CreateBuffer(Utilz::GUID id, const Buffer & buffer)
	{
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
	
	GRAPHICS_ERROR PipelineHandler::CreateViewport(Utilz::GUID id, const Viewport & viewport)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateShader(Utilz::GUID id, ShaderType type, const char * sourceCode, const char * entryPoint, const char * shaderModel)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateShader(Utilz::GUID id, ShaderType type, void * data, size_t size)
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
	GRAPHICS_ERROR PipelineHandler::CreateRasterizerState(Utilz::GUID id, const RasterizerState & state)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyRasterizerState(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateBlendState(Utilz::GUID id, const BlendState & state)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyBlendState(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateDepthStencilState(Utilz::GUID id, const DepthStencilState & state)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyDepthStencilState(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateSamplerState(Utilz::GUID id, const SamplerState & state)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroySamplerState(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateRenderTarget(Utilz::GUID id, const RenderTarget & target)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyRenderTarget(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateDepthStencilView(Utilz::GUID id, const DepthStencilView & view)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyDepthStencilView(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateUnorderedAccessView(Utilz::GUID id, const UnorderedAccessView & view)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyUnorderedAccessView(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
}