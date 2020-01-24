#include "PipelineHandler.h"
#include <Utilities/Profiler/Profiler.h>
#include "UpdateObjects.h"
#include <d3dcompiler.h>

#pragma comment(lib, "D3Dcompiler.lib")
#pragma comment(lib, "dxguid.lib")


#define EMPLACE_NULL(name) objects_RenderSide[PipelineObjects::##name].emplace(Utilities::GUID(), PipelineObjects::##name##_{ nullptr })
#define EMPLACE_DEF(name) objects_RenderSide[PipelineObjects::##name].emplace(Utilities::GUID(), PipelineObjects::##name##_{ })


static const char* fullscreenQuadVS =
"void VS_main( uint VertexID : SV_VertexID, out float4 oPosH : SV_POSITION, out float2 oTexC : TEXCOORD ) \
{ \
	oPosH.x = (VertexID == 2) ? 3.0f : -1.0f; \
	oPosH.y = (VertexID == 0) ? -3.0f : 1.0f; \
	oPosH.zw = 1.0f; \
 \
	oTexC = oPosH.xy * float2(0.5f, -0.5f) + 0.5f; \
}";

static const char* MultiPS =
"Texture2D gTexture : register(t0); \
SamplerState gTriLinearSam : register(s0); \
float4 PS_main(float4 posH : SV_POSITION, float2 texC : TEXCOORD) : SV_TARGET \
{ \
	return gTexture.Sample(gTriLinearSam, texC).rgba; \
}";

static const char* SinglePS =
"Texture2D gTexture : register(t0); \
SamplerState gTriLinearSam : register(s0); \
float4 PS_main(float4 posH : SV_POSITION, float2 texC : TEXCOORD) : SV_TARGET \
{ \
	return gTexture.Sample(gTriLinearSam, texC).rrrr; \
}";



namespace Graphics
{
	PipelineHandler::PipelineHandler( ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context ) : device( device ), context( context )
	{
		/*objects_RenderSide[PipelineObjects::RenderTarget].emplace( Default_RenderTarget, PipelineObjects::RenderTarget_{ backbuffer,{ 0.0f, 0.0f,1.0f,0.0f } } );
		objects_RenderSide[PipelineObjects::DepthStencilView].emplace( Default_DepthStencil, PipelineObjects::DepthStencilView_{ dsv } );
		objects_RenderSide[PipelineObjects::ShaderResourceView].emplace( Default_RenderTarget, PipelineObjects::ShaderResourceView_{ bbsrv } );
		objects_RenderSide[PipelineObjects::ShaderResourceView].emplace( Default_DepthStencil, PipelineObjects::ShaderResourceView_{ dsvsrv } );
		objects_RenderSide[PipelineObjects::Viewport].emplace( Default_Viewport, PipelineObjects::Viewport_{ vp } );*/

		//objects_ClientSide[PipelineObjects::RenderTarget].emplace( Default_RenderTarget );
		//objects_ClientSide[PipelineObjects::DepthStencilView].emplace( Default_DepthStencil );
		//objects_ClientSide[PipelineObjects::ShaderResourceView].emplace( Default_RenderTarget );
		//objects_ClientSide[PipelineObjects::ShaderResourceView].emplace( Default_DepthStencil );
		//objects_ClientSide[PipelineObjects::Viewport].emplace( Default_Viewport );

		EMPLACE_NULL( PipelineObjects::Buffer );

		EMPLACE_NULL( PipelineObjects::VertexShader );
		EMPLACE_NULL( PipelineObjects::GeometryShader );
		EMPLACE_NULL( PipelineObjects::PixelShader );
		EMPLACE_NULL( PipelineObjects::ComputeShader );

		EMPLACE_NULL( PipelineObjects::RenderTarget );
		EMPLACE_NULL( PipelineObjects::UnorderedAccessView );
		EMPLACE_NULL( PipelineObjects::ShaderResourceView );
		EMPLACE_NULL( PipelineObjects::DepthStencilView );

		EMPLACE_NULL( PipelineObjects::SamplerState );
		EMPLACE_NULL( PipelineObjects::BlendState );
		EMPLACE_NULL( PipelineObjects::RasterizerState );
		EMPLACE_NULL( PipelineObjects::DepthStencilState );

		EMPLACE_DEF( PipelineObjects::Viewport );

		CreateShader( Default_VertexShader_FullscreenQUAD, Pipeline::ShaderType::VERTEX, fullscreenQuadVS, strlen( fullscreenQuadVS ), "VS_main", "vs_5_0" );
		CreateShader( Default_PixelShader_POS_TEXTURE_MultiChannel, Pipeline::ShaderType::PIXEL, MultiPS, strlen( MultiPS ), "PS_main", "ps_5_0" );
		CreateShader( Default_PixelShader_POS_TEXTURE_SingleChannel, Pipeline::ShaderType::PIXEL, SinglePS, strlen( SinglePS ), "PS_main", "ps_5_0" );
	}


#define RELEASE_PLO(type) if(o.second.index() == type) std::get<type##_>(o.second).Release();
	PipelineHandler::~PipelineHandler()noexcept
	{
		objects_RenderSide[PipelineObjects::RenderTarget].erase( Default_RenderTarget );
		objects_RenderSide[PipelineObjects::DepthStencilView].erase( Default_DepthStencil );
		objects_RenderSide[PipelineObjects::ShaderResourceView].erase( Default_RenderTarget );
		objects_RenderSide[PipelineObjects::ShaderResourceView].erase( Default_DepthStencil );
	}


	void PipelineHandler::CreateBuffer( Utilities::GUID id, const Pipeline::Buffer& buffer )
	{
		auto pBuffer = _CreateBuffer( id, buffer );
		toAdd.push( { id, PipelineObjects::Buffer_{ pBuffer, buffer } } );

		objects_ClientSide[PipelineObjects::Buffer].emplace( id );
	}

	void PipelineHandler::DestroyBuffer( Utilities::GUID id )noexcept
	{
		PROFILE;
		if ( auto find = objects_ClientSide[PipelineObjects::Buffer].find( id ); find != objects_ClientSide[PipelineObjects::Buffer].end() )
		{
			objects_ClientSide[PipelineObjects::Buffer].erase( id );
			toRemove.push( { id,PipelineObjects::Buffer } );
		}
	}

	void PipelineHandler::CreateViewport( Utilities::GUID id, const Pipeline::Viewport& viewport )
	{
		PROFILE;
		if ( auto find = objects_ClientSide[PipelineObjects::Viewport].find( id ); find != objects_ClientSide[PipelineObjects::Viewport].end() )
			throw Could_Not_Create_Viewport( "Viewport with ID already exists", id, viewport );


		objects_ClientSide[PipelineObjects::Viewport].emplace( id );

		toAdd.push( { id , PipelineObjects::Viewport_{ viewport.topLeftX, viewport.topLeftY, viewport.width, viewport.height, viewport.minDepth, viewport.maxDepth } } );
	}
	void PipelineHandler::CreateShader( Utilities::GUID id, Pipeline::ShaderType type, const char* sourceCode, size_t size, const char* entryPoint, const char* shaderModel )
	{
		PROFILE;
		ComPtr<ID3DBlob> blob;
		ComPtr<ID3DBlob> error;
		if ( auto hr = D3DCompile( sourceCode, size, NULL, NULL, NULL, entryPoint, shaderModel, 0, 0, &blob, &error ); FAILED( hr ) )
			throw Could_Not_Create_Shader( "Could not compile shader. \n" + std::string( ( char* )error.Get() ), id, type, hr );
		return CreateShader( id, type, blob->GetBufferPointer(), blob->GetBufferSize() );
	}

	void PipelineHandler::CreateShader( Utilities::GUID id, Pipeline::ShaderType type, const void* data, size_t size )
	{
		PROFILE;
		switch ( type )
		{
		case Graphics::Pipeline::ShaderType::VERTEX:
			if ( auto find = objects_ClientSide[PipelineObjects::VertexShader].find( id ); find != objects_ClientSide[PipelineObjects::VertexShader].end() )
				throw Could_Not_Create_Shader( "Vertex shader with id already exists", id, type );
			break;
		case Graphics::Pipeline::ShaderType::GEOMETRY:
		case Graphics::Pipeline::ShaderType::GEOMETRY_STREAM_OUT:
			if ( auto find = objects_ClientSide[PipelineObjects::GeometryShader].find( id ); find != objects_ClientSide[PipelineObjects::GeometryShader].end() )
				throw Could_Not_Create_Shader( "Geometry shader with id already exists", id, type );
			break;
		case Graphics::Pipeline::ShaderType::PIXEL:
			if ( auto find = objects_ClientSide[PipelineObjects::PixelShader].find( id ); find != objects_ClientSide[PipelineObjects::PixelShader].end() )
				throw Could_Not_Create_Shader( "Pixel shader with id already exists", id, type );
			break;
		case Graphics::Pipeline::ShaderType::COMPUTE:
			if ( auto find = objects_ClientSide[PipelineObjects::ComputeShader].find( id ); find != objects_ClientSide[PipelineObjects::ComputeShader].end() )
				throw Could_Not_Create_Shader( "Compute shader with id already exists", id, type );
			break;
		default:
			throw Could_Not_Create_Shader( "Shader type not supported", id, type );
			break;
		}

		//Create the input layout with the help of shader reflection
		ComPtr<ID3D11ShaderReflection> reflection;

		if ( auto hr = D3DReflect( data, size, IID_ID3D11ShaderReflection, &reflection ); FAILED( hr ) )
			throw Could_Not_Create_Shader( "D3DReflect failed", id, type, hr );

		D3D11_SHADER_DESC shaderDesc;
		reflection->GetDesc( &shaderDesc );
		std::vector<ShaderResourceToAndBindSlotAndObject> cbuffers;
		std::vector<ShaderResourceToAndBindSlot> textures;
		for ( unsigned int i = 0; i < shaderDesc.BoundResources; ++i )
		{
			D3D11_SHADER_INPUT_BIND_DESC sibd;
			reflection->GetResourceBindingDesc( i, &sibd );
			if ( sibd.Type == D3D_SIT_CBUFFER )
			{
				//Can't get the size from the RBD, can't get bindslot from the SBD...	
				//Find the sbd with the same name to get the size.
				Utilities::GUID sibdName = std::string_view( sibd.Name );
				for ( unsigned int j = 0; j < shaderDesc.ConstantBuffers; ++j )
				{
					D3D11_SHADER_BUFFER_DESC sbd;
					ID3D11ShaderReflectionConstantBuffer* srcb = reflection->GetConstantBufferByIndex( j );
					srcb->GetDesc( &sbd );
					Utilities::GUID name = std::string_view( sbd.Name );
					if ( name == sibdName )
					{
						auto buffer = Pipeline::Buffer::ConstantBuffer( uint16_t( sbd.Size ) );
						auto pBuffer = _CreateBuffer( name, buffer );
						toAdd.push( { name, PipelineObjects::Buffer_{ pBuffer, buffer } } );
						objects_ClientSide[PipelineObjects::Buffer].emplace( name );

						cbuffers.push_back( { name,  pBuffer, sibd.BindPoint } );

						break;
					}
				}
			}
			else if ( sibd.Type == D3D_SIT_TEXTURE )
			{
				const Utilities::GUID bindGuid( sibd.Name );
				const Utilities::GUID combinedGuid = id + bindGuid;
				textures.push_back( { combinedGuid, sibd.BindPoint } );
			}
		}
		switch ( type )
		{
		case Graphics::Pipeline::ShaderType::VERTEX:
		{
			ComPtr<ID3D11InputLayout> inputLayout;
			ComPtr<ID3D11VertexShader> vs;


			if ( auto hr = device->CreateVertexShader( data, size, nullptr, &vs ); FAILED( hr ) )
				throw Could_Not_Create_Shader( "CreateVertexShader failed", id, type, hr );


			std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs;
			uint32_t offset = 0;
			for ( uint32_t i = 0; i < shaderDesc.InputParameters; ++i )
			{
				D3D11_SIGNATURE_PARAMETER_DESC signatureParamaterDesc;
				reflection->GetInputParameterDesc( i, &signatureParamaterDesc );
				D3D11_INPUT_ELEMENT_DESC inputElementDesc;
				inputElementDesc.SemanticName = signatureParamaterDesc.SemanticName;
				inputElementDesc.SemanticIndex = signatureParamaterDesc.SemanticIndex;
				inputElementDesc.AlignedByteOffset = offset;
				inputElementDesc.InputSlot = 0;
				inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				inputElementDesc.InstanceDataStepRate = 0;

				if ( signatureParamaterDesc.Mask == 1 )
				{
					const std::string semName( inputElementDesc.SemanticName );
					if ( semName == "SV_InstanceID" )
						continue;
					if ( semName == "SV_VertexID" )
						continue;
					if ( signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 )
						inputElementDesc.Format = DXGI_FORMAT_R32_FLOAT;
					else if ( signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 )
						inputElementDesc.Format = DXGI_FORMAT_R32_SINT;
					else if ( signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 )
						inputElementDesc.Format = DXGI_FORMAT_R32_UINT;
					offset += 4;

				}
				else if ( signatureParamaterDesc.Mask <= 3 )
				{
					if ( signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 )
						inputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
					else if ( signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 )
						inputElementDesc.Format = DXGI_FORMAT_R32G32_SINT;
					else if ( signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 )
						inputElementDesc.Format = DXGI_FORMAT_R32G32_UINT;
					offset += 8;
				}
				else if ( signatureParamaterDesc.Mask <= 7 )
				{
					if ( signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 )
						inputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					else if ( signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 )
						inputElementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
					else if ( signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 )
						inputElementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
					offset += 12;
				}
				else if ( signatureParamaterDesc.Mask <= 15 )
				{
					if ( signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 )
						inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					else if ( signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 )
						inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
					else if ( signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 )
						inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
					offset += 16;
				}
				inputElementDescs.push_back( inputElementDesc );
			}
			if ( inputElementDescs.size() > 0 )
				if ( auto hr = device->CreateInputLayout( inputElementDescs.data(), UINT( inputElementDescs.size() ), data, size, &inputLayout ); FAILED( hr ) )
					throw  Could_Not_Create_Shader( "CreateInputLayout failed", id, type, hr );

			toAdd.push( { id, PipelineObjects::VertexShader_{vs, inputLayout, cbuffers} } );
			objects_ClientSide[PipelineObjects::VertexShader].emplace( id );
			break;
		}
		case Graphics::Pipeline::ShaderType::GEOMETRY:
		{

			ComPtr<ID3D11GeometryShader> s;
			if ( auto hr = device->CreateGeometryShader( data, size, nullptr, &s ); FAILED( hr ) )
				throw Could_Not_Create_Shader( "CreateGeometryShader failed", id, type, hr );

			toAdd.push( { id, PipelineObjects::GeometryShader_{ s, cbuffers } } );
			objects_ClientSide[PipelineObjects::GeometryShader].emplace( id );
			break;
		}
		case Graphics::Pipeline::ShaderType::GEOMETRY_STREAM_OUT:
		{
			std::vector<D3D11_SO_DECLARATION_ENTRY> SOEntries;
			for ( UINT i = 0; i < shaderDesc.InputParameters; ++i )
			{
				D3D11_SIGNATURE_PARAMETER_DESC signatureParameterDesc;
				reflection->GetInputParameterDesc( i, &signatureParameterDesc );
				BYTE mask = signatureParameterDesc.Mask;
				int varCount = 0;
				while ( mask )
				{
					if ( mask & 0x01 ) varCount++;
					mask = mask >> 1;
				}

				D3D11_SO_DECLARATION_ENTRY sode;
				sode.SemanticName = signatureParameterDesc.SemanticName;
				sode.Stream = signatureParameterDesc.Stream;
				sode.OutputSlot = 0;
				sode.StartComponent = 0;
				sode.ComponentCount = varCount;
				sode.SemanticIndex = signatureParameterDesc.SemanticIndex;

				SOEntries.push_back( sode );
			}
			uint32_t bufferStrides = 0;
			for ( auto& e : SOEntries )
				bufferStrides += e.ComponentCount * 4;

			ComPtr<ID3D11GeometryShader> s;
			if ( auto hr = device->CreateGeometryShaderWithStreamOutput( data, size, SOEntries.data(), UINT( SOEntries.size() ), &bufferStrides, 1, D3D11_SO_NO_RASTERIZED_STREAM, nullptr, &s ); FAILED( hr ) )
				throw Could_Not_Create_Shader( "CreateGeometryShaderWithStreamOutput failed", id, type, hr );

			toAdd.push( { id, PipelineObjects::GeometryShader_{ s, cbuffers } } );
			objects_ClientSide[PipelineObjects::GeometryShader].emplace( id );
			break;
		}
		case Graphics::Pipeline::ShaderType::PIXEL:
		{
			ComPtr<ID3D11PixelShader> s;
			if ( auto hr = device->CreatePixelShader( data, size, nullptr, &s ); FAILED( hr ) )
				throw Could_Not_Create_Shader( "CreatePixelShader failed", id, type, hr );

			toAdd.push( { id, PipelineObjects::PixelShader_{ s, cbuffers, textures } } );
			objects_ClientSide[PipelineObjects::PixelShader].emplace( id );
			break;
		}
		case Graphics::Pipeline::ShaderType::COMPUTE:
		{
			ComPtr<ID3D11ComputeShader> s;
			if ( auto hr = device->CreateComputeShader( data, size, nullptr, &s ); FAILED( hr ) )
				throw Could_Not_Create_Shader( "Could not create compute shader", id, type, hr );

			toAdd.push( { id, PipelineObjects::ComputeShader_{ s, cbuffers } } );
			objects_ClientSide[PipelineObjects::ComputeShader].emplace( id );
			break;
		}
		}

	}

	void PipelineHandler::DestroyShader( Utilities::GUID id, Pipeline::ShaderType type )noexcept
	{
		PROFILE;
		uint32_t t = -1;
		if ( type == Pipeline::ShaderType::VERTEX )
			t = PipelineObjects::VertexShader;
		else if ( type == Pipeline::ShaderType::PIXEL )
			t = PipelineObjects::PixelShader;
		else if ( type == Pipeline::ShaderType::GEOMETRY || type == Pipeline::ShaderType::GEOMETRY_STREAM_OUT )
			t = PipelineObjects::GeometryShader;
		else if ( type == Pipeline::ShaderType::COMPUTE )
			t = PipelineObjects::ComputeShader;

		if ( auto find = objects_ClientSide[t].find( id ); find != objects_ClientSide[t].end() )
		{
			objects_ClientSide[t].erase( id );
			toRemove.push( { id, t } );
		}
	}
	/*void PipelineHandler::CreateTexture( Utilities::GUID id, void* data, size_t width, size_t height )
	{
		PROFILE;
		if ( auto find = objects_ClientSide[PipelineObjects::ShaderResourceView].find( id ); find != objects_ClientSide[PipelineObjects::ShaderResourceView].end() )
			throw Could_Not_Create_Texture( "Texture already exists", id );

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		SafeDXP<ID3D11Texture2D> texture;
		D3D11_SUBRESOURCE_DATA d;
		d.pSysMem = data;
		d.SysMemPitch = UINT( width * 4 );
		d.SysMemSlicePitch = 0;
		if ( auto hr = device->CreateTexture2D( &desc, &d, texture.Create() ); FAILED( hr ) )
			throw Could_Not_Create_Texture( "CreateTexture2D Failed", id );


		SafeDXP<ID3D11ShaderResourceView> srv;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		if ( auto hr = device->CreateShaderResourceView( texture.Get(), &srvDesc, srv.Create() ); FAILED( hr ) )
			throw Could_Not_Create_Texture( "Could not create shader resource view", id );

		objects_ClientSide[PipelineObjects::ShaderResourceView].emplace( id );
		toAdd.push( { id, PipelineObjects::ShaderResourceView_{srv.Done()} } );
	}
	void PipelineHandler::DestroyTexture( Utilities::GUID id )noexcept
	{
		PROFILE;
		if ( auto find = objects_ClientSide[PipelineObjects::ShaderResourceView].find( id ); find != objects_ClientSide[PipelineObjects::ShaderResourceView].end() )
		{
			objects_ClientSide[PipelineObjects::ShaderResourceView].erase( id );
			toRemove.push( { id, PipelineObjects::ShaderResourceView } );
		}
	}*/
	void PipelineHandler::CreateRasterizerState( Utilities::GUID id, const Pipeline::RasterizerState& state )
	{
		PROFILE;
		if ( auto find = objects_ClientSide[PipelineObjects::RasterizerState].find( id ); find != objects_ClientSide[PipelineObjects::RasterizerState].end() )
			throw Could_Not_Create_RasterizerState( "Rasterizer already exists", id, state );

		D3D11_RASTERIZER_DESC rd;
		rd.AntialiasedLineEnable = false;
		switch ( state.cullMode )
		{
		case Pipeline::CullMode::CULL_BACK: rd.CullMode = D3D11_CULL_BACK; break;
		case Pipeline::CullMode::CULL_FRONT: rd.CullMode = D3D11_CULL_FRONT; break;
		case Pipeline::CullMode::CULL_NONE: rd.CullMode = D3D11_CULL_NONE; break;
		}
		switch ( state.fillMode )
		{
		case Pipeline::FillMode::FILL_SOLID:		rd.FillMode = D3D11_FILL_SOLID; break;
		case Pipeline::FillMode::FILL_WIREFRAME:  rd.FillMode = D3D11_FILL_WIREFRAME; break;
		}
		switch ( state.windingOrder )
		{
		case Pipeline::WindingOrder::CLOCKWISE:		 rd.FrontCounterClockwise = false; break;
		case Pipeline::WindingOrder::COUNTERCLOCKWISE: rd.FrontCounterClockwise = true; break;
		}
		rd.DepthBias = false;
		rd.DepthClipEnable = true;
		rd.DepthBiasClamp = 0;
		rd.MultisampleEnable = false;
		rd.ScissorEnable = false;
		rd.SlopeScaledDepthBias = 0;

		ComPtr<ID3D11RasterizerState> rs;
		if ( auto hr = device->CreateRasterizerState( &rd, &rs ); FAILED( hr ) )
			throw Could_Not_Create_RasterizerState( "CreateRasterizerState failed", id, state, hr );

		objects_ClientSide[PipelineObjects::RasterizerState].emplace( id );
		toAdd.push( { id, PipelineObjects::RasterizerState_{rs} } );
	}

	void PipelineHandler::DestroyRasterizerState( Utilities::GUID id )noexcept
	{
		PROFILE;
		if ( auto find = objects_ClientSide[PipelineObjects::RasterizerState].find( id ); find != objects_ClientSide[PipelineObjects::RasterizerState].end() )
		{
			objects_ClientSide[PipelineObjects::RasterizerState].erase( id );
			toRemove.push( { id, PipelineObjects::RasterizerState } );
		}
	}
	void PipelineHandler::CreateBlendState( Utilities::GUID id, const Pipeline::BlendState& state )
	{
		PROFILE;
		if ( auto find = objects_ClientSide[PipelineObjects::BlendState].find( id ); find != objects_ClientSide[PipelineObjects::BlendState].end() )
			throw Could_Not_Create_BlendState( "BlendState already exists", id, state );


		D3D11_BLEND_DESC bd;
		bd.IndependentBlendEnable = false;

		D3D11_RENDER_TARGET_BLEND_DESC rtbd[8];
		rtbd[0].BlendEnable = state.enable;
		switch ( state.blendOperation )
		{
		case Pipeline::BlendOperation::ADD: rtbd[0].BlendOp = D3D11_BLEND_OP_ADD; break;
		case Pipeline::BlendOperation::MAX: rtbd[0].BlendOp = D3D11_BLEND_OP_MAX; break;
		case Pipeline::BlendOperation::MIN: rtbd[0].BlendOp = D3D11_BLEND_OP_MIN; break;
		case Pipeline::BlendOperation::SUB: rtbd[0].BlendOp = D3D11_BLEND_OP_SUBTRACT; break;
		}

		switch ( state.blendOperationAlpha )
		{
		case Pipeline::BlendOperation::ADD: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; break;
		case Pipeline::BlendOperation::MAX: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_MAX; break;
		case Pipeline::BlendOperation::MIN: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_MIN; break;
		case Pipeline::BlendOperation::SUB: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_SUBTRACT; break;
		}

		switch ( state.dstBlend )
		{
		case Pipeline::Blend::BLEND_FACTOR:	rtbd[0].DestBlend = D3D11_BLEND_BLEND_FACTOR; break;
		case Pipeline::Blend::DEST_ALPHA:		rtbd[0].DestBlend = D3D11_BLEND_DEST_ALPHA; break;
		case Pipeline::Blend::DEST_COLOR:		rtbd[0].DestBlend = D3D11_BLEND_DEST_COLOR; break;
		case Pipeline::Blend::INV_DEST_ALPHA:	rtbd[0].DestBlend = D3D11_BLEND_INV_DEST_ALPHA; break;
		case Pipeline::Blend::INV_DEST_COLOR:	rtbd[0].DestBlend = D3D11_BLEND_INV_DEST_COLOR; break;
		case Pipeline::Blend::INV_SRC_ALPHA:	rtbd[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; break;
		case Pipeline::Blend::INV_SRC_COLOR:	rtbd[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR; break;
		case Pipeline::Blend::ONE:			rtbd[0].DestBlend = D3D11_BLEND_ONE; break;
		case Pipeline::Blend::SRC_ALPHA:		rtbd[0].DestBlend = D3D11_BLEND_SRC_ALPHA; break;
		case Pipeline::Blend::SRC_COLOR:		rtbd[0].DestBlend = D3D11_BLEND_SRC_COLOR; break;
		case Pipeline::Blend::ZERO:			rtbd[0].DestBlend = D3D11_BLEND_ZERO; break;
		}

		switch ( state.srcBlend )
		{
		case Pipeline::Blend::BLEND_FACTOR:	rtbd[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR; break;
		case Pipeline::Blend::DEST_ALPHA:		rtbd[0].SrcBlend = D3D11_BLEND_DEST_ALPHA; break;
		case Pipeline::Blend::DEST_COLOR:		rtbd[0].SrcBlend = D3D11_BLEND_DEST_COLOR; break;
		case Pipeline::Blend::INV_DEST_ALPHA:	rtbd[0].SrcBlend = D3D11_BLEND_INV_DEST_ALPHA; break;
		case Pipeline::Blend::INV_DEST_COLOR:	rtbd[0].SrcBlend = D3D11_BLEND_INV_DEST_COLOR; break;
		case Pipeline::Blend::INV_SRC_ALPHA:	rtbd[0].SrcBlend = D3D11_BLEND_INV_SRC_ALPHA; break;
		case Pipeline::Blend::INV_SRC_COLOR:	rtbd[0].SrcBlend = D3D11_BLEND_INV_SRC_COLOR; break;
		case Pipeline::Blend::ONE:			rtbd[0].SrcBlend = D3D11_BLEND_ONE; break;
		case Pipeline::Blend::SRC_ALPHA:		rtbd[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; break;
		case Pipeline::Blend::SRC_COLOR:		rtbd[0].SrcBlend = D3D11_BLEND_SRC_COLOR; break;
		case Pipeline::Blend::ZERO:			rtbd[0].SrcBlend = D3D11_BLEND_ZERO; break;
		}

		switch ( state.dstBlendAlpha )
		{
		case Pipeline::Blend::BLEND_FACTOR:	rtbd[0].DestBlendAlpha = D3D11_BLEND_BLEND_FACTOR; break;
		case Pipeline::Blend::DEST_ALPHA:		rtbd[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA; break;
		case Pipeline::Blend::DEST_COLOR:		rtbd[0].DestBlendAlpha = D3D11_BLEND_DEST_COLOR; break;
		case Pipeline::Blend::INV_DEST_ALPHA:	rtbd[0].DestBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA; break;
		case Pipeline::Blend::INV_DEST_COLOR:	rtbd[0].DestBlendAlpha = D3D11_BLEND_INV_DEST_COLOR; break;
		case Pipeline::Blend::INV_SRC_ALPHA:	rtbd[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA; break;
		case Pipeline::Blend::INV_SRC_COLOR:	rtbd[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_COLOR; break;
		case Pipeline::Blend::ONE:			rtbd[0].DestBlendAlpha = D3D11_BLEND_ONE; break;
		case Pipeline::Blend::SRC_ALPHA:		rtbd[0].DestBlendAlpha = D3D11_BLEND_SRC_ALPHA; break;
		case Pipeline::Blend::SRC_COLOR:		rtbd[0].DestBlendAlpha = D3D11_BLEND_SRC_COLOR; break;
		case Pipeline::Blend::ZERO:			rtbd[0].DestBlendAlpha = D3D11_BLEND_ZERO; break;
		}

		switch ( state.srcBlendAlpha )
		{
		case Pipeline::Blend::BLEND_FACTOR:	rtbd[0].SrcBlendAlpha = D3D11_BLEND_BLEND_FACTOR; break;
		case Pipeline::Blend::DEST_ALPHA:		rtbd[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA; break;
		case Pipeline::Blend::DEST_COLOR:		rtbd[0].SrcBlendAlpha = D3D11_BLEND_DEST_COLOR; break;
		case Pipeline::Blend::INV_DEST_ALPHA:	rtbd[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA; break;
		case Pipeline::Blend::INV_DEST_COLOR:	rtbd[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_COLOR; break;
		case Pipeline::Blend::INV_SRC_ALPHA:	rtbd[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA; break;
		case Pipeline::Blend::INV_SRC_COLOR:	rtbd[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_COLOR; break;
		case Pipeline::Blend::ONE:			rtbd[0].SrcBlendAlpha = D3D11_BLEND_ONE; break;
		case Pipeline::Blend::SRC_ALPHA:		rtbd[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA; break;
		case Pipeline::Blend::SRC_COLOR:		rtbd[0].SrcBlendAlpha = D3D11_BLEND_SRC_COLOR; break;
		case Pipeline::Blend::ZERO:			rtbd[0].SrcBlendAlpha = D3D11_BLEND_ZERO; break;
		}
		rtbd[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		bd.AlphaToCoverageEnable = false;
		bd.RenderTarget[0] = rtbd[0];

		ComPtr<ID3D11BlendState> blendState;
		if ( auto hr = device->CreateBlendState( &bd, &blendState ); FAILED( hr ) )
			throw Could_Not_Create_BlendState( "CreateBlendState failed", id, state, hr );


		objects_ClientSide[PipelineObjects::BlendState].emplace( id );
		toAdd.push( { id, PipelineObjects::BlendState_{ blendState } } );
	}
	void PipelineHandler::DestroyBlendState( Utilities::GUID id )noexcept
	{
		PROFILE;
		if ( auto find = objects_ClientSide[PipelineObjects::BlendState].find( id ); find != objects_ClientSide[PipelineObjects::BlendState].end() )
		{
			objects_ClientSide[PipelineObjects::BlendState].erase( id );
			toRemove.push( { id, PipelineObjects::BlendState } );
		}
	}
	void PipelineHandler::CreateDepthStencilState( Utilities::GUID id, const Pipeline::DepthStencilState& state )
	{
		PROFILE;
		if ( auto find = objects_ClientSide[PipelineObjects::DepthStencilState].find( id ); find != objects_ClientSide[PipelineObjects::DepthStencilState].end() )
			throw Could_Not_Create_DepthStencilState( "DepthStencilState already exists", id, state );

		D3D11_DEPTH_STENCIL_DESC dsd;
		dsd.DepthEnable = state.enableDepth;
		switch ( state.comparisonOperation )
		{
		case Pipeline::ComparisonOperation::EQUAL:			dsd.DepthFunc = D3D11_COMPARISON_EQUAL; break;
		case Pipeline::ComparisonOperation::GREATER:			dsd.DepthFunc = D3D11_COMPARISON_GREATER; break;
		case Pipeline::ComparisonOperation::GREATER_EQUAL:	dsd.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL; break;
		case Pipeline::ComparisonOperation::LESS:				dsd.DepthFunc = D3D11_COMPARISON_LESS; break;
		case Pipeline::ComparisonOperation::LESS_EQUAL:		dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; break;
		case Pipeline::ComparisonOperation::NO_COMPARISON:	dsd.DepthFunc = D3D11_COMPARISON_NEVER; break;
		}

		dsd.DepthWriteMask = state.enableDepth ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

		dsd.StencilEnable = true;
		dsd.StencilReadMask = 0xFF;
		dsd.StencilWriteMask = 0xFF;

		dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

		dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

		ComPtr<ID3D11DepthStencilState> dss;
		if ( auto hr = device->CreateDepthStencilState( &dsd, &dss ); FAILED( hr ) )
			throw Could_Not_Create_DepthStencilState( "Could not create DepthStencilState", id, state, hr );

		objects_ClientSide[PipelineObjects::DepthStencilState].emplace( id );
		toAdd.push( { id, PipelineObjects::DepthStencilState_{ dss } } );
	}
	void PipelineHandler::DestroyDepthStencilState( Utilities::GUID id )noexcept
	{
		PROFILE;
		if ( auto find = objects_ClientSide[PipelineObjects::DepthStencilState].find( id ); find != objects_ClientSide[PipelineObjects::DepthStencilState].end() )
		{
			objects_ClientSide[PipelineObjects::DepthStencilState].erase( id );
			toRemove.push( { id, PipelineObjects::DepthStencilState } );
		}
	}
	void PipelineHandler::CreateSamplerState( Utilities::GUID id, const Pipeline::SamplerState& state )
	{
		PROFILE;
		if ( auto find = objects_ClientSide[PipelineObjects::SamplerState].find( id ); find != objects_ClientSide[PipelineObjects::SamplerState].end() )
			throw Could_Not_Create_SamplerState( "SamplerState already exists", id, state );

		D3D11_SAMPLER_DESC sd;
		ZeroMemory( &sd, sizeof( D3D11_SAMPLER_DESC ) );

		switch ( state.addressU )
		{
		case Pipeline::AddressingMode::WRAP:		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; break;
		case Pipeline::AddressingMode::CLAMP:		sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP; break;
		case Pipeline::AddressingMode::MIRROR:	sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR; break;
		}
		switch ( state.addressV )
		{
		case Pipeline::AddressingMode::WRAP:		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; break;
		case Pipeline::AddressingMode::CLAMP:		sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP; break;
		case Pipeline::AddressingMode::MIRROR:	sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR; break;
		}
		switch ( state.addressW )
		{
		case Pipeline::AddressingMode::WRAP:		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; break;
		case Pipeline::AddressingMode::CLAMP:		sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP; break;
		case Pipeline::AddressingMode::MIRROR:	sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR; break;
		}
		switch ( state.filter )
		{
		case Pipeline::Filter::ANISOTROPIC:	sd.Filter = D3D11_FILTER_ANISOTROPIC; break;
		case Pipeline::Filter::LINEAR:		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; break;
		case Pipeline::Filter::POINT:			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; break;
		}
		sd.BorderColor[0] = 0.0f; sd.BorderColor[1] = 0.0f; sd.BorderColor[2] = 0.0f; sd.BorderColor[3] = 0.0f;
		sd.MinLOD = 0;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
		sd.MaxAnisotropy = state.maxAnisotropy;
		ComPtr<ID3D11SamplerState> samplerState;
		if ( auto hr = device->CreateSamplerState( &sd, &samplerState ); FAILED( hr ) )
			throw Could_Not_Create_SamplerState( "Could not create SamplerState", id, state, hr );

		objects_ClientSide[PipelineObjects::SamplerState].emplace( id );
		toAdd.push( { id, PipelineObjects::SamplerState_{ samplerState } } );
	}
	void PipelineHandler::DestroySamplerState( Utilities::GUID id )noexcept
	{
		PROFILE;
		if ( auto find = objects_ClientSide[PipelineObjects::SamplerState].find( id ); find != objects_ClientSide[PipelineObjects::SamplerState].end() )
		{
			objects_ClientSide[PipelineObjects::SamplerState].erase( id );
			toRemove.push( { id, PipelineObjects::SamplerState } );
		}
	}
	void PipelineHandler::AddTexture( Utilities::GUID id, ComPtr<ID3D11ShaderResourceView> srv )
	{
		if ( auto find = objects_ClientSide[PipelineObjects::ShaderResourceView].find( id ); find != objects_ClientSide[PipelineObjects::ShaderResourceView].end() )
			throw Could_Not_Add_Texture( "Texture already exists", id );
		objects_ClientSide[PipelineObjects::ShaderResourceView].emplace( id );
		toAdd.push( { id, PipelineObjects::ShaderResourceView_{ srv} } );
	}
	void PipelineHandler::AddTexture( Utilities::GUID id, ComPtr<ID3D11RenderTargetView> rtv )
	{
		if ( auto find = objects_ClientSide[PipelineObjects::RenderTarget].find( id ); find != objects_ClientSide[PipelineObjects::RenderTarget].end() )
			throw Could_Not_Add_Texture( "RenderTarget already exists", id );

		objects_ClientSide[PipelineObjects::RenderTarget].emplace( id );
		toAdd.push( { id, PipelineObjects::RenderTarget_{ rtv,{ 0.0f,0.0f,0.0f,0.0f } } } );
	}
	void PipelineHandler::CreateTexture( Utilities::GUID id, const Pipeline::Texture& info )
	{
		PROFILE;

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = info.width;
		desc.Height = info.height;
		desc.MipLevels = info.mipLevels;
		desc.ArraySize = info.arraySize;
		switch ( info.format )
		{
		case Pipeline::TextureFormat::R32G32B32A32_FLOAT:	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
		case Pipeline::TextureFormat::R8G8B8A8_UNORM:		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
		default: throw Could_Not_Create_Texture( "Texture format unsupported", id, info ); break;
		}

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = 0;
		if ( flag_has( info.flags, Pipeline::TextureFlags::SHADER_RESOURCE ) ) desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		if ( flag_has( info.flags, Pipeline::TextureFlags::UNORDERED_ACCESS ) ) desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		if ( flag_has( info.flags, Pipeline::TextureFlags::RENDER_TARGET ) ) desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		desc.CPUAccessFlags = flag_has( info.flags, Pipeline::TextureFlags::CPU_ACCESS_READ ) ? D3D11_CPU_ACCESS_READ : 0;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		ComPtr<ID3D11Texture2D> texture;
		if ( info.data )
		{
			D3D11_SUBRESOURCE_DATA d;
			d.pSysMem = info.data;
			d.SysMemPitch = info.memPitch;
			d.SysMemSlicePitch = info.memSlicePitch;
			if ( auto hr = device->CreateTexture2D( &desc, &d, &texture ); FAILED( hr ) )
				throw Could_Not_Create_Texture( "CreateTexture2D failed", id, info, hr );
		}
		else
			if ( auto hr = device->CreateTexture2D( &desc, nullptr, &texture ); FAILED( hr ) )
				throw Could_Not_Create_Texture( "CreateTexture2D failed", id, info, hr );


		if ( flag_has( info.flags, Pipeline::TextureFlags::SHADER_RESOURCE ) )
		{
			if ( auto find = objects_ClientSide[PipelineObjects::ShaderResourceView].find( id ); find != objects_ClientSide[PipelineObjects::ShaderResourceView].end() )
				throw Could_Not_Create_Texture( "Texture already exists", id, info );

			ComPtr<ID3D11ShaderResourceView> srv;
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = desc.Format;
			switch ( info.viewDimension )
			{
			case Pipeline::ViewDimension::Texture_1D:
				srvDesc.ViewDimension = info.arraySize > 1 ? D3D11_SRV_DIMENSION_TEXTURE1DARRAY : D3D11_SRV_DIMENSION_TEXTURE1D;
				break;
			case Pipeline::ViewDimension::Texture_2D:
				srvDesc.ViewDimension = info.arraySize > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DARRAY : D3D11_SRV_DIMENSION_TEXTURE2D;
				break;
			case Pipeline::ViewDimension::Texture_3D:
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
				break;
			case Pipeline::ViewDimension::Cube:
				srvDesc.ViewDimension = info.arraySize > 1 ? D3D11_SRV_DIMENSION_TEXTURECUBEARRAY : D3D11_SRV_DIMENSION_TEXTURECUBE;
				break;
			default: throw Could_Not_Create_Texture( "View Dimention unsupported", id, info ); break;
			}

			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = info.mipLevels;
			if ( auto hr = device->CreateShaderResourceView( texture.Get(), &srvDesc, &srv ); FAILED( hr ) )
				throw Could_Not_Create_Texture( "CreateShaderResourceView failed", id, info, hr );

			objects_ClientSide[PipelineObjects::ShaderResourceView].emplace( id );
			toAdd.push( { id, PipelineObjects::ShaderResourceView_{ srv} } );
		}
		if ( flag_has( info.flags, Pipeline::TextureFlags::UNORDERED_ACCESS ) )
		{
			if ( auto find = objects_ClientSide[PipelineObjects::UnorderedAccessView].find( id ); find != objects_ClientSide[PipelineObjects::UnorderedAccessView].end() )
				throw Could_Not_Create_Texture( "UnorderedAccessView already exists", id, info );

			D3D11_UNORDERED_ACCESS_VIEW_DESC description;
			ZeroMemory( &description, sizeof( description ) );
			description.Format = desc.Format;
			switch ( info.viewDimension )
			{
			case Pipeline::ViewDimension::Texture_1D:
				description.ViewDimension = info.arraySize > 1 ? D3D11_UAV_DIMENSION_TEXTURE1DARRAY : D3D11_UAV_DIMENSION_TEXTURE1D;
				break;
			case Pipeline::ViewDimension::Texture_2D:
				description.ViewDimension = info.arraySize > 1 ? D3D11_UAV_DIMENSION_TEXTURE2DARRAY : D3D11_UAV_DIMENSION_TEXTURE2D;
				break;
			case Pipeline::ViewDimension::Texture_3D:
				description.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
				break;
			default: throw Could_Not_Create_Texture( "View Dimention unsupported", id, info ); break;
			}
			description.Texture2D.MipSlice = 0;

			ComPtr<ID3D11UnorderedAccessView> unorderedAccessView;
			if ( auto hr = device->CreateUnorderedAccessView( texture.Get(), &description, &unorderedAccessView ); FAILED( hr ) )
				throw Could_Not_Create_Texture( "CreateUnorderedAccessView failed", id, info, hr );

			objects_ClientSide[PipelineObjects::UnorderedAccessView].emplace( id );
			toAdd.push( { id, PipelineObjects::UnorderedAccessView_{ unorderedAccessView, { info.clearColor[0] , info.clearColor[1], info.clearColor[2], info.clearColor[3] } } } );
		}


		if ( flag_has( info.flags, Pipeline::TextureFlags::RENDER_TARGET ) )
		{
			if ( auto find = objects_ClientSide[PipelineObjects::RenderTarget].find( id ); find != objects_ClientSide[PipelineObjects::RenderTarget].end() )
				throw Could_Not_Create_Texture( "RenderTarget already exists", id, info );

			D3D11_RENDER_TARGET_VIEW_DESC rtvd;
			rtvd.Format = desc.Format;
			switch ( info.viewDimension )
			{
			case Pipeline::ViewDimension::Texture_1D:
				rtvd.ViewDimension = info.arraySize > 1 ? D3D11_RTV_DIMENSION_TEXTURE1DARRAY : D3D11_RTV_DIMENSION_TEXTURE1D;
				break;
			case Pipeline::ViewDimension::Texture_2D:
				rtvd.ViewDimension = info.arraySize > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DARRAY : D3D11_RTV_DIMENSION_TEXTURE2D;
				break;
			case Pipeline::ViewDimension::Texture_3D:
				rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
				break;
			default: throw Could_Not_Create_Texture( "View Dimention unsupported", id, info ); break;
			}
			rtvd.Texture2D.MipSlice = 0;
			ComPtr<ID3D11RenderTargetView> rtv;
			if ( auto hr = device->CreateRenderTargetView( texture.Get(), &rtvd, &rtv ); FAILED( hr ) )
				throw Could_Not_Create_Texture( "CreateRenderTargetView failed", id, info, hr );

			objects_ClientSide[PipelineObjects::RenderTarget].emplace( id );
			toAdd.push( { id, PipelineObjects::RenderTarget_{ rtv,{ info.clearColor[0] , info.clearColor[1], info.clearColor[2], info.clearColor[3] } } } );
		}
	}
	void PipelineHandler::DestroyTexture( Utilities::GUID id )noexcept
	{
		PROFILE;

		if ( auto find = objects_ClientSide[PipelineObjects::RenderTarget].find( id ); find != objects_ClientSide[PipelineObjects::RenderTarget].end() )
		{
			objects_ClientSide[PipelineObjects::RenderTarget].erase( id );
			toRemove.push( { id, PipelineObjects::RenderTarget } );
		}
		if ( auto find = objects_ClientSide[PipelineObjects::UnorderedAccessView].find( id ); find != objects_ClientSide[PipelineObjects::UnorderedAccessView].end() )
		{
			objects_ClientSide[PipelineObjects::UnorderedAccessView].erase( id );
			toRemove.push( { id, PipelineObjects::UnorderedAccessView } );
		}
		if ( auto find = objects_ClientSide[PipelineObjects::ShaderResourceView].find( id ); find != objects_ClientSide[PipelineObjects::ShaderResourceView].end() )
		{
			objects_ClientSide[PipelineObjects::ShaderResourceView].erase( id );
			toRemove.push( { id, PipelineObjects::ShaderResourceView } );
		}
	}
	void PipelineHandler::AddDepthStencilView( Utilities::GUID id, ComPtr<ID3D11DepthStencilView> dsv )
	{
		if ( auto find = objects_ClientSide[PipelineObjects::DepthStencilView].find( id ); find != objects_ClientSide[PipelineObjects::DepthStencilView].end() )
			throw Could_Not_Add_DepthStencilView( "DepthStencilView already exists", id );

		objects_ClientSide[PipelineObjects::DepthStencilView].emplace( id );
		toAdd.push( { id, PipelineObjects::DepthStencilView_{ dsv } } );
	}
	void PipelineHandler::CreateDepthStencilView( Utilities::GUID id, const Pipeline::DepthStencilView& view )
	{
		PROFILE;

		if ( auto find = objects_ClientSide[PipelineObjects::DepthStencilView].find( id ); find != objects_ClientSide[PipelineObjects::DepthStencilView].end() )
			throw Could_Not_Create_DepthStencilView( "DepthStencilView already exists", id, view );

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = UINT( view.width );
		desc.Height = UINT( view.height );
		desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		if ( flag_has( view.flags, Pipeline::DepthStencilViewFlags::SHADER_RESOURCE ) ) desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Texture2D.MipSlice = 0;
		dsvd.Flags = 0;

		ComPtr<ID3D11Texture2D> texture;

		if ( auto hr = device->CreateTexture2D( &desc, nullptr, &texture ); FAILED( hr ) )
			throw Could_Not_Create_DepthStencilView( "CreateTexture2D failed", id, view, hr );

		if ( flag_has( view.flags, Pipeline::DepthStencilViewFlags::SHADER_RESOURCE ) )
		{
			if ( auto find = objects_ClientSide[PipelineObjects::ShaderResourceView].find( id ); find != objects_ClientSide[PipelineObjects::ShaderResourceView].end() )
				throw Could_Not_Create_DepthStencilView( "Texture already exists", id, view );

			D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
			srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvd.Texture2D.MostDetailedMip = 0;
			srvd.Texture2D.MipLevels = 1;
			ComPtr<ID3D11ShaderResourceView> srv;
			if ( auto hr = device->CreateShaderResourceView( texture.Get(), &srvd, &srv ); FAILED( hr ) )
				throw Could_Not_Create_DepthStencilView( "Could not create ShaderResourceView", id, view, hr );
			objects_ClientSide[PipelineObjects::ShaderResourceView].emplace( id );
			toAdd.push( { id, PipelineObjects::ShaderResourceView_{ srv } } );
		}

		ComPtr<ID3D11DepthStencilView> dsv;
		if ( auto hr = device->CreateDepthStencilView( texture.Get(), &dsvd, &dsv ); FAILED( hr ) )
			throw Could_Not_Create_DepthStencilView( "Could not create depthstencilview", id, view, hr );

		objects_ClientSide[PipelineObjects::DepthStencilView].emplace( id );
		toAdd.push( { id, PipelineObjects::DepthStencilView_{ dsv } } );
	}
	void PipelineHandler::DestroyDepthStencilView( Utilities::GUID id )noexcept
	{
		PROFILE;

		if ( auto find = objects_ClientSide[PipelineObjects::DepthStencilView].find( id ); find != objects_ClientSide[PipelineObjects::DepthStencilView].end() )
		{
			objects_ClientSide[PipelineObjects::DepthStencilView].erase( id );
			toRemove.push( { id, PipelineObjects::DepthStencilView } );
		}
		if ( auto find = objects_ClientSide[PipelineObjects::ShaderResourceView].find( id ); find != objects_ClientSide[PipelineObjects::ShaderResourceView].end() )
		{
			objects_ClientSide[PipelineObjects::ShaderResourceView].erase( id );
			toRemove.push( { id, PipelineObjects::ShaderResourceView } );
		}
	}

	void PipelineHandler::UpdatePipelineObjects()noexcept
	{
		PROFILE;
		while ( !toRemove.isEmpty() )
		{
			auto& t = toRemove.top();
			objects_RenderSide[t.type].erase( t.id );

			toRemove.pop();
		}
		while ( !toAdd.isEmpty() )
		{
			auto& t = toAdd.top();
			objects_RenderSide[t.obj.index()].emplace( t.id, std::move( t.obj ) );

			toAdd.pop();
		}
	}
	void PipelineHandler::UpdateObject( Utilities::GUID id, PipelineObjectType type, const std::function<void( UpdateObjectRef & obj )>& cb )
	{
		switch ( type )
		{
		case Graphics::PipelineObjectType::Buffer:
			if ( auto find = objects_RenderSide[PipelineObjects::Buffer].find( id ); find != objects_RenderSide[PipelineObjects::Buffer].end() )
			{
				Buffer_UO obj( context, std::get<PipelineObjects::Buffer_>( find->second ).obj, std::get<PipelineObjects::Buffer_>( find->second ).info );
				cb( obj );
			}
			break;
		case Graphics::PipelineObjectType::RenderTarget:
			break;
		case Graphics::PipelineObjectType::UnorderedAccessView:
			break;
		case Graphics::PipelineObjectType::Texture:
			break;
		case Graphics::PipelineObjectType::DepthStencilView:
			break;
		case Graphics::PipelineObjectType::Viewport:
			break;
		default:
			break;
		}

	}
	ComPtr<ID3D11Buffer> PipelineHandler::_CreateBuffer( Utilities::GUID id, const Pipeline::Buffer& buffer )
	{
		PROFILE;
		if ( buffer.elementCount == 0 )
			throw Could_Not_Create_Buffer( "Buffer element count can not be zero", id, buffer );
		if ( flag_has( buffer.flags, Pipeline::BufferFlags::BIND_CONSTANT ) )
		{
			if ( buffer.elementStride % 16 != 0 )
				throw Could_Not_Create_Buffer( "Constant buffer memory must be a multiple of 16 bytes", id, buffer );
		}

		if ( auto find = objects_ClientSide[PipelineObjects::Buffer].find( id ); find != objects_ClientSide[PipelineObjects::Buffer].end() )
		{
			throw Could_Not_Create_Buffer( "Buffer already exists", id, buffer );
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof( bd ) );
		bd.BindFlags = 0;
		if ( flag_has( buffer.flags, Pipeline::BufferFlags::BIND_CONSTANT ) ) bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		if ( flag_has( buffer.flags, Pipeline::BufferFlags::BIND_VERTEX ) ) bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		if ( flag_has( buffer.flags, Pipeline::BufferFlags::BIND_INDEX ) ) bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		if ( flag_has( buffer.flags, Pipeline::BufferFlags::BIND_STREAMOUT ) ) bd.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
		bd.ByteWidth = buffer.maxElements * buffer.elementStride;
		bd.CPUAccessFlags = 0;
		if ( flag_has( buffer.flags, Pipeline::BufferFlags::CPU_READ ) && flag_has( buffer.flags, Pipeline::BufferFlags::BIND_CONSTANT ) )
			throw Could_Not_Create_Buffer( "Constant buffer can not have read access", id, buffer );
		if ( flag_has( buffer.flags, Pipeline::BufferFlags::CPU_WRITE ) ) bd.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
		if ( flag_has( buffer.flags, Pipeline::BufferFlags::CPU_READ ) ) bd.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
		bd.Usage = D3D11_USAGE_DEFAULT;
		if ( flag_has( buffer.flags, Pipeline::BufferFlags::DYNAMIC ) ) bd.Usage = D3D11_USAGE_DYNAMIC;
		if ( flag_has( buffer.flags, Pipeline::BufferFlags::IMMUTABLE ) ) bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.MiscFlags = 0;
		if ( flag_has( buffer.flags, Pipeline::BufferFlags::STRUCTURED ) )
		{
			bd.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; bd.StructureByteStride = buffer.elementStride;
		};
		if ( flag_has( buffer.flags, Pipeline::BufferFlags::RAW ) ) bd.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

		ComPtr<ID3D11Buffer> pBuffer;
		PipelineObject object;
		HRESULT hr;

		if ( buffer.data )
		{
			D3D11_SUBRESOURCE_DATA d;
			d.pSysMem = buffer.data;
			d.SysMemPitch = 0;
			d.SysMemSlicePitch = 0;
			hr = device->CreateBuffer( &bd, &d, &pBuffer );
		}
		else
		{
			hr = device->CreateBuffer( &bd, nullptr, &pBuffer );
		}
		if ( FAILED( hr ) )
			throw Could_Not_Create_Buffer( "'CreateBuffer' failed", id, buffer, hr );
	
		return pBuffer;
	}
}