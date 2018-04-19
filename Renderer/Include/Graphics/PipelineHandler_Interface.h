#ifndef _GRAPHICS_PIPELINE_HANDLER_INTERFACE_H_
#define _GRAPHICS_PIPELINE_HANDLER_INTERFACE_H_
#include "Pipeline.h"
#include <Error.h>
#include <functional>
namespace Graphics
{
	
	class PipelineHandler_Interface
	{
	public:

		PipelineHandler_Interface() {};

		virtual UERROR CreateBuffer(Utilities::GUID id, const Pipeline::Buffer& buffer) = 0;
		/*virtual UERROR UpdateBuffer(Utilities::GUID id, void* data, size_t size) = 0;
		virtual UERROR UpdateBuffer(Utilities::GUID id, const std::function<void(void* mappedResource)>& mapCallback) = 0;*/
		virtual UERROR DestroyBuffer(Utilities::GUID id) = 0;

		virtual UERROR CreateViewport(Utilities::GUID id, const Pipeline::Viewport& viewport) = 0;
			
		virtual UERROR CreateShader(Utilities::GUID id, Pipeline::ShaderType type, const char* sourceCode, size_t size, const char* entryPoint, const char* shaderModel) = 0;
		virtual UERROR CreateShader(Utilities::GUID id, Pipeline::ShaderType type, void* data, size_t size) = 0;
		virtual UERROR DestroyShader(Utilities::GUID id, Pipeline::ShaderType type) = 0;
				
		virtual UERROR CreateTexture(Utilities::GUID id, void* data, size_t width, size_t height) = 0;
		virtual UERROR DestroyTexture(Utilities::GUID id) = 0;
				
		virtual UERROR CreateRasterizerState(Utilities::GUID id, const Pipeline::RasterizerState& state) = 0;
		virtual UERROR DestroyRasterizerState(Utilities::GUID id) = 0;
				
		virtual UERROR CreateBlendState(Utilities::GUID id, const Pipeline::BlendState& state) = 0;
		virtual UERROR DestroyBlendState(Utilities::GUID id) = 0;
		
		virtual UERROR CreateDepthStencilState(Utilities::GUID id, const Pipeline::DepthStencilState& state) = 0;
		virtual UERROR DestroyDepthStencilState(Utilities::GUID id) = 0;
			
		virtual UERROR CreateSamplerState(Utilities::GUID id, const Pipeline::SamplerState& state) = 0;
		virtual UERROR DestroySamplerState(Utilities::GUID id) = 0;
			
		virtual UERROR CreateTarget(Utilities::GUID id, const Pipeline::Target& target) = 0;
		virtual UERROR DestroyTarget(Utilities::GUID id) = 0;
				
		virtual UERROR CreateDepthStencilView(Utilities::GUID id, const Pipeline::DepthStencilView& view) = 0;
		virtual UERROR DestroyDepthStencilView(Utilities::GUID id) = 0;
	
	protected:
		virtual ~PipelineHandler_Interface() {};
	};
}
#endif