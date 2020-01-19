#ifndef SE_GRAPHICS_PIPELINE_H_
#define SE_GRAPHICS_PIPELINE_H_
#include <cstdint>
#include <Utilities/GUID.h>
#include <type_traits>
#include <functional>
#include <Utilities/Flags.h>
typedef unsigned int UINT;

namespace Graphics
{
	static const Utilities::GUID Default_RenderTarget("Backbuffer");
	static const Utilities::GUID Default_Viewport("FullscreenViewPort");
	static const Utilities::GUID Default_DepthStencil("BackbufferDepthStencil");
	static const Utilities::GUID Default_VertexShader_FullscreenQUAD("FullscreenQUADVS");
	static const Utilities::GUID Default_PixelShader_POS_TEXTURE_MultiChannel("MultichannelPixelShader");
	static const Utilities::GUID Default_PixelShader_POS_TEXTURE_SingleChannel("SinglechannelPixelShader");
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
		enum class DepthStencilViewFlags
		{
			NONE = 0,
			SHADER_RESOURCE = 1 << 0,
			CUBE = 1 << 1
		};
		ENUM_FLAGS(Graphics::Pipeline::DepthStencilViewFlags);
		struct DepthStencilView
		{
			uint32_t width = -1;
			uint32_t height = -1;
			DepthStencilViewFlags flags = DepthStencilViewFlags::NONE;
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

		enum class TextureFlags
		{
			NONE = 0,
			SHADER_RESOURCE = 1 << 0,
			RENDER_TARGET = 1 << 1,
			UNORDERED_ACCESS = 1 << 2,
			CPU_ACCESS_READ = 1 << 3
		};
		ENUM_FLAGS(Graphics::Pipeline::TextureFlags);

		enum class ViewDimension{
			Texture_1D,
			Texture_2D,
			Texture_3D,
			Cube
		};
		struct Texture
		{
			TextureFlags flags;
			UINT width = 0;
			UINT height = 0;
			UINT mipLevels = 1;
			UINT arraySize = 1;
			float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
			TextureFormat format = TextureFormat::R32G32B32A32_FLOAT;
			ViewDimension viewDimension = ViewDimension::Texture_2D;
			void* data = nullptr;
			UINT memPitch = 0;
			UINT memSlicePitch = 0;
			static Texture RenderTarget(uint32_t width, uint32_t height, bool shaderResource = false, bool cpuread = false)
			{
				return { TextureFlags::RENDER_TARGET | (shaderResource ? TextureFlags::SHADER_RESOURCE : TextureFlags::NONE)
					| (cpuread ? TextureFlags::CPU_ACCESS_READ : TextureFlags::NONE)
					, width, height };
			}
			static Texture UnorderedAccessView(uint32_t width, uint32_t height, bool shaderResource = false, bool cpuread = false)
			{
				return { TextureFlags::UNORDERED_ACCESS | (shaderResource ? TextureFlags::SHADER_RESOURCE : TextureFlags::NONE)
					| (cpuread ? TextureFlags::CPU_ACCESS_READ : TextureFlags::NONE)
					, width, height, 1, 1, {0.0f,0.0f,0.0f,0.0f}, TextureFormat::R8G8B8A8_UNORM };
			}
			static Texture TextureFromData( void* data, uint32_t width, uint32_t height, UINT mipLevels = 1 )
			{
				Texture t;
				t.flags = TextureFlags::SHADER_RESOURCE;
				t.width = width;
				t.height = height;
				t.mipLevels = mipLevels;
				t.format = TextureFormat::R8G8B8A8_UNORM;
				t.data = data;
				t.memPitch = width * 4;
				return t;
			}
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
			static Buffer StructuredBuffer(void* data, uint16_t byteWidth, uint32_t count)
			{
				return {data, count, byteWidth, count, BufferFlags::STRUCTURED | BufferFlags::DEFAULT};
			}
		};

		struct InputAssemblerStage
		{
			Utilities::GUID vertexBuffer;
			Utilities::GUID indexBuffer;
			PrimitiveTopology topology = PrimitiveTopology::TRIANGLE_LIST;
			Utilities::GUID inputLayout;

			Utilities::GUID GetID()const
			{
				return vertexBuffer + indexBuffer + inputLayout;
			}
		};

		struct StreamOutStage
		{
			Utilities::GUID streamOutTarget;
			Utilities::GUID GetID()const
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
			Utilities::GUID shader;
			Utilities::GUID textures[maxTextures];
			Utilities::GUID textureBindings[maxTextures];
			Utilities::GUID samplers[maxSamplers];

			uint8_t textureCount = 0;
			uint8_t samplerCount = 0;

			Utilities::GUID uavs[maxUAVs];
			uint8_t uavCount = 0;

			Utilities::GUID GetID()const
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
			Utilities::GUID shader;
			Utilities::GUID textures[maxTextures];
			Utilities::GUID uavs[maxUnorderedAccessViews];

		};

		struct OutputMergerStage
		{
			static const size_t maxRenderTargets = 4;
			Utilities::GUID blendState;
			Utilities::GUID depthStencilState;
			Utilities::GUID renderTargets[maxRenderTargets];
			Utilities::GUID depthStencilView;
			uint8_t renderTargetCount = 0;

			Utilities::GUID GetID()const
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
			Utilities::GUID rasterizerState;
			Utilities::GUID viewport;

			Utilities::GUID GetID()const
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

			Utilities::GUID id;

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
			inline const Utilities::GUID ID()const
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
