#include "GraphicsLib.h"

// compile with cl -Zi /EHsc GraphicsInterface.cpp /link GraphicsLib.lib

#define internal 	static 
#define local_persist 	static 
#define global_variable static 

using namespace GraphicsLib;

global_variable bool running;
global_variable win32_offscreen_buffer GlobalBackbuffer;

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
				/*for (DWORD ControllerIndex=0; ControllerIndex< XUSER_MAX_COUNT; ControllerIndex++ )
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

				}*/

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

				GraphicsLib::Functions::RenderAlignedBox(&GlobalBackbuffer, v2(-1, 0), 1281, 720, Black);
				
				GraphicsLib::Functions::RenderRotatedBox(&GlobalBackbuffer, v2(1200, 50), 150, 80, degrees * degreeToRadiance, White);

				GraphicsLib::Functions::RenderTexture(&GlobalBackbuffer, v2(1200, 100), scale, &image_data);
				GraphicsLib::Functions::RenderTexture(&GlobalBackbuffer, v2(1200, 400), scale, &partOfImage_data);

				GraphicsLib::Functions::RenderRotatedTexture(&GlobalBackbuffer, v2(700, 100), scale, &image_data, degrees * degreeToRadiance);
				GraphicsLib::Functions::RenderRotatedTexture(&GlobalBackbuffer, v2(700, 400), scale, &partOfImage_data, degrees * degreeToRadiance);

				GraphicsLib::Functions::RenderSphere(&GlobalBackbuffer, v2(-50, -50), 50, White);

				GraphicsLib::Functions::RenderLine(&GlobalBackbuffer, v2(200,200),v2(300,y),10,Red);

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
