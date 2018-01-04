#ifndef DECLDIR_GRAPHICS
#if defined DLL_EXPORT_GRAPHICS
#define DECLDIR_GRAPHICS_C extern "C" __declspec(dllexport)
#define DECLDIR_GRAPHICS __declspec(dllexport)
#else
#define DECLDIR_GRAPHICS_C extern "C" __declspec(dllimport)
#define DECLDIR_GRAPHICS __declspec(dllimport)
#endif
#endif