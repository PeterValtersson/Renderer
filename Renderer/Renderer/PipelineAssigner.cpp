#include "PipelineAssigner.h"


namespace Graphics
{
	PipelineAssigner::PipelineAssigner( ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context,
										ComPtr<ID3D11RenderTargetView> backbuffer, ComPtr<ID3D11ShaderResourceView> bbsrv,
										ComPtr<ID3D11DepthStencilView> dsv, ComPtr<ID3D11ShaderResourceView> dsvsrv,
										const D3D11_VIEWPORT& vp ) : PipelineHandler(device, context, backbuffer, bbsrv, dsv, dsvsrv, vp)
	{
	}


	PipelineAssigner::~PipelineAssigner()
	{
	}
}