#include "PipelineHandler.h"
#include <Profiler.h>
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

		EMPLACE_NULL(PipelineObjects::VertexBuffer);
		EMPLACE_NULL(PipelineObjects::IndexBuffer);
		EMPLACE_NULL(PipelineObjects::ConstantBuffer);
		EMPLACE_NULL(PipelineObjects::StructuredBuffer);
		EMPLACE_NULL(PipelineObjects::RawBuffer);

		EMPLACE_NULL(PipelineObjects::InputLayout);
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

	void PipelineHandler::Shutdown()
	{
		objects_RenderSide[PipelineObjects::RenderTarget].erase(Default_RenderTarget);
		objects_RenderSide[PipelineObjects::DepthStencilView].erase(Default_DepthStencil);

		for (auto& ot : objects_RenderSide)
			for (auto& o : ot)
				o.second.Release();
	}

	GRAPHICS_ERROR PipelineHandler::CreateBuffer(Utilz::GUID id, const Pipeline::Buffer & buffer)
	{
		StartProfile;
		uint32_t type = -1;
		if (buffer.flags & Pipeline::BufferFlags::BIND_VERTEX)
			type = PipelineObjects::VertexBuffer;
		else if (buffer.flags & Pipeline::BufferFlags::BIND_INDEX)
			type = PipelineObjects::IndexBuffer;
		else if (buffer.flags & Pipeline::BufferFlags::BIND_CONSTANT)
		{
			type = PipelineObjects::ConstantBuffer;
			if (buffer.elementStride % 16 != 0)
				RETURN_GRAPHICS_ERROR_C("Constant buffer memory must be a multiple of 16 bytes");
		}
		
		else
			RETURN_GRAPHICS_ERROR_C("Buffer must have a type");
		if (auto find = objects_ClientSide[type].find(id); find != objects_ClientSide[type].end())
		{
			RETURN_GRAPHICS_ERROR_C("Buffer already exists");
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

		if (type == PipelineObjects::VertexBuffer) {
			object = PipelineObjects::VertexBuffer_{ pBuffer, buffer.elementStride };
		}
		else if (type == PipelineObjects::IndexBuffer)
		{
			object = PipelineObjects::IndexBuffer_{ pBuffer, buffer.elementStride };
		}
		else if (type == PipelineObjects::ConstantBuffer)
		{
			object = PipelineObjects::ConstantBuffer_{ pBuffer };
		}
		else if (type == PipelineObjects::StructuredBuffer)
		{
			object = PipelineObjects::StructuredBuffer_{ pBuffer, buffer.elementStride };
		}

		toAdd.push({ id, std::move(object) });

		objects_ClientSide[type].emplace(id);
		
		RETURN_GRAPHICS_SUCCESS;
	}

	GRAPHICS_ERROR PipelineHandler::UpdateBuffer(Utilz::GUID id, void * data, size_t size)
	{
		
		RETURN_GRAPHICS_SUCCESS;
	}

	GRAPHICS_ERROR PipelineHandler::UpdateBuffer(Utilz::GUID id, const std::function<void(void*mappedResource)>& mapCallback)
	{
		RETURN_GRAPHICS_SUCCESS;
	}

	GRAPHICS_ERROR PipelineHandler::DestroyBuffer(Utilz::GUID id)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	
	GRAPHICS_ERROR PipelineHandler::CreateViewport(Utilz::GUID id, const Pipeline::Viewport & viewport)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateShader(Utilz::GUID id, Pipeline::ShaderType type, const char * sourceCode, const char * entryPoint, const char * shaderModel)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::CreateShader(Utilz::GUID id, Pipeline::ShaderType type, void * data, size_t size)
	{
		RETURN_GRAPHICS_SUCCESS;
	}
	GRAPHICS_ERROR PipelineHandler::DestroyShader(Utilz::GUID id)
	{
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
		while (!toAdd.wasEmpty())
		{
			auto& t = toAdd.top();
			objects_RenderSide[t.obj.type].emplace(t.id, std::move(t.obj));

			toAdd.pop();
		}
		RETURN_GRAPHICS_SUCCESS;
	}
}