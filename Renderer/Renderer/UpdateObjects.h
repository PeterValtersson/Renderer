#ifndef _GRAPHICS_UPDATE_OBJECTS_H_
#define _GRAPHICS_UPDATE_OBJECTS_H_
#include "PipelineObjects.h"
#include <Graphics\UpdateJob.h>
namespace Graphics
{
	struct Buffer_UO : public UpdateObject
	{
		ID3D11DeviceContext* c;
		ID3D11Buffer* bfr;
		D3D11_MAPPED_SUBRESOURCE msr;
		Buffer_UO(ID3D11DeviceContext* c, ID3D11Buffer* bfr): c(c), bfr(bfr)
		{
			_ASSERT(c);
			_ASSERT(bfr);
		}
		void* Map(AccessFlag flag = AccessFlag::READ)override
		{
			D3D11_MAP mode;
			if (flag & AccessFlag::READ && flag & AccessFlag::WRITE)
				mode = D3D11_MAP::D3D11_MAP_READ_WRITE;
			else if (flag & AccessFlag::READ)
				mode = D3D11_MAP::D3D11_MAP_READ;
			else if (flag & AccessFlag::WRITE)
				mode = D3D11_MAP::D3D11_MAP_WRITE_DISCARD;
			THROW_IF_GRAPHICS_ERROR(c->Map(bfr, 0, mode, 0, &msr), "Could not map buffer");
			return msr.pData;
		}
		void Unmap()
		{
			c->Unmap(bfr, 0);
		}
		GRAPHICS_ERROR WriteTo(void*data, size_t size)
		{
			RETURN_IF_GRAPHICS_ERROR(c->Map(bfr, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &msr), "Could not map buffer");
			memcpy(msr.pData, data, size);
			c->Unmap(bfr, 0);
			RETURN_GRAPHICS_SUCCESS;
		}
		GRAPHICS_ERROR ReadFrom(void*data, size_t size)
		{
			RETURN_IF_GRAPHICS_ERROR(c->Map(bfr, 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr), "Could not map buffer");
			memcpy(data, msr.pData, size);
			c->Unmap(bfr, 0);
			RETURN_GRAPHICS_SUCCESS;
		}
	};
}
#endif