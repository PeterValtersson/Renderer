#ifndef _GRAPHICS_PIPELINE_HANDLER_INTERFACE_H_
#define _GRAPHICS_PIPELINE_HANDLER_INTERFACE_H_
#include "Pipeline.h"
#include "../Graphics_Error.h"
#include <functional>
namespace Graphics
{
	class PipelineHandler_Interface
	{
	public:
		PipelineHandler_Interface() {};

		virtual GRAPHICS_ERROR CreateVertexBuffer(Utilz::GUID id, void* data, size_t vertexCount, size_t stride, bool dynamic = false) = 0;
		virtual GRAPHICS_ERROR UpdateDynamicVertexBuffer(Utilz::GUID id, void* data, size_t size) = 0;
		virtual GRAPHICS_ERROR CreateIndexBuffer(Utilz::GUID id, void* data, size_t indexCount, size_t indexSize) = 0;
		virtual GRAPHICS_ERROR CreateBuffer(Utilz::GUID id, void* data, size_t elementCount, size_t elementStride, size_t maxElements, uint32_t flags) = 0;
		virtual GRAPHICS_ERROR DestroyIndexBuffer(Utilz::GUID id) = 0;
		virtual GRAPHICS_ERROR DestroyVertexBuffer(Utilz::GUID id) = 0;
			
		virtual GRAPHICS_ERROR CreateViewport(Utilz::GUID id, const Viewport& viewport) = 0;
			
		virtual GRAPHICS_ERROR CreateVertexShaderFromSource(Utilz::GUID id, const std::string& sourceCode, const std::string& entrypoint, const std::string& shaderModel) = 0;
		virtual GRAPHICS_ERROR CreateVertexShader(Utilz::GUID id, void* data, size_t size) = 0;
		virtual GRAPHICS_ERROR CreateGeometryShader(Utilz::GUID id, void* data, size_t size) = 0;
		virtual GRAPHICS_ERROR CreateGeometryShaderStreamOut(Utilz::GUID id, void* data, size_t size) = 0;
		virtual GRAPHICS_ERROR CreatePixelShader(Utilz::GUID id, void* data, size_t size) = 0;
		virtual GRAPHICS_ERROR CreateComputeShader(Utilz::GUID id, void* data, size_t size) = 0;
		virtual GRAPHICS_ERROR DestroyVertexShader(Utilz::GUID id) = 0;
		virtual GRAPHICS_ERROR DestroyGeometryShader(Utilz::GUID id) = 0;
		virtual GRAPHICS_ERROR DestroyPixelShader(Utilz::GUID id) = 0;
		virtual GRAPHICS_ERROR DestroyComputeShader(Utilz::GUID id) = 0;
				
		virtual GRAPHICS_ERROR CreateConstantBuffer(Utilz::GUID id, size_t size, void* initialData = nullptr) = 0;
		virtual GRAPHICS_ERROR UpdateConstantBuffer(Utilz::GUID id, void* data, size_t size) = 0;
		virtual GRAPHICS_ERROR MapConstantBuffer(Utilz::GUID id, const std::function<void(void* mappedResource)>& mapCallback) = 0;
		virtual GRAPHICS_ERROR DestroyConstantBuffer(Utilz::GUID id) = 0;
			
		virtual GRAPHICS_ERROR CreateTexture(Utilz::GUID id, void* data, size_t width, size_t height) = 0;
		virtual GRAPHICS_ERROR DestroyTexture(Utilz::GUID id) = 0;
				
		virtual GRAPHICS_ERROR CreateRasterizerState(Utilz::GUID id, const RasterizerState& state) = 0;
		virtual GRAPHICS_ERROR DestroyRasterizerState(Utilz::GUID id) = 0;
				
		virtual GRAPHICS_ERROR CreateBlendState(Utilz::GUID id, const BlendState& state) = 0;
		virtual GRAPHICS_ERROR DestroyBlendState(Utilz::GUID id) = 0;
		
		virtual GRAPHICS_ERROR CreateDepthStencilState(Utilz::GUID id, const DepthStencilState& state) = 0;
		virtual GRAPHICS_ERROR DestroyDepthStencilState(Utilz::GUID id) = 0;
			
		virtual GRAPHICS_ERROR CreateSamplerState(Utilz::GUID id, const SamplerState& state) = 0;
		virtual GRAPHICS_ERROR DestroySamplerState(Utilz::GUID id) = 0;
			
		virtual GRAPHICS_ERROR CreateRenderTarget(Utilz::GUID id, const RenderTarget& target) = 0;
		virtual GRAPHICS_ERROR DestroyRenderTarget(Utilz::GUID id) = 0;
				
		virtual GRAPHICS_ERROR CreateDepthStencilView(Utilz::GUID id, size_t width, size_t height, bool bindAsTexture = false) = 0;
		virtual GRAPHICS_ERROR CreateDepthStencilViewCube(Utilz::GUID id, size_t width, size_t height, bool bindAsTexture = true) = 0;
		virtual GRAPHICS_ERROR DestroyDepthStencilView(Utilz::GUID id) = 0;
			
		virtual GRAPHICS_ERROR CreateUnorderedAccessView(Utilz::GUID id, const UnorderedAccessView& view) = 0;
		virtual GRAPHICS_ERROR DestroyUnorderedAccessView(Utilz::GUID id) = 0;
	protected:
		virtual ~PipelineHandler_Interface() {};
	};
}
#endif