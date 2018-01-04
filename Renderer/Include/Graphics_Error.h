#ifndef _GRAPHICS_GRAPHICS_ERROR_H_
#define _GRAPHICS_GRAPHICS_ERROR_H_
#include <CompileTimeString.h>
#include <stdint.h>
#define RETURN_ERROR(msg, nr) return Graphics::Graphics_Error{msg, nr, __FILE__, __LINE__}
#define RETURN_ERROR_C(msg) return Graphics::Graphics_Error{msg, -__COUNTER__, __FILE__, __LINE__}
#define RETURN_IF_ERROR(result, msg) if((result) < 0) return Graphics::Graphics_Error{msg, result, __FILE__, __LINE__}
#define PASS_IF_ERROR(x) if(x.errornr < 0) return x
#define RETURN_SUCCESS return Graphics::Graphics_Error{nullptr, 0, nullptr, 0}
namespace Graphics
{
	struct Graphics_Error
	{
		const char* errorMSG;	
		int32_t errornr;
		const char* file;
		int32_t line;
	};
}
#endif