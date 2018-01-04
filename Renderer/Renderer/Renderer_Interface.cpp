#include <Graphics\Renderer_Interface.h>
#include "Renderer_DX11.h"
DECLDIR_GRAPHICS_C Graphics::Renderer_Interface * CreateRenderer(Renderer_Backend backend)
{
	switch (backend)
	{
	case Renderer_Backend::DIRECTX11:
		return new Graphics::Renderer_DX11();
		break;
	default:
		break;
	}
	return nullptr;
}
