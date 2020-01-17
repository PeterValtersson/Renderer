#include <Utilities/ErrorHandling.h>
struct Graphics_Exception : public Utilities::Exception{
	Graphics_Exception(const std::string& what, long error ) 
		: Utilities::Exception( "Exception in Graphics", what ), error(error)
	{}
	Graphics_Exception( const std::string& what )
		: Utilities::Exception( "Exception in Graphics", what ), error(~0)
	{}
	long error; // ~0 if NA
};