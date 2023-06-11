#pragma once
#include <tchar.h>			// テキストや文字列を扱うために必要なヘッダーファイル
#include <Windows.h>		// Windowsプログラムで必要になるものが入っている
#include <D3D11.h>			// DirectX11SDKを使うために必要なもの
#include <d3dcompiler.h>	// シェーダーファイルをコンパイルするために必要
#include <directxcolors.h>	// DirectX::ライブラリを利用するために必要
#include <wrl/client.h>		// DirectXのスマートポインタ
#include <vector>
#include <string>
#include "App.h"
#include "Shader.h"
#include "Texture.h"
#include "Matrix.h"

#pragma comment(lib, "d3d11.lib")		// DirectX11ライブラリを利用するために必要
#pragma comment(lib, "d3dCompiler.lib")	// シェーダーコードをコンパイルするために必要

// 頂点データ構造体
struct vertex
{
	DirectX::XMFLOAT3 pos;		// 頂点座標	
	DirectX::XMFLOAT4 col;		// 頂点ごとの色
	DirectX::XMFLOAT2 uv;		// テクスチャ座標
	DirectX::XMFLOAT3 normal;	// 法線
};

class CPolygon
{
private:
	ShaderResources* pSr;
	std::vector<ID3D11ShaderResourceView*> pTextures;

	vertex vertexList[4];
	DirectX::XMMATRIX	wld = DirectX::XMMatrixIdentity();
	DirectX::XMFLOAT3	scale = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3	axis = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	float				angle = 0.0f;
	DirectX::XMFLOAT3	pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	ConstantBuffer constantBuffer;

public:
	void Init(ShaderResources* sr);
	void Render(ConstantBuffer cb, TextureResources* tex = Texture::GetInstance()->GetTextureResource(0));

	void Term();
	void SetColor(float r, float g, float b, float a, int32_t v);
	void SetUV(float cx, float cy, float nx, float ny);
};