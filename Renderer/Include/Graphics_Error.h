#ifndef _GRAPHICS_GRAPHICS_ERROR_H_
#define _GRAPHICS_GRAPHICS_ERROR_H_

#include <stdint.h>
#include <string>

#define RETURN_GRAPHICS_ERROR(msg, nr) return (Graphics::_lastError = Graphics::Graphics_Error{msg, nr, __FILE__, __LINE__})
#define RETURN_GRAPHICS_ERROR_C(msg) return (Graphics::_lastError = Graphics::Graphics_Error{msg, -(__COUNTER__+1), __FILE__, __LINE__})

#define RETURN_IF_GRAPHICS_ERROR(result, msg) { auto e = result; if(e < 0) return (Graphics::_lastError = Graphics::Graphics_Error{msg, e, __FILE__, __LINE__});}
#define RETURN_IF_GRAPHICS_ERROR_S(result, msg) { auto e = result; if(e < 0){Graphics::_lastErrorStringMsg = msg; return (Graphics::_lastError = Graphics::Graphics_Error{Graphics::_lastErrorStringMsg.c_str(), e, __FILE__, __LINE__});}}

#define THROW_IF_GRAPHICS_ERROR(result, msg) { auto e = result; if(e < 0)  throw (Graphics::_lastError = Graphics::Graphics_Error{msg, result, __FILE__, __LINE__});}
#define PASS_IF_GRAPHICS_ERROR(x) { GRAPHICS_ERROR e = x; if(e.errornr < 0) return e;}

#define THROW_PASS_IF_GRAPHICS_ERROR(x)  { GRAPHICS_ERROR e = x; if(e.errornr < 0) throw e;}
#define RETURN_GRAPHICS_SUCCESS return (Graphics::_lastError = Graphics::Graphics_Error{nullptr, 0, nullptr, 0})

#define GRAPHICS_ERROR const Graphics::Graphics_Error&
#define THROW_GRAPHICS_ERROR(msg) throw (Graphics::_lastError = Graphics::Graphics_Error{msg, -(__COUNTER__+1), __FILE__, __LINE__})

namespace Graphics
{
	struct Graphics_Error
	{
		const char* errorMSG;	
		int32_t errornr;
		const char* file;
		int32_t line;
	};
	inline bool operator==(GRAPHICS_ERROR l, GRAPHICS_ERROR r)
	{
		return r.errornr == l.errornr;
	}
	static Graphics_Error _lastError;
	static std::string _lastErrorStringMsg;
}
#endif