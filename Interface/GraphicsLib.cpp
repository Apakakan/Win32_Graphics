#include "stdafx.h"
#include "GraphicsLib.h"


typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

//Compile by using: cl /EHsc /DGRAPHICSLIB_EXPORTS /LD GraphicsLib.cpp
//cl -Zi ..\win32_window.cpp user32.lib Gdi32.lib /link /out:program.exe
//cl -Zi /EHsc //DGRAPHICSLIB_EXPORTS /LD GraphicsLib.cpp user32.lib Gdi32.lib

namespace GraphicsLib
{
	
	void Functions::ReadTGAImage(char* filename, struct image* image)
	{
		FILE *fp;
		uint16 header[9]; 

		fp = fopen(filename, "r");
		if(fp)
		{
			fread(header, 2, 9, fp);
			int width = header[6];
			int height = header[7];
			
			image->width = width;
			image->height = height;	
			image->pitch = width;

			image->data = (struct pixelInfo *)VirtualAlloc(0, width * height * sizeof(struct pixelInfo), MEM_COMMIT, PAGE_READWRITE);
			
			fread(image->data ,1 , width * height * sizeof(struct pixelInfo), fp);
		}
		else
		{
			OutputDebugStringA("Error in open file\n");
		}
	}

	void Functions::RenderAlignedBox(win32_offscreen_buffer *buffer, v2 position, int width, int height, win32_color color)
	{
		if(position.y < 0)
		{
			position.y = 0;
		}
		if(position.x < 0)
		{
			position.x = 0;
		}
		uint8 *row = (uint8 *)buffer->memory;
		row += (int)(position.y * buffer->pitch);
		row += (int)(position.x * buffer->bytesPerPixel);
		for(int y = position.y; y < height; y++)
		{
			if(y < buffer->height)
			{
				uint32 *pixel = (uint32*)row;
				for(int x = position.x; x < width && x < buffer->width; x++)
				{
					*pixel++ = (color.red << 16 | color.green << 8 | color.blue);	
				}
				row += buffer->pitch;
			}
		}
	}

	void Functions::RenderRotatedBox(win32_offscreen_buffer *buffer, v2 position, int width, int height, double radi, win32_color color)
	{
		v2 rigthAxis;
		rigthAxis.x = cos(radi);
		rigthAxis.y = sin(radi);

		v2 upAxis;
		upAxis.x = cos(radi + (3.14 / 2.0));
		upAxis.y = sin(radi + (3.14 / 2.0));

		v2 corners[4];
		corners[0].x = position.x;
		corners[0].y = position.y;

		corners[1].x = corners[0].x + (rigthAxis.x * width);
		corners[1].y = corners[0].y + (rigthAxis.y * width);

		corners[2].x = corners[0].x + (upAxis.x * height);
		corners[2].y = corners[0].y + (upAxis.y * height);

		corners[3].x = corners[0].x + (rigthAxis.x * width) + (upAxis.x * height);
		corners[3].y = corners[0].y + (rigthAxis.y * width) + (upAxis.y * height);

		v2 minXY;
		minXY.x = position.x;
		minXY.y = position.y; 
		v2 maxXY;
		maxXY.x = position.x;
		maxXY.y = position.y; 

		uint8 *row = (uint8 *)buffer->memory;
		for (int i = 0; i < 4; ++i)
		{
			if(corners[i].x < minXY.x)
			{
				minXY.x = corners[i].x;
			}
			else if(corners[i].x > maxXY.x)
			{
				maxXY.x = corners[i].x;
			}
			if(corners[i].y < minXY.y)
			{
				minXY.y = corners[i].y;
			}
			else if(corners[i].y > maxXY.y)
			{
				maxXY.y = corners[i].y;
			}

		}
		
		if(minXY.x < 0)
		{
			minXY.x = 0;
		}
		if(minXY.y < 0)
		{
			minXY.y = 0;
		}
		if(maxXY.x >= buffer->width)
		{
			maxXY.x = buffer->width-1;
		}
		if(maxXY.y >= buffer->height)
		{
			maxXY.y = buffer->height-1;
		}
		
		row += ((int)minXY.y * buffer->pitch) + ((int)minXY.x * buffer->bytesPerPixel);
		for (int y = minXY.y; y < maxXY.y; ++y)
		{
			uint32 *pixel = (uint32*)row;
			for (int x = minXY.x; x < maxXY.x; ++x)
			{
				v2 vecToPixel;
				vecToPixel.x = x - corners[0].x;
				vecToPixel.y = y - corners[0].y;
				float rightDot = vecToPixel.Dot(rigthAxis);
				if(rightDot <= width && rightDot >= 0)
				{
					float upDot = vecToPixel.Dot(upAxis);
					if(upDot <= height && upDot >= 0)
					{
						*pixel = (color.red << 16 | color.green << 8 | color.blue);
					}
				}
				pixel++;
			}
			row += buffer->pitch;
		}
	}

	void Functions::RenderSphere(win32_offscreen_buffer *buffer, v2 position, int radius, win32_color color)
	{
		int radius2 = radius * radius;
		v2 center;
		center.x = position.x + radius;
		center.y = position.y + radius;
		v2 start;
		start.x = position.x;
		start.y = position.y;
		v2 end;
		end.x = position.x + radius * 2;
		end.y = position.y + radius * 2;
		
		if(start.x < 0)
		{
			start.x = 0;
		}
		if(start.y < 0)
		{
			start.y = 0;
		}
		if(end.x >= buffer->width)
		{
			end.x = buffer->width-1;
		}
		if(end.y >= buffer->height)
		{
			end.y = buffer->height-1;
		}
		
		uint8 *row = (uint8 *)buffer->memory;
		row += ((int)start.y * buffer->pitch) + ((int)start.x * buffer->bytesPerPixel);

		for (int y = start.y; y < end.y; ++y)
		{
			uint32 *pixel = (uint32*)row;
			for (int x = start.x; x < end.x; ++x)
			{
				v2 vecToPixel;
				vecToPixel.x = x - center.x;
				vecToPixel.y = y - center.y;
				if(vecToPixel.LengthPow2() <radius2)
				{
					*pixel = (color.red << 16 | color.green << 8 | color.blue);
				}
				pixel++;
			}
			row += buffer->pitch;

		}
	}

	void Functions::RenderLine(win32_offscreen_buffer * buffer, v2 startPoint, v2 endPoint, int width, win32_color color)
	{
		v2 minXY;
		v2 maxXY;
		v2 lineDir;

		int width2 = width * width;

		lineDir = endPoint-startPoint;


		minXY.x = ((startPoint.x - width < endPoint.x - width) ? (startPoint.x - width) : (endPoint.x - width));
		minXY.y = ((startPoint.y - width < endPoint.y - width) ? (startPoint.y - width) : (endPoint.y - width));

		maxXY.x = ((startPoint.x + width > endPoint.x + width) ? (startPoint.x + width) : (endPoint.x + width));
		maxXY.y = ((startPoint.y + width > endPoint.y + width) ? (startPoint.y + width) : (endPoint.y + width));

		if(minXY.x < 0)
		{
			minXY.x = 0;
		}
		if(minXY.y < 0)
		{
			minXY.y = 0;
		}
		if(maxXY.x >= buffer->width)
		{
			maxXY.x = buffer->width-1;
		}
		if(maxXY.y >= buffer->height)
		{
			maxXY.y = buffer->height-1;
		}
		
		uint8 *row = (uint8 *)buffer->memory;
		row += ((int)minXY.y * buffer->pitch) + ((int)minXY.x * buffer->bytesPerPixel);

		for(int y = minXY.y; y < maxXY.y; ++y)
		{
			uint32 *pixel = (uint32*)row;
			for(int x = minXY.x; x < maxXY.x; ++x)
			{
				v2 vecToPixel;
				vecToPixel.x = x - startPoint.x;
				vecToPixel.y = y - startPoint.y;

				v2 vecToPixelProjected = vecToPixel.projectOntoClamped(lineDir);

				v2 vecToLine = vecToPixel-vecToPixelProjected;

				if(vecToLine.LengthPow2() < width2)
				{
					*pixel = (color.red << 16 | color.green << 8 | color.blue);
				}
				pixel++;
			}
			row += buffer->pitch;
		}

	}

	void Functions::RenderTexture(win32_offscreen_buffer *buffer, v2 position, float scale, struct image* image)
	{
		uint8 *row = (uint8 *)buffer->memory;	
		row += ((int)position.y * buffer->pitch) + ((int)position.x * buffer->bytesPerPixel);

		int width = image->width * scale;
		int height = image->height * scale;

		int pixelIndex = 0;
		for (int y = 0; y < height; ++y)
		{
			uint32 *pixel = (uint32*)row;
			for (int x = 0; x < width; ++x)
			{
					if(x + position.x >= 0 && x + position.x < buffer->width && y + position.y >= 0 && y + position.y < buffer->height)
					{
						pixelIndex = ((int)(y / scale) * image->pitch) + (int)(x / scale);
						*pixel = (image->data[pixelIndex].r << 16 | image->data[pixelIndex].g << 8 | image->data[pixelIndex].b);
						pixel++;
					}
					
			}
			row += buffer->pitch;
		}
	}

	void Functions::RenderRotatedTexture(win32_offscreen_buffer *buffer, v2 position, float scale, struct image* image, double radi)
	{

		v2 rigthAxis;
		rigthAxis.x = cos(radi);
		rigthAxis.y = sin(radi);

		v2 upAxis;
		upAxis.x = cos(radi + (3.14 / 2.0));
		upAxis.y = sin(radi + (3.14 / 2.0));


		int width = image->width * scale;
		int height = image->height * scale;

		v2 corners[4];
		corners[0].x = position.x;
		corners[0].y = position.y;

		corners[1].x = corners[0].x + (rigthAxis.x * width);
		corners[1].y = corners[0].y + (rigthAxis.y * width);

		corners[2].x = corners[0].x + (upAxis.x * height);
		corners[2].y = corners[0].y + (upAxis.y * height);

		corners[3].x = corners[0].x + (rigthAxis.x * width) + (upAxis.x * height);
		corners[3].y = corners[0].y + (rigthAxis.y * width) + (upAxis.y * height);

		v2 minXY;
		minXY.x = position.x;
		minXY.y = position.y; 
		v2 maxXY;
		maxXY.x = position.x;
		maxXY.y = position.y; 

		uint8 *row = (uint8 *)buffer->memory;
		for (int i = 0; i < 4; ++i)
		{
			if(corners[i].x < minXY.x)
			{
				minXY.x = corners[i].x;
			}
			else if(corners[i].x > maxXY.x)
			{
				maxXY.x = corners[i].x;
			}
			if(corners[i].y < minXY.y)
			{
				minXY.y = corners[i].y;
			}
			else if(corners[i].y > maxXY.y)
			{
				maxXY.y = corners[i].y;
			}

		}

		row += ((int)minXY.y * buffer->pitch) + ((int)minXY.x * buffer->bytesPerPixel);
		for (int y = minXY.y; y < maxXY.y; ++y)
		{
			uint32 *pixel = (uint32*)row;
			for (int x = minXY.x; x < maxXY.x; ++x)
			{
				v2 vecToPixel;
				vecToPixel.x = x - corners[0].x;
				vecToPixel.y = y - corners[0].y;
				float rightDot = vecToPixel.Dot(rigthAxis);
				if(rightDot <= width && rightDot >= 0 && x >= 0 && x < buffer->width)
				{
					float upDot = vecToPixel.Dot(upAxis);
					if(upDot <= height && upDot >= 0 && y >= 0 && y < buffer->height)
					{
						int pixelIndex = ((int)(upDot / scale) * image->pitch) + (int)(rightDot / scale);
						*pixel = (image->data[pixelIndex].r << 16 | image->data[pixelIndex].g << 8 | image->data[pixelIndex].b);
					}
				}
				pixel++;
			}
			row += buffer->pitch;
		}
	}

	void Functions::Win32ResizeDIBSection(win32_offscreen_buffer *buffer, int width, int height)
	{
	
	if(buffer->memory)
	{
		VirtualFree(buffer->memory, 0, MEM_RELEASE);
	}
	
	buffer->width = width;
	buffer->height = height;	

	//NOTE When biHeight is negative this means the Window treats the 
	//bitmap as a top-down not bottom-up. meaning that the first three 
	//bytes of the image are the color for the top left pixel int the bitmap
	buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
	buffer->info.bmiHeader.biWidth = buffer->width;
	buffer->info.bmiHeader.biHeight = buffer->height;
	buffer->info.bmiHeader.biPlanes = 1;
	buffer->info.bmiHeader.biBitCount = 32;
	buffer->info.bmiHeader.biCompression = BI_RGB;
	
	buffer->bytesPerPixel = 4;
	int bitmapMemorySize = (width*height)* buffer->bytesPerPixel;

	buffer->memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
	buffer->pitch = width * buffer->bytesPerPixel;
	
	}
}
























	