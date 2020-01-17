#ifndef _GRAPHICS_UPDATE_OBJECTS_H_
#define _GRAPHICS_UPDATE_OBJECTS_H_
#include "PipelineObjects.h"
#include <Graphics\UpdateJob.h>
namespace Graphics
{
	struct Buffer_UO : public UpdateObjectRef
	{
		ID3D11DeviceContext* c;
		ID3D11Buffer* bfr;
		D3D11_MAPPED_SUBRESOURCE msr;
		const Pipeline::Buffer& info;
		Buffer_UO(ID3D11DeviceContext* c, ID3D11Buffer* bfr, const Pipeline::Buffer& bf): c(c), bfr(bfr), info(bf)
		{
			_ASSERT(c);
			_ASSERT(bfr);
		}
		void* Map(AccessFlag flag = AccessFlag::READ)override;
		inline void Unmap()
		{
			c->Unmap(bfr, 0);
		}
		void WriteTo(void*data, size_t size)override;
		void ReadFrom(void*data, size_t size)override;

	private:
		const void* GetInfo_()const override;
	};
}
#endif