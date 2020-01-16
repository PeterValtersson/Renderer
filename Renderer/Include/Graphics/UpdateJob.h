#ifndef SE_GRAPHICS_UPDATE_JOB_H_
#define SE_GRAPHICS_UPDATE_JOB_H_
#include "Pipeline.h"
#include <functional>
#include <vector>
#include "RenderJob.h"
#include <Utilities/ErrorHandling.h>

namespace Graphics
{
	struct Map_Not_Implemented : public Utilities::Exception{
		Map_Not_Implemented() : Utilities::Exception( "Map has not been implemented" )
		{};
	};
	struct WriteTo_Not_Implemented : public Utilities::Exception{
		WriteTo_Not_Implemented() : Utilities::Exception( "WriteTo has not been implemented" )
		{};
	};
	struct ReadFrom_Not_Implemented : public Utilities::Exception{
		ReadFrom_Not_Implemented() : Utilities::Exception( "ReadFrom has not been implemented" )
		{};
	};
	struct GetInfo__Not_Implemented : public Utilities::Exception{
		GetInfo__Not_Implemented() : Utilities::Exception( "GetInfo_ has not been implemented" )
		{};
	};

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
		virtual void* Map(AccessFlag flag = AccessFlag::READ)
		{
			throw Map_Not_Implemented();
		}
		virtual void Unmap() {}
		template<class T>
		const T& GetInfo()const
		{
			return *(T*)GetInfo_();
		}

		virtual void WriteTo(void*data, size_t size)
		{
			throw WriteTo_Not_Implemented();
		}
		virtual void ReadFrom(void*data, size_t size)
		{
			throw ReadFrom_Not_Implemented();
		}
	protected:
		virtual const void* GetInfo_()const
		{
			throw GetInfo__Not_Implemented();
		}
	};


	enum class PipelineObjectType
	{
		Buffer,

		RenderTarget,
		UnorderedAccessView,
		Texture,
		DepthStencilView,

		Viewport
	};



	struct UpdateJob
	{
		Utilities::GUID objectToMap;
		UpdateFrequency frequency;
		PipelineObjectType type;
		std::function<void(UpdateObject& obj)> updateCallback;

		static UpdateJob Buffer(Utilities::GUID id, UpdateFrequency freq, const std::function<void(UpdateObject& obj)>& cb)
		{
			return { id, freq, PipelineObjectType::Buffer, cb };
		}
	};
}


#endif
