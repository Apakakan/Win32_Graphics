#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <xinput.h>
#include <math.h>

#define internal 	static 
#define local_persist 	static 
#define global_variable static 

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

struct PixelInfo
{
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
};
struct Image
{
	char header[18];
	int width;
	int height;
	int pitch;
	struct PixelInfo* data;
};

struct win32_offscreen_buffer
{
	BITMAPINFO Info;
	void *Memory;
	int Width;
	int Height;
	int BytesPerPixel;
	int Pitch;
};

struct win32_window_dimension
{
	int Width;
	int Height;
};

global_variable bool Running;
global_variable win32_offscreen_buffer GlobalBackbuffer;


#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
	return (0);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
	return (0);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal void
Win32LoadXInput(void)
{
	HMODULE XInputLibrary = LoadLibraryA("xinput1_3.dll");
	if(XInputLibrary)
	{
		XInputGetState_ = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
		XInputSetState_ = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
	}
}


struct Win32_color
{
	uint8 Red, Green, Blue, Alpha;
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
		float length = sqrt(x*x) + sqrt(y*y); 
		return v2(x / length, y / length);
	}
	float Length()
	{
		return fabs(x) + fabs(y);
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

win32_window_dimension Win32GetWindowDimension(HWND Window)
{
	win32_window_dimension Result;
	RECT CLientRect;
	GetClientRect(Window,&CLientRect);
	Result.Width = CLientRect.right - CLientRect.left;
	Result.Height = CLientRect.bottom - CLientRect.top;
	return Result;
}

internal void ReadTGAImage(char* filename, struct Image* image)
{
	FILE *fp;
	uint16 header[9]; 

	fp = fopen(filename, "r");
	if(fp)
	{
		//fread(header, 1, 18, fp);
		//unsigned int width = header[12] | header[13] << 8;
		//unsigned int height = header[14] | header[15] << 8;
		
		fread(header, 2, 9, fp);
		int width = header[6];
		int height = header[7];

		char str[40];
		sprintf(str, "Width is %d\n Height is %d\n", width, height);
		OutputDebugString(str);
		
		/*for(int i = 0; i < 18; i++)
		{
			image->header[i] = header[i];
		}*/
		image->width = width;
		image->height = height;	
		image->pitch = width;

		image->data = (struct PixelInfo *)VirtualAlloc(0, width * height * sizeof(struct PixelInfo), MEM_COMMIT, PAGE_READWRITE);
		
		fread(image->data ,1 , width * height * sizeof(struct PixelInfo), fp);
		/*for (int i = 0; i < width * height; ++i)
		{
			char str[10];
			sprintf(str, "r:%d,g:%d,b:%d\n", image->data[i].r, image->data[i].g, image->data[i].b);
			OutputDebugStringA(str);
		}*/
	}
	else
	{
		OutputDebugStringA("Error in open file\n");
	}
}

/*internal float
DotProduct(v2 vector1, v2 vector2)
{
	float result = (vector1.x * vector2.x) + (vector1.y * vector2.y);
	return result;
}

internal void 
NormalizeVector(v2* vectorToNormalize)
{
	float sum = fabs(vectorToNormalize->x) + fabs(vectorToNormalize->y);
	vectorToNormalize->x /= sum;
	vectorToNormalize->y /= sum;
}*/

internal void 
RenderAlignedBox(win32_offscreen_buffer *Buffer, int srcX, int srcY, int width, int height, Win32_color color)
{
	uint8 *Row = (uint8 *)Buffer->Memory;
	Row += (srcY * Buffer->Pitch);
	Row += (srcX * Buffer->BytesPerPixel);
	for(int y = 0; y < height && (y + srcY) < Buffer->Height; y++)
	{
		uint32 *Pixel = (uint32*)Row;
		for(int x = 0; x < width && (x + srcX) < Buffer->Width; x++)
		{
			*Pixel++ = (color.Red << 16 | color.Green << 8 | color.Blue);	
		}
		Row += Buffer->Pitch;
	}
}

internal void
RenderRotatedBox(win32_offscreen_buffer *Buffer, int srcX, int srcY, int width, int height, double radi, Win32_color color)
{
	v2 rigthAxis;
	rigthAxis.x = cos(radi);
	rigthAxis.y = sin(radi);

	v2 upAxis;
	upAxis.x = cos(radi + (3.14 / 2.0));
	upAxis.y = sin(radi + (3.14 / 2.0));

	v2 corners[4];
	corners[0].x = srcX;
	corners[0].y = srcY;

	corners[1].x = corners[0].x + (rigthAxis.x * width);
	corners[1].y = corners[0].y + (rigthAxis.y * width);

	corners[2].x = corners[0].x + (upAxis.x * height);
	corners[2].y = corners[0].y + (upAxis.y * height);

	corners[3].x = corners[0].x + (rigthAxis.x * width) + (upAxis.x * height);
	corners[3].y = corners[0].y + (rigthAxis.y * width) + (upAxis.y * height);

	v2 minXY;
	minXY.x = srcX;
	minXY.y = srcY; 
	v2 maxXY;
	maxXY.x = srcX;
	maxXY.y = srcY; 

	uint8 *Row = (uint8 *)Buffer->Memory;
	for (int i = 0; i < 4; ++i)
	{
		uint32* Pixel = (uint32*)(Row + ((int)corners[i].y * Buffer->Pitch) + ((int)corners[i].x * Buffer->BytesPerPixel));
		*Pixel = (255 << 16 | 255 << 8 | 255);
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

	Row += ((int)minXY.y * Buffer->Pitch) + ((int)minXY.x * Buffer->BytesPerPixel);
	for (int y = minXY.y; y < maxXY.y; ++y)
	{
		uint32 *Pixel = (uint32*)Row;
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
					*Pixel = (color.Red << 16 | color.Green << 8 | color.Blue);
				}
			}
			Pixel++;
		}
		Row += Buffer->Pitch;
	}
}

internal void
RenderSphere(win32_offscreen_buffer *Buffer, int srcX, int srcY, int radius, Win32_color color)
{
	v2 center;
	center.x = srcX + radius;
	center.y = srcY + radius;
	v2 start;
	start.x = srcX;
	start.y = srcY;
	v2 end;
	end.x = srcX + radius * 2;
	end.y = srcY + radius * 2;
	uint8 *Row = (uint8 *)Buffer->Memory;
	Row += ((int)start.y * Buffer->Pitch) + ((int)start.x * Buffer->BytesPerPixel);

	for (int y = start.y; y < end.y; ++y)
	{
		uint32 *Pixel = (uint32*)Row;
		for (int x = start.x; x < end.x; ++x)
		{
			v2 vecToPixel;
			vecToPixel.x = x - center.x;
			vecToPixel.y = y - center.y;
			if((vecToPixel.x * vecToPixel.x + vecToPixel.y * vecToPixel.y) <radius * radius)
			{
				*Pixel = (color.Red << 16 | color.Green << 8 | color.Blue);
			}
			Pixel++;
		}
		Row += Buffer->Pitch;

	}
}

internal void
RenderLine(win32_offscreen_buffer * Buffer, v2 startPoint, v2 endPoint, int width, Win32_color color)
{
	v2 minXY;
	v2 maxXY;
	v2 lineDir;

	//lineDir.x = endPoint.x - startPoint.x;
	//lineDir.y = endPoint.y - startPoint.y;
	lineDir = endPoint-startPoint;
	lineDir = lineDir.Normalize();


	minXY.x = ((startPoint.x - width < endPoint.x - width) ? (startPoint.x - width) : (endPoint.x - width));
	minXY.y = ((startPoint.y - width < endPoint.y - width) ? (startPoint.y - width) : (endPoint.y - width));

	maxXY.x = ((startPoint.x + width > endPoint.x + width) ? (startPoint.x + width) : (endPoint.x + width));
	maxXY.y = ((startPoint.y + width > endPoint.y + width) ? (startPoint.y + width) : (endPoint.y + width));


	uint8 *Row = (uint8 *)Buffer->Memory;
	Row += ((int)minXY.y * Buffer->Pitch) + ((int)minXY.x * Buffer->BytesPerPixel);

	for(int y = minXY.y; y < maxXY.y; ++y)
	{
		uint32 *Pixel = (uint32*)Row;
		for(int x = minXY.x; x < maxXY.x; ++x)
		{
			v2 vecToPixel;
			vecToPixel.x = x - startPoint.x;
			vecToPixel.y = y - startPoint.y;

			float dot = vecToPixel.Dot(lineDir);
			v2 vecToPixelProjected = lineDir.Scale(dot);

			v2 vecToLine = vecToPixel-vecToPixelProjected;

			int test = vecToLine.Length();

			*Pixel = (test << 16 | test << 8 | test);
			if(vecToLine.Length() < width)
			{
				*Pixel = (color.Red << 16 | color.Green << 8 | color.Blue);
			}
			Pixel++;
		}
		Row += Buffer->Pitch;
	}

}

internal void
RenderTexture(win32_offscreen_buffer *Buffer, int srcX, int srcY, struct Image* image)
{
	uint8 *Row = (uint8 *)Buffer->Memory;	
	Row += (srcY * Buffer->Pitch) + (srcX * Buffer->BytesPerPixel);

	int pixelIndex = 0;
	for (int y = 0; y < image->height; ++y)
	{
		uint32 *Pixel = (uint32*)Row;
		for (int x = 0; x < image->width; ++x)
		{
				if(x + srcX >= 0 && x + srcX < Buffer->Width && y + srcY >= 0 && y + srcY < Buffer->Height)
				{
					pixelIndex = (y * image->pitch) + x;
					*Pixel = (image->data[pixelIndex].r << 16 | image->data[pixelIndex].g << 8 | image->data[pixelIndex].b);
					Pixel++;
				}
				
		}
		Row += Buffer->Pitch;
	}
}

internal void 
RenderRotatedTexture(win32_offscreen_buffer *Buffer, int srcX, int srcY, struct Image* image, double radi)
{

	v2 rigthAxis;
	rigthAxis.x = cos(radi);
	rigthAxis.y = sin(radi);

	v2 upAxis;
	upAxis.x = cos(radi + (3.14 / 2.0));
	upAxis.y = sin(radi + (3.14 / 2.0));

	v2 corners[4];
	corners[0].x = srcX;
	corners[0].y = srcY;

	corners[1].x = corners[0].x + (rigthAxis.x * image->width);
	corners[1].y = corners[0].y + (rigthAxis.y * image->width);

	corners[2].x = corners[0].x + (upAxis.x * image->height);
	corners[2].y = corners[0].y + (upAxis.y * image->height);

	corners[3].x = corners[0].x + (rigthAxis.x * image->width) + (upAxis.x * image->height);
	corners[3].y = corners[0].y + (rigthAxis.y * image->width) + (upAxis.y * image->height);

	v2 minXY;
	minXY.x = srcX;
	minXY.y = srcY; 
	v2 maxXY;
	maxXY.x = srcX;
	maxXY.y = srcY; 

	uint8 *Row = (uint8 *)Buffer->Memory;
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

	Row += ((int)minXY.y * Buffer->Pitch) + ((int)minXY.x * Buffer->BytesPerPixel);
	for (int y = minXY.y; y < maxXY.y; ++y)
	{
		uint32 *Pixel = (uint32*)Row;
		for (int x = minXY.x; x < maxXY.x; ++x)
		{
			v2 vecToPixel;
			vecToPixel.x = x - corners[0].x;
			vecToPixel.y = y - corners[0].y;
			float rightDot = vecToPixel.Dot(rigthAxis);
			if(rightDot <= image->width && rightDot >= 0)
			{
				float upDot = vecToPixel.Dot(upAxis);
				if(upDot <= image->height && upDot >= 0)
				{
					int pixelIndex = ((int)upDot * image->pitch) + (int)rightDot;
					*Pixel = (image->data[pixelIndex].r << 16 | image->data[pixelIndex].g << 8 | image->data[pixelIndex].b);
				}
			}
			Pixel++;
		}
		Row += Buffer->Pitch;
	}
}


internal void
RenderWeirdGradient(win32_offscreen_buffer *Buffer, int XOffset, int YOffset)
{

	uint8 *Row = (uint8 *)Buffer->Memory;
	for(int y = 0; y < Buffer->Height; y++)
	{
		uint32 *Pixel = (uint32 *)Row;
		for(int x = 0; x < Buffer->Width; x++)
		{
			uint8 Blue = (x + XOffset);
			uint8 Green = (y + YOffset);
			*Pixel++ = (124 << 16 | Green << 8 | Blue);	 	
		}
		Row += Buffer->Pitch;
	}
}

internal void 
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
	if(Buffer->Memory)
	{
		VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
	}
	
	Buffer->Width = Width;
	Buffer->Height = Height;	

	//NOTE When biHeight is negative this means the Window treats the 
	//bitmap as a top-down not bottom-up. meaning that the first three 
	//bytes of the image are the color for the top left pixel int the bitmap
	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;
	
	Buffer->BytesPerPixel = 4;
	int BitmapMemorySize = (Width*Height)* Buffer->BytesPerPixel;

	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
	Buffer->Pitch = Width * Buffer->BytesPerPixel;
	
}

internal void
Win32DisplayBufferInWindow(win32_offscreen_buffer *Buffer, 
					HDC DeviceContext, int WindowWidth, int WindowHeight)
{

	StretchDIBits(DeviceContext,
			0, 0, WindowWidth, WindowHeight,
			0, 0, Buffer->Width, Buffer->Height,
			Buffer->Memory,
			&Buffer->Info,
			DIB_RGB_COLORS,	SRCCOPY);
}

LRESULT CALLBACK 
Win32MainWindowCallBack(HWND	Window,
		UINT   Message,
		WPARAM WParam,
		LPARAM LParam)
{
	LRESULT Result = 0;
	switch(Message)
	{
		case WM_CLOSE:
		{
			Running = false;
			OutputDebugStringA("WM_CLOSE\n");
		}break;

		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		}break;

		case WM_DESTROY:
		{
			Running  = false;
			OutputDebugStringA("WM_DESTROY\n");	
		}break;

		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			uint32 VKCode = WParam;
			bool WasDown = ((LParam & (1 << 30)) != 0);
			bool IsDown = ((LParam & (1 << 31)) == 0);
			if(WasDown != IsDown)
			{
				if(VKCode == 'W')
				{
				}
				if(VKCode == 'A')
				{
				}
				if(VKCode == 'S')
				{
				}
				if(VKCode == 'D')
				{
				}
				if(VKCode == 'Q')
				{
				}
				if(VKCode == 'E')
				{
				}
				if(VKCode == VK_UP)
				{
				}
				if(VKCode == VK_LEFT)
				{
				}
				if(VKCode == VK_DOWN)
				{
				}
				if(VKCode == VK_RIGHT)
				{
				}
				if(VKCode == VK_ESCAPE)
				{
					OutputDebugStringA("ESPACE: ");
					if(IsDown)
					{
						OutputDebugStringA("ISDown");
					}
					if(WasDown)
					{
						OutputDebugStringA("WasDown");
					}
					OutputDebugStringA("\n");
				}
				if(VKCode == VK_SPACE)
				{
				}
			}
		}break;

		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			//int x = Paint.rcPaint.left;
			//int y = Paint.rcPaint.top;
			//int Width = Paint.rcPaint.right - Paint.rcPaint.left;
			//int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

			win32_window_dimension Dimension = Win32GetWindowDimension(Window);
			Win32DisplayBufferInWindow(&GlobalBackbuffer, DeviceContext, 
				Dimension.Width, Dimension.Height);

			EndPaint(Window,&Paint);
		}break;

		default:
		{
			Result = DefWindowProc(Window, Message, WParam, LParam);
		}break;
	}
	return Result;
}

int CALLBACK
WinMain(HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR CommandLine,
	int ShowCode)
{
	/*MessageBoxA(0, 
		"This is a window",
		"WINDOW?",
		MB_OK|MB_ICONINFORMATION );*/

	Win32LoadXInput();

	Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);

	WNDCLASSA WindowClass = {};
	WindowClass.style = CS_HREDRAW|CS_VREDRAW;
  	WindowClass.lpfnWndProc = Win32MainWindowCallBack;
  	WindowClass.hInstance = Instance;
  	//WindowClass.hIcon;
  	WindowClass.lpszClassName = "Name of WindowClass";
	
	if(RegisterClassA(&WindowClass))
	{
		HWND Window = CreateWindowEx(
  		0,
  		WindowClass.lpszClassName,
  		"Name of WindowClass",
  		WS_OVERLAPPEDWINDOW|WS_VISIBLE,
  		CW_USEDEFAULT,
  		CW_USEDEFAULT,
  		CW_USEDEFAULT,
  		CW_USEDEFAULT,
  		0,
  		0,
  		Instance,
  		0);
		if(Window) // != NULL
		{
			struct Image image;
			struct Image partOfImage;
			char* filename = "32test.tga";
			ReadTGAImage(filename, &image);
			partOfImage.width = image.width / 2;
			partOfImage.height = image.height / 2;
			partOfImage.pitch = image.pitch;
			partOfImage.data = &image.data[image.width / 2];

			int XOffset = 0;
			int YOffset = 0;
			int y = 300;
			double degrees = 0.0f;
			double degreeToRadiance = (3.14 / 180.0);

			Running = true;
			while(Running)
			{
				MSG Message;
				while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					if(Message.message == WM_QUIT)
					{
						Running = false;
					}
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				for (DWORD ControllerIndex=0; ControllerIndex< XUSER_MAX_COUNT; ControllerIndex++ )
				{
					//gamepad input 
					XINPUT_STATE ControllerState;
					if(XInputGetState(ControllerIndex,&ControllerState) == ERROR_SUCCESS)
					{
						//controller is plugged in
						XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

						bool Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						bool Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						bool Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						bool Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
						bool Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
						bool Back = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
						bool LeftShoulder = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
						bool RightShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
						bool AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
						bool BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
						bool XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
						bool YButton = (Pad->wButtons & XINPUT_GAMEPAD_Y);

						int16 StickX = Pad->sThumbLX;
						int16 StickY = Pad->sThumbLY;
					}
					else
					{
						//controller note available
					}

				}

				//RenderWeirdGradient(&GlobalBackbuffer, XOffset, YOffset);
				Win32_color White;
				White.Red = 255;
				White.Green = 255;
				White.Blue = 255;
				Win32_color Black;
				Black.Red = 0;
				Black.Green = 0;
				Black.Blue = 0;	
				Win32_color Red;
				Red.Red = 255;
				Red.Green = 0;
				Red.Blue = 0;			

				RenderAlignedBox(&GlobalBackbuffer, 0, 0, 1280, 720, Black);
				
				//RenderRotatedBox(&GlobalBackbuffer, 600, 500, 150, 80, degrees * degreeToRadiance, White);

				//RenderTexture(&GlobalBackbuffer, 400, 100, &image);
				//RenderTexture(&GlobalBackbuffer, 400, 400, &partOfImage);

				RenderRotatedTexture(&GlobalBackbuffer, 700, 100, &image, degrees * degreeToRadiance);
				RenderRotatedTexture(&GlobalBackbuffer, 700, 400, &partOfImage, degrees * degreeToRadiance);

				//RenderSphere(&GlobalBackbuffer, 200, 500, 50, White);


				HDC DeviceContext = GetDC(Window); //REMEMBER DeviceContext is a copy and MUST be Released later or MEMORY LEAK

				win32_window_dimension Dimension = Win32GetWindowDimension(Window);
				Win32DisplayBufferInWindow(&GlobalBackbuffer, DeviceContext, 
						Dimension.Width, Dimension.Height);
				XOffset++;
				//YOffset++;
				y++;
				if(y > 400)
				{
					y = 200;
				}
				degrees += 0.1;
				ReleaseDC(Window, DeviceContext);
			}
		}
		else
		{
			//handel error
		}
	}
	else
	{
		//error window creation
	}

	return 0;
}


























	