#ifndef _GRAPHICS_PIPELINE_HANDLER_H_
#define _GRAPHICS_PIPELINE_HANDLER_H_
#include <Graphics\PipelineHandler_Interface.h>
#include <d3d11.h>
#include <unordered_map>
#include <variant>
#include <array>
#include "CircularFIFO.h"
#include <set>
#define MAKE_RELEASEABLE_STRUCT(name_, objtype, ...)\
static constexpr uint32_t name_ = __COUNTER__ - offset - 1;\
struct name_##_\
{\
objtype * obj = nullptr;\
~name_##_()\
{\
if (obj)\
obj->Release();\
}\
 __VA_ARGS__  }
#define MAKE_SIMPLE_STRUCT(name_, objtype)\
static constexpr uint32_t name_ = __COUNTER__ - offset - 1;\
struct name_##_\
{\
objtype obj; }
namespace Graphics
{
	class PipelineHandler : public PipelineHandler_Interface
	{
	public:
		PipelineHandler();
		virtual ~PipelineHandler();

		virtual GRAPHICS_ERROR Init(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11RenderTargetView* backbuffer, ID3D11DepthStencilView* dsv);
		virtual void Shutdown();

		virtual GRAPHICS_ERROR CreateBuffer(Utilz::GUID id, const Pipeline::Buffer& buffer) override;
		virtual GRAPHICS_ERROR UpdateBuffer(Utilz::GUID id, void* data, size_t size) override;
		virtual GRAPHICS_ERROR UpdateBuffer(Utilz::GUID id, const std::function<void(void* mappedResource)>& mapCallback) override;
		virtual GRAPHICS_ERROR DestroyBuffer(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateViewport(Utilz::GUID id, const Pipeline::Viewport& viewport) override;

		virtual GRAPHICS_ERROR CreateShader(Utilz::GUID id, Pipeline::ShaderType type, const char* sourceCode, const char* entryPoint, const char* shaderModel) override;
		virtual GRAPHICS_ERROR CreateShader(Utilz::GUID id, Pipeline::ShaderType type, void* data, size_t size) override;
		virtual GRAPHICS_ERROR DestroyShader(Utilz::GUID id) override;


		virtual GRAPHICS_ERROR CreateTexture(Utilz::GUID id, void* data, size_t width, size_t height) override;
		virtual GRAPHICS_ERROR DestroyTexture(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateRasterizerState(Utilz::GUID id, const Pipeline::RasterizerState& state) override;
		virtual GRAPHICS_ERROR DestroyRasterizerState(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateBlendState(Utilz::GUID id, const Pipeline::BlendState& state) override;
		virtual GRAPHICS_ERROR DestroyBlendState(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateDepthStencilState(Utilz::GUID id, const Pipeline::DepthStencilState& state) override;
		virtual GRAPHICS_ERROR DestroyDepthStencilState(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateSamplerState(Utilz::GUID id, const Pipeline::SamplerState& state) override;
		virtual GRAPHICS_ERROR DestroySamplerState(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateRenderTarget(Utilz::GUID id, const Pipeline::RenderTarget& target) override;
		virtual GRAPHICS_ERROR DestroyRenderTarget(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateDepthStencilView(Utilz::GUID id, const Pipeline::DepthStencilView& view) override;
		virtual GRAPHICS_ERROR DestroyDepthStencilView(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateUnorderedAccessView(Utilz::GUID id, const Pipeline::UnorderedAccessView& view) override;
		virtual GRAPHICS_ERROR DestroyUnorderedAccessView(Utilz::GUID id) override;

	protected:
		ID3D11Device * device;
		ID3D11DeviceContext* context;
		struct PipelineObjects
		{
			static constexpr uint32_t offset = __COUNTER__;
			MAKE_RELEASEABLE_STRUCT(VertexBuffer, ID3D11Buffer, uint32_t stride;);
			MAKE_RELEASEABLE_STRUCT(IndexBuffer, ID3D11Buffer, uint32_t stride;);
			MAKE_RELEASEABLE_STRUCT(ConstantBuffer, ID3D11Buffer);
			MAKE_RELEASEABLE_STRUCT(InputLayout, ID3D11InputLayout);
			MAKE_RELEASEABLE_STRUCT(VertexShader, ID3D11VertexShader, std::vector<Utilz::GUID> constantBuffers;);
			MAKE_RELEASEABLE_STRUCT(GeomtryShader, ID3D11GeometryShader, std::vector<Utilz::GUID> constantBuffers;);
			MAKE_RELEASEABLE_STRUCT(PixelShader, ID3D11PixelShader, std::vector<Utilz::GUID> constantBuffers;);
			MAKE_RELEASEABLE_STRUCT(ComputeShader, ID3D11VertexShader, std::vector<Utilz::GUID> constantBuffers;);
			MAKE_RELEASEABLE_STRUCT(RenderTarget, ID3D11RenderTargetView, float clearColor[4];);
			MAKE_RELEASEABLE_STRUCT(UnorderedAccessView, ID3D11UnorderedAccessView, float clearColor[4];);
			MAKE_RELEASEABLE_STRUCT(ShaderResourceView, ID3D11ShaderResourceView);
			MAKE_RELEASEABLE_STRUCT(DepthStencilView, ID3D11DepthStencilView);
			MAKE_RELEASEABLE_STRUCT(SamplerState, ID3D11SamplerState);
			MAKE_RELEASEABLE_STRUCT(BlendState, ID3D11BlendState);
			MAKE_RELEASEABLE_STRUCT(RasterizerState, ID3D11RasterizerState);
			MAKE_RELEASEABLE_STRUCT(DepthStencilState, ID3D11DepthStencilState);
			MAKE_SIMPLE_STRUCT(Viewport, D3D11_VIEWPORT);
			static constexpr uint32_t NUM_TYPES = __COUNTER__ - offset - 1;
			using VariantObject = std::variant<
				VertexBuffer_,
				IndexBuffer_,
				ConstantBuffer_,
				InputLayout_,
				VertexShader_,
				GeomtryShader_,
				PixelShader_,
				ComputeShader_,
				RenderTarget_,
				UnorderedAccessView_,
				ShaderResourceView_,
				DepthStencilView_,
				SamplerState_,
				BlendState_,
				RasterizerState_,
				DepthStencilState_,
				Viewport_
			>;
		
		};
		std::array<std::unordered_map<Utilz::GUID, PipelineObjects::VariantObject, Utilz::GUID::Hasher>, PipelineObjects::NUM_TYPES> objects_RenderSide;
		std::array<std::set<Utilz::GUID, Utilz::GUID::Compare>, PipelineObjects::NUM_TYPES> objects_ClientSide;

		struct ToAdd
		{
			Utilz::GUID id;
			uint32_t type;
			PipelineObjects::VariantObject obj;
		};
		Utilz::CircularFiFo<ToAdd> toAdd;
		struct ToRemove
		{
			Utilz::GUID id;
			uint32_t type;
		};
		Utilz::CircularFiFo<ToRemove> toRemove;

	/**<Key is evaluated by (GUID(shader) + GUID(resourceBindingName))*/
		std::unordered_map<Utilz::GUID, int, Utilz::GUID::Hasher> shaderAndResourceNameToBindSlot;


	};
}
#endif