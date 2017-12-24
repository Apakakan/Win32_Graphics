#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "MathLib.h"
//#include <math.h>

#ifdef GRAPHICSLIB_EXPORTS  
#define GRAPHICSLIB_API __declspec(dllexport)   
#else  
#define GRAPHICSLIB_API __declspec(dllimport)   
#endif

namespace GraphicsLib
{
	struct pixelInfo
	{
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t a;
	};
	struct image
	{
		char header[18];
		int width;
		int height;
		int pitch;
		struct pixelInfo* data;
	};

	struct win32_offscreen_buffer
	{
		BITMAPINFO info;
		void *memory;
		int width;
		int height;
		int bytesPerPixel;
		int pitch;
	};

	struct win32_window_dimension
	{
		int width;
		int height;
	};
	struct win32_color
	{
		uint8_t red, green, blue, alpha;
	};


	class Functions
	{
		public:
			static GRAPHICSLIB_API void ReadTGAImage(char* filename, struct image* image);

			static GRAPHICSLIB_API void RenderAlignedBox(win32_offscreen_buffer *buffer, v2 position, AABB box, win32_color color);

			static GRAPHICSLIB_API void RenderRotatedBox(win32_offscreen_buffer *buffer, v2 position, OBB box, win32_color color);

			static GRAPHICSLIB_API void RenderSphere(win32_offscreen_buffer *buffer, v2 position, Sphere sphere, win32_color color);

			static GRAPHICSLIB_API void RenderLine(win32_offscreen_buffer * buffer, Line line, win32_color color);

			static GRAPHICSLIB_API void RenderTexture(win32_offscreen_buffer *buffer, v2 position, struct image* image, AABB box);

			static GRAPHICSLIB_API void RenderRotatedTexture(win32_offscreen_buffer *buffer, v2 position, struct image* image, OBB box);

			static GRAPHICSLIB_API void Win32ResizeDIBSection(win32_offscreen_buffer *buffer, int width, int height);

	};
}