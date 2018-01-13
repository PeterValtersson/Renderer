#ifndef SE_GRAPHICS_UPDATE_JOB_H_
#define SE_GRAPHICS_UPDATE_JOB_H_
#include "Pipeline.h"
#include <functional>
#include <vector>
#include "RenderJob.h"
#include "../Graphics_Error.h"

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
	enum class UpdateFrequency : uint8_t
	{
		EVERY_FRAME,
		ONCE
	};
	enum class AccessFlag
	{
		READ = 1 << 0,
		WRITE = 1 << 1
	};

	ENUM_FLAGS(Graphics::AccessFlag);

	struct UpdateObject
	{
		virtual ~UpdateObject(){}

		template<class T>
		T& GetMapObject(AccessFlag flag = AccessFlag::READ)
		{
			void* data = nullptr;
			return *(T*)Map(flag);
		}
		virtual void* Map(AccessFlag flag = AccessFlag::READ) = 0;
		virtual void Unmap() = 0;
		virtual GRAPHICS_ERROR WriteTo(void*data, size_t size) = 0;
		virtual GRAPHICS_ERROR ReadFrom(void*data, size_t size) = 0;
	protected:
		
	};


	enum class PipelineObjectType
	{
		VertexBuffer,
		IndexBuffer,
		ConstantBuffer,
		StructuredBuffer,
		RawBuffer,

		RenderTarget,
		UnorderedAccessView,
		Texture,
		DepthStencilView,

		Viewport
	};



	struct UpdateJob
	{
		Utilz::GUID objectToMap;
		UpdateFrequency frequency;
		PipelineObjectType type;
		std::function<GRAPHICS_ERROR(UpdateObject* obj)> updateCallback;

		static UpdateJob ConstantBuffer(Utilz::GUID id, UpdateFrequency freq, const std::function<GRAPHICS_ERROR(UpdateObject* obj)>& cb)
		{
			return { id, freq, PipelineObjectType::ConstantBuffer, cb };
		}
	};
}


#endif
