#ifndef _GRAPHICS_PIPELINE_ASSIGNER_H_
#define _GRAPHICS_PIPELINE_ASSIGNER_H_
#include "PipelineHandler.h"
#include <Graphics/Pipeline.h>

namespace Graphics
{
	class PipelineAssigner : public PipelineHandler
	{
	public:
		PipelineAssigner( ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context,
						  ComPtr<ID3D11RenderTargetView> backbuffer, ComPtr<ID3D11ShaderResourceView> bbsrv,
						  ComPtr<ID3D11DepthStencilView> dsv, ComPtr<ID3D11ShaderResourceView> dsvsrv,
						  const D3D11_VIEWPORT& vp );
		virtual ~PipelineAssigner()noexcept;


		void Set_Pipeline(const Pipeline::Pipeline& pl)
	};
}
#endif