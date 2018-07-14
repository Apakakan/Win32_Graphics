#include "Model.h"


Model::Model()
{
	gp_VertexData		= nullptr;
	gm_NumOfVertexes	= 0;
	gp_VertexBuffer		= nullptr;
}
Model::~Model()
{
	Release();
}
void Model::Release()
{
	delete[]gp_VertexData;
}

void Model::CreateVertexBUffer(ID3D11Device *dev, ID3D11DeviceContext *devcon)
{
	gp_VertexData = (VERTEX*)malloc(sizeof(VERTEX) * 3);
	gp_VertexData[0] = { 0.0f,  1.0f, 1.0f, 1.0f,0.0f,0.0f,1.0f };
	gp_VertexData[1] = { 0.9f, -0.9f, 1.0f, 1.0f,0.0f,0.0f,1.0f };
	gp_VertexData[2] = { -0.9f, -0.9f,1.0f, 1.0f,0.0f,0.0f,1.0f };

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX) * gm_NumOfVertexes;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr;
	hr = dev->CreateBuffer(&bd, NULL, &gp_VertexBuffer);

	D3D11_MAPPED_SUBRESOURCE ms;
	hr = devcon->Map(gp_VertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, gp_VertexData, sizeof(VERTEX) * gm_NumOfVertexes);
	devcon->Unmap(gp_VertexBuffer, NULL);
}

void Model::SetVertexData(VERTEX *vertex_data, int num_of_vertexes, ID3D11Device *dev, ID3D11DeviceContext *devcon)
{
	gp_VertexData		= vertex_data;
	gm_NumOfVertexes	= num_of_vertexes;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = gp_VertexData;

	HRESULT hr;
	hr = dev->CreateBuffer(&bd, &data, &gp_VertexBuffer);


}
void Model::SetPosition(DirectX::XMFLOAT3 position)
{
	gm_Position = position;
}

VERTEX* Model::GetVertexData()
{
	return gp_VertexData;
}
int Model::GetNumOfVertexes()
{
	return gm_NumOfVertexes;
}
ID3D11Buffer* Model::GetVertexBuffer()
{
	return gp_VertexBuffer;
}
DirectX::XMFLOAT3 Model::GetPosition()
{
	return gm_Position;
}
DirectX::XMMATRIX Model::GetWorldMatrix()
{
	return DirectX::XMMatrixTranslation(gm_Position.x, gm_Position.y, gm_Position.z);
}