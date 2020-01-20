#ifndef _GRAPHICS_PIPELINE_HANDLER_H_
#define _GRAPHICS_PIPELINE_HANDLER_H_
#include <Graphics\PipelineHandler_Interface.h>
#include <d3d11.h>
#include <unordered_map>
#include <array>
#include <Utilities/CircularFIFO.h>
#include <set>
#include <Graphics\UpdateJob.h>
#include "PipelineObjects.h"
#include <Utilities/ErrorHandling.h>

namespace Graphics
{
	class PipelineHandler : public PipelineHandler_Interface
	{
	public:
		PipelineHandler( ID3D11Device* device, ID3D11DeviceContext* context,
						 ID3D11RenderTargetView* backbuffer, ID3D11ShaderResourceView* bbsrv,
						 ID3D11DepthStencilView* dsv, ID3D11ShaderResourceView* dsvsrv,
						 const D3D11_VIEWPORT& vp );
		virtual ~PipelineHandler()noexcept;

		virtual void CreateBuffer( Utilities::GUID id, const Pipeline::Buffer& buffer ) override;
		/*virtual void UpdateBuffer(Utilities::GUID id, void* data, size_t size) override;
		virtual void UpdateBuffer(Utilities::GUID id, const std::function<void(void* mappedResource)>& mapCallback) override;*/
		virtual void DestroyBuffer( Utilities::GUID id )noexcept override;

		virtual void CreateViewport( Utilities::GUID id, const Pipeline::Viewport& viewport ) override;

		virtual void CreateShader( Utilities::GUID id, Pipeline::ShaderType type, const char* sourceCode, size_t size, const char* entryPoint, const char* shaderModel ) override;
		virtual void CreateShader( Utilities::GUID id, Pipeline::ShaderType type, void* data, size_t size ) override;
		virtual void DestroyShader( Utilities::GUID id, Pipeline::ShaderType type )noexcept override;

		//virtual void CreateTexture( Utilities::GUID id, void* data, size_t width, size_t height ) override;
		//virtual void DestroyTexture( Utilities::GUID id )noexcept override;

		virtual void CreateRasterizerState( Utilities::GUID id, const Pipeline::RasterizerState& state ) override;
		virtual void DestroyRasterizerState( Utilities::GUID id )noexcept override;

		virtual void CreateBlendState( Utilities::GUID id, const Pipeline::BlendState& state ) override;
		virtual void DestroyBlendState( Utilities::GUID id )noexcept override;

		virtual void CreateDepthStencilState( Utilities::GUID id, const Pipeline::DepthStencilState& state ) override;
		virtual void DestroyDepthStencilState( Utilities::GUID id )noexcept override;

		virtual void CreateSamplerState( Utilities::GUID id, const Pipeline::SamplerState& state ) override;
		virtual void DestroySamplerState( Utilities::GUID id )noexcept override;

		virtual void CreateTexture( Utilities::GUID id, const Pipeline::Texture& target ) override;
		virtual void DestroyTexture( Utilities::GUID id )noexcept override;

		virtual void CreateDepthStencilView( Utilities::GUID id, const Pipeline::DepthStencilView& view ) override;
		virtual void DestroyDepthStencilView( Utilities::GUID id )noexcept override;

		virtual void UpdatePipelineObjects()noexcept;

		virtual void UpdateObject(Utilities::GUID id, PipelineObjectType type, const std::function<void( UpdateObjectRef & obj )>& cb );
	protected:
		ID3D11Device * device;
		ID3D11DeviceContext* context;
		
		std::array<std::unordered_map<Utilities::GUID, PipelineObject, Utilities::GUID::Hasher>, PipelineObjects::NUM_TYPES> objects_RenderSide;
		std::array<std::set<Utilities::GUID, Utilities::GUID::Compare>, PipelineObjects::NUM_TYPES> objects_ClientSide;
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