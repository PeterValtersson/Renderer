#include "PipelineHandler.h"
#include <Profiler.h>
#include "UpdateObjects.h"
#include <d3dcompiler.h>

#pragma comment(lib, "D3Dcompiler.lib")
#pragma comment(lib, "dxguid.lib")


#define EMPLACE_NULL(name) objects_RenderSide[PipelineObjects::##name].emplace(Utilz::GUID(), PipelineObjects::##name##_{ nullptr })
#define EMPLACE_DEF(name) objects_RenderSide[PipelineObjects::##name].emplace(Utilz::GUID(), PipelineObjects::##name##_{ })
namespace Graphics
{
	PipelineHandler::PipelineHandler()
	{
	}


	PipelineHandler::~PipelineHandler()
	{
	}

	GRAPHICS_ERROR PipelineHandler::Init(
		ID3D11Device * device, ID3D11DeviceContext * context, 
		ID3D11RenderTargetView * backbuffer, ID3D11DepthStencilView * dsv,
		const D3D11_VIEWPORT& vp)
	{
		StartProfile;
		this->device = device;
		this->context = context;
		

		objects_RenderSide[PipelineObjects::RenderTarget].emplace(Default_RenderTarget, PipelineObjects::RenderTarget_{ backbuffer,{ 0.0f, 0.0f,1.0f,0.0f } });
		objects_RenderSide[PipelineObjects::DepthStencilView].emplace(Default_DepthStencil, PipelineObjects::DepthStencilView_{ dsv });
		objects_RenderSide[PipelineObjects::Viewport].emplace(Default_Viewport, PipelineObjects::Viewport_{ vp});
		objects_ClientSide[PipelineObjects::RenderTarget].emplace(Default_RenderTarget);
		objects_ClientSide[PipelineObjects::DepthStencilView].emplace(Default_DepthStencil);
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

		RETURN_GRAPHICS_SUCCESS;
	}
#define RELEASE_PLO(type) if(o.second.index() == type) std::get<type##_>(o.second).Release()
	void PipelineHandler::Shutdown()
	{
		objects_RenderSide[PipelineObjects::RenderTarget].erase(Default_RenderTarget);
		objects_RenderSide[PipelineObjects::DepthStencilView].erase(Default_DepthStencil);

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

	GRAPHICS_ERROR PipelineHandler::CreateBuffer(Utilz::GUID id, const Pipeline::Buffer & buffer)
	{
		StartProfile;
		if (buffer.flags & Pipeline::BufferFlags::BIND_CONSTANT)
		{
			if (buffer.elementStride % 16 != 0)
				RETURN_GRAPHICS_ERROR_C("Constant buffer memory must be a multiple of 16 bytes");
		}

		if (auto find = objects_ClientSide[PipelineObjects::Buffer].find(id); find != objects_ClientSide[PipelineObjects::Buffer].end())
		{
			RETURN_GRAPHICS_ERROR("Buffer already exists", 1);
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

		RETURN_IF_GRAPHICS_ERROR(hr, "Could not create buffer. Likely due to incompatible flags");

		toAdd.push({ id, PipelineObjects::Buffer_{ pBuffer, buffer }});

		objects_ClientSide[PipelineObjects::Buffer].emplace(id);
		
		RETURN_GRAPHICS_SUCCESS;
	}

	GRAPHICS_ERROR PipelineHandler::DestroyBuffer(Utilz::GUID id)
	{
		sizeof(PipelineObject);
		if (auto find = objects_ClientSide[PipelineObjects::Buffer].find(id); find != objects_ClientSide[PipelineObjects::Buffer].end())
		{
			objects_ClientSide[PipelineObjects::Buffer].erase(id);
			toRemove.push({ id,PipelineObjects::Buffer });			
		}
		
		RETURN_GRAPHICS_SUCCESS;
	}
	
	GRAPHICS_ERROR PipelineHandler::CreateViewport(Utilz::GUID id, const Pipeline::Viewport & viewport)
	{
		if (auto find = objects_ClientSide[PipelineObjects::Viewport].find(id); find != objects_ClientSide[PipelineObjects::Viewport].end())
			RETURN_GRAPHICS_ERROR_C("Viewport with name already exists");


		objects_ClientSide[PipelineObjects::Viewport].emplace(id);

		toAdd.push({ id , PipelineObjects::Viewport_{ viewport.topLeftX, viewport.topLeftY, viewport.width, viewport.height, viewport.minDepth, viewport.maxDepth } });

		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateShader(Utilz::GUID id, Pipeline::ShaderType type, const char * sourceCode, size_t size, const char * entryPoint, const char * shaderModel)
	{
		ID3DBlob* blob;
		ID3DBlob* error;
		RETURN_IF_GRAPHICS_ERROR_S(D3DCompile(sourceCode, size, NULL, NULL, NULL, entryPoint, shaderModel, 0, 0, &blob, &error), (char*) error);

		return CreateShader(id, type, blob->GetBufferPointer(), blob->GetBufferSize());
	}
	template<class T>
	struct SafeDXP
	{
		T* operator->()
		{
			return obj;
		}
		T** Get()
		{
			return &obj;
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
	GRAPHICS_ERROR PipelineHandler::CreateShader(Utilz::GUID id, Pipeline::ShaderType type, void * data, size_t size)
	{
		if (type == Pipeline::ShaderType::VERTEX)
		{
			if (auto find = objects_ClientSide[PipelineObjects::VertexShader].find(id); find != objects_ClientSide[PipelineObjects::VertexShader].end())
				RETURN_GRAPHICS_ERROR_C("Vertex shader with name already exists");
		}
		else if (type == Pipeline::ShaderType::PIXEL)
		{
			if (auto find = objects_ClientSide[PipelineObjects::PixelShader].find(id); find != objects_ClientSide[PipelineObjects::PixelShader].end())
				RETURN_GRAPHICS_ERROR_C("Pixel shader with name already exists");
		}
		else if (type == Pipeline::ShaderType::GEOMETRY || type == Pipeline::ShaderType::GEOMETRY_STREAM_OUT)
		{
			if (auto find = objects_ClientSide[PipelineObjects::GeometryShader].find(id); find != objects_ClientSide[PipelineObjects::GeometryShader].end())
				RETURN_GRAPHICS_ERROR_C("Geometry shader with name already exists");
		}
		else if (type == Pipeline::ShaderType::COMPUTE)
		{
			if (auto find = objects_ClientSide[PipelineObjects::ComputeShader].find(id); find != objects_ClientSide[PipelineObjects::ComputeShader].end())
				RETURN_GRAPHICS_ERROR_C("Compute shader with name already exists");
		}
		else
		{
			RETURN_GRAPHICS_ERROR_C("Shader type not supported yet");
		}
		//Create the input layout with the help of shader reflection
		SafeDXP<ID3D11ShaderReflection> reflection;

		RETURN_IF_GRAPHICS_ERROR(D3DReflect(data, size, IID_ID3D11ShaderReflection, (void**)reflection.Get()), "Could not reflect shader");
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
				Utilz::GUID sibdName = std::string(sibd.Name);
				for (unsigned int j = 0; j < shaderDesc.ConstantBuffers; ++j)
				{
					D3D11_SHADER_BUFFER_DESC sbd;
					ID3D11ShaderReflectionConstantBuffer* srcb = reflection->GetConstantBufferByIndex(j);
					srcb->GetDesc(&sbd);
					Utilz::GUID name = std::string(sbd.Name);
					if (name == sibdName)
					{
						PASS_IF_GRAPHICS_ERROR(CreateBuffer(name, Pipeline::Buffer::ConstantBuffer(uint16_t(sbd.Size))));
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
			

			RETURN_IF_GRAPHICS_ERROR(device->CreateVertexShader(data, size, nullptr, vs.Get()), "Could not create vertex shader");


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
				RETURN_IF_GRAPHICS_ERROR(device->CreateInputLayout(inputElementDescs.data(), UINT(inputElementDescs.size()), data, size, inputLayout.Get()), "Could not create input layout");

			toAdd.push({ id, PipelineObjects::VertexShader_{vs.Done(), inputLayout.Done(), cbuffers} });
			objects_ClientSide[PipelineObjects::VertexShader].emplace(id);
		}
		else if (type == Pipeline::ShaderType::PIXEL)
		{
			SafeDXP<ID3D11PixelShader> s;
			RETURN_IF_GRAPHICS_ERROR(device->CreatePixelShader(data, size, nullptr, s.Get()), "Could not create pixel shader");
			toAdd.push({ id, PipelineObjects::PixelShader_{ s.Done(), cbuffers } });
			objects_ClientSide[PipelineObjects::PixelShader].emplace(id);
		}
		else if (type == Pipeline::ShaderType::GEOMETRY)
		{
			SafeDXP<ID3D11GeometryShader> s;
			RETURN_IF_GRAPHICS_ERROR(device->CreateGeometryShader(data, size, nullptr, s.Get()), "Could not create geometry shader");
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
			RETURN_IF_GRAPHICS_ERROR(device->CreateGeometryShaderWithStreamOutput(data, size, SOEntries.data(),UINT( SOEntries.size()), &bufferStrides, 1, D3D11_SO_NO_RASTERIZED_STREAM, nullptr, s.Get()), "Could not create geometry shader");
			toAdd.push({ id, PipelineObjects::GeometryShader_{ s.Done(), cbuffers } });
			objects_ClientSide[PipelineObjects::GeometryShader].emplace(id);
		}
		else if (type == Pipeline::ShaderType::COMPUTE)
		{
			SafeDXP<ID3D11ComputeShader> s;
			RETURN_IF_GRAPHICS_ERROR(device->CreateComputeShader(data, size, nullptr, s.Get()), "Could not create compute shader");
			toAdd.push({ id, PipelineObjects::ComputeShader_{ s.Done(), cbuffers } });
			objects_ClientSide[PipelineObjects::ComputeShader].emplace(id);
		}
		

		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyShader(Utilz::GUID id, Pipeline::ShaderType type)
	{
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

		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateTexture(Utilz::GUID id, void * data, size_t width, size_t height)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyTexture(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateRasterizerState(Utilz::GUID id, const Pipeline::RasterizerState & state)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyRasterizerState(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateBlendState(Utilz::GUID id, const Pipeline::BlendState & state)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyBlendState(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateDepthStencilState(Utilz::GUID id, const Pipeline::DepthStencilState & state)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyDepthStencilState(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateSamplerState(Utilz::GUID id, const Pipeline::SamplerState & state)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroySamplerState(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateRenderTarget(Utilz::GUID id, const Pipeline::RenderTarget & target)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyRenderTarget(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateDepthStencilView(Utilz::GUID id, const Pipeline::DepthStencilView & view)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyDepthStencilView(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateUnorderedAccessView(Utilz::GUID id, const Pipeline::UnorderedAccessView & view)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyUnorderedAccessView(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::UpdatePipelineObjects()
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
		RETURN_GRAPHICS_SUCCESS;
	}
	UpdateObject * PipelineHandler::GetUpdateObject(Utilz::GUID id, PipelineObjectType type)
	{
		if(type == PipelineObjectType::Buffer)
		if (auto find = objects_RenderSide[PipelineObjects::Buffer].find(id); find != objects_RenderSide[PipelineObjects::Buffer].end())
			return new Buffer_UO(context, std::get<PipelineObjects::Buffer_>(find->second).obj, std::get<PipelineObjects::Buffer_>(find->second).buffer);
	
		return nullptr;
	}
}