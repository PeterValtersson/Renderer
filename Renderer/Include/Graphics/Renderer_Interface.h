#ifndef _GRAPHICS_RENDERER_INTERFACE_H_
#define _GRAPHICS_RENDERER_INTERFACE_H_
#include "../DLLExport.h"
#include "PipelineHandler_Interface.h"
#include <stdint.h>
#include "RenderJob.h"
#include "UpdateJob.h"
#include <Utilities/GUID.h>
#include <Utilities/ErrorHandling.h>
namespace Renderer
{

	struct Could_Not_Create_Device : Graphics_Exception{
		Could_Not_Create_Device( std::string_view type, long error ) : Graphics_Exception( "Could not create rendering device.\nErrors: " + std::to_string( error )  + "\nType: " + std::string(type)), type( std::string(type) )
		{}
		std::string type;
	};

	struct Could_Not_Create_SwapChain : Graphics_Exception{
		Could_Not_Create_SwapChain( std::string_view what, long error ) : Graphics_Exception( "Could not create SwapChain.\nErrors: " + std::to_string( error ) + "\n" + std::string( what ) )
		{}
	};

	struct Could_Not_Create_Backbuffer_Resources : Graphics_Exception{
		Could_Not_Create_Backbuffer_Resources( std::string_view what, long error ) : Graphics_Exception( "Could not create backbuffer resources.\nErrors: " + std::to_string( error ) + "\n" + std::string( what ) )
		{}
	};
	
	struct Could_Not_Create_Default_DepthStencil : Graphics_Exception{
		Could_Not_Create_Default_DepthStencil( std::string_view what, long error ) : Graphics_Exception( "Could not create default depth stencil.\nErrors: " + std::to_string( error ) + "\n" + std::string( what ) )
		{}
	};

	struct Could_Not_Create_Default_BlendState : Graphics_Exception{
		Could_Not_Create_Default_BlendState( std::string_view what, long error ) : Graphics_Exception( "Could not create default depth stencil.\nErrors: " + std::to_string( error ) + "\n" + std::string( what ) )
		{}
	};

	struct RenderJob_Exisits : Utilities::Exception{
		RenderJob_Exisits( Utilities::GUID ID ) : Utilities::Exception( "RenderJob \"" + ID.to_string() + "\" already exists" )
		{};
	};

	struct UpdateJob_Exisits : Utilities::Exception{
		UpdateJob_Exisits( Utilities::GUID ID ) : Utilities::Exception( "UpdateJob \"" + ID.to_string() + "\" already exists" )
		{};
	};

	enum class WindowState : uint8_t{
		WINDOWED,
		FULLSCREEN,
		FULLSCREEN_BORDERLESS
	};
	struct Resolution{
		uint32_t width;
		uint32_t height;
	};
	struct RendererInitializationInfo{
		void* windowHandle = nullptr;
		WindowState windowState = WindowState::WINDOWED;
		uint8_t bufferCount = 2;
		Resolution resolution = { 1280, 720 };
		uint8_t vsync = 1Ui8;
	};
	enum class Renderer_Backend{
		DIRECTX11
	};
	class Renderer_Interface{
	public:
		DECLDIR_GRAPHICS static std::shared_ptr<Renderer_Interface> Create_Renderer( Renderer_Backend backend, const RendererInitializationInfo& ii );

		virtual ~Renderer_Interface()noexcept
		{};

		virtual void Pause()noexcept = 0;
		virtual void Start()noexcept = 0;

		virtual void UpdateSettings( const RendererInitializationInfo& ii ) = 0;
		virtual const RendererInitializationInfo& GetSettings()const noexcept = 0;

		virtual void UsePipelineHandler( const std::function<void( PipelineHandler_Interface & pipeline_handler )>& callback ) = 0;

		virtual void AddRenderJob( Utilities::GUID id, const RenderJob& job, RenderGroup renderGroup ) = 0;
		// Will use the id of the pipeline
		virtual void AddRenderJob( const RenderJob& job, RenderGroup renderGroup ) = 0;
		virtual void RemoveRenderJob( Utilities::GUID id, RenderGroup renderGroup )noexcept = 0;
		virtual void RemoveRenderJob( Utilities::GUID id )noexcept = 0;
		virtual size_t GetNumberOfRenderJobs()const noexcept = 0;
		// Returns the number of groups this job is registered under.
		virtual size_t IsRenderJobRegistered( Utilities::GUID id )const noexcept = 0;

		virtual void AddUpdateJob( Utilities::GUID id, const UpdateJob& job, RenderGroup renderGroupToPerformUpdateBefore ) = 0;
		// Will use the id of the object
		virtual void AddUpdateJob( const UpdateJob& job, RenderGroup renderGroup ) = 0;
		virtual void RemoveUpdateJob( Utilities::GUID id, RenderGroup renderGroupToPerformUpdateBefore )noexcept = 0;
		virtual void RemoveUpdateJob( Utilities::GUID id )noexcept = 0;
		virtual size_t GetNumberOfUpdateJobs()const noexcept = 0;
		// Returns the number of groups this job is registered under.
		virtual size_t IsUpdateJobRegistered( Utilities::GUID id )const noexcept = 0;
	protected:
		Renderer_Interface()
		{};
	};
}

//DECLDIR_GRAPHICS_C Renderer::Renderer_Interface* CreateRenderer(Renderer_Backend backend, const Renderer::RendererInitializationInfo & ii);
//DECLDIR_GRAPHICS_C Utilities::Error Renderer_Initialize_C(Renderer::Renderer_Interface* r);
//DECLDIR_GRAPHICS_C void Renderer_Shutdown_C(Renderer::Renderer_Interface* r);
//DECLDIR_GRAPHICS_C void Renderer_Pause_C(Renderer::Renderer_Interface* r);
//DECLDIR_GRAPHICS_C Utilities::Error Renderer_Start_C(Renderer::Renderer_Interface* r);
//
//DECLDIR_GRAPHICS_C Utilities::Error Renderer_UpdateSettings_C(Renderer::Renderer_Interface* r, Renderer::RendererInitializationInfo ii);
//DECLDIR_GRAPHICS_C Renderer::RendererInitializationInfo Renderer_GetSettings_C(Renderer::Renderer_Interface* r);

#endif
