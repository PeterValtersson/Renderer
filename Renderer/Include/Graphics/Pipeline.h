#ifndef SE_GRAPHICS_PIPELINE_H_
#define SE_GRAPHICS_PIPELINE_H_
#include <cstdint>
#include <GUID.h>
#include <type_traits>
#include <functional>

#ifndef ENUM_FLAG_OPERATOR
#define ENUM_FLAG_OPERATOR(T,X) inline T operator X (T lhs, T rhs) { return (T) (static_cast<std::underlying_type_t <T>>(lhs) X static_cast<std::underlying_type_t <T>>(rhs)); } 
#endif
#ifndef ENUM_FLAG_OPERATOR2
#define ENUM_FLAG_OPERATOR2(T,X) inline void operator X= (T& lhs, T rhs) { lhs = (T)(static_cast<std::underlying_type_t <T>>(lhs) X static_cast<std::underlying_type_t <T>>(rhs)); } 
#endif
#ifndef ENUM_FLAGS
#define ENUM_FLAGS(T) \
inline T operator ~ (T t) { return (T) (~static_cast<std::underlying_type_t <T>>(t)); } \
inline bool operator & (T lhs, T rhs) { return (static_cast<std::underlying_type_t <T>>(lhs) & static_cast<std::underlying_type_t <T>>(rhs));  } \
ENUM_FLAG_OPERATOR2(T,|) \
ENUM_FLAG_OPERATOR2(T,&) \
ENUM_FLAG_OPERATOR(T,|) \
ENUM_FLAG_OPERATOR(T,^)
#endif

namespace Graphics
{
	static const Utilz::GUID Default_RenderTarget("Backbuffer");
	static const Utilz::GUID Default_Viewport("FullscreenViewPort");
	static const Utilz::GUID Default_DepthStencil("BackbufferDepthStencil");
	static const Utilz::GUID Default_VertexShader_FullscreenQUAD("FullscreenQUADVS");
	static const Utilz::GUID Default_PixelShader_POS_TEXTURE_MULTICHANNGEL("MultichannelPixelShader");
	static const Utilz::GUID Default_PixelShader_POS_TEXTURE_SingleCHANNGEL("SinglechannelPixelShader");
	namespace Pipeline
	{
		enum class CullMode
		{
			CULL_FRONT,
			CULL_BACK,
			CULL_NONE
		};
		enum class WindingOrder
		{
			CLOCKWISE,
			COUNTERCLOCKWISE
		};
		enum class FillMode
		{
			FILL_SOLID,
			FILL_WIREFRAME
		};
		struct RasterizerState
		{
			CullMode cullMode;
			WindingOrder windingOrder;
			FillMode fillMode;
		};

		enum class BlendOperation
		{
			ADD,
			SUB,
			MAX,
			MIN
		};

		enum class Blend
		{
			ZERO,
			ONE,
			SRC_COLOR,
			INV_SRC_COLOR,
			SRC_ALPHA,
			INV_SRC_ALPHA,
			DEST_ALPHA,
			INV_DEST_ALPHA,
			DEST_COLOR,
			INV_DEST_COLOR,
			BLEND_FACTOR
		};

		struct BlendState
		{
			bool enable = false;
			BlendOperation blendOperation = BlendOperation::ADD;
			BlendOperation blendOperationAlpha = BlendOperation::ADD;
			Blend srcBlend = Blend::SRC_ALPHA;
			Blend dstBlend = Blend::DEST_ALPHA;
			Blend srcBlendAlpha = Blend::SRC_ALPHA;
			Blend dstBlendAlpha = Blend::DEST_ALPHA;
		};

		enum class ComparisonOperation
		{
			LESS,
			LESS_EQUAL,
			EQUAL,
			GREATER,
			GREATER_EQUAL,
			NO_COMPARISON
		};

		struct DepthStencilState
		{
			bool enableDepth = true;
			bool writeDepth = true;
			ComparisonOperation comparisonOperation = ComparisonOperation::LESS;
		};
		struct DepthStencilView
		{
			uint32_t width = -1;
			uint32_t height = -1;
			bool bindAsTexture = false;
			bool cube = false;
		};
		enum class AddressingMode
		{
			WRAP,
			MIRROR,
			CLAMP
		};
		enum class Filter
		{
			ANISOTROPIC,
			LINEAR,
			POINT
		};
		struct SamplerState
		{
			AddressingMode addressU = AddressingMode::WRAP;
			AddressingMode addressV = AddressingMode::WRAP;
			AddressingMode addressW = AddressingMode::WRAP;
			Filter filter = Filter::ANISOTROPIC;
			int maxAnisotropy = 1;
		};

		enum class TextureFormat
		{
			R32G32B32A32_FLOAT,
			R8G8B8A8_UNORM
		};

		struct RenderTarget
		{
			bool bindAsShaderResource = false;
			bool bindAsUnorderedAccess = false;
			uint32_t width = -1;
			uint32_t height = -1;
			float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
			TextureFormat format = TextureFormat::R32G32B32A32_FLOAT;

		};
		struct UnorderedAccessView
		{
			bool bindAsShaderResource = false;
			uint32_t width = -1;
			uint32_t height = -1;
			float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
			TextureFormat format = TextureFormat::R8G8B8A8_UNORM;
		};
		enum class PrimitiveTopology : uint8_t
		{
			POINT_LIST,
			LINE_LIST,
			LINE_STRIP,
			TRIANGLE_LIST,
			TRIANGLE_STRIP
		};
		enum class BufferFlags
		{
			NONE = 0 << 0,
			BIND_VERTEX = 1 << 0,
			BIND_INDEX = 1 << 1,
			BIND_CONSTANT = 1 << 2,
			BIND_STREAMOUT = 1 << 3,
			CPU_READ = 1 << 5,
			CPU_WRITE = 1 << 6,
			DEFAULT = 1 << 7,
			DYNAMIC = 1 << 8,
			IMMUTABLE = 1 << 9,
			STRUCTURED = 1 << 10,
			RAW			= 1 << 11
		};

		ENUM_FLAGS(Graphics::Pipeline::BufferFlags);
		struct Buffer
		{
			void* data = nullptr;
			uint32_t elementCount = 0;
			uint16_t elementStride = 0;
			uint32_t maxElements = 0;
			BufferFlags flags = BufferFlags::NONE;

			static Buffer ConstantBuffer(uint16_t size, void* data = nullptr)
			{
				return { data, 1, size, 1, BufferFlags::BIND_CONSTANT | BufferFlags::CPU_WRITE | BufferFlags::DYNAMIC };
			}
			static Buffer VertexBuffer(void* data, uint16_t byteWidth, uint32_t numVerts, bool dynamic = false)
			{
				return { data, numVerts, byteWidth, numVerts, BufferFlags::BIND_VERTEX | (dynamic ? BufferFlags::DYNAMIC : BufferFlags::DEFAULT) };
			}
		};

		struct InputAssemblerStage
		{
			Utilz::GUID vertexBuffer;
			Utilz::GUID indexBuffer;
			PrimitiveTopology topology = PrimitiveTopology::TRIANGLE_LIST;
			Utilz::GUID inputLayout;

			Utilz::GUID GetID()const
			{
				return vertexBuffer + indexBuffer + inputLayout;
			}
		};

		struct StreamOutStage
		{
			Utilz::GUID streamOutTarget;
			Utilz::GUID GetID()const
			{
				return streamOutTarget;
			}
		};

		enum class ShaderType
		{
			VERTEX,
			HULL,
			DOMAIN_,
			GEOMETRY,
			GEOMETRY_STREAM_OUT,
			PIXEL,
			COMPUTE
		};

		struct ShaderStage
		{
			static const size_t maxTextures = 4;
			static const size_t maxSamplers = 2;
			static const size_t maxUAVs = 4;
			Utilz::GUID shader;
			Utilz::GUID textures[maxTextures];
			Utilz::GUID textureBindings[maxTextures];
			Utilz::GUID samplers[maxSamplers];

			uint8_t textureCount = 0;
			uint8_t samplerCount = 0;

			Utilz::GUID uavs[maxUAVs];
			uint8_t uavCount = 0;

			Utilz::GUID GetID()const
			{
				return shader + textures[0] + textures[1] + textures[2] + textures[3]
					+ textureBindings[0] + textureBindings[1] + textureBindings[2] + textureBindings[3]
					+ samplers[0];
			}
		};

		struct ComputeShaderStage
		{
			static const size_t maxTextures = 4;
			static const size_t maxUnorderedAccessViews = 4;
			Utilz::GUID shader;
			Utilz::GUID textures[maxTextures];
			Utilz::GUID uavs[maxUnorderedAccessViews];

		};

		struct OutputMergerStage
		{
			static const size_t maxRenderTargets = 4;
			Utilz::GUID blendState;
			Utilz::GUID depthStencilState;
			Utilz::GUID renderTargets[maxRenderTargets];
			Utilz::GUID depthStencilView;
			uint8_t renderTargetCount = 0;

			Utilz::GUID GetID()const
			{
				return blendState + depthStencilState + depthStencilView + renderTargets[0] + renderTargets[1] + renderTargets[2] + renderTargets[3];
			}
		};
		struct Viewport
		{
			float width = 0.0f;
			float height = 0.0f;
			float maxDepth = 0.0f;
			float minDepth = 0.0f;
			float topLeftX = 0.0f;
			float topLeftY = 0.0f;
		};

		struct RasterizerStage
		{
			Utilz::GUID rasterizerState;
			Utilz::GUID viewport;

			Utilz::GUID GetID()const
			{
				return rasterizerState + viewport;
			}
		};

		struct Pipeline_Mutable
		{
			InputAssemblerStage &IAStage;
			ShaderStage			&VSStage;
			ShaderStage			&GSStage;
			StreamOutStage		&SOStage;
			RasterizerStage		&RStage;
			ShaderStage			&PSStage;
			OutputMergerStage	&OMStage;

			ShaderStage			&CSStage;
		};

		class Pipeline
		{
			InputAssemblerStage IAStage_;
			ShaderStage	VSStage_;
			ShaderStage	GSStage_;
			StreamOutStage		SOStage_;
			RasterizerStage		RStage_;
			ShaderStage	PSStage_;
			OutputMergerStage	OMStage_;

			ShaderStage CSStage_;

			Utilz::GUID id;

		public:
			inline const InputAssemblerStage&			IAStage()const {
				return IAStage_;
			}
			inline const ShaderStage			&VSStage()const {
				return VSStage_;
			}
			inline const ShaderStage			&GSStage()const {
				return GSStage_;
			}
			inline const StreamOutStage		&SOStage()const {
				return SOStage_;
			}
			inline const RasterizerStage		&RStage()const {
				return RStage_;
			}
			inline const ShaderStage			&PSStage()const {
				return PSStage_;
			}
			inline const OutputMergerStage	&OMStage()const {
				return OMStage_;
			}
			inline const ShaderStage& CSStage()const
			{
				return CSStage_;
			}
			inline const Utilz::GUID ID()const
			{
				return id;
			}

			inline bool operator==(const Pipeline& other)const
			{
				return this->id == other.id;
			}

			void Edit(const std::function<void(Pipeline_Mutable)>& callback)
			{
				callback({ IAStage_, VSStage_ ,GSStage_, SOStage_ , RStage_, PSStage_ ,OMStage_,CSStage_ });
				id = IAStage_.GetID() + VSStage_.GetID() +
					GSStage_.GetID() + SOStage_.GetID() +
					RStage_.GetID() + PSStage_.GetID() +
					OMStage_.GetID() + CSStage_.GetID();
			}
		};
	}
}

#endif
