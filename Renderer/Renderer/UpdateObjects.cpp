#include "UpdateObjects.h"
namespace Graphics
{
	void * Buffer_UO::Map(AccessFlag flag)
	{
		D3D11_MAP mode;
		if (flag & AccessFlag::READ && flag & AccessFlag::WRITE)
			mode = D3D11_MAP::D3D11_MAP_READ_WRITE;
		else if (flag & AccessFlag::READ)
			mode = D3D11_MAP::D3D11_MAP_READ;
		else if (flag & AccessFlag::WRITE)
			mode = D3D11_MAP::D3D11_MAP_WRITE_DISCARD;
		THROW_IF_HR_ERROR(c->Map(bfr, 0, mode, 0, &msr), "Could not map buffer");
		return msr.pData;
	}
	UERROR Buffer_UO::WriteTo(void * data, size_t size)
	{
		RETURN_IF_HR_ERROR(c->Map(bfr, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &msr), "Could not map buffer");
		memcpy(msr.pData, data, size);
		c->Unmap(bfr, 0);
		RETURN_SUCCESS;
	}
	UERROR Graphics::Buffer_UO::ReadFrom(void * data, size_t size)
	{
		RETURN_IF_HR_ERROR(c->Map(bfr, 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr), "Could not map buffer");
		memcpy(data, msr.pData, size);
		c->Unmap(bfr, 0);
		RETURN_SUCCESS;
	}
	const void * Buffer_UO::GetInfo_() const
	{
		return &info;
	}
}