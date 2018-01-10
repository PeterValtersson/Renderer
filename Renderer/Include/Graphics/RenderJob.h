#ifndef SE_GRAPHICS_RENDER_JOB_H_
#define SE_GRAPHICS_RENDER_JOB_H_
#include "Pipeline.h"
#include <functional>
#include <vector>

namespace Graphics
{
	enum class RenderGroup : uint8_t
	{
		PRE_PASS_0,
		PRE_PASS_1,
		PRE_PASS_2,
		PRE_PASS_3,
		PRE_PASS_4,
		PRE_PASS_5,
		RENDER_PASS_0,
		RENDER_PASS_1,
		RENDER_PASS_2,
		RENDER_PASS_3,
		RENDER_PASS_4,
		RENDER_PASS_5,
		POST_PASS_0,
		POST_PASS_1,
		POST_PASS_2,
		POST_PASS_3,
		POST_PASS_4,
		POST_PASS_5,
		FINAL_PASS
	};


	struct RenderJob
	{
		Pipeline pipeline;
		uint32_t vertexCount = 0;
		uint32_t vertexOffset = 0;
		uint32_t indexCount = 0;
		uint32_t indexOffset = 0;
		uint32_t instanceCount = 0;
		uint32_t instanceOffset = 0;
		uint32_t maxInstances = 0;
		uint32_t ThreadGroupCountX = 0;
		uint32_t ThreadGroupCountY = 0;
		uint32_t ThreadGroupCountZ = 0;
		std::vector<std::function<void(uint32_t instanceCount, uint32_t instanceOffset)>> mappingFunctions;
	};
}


#endif
