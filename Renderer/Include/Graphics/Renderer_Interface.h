#ifndef _GRAPHICS_RENDERER_INTERFACE_H_
#define _GRAPHICS_RENDERER_INTERFACE_H_
#include "../DLLExport.h"
namespace Graphics
{
	class Renderer_Interface
	{
	public:
		virtual ~Renderer_Interface() {};
	protected:
		Renderer_Interface() {};
	};
}
enum class Renderer_Backend
{
	DIRECTX11
};
DECLDIR_GRAPHICS_C Graphics::Renderer_Interface* CreateRenderer(Renderer_Backend backend);

#endif
