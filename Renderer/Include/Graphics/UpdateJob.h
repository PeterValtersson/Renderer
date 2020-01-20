#ifndef SE_GRAPHICS_UPDATE_JOB_H_
#define SE_GRAPHICS_UPDATE_JOB_H_
#include "Pipeline.h"
#include <functional>
#include <vector>
#include "RenderJob.h"
#include "Graphics_Exception.h"
#include <variant>

namespace Graphics
{
	struct Map_Not_Implemented : public Graphics_Exception {
		Map_Not_Implemented() : Graphics_Exception( "Map has not been implemented" )
		{};
	};
	struct WriteTo_Not_Implemented : public Graphics_Exception {
		WriteTo_Not_Implemented() : Graphics_Exception( "WriteTo has not been implemented" )
		{};
	};
	struct ReadFrom_Not_Implemented : public Graphics_Exception {
		ReadFrom_Not_Implemented() : Graphics_Exception( "ReadFrom has not been implemented" )
		{};
	};
	struct GetInfo_Not_Implemented : public Graphics_Exception {
		GetInfo_Not_Implemented() : Graphics_Exception( "GetInfo_ has not been implemented" )
		{};
	};

	enum class UpdateFrequency : uint8_t {
		EVERY_FRAME,
		ONCE
	};
	enum class AccessFlag {
		READ = 1 << 0,
		WRITE = 1 << 1
	};

	ENUM_FLAGS( Graphics::AccessFlag );

	struct UpdateObjectRef {
		typedef std::variant<Pipeline::Buffer> UpdateObjectInfoVariant;
		virtual ~UpdateObjectRef() {}

		template<class T>
		inline void Map( const std::function<void( T& )>& callback, AccessFlag flag = AccessFlag::READ )
		{
			Map( [&]( void* data, size_t row_pitch, size_t depth_pitch )
			{
				callback( *( T* )data );
			}, flag );
		}
		virtual void Map( const std::function<void( void*, size_t row_pitch, size_t depth_pitch )>& callback, AccessFlag flag = AccessFlag::READ )
		{
			throw Map_Not_Implemented();
		}

		virtual UpdateObjectInfoVariant GetInfo()const
		{
			throw GetInfo_Not_Implemented();
		}

		virtual void WriteTo( void* data, size_t size )
		{
			throw WriteTo_Not_Implemented();
		}
		virtual void ReadFrom( void* data, size_t size )
		{
			throw ReadFrom_Not_Implemented();
		}
	};


	enum class PipelineObjectType {
		Buffer,

		RenderTarget,
		UnorderedAccessView,
		Texture,
		DepthStencilView,

		Viewport
	};



	struct UpdateJob {
		Utilities::GUID objectToMap;
		UpdateFrequency frequency;
		PipelineObjectType type;
		std::function<void( UpdateObjectRef & obj )> updateCallback;

		static UpdateJob UpdateBuffer( Utilities::GUID id, UpdateFrequency freq, const std::function<void( UpdateObjectRef & obj )>& cb )
		{
			return { id, freq, PipelineObjectType::Buffer, cb };
		}
	};
}


#endif
