#ifndef _GRAPHICS_SAFE_RELEASE_H_
#define _GRAPHICS_SAFE_RELEASE_H_
namespace Graphics
{
	template<class T>
	inline void Safe_Release(T& t)
	{
		if (t != nullptr)
		{
			t->Release();
			t = nullptr;
		}
	}
}
#endif