#include "GraphicsLib.h"


// compile with cl -Zi /EHsc GraphicsInterface.cpp /link GraphicsLib.lib

#define internal 	static 
#define local_persist 	static 
#define global_variable static 

using namespace GraphicsLib;

global_variable bool running;
global_variable win32_offscreen_buffer GlobalBackbuffer;
struct v2 pos(0,0);

win32_window_dimension Win32GetWindowDimension(HWND window)
	{
		win32_window_dimension result;
		RECT cLientRect;
		GetClientRect(window,&cLientRect);
		result.width = cLientRect.right - cLientRect.left;
		result.height = cLientRect.bottom - cLientRect.top;
		return result;
	}

void Win32DisplayBufferInWindow(win32_offscreen_buffer *Buffer, 
					HDC DeviceContext, int WindowWidth, int WindowHeight)
{

	StretchDIBits(DeviceContext,
			0, 0, WindowWidth, WindowHeight,
			0, 0, Buffer->width, Buffer->height,
			Buffer->memory,
			&Buffer->info,
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
			running = false;
			OutputDebugStringA("WM_CLOSE\n");
		}break;

		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		}break;

		case WM_DESTROY:
		{
			running  = false;
			OutputDebugStringA("WM_DESTROY\n");	
		}break;

		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		{
			uint32_t VKCode = WParam;
			if(VKCode == 'W')
			{
				pos.y += 20;
			}
			if(VKCode == 'A')
			{
				pos.x -= 20;
			}
			if(VKCode == 'S')
			{
				pos.y -= 20;
			}
			if(VKCode == 'D')
			{
				pos.x += 20;
			}
			if(VKCode == VK_UP)
			{
				pos.y += 1;
			}
			if(VKCode == VK_LEFT)
			{
				pos.x -= 1;
			}
			if(VKCode == VK_DOWN)
			{
				pos.y -= 1;
			}
			if(VKCode == VK_RIGHT)
			{
				pos.x += 1;
			}
		}
		case WM_KEYUP:
		{
			uint32_t VKCode = WParam;
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

			win32_window_dimension Dimension = Win32GetWindowDimension(Window);
			Win32DisplayBufferInWindow(&GlobalBackbuffer, DeviceContext, 
				Dimension.width, Dimension.height);

			EndPaint(Window,&Paint);
		}break;

		default:
		{
			Result = DefWindowProc(Window, Message, WParam, LParam);
		}break;
	}
	return Result;
}

int CALLBACK WinMain(HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR CommandLine,
	int ShowCode)
{


	GraphicsLib::Functions::Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);

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
			struct image image_data;
			struct image partOfImage_data;
			char* filename = "32test.tga";
			GraphicsLib::Functions::ReadTGAImage(filename, &image_data);
			partOfImage_data.width = image_data.width / 2;
			partOfImage_data.height = image_data.height / 2;
			partOfImage_data.pitch = image_data.pitch;
			partOfImage_data.data = &image_data.data[image_data.width / 2];

			int XOffset = 0;
			int YOffset = 0;
			int y = 300;
			float scale = 1.0;
			double degrees = 0.0f;
			double degreeToRadiance = (3.14 / 180.0);
			
			struct v2 pos2(400,400);
			struct AABB aabb1(100,100);
			struct AABB aabb2(50,100);
			
			float rotation = 45 * degreeToRadiance;
			
			struct OBB obb (v2(cos(rotation), sin(rotation)),v2(cos(rotation + 3.14 / 2), sin(rotation + 3.14 / 2)),50,50);

			running = true;
			while(running)
			{
				MSG Message;
				while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					if(Message.message == WM_QUIT)
					{
						running = false;
					}
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}


				win32_color White;
				White.red = 255;
				White.green = 255;
				White.blue = 255;
				win32_color Black;
				Black.red = 0;
				Black.green = 0;
				Black.blue = 0;	
				win32_color Red;
				Red.red = 255;
				Red.green = 0;
				Red.blue = 0;			

				GraphicsLib::Functions::RenderAlignedBox(&GlobalBackbuffer, v2(640, 360), AABB(1280, 720), Black);
				
				//GraphicsLib::Functions::RenderAlignedBox(&GlobalBackbuffer, pos2, aabb1, White);
				GraphicsLib::Functions::RenderTexture(&GlobalBackbuffer, pos2, &image_data, aabb1);
				
				/*if(!aabb1.AABBvsAABBTest(aabb2, pos-pos2))
				{
					GraphicsLib::Functions::RenderAlignedBox(&GlobalBackbuffer, pos, aabb2, White);
				}*/
				if(!MathLib::IntersectionTest::AABBvsOBB(aabb1,obb,pos2-pos))
				{
					//GraphicsLib::Functions::RenderRotatedBox(&GlobalBackbuffer, pos, obb, White);
					GraphicsLib::Functions::RenderRotatedTexture(&GlobalBackbuffer, pos, &image_data, obb);
				}

				HDC DeviceContext = GetDC(Window); //REMEMBER DeviceContext is a copy and MUST be Released later or MEMORY LEAK

				win32_window_dimension Dimension = Win32GetWindowDimension(Window);
				Win32DisplayBufferInWindow(&GlobalBackbuffer, DeviceContext, 
						Dimension.width, Dimension.height);
				XOffset++;
				//YOffset++;

				scale += 0.001f;
				if(scale > 2.0)
				{
					scale = 0.5;
				}

				y++;
				if(y > 300)
				{
					y = -100;
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