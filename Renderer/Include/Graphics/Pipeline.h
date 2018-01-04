#ifndef SE_GRAPHICS_PIPELINE_H_
#define SE_GRAPHICS_PIPELINE_H_
#include <cstdint>
#include <GUID.h>

namespace Graphics
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
		bool enable;
		BlendOperation blendOperation;
		BlendOperation blendOperationAlpha;
		Blend srcBlend;
		Blend dstBlend;
		Blend srcBlendAlpha;
		Blend dstBlendAlpha;
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
		bool enableDepth;
		bool writeDepth;
		ComparisonOperation comparisonOperation;
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
		bool bindAsShaderResource;
		bool bindAsUnorderedAccess;
		int width;
		int height;
		float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
		TextureFormat format;

	};
	struct UnorderedAccessView
	{
		bool bindAsShaderResource;
		bool bindAsUnorderedAccess;
		int width;
		int height;
		float clearColor[4];
		TextureFormat format;
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
		BIND_VERTEX = 1 << 0,
		BIND_INDEX = 1 << 1,
		BIND_CONSTANT = 1 << 2,
		BIND_STREAMOUT = 1 << 3,
		CPU_READ = 1 << 5,
		CPU_WRITE = 1 << 6,
		DEFAULT = 1 << 7,
		DYNAMIC = 1 << 8,
		IMMUTABLE = 1 << 9
	};
	struct InputAssemblerStage
	{
		Utilz::GUID vertexBuffer;
		Utilz::GUID indexBuffer;
		PrimitiveTopology topology;
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


	struct ShaderStage
	{
		static const size_t maxTextures = 4;
		static const size_t maxSamplers = 2;
		static const size_t maxUAVs = 4;
		Utilz::GUID shader = Utilz::GUID();
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
		float width;
		float height;
		float maxDepth;
		float minDepth;
		float topLeftX;
		float topLeftY;
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

	struct Pipeline
	{
		InputAssemblerStage IAStage;
		ShaderStage	VSStage;
		ShaderStage	GSStage;
		StreamOutStage		SOStage;
		RasterizerStage		RStage;
		ShaderStage	PSStage;
		OutputMergerStage	OMStage;

		ShaderStage CSStage;

		Utilz::GUID id;
		void SetID()
		{
			id = IAStage.GetID() + VSStage.GetID() +
				GSStage.GetID() + SOStage.GetID() +
				RStage.GetID() + PSStage.GetID() +
				OMStage.GetID() + CSStage.GetID();
		}
		bool operator==(const Pipeline& other)const
		{
			return this->id == other.id;
		}


	};
}

#endif
