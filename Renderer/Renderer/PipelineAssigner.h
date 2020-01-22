#ifndef _GRAPHICS_PIPELINE_ASSIGNER_H_
#define _GRAPHICS_PIPELINE_ASSIGNER_H_
#include "PipelineHandler.h"
#include <Graphics/Pipeline.h>

namespace Graphics
{
	class PipelineAssigner : public PipelineHandler
	{
	public:
		PipelineAssigner( ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);

		virtual ~PipelineAssigner()noexcept;


		void Set_Pipeline( const Pipeline::Pipeline& pipeline )noexcept;
		void Clear_Pipeline()noexcept;

	private:
		void Set_InputAssemblerStage( const Pipeline::InputAssemblerStage& pIA )noexcept;
		void Set_VertexShaderStage( const Pipeline::ShaderStage& vss )noexcept;
		void Set_GeometryShaderStage( const Pipeline::ShaderStage& gss )noexcept;
		void Set_RasterizerStage( const Pipeline::RasterizerStage& rs )noexcept;
		void Set_PixelShaderStage( const Pipeline::ShaderStage& pss )noexcept;
		void Set_OutputMergerStage( const Pipeline::OutputMergerStage& oms )noexcept;
		void Set_ComputeShaderStage( const Pipeline::ShaderStage& css )noexcept;
	};
}
#endif