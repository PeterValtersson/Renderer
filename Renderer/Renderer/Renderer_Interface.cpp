#include <Graphics\Renderer_Interface.h>
#include "Renderer_DX11.h"
//DECLDIR_GRAPHICS_C Graphics::Renderer_Interface * CreateRenderer(Renderer_Backend backend, const Graphics::RendererInitializationInfo & ii)
//{
//	switch (backend)
//	{
//	case Renderer_Backend::DIRECTX11:
//		return new Graphics::Renderer_DX11(ii);
//		break;
//	default:
//		break;
//	}
//	return nullptr;
//}
//
//DECLDIR_GRAPHICS_C Utilities::Error Renderer_Initialize_C(Graphics::Renderer_Interface * r)
//{
//	return r->Initialize();
//}
//
//DECLDIR_GRAPHICS_C void Renderer_Shutdown_C(Graphics::Renderer_Interface * r)
//{
//	r->Shutdown();
//	delete r;
//}
//
//DECLDIR_GRAPHICS_C void Renderer_Pause_C(Graphics::Renderer_Interface * r)
//{
//	r->Pause();
//}
//
//DECLDIR_GRAPHICS_C Utilities::Error Renderer_Start_C(Graphics::Renderer_Interface * r)
//{
//	return r->Start();
//}
//
//DECLDIR_GRAPHICS_C Utilities::Error Renderer_UpdateSettings_C(Graphics::Renderer_Interface * r, Graphics::RendererInitializationInfo ii)
//{
//	return r->UpdateSettings(ii);
//}
//
//DECLDIR_GRAPHICS_C Graphics::RendererInitializationInfo Renderer_GetSettings_C(Graphics::Renderer_Interface * r)
//{
//	return r->GetSettings();
//}
//
//Graphics::Renderer_Interface * Graphics::Get()
//{
//	if (!renderer)
//		THROW_ERROR("No renderer instance");
//	return renderer;
//}

DECLDIR_GRAPHICS std::shared_ptr<Graphics::Renderer_Interface> Graphics::Renderer_Interface::Create_Renderer( Renderer_Backend backend, const RendererInitializationInfo& ii )
{
	switch ( backend )
	{
	case Renderer_Backend::DIRECTX11:
		return std::make_shared<Renderer_DX11>( ii );
		break;
	default:
		break;
	}
	return nullptr;
}
