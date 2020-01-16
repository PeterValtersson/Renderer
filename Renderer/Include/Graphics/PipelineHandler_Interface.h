#ifndef _GRAPHICS_PIPELINE_HANDLER_INTERFACE_H_
#define _GRAPHICS_PIPELINE_HANDLER_INTERFACE_H_
#include "Pipeline.h"
#include <Utilities/ErrorHandling.h>
#include <functional>
#include <Utilities/GUID.h>
namespace Graphics
{
	
	class PipelineHandler_Interface
	{
	public:

		PipelineHandler_Interface() {};

		virtual void CreateBuffer(Utilities::GUID id, const Pipeline::Buffer& buffer) = 0;
		/*virtual void UpdateBuffer(Utilities::GUID id, void* data, size_t size) = 0;
		virtual void UpdateBuffer(Utilities::GUID id, const std::function<void(void* mappedResource)>& mapCallback) = 0;*/
		virtual void DestroyBuffer(Utilities::GUID id)noexcept = 0;

		virtual void CreateViewport(Utilities::GUID id, const Pipeline::Viewport& viewport) = 0;
			
		virtual void CreateShader(Utilities::GUID id, Pipeline::ShaderType type, const char* sourceCode, size_t size, const char* entryPoint, const char* shaderModel) = 0;
		virtual void CreateShader(Utilities::GUID id, Pipeline::ShaderType type, void* data, size_t size) = 0;
		virtual void DestroyShader(Utilities::GUID id, Pipeline::ShaderType type)noexcept = 0;
				
		virtual void CreateTexture(Utilities::GUID id, void* data, size_t width, size_t height) = 0;
		virtual void DestroyTexture(Utilities::GUID id)noexcept = 0;
				
		virtual void CreateRasterizerState(Utilities::GUID id, const Pipeline::RasterizerState& state) = 0;
		virtual void DestroyRasterizerState(Utilities::GUID id)noexcept = 0;
				
		virtual void CreateBlendState(Utilities::GUID id, const Pipeline::BlendState& state) = 0;
		virtual void DestroyBlendState(Utilities::GUID id)noexcept = 0;
		
		virtual void CreateDepthStencilState(Utilities::GUID id, const Pipeline::DepthStencilState& state) = 0;
		virtual void DestroyDepthStencilState(Utilities::GUID id)noexcept = 0;
			
		virtual void CreateSamplerState(Utilities::GUID id, const Pipeline::SamplerState& state) = 0;
		virtual void DestroySamplerState(Utilities::GUID id)noexcept = 0;
			
		virtual void CreateTarget(Utilities::GUID id, const Pipeline::Target& target) = 0;
		virtual void DestroyTarget(Utilities::GUID id)noexcept = 0;
				
		virtual void CreateDepthStencilView(Utilities::GUID id, const Pipeline::DepthStencilView& view) = 0;
		virtual void DestroyDepthStencilView(Utilities::GUID id)noexcept = 0;
	
	protected:
		virtual ~PipelineHandler_Interface() {};
	};
}
#endif