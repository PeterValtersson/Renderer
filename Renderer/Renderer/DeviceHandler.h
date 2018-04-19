#ifndef _GRAPHICS_DEVICE_HANDLER_H_
#define _GRAPHICS_DEVICE_HANDLER_H_

#include <d3d11.h>
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <Windows.h>
#include <Error.h>

namespace Graphics {


	class DeviceHandler {

	public:

		DeviceHandler();
		~DeviceHandler();

		/**
		* @brief Init initializes the DeviceHandler
		* @details Calls for all necessary functions for all components to be initialized
		* @retval return_value_n Returns a HRESULT indicating if the DirectX core was successfully created or not
		* @endcode
		*/
		UERROR Init(HWND windowHandle, bool fullscreen, bool borderless, UINT bufferCount);

		/**
		* @brief Shutdown releases the DirectX components
		* @details Releases the pointers
		* @endcode
		*/
		void Shutdown();

		UERROR CreateDeviceResources();


		UERROR CreateSwapChain(HWND windowHandle, bool fullscreen, bool borderless, UINT bufferCount);


		UERROR CreateBackBufferRTV();


		UERROR CreateDepthStencil();

		/**
		* @brief SetViewport initializes the default viewport
		* @details From the back buffer texture, width and height is used to set the size of the viewport
		* @endcode
		*/
		void SetViewport();


		/**
		* @brief Present displays the rendered image to the user
		* @endcode
		*/
		HRESULT Present(UINT vsync);

		/**
		* @brief Resizes the swapchain
		* @param[in] windowHandle A window handle.
		* @endcode
		*/
		UERROR ResizeSwapChain(HWND windowHandle, bool fullscreen, bool borderless, UINT bufferCount);

		/**
		* @brief Creates a blend state
		* @endcode
		*/
		UERROR CreateBlendState();

		inline ID3D11Device*           GetDevice() { return gDevice; };
		inline ID3D11DeviceContext*    GetDeviceContext() { return gDeviceContext; };
		inline ID3D11DeviceContext*    GetSecondaryDeviceContext() { return gSecDeviceContext; };
		inline ID3D11RenderTargetView* GetRTV() const { return gBackbufferRTV; };
		inline ID3D11ShaderResourceView* GetSRV() const { return gBBSRV; };
		inline ID3D11DepthStencilView* GetDepthStencil() { return gDepthStencilView; };
		inline ID3D11ShaderResourceView* GetDepthSRV() const { return gDepthStencilSRV; };
		inline ID3D11BlendState*	   GetBlendState() { return blendTransState; };
		inline const D3D11_VIEWPORT&   GetViewport()const { return gViewportDefault; }
		inline void SetDepthStencilStateAndRS()
		{
			gDeviceContext->RSSetState(rasterSolidState);
			gDeviceContext->OMSetDepthStencilState(pDSState, 1);
		}

		inline void SetRasterStateFill(uint8_t fillSolid)
		{
			if (fillSolid == 1)
			{
				gDeviceContext->RSSetState(rasterSolidState);
			}
			else
			{
				gDeviceContext->RSSetState(rasterWireState);
			}
		}

		inline void SetBlendTransparencyState(uint8_t transparency)
		{
			if (transparency == 1)
			{
				UINT sampleM = 0xFF;
				gDeviceContext->OMSetBlendState(blendTransState, NULL, sampleM);
			}
			else
			{
				UINT sampleM = 0xFF;
				gDeviceContext->OMSetBlendState(blendSolidState, NULL, sampleM);
			}
		}

		inline D3D11_TEXTURE2D_DESC GetTexDesc() {
			return gBB_Desc;
		}

		inline ID3D11ShaderResourceView* GetDepthStencilSRV()
		{
			return gDepthStencilSRV;
		}

		ID3D11Texture2D* GetBackBufferTexture();
	private:

		ID3D11Device * gDevice;
		ID3D11DeviceContext*	gDeviceContext;
		ID3D11DeviceContext*	gSecDeviceContext;
		IDXGISwapChain*			gSwapChain;

		ID3D11Texture2D*		gBackBuffer;
		ID3D11RenderTargetView*	gBackbufferRTV;
		ID3D11ShaderResourceView* gBBSRV;

		ID3D11Texture2D*		gDepthStencil;
		ID3D11DepthStencilView*	gDepthStencilView;
		ID3D11ShaderResourceView* gDepthStencilSRV;
		ID3D11DepthStencilState * pDSState;

		ID3D11BlendState*		blendSolidState;
		ID3D11BlendState*		blendTransState;
		ID3D11RasterizerState * rasterSolidState;
		ID3D11RasterizerState * rasterWireState;

		D3D11_TEXTURE2D_DESC	gBB_Desc;
		D3D_FEATURE_LEVEL		gFeatureLevel;
		D3D11_VIEWPORT			gViewportDefault;

	};
}


#endif 
