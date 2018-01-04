#ifndef _GRAPHICS_RENDERER_DX11_H_
#define _GRAPHICS_RENDERER_DX11_H_
#include <Graphics\Renderer_Interface.h>
#include "DeviceHandler.h"
namespace Graphics
{
	class Renderer_DX11 : public Renderer_Interface
	{
	public:
		Renderer_DX11(const RendererInitializationInfo & ii);
		~Renderer_DX11();

		Graphics_Error Initialize() override;
		void Shutdown()  override;

		void Pause()  override;
		Graphics_Error Start()  override;

		Graphics_Error UpdateSettings(const RendererInitializationInfo& ii)  override;
		const RendererInitializationInfo& GetSettings()const  override;

	private:
		RendererInitializationInfo settings;
		DeviceHandler device;
	};
}
#endif