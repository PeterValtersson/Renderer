#ifndef _GRAPHICS_PIPELINE_HANDLER_H_
#define _GRAPHICS_PIPELINE_HANDLER_H_
#include <Graphics\PipelineHandler_Interface.h>
#include <d3d11.h>
#include <unordered_map>
#include <array>
#include <CircularFIFO.h>
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
			ID3D11RenderTargetView* backbuffer, ID3D11ShaderResourceView* bbsrv, 
			ID3D11DepthStencilView* dsv, ID3D11ShaderResourceView* dsvsrv,
			const D3D11_VIEWPORT& vp);
		virtual void Shutdown();

		virtual GRAPHICS_ERROR CreateBuffer(Utilities::GUID id, const Pipeline::Buffer& buffer) override;
	//	virtual GRAPHICS_ERROR UpdateBuffer(Utilities::GUID id, void* data, size_t size);
	//	virtual GRAPHICS_ERROR UpdateBuffer(Utilities::GUID id, const std::function<void(void* mappedResource, size_t maxSize)>& mapCallback);
		virtual GRAPHICS_ERROR DestroyBuffer(Utilities::GUID id) override;

		virtual GRAPHICS_ERROR CreateViewport(Utilities::GUID id, const Pipeline::Viewport& viewport) override;

		virtual GRAPHICS_ERROR CreateShader(Utilities::GUID id, Pipeline::ShaderType type, const char* sourceCode, size_t size, const char* entryPoint, const char* shaderModel) override;
		virtual GRAPHICS_ERROR CreateShader(Utilities::GUID id, Pipeline::ShaderType type, void* data, size_t size) override;
		virtual GRAPHICS_ERROR DestroyShader(Utilities::GUID id, Pipeline::ShaderType type) override;


		virtual GRAPHICS_ERROR CreateTexture(Utilities::GUID id, void* data, size_t width, size_t height) override;
		virtual GRAPHICS_ERROR DestroyTexture(Utilities::GUID id) override;

		virtual GRAPHICS_ERROR CreateRasterizerState(Utilities::GUID id, const Pipeline::RasterizerState& state) override;
		virtual GRAPHICS_ERROR DestroyRasterizerState(Utilities::GUID id) override;

		virtual GRAPHICS_ERROR CreateBlendState(Utilities::GUID id, const Pipeline::BlendState& state) override;
		virtual GRAPHICS_ERROR DestroyBlendState(Utilities::GUID id) override;

		virtual GRAPHICS_ERROR CreateDepthStencilState(Utilities::GUID id, const Pipeline::DepthStencilState& state) override;
		virtual GRAPHICS_ERROR DestroyDepthStencilState(Utilities::GUID id) override;

		virtual GRAPHICS_ERROR CreateSamplerState(Utilities::GUID id, const Pipeline::SamplerState& state) override;
		virtual GRAPHICS_ERROR DestroySamplerState(Utilities::GUID id) override;

		virtual GRAPHICS_ERROR CreateTarget(Utilities::GUID id, const Pipeline::Target& target) override;
		virtual GRAPHICS_ERROR DestroyTarget(Utilities::GUID id) override;

		virtual GRAPHICS_ERROR CreateDepthStencilView(Utilities::GUID id, const Pipeline::DepthStencilView& view) override;
		virtual GRAPHICS_ERROR DestroyDepthStencilView(Utilities::GUID id) override;

		GRAPHICS_ERROR UpdatePipelineObjects();

		virtual UpdateObject* GetUpdateObject(Utilities::GUID id, PipelineObjectType type);
	protected:
		ID3D11Device * device;
		ID3D11DeviceContext* context;
		
		std::array<std::unordered_map<Utilities::GUID, PipelineObject, Utilities::GUID::Hasher>, PipelineObjects::NUM_TYPES> objects_RenderSide;
		std::array<std::set<Utilities::GUID, Utilities::GUID::Compare>, PipelineObjects::NUM_TYPES> objects_ClientSide;
		PipelineObject o;
		struct ToAdd
		{
			Utilities::GUID id;
			PipelineObject obj;
			ToAdd& operator=(ToAdd&& other)
			{
				id = other.id;
				obj = std::move(other.obj);
				return *this;
			}
		};
		Utilities::CircularFiFo<ToAdd> toAdd;
		struct ToRemove
		{
			Utilities::GUID id;
			uint32_t type;
		};
		Utilities::CircularFiFo<ToRemove> toRemove;

	/**<Key is evaluated by (GUID(shader) + GUID(resourceBindingName))*/
		std::unordered_map<Utilities::GUID, int, Utilities::GUID::Hasher> shaderAndResourceNameToBindSlot;


	};
}
#endif