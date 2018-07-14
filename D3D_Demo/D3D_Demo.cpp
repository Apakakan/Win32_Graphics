#include <windows.h>
//#include <windowsx.h>
//https://users.ece.cmu.edu/~eno/coding/CppCodingStandard.html#attrnames
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "Camera.h"
#include "Model.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#define SCREEN_WIDTH   800
#define SCREEN_HEIGHT  600




IDXGISwapChain * swapchain;
ID3D11Device *dev;
ID3D11DeviceContext *devcon;
ID3D11RenderTargetView *backbuffer;
ID3D11DepthStencilView *DSV;

ID3D11VertexShader *pVS;
ID3D11PixelShader *pPS;

ID3D11InputLayout *pLayout;

ID3D11Buffer* pVS_ConstantBuffer;

DirectX::XMMATRIX mWorld;
DirectX::XMMATRIX mProj;

Camera gCamera;

int gm_Nr_Of_Models = 1;
Model* gModels;

struct VS_CONSTANT_BUFFER
{
	DirectX::XMFLOAT4X4 mWorldViewProj;
};

void InitD3D(HWND hWnd);    
void RenderFrame();     
void CleanD3D(); 
void ReleaseObjects();
void InitGraphics();    
void InitPipeline(); 
void InitDepthStencil();
void CreateConstantBuffers();
void UpdateConstantBuffer();

DirectX::XMVECTOR toXMV3(DirectX::XMFLOAT3 pi_xmfloat3);	
DirectX::XMVECTOR toXMV4(DirectX::XMFLOAT4 pi_xmfloat4);	
DirectX::XMMATRIX toXMM3X3(DirectX::XMFLOAT3X3 pi_xmfloat3x3);
DirectX::XMMATRIX toXMM4X4(DirectX::XMFLOAT4X4 pi_xmfloat4x4);

void toXMF3(DirectX::XMFLOAT3 &pi_dest, DirectX::XMVECTOR pi_source);		
void toXMF4(DirectX::XMFLOAT4 &pi_dest, DirectX::XMVECTOR pi_source);		
void toXMF3X3(DirectX::XMFLOAT3X3 &pi_dest, DirectX::XMMATRIX pi_source);
void toXMF4X4(DirectX::XMFLOAT4X4 &pi_dest, DirectX::XMMATRIX pi_source);

LRESULT CALLBACK WindProc(HWND hWnd,
						  UINT message,
						  WPARAM wParam,
						  LPARAM lParam)
{
	LRESULT Result = 0;
	switch(message)
	{
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			break;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}break;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
	
}

void InitGraphics()
{
	VERTEX* ourVertecies = (VERTEX*)malloc(sizeof(VERTEX) * 3);
	ourVertecies[0] = { 0.0f,  1.0f, 1.0f, 1.0f,0.0f,0.0f,1.0f };
	ourVertecies[1] = { 0.9f, -0.9f, 1.0f, 1.0f,0.0f,0.0f,1.0f };
	ourVertecies[2] = { -0.9f, -0.9f,1.0f, 1.0f,0.0f,0.0f,1.0f };
	
	gModels[0].SetVertexData(ourVertecies, 3, dev, devcon);
	gModels[0].SetPosition(DirectX::XMFLOAT3(0, 0, 1));
	
}


void InitPipeline()
{
	ID3DBlob *VS, *PS;
	HRESULT result;
	ID3DBlob** ppErrorMsgs = nullptr;
	result = D3DCompileFromFile(L"shaders.hlsl",
						  nullptr,
						  nullptr,
						  "VShader",
						  "vs_4_0",
						  0,
						  0,
						  &VS,
						  ppErrorMsgs);
	result = D3DCompileFromFile(L"shaders.hlsl",
						  nullptr,
						  nullptr,
						  "PShader",
						  "ps_4_0",
						  0,
						  0,
						  &PS,
						  ppErrorMsgs);
	
	
	dev->CreateVertexShader(VS->GetBufferPointer(),VS->GetBufferSize(),NULL,&pVS);
	dev->CreatePixelShader(PS->GetBufferPointer(),PS->GetBufferSize(),NULL,&pPS);
	
	devcon->VSSetShader(pVS,0,0);
	devcon->PSSetShader(pPS,0,0);
	
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",   0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	dev->CreateInputLayout(ied,2,VS->GetBufferPointer(),VS->GetBufferSize(),&pLayout);
	devcon->IASetInputLayout(pLayout);
}

void InitDepthStencil()
{
	HRESULT hr;
	ID3D11Texture2D* pDepthStencil = nullptr;
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth,sizeof(D3D11_TEXTURE2D_DESC));
	descDepth.Width = SCREEN_WIDTH;
	descDepth.Height = SCREEN_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = dev->CreateTexture2D(&descDepth,NULL,&pDepthStencil);
	
	
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc,sizeof(D3D11_DEPTH_STENCIL_DESC));
		
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	ID3D11DepthStencilState* pDSState;
	dev->CreateDepthStencilState(&dsDesc,&pDSState);
	
	devcon->OMSetDepthStencilState(pDSState,1);
	
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV,sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	
	hr = dev->CreateDepthStencilView(pDepthStencil,&descDSV,&DSV);
	pDepthStencil->Release();
}

void CreateConstantBuffers()
{
	VS_CONSTANT_BUFFER bufferData;

	mWorld = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX view = gCamera.GetViewMatrix();
	mProj =  DirectX::XMMatrixPerspectiveFovLH(3.14f*0.45f, SCREEN_WIDTH / SCREEN_HEIGHT, 0.5f, 20.0f);

	DirectX::XMMATRIX mWorldView = DirectX::XMMatrixMultiply(mWorld, view);
	DirectX::XMMATRIX mWorldViewProj = DirectX::XMMatrixMultiply(mWorldView, mProj);

	toXMF4X4(bufferData.mWorldViewProj, DirectX::XMMatrixTranspose(mWorldViewProj));

	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
	cbDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &bufferData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	HRESULT hr;
	hr = dev->CreateBuffer(&cbDesc, &InitData, &pVS_ConstantBuffer);

	devcon->VSSetConstantBuffers(0, 1, &pVS_ConstantBuffer);


}

void InitD3D(HWND hWnd)
{
	
	DXGI_SWAP_CHAIN_DESC scd;
	
	ZeroMemory(&scd,sizeof(DXGI_SWAP_CHAIN_DESC));
	
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = SCREEN_WIDTH;
	scd.BufferDesc.Height = SCREEN_HEIGHT;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 1;
	scd.Windowed = true;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	
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

	ID3D11Texture2D *pBackbuffer;
	swapchain->GetBuffer(0,__uuidof(ID3D11Texture2D),(LPVOID*)&pBackbuffer);
	
	dev->CreateRenderTargetView(pBackbuffer,NULL,&backbuffer);
	pBackbuffer->Release();
	
	InitDepthStencil();
	
	devcon->OMSetRenderTargets(1,&backbuffer,DSV);
	
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;
	
	devcon->RSSetViewports(1,&viewport);
	
	CreateConstantBuffers();
	InitPipeline();
	InitGraphics();
}

void UpdateConstantBuffer()
{
	VS_CONSTANT_BUFFER bufferData;

	DirectX::XMMATRIX view = gCamera.GetViewMatrix();

	DirectX::XMMATRIX mWorldView = DirectX::XMMatrixMultiply(mWorld, view);
	DirectX::XMMATRIX mWorldViewProj = DirectX::XMMatrixMultiply(mWorldView, mProj);

	toXMF4X4(bufferData.mWorldViewProj, DirectX::XMMatrixTranspose(mWorldViewProj));

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE data;
	memset(&data,0, sizeof(D3D11_MAPPED_SUBRESOURCE));

	hr = devcon->Map(pVS_ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);

	memcpy(data.pData, &bufferData, sizeof(VS_CONSTANT_BUFFER));

	devcon->Unmap(pVS_ConstantBuffer, 0);
}

void UpdateAConstantBuffer(ID3D11Buffer* buffer, void* buffer_data, int size_of_data)
{

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE data;
	memset(&data, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

	hr = devcon->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);

	memcpy(data.pData, buffer_data, size_of_data);

	devcon->Unmap(buffer,0);
}

void CleanD3D()
{
	swapchain->SetFullscreenState(FALSE,NULL);
	DSV->Release();
	pVS_ConstantBuffer->Release();
	pLayout->Release();
	pVS->Release();
	pPS->Release();
	swapchain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();	
}	

void ReleaseObjects()
{
	int num_of_objects = sizeof(*gModels) / sizeof(VERTEX);
	for (int i = 0; i < num_of_objects; i++)
	{
		gModels[i].Release();
	}
}

void RenderFrame()
{
	const FLOAT clearColor[4] = {0.0f,0.2f,0.4f,1.0f};
	devcon->ClearRenderTargetView(backbuffer,clearColor);
	devcon->ClearDepthStencilView(DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 1);
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	for (int i = 0; i < gm_Nr_Of_Models; i++)
	{
		ID3D11Buffer* vBuffer = gModels[i].GetVertexBuffer();
		devcon->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
		devcon->VSSetConstantBuffers(0, 1, &pVS_ConstantBuffer);

		devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		//devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		devcon->Draw(gModels[i].GetNumOfVertexes(), 0);
	}

	swapchain->Present(0,0);
}
	
	
int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	HWND hWnd;
	
	WNDCLASSEX wc;
	
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "WindowClass1";
	
	RegisterClassEx(&wc);
	
	RECT wr = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
	AdjustWindowRect(&wr,WS_OVERLAPPEDWINDOW, FALSE);
	
	hWnd = CreateWindowEx(NULL,
						  "WindowClass1",
						  "Our First Window Program",
						  WS_OVERLAPPEDWINDOW,
						  300, 300,
						  SCREEN_WIDTH, SCREEN_HEIGHT,
						  NULL,
						  NULL,
						  hInstance,
						  NULL);
	
	ShowWindow(hWnd, nCmdShow);

	gCamera = Camera();
	gModels = (Model*)malloc(sizeof(Model) * gm_Nr_Of_Models);
	InitD3D(hWnd);
	

	MSG msg = {0};
	while(TRUE)
	{
		if(PeekMessage(&msg, NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if(msg.message == WM_QUIT)
				break;
		}
		gCamera.HandleInput();
		UpdateConstantBuffer();
		RenderFrame();
	}

	CleanD3D();
	ReleaseObjects();
	
	return msg.wParam;
}




DirectX::XMVECTOR toXMV3(DirectX::XMFLOAT3 pi_xmfloat3)
{
	return DirectX::XMLoadFloat3(&pi_xmfloat3);
}
DirectX::XMVECTOR toXMV4(DirectX::XMFLOAT4 pi_xmfloat4)
{
	return DirectX::XMLoadFloat4(&pi_xmfloat4);
}
DirectX::XMMATRIX toXMM3X3(DirectX::XMFLOAT3X3 pi_xmfloat3x3)
{
	return DirectX::XMLoadFloat3x3(&pi_xmfloat3x3);
}
DirectX::XMMATRIX toXMM4X4(DirectX::XMFLOAT4X4 pi_xmfloat4x4)
{
	return DirectX::XMLoadFloat4x4(&pi_xmfloat4x4);
}

void toXMF3(DirectX::XMFLOAT3 &pi_dest, DirectX::XMVECTOR pi_source)
{
	DirectX::XMStoreFloat3(&pi_dest, pi_source);
}
void toXMF4(DirectX::XMFLOAT4 &pi_dest, DirectX::XMVECTOR pi_source)
{
	DirectX::XMStoreFloat4(&pi_dest, pi_source);
}
void toXMF3X3(DirectX::XMFLOAT3X3 &pi_dest, DirectX::XMMATRIX pi_source)
{
	DirectX::XMStoreFloat3x3(&pi_dest, pi_source);
}
void toXMF4X4(DirectX::XMFLOAT4X4 &pi_dest, DirectX::XMMATRIX pi_source)
{
	DirectX::XMStoreFloat4x4(&pi_dest, pi_source);
}


















