#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

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

	struct v2
	{
	float x,y;

	v2(float x=0, float y=0)
		:x(x),y(y)
	{

	}

	v2 Scale(float scalar)
	{
		return v2(x*scalar, y*scalar);
	}
	float Dot(const v2& a)
	{
		return (x*a.x + y*a.y);
	}
	v2 Normalize()
	{
		//float sum = this->Length();
		//return v2(x/sum,y/sum);
		float length = this->Length(); 
		return v2(x / length, y / length);
	}
	float Length()
	{
		return sqrt(x*x + y*y); 
	}
	float LengthPow2()
	{
		return x*x + y*y; 
	}
	v2 projectOnto(v2 a)
	{
		v2 result;
		float magnitude = 0;
		float dot = 0;

		dot = this->Dot(a);
		magnitude = a.x * a.x + a.y * a.y;

		return result = a.Scale(dot/magnitude);

	}
	v2 projectOntoClamped(v2 a)
	{
		v2 result;
		float lengthOfA = a.Length();
		float magnitude = 0;
		float dot = 0;

		dot = this->Dot(a);
		magnitude = a.x * a.x + a.y * a.y;
		float scalar = dot/magnitude;
		if(scalar < 0)
		{
			scalar = 0;
		}
		else if(scalar > 1.0)
		{
			scalar = 1.0;
		}

		return result = a.Scale(scalar);

	}

	v2& operator=(const v2& a)
	{
		x=a.x;
		y=a.y;
		return*this;
	}

	v2 operator+(const v2& a)
	{
		return v2(x+a.x,y+a.y);
	}

	v2 operator-(const v2& a)
	{
		return v2(x-a.x,y-a.y);
	}

	v2 operator==(const v2& a) const
	{
		return (a.x == x && a.y == y);
	}
};

	class Functions
	{
		public:
			static GRAPHICSLIB_API void ReadTGAImage(char* filename, struct image* image);

			static GRAPHICSLIB_API void RenderAlignedBox(win32_offscreen_buffer *buffer, v2 position, int width, int height, win32_color color);

			static GRAPHICSLIB_API void RenderRotatedBox(win32_offscreen_buffer *buffer, v2 position, int width, int height, double radi, win32_color color);

			static GRAPHICSLIB_API void RenderSphere(win32_offscreen_buffer *buffer, v2 position, int radius, win32_color color);

			static GRAPHICSLIB_API void RenderLine(win32_offscreen_buffer * buffer, v2 startPoint, v2 endPoint, int width, win32_color color);

			static GRAPHICSLIB_API void RenderTexture(win32_offscreen_buffer *buffer, v2 position, float scale, struct image* image);

			static GRAPHICSLIB_API void RenderRotatedTexture(win32_offscreen_buffer *buffer, v2 position, float scale, struct image* image, double radi);

			static GRAPHICSLIB_API void Win32ResizeDIBSection(win32_offscreen_buffer *buffer, int width, int height);

	};
}