#ifndef _GRAPHICS_PIPELINE_ASSIGNER_H_
#define _GRAPHICS_PIPELINE_ASSIGNER_H_
#include "PipelineHandler.h"
namespace Graphics
{
	class PipelineAssigner : public PipelineHandler
	{
	public:
		PipelineAssigner( ID3D11Device* device, ID3D11DeviceContext* context,
						  ID3D11RenderTargetView* backbuffer, ID3D11ShaderResourceView* bbsrv,
						  ID3D11DepthStencilView* dsv, ID3D11ShaderResourceView* dsvsrv,
						  const D3D11_VIEWPORT& vp );
		virtual ~PipelineAssigner()noexcept;
	};
}
#endif