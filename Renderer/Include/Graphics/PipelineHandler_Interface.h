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

		virtual GRAPHICS_ERROR CreateBuffer(Utilz::GUID id, const Pipeline::Buffer& buffer) = 0;
		/*virtual GRAPHICS_ERROR UpdateBuffer(Utilz::GUID id, void* data, size_t size) = 0;
		virtual GRAPHICS_ERROR UpdateBuffer(Utilz::GUID id, const std::function<void(void* mappedResource)>& mapCallback) = 0;*/
		virtual GRAPHICS_ERROR DestroyBuffer(Utilz::GUID id) = 0;

		virtual GRAPHICS_ERROR CreateViewport(Utilz::GUID id, const Pipeline::Viewport& viewport) = 0;
			
		virtual GRAPHICS_ERROR CreateShader(Utilz::GUID id, Pipeline::ShaderType type, const char* sourceCode, size_t size, const char* entryPoint, const char* shaderModel) = 0;
		virtual GRAPHICS_ERROR CreateShader(Utilz::GUID id, Pipeline::ShaderType type, void* data, size_t size) = 0;
		virtual GRAPHICS_ERROR DestroyShader(Utilz::GUID id, Pipeline::ShaderType type) = 0;
				
		virtual GRAPHICS_ERROR CreateTexture(Utilz::GUID id, void* data, size_t width, size_t height) = 0;
		virtual GRAPHICS_ERROR DestroyTexture(Utilz::GUID id) = 0;
				
		virtual GRAPHICS_ERROR CreateRasterizerState(Utilz::GUID id, const Pipeline::RasterizerState& state) = 0;
		virtual GRAPHICS_ERROR DestroyRasterizerState(Utilz::GUID id) = 0;
				
		virtual GRAPHICS_ERROR CreateBlendState(Utilz::GUID id, const Pipeline::BlendState& state) = 0;
		virtual GRAPHICS_ERROR DestroyBlendState(Utilz::GUID id) = 0;
		
		virtual GRAPHICS_ERROR CreateDepthStencilState(Utilz::GUID id, const Pipeline::DepthStencilState& state) = 0;
		virtual GRAPHICS_ERROR DestroyDepthStencilState(Utilz::GUID id) = 0;
			
		virtual GRAPHICS_ERROR CreateSamplerState(Utilz::GUID id, const Pipeline::SamplerState& state) = 0;
		virtual GRAPHICS_ERROR DestroySamplerState(Utilz::GUID id) = 0;
			
		virtual GRAPHICS_ERROR CreateRenderTarget(Utilz::GUID id, const Pipeline::RenderTarget& target) = 0;
		virtual GRAPHICS_ERROR DestroyRenderTarget(Utilz::GUID id) = 0;
				
		virtual GRAPHICS_ERROR CreateDepthStencilView(Utilz::GUID id, const Pipeline::DepthStencilView& view) = 0;
		virtual GRAPHICS_ERROR DestroyDepthStencilView(Utilz::GUID id) = 0;
			
		virtual GRAPHICS_ERROR CreateUnorderedAccessView(Utilz::GUID id, const Pipeline::UnorderedAccessView& view) = 0;
		virtual GRAPHICS_ERROR DestroyUnorderedAccessView(Utilz::GUID id) = 0;
	protected:
		virtual ~PipelineHandler_Interface() {};
	};
}
#endif