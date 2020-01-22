#include "PipelineAssigner.h"


namespace Graphics
{
	PipelineAssigner::PipelineAssigner( ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context ) : PipelineHandler( device, context )
	{}


	PipelineAssigner::~PipelineAssigner()
	{}
	void PipelineAssigner::Set_Pipeline( const Pipeline::Pipeline & pipeline ) noexcept
	{}
	void PipelineAssigner::Clear_Pipeline() noexcept
	{}
	void PipelineAssigner::Set_InputAssemblerStage( const Pipeline::InputAssemblerStage & pIA ) noexcept
	{}
	void PipelineAssigner::Set_VertexShaderStage( const Pipeline::ShaderStage & vss ) noexcept
	{}
	void PipelineAssigner::Set_GeometryShaderStage( const Pipeline::ShaderStage & gss ) noexcept
	{}
	void PipelineAssigner::Set_RasterizerStage( const Pipeline::RasterizerStage & rs ) noexcept
	{}
	void PipelineAssigner::Set_PixelShaderStage( const Pipeline::ShaderStage & pss ) noexcept
	{}
	void PipelineAssigner::Set_OutputMergerStage( const Pipeline::OutputMergerStage & oms ) noexcept
	{}
	void PipelineAssigner::Set_ComputeShaderStage( const Pipeline::ShaderStage & css ) noexcept
	{}
}