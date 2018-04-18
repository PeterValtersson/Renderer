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

		virtual GRAPHICS_ERROR CreateBuffer(Utilities::GUID id, const Pipeline::Buffer& buffer) = 0;
		/*virtual GRAPHICS_ERROR UpdateBuffer(Utilities::GUID id, void* data, size_t size) = 0;
		virtual GRAPHICS_ERROR UpdateBuffer(Utilities::GUID id, const std::function<void(void* mappedResource)>& mapCallback) = 0;*/
		virtual GRAPHICS_ERROR DestroyBuffer(Utilities::GUID id) = 0;

		virtual GRAPHICS_ERROR CreateViewport(Utilities::GUID id, const Pipeline::Viewport& viewport) = 0;
			
		virtual GRAPHICS_ERROR CreateShader(Utilities::GUID id, Pipeline::ShaderType type, const char* sourceCode, size_t size, const char* entryPoint, const char* shaderModel) = 0;
		virtual GRAPHICS_ERROR CreateShader(Utilities::GUID id, Pipeline::ShaderType type, void* data, size_t size) = 0;
		virtual GRAPHICS_ERROR DestroyShader(Utilities::GUID id, Pipeline::ShaderType type) = 0;
				
		virtual GRAPHICS_ERROR CreateTexture(Utilities::GUID id, void* data, size_t width, size_t height) = 0;
		virtual GRAPHICS_ERROR DestroyTexture(Utilities::GUID id) = 0;
				
		virtual GRAPHICS_ERROR CreateRasterizerState(Utilities::GUID id, const Pipeline::RasterizerState& state) = 0;
		virtual GRAPHICS_ERROR DestroyRasterizerState(Utilities::GUID id) = 0;
				
		virtual GRAPHICS_ERROR CreateBlendState(Utilities::GUID id, const Pipeline::BlendState& state) = 0;
		virtual GRAPHICS_ERROR DestroyBlendState(Utilities::GUID id) = 0;
		
		virtual GRAPHICS_ERROR CreateDepthStencilState(Utilities::GUID id, const Pipeline::DepthStencilState& state) = 0;
		virtual GRAPHICS_ERROR DestroyDepthStencilState(Utilities::GUID id) = 0;
			
		virtual GRAPHICS_ERROR CreateSamplerState(Utilities::GUID id, const Pipeline::SamplerState& state) = 0;
		virtual GRAPHICS_ERROR DestroySamplerState(Utilities::GUID id) = 0;
			
		virtual GRAPHICS_ERROR CreateTarget(Utilities::GUID id, const Pipeline::Target& target) = 0;
		virtual GRAPHICS_ERROR DestroyTarget(Utilities::GUID id) = 0;
				
		virtual GRAPHICS_ERROR CreateDepthStencilView(Utilities::GUID id, const Pipeline::DepthStencilView& view) = 0;
		virtual GRAPHICS_ERROR DestroyDepthStencilView(Utilities::GUID id) = 0;
	
	protected:
		virtual ~PipelineHandler_Interface() {};
	};
}
#endif