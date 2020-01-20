#ifndef _GRAPHICS_PIPELINE_OBJECTS_H_
#define _GRAPHICS_PIPELINE_OBJECTS_H_
#include <stdint.h>
#include <d3d11.h>
#include <variant>
#include <Graphics\Pipeline.h>

template <>
struct ::std::default_delete<ID3D11Buffer>{
	default_delete() = default;
	template <class U, class = std::enable_if_t<std::is_convertible<U*, ID3D11Buffer*>()>>
	constexpr default_delete( default_delete<U> ) noexcept\
	{}
	void operator()( ID3D11Buffer* p ) const noexcept
	{
		p->Release();
	}
};
#define MAKE_RELEASEABLE_STRUCT(name, objtype, ...)\
static constexpr uint32_t name = __COUNTER__ - offset - 1;\
struct name##_\
{\
objtype* obj;\
void Release()\
{\
if (obj)\
obj->Release();\
obj=nullptr;\
this->~##name##_();\
}\
~name##_()\
{\
}\
 __VA_ARGS__  }
#define MAKE_RELEASEABLE_STRUCT_DOUBLE(name, objtype, objtype2, ...)\
static constexpr uint32_t name = __COUNTER__ - offset - 1;\
struct name##_\
{\
objtype * obj = nullptr;\
objtype2 * obj2 = nullptr;\
void Release()\
{\
if (obj)\
obj->Release();\
obj=nullptr;\
if (obj2)\
obj2->Release();\
obj2=nullptr;\
this->~##name##_();\
}\
~name##_()\
{\
}\
 __VA_ARGS__  }
#define MAKE_SIMPLE_STRUCT(name, objtype)\
static constexpr uint32_t name = __COUNTER__ - offset - 1;\
struct name##_\
{\
void Release()\
{}\
objtype obj; }
#define IF_CEXPR_A(T, name)\
if constexpr(std::is_same<T, PipelineObjects::##name##_>::value)\
{\
	new (this) PipelineObjects::##name##_();\
	_##name = std::move(a);\
	type = PipelineObjects::##name;\
}
#define IF_CEXPR_R(t, name)\
if (t == PipelineObjects::##name)\
{\
if(type != -1)\
_##name.Release(); \
	type = -1;\
}
#define IF_CEXPR_C(t, name)\
if (t == PipelineObjects::##name)\
{\
	_##name = o._##name;\
}
#define PO_GET(name)	const PipelineObjects::##name##_& name ()const\
			{\
				return _##name;\
			}

namespace Graphics
{
	struct ShaderResourceToAndBindSlot{
		Utilities::GUID id;
		UINT binding;
	};
	struct PipelineObjects{
		static constexpr uint32_t offset = __COUNTER__;


		MAKE_RELEASEABLE_STRUCT( Buffer, ID3D11Buffer, Pipeline::Buffer buffer;);

		MAKE_RELEASEABLE_STRUCT_DOUBLE( VertexShader, ID3D11VertexShader, ID3D11InputLayout, std::vector<ShaderResourceToAndBindSlot> constantBuffers;);
		MAKE_RELEASEABLE_STRUCT( GeometryShader, ID3D11GeometryShader, std::vector<ShaderResourceToAndBindSlot> constantBuffers;);
		MAKE_RELEASEABLE_STRUCT( PixelShader, ID3D11PixelShader, std::vector<ShaderResourceToAndBindSlot> constantBuffers;);
		MAKE_RELEASEABLE_STRUCT( ComputeShader, ID3D11ComputeShader, std::vector<ShaderResourceToAndBindSlot> constantBuffers;);

		MAKE_RELEASEABLE_STRUCT( RenderTarget, ID3D11RenderTargetView, float clearColor[4];);
		MAKE_RELEASEABLE_STRUCT( UnorderedAccessView, ID3D11UnorderedAccessView, float clearColor[4];);
		MAKE_RELEASEABLE_STRUCT( ShaderResourceView, ID3D11ShaderResourceView );
		MAKE_RELEASEABLE_STRUCT( DepthStencilView, ID3D11DepthStencilView );

		MAKE_RELEASEABLE_STRUCT( SamplerState, ID3D11SamplerState );
		MAKE_RELEASEABLE_STRUCT( BlendState, ID3D11BlendState );
		MAKE_RELEASEABLE_STRUCT( RasterizerState, ID3D11RasterizerState );
		MAKE_RELEASEABLE_STRUCT( DepthStencilState, ID3D11DepthStencilState );

		MAKE_SIMPLE_STRUCT( Viewport, D3D11_VIEWPORT );

		static constexpr uint32_t NUM_TYPES = __COUNTER__ - offset - 1;



	};

	using PipelineObject = std::variant<
		PipelineObjects::Buffer_,
		PipelineObjects::VertexShader_,
		PipelineObjects::GeometryShader_,
		PipelineObjects::PixelShader_,
		PipelineObjects::ComputeShader_,
		PipelineObjects::RenderTarget_,
		PipelineObjects::UnorderedAccessView_,
		PipelineObjects::ShaderResourceView_,
		PipelineObjects::DepthStencilView_,
		PipelineObjects::SamplerState_,
		PipelineObjects::BlendState_,
		PipelineObjects::RasterizerState_,
		PipelineObjects::DepthStencilState_,
		PipelineObjects::Viewport_
	>;

	/*struct PipelineObject
	{
		uint32_t type;


		void Release()
		{
			IF_CEXPR_R(type, VertexBuffer)
			else IF_CEXPR_R(type, IndexBuffer)
			else IF_CEXPR_R(type, ConstantBuffer)
			else IF_CEXPR_R(type, StructuredBuffer)
			else IF_CEXPR_R(type, RawBuffer)

			else IF_CEXPR_R(type, InputLayout)
			else IF_CEXPR_R(type, VertexShader)
			else IF_CEXPR_R(type, GeometryShader)
			else IF_CEXPR_R(type, PixelShader)
			else IF_CEXPR_R(type, ComputeShader)

			else IF_CEXPR_R(type, RenderTarget)
			else IF_CEXPR_R(type, UnorderedAccessView)
			else IF_CEXPR_R(type, ShaderResourceView)
			else IF_CEXPR_R(type, DepthStencilView)

			else IF_CEXPR_R(type, SamplerState)
			else IF_CEXPR_R(type, BlendState)
			else IF_CEXPR_R(type, RasterizerState)
			else IF_CEXPR_R(type, DepthStencilState)

			else IF_CEXPR_R(type, Viewport);
		}

		PipelineObject() : type(-1)
		{

		}

		template<class T>
		PipelineObject(T&& a)
		{
			IF_CEXPR_A(T, VertexBuffer);
			IF_CEXPR_A(T, IndexBuffer);
			IF_CEXPR_A(T, ConstantBuffer);
			IF_CEXPR_A(T, StructuredBuffer);
			IF_CEXPR_A(T, RawBuffer);

			IF_CEXPR_A(T, InputLayout);
			IF_CEXPR_A(T, VertexShader);
			IF_CEXPR_A(T, GeometryShader);
			IF_CEXPR_A(T, PixelShader);
			IF_CEXPR_A(T, ComputeShader);

			IF_CEXPR_A(T, RenderTarget);
			IF_CEXPR_A(T, UnorderedAccessView);
			IF_CEXPR_A(T, ShaderResourceView);
			IF_CEXPR_A(T, DepthStencilView);

			IF_CEXPR_A(T, SamplerState);
			IF_CEXPR_A(T, BlendState);
			IF_CEXPR_A(T, RasterizerState);
			IF_CEXPR_A(T, DepthStencilState);

			IF_CEXPR_A(T, Viewport);
		}
		PipelineObject(const PipelineObject& o) = delete;
		PipelineObject& operator=(PipelineObject&& o)
		{
			this->type = o.type;
			IF_CEXPR_C(o.type, VertexBuffer)
			else IF_CEXPR_C(o.type, IndexBuffer)
			else IF_CEXPR_C(o.type, ConstantBuffer)
			else IF_CEXPR_C(o.type, StructuredBuffer)
			else IF_CEXPR_C(o.type, RawBuffer)

			else IF_CEXPR_C(o.type, InputLayout)
			else IF_CEXPR_C(o.type, VertexShader)
			else IF_CEXPR_C(o.type, GeometryShader)
			else IF_CEXPR_C(o.type, PixelShader)
			else IF_CEXPR_C(o.type, ComputeShader)

			else IF_CEXPR_C(o.type, RenderTarget)
			else IF_CEXPR_C(o.type, UnorderedAccessView)
			else IF_CEXPR_C(o.type, ShaderResourceView)
			else IF_CEXPR_C(o.type, DepthStencilView)

			else IF_CEXPR_C(o.type, SamplerState)
			else IF_CEXPR_C(o.type, BlendState)
			else IF_CEXPR_C(o.type, RasterizerState)
			else IF_CEXPR_C(o.type, DepthStencilState)

			else IF_CEXPR_C(o.type, Viewport);
			return *this;
		}
		PipelineObject(PipelineObject&& o)
		{
			this->type = o.type;
			if (type != -1)
			{
				IF_CEXPR_C(o.type, VertexBuffer)
				else IF_CEXPR_C(o.type, IndexBuffer)
				else IF_CEXPR_C(o.type, ConstantBuffer)
				else IF_CEXPR_C(o.type, StructuredBuffer)
				else IF_CEXPR_C(o.type, RawBuffer)

				else IF_CEXPR_C(o.type, InputLayout)
				else IF_CEXPR_C(o.type, VertexShader)
				else IF_CEXPR_C(o.type, GeometryShader)
				else IF_CEXPR_C(o.type, PixelShader)
				else IF_CEXPR_C(o.type, ComputeShader)

				else IF_CEXPR_C(o.type, RenderTarget)
				else IF_CEXPR_C(o.type, UnorderedAccessView)
				else IF_CEXPR_C(o.type, ShaderResourceView)
				else IF_CEXPR_C(o.type, DepthStencilView)

				else IF_CEXPR_C(o.type, SamplerState)
				else IF_CEXPR_C(o.type, BlendState)
				else IF_CEXPR_C(o.type, RasterizerState)
				else IF_CEXPR_C(o.type, DepthStencilState)

				else IF_CEXPR_C(o.type, Viewport);
			}
		}

		PO_GET(VertexBuffer);
		PO_GET(IndexBuffer);
		PO_GET(ConstantBuffer);
		PO_GET(StructuredBuffer);
		PO_GET(RawBuffer);

		PO_GET(InputLayout);
		PO_GET(VertexShader);
		PO_GET(GeometryShader);
		PO_GET(PixelShader);
		PO_GET(ComputeShader);

		PO_GET(RenderTarget);
		PO_GET(UnorderedAccessView);
		PO_GET(ShaderResourceView);
		PO_GET(DepthStencilView);

		PO_GET(SamplerState);
		PO_GET(BlendState);
		PO_GET(RasterizerState);
		PO_GET(DepthStencilState);

		PO_GET(Viewport);

		~PipelineObject()
		{

		}
	private:
		union
		{
			PipelineObjects::VertexBuffer_ _VertexBuffer;
			PipelineObjects::IndexBuffer_ _IndexBuffer;
			PipelineObjects::ConstantBuffer_ _ConstantBuffer;
			PipelineObjects::StructuredBuffer_ _StructuredBuffer;
			PipelineObjects::RawBuffer_ _RawBuffer;

			PipelineObjects::InputLayout_ _InputLayout;
			PipelineObjects::VertexShader_ _VertexShader;
			PipelineObjects::GeometryShader_ _GeometryShader;
			PipelineObjects::PixelShader_ _PixelShader;
			PipelineObjects::ComputeShader_ _ComputeShader;

			PipelineObjects::RenderTarget_ _RenderTarget;
			PipelineObjects::UnorderedAccessView_ _UnorderedAccessView;
			PipelineObjects::ShaderResourceView_ _ShaderResourceView;
			PipelineObjects::DepthStencilView_ _DepthStencilView;

			PipelineObjects::SamplerState_ _SamplerState;
			PipelineObjects::BlendState_ _BlendState;
			PipelineObjects::RasterizerState_ _RasterizerState;
			PipelineObjects::DepthStencilState_ _DepthStencilState;

			PipelineObjects::Viewport_ _Viewport;
		};
	};*/
}
#endif