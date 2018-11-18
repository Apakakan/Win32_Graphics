#ifndef MODEL_H
#define MODEL_H

#include <DirectXMath.h>
#include <d3d11.h>

struct VERTEX
{
	float X, Y, Z;
	//float R, G, B, A;
};
struct NORMAL
{
	float X, Y, Z;
};
struct UVCOORD
{
	float U,V;
};
struct VERTEX_FACE_INDEXES
{
	int pos, norm, uvcoord;
};

class Model
{
private:
	VERTEX *gp_VertexData;
	int gm_NumOfVertexes;
	
	ID3D11Buffer *gp_VertexBuffer;
	ID3D11Buffer *gp_IndexBuffer;

	unsigned int *gp_IndexList;
	int gm_NumOfIndexes;

	DirectX::XMFLOAT3 gm_Position;
	void CreateVertexBuffer(ID3D11Device *dev, ID3D11DeviceContext *devcon);
	void CreatetIndexBuffer(ID3D11Device *dev, ID3D11DeviceContext *devcon);
public:
	Model();
	~Model();
	void Release();

	void SetVertexData(VERTEX *vertex_data, int num_of_vertexes, unsigned int* index_list, int num_of_indexes, ID3D11Device *dev, ID3D11DeviceContext *devcon);
	void SetPosition(DirectX::XMFLOAT3 position);

	VERTEX* GetVertexData();
	int GetNumOfVertexes();
	ID3D11Buffer* GetVertexBuffer();

	unsigned int* GetIndexData();
	ID3D11Buffer* GetIndexBuffer();
	int GetNumIndexes();
	
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMMATRIX GetWorldMatrix();

};

#endif