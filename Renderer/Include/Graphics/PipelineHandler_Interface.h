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

		virtual Graphics_Error CreateVertexBuffer(const Utilz::GUID& id, void* data, size_t vertexCount, size_t stride, bool dynamic = false) = 0;
		virtual int UpdateDynamicVertexBuffer(const Utilz::GUID& id, void* data, size_t size) = 0;
		virtual int CreateIndexBuffer(const Utilz::GUID& id, void* data, size_t indexCount, size_t indexSize) = 0;
		virtual int CreateBuffer(const Utilz::GUID& id, void* data, size_t elementCount, size_t elementStride, size_t maxElements, uint32_t flags) = 0;
		virtual int DestroyIndexBuffer(const Utilz::GUID& id) = 0;
		virtual int DestroyVertexBuffer(const Utilz::GUID& id) = 0;

		virtual int CreateViewport(const Utilz::GUID& id, const Viewport& viewport) = 0;

		virtual int CreateVertexShaderFromSource(const Utilz::GUID& id, const std::string& sourceCode, const std::string& entrypoint, const std::string& shaderModel) = 0;
		virtual int CreateVertexShader(const Utilz::GUID& id, void* data, size_t size) = 0;
		virtual int CreateGeometryShader(const Utilz::GUID& id, void* data, size_t size) = 0;
		virtual int CreateGeometryShaderStreamOut(const Utilz::GUID& id, void* data, size_t size) = 0;
		virtual int CreatePixelShader(const Utilz::GUID& id, void* data, size_t size) = 0;
		virtual int CreateComputeShader(const Utilz::GUID& id, void* data, size_t size) = 0;
		virtual int DestroyVertexShader(const Utilz::GUID& id) = 0;
		virtual int DestroyGeometryShader(const Utilz::GUID& id) = 0;
		virtual int DestroyPixelShader(const Utilz::GUID& id) = 0;
		virtual int DestroyComputeShader(const Utilz::GUID& id) = 0;

		virtual int CreateConstantBuffer(const Utilz::GUID& id, size_t size, void* initialData = nullptr) = 0;
		virtual int UpdateConstantBuffer(const Utilz::GUID& id, void* data, size_t size) = 0;
		virtual int MapConstantBuffer(const Utilz::GUID& id, const std::function<void(void* mappedResource)>& mapCallback) = 0;
		virtual int DestroyConstantBuffer(const Utilz::GUID& id) = 0;

		virtual int CreateTexture(const Utilz::GUID& id, void* data, size_t width, size_t height) = 0;
		virtual int DestroyTexture(const Utilz::GUID& id) = 0;

		virtual int CreateRasterizerState(const Utilz::GUID& id, const RasterizerState& state) = 0;
		virtual int DestroyRasterizerState(const Utilz::GUID& id) = 0;

		virtual int CreateBlendState(const Utilz::GUID& id, const BlendState& state) = 0;
		virtual int DestroyBlendState(const Utilz::GUID& id) = 0;

		virtual int CreateDepthStencilState(const Utilz::GUID& id, const DepthStencilState& state) = 0;
		virtual int DestroyDepthStencilState(const Utilz::GUID& id) = 0;

		virtual int CreateSamplerState(const Utilz::GUID& id, const SamplerState& state) = 0;
		virtual int DestroySamplerState(const Utilz::GUID& id) = 0;

		virtual int CreateRenderTarget(const Utilz::GUID& id, const RenderTarget& target) = 0;
		virtual int DestroyRenderTarget(const Utilz::GUID& id) = 0;

		virtual int CreateDepthStencilView(const Utilz::GUID& id, size_t width, size_t height, bool bindAsTexture = false) = 0;
		virtual int CreateDepthStencilViewCube(const Utilz::GUID& id, size_t width, size_t height, bool bindAsTexture = true) = 0;
		virtual int DestroyDepthStencilView(const Utilz::GUID& id) = 0;

		virtual int CreateUnorderedAccessView(const Utilz::GUID& id, const UnorderedAccessView& view) = 0;
		virtual int DestroyUnorderedAccessView(const Utilz::GUID& id) = 0;
	protected:
		virtual ~PipelineHandler_Interface() {};
	};
}
#endif