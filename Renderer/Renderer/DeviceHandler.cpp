#include "DeviceHandler.h"
#include "Safe_Release.h"
#include <Profiler.h>

#pragma comment(lib, "d3d11.lib")

using namespace DirectX;
namespace Graphics
{
	DeviceHandler::DeviceHandler() : gFeatureLevel()
	{
		gDevice = nullptr;
		gDeviceContext = nullptr;
		gSecDeviceContext = nullptr;
		gSwapChain = nullptr;
		gBackBuffer = nullptr;
		gBackbufferRTV = nullptr;
		gBBSRV = nullptr;
		gDepthStencil = nullptr;
		gDepthStencilView = nullptr;
		gDepthStencilSRV = nullptr;
		pDSState = nullptr;
		blendSolidState = nullptr;
		blendTransState = nullptr;
		rasterSolidState = nullptr;
		rasterWireState = nullptr;
	}

	DeviceHandler::~DeviceHandler() {


	}

	GRAPHICS_ERROR DeviceHandler::Init(HWND windowHandle, bool fullscreen, bool borderless, UINT bufferCount) {

		StartProfile;

		auto res = CreateDeviceResources();
		PASS_IF_ERROR(res);

		res = CreateSwapChain(windowHandle, fullscreen, borderless, bufferCount);
		PASS_IF_ERROR(res);

		res = CreateBackBufferRTV();
		PASS_IF_ERROR(res);

		res = CreateDepthStencil();
		PASS_IF_ERROR(res);

		SetViewport();
		CreateBlendState();


		D3D11_RASTERIZER_DESC rasterizerState;
		rasterizerState.FillMode = D3D11_FILL_SOLID;
		rasterizerState.CullMode = D3D11_CULL_NONE;
		rasterizerState.FrontCounterClockwise = false;
		rasterizerState.DepthBias = false;
		rasterizerState.DepthBiasClamp = 0;
		rasterizerState.SlopeScaledDepthBias = 0;
		rasterizerState.DepthClipEnable = true;
		rasterizerState.ScissorEnable = false;
		rasterizerState.MultisampleEnable = false;
		rasterizerState.AntialiasedLineEnable = false;

		auto hr = gDevice->CreateRasterizerState(&rasterizerState, &rasterSolidState);
		RETURN_IF_ERROR(hr, "Failed to create rasterizer state");

		gDeviceContext->RSSetState(rasterSolidState);

		rasterizerState.FillMode = D3D11_FILL_WIREFRAME;

		hr = gDevice->CreateRasterizerState(&rasterizerState, &rasterWireState);
		RETURN_IF_ERROR(hr, "Failed to create rasterizer state");


		RETURN_SUCCESS;

	}

	void DeviceHandler::Shutdown() {

		StartProfile;
		Safe_Release(gBBSRV);
		Safe_Release(gBackBuffer);
		Safe_Release(gBackbufferRTV);
		Safe_Release(pDSState);
		Safe_Release(gDepthStencilView);
		Safe_Release(gDepthStencilSRV);
		Safe_Release(blendSolidState);
		Safe_Release(blendTransState);
		Safe_Release(rasterSolidState);
		Safe_Release(rasterWireState);
		Safe_Release(gSwapChain);
		Safe_Release(gSecDeviceContext);
		Safe_Release(gDeviceContext);
		Safe_Release(gDevice);
	}

	GRAPHICS_ERROR DeviceHandler::CreateDeviceResources() {

		StartProfile;

		D3D_FEATURE_LEVEL levels[] = {

			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_11_1

		};

		UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)

		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

#endif
		HRESULT hr = D3D11CreateDevice(

			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			deviceFlags,
			levels,
			ARRAYSIZE(levels),
			D3D11_SDK_VERSION,
			&gDevice,
			&gFeatureLevel,
			&gDeviceContext

		);
		RETURN_IF_ERROR(hr, "Could not create device and device context");

		hr = gDevice->CreateDeferredContext(0, &gSecDeviceContext);
		RETURN_IF_ERROR(hr, "Could not create deferred context");

		RETURN_SUCCESS;
	}

	GRAPHICS_ERROR DeviceHandler::CreateSwapChain(HWND windowHandle, bool fullscreen, bool borderless, UINT bufferCount) {

		StartProfile;

		DXGI_SWAP_CHAIN_DESC swChDesc;
		ZeroMemory(&swChDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		swChDesc.Windowed = (fullscreen && !borderless)? FALSE : TRUE;
		swChDesc.BufferCount = bufferCount;
		swChDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swChDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		swChDesc.SampleDesc.Count = 1;
		swChDesc.SampleDesc.Quality = 0;
		swChDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swChDesc.OutputWindow = windowHandle;
		
		IDXGIDevice* dxgiDevice = 0;
		HRESULT hr = gDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)& dxgiDevice);
		RETURN_IF_ERROR(hr, "Failed to query interface, DXGIDevice");

		IDXGIAdapter* dxgiAdapter = 0;
		hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)& dxgiAdapter);
		RETURN_IF_ERROR(hr, "Failed to get DXGIAdapter");

		IDXGIFactory* dxgiFactory = 0;
		hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)& dxgiFactory);
		RETURN_IF_ERROR(hr, "Failed to get DXGI Factory");

		hr = dxgiFactory->CreateSwapChain(gDevice, &swChDesc, &gSwapChain);
		RETURN_IF_ERROR(hr, "Failed to create swap chain");

		Safe_Release(dxgiFactory);
		Safe_Release(dxgiAdapter);
		Safe_Release(dxgiDevice);
		RETURN_SUCCESS;
	}

	GRAPHICS_ERROR DeviceHandler::CreateBackBufferRTV() {

		StartProfile;

		HRESULT hr = gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&gBackBuffer);
		RETURN_IF_ERROR(hr, "Could not get backbuffer texture");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = 1;

		hr = gDevice->CreateShaderResourceView(gBackBuffer, &srvd, &gBBSRV);
		RETURN_IF_ERROR(hr, "Could not create shader resource view");


		hr = gDevice->CreateRenderTargetView(gBackBuffer, nullptr, &gBackbufferRTV);
		RETURN_IF_ERROR(hr, "Could not create render target view");
	
		RETURN_SUCCESS;
	}

	GRAPHICS_ERROR DeviceHandler::CreateDepthStencil() {

		StartProfile;

		gBackBuffer->GetDesc(&gBB_Desc);
		D3D11_TEXTURE2D_DESC td;
		td.Width = static_cast<UINT>(gBB_Desc.Width);
		td.Height = static_cast<UINT>(gBB_Desc.Height);
		td.ArraySize = 1;
		td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		td.Format = DXGI_FORMAT_R24G8_TYPELESS;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.MipLevels = 1;
		td.MiscFlags = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.CPUAccessFlags = 0;

		ID3D11Texture2D* depthTexture;
		HRESULT hr = gDevice->CreateTexture2D(&td, nullptr, &depthTexture);
		RETURN_IF_ERROR(hr, "Could not create depth texture");

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Texture2D.MipSlice = 0;
		dsvd.Flags = 0;

		hr = gDevice->CreateDepthStencilView(depthTexture, &dsvd, &gDepthStencilView);
		RETURN_IF_ERROR(hr, "Could not create depth stencil view");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = td.MipLevels;
		srvd.Texture2D.MostDetailedMip = 0;

		hr = gDevice->CreateShaderResourceView(depthTexture, &srvd, &gDepthStencilSRV);
		RETURN_IF_ERROR(hr, "Could not create depth stencil SRV");

		D3D11_DEPTH_STENCIL_DESC dsDesc;

		// Depth test parameters
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

		// Stencil test parameters
		dsDesc.StencilEnable = true;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create depth stencil state
		hr =gDevice->CreateDepthStencilState(&dsDesc, &pDSState);
		RETURN_IF_ERROR(hr, "Could not create depth stencil state");

		gDeviceContext->OMSetDepthStencilState(pDSState, 1);
		Safe_Release(depthTexture);
		
		RETURN_SUCCESS;
	}

	void DeviceHandler::SetViewport() {
		StartProfile;
		ZeroMemory(&gViewportDefault, sizeof(D3D11_VIEWPORT));
		gViewportDefault.TopLeftX = 0.0f;
		gViewportDefault.TopLeftY = 0.0f;
		gViewportDefault.MinDepth = 0.0f;
		gViewportDefault.MaxDepth = 1.0f;
		gViewportDefault.Width = (FLOAT)gBB_Desc.Width;
		gViewportDefault.Height = (FLOAT)gBB_Desc.Height;

		gDeviceContext->RSSetViewports(1, &gViewportDefault);
	}



	HRESULT DeviceHandler::Present(UINT vsync) {

		return gSwapChain->Present(vsync, 0);
	}

	GRAPHICS_ERROR DeviceHandler::ResizeSwapChain(HWND windowHandle, bool fullscreen, bool borderless, UINT bufferCount)
	{
		StartProfile;
		Safe_Release(gDepthStencilSRV);
		Safe_Release(gDepthStencilView);
		Safe_Release(pDSState);
		Safe_Release(gBackbufferRTV);
		Safe_Release(gBBSRV);
		Safe_Release(gBackBuffer);
		Safe_Release(gSwapChain);
		
		PASS_IF_ERROR(CreateSwapChain(windowHandle, fullscreen, borderless, bufferCount));
		PASS_IF_ERROR(CreateBackBufferRTV());
		PASS_IF_ERROR(CreateDepthStencil());
		SetViewport();

		RETURN_SUCCESS;
	}

	GRAPHICS_ERROR DeviceHandler::CreateBlendState()
	{
		StartProfile;
		// Transparency off
		D3D11_RENDER_TARGET_BLEND_DESC rendTarBlendState[8];
		for (auto& rtbs : rendTarBlendState)
		{
			rtbs.BlendEnable = false;
			rtbs.BlendOp = D3D11_BLEND_OP_ADD;
			rtbs.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			rtbs.DestBlend = D3D11_BLEND_ZERO;
			rtbs.DestBlendAlpha = D3D11_BLEND_ZERO;
			rtbs.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			rtbs.SrcBlend = D3D11_BLEND_ONE;
			rtbs.SrcBlendAlpha = D3D11_BLEND_ONE;
		}

		// just default values right now
		D3D11_BLEND_DESC blendStateDesc;
		blendStateDesc.AlphaToCoverageEnable = false;
		blendStateDesc.IndependentBlendEnable = false;
		blendStateDesc.RenderTarget[0] = rendTarBlendState[0];
		blendStateDesc.RenderTarget[1] = rendTarBlendState[1];
		blendStateDesc.RenderTarget[2] = rendTarBlendState[2];
		blendStateDesc.RenderTarget[3] = rendTarBlendState[3];
		blendStateDesc.RenderTarget[4] = rendTarBlendState[4];
		blendStateDesc.RenderTarget[5] = rendTarBlendState[5];
		blendStateDesc.RenderTarget[6] = rendTarBlendState[6];
		blendStateDesc.RenderTarget[7] = rendTarBlendState[7];

		HRESULT hr = gDevice->CreateBlendState(&blendStateDesc, &blendSolidState);
		RETURN_IF_ERROR(hr, "Could not create blend state");

		UINT sampleM = 0xffffffff;
		gDeviceContext->OMSetBlendState(blendSolidState, NULL, sampleM);

		// Transparency on
		D3D11_RENDER_TARGET_BLEND_DESC rendTransBlendState[8];
		for (auto& rtbs : rendTransBlendState)
		{
			rtbs.BlendEnable = true;
			rtbs.BlendOp = D3D11_BLEND_OP_ADD;
			rtbs.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			rtbs.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			rtbs.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			rtbs.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			rtbs.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			rtbs.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		}

		D3D11_BLEND_DESC blendTransStateDesc;
		blendTransStateDesc.AlphaToCoverageEnable = false;
		blendTransStateDesc.IndependentBlendEnable = false;
		blendTransStateDesc.RenderTarget[0] = rendTransBlendState[0];
		blendTransStateDesc.RenderTarget[1] = rendTransBlendState[1];
		blendTransStateDesc.RenderTarget[2] = rendTransBlendState[2];
		blendTransStateDesc.RenderTarget[3] = rendTransBlendState[3];
		blendTransStateDesc.RenderTarget[4] = rendTransBlendState[4];
		blendTransStateDesc.RenderTarget[5] = rendTransBlendState[5];
		blendTransStateDesc.RenderTarget[6] = rendTransBlendState[6];
		blendTransStateDesc.RenderTarget[7] = rendTransBlendState[7];

		hr = gDevice->CreateBlendState(&blendTransStateDesc, &blendTransState);
		RETURN_IF_ERROR(hr, "Could not create blend state");

		RETURN_SUCCESS;
	}

	ID3D11Texture2D* DeviceHandler::GetBackBufferTexture()
	{
		return gBackBuffer;
	}
}