#include "PipelineAssigner.h"


namespace Graphics
{
	PipelineAssigner::PipelineAssigner( ID3D11Device* device, ID3D11DeviceContext* context,
										ID3D11RenderTargetView* backbuffer, ID3D11ShaderResourceView* bbsrv,
										ID3D11DepthStencilView* dsv, ID3D11ShaderResourceView* dsvsrv,
										const D3D11_VIEWPORT& vp ) : PipelineHandler(device, context, backbuffer, bbsrv, dsv, dsvsrv, vp)
	{
	}


	PipelineAssigner::~PipelineAssigner()
	{
	}
}