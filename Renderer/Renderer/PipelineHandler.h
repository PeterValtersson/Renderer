#ifndef _GRAPHICS_PIPELINE_HANDLER_H_
#define _GRAPHICS_PIPELINE_HANDLER_H_
#include <Graphics\PipelineHandler_Interface.h>
#include <d3d11.h>
#include <unordered_map>
namespace Graphics
{
	class PipelineHandler : public PipelineHandler_Interface
	{
	public:
		PipelineHandler();
		virtual ~PipelineHandler();

		virtual GRAPHICS_ERROR Init(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11RenderTargetView* backbuffer, ID3D11DepthStencilView* dsv);
		virtual void Shutdown();

		virtual GRAPHICS_ERROR CreateBuffer(Utilz::GUID id, const Buffer& buffer) override;
		virtual GRAPHICS_ERROR UpdateBuffer(Utilz::GUID id, void* data, size_t size) override;
		virtual GRAPHICS_ERROR UpdateBuffer(Utilz::GUID id, const std::function<void(void* mappedResource)>& mapCallback) override;
		virtual GRAPHICS_ERROR DestroyBuffer(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateViewport(Utilz::GUID id, const Viewport& viewport) override;

		virtual GRAPHICS_ERROR CreateShader(Utilz::GUID id, ShaderType type, const char* sourceCode, const char* entryPoint, const char* shaderModel) override;
		virtual GRAPHICS_ERROR CreateShader(Utilz::GUID id, ShaderType type, void* data, size_t size) override;
		virtual GRAPHICS_ERROR DestroyShader(Utilz::GUID id) override;

	
		virtual GRAPHICS_ERROR CreateTexture(Utilz::GUID id, void* data, size_t width, size_t height) override;
		virtual GRAPHICS_ERROR DestroyTexture(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateRasterizerState(Utilz::GUID id, const RasterizerState& state) override;
		virtual GRAPHICS_ERROR DestroyRasterizerState(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateBlendState(Utilz::GUID id, const BlendState& state) override;
		virtual GRAPHICS_ERROR DestroyBlendState(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateDepthStencilState(Utilz::GUID id, const DepthStencilState& state) override;
		virtual GRAPHICS_ERROR DestroyDepthStencilState(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateSamplerState(Utilz::GUID id, const SamplerState& state) override;
		virtual GRAPHICS_ERROR DestroySamplerState(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateRenderTarget(Utilz::GUID id, const RenderTarget& target) override;
		virtual GRAPHICS_ERROR DestroyRenderTarget(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateDepthStencilView(Utilz::GUID id, const DepthStencilView& view) override;
		virtual GRAPHICS_ERROR DestroyDepthStencilView(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateUnorderedAccessView(Utilz::GUID id, const UnorderedAccessView& view) override;
		virtual GRAPHICS_ERROR DestroyUnorderedAccessView(Utilz::GUID id) override;

	protected:
		ID3D11Device * device;
		ID3D11DeviceContext* context;

		struct PipelineObjects
		{
			struct VertexBuffer
			{
				ID3D11Buffer* buffer;
				uint32_t stride;
			};
			struct IndexBuffer
			{
				ID3D11Buffer* buffer;
				uint32_t stride;
			};
			Pipeline currentPipeline;


			struct VertexShaderInfo
			{
				ID3D11VertexShader* shader;
				std::vector<Utilz::GUID> constantBuffers;
			};
			struct GeomtryShaderInfo
			{
				ID3D11GeometryShader* shader;
				std::vector<Utilz::GUID> constantBuffers;
			};
			struct PixelShaderInfo
			{
				ID3D11PixelShader* shader;
				std::vector<Utilz::GUID> constantBuffers;
			};
			struct RenderTargetInfo
			{
				ID3D11RenderTargetView* rtv;
				float clearColor[4];
			};
			struct UnorderedAccessViewInfo
			{
				ID3D11UnorderedAccessView* uav;
				float clearColor[4];
			};
			struct ComputeShaderInfo
			{
				ID3D11ComputeShader* shader;
				std::vector<Utilz::GUID> constantBuffers;
			};

		
			std::unordered_map<Utilz::GUID, VertexBuffer, Utilz::GUID::Hasher> vertexBuffers;
			std::unordered_map<Utilz::GUID, IndexBuffer, Utilz::GUID::Hasher> indexBuffers;
			std::unordered_map<Utilz::GUID, ID3D11InputLayout*, Utilz::GUID::Hasher> inputLayouts;
			std::unordered_map<Utilz::GUID, VertexShaderInfo, Utilz::GUID::Hasher> vertexShaders;
			std::unordered_map<Utilz::GUID, GeomtryShaderInfo, Utilz::GUID::Hasher> geometryShaders;
			std::unordered_map<Utilz::GUID, PixelShaderInfo, Utilz::GUID::Hasher> pixelShaders;
			std::unordered_map<Utilz::GUID, ComputeShaderInfo, Utilz::GUID::Hasher> computeShaders;
			std::unordered_map<Utilz::GUID, ID3D11Buffer*, Utilz::GUID::Hasher> constantBuffers;
			std::unordered_map<Utilz::GUID, ID3D11ShaderResourceView*, Utilz::GUID::Hasher> shaderResourceViews;
			std::unordered_map<Utilz::GUID, RenderTargetInfo, Utilz::GUID::Hasher> renderTargetViews;
			std::unordered_map<Utilz::GUID, ID3D11DepthStencilView*, Utilz::GUID::Hasher> depthStencilViews;
			std::unordered_map<Utilz::GUID, ID3D11SamplerState*, Utilz::GUID::Hasher> samplerStates;
			std::unordered_map<Utilz::GUID, ID3D11BlendState*, Utilz::GUID::Hasher> blendStates;
			std::unordered_map<Utilz::GUID, ID3D11RasterizerState*, Utilz::GUID::Hasher> rasterizerStates;
			std::unordered_map<Utilz::GUID, D3D11_VIEWPORT, Utilz::GUID::Hasher> viewports;
			std::unordered_map<Utilz::GUID, ID3D11DepthStencilState*, Utilz::GUID::Hasher> depthStencilStates;
			std::unordered_map<Utilz::GUID, UnorderedAccessViewInfo, Utilz::GUID::Hasher> unorderedAccessViews;

			/**<Key is evaluated by (GUID(shader) + GUID(resourceBindingName))*/
			std::unordered_map<Utilz::GUID, int, Utilz::GUID::Hasher> shaderAndResourceNameToBindSlot;
		};

		PipelineObjects pipelineObjects;
		PipelineObjects pipelineObjects_ToAdd;
		PipelineObjects pipelineObjects_ToRemove;
	};
}
#endif