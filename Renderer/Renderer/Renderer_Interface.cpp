#include <Graphics\Renderer_Interface.h>
#include "Renderer_DX11.h"
//DECLDIR_GRAPHICS_C Renderer::Renderer_Interface * CreateRenderer(Renderer_Backend backend, const Renderer::RendererInitializationInfo & ii)
//{
//	switch (backend)
//	{
//	case Renderer_Backend::DIRECTX11:
//		return new Renderer::Renderer_DX11(ii);
//		break;
//	default:
//		break;
//	}
//	return nullptr;
//}
//
//DECLDIR_GRAPHICS_C Utilities::Error Renderer_Initialize_C(Renderer::Renderer_Interface * r)
//{
//	return r->Initialize();
//}
//
//DECLDIR_GRAPHICS_C void Renderer_Shutdown_C(Renderer::Renderer_Interface * r)
//{
//	r->Shutdown();
//	delete r;
//}
//
//DECLDIR_GRAPHICS_C void Renderer_Pause_C(Renderer::Renderer_Interface * r)
//{
//	r->Pause();
//}
//
//DECLDIR_GRAPHICS_C Utilities::Error Renderer_Start_C(Renderer::Renderer_Interface * r)
//{
//	return r->Start();
//}
//
//DECLDIR_GRAPHICS_C Utilities::Error Renderer_UpdateSettings_C(Renderer::Renderer_Interface * r, Renderer::RendererInitializationInfo ii)
//{
//	return r->UpdateSettings(ii);
//}
//
//DECLDIR_GRAPHICS_C Renderer::RendererInitializationInfo Renderer_GetSettings_C(Renderer::Renderer_Interface * r)
//{
//	return r->GetSettings();
//}
//
//Renderer::Renderer_Interface * Renderer::Get()
//{
//	if (!renderer)
//		THROW_ERROR("No renderer instance");
//	return renderer;
//}

DECLDIR_GRAPHICS std::shared_ptr<Renderer::Renderer_Interface> Renderer::Renderer_Interface::Create_Renderer( Renderer_Backend backend, const RendererInitializationInfo& ii )
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
