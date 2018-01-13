#ifndef _GRAPHICS_PIPELINE_HANDLER_H_
#define _GRAPHICS_PIPELINE_HANDLER_H_
#include <Graphics\PipelineHandler_Interface.h>
#include <d3d11.h>
#include <unordered_map>
#include <array>
#include "CircularFIFO.h"
#include <set>
#include <Graphics\UpdateJob.h>
#include "PipelineObjects.h"

namespace Graphics
{
	class PipelineHandler : public PipelineHandler_Interface
	{
	public:
		PipelineHandler();
		virtual ~PipelineHandler();

		virtual GRAPHICS_ERROR Init(
			ID3D11Device* device, ID3D11DeviceContext* context, 
			ID3D11RenderTargetView* backbuffer, ID3D11DepthStencilView* dsv,
			const D3D11_VIEWPORT& vp);
		virtual void Shutdown();

		virtual GRAPHICS_ERROR CreateBuffer(Utilz::GUID id, const Pipeline::Buffer& buffer) override;
	//	virtual GRAPHICS_ERROR UpdateBuffer(Utilz::GUID id, void* data, size_t size);
	//	virtual GRAPHICS_ERROR UpdateBuffer(Utilz::GUID id, const std::function<void(void* mappedResource, size_t maxSize)>& mapCallback);
		virtual GRAPHICS_ERROR DestroyBuffer(Utilz::GUID id) override;

		virtual GRAPHICS_ERROR CreateViewport(Utilz::GUID id, const Pipeline::Viewport& viewport) override;

		virtual GRAPHICS_ERROR CreateShader(Utilz::GUID id, Pipeline::ShaderType type, const char* sourceCode, size_t size, const char* entryPoint, const char* shaderModel) override;
		virtual GRAPHICS_ERROR CreateShader(Utilz::GUID id, Pipeline::ShaderType type, void* data, size_t size) override;
		virtual GRAPHICS_ERROR DestroyShader(Utilz::GUID id, Pipeline::ShaderType type) override;


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

		GRAPHICS_ERROR UpdatePipelineObjects();

		virtual UpdateObject* GetUpdateObject(Utilz::GUID id, PipelineObjectType type);
	protected:
		ID3D11Device * device;
		ID3D11DeviceContext* context;
		
		std::array<std::unordered_map<Utilz::GUID, PipelineObject, Utilz::GUID::Hasher>, PipelineObjects::NUM_TYPES> objects_RenderSide;
		std::array<std::set<Utilz::GUID, Utilz::GUID::Compare>, PipelineObjects::NUM_TYPES> objects_ClientSide;
		PipelineObject o;
		struct ToAdd
		{
			Utilz::GUID id;
			PipelineObject obj;
			ToAdd& operator=(ToAdd&& other)
			{
				id = other.id;
				obj = std::move(other.obj);
				return *this;
			}
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