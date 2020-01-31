#ifndef _GRAPHICS_DEVICE_HANDLER_H_
#define _GRAPHICS_DEVICE_HANDLER_H_

#include <d3d11.h>
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <Windows.h>
#include <Graphics/Graphics_Exception.h>
#include <wrl.h>
#include <Graphics/Renderer_Interface.h>
using Microsoft::WRL::ComPtr;

namespace Renderer {


	class DeviceHandler {

	public:
		/**
		* @brief Init initializes the DeviceHandler
		* @details Calls for all necessary functions for all components to be initialized
		* @exception
		* @endcode
		*/
		DeviceHandler( const RendererInitializationInfo& ii );
		/**
		* @brief Shutdown releases the DirectX components
		* @details Releases the pointers
		* @endcode
		*/
		~DeviceHandler();

		void CreateDeviceResources();


		void CreateSwapChain( const RendererInitializationInfo& ii );


		void CreateBackBufferRTV();


		void CreateDepthStencil();

		/**
		* @brief SetViewport initializes the default viewport
		* @details From the back buffer texture, width and height is used to set the size of the viewport
		* @endcode
		*/
		void SetViewport()noexcept;


		/**
		* @brief Present displays the rendered image to the user
		* @endcode
		*/
		HRESULT Present(UINT vsync)noexcept;

		/**
		* @brief Resizes the swapchain
		* @param[in] windowHandle A window handle.
		* @endcode
		*/
		void ResizeSwapChain( const RendererInitializationInfo& ii );

		/**
		* @brief Creates a blend state
		* @endcode
		*/
		//void CreateBlendState();

		inline ComPtr <ID3D11Device>           GetDevice() { return gDevice; };
		inline ComPtr <ID3D11DeviceContext>    GetDeviceContext() { return gDeviceContext; };
		inline ComPtr <ID3D11DeviceContext>    GetSecondaryDeviceContext() { return gSecDeviceContext; };
		inline ComPtr <ID3D11RenderTargetView> GetRTV() const { return gBackbufferRTV; };
		inline ComPtr <ID3D11ShaderResourceView> GetSRV() const { return gBBSRV; };
		inline ComPtr <ID3D11DepthStencilView> GetDepthStencil() { return gDepthStencilView; };
		inline ComPtr <ID3D11ShaderResourceView> GetDepthSRV() const { return gDepthStencilSRV; };
		//inline ComPtr<ID3D11BlendState>    GetBlendState() { return blendTransState; };
		inline const D3D11_VIEWPORT&   GetViewport()const { return gViewportDefault; }
		/*inline void SetDepthStencilStateAndRS()noexcept
		{
			gDeviceContext->RSSetState(rasterSolidState.Get());
			gDeviceContext->OMSetDepthStencilState(pDSState.Get(), 1);
		}*/

		/*inline void SetRasterStateFill(uint8_t fillSolid)noexcept
		{
			if (fillSolid == 1)
			{
				gDeviceContext->RSSetState(rasterSolidState.Get());
			}
			else
			{
				gDeviceContext->RSSetState(rasterWireState.Get());
			}
		}

		inline void SetBlendTransparencyState(uint8_t transparency)noexcept
		{
			if (transparency == 1)
			{
				UINT sampleM = 0xFF;
				gDeviceContext->OMSetBlendState(blendTransState.Get(), NULL, sampleM);
			}
			else
			{
				UINT sampleM = 0xFF;
				gDeviceContext->OMSetBlendState(blendSolidState.Get(), NULL, sampleM);
			}
		}*/

		inline D3D11_TEXTURE2D_DESC GetTexDesc()noexcept
		{
			return gBB_Desc;
		}

		inline ComPtr <ID3D11ShaderResourceView> GetDepthStencilSRV()noexcept
		{
			return gDepthStencilSRV;
		}

		inline ComPtr <ID3D11Texture2D> GetBackBufferTexture()noexcept
		{
			return gBackBuffer;
		}
	private:

		ComPtr<ID3D11Device> gDevice;
		ComPtr<ID3D11DeviceContext> gDeviceContext;
		ComPtr<ID3D11DeviceContext> gSecDeviceContext;
		ComPtr<IDXGISwapChain> 		gSwapChain;

		ComPtr<ID3D11Texture2D> 	gBackBuffer;
		ComPtr<ID3D11RenderTargetView> gBackbufferRTV;
		ComPtr<ID3D11ShaderResourceView> gBBSRV;

		ComPtr<ID3D11Texture2D> 	gDepthStencil;
		ComPtr<ID3D11DepthStencilView> gDepthStencilView;
		ComPtr<ID3D11ShaderResourceView> gDepthStencilSRV;
		ComPtr<ID3D11DepthStencilState > pDSState;

		/*ComPtr<ID3D11BlendState> 	blendSolidState;
		ComPtr<ID3D11BlendState> 	blendTransState;
		ComPtr<ID3D11RasterizerState > rasterSolidState;
		ComPtr<ID3D11RasterizerState > rasterWireState;*/

		D3D11_TEXTURE2D_DESC	gBB_Desc;
		D3D_FEATURE_LEVEL		gFeatureLevel;
		D3D11_VIEWPORT			gViewportDefault;

	};
}


#endif 
