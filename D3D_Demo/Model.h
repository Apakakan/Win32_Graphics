#ifndef MODEL_H
#define MODEL_H

#include <DirectXMath.h>
#include <d3d11.h>

struct VERTEX
{
	float X, Y, Z;
	float R, G, B, A;
};

class Model
{
private:
	VERTEX *gp_VertexData;
	int gm_NumOfVertexes;
	
	ID3D11Buffer *gp_VertexBuffer;

	VERTEX datatest[3];

	DirectX::XMFLOAT3 gm_Position;
	void CreateVertexBUffer(ID3D11Device *dev, ID3D11DeviceContext *devcon);
public:
	Model();
	~Model();
	void Release();

	void SetVertexData(VERTEX *vertex_data, int num_of_vertexes, ID3D11Device *dev, ID3D11DeviceContext *devcon);
	void SetPosition(DirectX::XMFLOAT3 position);

	VERTEX* GetVertexData();
	int GetNumOfVertexes();
	ID3D11Buffer* GetVertexBuffer();
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMMATRIX GetWorldMatrix();

};

#endif