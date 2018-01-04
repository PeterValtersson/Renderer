#ifndef _GRAPHICS_RENDERER_DX11_H_
#define _GRAPHICS_RENDERER_DX11_H_
#include <Graphics\Renderer_Interface.h>
namespace Graphics
{
	class Renderer_DX11 : public Renderer_Interface
	{
	public:
		Renderer_DX11();
		~Renderer_DX11();
	};
}
#endif