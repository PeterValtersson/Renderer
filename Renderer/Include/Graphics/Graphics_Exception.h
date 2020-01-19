#ifndef _GRAPHICS_GRAPHICS_EXCEPTION_H_
#define _GRAPHICS_GRAPHICS_EXCEPTION_H_
#pragma once

#include <Utilities/ErrorHandling.h>
namespace Graphics
{
	struct Graphics_Exception : public Utilities::Exception {
		Graphics_Exception( const std::string& what, long error )
			: Utilities::Exception( "Exception in Graphics", what ), error( error )
		{}
		Graphics_Exception( const std::string& what )
			: Utilities::Exception( "Exception in Graphics", what ), error( ~0 )
		{}
		long error; // ~0 if NA
	};
}
#endif