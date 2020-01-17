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

		virtual void CreateBuffer( Utilities::GUID id, const Pipeline::Buffer& buffer ) = 0;
		/*virtual void UpdateBuffer(Utilities::GUID id, void* data, size_t size) = 0;
		virtual void UpdateBuffer(Utilities::GUID id, const std::function<void(void* mappedResource)>& mapCallback) = 0;*/
		virtual void DestroyBuffer( Utilities::GUID id )noexcept = 0;

		virtual void CreateViewport( Utilities::GUID id, const Pipeline::Viewport& viewport ) = 0;

		virtual void CreateShader( Utilities::GUID id, Pipeline::ShaderType type, const char* sourceCode, size_t size, const char* entryPoint, const char* shaderModel ) = 0;
		virtual void CreateShader( Utilities::GUID id, Pipeline::ShaderType type, void* data, size_t size ) = 0;
		virtual void DestroyShader( Utilities::GUID id, Pipeline::ShaderType type )noexcept = 0;

		//virtual void CreateTexture( Utilities::GUID id, void* data, size_t width, size_t height ) = 0;
		//virtual void DestroyTexture( Utilities::GUID id )noexcept = 0;

		virtual void CreateRasterizerState( Utilities::GUID id, const Pipeline::RasterizerState& state ) = 0;
		virtual void DestroyRasterizerState( Utilities::GUID id )noexcept = 0;

		virtual void CreateBlendState( Utilities::GUID id, const Pipeline::BlendState& state ) = 0;
		virtual void DestroyBlendState( Utilities::GUID id )noexcept = 0;

		virtual void CreateDepthStencilState( Utilities::GUID id, const Pipeline::DepthStencilState& state ) = 0;
		virtual void DestroyDepthStencilState( Utilities::GUID id )noexcept = 0;

		virtual void CreateSamplerState( Utilities::GUID id, const Pipeline::SamplerState& state ) = 0;
		virtual void DestroySamplerState( Utilities::GUID id )noexcept = 0;

		virtual void CreateTexture( Utilities::GUID id, const Pipeline::Texture& target ) = 0;
		virtual void DestroyTexture( Utilities::GUID id )noexcept = 0;

		virtual void CreateDepthStencilView( Utilities::GUID id, const Pipeline::DepthStencilView& view ) = 0;
		virtual void DestroyDepthStencilView( Utilities::GUID id )noexcept = 0;

		virtual void UpdatePipelineObjects()noexcept;

		virtual void UpdateObject(Utilities::GUID id, PipelineObjectType type, const std::function<void( UpdateObjectRef & obj )>& cb );
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