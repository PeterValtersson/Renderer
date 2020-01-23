#include "PipelineAssigner.h"
#include <Utilities/Profiler/Profiler.h>
#include <Utilities/MonadicOptional.h>
namespace Graphics
{
	template<class T>
	std::optional<std::reference_wrapper<const T>> find( const std::unordered_map<Utilities::GUID, PipelineObject, Utilities::GUID::Hasher>& m, Utilities::GUID ID )
	{
		if ( auto find = m.find( ID ); find != m.end() )
		{
			auto& r = find->second;
			return std::get<T>( r );
		}
		return std::nullopt;
	}
	template<class T, size_t N>
	bool has_changed( const T( &a1 )[N], const T( &a2 )[N] )
	{
		for ( size_t i = 0; i < N; ++i )
			if ( a1[i] != a2[i] )
				return true;
		return false;
	}
	PipelineAssigner::PipelineAssigner( ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context ) : PipelineHandler( device, context )
	{
		context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	
	}


	PipelineAssigner::~PipelineAssigner()
	{}
	void PipelineAssigner::Set_Pipeline( const Pipeline::Pipeline& pipeline ) noexcept
	{
		PROFILE;
		if ( current_pipeline.ID() == pipeline.ID() )
			return;

		Set_InputAssemblerStage( pipeline.IAStage() );
		Set_VertexShaderStage( pipeline.VSStage() );
		Set_GeometryShaderStage( pipeline.GSStage() );
		Set_StreamOutputStage( pipeline.SOStage() );
		Set_RasterizerStage( pipeline.RStage() );
		Set_PixelShaderStage( pipeline.PSStage() );
		Set_OutputMergerStage( pipeline.OMStage() );
		Set_ComputeShaderStage( pipeline.CSStage() );
		current_pipeline = pipeline;
	}
	void PipelineAssigner::Clear_Pipeline() noexcept
	{
		Set_Pipeline( {} );
	}
	void PipelineAssigner::Set_InputAssemblerStage( const Pipeline::InputAssemblerStage& pIA ) noexcept
	{
		PROFILE;

		auto& cIA = current_pipeline.IAStage();

		if ( pIA.topology != cIA.topology )
		{
			switch ( pIA.topology )
			{
			case Pipeline::PrimitiveTopology::LINE_LIST:		context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST ); break;
			case Pipeline::PrimitiveTopology::LINE_STRIP:		context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP ); break;
			case Pipeline::PrimitiveTopology::POINT_LIST:		context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST ); break;
			case Pipeline::PrimitiveTopology::TRIANGLE_LIST:	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ); break;
			case Pipeline::PrimitiveTopology::TRIANGLE_STRIP:	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP ); break;
			}
		}
		if ( pIA.indexBuffer != cIA.indexBuffer )
		{
			if ( auto fnd = find< PipelineObjects::Buffer_>( objects_RenderSide[PipelineObjects::Buffer], pIA.indexBuffer ); !fnd.has_value() )
				return; // Should add an error message to the log instead
			else
			{
				DXGI_FORMAT f = fnd->get().info.elementStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
				context->IASetIndexBuffer( fnd->get().obj.Get(), f, 0 );
			}

		}
		if ( pIA.vertexBuffer != cIA.vertexBuffer )
		{
			if ( auto fnd = find< PipelineObjects::Buffer_>( objects_RenderSide[PipelineObjects::Buffer], pIA.vertexBuffer ); !fnd.has_value() )
				return; // Should add an error message to the log instead
			else
			{
				uint32_t offset = 0;
				auto s = fnd->get().info.elementStride;
				context->IASetVertexBuffers( 0, 1, fnd->get().obj.GetAddressOf(), &s, &offset );
			}
		}
	}
	void PipelineAssigner::Set_VertexShaderStage( const Pipeline::ShaderStage& vss ) noexcept
	{
		PROFILE;
		auto& cVS = current_pipeline.VSStage();
		if ( cVS.GetID() == vss.GetID() )
			return;

		if ( auto fnd = find< PipelineObjects::VertexShader_>( objects_RenderSide[PipelineObjects::VertexShader], vss.shader ); !fnd.has_value() )
			return; // Should add an error message to the log instead_ASSERT_EXPR( vertexShaders.find( vss.shader ) != vertexShaders.end(), "Create vertex shader has not been called for vertex shader in pipeline." );
		else
		{
			context->VSSetShader( fnd->get().obj.Get(), nullptr, 0 );
			for ( auto& cb : fnd->get().constantBuffers )
				context->VSSetConstantBuffers( cb.binding, 1, cb.obj.GetAddressOf() );

			context->IASetInputLayout( fnd->get().obj2.Get() );
		}

	}
	void PipelineAssigner::Set_GeometryShaderStage( const Pipeline::ShaderStage& gss ) noexcept
	{
		PROFILE;
	}
	void PipelineAssigner::Set_StreamOutputStage( const Pipeline::StreamOutStage& sos ) noexcept
	{
		PROFILE;
	}
	void PipelineAssigner::Set_RasterizerStage( const Pipeline::RasterizerStage& rs ) noexcept
	{
		PROFILE;
		auto& cRS = current_pipeline.RStage();
		if ( cRS.GetID() == rs.GetID() )
			return;

		if ( auto fnd = find< PipelineObjects::RasterizerState_>( objects_RenderSide[PipelineObjects::RasterizerState], rs.rasterizerState ); !fnd.has_value() )
			return; // Should add an error message to the log instead
		else
			context->RSSetState( fnd->get().obj.Get() );

		if ( auto fnd = find< PipelineObjects::Viewport_>( objects_RenderSide[PipelineObjects::Viewport], rs.viewport ); !fnd.has_value() )
			return; // Should add an error message to the log instead
		else
			context->RSSetViewports( 1, &fnd->get().obj );
	}
	void PipelineAssigner::Set_PixelShaderStage( const Pipeline::ShaderStage& pss ) noexcept
	{
		PROFILE;
		auto& cPS = current_pipeline.VSStage();

		if ( cPS.shader != pss.shader )
		{
			if ( auto fnd = find< PipelineObjects::PixelShader_>( objects_RenderSide[PipelineObjects::PixelShader], pss.shader ); !fnd.has_value() )
				return; // Should add an error message to the log instead_ASSERT_EXPR( vertexShaders.find( vss.shader ) != vertexShaders.end(), "Create pixel shader has not been called for pixel shader in pipeline." );
			else
			{
				context->PSSetShader( fnd->get().obj.Get(), nullptr, 0 );
				for ( auto& cb : fnd->get().constantBuffers )
					context->PSSetConstantBuffers( cb.binding, 1, cb.obj.GetAddressOf() );
			}

		}


		if ( pss.textureCount != cPS.textureCount || has_changed( pss.textures, cPS.textures ) || has_changed( pss.textureBindings, cPS.textureBindings ) )
		{
			for ( int i = 0; i < pss.textureCount; ++i )
			{
				if ( auto fnd = find< PipelineObjects::PixelShader_>( objects_RenderSide[PipelineObjects::PixelShader], pss.shader ); !fnd.has_value() )
					return; // Should add an error message to the log instead_ASSERT_EXPR( vertexShaders.find( vss.shader ) != vertexShaders.end(), "Create pixel shader has not been called for pixel shader in pipeline." );
				else
				{
					if ( auto fnd_srv = find< PipelineObjects::ShaderResourceView_>( objects_RenderSide[PipelineObjects::ShaderResourceView], pss.textures[i] ); !fnd_srv.has_value() )
						return; // Should add an error message to the log instead_ASSERT_EXPR( vertexShaders.find( vss.shader ) != vertexShaders.end(), "Create pixel shader has not been called for pixel shader in pipeline." );
					else
					{
						auto binding_ID = pss.shader + pss.textureBindings[i];
						for ( auto& binding : fnd->get().textures ) // Add error to log if binding did not match any.
							if ( binding.id == binding_ID )
								context->PSSetShaderResources( binding.binding, 1, fnd_srv->get().obj.GetAddressOf() );
					}
				}
			}
		}



		ID3D11SamplerState* samplers[Pipeline::ShaderStage::maxSamplers] = { nullptr };
		bool samplerChanged = false;
		for ( int i = 0; i < pss.samplerCount; ++i )
		{
			if ( pss.samplers[i] != cPS.samplers[i] )
			{
				if ( auto fnd = find< PipelineObjects::SamplerState_>( objects_RenderSide[PipelineObjects::SamplerState], pss.samplers[i] ); !fnd.has_value() )
					return; // Should add an error message to the log instead_ASSERT_EXPR( vertexShaders.find( vss.shader ) != vertexShaders.end(), "Create pixel shader has not been called for pixel shader in pipeline." );
				else
				{
					samplers[i] = fnd->get().obj.Get();
					samplerChanged = true;
				}
			}
		}
		if ( samplerChanged )
			context->PSSetSamplers( 0, pss.samplerCount, samplers );
	}
	void PipelineAssigner::Set_OutputMergerStage( const Pipeline::OutputMergerStage& oms ) noexcept
	{
		PROFILE;

		auto& cOMS = current_pipeline.OMStage();


		if ( oms.renderTargetCount != cOMS.renderTargetCount || has_changed( oms.renderTargets, cOMS.renderTargets ) || oms.depthStencilView != cOMS.depthStencilView )
		{

			ID3D11RenderTargetView* renderTargets[Pipeline::OutputMergerStage::maxRenderTargets] = { nullptr };
			for ( int i = 0; i < oms.renderTargetCount; ++i )
			{
				if ( auto fnd = find< PipelineObjects::RenderTarget_>( objects_RenderSide[PipelineObjects::RenderTarget], oms.renderTargets[i] ); !fnd.has_value() )
					return; // Should add an error message to the log instead
				else
				{
					renderTargets[i] = fnd->get().obj.Get();
					if ( oms.clearTargets )
						context->ClearRenderTargetView( renderTargets[i], fnd->get().clearColor );
				}

			}
			ID3D11DepthStencilView* depthview = nullptr;
			if ( auto fnd = find< PipelineObjects::DepthStencilView_>( objects_RenderSide[PipelineObjects::DepthStencilView], oms.depthStencilView ); !fnd.has_value() )
				return; // Should add an error message to the log instead
			else
			{
				depthview = fnd->get().obj.Get();
				if ( oms.clearDepthStencilView )
					context->ClearDepthStencilView( depthview, D3D11_CLEAR_DEPTH, 1.0f, 0 );
			}

			context->OMSetRenderTargets( oms.maxRenderTargets, renderTargets, depthview );
		}
		else
		{
			if ( oms.clearTargets )
				for ( size_t i = 0; i < oms.renderTargetCount; i++ )
					if ( auto fnd = find< PipelineObjects::RenderTarget_>( objects_RenderSide[PipelineObjects::RenderTarget], oms.renderTargets[i] ); !fnd.has_value() )
						return; // Should add an error message to the log instead
					else
						context->ClearRenderTargetView( fnd->get().obj.Get(), fnd->get().clearColor );
			if ( oms.clearDepthStencilView )
				if ( auto fnd = find< PipelineObjects::DepthStencilView_>( objects_RenderSide[PipelineObjects::DepthStencilView], oms.depthStencilView ); !fnd.has_value() )
					return; // Should add an error message to the log instead
				else
					context->ClearDepthStencilView( fnd->get().obj.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );
		}
		if ( oms.blendState != cOMS.blendState )
		{
			if ( auto fnd = find< PipelineObjects::BlendState_>( objects_RenderSide[PipelineObjects::BlendState], oms.blendState ); !fnd.has_value() )
				return; // Should add an error message to the log instead
			else
			{
				float blend[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
				context->OMSetBlendState( fnd->get().obj.Get(), blend, 0xFF );
			}
		}

		if ( oms.depthStencilState != cOMS.depthStencilState )
		{
			if ( auto fnd = find< PipelineObjects::DepthStencilState_>( objects_RenderSide[PipelineObjects::DepthStencilState], oms.blendState ); !fnd.has_value() )
				return; // Should add an error message to the log instead
			else
			{
				context->OMSetDepthStencilState( fnd->get().obj.Get(), 0 );
			}
		}
	}
	void PipelineAssigner::Set_ComputeShaderStage( const Pipeline::ShaderStage& css ) noexcept
	{
		PROFILE;
	}
}