#ifndef _GRAPHICS_UPDATE_OBJECTS_H_
#define _GRAPHICS_UPDATE_OBJECTS_H_
#include "PipelineObjects.h"
#include <Graphics\UpdateJob.h>
namespace Renderer
{
	struct Buffer_UO : public UpdateObjectRef
	{
		ComPtr<ID3D11DeviceContext> c;
		ComPtr<ID3D11Buffer> bfr;

		const Pipeline::Buffer& info;
		Buffer_UO( ComPtr<ID3D11DeviceContext> c, ComPtr<ID3D11Buffer> bfr, const Pipeline::Buffer& bf): c(c), bfr(bfr), info(bf)
		{
			_ASSERT(c);
			_ASSERT(bfr);
		}
		void Map( const std::function<void( void*, size_t row_pitch, size_t depth_pitch )>& callback, AccessFlag flag = AccessFlag::READ )override;
		void WriteTo(void*data, size_t size)override;
		void ReadFrom(void*data, size_t size)override;
		UpdateObjectInfoVariant GetInfo()const override;
	};
}
#endif