#include "Model.h"


Model::Model()
{
	gp_VertexData		= nullptr;
	gm_NumOfVertexes	= 0;
	gp_VertexBuffer		= nullptr;
	gp_IndexBuffer		= nullptr;
}
Model::~Model()
{
	Release();
}
void Model::Release()
{
	delete[]gp_VertexData;
}

void Model::CreateVertexBuffer(ID3D11Device *dev, ID3D11DeviceContext *devcon)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX) * gm_NumOfVertexes;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = gp_VertexData;

	HRESULT hr;
	hr = dev->CreateBuffer(&bd, &data, &gp_VertexBuffer);

	CreatetIndexBuffer(dev, devcon);

}
void Model::CreatetIndexBuffer(ID3D11Device *dev, ID3D11DeviceContext *devcon)
{
	unsigned int indexes[] = { 0, 1, 3, 1,2,3 };

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(unsigned int) * 6;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = indexes;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr;
	hr = dev->CreateBuffer(&desc, &data, &gp_IndexBuffer);
}

void Model::SetVertexData(VERTEX *vertex_data, int num_of_vertexes, ID3D11Device *dev, ID3D11DeviceContext *devcon)
{
	gp_VertexData		= vertex_data;
	gm_NumOfVertexes	= num_of_vertexes;

	CreateVertexBuffer(dev,devcon);
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
ID3D11Buffer* Model::GetIndexBuffer()
{
	return gp_IndexBuffer;
}
DirectX::XMFLOAT3 Model::GetPosition()
{
	return gm_Position;
}
DirectX::XMMATRIX Model::GetWorldMatrix()
{
	return DirectX::XMMatrixTranslation(gm_Position.x, gm_Position.y, gm_Position.z);
}