#include "GraphicsLib.h" // for old 2d rendering

//for directX 3D
//#include <windows.h> // also in Graphics.h
#include <windowsx.h>
#include <d3d11.h>

#pragma comment (lib, "d3d11.lib")
// 


// compile with cl -Zi /EHsc GraphicsInterface.cpp /link GraphicsLib.lib

#define internal 	static 
#define local_persist 	static 
#define global_variable static 

using namespace GraphicsLib;

global_variable bool running;
global_variable win32_offscreen_buffer GlobalBackbuffer;
struct v2 pos(0,0);

IDXGISwapChain *swapchain;
ID3D11Device *dev;
ID3D11DeviceContext *devcon;

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
			PostQuitMessage(0);
			return 0;
		}break;

		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		}break;

		case WM_DESTROY:
		{
			running  = false;
			OutputDebugStringA("WM_DESTROY\n");	
			return 0;
		}break;

		//case WM_SYSKEYDOWN:
		//case WM_SYSKEYUP:
		//case WM_KEYDOWN:
		//{
		//	uint32_t VKCode = WParam;
		//	if(VKCode == 'W')
		//	{
		//		pos.y += 20;
		//	}
		//	if(VKCode == 'A')
		//	{
		//		pos.x -= 20;
		//	}
		//	if(VKCode == 'S')
		//	{
		//		pos.y -= 20;
		//	}
		//	if(VKCode == 'D')
		//	{
		//		pos.x += 20;
		//	}
		//	if(VKCode == VK_UP)
		//	{
		//		pos.y += 1;
		//	}
		//	if(VKCode == VK_LEFT)
		//	{
		//		pos.x -= 1;
		//	}
		//	if(VKCode == VK_DOWN)
		//	{
		//		pos.y -= 1;
		//	}
		//	if(VKCode == VK_RIGHT)
		//	{
		//		pos.x += 1;
		//	}
		//}
		//case WM_KEYUP:
		//{
		//	uint32_t VKCode = WParam;
		//	bool WasDown = ((LParam & (1 << 30)) != 0);
		//	bool IsDown = ((LParam & (1 << 31)) == 0);
		//	if(WasDown != IsDown)
		//	{
		//		if(VKCode == 'W')
		//		{
		//		}
		//		if(VKCode == 'A')
		//		{
		//		}
		//		if(VKCode == 'S')
		//		{
		//		}
		//		if(VKCode == 'D')
		//		{
		//		}
		//		if(VKCode == 'Q')
		//		{
		//		}
		//		if(VKCode == 'E')
		//		{
		//		}
		//		if(VKCode == VK_UP)
		//		{
		//		}
		//		if(VKCode == VK_LEFT)
		//		{
		//		}
		//		if(VKCode == VK_DOWN)
		//		{
		//		}
		//		if(VKCode == VK_RIGHT)
		//		{
		//		}
		//		if(VKCode == VK_ESCAPE)
		//		{
		//			OutputDebugStringA("ESPACE: ");
		//			if(IsDown)
		//			{
		//				OutputDebugStringA("ISDown");
		//			}
		//			if(WasDown)
		//			{
		//				OutputDebugStringA("WasDown");
		//			}
		//			OutputDebugStringA("\n");
		//		}
		//		if(VKCode == VK_SPACE)
		//		{
		//		}
		//	}
		//}break;
        //
		//case WM_PAINT:
		//{
		//	PAINTSTRUCT Paint;
		//	HDC DeviceContext = BeginPaint(Window, &Paint);
        //    
		//	win32_window_dimension Dimension = Win32GetWindowDimension(Window);
		//	Win32DisplayBufferInWindow(&GlobalBackbuffer, DeviceContext, 
		//		Dimension.width, Dimension.height);
        //    
		//	EndPaint(Window,&Paint);
		//}break;

		default:
		{
			Result = DefWindowProc(Window, Message, WParam, LParam);
		}break;
	}
	return Result;
}

void InitD3D(HWND hWnd)
{
	//Create a struct to hold inforamtion about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;
	
	//clear out the struct for use
	ZeroMemory(&scd,sizeof(DXGI_SWAP_CHAIN_DESC));
	
	//fill the swap chain description
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 4;
	scd.Windowed = true;
	
	//create a device, device contex and swap chain using the inforamtion in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,
								  D3D_DRIVER_TYPE_HARDWARE,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  D3D11_SDK_VERSION,
								  &scd,
								  &swapchain,
								  &dev,
								  NULL,
								  &devcon);
}
void CleanD3D()
{
	//close and realese all exsisting COM objects
	swapchain->Release();
	dev->Release();
	devcon->Release();
}


int CALLBACK WinMain(HINSTANCE hInstance,
HINSTANCE hPrevInstance,
LPSTR lpCmdShow,
int nCmdShow)
{
	
	//WinMain for 3D
	running = true;
	HWND hWnd;
	
	WNDCLASSEX wc;
	
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = Win32MainWindowCallBack;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = (LPCSTR)L"WindowClass1";
	
	RegisterClassEx(&wc);
	
	hWnd = CreateWindowEx(NULL,
						  (LPCSTR)L"WindowClass1",
						  (LPCSTR)L"Our First Window Program",
						  WS_OVERLAPPEDWINDOW,
						  300,
						  300,
						  500,
						  400,
						  NULL,
						  NULL,
						  hInstance,
						  NULL);
	
	ShowWindow(hWnd, nCmdShow);
	
	MSG msg;
	while(GetMessage(&msg, NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;

	
	
}

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



	bool test(AABB box, v2 boxPos, Line line)
	{
		v2 toAABBVec = boxPos - line.startPoint;
		v2 lineDir = line.getLineDir();
		v2 lineCenter = (line.startPoint + lineDir.Scale(line.getLength() / 2.0));
		v2 lineCenterToAABB = toAABBVec - (lineCenter - line.startPoint);
		
		v2 lineAABBMin;
		lineAABBMin.x = (line.startPoint.x - line.width) < (line.endPoint.x - line.width) ? (line.startPoint.x - line.width) : (line.endPoint.x - line.width);
		lineAABBMin.y = (line.startPoint.y - line.width) < (line.endPoint.y - line.width) ? (line.startPoint.y - line.width) : (line.endPoint.y - line.width);
		v2 lineAABBMax;
		lineAABBMax.x = (line.startPoint.x + line.width) > (line.endPoint.x + line.width) ? (line.startPoint.x + line.width) : (line.endPoint.x + line.width);
		lineAABBMax.y = (line.startPoint.y + line.width) > (line.endPoint.y + line.width) ? (line.startPoint.y + line.width) : (line.endPoint.y + line.width);
		
		float lineAABBWidth = lineAABBMax.x - lineAABBMin.x;
		float lineAABBHeight = lineAABBMax.y - lineAABBMin.y;
		
		v2 lineAABBCorners[4];
		lineAABBCorners[0] = lineAABBMax;
		lineAABBCorners[1] = lineAABBMax - v2(0,lineAABBHeight);
		lineAABBCorners[2] = lineAABBMax - v2(lineAABBWidth,0);
		lineAABBCorners[3] = lineAABBMin;
		
		v2 AABBCorners[4];
		AABBCorners[0] = boxPos + v2( box.width,  box.height);
		AABBCorners[1] = boxPos + v2( box.width, -box.height);
		AABBCorners[2] = boxPos + v2(-box.width,  box.height);
		AABBCorners[3] = boxPos + v2(-box.width, -box.height);
		
		
		if(fabs(lineCenter.x - boxPos.x) > (lineAABBWidth / 2) + box.width)
			return false;
		if(fabs(lineCenter.y - boxPos.y) > (lineAABBHeight / 2) + box.height)
			return false;

		float projectionMin = (boxPos - line.startPoint).Dot(lineDir);
		float projectionMax = projectionMin;
		for(int i = 0; i < 4; i++)
		{
			float dot = (AABBCorners[i] - line.startPoint).Dot(lineDir);
			if(dot > projectionMax)
			{
				projectionMax = dot;
			}
			if(dot < projectionMin)
			{
				projectionMin = dot;
			}
		}
		if((projectionMax > -line.width && projectionMax < line.getLength() + line.width)
			|| (projectionMin > -line.width && projectionMin < line.getLength() + line.width)
			|| (projectionMin < -line.width && projectionMax > line.getLength() + line.width))
		{
			v2 lineDirPerp = lineDir.getPerpendicular().Normalize();
			projectionMin = (boxPos - line.startPoint).Dot(lineDirPerp);
			projectionMax = projectionMin;
			for(int i = 0; i < 4; i++)
			{
				float dot = (AABBCorners[i] - line.startPoint).Dot(lineDirPerp);
				if(dot > projectionMax)
				{
					projectionMax = dot;
				}
				if(dot < projectionMin)
				{
					projectionMin = dot;
				}
			}
			if((projectionMax > -line.width && projectionMax < line.width)
			|| (projectionMin > -line.width && projectionMin < line.width)
			|| (projectionMin < -line.width && projectionMax > line.width))
			{
				
				return true;
			}
			
			
		}
		return false;
	}


	
	
/*int CALLBACK WinMain(HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR CommandLine,
	int ShowCode)
{
	//old winmain

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
			
			double rotation = 0.0 * degreeToRadiance;
			
			struct OBB obb (v2(cos(rotation), sin(rotation)),v2(cos(rotation + 3.14 / 2), sin(rotation + 3.14 / 2)),50,50);
			struct OBB obb2 (v2(cos(rotation), sin(rotation)),v2(cos(rotation + 3.14 / 2), sin(rotation + 3.14 / 2)),100,50);
			
			struct Sphere sphere1(10);
			struct Sphere sphere2(20);
			
			struct Line line(pos, pos+(200,200),5);

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
				
				
				
				v2 toAABBVec = pos2 - line.startPoint;
				v2 lineDir = line.getLineDir();
				v2 lineCenter = line.startPoint + lineDir.Scale(line.getLength() / 2.0);
				v2 lineCenterToAABB = toAABBVec - (lineCenter - line.startPoint);
				v2 diff = lineDir.Scale(line.getLength() / 2.0);
				//GraphicsLib::Functions::RenderAlignedBox(&GlobalBackbuffer,lineCenter,AABB(fabs(diff.x) + line.width, fabs(diff.y) + line.width),Red);
				v2 aabbMin;
				aabbMin.x = (line.startPoint.x - line.width) < (line.endPoint.x - line.width) ? (line.startPoint.x - line.width) : (line.endPoint.x - line.width);
				aabbMin.y = (line.startPoint.y - line.width) < (line.endPoint.y - line.width) ? (line.startPoint.y - line.width) : (line.endPoint.y - line.width);
				v2 aabbMax;
				aabbMax.x = (line.startPoint.x + line.width) > (line.endPoint.x + line.width) ? (line.startPoint.x + line.width) : (line.endPoint.x + line.width);
				aabbMax.y = (line.startPoint.y + line.width) > (line.endPoint.y + line.width) ? (line.startPoint.y + line.width) : (line.endPoint.y + line.width);
				
				AABB lineAABB((aabbMax.x - aabbMin.x) / 2, (aabbMax.y - aabbMin.y) / 2);
				GraphicsLib::Functions::RenderAlignedBox(&GlobalBackbuffer,lineCenter,lineAABB,Red);
				
				//GraphicsLib::Functions::RenderRotatedBox(&GlobalBackbuffer,pos2,obb,White);
				//GraphicsLib::Functions::RenderAlignedBox(&GlobalBackbuffer, pos2, aabb1, White);
				GraphicsLib::Functions::RenderSphere(&GlobalBackbuffer,pos2,sphere2,White);
				
				v2 test1(100,0);
				v2 r = test1.getPerpendicular();
				
				v2 test2(100,100);
				r = test2.getPerpendicular();
				
				v2 test3(0,100);
				r = test3.getPerpendicular();
				
				
				
				line.endPoint = pos+v2(200,200);
				line.startPoint = pos;
				if(!MathLib::IntersectionTest::SpherevsLine(sphere2,pos2,line))
				{
					GraphicsLib::Functions::RenderLine(&GlobalBackbuffer, line, White);
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
}*/
