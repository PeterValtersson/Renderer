#include "UpdateObjects.h"
#include <Graphics/Graphics_Exception.h>
namespace Graphics
{
	struct UpdateObject_Exception : public Graphics_Exception {
		UpdateObject_Exception( std::string_view what, long error )
			: Graphics_Exception( "Exception in UpdateObject.\n" + std::to_string( error ) + "\n" + std::string( what ), error )
		{}
	};
	void Buffer_UO::Map( const std::function<void( void*, size_t row_pitch, size_t depth_pitch )>& callback, AccessFlag flag )
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		D3D11_MAP mode;
		if ( flag_has( flag, AccessFlag::READ ) && flag_has( flag, AccessFlag::WRITE ) )
			mode = D3D11_MAP::D3D11_MAP_READ_WRITE;
		else if ( flag_has( flag, AccessFlag::READ ) )
			mode = D3D11_MAP::D3D11_MAP_READ;
		else if ( flag_has( flag, AccessFlag::WRITE ) )
			mode = D3D11_MAP::D3D11_MAP_WRITE_DISCARD;
		if ( auto hr = c->Map( bfr.Get(), 0, mode, 0, &msr ); FAILED( hr ) )
			throw UpdateObject_Exception( "Could not map buffer", hr );
		callback( msr.pData, msr.RowPitch, msr.DepthPitch );
		c->Unmap( bfr.Get(), 0 );
	}
	void Buffer_UO::WriteTo( void* data, size_t size )
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		if ( auto hr = c->Map( bfr.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &msr ); FAILED( hr ) )
			throw UpdateObject_Exception( "Could not map buffer", hr );
		memcpy( msr.pData, data, size );
		c->Unmap( bfr.Get(), 0 );
	}
	void Graphics::Buffer_UO::ReadFrom( void* data, size_t size )
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		if ( auto hr = c->Map( bfr.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr ); FAILED( hr ) )
			throw UpdateObject_Exception( "Could not map buffer", hr );
		memcpy( data, msr.pData, size );
		c->Unmap( bfr.Get(), 0 );
	}
	Graphics::UpdateObjectRef::UpdateObjectInfoVariant Buffer_UO::GetInfo() const
	{
		return info;
	}

}