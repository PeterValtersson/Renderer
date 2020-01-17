#ifndef _GRAPHICS_PIPELINE_HANDLER_INTERFACE_H_
#define _GRAPHICS_PIPELINE_HANDLER_INTERFACE_H_
#include "Pipeline.h"
#include "Graphics_Exception.h"
#include <functional>
#include <Utilities/GUID.h>

namespace Graphics
{

	struct Could_Not_Create_Buffer : Graphics_Exception{
		Could_Not_Create_Buffer( const std::string& what, Utilities::GUID ID, const Pipeline::Buffer& info, long error )
			: Graphics_Exception( "Buffer with ID " + ID.to_string() + " could not be created.\n Error: " + std::to_string( error ) + "\n" + what, error ), ID( ID ), info( info )
		{}
		Could_Not_Create_Buffer( const std::string& what, Utilities::GUID ID, const Pipeline::Buffer& info )
			: Graphics_Exception( "Buffer with ID " + ID.to_string() + " could not be created.\n " + what ), ID( ID ), info( info )
		{}
		Pipeline::Buffer info;
		Utilities::GUID ID;
	};
	struct Could_Not_Create_Shader : Graphics_Exception{
		Could_Not_Create_Shader( const std::string& what, Utilities::GUID ID, Pipeline::ShaderType type, long error )
			: Graphics_Exception( "Shader with ID " + ID.to_string() + " could not be created.\n Error: " + std::to_string( error ) + "\n" + what, error ), ID( ID ), type( type )
		{}
		Could_Not_Create_Shader( const std::string& what, Utilities::GUID ID, const Pipeline::ShaderType& type )
			: Graphics_Exception( "Shader with ID " + ID.to_string() + " could not be created.\n " + what ), ID( ID ), type( type )
		{}
		Pipeline::ShaderType type;
		Utilities::GUID ID;
	};

	

	struct Could_Not_Create_RasterizerState : Graphics_Exception{
		Could_Not_Create_RasterizerState( const std::string& what, Utilities::GUID ID, const Pipeline::RasterizerState& info, long error )
			: Graphics_Exception( "RasterizerState with ID " + ID.to_string() + " could not be created.\n Error: " + std::to_string( error ) + "\n" + what, error ), ID( ID ), info( info )
		{}
		Could_Not_Create_RasterizerState( const std::string& what, Utilities::GUID ID, const Pipeline::RasterizerState& info )
			: Graphics_Exception( "RasterizerState with ID " + ID.to_string() + " could not be created.\n " + what ), ID( ID ), info( info )
		{}
		Pipeline::RasterizerState info;
		Utilities::GUID ID;
	};

	struct Could_Not_Create_BlendState : Graphics_Exception{
		Could_Not_Create_BlendState( const std::string& what, Utilities::GUID ID, const Pipeline::BlendState& info, long error )
			: Graphics_Exception( "BlendState with ID " + ID.to_string() + " could not be created.\n Error: " + std::to_string( error ) + "\n" + what, error ), ID( ID ), info( info )
		{}
		Could_Not_Create_BlendState( const std::string& what, Utilities::GUID ID, const Pipeline::BlendState& info )
			: Graphics_Exception( "BlendState with ID " + ID.to_string() + " could not be created.\n " + what ), ID( ID ), info( info )
		{}
		Pipeline::BlendState info;
		Utilities::GUID ID;
	};

	struct Could_Not_Create_DepthStencilState : Graphics_Exception{
		Could_Not_Create_DepthStencilState( const std::string& what, Utilities::GUID ID, const Pipeline::DepthStencilState& info, long error )
			: Graphics_Exception( "DepthStencilState with ID " + ID.to_string() + " could not be created.\n Error: " + std::to_string( error ) + "\n" + what, error ), ID( ID ), info( info )
		{}
		Could_Not_Create_DepthStencilState( const std::string& what, Utilities::GUID ID, const Pipeline::DepthStencilState& info )
			: Graphics_Exception( "DepthStencilState with ID " + ID.to_string() + " could not be created.\n " + what ), ID( ID ), info( info )
		{}
		Pipeline::DepthStencilState info;
		Utilities::GUID ID;
	};

	struct Could_Not_Create_SamplerState : Graphics_Exception{
		Could_Not_Create_SamplerState( const std::string& what, Utilities::GUID ID, const Pipeline::SamplerState& info, long error )
			: Graphics_Exception( "SamplerState with ID " + ID.to_string() + " could not be created.\n Error: " + std::to_string( error ) + "\n" + what, error ), ID( ID ), info( info )
		{}
		Could_Not_Create_SamplerState( const std::string& what, Utilities::GUID ID, const Pipeline::SamplerState& info )
			: Graphics_Exception( "SamplerState with ID " + ID.to_string() + " could not be created.\n " + what ), ID( ID ), info( info )
		{}
		Pipeline::SamplerState info;
		Utilities::GUID ID;
	};

	struct Could_Not_Create_Texture : Graphics_Exception{
		Could_Not_Create_Texture( const std::string& what, Utilities::GUID ID, const Pipeline::Texture& info, long error )
			: Graphics_Exception( "Texture with ID " + ID.to_string() + " could not be created.\n Error: " + std::to_string( error ) + "\n" + what, error ), ID( ID ), info( info )
		{}
		Could_Not_Create_Texture( const std::string& what, Utilities::GUID ID, const Pipeline::Texture& info )
			: Graphics_Exception( "Texture with ID " + ID.to_string() + " could not be created.\n " + what ), ID( ID ), info( info )
		{}
		Pipeline::Texture info;
		Utilities::GUID ID;
	};

	struct Could_Not_Create_DepthStencilView : Graphics_Exception{
		Could_Not_Create_DepthStencilView( const std::string& what, Utilities::GUID ID, const Pipeline::DepthStencilView& info, long error )
			: Graphics_Exception( "DepthStencilView with ID " + ID.to_string() + " could not be created.\n Error: " + std::to_string( error ) + "\n" + what, error ), ID( ID ), info( info )
		{}
		Could_Not_Create_DepthStencilView( const std::string& what, Utilities::GUID ID, const Pipeline::DepthStencilView& info )
			: Graphics_Exception( "DepthStencilView with ID " + ID.to_string() + " could not be created.\n " + what ), ID( ID ), info( info )
		{}
		Pipeline::DepthStencilView info;
		Utilities::GUID ID;
	};
	struct Could_Not_Create_Viewport : Graphics_Exception{
		Could_Not_Create_Viewport( const std::string& what, Utilities::GUID ID, const Pipeline::Viewport& info )
			: Graphics_Exception( "Viewport with ID " + ID.to_string() + " could not be created.\n" + what ), ID( ID ), info( info )
		{}

		Pipeline::Viewport info;
		Utilities::GUID ID;
	};
	class PipelineHandler_Interface{
	public:

		PipelineHandler_Interface()
		{};

		virtual void CreateBuffer( Utilities::GUID id, const Pipeline::Buffer& buffer ) = 0;
		/*virtual void UpdateBuffer(Utilities::GUID id, void* data, size_t size) = 0;
		virtual void UpdateBuffer(Utilities::GUID id, const std::function<void(void* mappedResource)>& mapCallback) = 0;*/
		virtual void DestroyBuffer( Utilities::GUID id )noexcept = 0;

		virtual void CreateViewport( Utilities::GUID id, const Pipeline::Viewport& viewport ) = 0;

		virtual void CreateShader( Utilities::GUID id, Pipeline::ShaderType type, const char* sourceCode, size_t size, const char* entryPoint, const char* shaderModel ) = 0;
		virtual void CreateShader( Utilities::GUID id, Pipeline::ShaderType type, void* data, size_t size ) = 0;
		virtual void DestroyShader( Utilities::GUID id, Pipeline::ShaderType type )noexcept = 0;

		/*virtual void CreateTexture( Utilities::GUID id, void* data, size_t width, size_t height ) = 0;
		virtual void DestroyTexture( Utilities::GUID id )noexcept = 0;*/

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

	protected:
		virtual ~PipelineHandler_Interface()noexcept
		{};
	};
}
#endif