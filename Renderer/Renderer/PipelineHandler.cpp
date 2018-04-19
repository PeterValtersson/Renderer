#include "PipelineHandler.h"
#include <Profiler.h>
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
	PipelineHandler::PipelineHandler()
	{
	}


	PipelineHandler::~PipelineHandler()
	{
	}

	UERROR PipelineHandler::Init(
		ID3D11Device * device, ID3D11DeviceContext * context, 
		ID3D11RenderTargetView* backbuffer, ID3D11ShaderResourceView* bbsrv,
		ID3D11DepthStencilView* dsv, ID3D11ShaderResourceView* dsvsrv,
		const D3D11_VIEWPORT& vp)
	{
		StartProfile;
		this->device = device;
		this->context = context;
		

		objects_RenderSide[PipelineObjects::RenderTarget].emplace(Default_RenderTarget, PipelineObjects::RenderTarget_{ backbuffer,{ 0.0f, 0.0f,1.0f,0.0f } });
		objects_RenderSide[PipelineObjects::DepthStencilView].emplace(Default_DepthStencil, PipelineObjects::DepthStencilView_{ dsv });
		objects_RenderSide[PipelineObjects::ShaderResourceView].emplace(Default_RenderTarget, PipelineObjects::ShaderResourceView_{ bbsrv });
		objects_RenderSide[PipelineObjects::ShaderResourceView].emplace(Default_DepthStencil, PipelineObjects::ShaderResourceView_{ dsvsrv });
		objects_RenderSide[PipelineObjects::Viewport].emplace(Default_Viewport, PipelineObjects::Viewport_{ vp});

		objects_ClientSide[PipelineObjects::RenderTarget].emplace(Default_RenderTarget);
		objects_ClientSide[PipelineObjects::DepthStencilView].emplace(Default_DepthStencil);
		objects_ClientSide[PipelineObjects::ShaderResourceView].emplace(Default_RenderTarget);
		objects_ClientSide[PipelineObjects::ShaderResourceView].emplace(Default_DepthStencil);
		objects_ClientSide[PipelineObjects::Viewport].emplace(Default_Viewport);

		EMPLACE_NULL(PipelineObjects::Buffer);

		EMPLACE_NULL(PipelineObjects::VertexShader);
		EMPLACE_NULL(PipelineObjects::GeometryShader);
		EMPLACE_NULL(PipelineObjects::PixelShader);
		EMPLACE_NULL(PipelineObjects::ComputeShader);

		EMPLACE_NULL(PipelineObjects::RenderTarget);
		EMPLACE_NULL(PipelineObjects::UnorderedAccessView);
		EMPLACE_NULL(PipelineObjects::ShaderResourceView);
		EMPLACE_NULL(PipelineObjects::DepthStencilView);

		EMPLACE_NULL(PipelineObjects::SamplerState);
		EMPLACE_NULL(PipelineObjects::BlendState);
		EMPLACE_NULL(PipelineObjects::RasterizerState);
		EMPLACE_NULL(PipelineObjects::DepthStencilState);

		EMPLACE_DEF(PipelineObjects::Viewport);

		PASS_IF_ERROR(CreateShader(Default_VertexShader_FullscreenQUAD, Pipeline::ShaderType::VERTEX, fullscreenQuadVS, strlen(fullscreenQuadVS), "VS_main", "vs_5_0"));
		PASS_IF_ERROR(CreateShader(Default_PixelShader_POS_TEXTURE_MULTICHANNGEL, Pipeline::ShaderType::PIXEL, MultiPS, strlen(MultiPS), "PS_main", "ps_5_0"));
		PASS_IF_ERROR(CreateShader(Default_PixelShader_POS_TEXTURE_SingleCHANNGEL, Pipeline::ShaderType::PIXEL, SinglePS, strlen(SinglePS), "PS_main", "ps_5_0"));



		RETURN_SUCCESS;
	}
#define RELEASE_PLO(type) if(o.second.index() == type) std::get<type##_>(o.second).Release()
	void PipelineHandler::Shutdown()
	{
		StartProfile;
		objects_RenderSide[PipelineObjects::RenderTarget].erase(Default_RenderTarget);
		objects_RenderSide[PipelineObjects::DepthStencilView].erase(Default_DepthStencil);
		objects_RenderSide[PipelineObjects::ShaderResourceView].erase(Default_RenderTarget);
		objects_RenderSide[PipelineObjects::ShaderResourceView].erase(Default_DepthStencil);


		for (auto& ot : objects_RenderSide)
			for (auto& o : ot)
			{
				RELEASE_PLO(PipelineObjects::Buffer);

				else RELEASE_PLO(PipelineObjects::VertexShader);
				else RELEASE_PLO(PipelineObjects::GeometryShader);
				else RELEASE_PLO(PipelineObjects::PixelShader);
				else RELEASE_PLO(PipelineObjects::ComputeShader);

				else RELEASE_PLO(PipelineObjects::RenderTarget);
				else RELEASE_PLO(PipelineObjects::UnorderedAccessView);
				else RELEASE_PLO(PipelineObjects::ShaderResourceView);
				else RELEASE_PLO(PipelineObjects::DepthStencilView);

				else RELEASE_PLO(PipelineObjects::SamplerState);
				else RELEASE_PLO(PipelineObjects::BlendState);
				else RELEASE_PLO(PipelineObjects::RasterizerState);
				else RELEASE_PLO(PipelineObjects::DepthStencilState);
			}
				
	}

	UERROR PipelineHandler::CreateBuffer(Utilities::GUID id, const Pipeline::Buffer & buffer)
	{
		StartProfile;
		if (buffer.flags & Pipeline::BufferFlags::BIND_CONSTANT)
		{
			if (buffer.elementStride % 16 != 0)
				RETURN_ERROR("Constant buffer memory must be a multiple of 16 bytes");
		}

		if (auto find = objects_ClientSide[PipelineObjects::Buffer].find(id); find != objects_ClientSide[PipelineObjects::Buffer].end())
		{
			RETURN_ERROR("Buffer already exists");
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.BindFlags = 0;
		if (buffer.flags & Pipeline::BufferFlags::BIND_CONSTANT) bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		if (buffer.flags & Pipeline::BufferFlags::BIND_VERTEX) bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		if (buffer.flags & Pipeline::BufferFlags::BIND_INDEX) bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		if (buffer.flags & Pipeline::BufferFlags::BIND_STREAMOUT) bd.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
		bd.ByteWidth = buffer.maxElements * buffer.elementStride;
		bd.CPUAccessFlags = 0;
		if (buffer.flags & Pipeline::BufferFlags::CPU_WRITE) bd.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
		if (buffer.flags & Pipeline::BufferFlags::CPU_READ) bd.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
		bd.Usage = D3D11_USAGE_DEFAULT;
		if (buffer.flags & Pipeline::BufferFlags::DYNAMIC) bd.Usage = D3D11_USAGE_DYNAMIC;
		if (buffer.flags & Pipeline::BufferFlags::IMMUTABLE) bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.MiscFlags = 0;
		if (buffer.flags & Pipeline::BufferFlags::STRUCTURED) { bd.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; bd.StructureByteStride = buffer.elementStride; };
		if (buffer.flags & Pipeline::BufferFlags::RAW) bd.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

		ID3D11Buffer* pBuffer;
		PipelineObject object;
		HRESULT hr;

		if (buffer.data)
		{
			D3D11_SUBRESOURCE_DATA d;
			d.pSysMem = buffer.data;
			d.SysMemPitch = 0;
			d.SysMemSlicePitch = 0;
			hr = device->CreateBuffer(&bd, &d, &pBuffer);
		}
		else
		{
			hr = device->CreateBuffer(&bd, nullptr, &pBuffer);
		}

		RETURN_IF_HR_ERROR(hr, "Could not create buffer. Likely due to incompatible flags");

		toAdd.push({ id, PipelineObjects::Buffer_{ pBuffer, buffer }});

		objects_ClientSide[PipelineObjects::Buffer].emplace(id);
		
		RETURN_SUCCESS;
	}

	UERROR PipelineHandler::DestroyBuffer(Utilities::GUID id)
	{
		StartProfile;
		if (auto find = objects_ClientSide[PipelineObjects::Buffer].find(id); find != objects_ClientSide[PipelineObjects::Buffer].end())
		{
			objects_ClientSide[PipelineObjects::Buffer].erase(id);
			toRemove.push({ id,PipelineObjects::Buffer });			
		}
		
		RETURN_SUCCESS;
	}
	
	UERROR PipelineHandler::CreateViewport(Utilities::GUID id, const Pipeline::Viewport & viewport)
	{
		StartProfile;
		if (auto find = objects_ClientSide[PipelineObjects::Viewport].find(id); find != objects_ClientSide[PipelineObjects::Viewport].end())
			RETURN_ERROR("Viewport with name already exists");


		objects_ClientSide[PipelineObjects::Viewport].emplace(id);

		toAdd.push({ id , PipelineObjects::Viewport_{ viewport.topLeftX, viewport.topLeftY, viewport.width, viewport.height, viewport.minDepth, viewport.maxDepth } });

		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::CreateShader(Utilities::GUID id, Pipeline::ShaderType type, const char * sourceCode, size_t size, const char * entryPoint, const char * shaderModel)
	{
		StartProfile;
		ID3DBlob* blob;
		ID3DBlob* error;
		if(FAILED(D3DCompile(sourceCode, size, NULL, NULL, NULL, entryPoint, shaderModel, 0, 0, &blob, &error)))
			RETURN_ERROR_EX("Could not compile shader. Error: ", (char*) error);
		return CreateShader(id, type, blob->GetBufferPointer(), blob->GetBufferSize());
	}
	template<class T>
	struct SafeDXP
	{
		T* operator->()
		{
			return obj;
		}
		T** Create()
		{
			return &obj;
		}
		T* Get()
		{
			return obj;
		}
		T* Done()
		{
			T* t = obj;
			obj = nullptr;
			return t;
		}
		~SafeDXP()
		{
			if (obj)
				obj->Release();
		}
	private:
		T* obj = nullptr;
	};
	UERROR PipelineHandler::CreateShader(Utilities::GUID id, Pipeline::ShaderType type, void * data, size_t size)
	{
		StartProfile;
		if (type == Pipeline::ShaderType::VERTEX)
		{
			if (auto find = objects_ClientSide[PipelineObjects::VertexShader].find(id); find != objects_ClientSide[PipelineObjects::VertexShader].end())
				RETURN_ERROR("Vertex shader with name already exists");
		}
		else if (type == Pipeline::ShaderType::PIXEL)
		{
			if (auto find = objects_ClientSide[PipelineObjects::PixelShader].find(id); find != objects_ClientSide[PipelineObjects::PixelShader].end())
				RETURN_ERROR("Pixel shader with name already exists");
		}
		else if (type == Pipeline::ShaderType::GEOMETRY || type == Pipeline::ShaderType::GEOMETRY_STREAM_OUT)
		{
			if (auto find = objects_ClientSide[PipelineObjects::GeometryShader].find(id); find != objects_ClientSide[PipelineObjects::GeometryShader].end())
				RETURN_ERROR("Geometry shader with name already exists");
		}
		else if (type == Pipeline::ShaderType::COMPUTE)
		{
			if (auto find = objects_ClientSide[PipelineObjects::ComputeShader].find(id); find != objects_ClientSide[PipelineObjects::ComputeShader].end())
				RETURN_ERROR("Compute shader with name already exists");
		}
		else
		{
			RETURN_ERROR("Shader type not supported yet");
		}
		//Create the input layout with the help of shader reflection
		SafeDXP<ID3D11ShaderReflection> reflection;

		RETURN_IF_HR_ERROR(D3DReflect(data, size, IID_ID3D11ShaderReflection, (void**)reflection.Create()), "Could not reflect shader");
		D3D11_SHADER_DESC shaderDesc;
		reflection->GetDesc(&shaderDesc);
		std::vector<ShaderResourceToAndBindSlot> cbuffers;
		for (unsigned int i = 0; i < shaderDesc.BoundResources; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC sibd;
			reflection->GetResourceBindingDesc(i, &sibd);
			if (sibd.Type == D3D_SIT_CBUFFER)
			{
				//Can't get the size from the RBD, can't get bindslot from the SBD...	
				//Find the sbd with the same name to get the size.
				Utilities::GUID sibdName = std::string(sibd.Name);
				for (unsigned int j = 0; j < shaderDesc.ConstantBuffers; ++j)
				{
					D3D11_SHADER_BUFFER_DESC sbd;
					ID3D11ShaderReflectionConstantBuffer* srcb = reflection->GetConstantBufferByIndex(j);
					srcb->GetDesc(&sbd);
					Utilities::GUID name = std::string(sbd.Name);
					if (name == sibdName)
					{
						PASS_IF_ERROR(CreateBuffer(name, Pipeline::Buffer::ConstantBuffer(uint16_t(sbd.Size))));
						cbuffers.push_back({ name, sibd.BindPoint });

						break;
					}
				}
			}
		}

		if (type == Pipeline::ShaderType::VERTEX)
		{
			SafeDXP<ID3D11InputLayout> inputLayout;
			SafeDXP<ID3D11VertexShader> vs;
			

			RETURN_IF_HR_ERROR(device->CreateVertexShader(data, size, nullptr, vs.Create()), "Could not create vertex shader");


			std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs;
			uint32_t offset = 0;
			for (uint32_t i = 0; i < shaderDesc.InputParameters; ++i)
			{
				D3D11_SIGNATURE_PARAMETER_DESC signatureParamaterDesc;
				reflection->GetInputParameterDesc(i, &signatureParamaterDesc);
				D3D11_INPUT_ELEMENT_DESC inputElementDesc;
				inputElementDesc.SemanticName = signatureParamaterDesc.SemanticName;
				inputElementDesc.SemanticIndex = signatureParamaterDesc.SemanticIndex;
				inputElementDesc.AlignedByteOffset = offset;
				inputElementDesc.InputSlot = 0;
				inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				inputElementDesc.InstanceDataStepRate = 0;

				if (signatureParamaterDesc.Mask == 1)
				{
					const std::string semName(inputElementDesc.SemanticName);
					if (semName == "SV_InstanceID")
						continue;
					if (semName == "SV_VertexID")
						continue;
					if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
						inputElementDesc.Format = DXGI_FORMAT_R32_FLOAT;
					else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
						inputElementDesc.Format = DXGI_FORMAT_R32_SINT;
					else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
						inputElementDesc.Format = DXGI_FORMAT_R32_UINT;
					offset += 4;

				}
				else if (signatureParamaterDesc.Mask <= 3)
				{
					if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
						inputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
					else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
						inputElementDesc.Format = DXGI_FORMAT_R32G32_SINT;
					else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
						inputElementDesc.Format = DXGI_FORMAT_R32G32_UINT;
					offset += 8;
				}
				else if (signatureParamaterDesc.Mask <= 7)
				{
					if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
						inputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
						inputElementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
					else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
						inputElementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
					offset += 12;
				}
				else if (signatureParamaterDesc.Mask <= 15)
				{
					if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
						inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
						inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
					else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
						inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
					offset += 16;
				}
				inputElementDescs.push_back(inputElementDesc);
			}
			if (inputElementDescs.size() > 0)
				RETURN_IF_HR_ERROR(device->CreateInputLayout(inputElementDescs.data(), UINT(inputElementDescs.size()), data, size, inputLayout.Create()), "Could not create input layout");

			toAdd.push({ id, PipelineObjects::VertexShader_{vs.Done(), inputLayout.Done(), cbuffers} });
			objects_ClientSide[PipelineObjects::VertexShader].emplace(id);
		}
		else if (type == Pipeline::ShaderType::PIXEL)
		{
			SafeDXP<ID3D11PixelShader> s;
			RETURN_IF_HR_ERROR(device->CreatePixelShader(data, size, nullptr, s.Create()), "Could not create pixel shader");
			toAdd.push({ id, PipelineObjects::PixelShader_{ s.Done(), cbuffers } });
			objects_ClientSide[PipelineObjects::PixelShader].emplace(id);
		}
		else if (type == Pipeline::ShaderType::GEOMETRY)
		{
			SafeDXP<ID3D11GeometryShader> s;
			RETURN_IF_HR_ERROR(device->CreateGeometryShader(data, size, nullptr, s.Create()), "Could not create geometry shader");
			toAdd.push({ id, PipelineObjects::GeometryShader_{ s.Done(), cbuffers } });
			objects_ClientSide[PipelineObjects::GeometryShader].emplace(id);
		}
		else if (type == Pipeline::ShaderType::GEOMETRY_STREAM_OUT)
		{
			std::vector<D3D11_SO_DECLARATION_ENTRY> SOEntries;
			for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
			{
				D3D11_SIGNATURE_PARAMETER_DESC signatureParameterDesc;
				reflection->GetInputParameterDesc(i, &signatureParameterDesc);
				BYTE mask = signatureParameterDesc.Mask;
				int varCount = 0;
				while (mask)
				{
					if (mask & 0x01) varCount++;
					mask = mask >> 1;
				}

				D3D11_SO_DECLARATION_ENTRY sode;
				sode.SemanticName = signatureParameterDesc.SemanticName;
				sode.Stream = signatureParameterDesc.Stream;
				sode.OutputSlot = 0;
				sode.StartComponent = 0;
				sode.ComponentCount = varCount;
				sode.SemanticIndex = signatureParameterDesc.SemanticIndex;

				SOEntries.push_back(sode);
			}
			uint32_t bufferStrides = 0;
			for (auto& e : SOEntries)
				bufferStrides += e.ComponentCount * 4;

			SafeDXP<ID3D11GeometryShader> s;
			RETURN_IF_HR_ERROR(device->CreateGeometryShaderWithStreamOutput(data, size, SOEntries.data(),UINT( SOEntries.size()), &bufferStrides, 1, D3D11_SO_NO_RASTERIZED_STREAM, nullptr, s.Create()), "Could not create geometry shader");
			toAdd.push({ id, PipelineObjects::GeometryShader_{ s.Done(), cbuffers } });
			objects_ClientSide[PipelineObjects::GeometryShader].emplace(id);
		}
		else if (type == Pipeline::ShaderType::COMPUTE)
		{
			SafeDXP<ID3D11ComputeShader> s;
			RETURN_IF_HR_ERROR(device->CreateComputeShader(data, size, nullptr, s.Create()), "Could not create compute shader");
			toAdd.push({ id, PipelineObjects::ComputeShader_{ s.Done(), cbuffers } });
			objects_ClientSide[PipelineObjects::ComputeShader].emplace(id);
		}
		

		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::DestroyShader(Utilities::GUID id, Pipeline::ShaderType type)
	{
		StartProfile;
		uint32_t t = -1;
		if (type == Pipeline::ShaderType::VERTEX)
			t = PipelineObjects::VertexShader;
		else if (type == Pipeline::ShaderType::PIXEL)
			t = PipelineObjects::PixelShader;	
		else if (type == Pipeline::ShaderType::GEOMETRY || type == Pipeline::ShaderType::GEOMETRY_STREAM_OUT)
			t = PipelineObjects::GeometryShader;		
		else if (type == Pipeline::ShaderType::COMPUTE)
			t = PipelineObjects::ComputeShader;
	
		if (auto find = objects_ClientSide[t].find(id); find != objects_ClientSide[t].end())
		{
			objects_ClientSide[t].erase(id);
			toRemove.push({ id, t });
		}

		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::CreateTexture(Utilities::GUID id, void * data, size_t width, size_t height)
	{
		StartProfile;
		if (auto find = objects_ClientSide[PipelineObjects::ShaderResourceView].find(id); find != objects_ClientSide[PipelineObjects::ShaderResourceView].end())
			RETURN_ERROR("Texture already exists");

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = UINT(width);
		desc.Height = UINT(height);
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
		d.SysMemPitch = UINT( width * 4);
		d.SysMemSlicePitch = 0;
		RETURN_IF_HR_ERROR(device->CreateTexture2D(&desc, &d, texture.Create()), "Could not create texture");


		SafeDXP<ID3D11ShaderResourceView> srv;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		RETURN_IF_HR_ERROR(device->CreateShaderResourceView(texture.Get(), &srvDesc, srv.Create()), "Could not create shader resource view");

		objects_ClientSide[PipelineObjects::ShaderResourceView].emplace(id);
		toAdd.push({ id, PipelineObjects::ShaderResourceView_{srv.Done()} });
		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::DestroyTexture(Utilities::GUID id)
	{
		StartProfile;
		if (auto find = objects_ClientSide[PipelineObjects::ShaderResourceView].find(id); find != objects_ClientSide[PipelineObjects::ShaderResourceView].end())
		{
			objects_ClientSide[PipelineObjects::ShaderResourceView].erase(id);
			toRemove.push({ id, PipelineObjects::ShaderResourceView });
		}
		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::CreateRasterizerState(Utilities::GUID id, const Pipeline::RasterizerState & state)
	{
		StartProfile;
		if (auto find = objects_ClientSide[PipelineObjects::RasterizerState].find(id); find != objects_ClientSide[PipelineObjects::RasterizerState].end())
			RETURN_ERROR("Rasterizer already exists");

		D3D11_RASTERIZER_DESC rd;
		rd.AntialiasedLineEnable = false;
		switch (state.cullMode)
		{
		case Pipeline::CullMode::CULL_BACK: rd.CullMode = D3D11_CULL_BACK; break;
		case Pipeline::CullMode::CULL_FRONT: rd.CullMode = D3D11_CULL_FRONT; break;
		case Pipeline::CullMode::CULL_NONE: rd.CullMode = D3D11_CULL_NONE; break;
		}
		switch (state.fillMode)
		{
		case Pipeline::FillMode::FILL_SOLID:		rd.FillMode = D3D11_FILL_SOLID; break;
		case Pipeline::FillMode::FILL_WIREFRAME:  rd.FillMode = D3D11_FILL_WIREFRAME; break;
		}
		switch (state.windingOrder)
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

		ID3D11RasterizerState* rs;
		RETURN_IF_HR_ERROR(device->CreateRasterizerState(&rd, &rs), "Could not create rasterizer state");

		objects_ClientSide[PipelineObjects::RasterizerState].emplace(id);
		toAdd.push({ id, PipelineObjects::RasterizerState_{rs} });


		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::DestroyRasterizerState(Utilities::GUID id)
	{
		StartProfile;
		if (auto find = objects_ClientSide[PipelineObjects::RasterizerState].find(id); find != objects_ClientSide[PipelineObjects::RasterizerState].end())
		{
			objects_ClientSide[PipelineObjects::RasterizerState].erase(id);
			toRemove.push({ id, PipelineObjects::RasterizerState });
		}
		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::CreateBlendState(Utilities::GUID id, const Pipeline::BlendState & state)
	{
		StartProfile;
		if (auto find = objects_ClientSide[PipelineObjects::BlendState].find(id); find != objects_ClientSide[PipelineObjects::BlendState].end())
			RETURN_ERROR("BlendState already exists");


		D3D11_BLEND_DESC bd;
		bd.IndependentBlendEnable = false;

		D3D11_RENDER_TARGET_BLEND_DESC rtbd[8];
		rtbd[0].BlendEnable = state.enable;
		switch (state.blendOperation)
		{
		case Pipeline::BlendOperation::ADD: rtbd[0].BlendOp = D3D11_BLEND_OP_ADD; break;
		case Pipeline::BlendOperation::MAX: rtbd[0].BlendOp = D3D11_BLEND_OP_MAX; break;
		case Pipeline::BlendOperation::MIN: rtbd[0].BlendOp = D3D11_BLEND_OP_MIN; break;
		case Pipeline::BlendOperation::SUB: rtbd[0].BlendOp = D3D11_BLEND_OP_SUBTRACT; break;
		}

		switch (state.blendOperationAlpha)
		{
		case Pipeline::BlendOperation::ADD: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; break;
		case Pipeline::BlendOperation::MAX: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_MAX; break;
		case Pipeline::BlendOperation::MIN: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_MIN; break;
		case Pipeline::BlendOperation::SUB: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_SUBTRACT; break;
		}

		switch (state.dstBlend)
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

		switch (state.srcBlend)
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

		switch (state.dstBlendAlpha)
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

		switch (state.srcBlendAlpha)
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

		ID3D11BlendState* blendState;
		RETURN_IF_HR_ERROR(device->CreateBlendState(&bd, &blendState), "Could not create blendstate");


		objects_ClientSide[PipelineObjects::BlendState].emplace(id);
		toAdd.push({ id, PipelineObjects::BlendState_{ blendState } });

		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::DestroyBlendState(Utilities::GUID id)
	{
		StartProfile;
		if (auto find = objects_ClientSide[PipelineObjects::BlendState].find(id); find != objects_ClientSide[PipelineObjects::BlendState].end())
		{
			objects_ClientSide[PipelineObjects::BlendState].erase(id);
			toRemove.push({ id, PipelineObjects::BlendState });
		}
		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::CreateDepthStencilState(Utilities::GUID id, const Pipeline::DepthStencilState & state)
	{
		StartProfile;
		if (auto find = objects_ClientSide[PipelineObjects::DepthStencilState].find(id); find != objects_ClientSide[PipelineObjects::DepthStencilState].end())
			RETURN_ERROR("DepthStencilState already exists");

		D3D11_DEPTH_STENCIL_DESC dsd;
		dsd.DepthEnable = state.enableDepth;
		switch (state.comparisonOperation)
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

		ID3D11DepthStencilState* dss;
		RETURN_IF_HR_ERROR(device->CreateDepthStencilState(&dsd, &dss), "Could not create DepthStencilState");

		objects_ClientSide[PipelineObjects::DepthStencilState].emplace(id);
		toAdd.push({ id, PipelineObjects::DepthStencilState_{ dss } });

		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::DestroyDepthStencilState(Utilities::GUID id)
	{
		StartProfile;
		if (auto find = objects_ClientSide[PipelineObjects::DepthStencilState].find(id); find != objects_ClientSide[PipelineObjects::DepthStencilState].end())
		{
			objects_ClientSide[PipelineObjects::DepthStencilState].erase(id);
			toRemove.push({ id, PipelineObjects::DepthStencilState });
		}
		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::CreateSamplerState(Utilities::GUID id, const Pipeline::SamplerState & state)
	{
		StartProfile;
		if (auto find = objects_ClientSide[PipelineObjects::SamplerState].find(id); find != objects_ClientSide[PipelineObjects::SamplerState].end())
			RETURN_ERROR("SamplerState already exists");

		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));

		switch (state.addressU)
		{
		case Pipeline::AddressingMode::WRAP:		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; break;
		case Pipeline::AddressingMode::CLAMP:		sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP; break;
		case Pipeline::AddressingMode::MIRROR:	sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR; break;
		}
		switch (state.addressV)
		{
		case Pipeline::AddressingMode::WRAP:		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; break;
		case Pipeline::AddressingMode::CLAMP:		sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP; break;
		case Pipeline::AddressingMode::MIRROR:	sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR; break;
		}
		switch (state.addressW)
		{
		case Pipeline::AddressingMode::WRAP:		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; break;
		case Pipeline::AddressingMode::CLAMP:		sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP; break;
		case Pipeline::AddressingMode::MIRROR:	sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR; break;
		}
		switch (state.filter)
		{
		case Pipeline::Filter::ANISOTROPIC:	sd.Filter = D3D11_FILTER_ANISOTROPIC; break;
		case Pipeline::Filter::LINEAR:		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; break;
		case Pipeline::Filter::POINT:			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; break;
		}
		sd.BorderColor[0] = 0.0f; sd.BorderColor[1] = 0.0f; sd.BorderColor[2] = 0.0f; sd.BorderColor[3] = 0.0f;
		sd.MinLOD = 0;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
		sd.MaxAnisotropy = state.maxAnisotropy;
		ID3D11SamplerState* samplerState;
		RETURN_IF_HR_ERROR(device->CreateSamplerState(&sd, &samplerState), "Could not create SamplerState");

		objects_ClientSide[PipelineObjects::SamplerState].emplace(id);
		toAdd.push({ id, PipelineObjects::SamplerState_{ samplerState } });

		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::DestroySamplerState(Utilities::GUID id)
	{
		StartProfile;
		if (auto find = objects_ClientSide[PipelineObjects::SamplerState].find(id); find != objects_ClientSide[PipelineObjects::SamplerState].end())
		{
			objects_ClientSide[PipelineObjects::SamplerState].erase(id);
			toRemove.push({ id, PipelineObjects::SamplerState });
		}
		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::CreateTarget(Utilities::GUID id, const Pipeline::Target & target)
	{
		StartProfile;

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = target.width;
		desc.Height = target.height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		switch (target.format)
		{
		case Pipeline::TextureFormat::R32G32B32A32_FLOAT: desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
		case Pipeline::TextureFormat::R8G8B8A8_UNORM:		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
		}

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = 0;
		if (target.flags & Pipeline::TargetFlags::SHADER_RESOURCE) desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		if (target.flags & Pipeline::TargetFlags::UNORDERED_ACCESS) desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		if (target.flags & Pipeline::TargetFlags::RENDER_TARGET) desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		desc.CPUAccessFlags = target.flags & Pipeline::TargetFlags::CPU_ACCESS_READ ? D3D11_CPU_ACCESS_READ : 0;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		SafeDXP<ID3D11Texture2D> texture;
		RETURN_IF_HR_ERROR(device->CreateTexture2D(&desc, nullptr, texture.Create()), "Could not create texture");


		if (target.flags & Pipeline::TargetFlags::SHADER_RESOURCE)
		{
			if (auto find = objects_ClientSide[PipelineObjects::ShaderResourceView].find(id); find != objects_ClientSide[PipelineObjects::ShaderResourceView].end())
				RETURN_ERROR("Texture already exists");

			ID3D11ShaderResourceView* srv;
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = desc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;
			RETURN_IF_HR_ERROR(device->CreateShaderResourceView(texture.Get(), &srvDesc, &srv), "Could not create ShaderResourceView");

			objects_ClientSide[PipelineObjects::ShaderResourceView].emplace(id);
			toAdd.push({ id, PipelineObjects::ShaderResourceView_{ srv} });
		}
		if (target.flags & Pipeline::TargetFlags::UNORDERED_ACCESS)
		{
			if (auto find = objects_ClientSide[PipelineObjects::UnorderedAccessView].find(id); find != objects_ClientSide[PipelineObjects::UnorderedAccessView].end())
				RETURN_ERROR("UnorderedAccessView already exists");

			D3D11_UNORDERED_ACCESS_VIEW_DESC description;
			ZeroMemory(&description, sizeof(description));
			description.Format = desc.Format;
			description.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			description.Texture2D.MipSlice = 0;

			ID3D11UnorderedAccessView* unorderedAccessView;
			RETURN_IF_HR_ERROR(device->CreateUnorderedAccessView(texture.Get(), &description, &unorderedAccessView), "Could not create UnorderedAccessView");
			
			objects_ClientSide[PipelineObjects::UnorderedAccessView].emplace(id);
			toAdd.push({ id, PipelineObjects::UnorderedAccessView_{ unorderedAccessView, { target.clearColor[0] , target.clearColor[1], target.clearColor[2], target.clearColor[3] } } });
		}
	

		if (target.flags & Pipeline::TargetFlags::RENDER_TARGET)
		{
			if (auto find = objects_ClientSide[PipelineObjects::RenderTarget].find(id); find != objects_ClientSide[PipelineObjects::RenderTarget].end())
				RETURN_ERROR("RenderTarget already exists");

			D3D11_RENDER_TARGET_VIEW_DESC rtvd;
			rtvd.Format = desc.Format;
			rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvd.Texture2D.MipSlice = 0;
			ID3D11RenderTargetView* rtv;
			RETURN_IF_HR_ERROR(device->CreateRenderTargetView(texture.Get(), &rtvd, &rtv), "Could not create RenderTarget");

			objects_ClientSide[PipelineObjects::RenderTarget].emplace(id);
			toAdd.push({ id, PipelineObjects::RenderTarget_{ rtv,{ target.clearColor[0] , target.clearColor[1], target.clearColor[2], target.clearColor[3] } } });
		}



		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::DestroyTarget(Utilities::GUID id)
	{
		if (auto find = objects_ClientSide[PipelineObjects::RenderTarget].find(id); find != objects_ClientSide[PipelineObjects::RenderTarget].end())
		{
			objects_ClientSide[PipelineObjects::RenderTarget].erase(id);
			toRemove.push({ id, PipelineObjects::RenderTarget });
		}
		if (auto find = objects_ClientSide[PipelineObjects::UnorderedAccessView].find(id); find != objects_ClientSide[PipelineObjects::UnorderedAccessView].end())
		{
			objects_ClientSide[PipelineObjects::UnorderedAccessView].erase(id);
			toRemove.push({ id, PipelineObjects::UnorderedAccessView });
		}
		if (auto find = objects_ClientSide[PipelineObjects::ShaderResourceView].find(id); find != objects_ClientSide[PipelineObjects::ShaderResourceView].end())
		{
			objects_ClientSide[PipelineObjects::ShaderResourceView].erase(id);
			toRemove.push({ id, PipelineObjects::ShaderResourceView });
		}
		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::CreateDepthStencilView(Utilities::GUID id, const Pipeline::DepthStencilView & view)
	{
		if (auto find = objects_ClientSide[PipelineObjects::DepthStencilView].find(id); find != objects_ClientSide[PipelineObjects::DepthStencilView].end())
			RETURN_ERROR("DepthStencilView already exists");

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = UINT(view.width);
		desc.Height =  UINT(view.height);
		desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		if (view.flags & Pipeline::DepthStencilViewFlags::SHADER_RESOURCE) desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
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

		 SafeDXP<ID3D11Texture2D> texture;

		RETURN_IF_HR_ERROR(device->CreateTexture2D(&desc, nullptr, texture.Create()), "Could not create texture");

		if (view.flags & Pipeline::DepthStencilViewFlags::SHADER_RESOURCE)
		{
			if (auto find = objects_ClientSide[PipelineObjects::ShaderResourceView].find(id); find != objects_ClientSide[PipelineObjects::ShaderResourceView].end())
				RETURN_ERROR("Texture already exists");

			D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
			srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvd.Texture2D.MostDetailedMip = 0;
			srvd.Texture2D.MipLevels = 1;
			ID3D11ShaderResourceView* srv;
			RETURN_IF_HR_ERROR(device->CreateShaderResourceView(texture.Get(), &srvd, &srv), "Could not create ShaderResourceView");
			objects_ClientSide[PipelineObjects::ShaderResourceView].emplace(id);
			toAdd.push({ id, PipelineObjects::ShaderResourceView_{ srv } });
		}

		ID3D11DepthStencilView* dsv;
		RETURN_IF_HR_ERROR(device->CreateDepthStencilView(texture.Get(), &dsvd, &dsv), "Could not create depthstencilview");

		objects_ClientSide[PipelineObjects::DepthStencilView].emplace(id);
		toAdd.push({ id, PipelineObjects::DepthStencilView_{ dsv } });

		RETURN_SUCCESS;
	}
	UERROR PipelineHandler::DestroyDepthStencilView(Utilities::GUID id)
	{
		if (auto find = objects_ClientSide[PipelineObjects::DepthStencilView].find(id); find != objects_ClientSide[PipelineObjects::DepthStencilView].end())
		{
			objects_ClientSide[PipelineObjects::DepthStencilView].erase(id);
			toRemove.push({ id, PipelineObjects::DepthStencilView });
		}
		if (auto find = objects_ClientSide[PipelineObjects::ShaderResourceView].find(id); find != objects_ClientSide[PipelineObjects::ShaderResourceView].end())
		{
			objects_ClientSide[PipelineObjects::ShaderResourceView].erase(id);
			toRemove.push({ id, PipelineObjects::ShaderResourceView });
		}
		RETURN_SUCCESS;
	}

	UERROR PipelineHandler::UpdatePipelineObjects()
	{
		StartProfile;
		while (!toRemove.wasEmpty())
		{
			auto& t = toRemove.top();
			objects_RenderSide[t.type].erase(t.id);

			toRemove.pop();
		}
		while (!toAdd.wasEmpty())
		{
			auto& t = toAdd.top();
			objects_RenderSide[t.obj.index()].emplace(t.id, std::move(t.obj));

			toAdd.pop();
		}
		RETURN_SUCCESS;
	}
	UpdateObject * PipelineHandler::GetUpdateObject(Utilities::GUID id, PipelineObjectType type)
	{
		if(type == PipelineObjectType::Buffer)
		if (auto find = objects_RenderSide[PipelineObjects::Buffer].find(id); find != objects_RenderSide[PipelineObjects::Buffer].end())
			return new Buffer_UO(context, std::get<PipelineObjects::Buffer_>(find->second).obj, std::get<PipelineObjects::Buffer_>(find->second).buffer);
	
		return nullptr;
	}
}