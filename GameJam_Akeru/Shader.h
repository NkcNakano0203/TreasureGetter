#pragma once
#include <D3D11.h>			// DirectX11SDKを使うために必要なもの
#include <d3dcompiler.h>	// シェーダーファイルをコンパイルするために必要
#include <string>
#include <wrl/client.h>		// DirectXのスマートポインタ
#include <vector>

#pragma comment(lib, "d3d11.lib")		// DirectX11ライブラリを利用するために必要
#pragma comment(lib, "d3dCompiler.lib")	// シェーダーコードをコンパイルするために必要

struct ShaderResources
{
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pVertexLayout = nullptr;		// 頂点データの構造を定義
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;			// 頂点データを保存
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader = nullptr;		// 頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;		// ピクセルシェーダ
};

class Shader
{
public:
	static Shader* GetInstance()
	{
		static Shader instance;
		return &instance;
	}
	void Term();
	int32_t LoadShader(const std::string& vs, const std::string& ps);
	ShaderResources* GetShader(int32_t num = 0);
private:
	Shader() :cnt(0), CsoSize(0), pCsoBin(nullptr) {};
	~Shader() {};
	int32_t CreateVS(const std::string& vsStr, int32_t n);	// 頂点シェーダ作成
	int32_t CreatePS(const std::string& psStr, int32_t n);	// ピクセルシェーダ作成
	std::vector<ShaderResources> pSr;
	int32_t cnt;
	int32_t CsoSize;
	uint8_t* pCsoBin;

	HRESULT CompileShaderFromFile(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	void    ReadCSOFile(LPCWSTR szFileName);

};