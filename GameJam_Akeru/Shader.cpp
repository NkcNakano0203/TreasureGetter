#include <fstream>
#include "Shader.h"
#include "App.h"


D3D11_INPUT_ELEMENT_DESC vertexDesc[]
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

void Shader::Term()
{
	// 要素番号1以降のデータを全部リセット
	for (auto sr = pSr.begin() + 1; sr != pSr.end(); ++sr)
	{
		sr->pVertexLayout.Reset();
		sr->pVertexBuffer.Reset();
		sr->pVertexShader.Reset();
		sr->pPixelShader.Reset();
	}
	cnt = 1;
}

int32_t Shader::LoadShader(const std::string& vs, const std::string& ps)
{
	pSr.emplace_back();
	CreateVS(vs, cnt);
	CreatePS(ps, cnt);
	return cnt++;
}

ShaderResources* Shader::GetShader(int32_t num)
{
	return &pSr[num];
}

// 頂点シェーダ作成
int32_t Shader::CreateVS(const std::string& vsStr, int32_t n)
{

	// 生成が正常に行われたか判断する為に用意
	HRESULT hr = S_OK;
	// ファイルの拡張子を取得する
	size_t num = vsStr.find_last_of('.');
	std::string extension = vsStr.substr(num + 1);
	// stringからconst wchar_tに変換する
	wchar_t* vs = new wchar_t[vsStr.length() + 1];
	size_t value = 0;
	mbstowcs_s(&value, vs, 20, vsStr.c_str(), vsStr.length() + 1);


	// 読み込む拡張子がhlsl?
	if (extension == "hlsl")
	{
		Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob = nullptr;	// オブジェクトコードとエラーメッセージを返す
												
		// シェーダーファイル読み込み
		hr = CompileShaderFromFile(
			vs,						// ファイルパス
			"main",					// エントリーポイントの名前
			"vs_5_0",				// シェーダーのバージョン
			pVSBlob.GetAddressOf()	// 保存する変数 
		);
		if (FAILED(hr))	return false;	// エラーチェック

		// 頂点シェーダの作成
		hr = App::GetInstance()->GetDevice().Get()->CreateVertexShader(
			pVSBlob.Get()->GetBufferPointer(),	// コンパイル済みシェーダへのポインタ
			pVSBlob.Get()->GetBufferSize(),		// コンパイル済みシェーダのサイズ
			nullptr,							// クラスリンクインターフェイスへのポインタ
			pSr[n].pVertexShader.GetAddressOf()	// ID3D11VertexShaderインターフェイスへのポインタのアドレス
		);
		if (FAILED(hr))	return false;	// エラーチェック

		// インプットレイアウトの作成
		hr = App::GetInstance()->GetDevice().Get()->CreateInputLayout(
			vertexDesc,							// 入力データ型の配列
			ARRAYSIZE(vertexDesc),				// 配列内の入力データ型の数
			pVSBlob.Get()->GetBufferPointer(),	// コンパイル済みシェーダへのポインタ
			pVSBlob.Get()->GetBufferSize(),		// コンパイル済みシェーダのサイズ
			pSr[n].pVertexLayout.GetAddressOf()	// 作成される入力レイアウトオブジェクトへのポインタ
		);
		pVSBlob.Reset();				// 解放		
		if (FAILED(hr))	return false;	// エラーチェック
	}
	// 読み込む拡張子がcso?
	else if (extension == "cso")
	{
		ReadCSOFile(vs);

		// 頂点シェーダの作成
		hr = App::GetInstance()->GetDevice().Get()->CreateVertexShader(
			pCsoBin,							// コンパイル済みシェーダへのポインタ
			CsoSize,							// コンパイル済みシェーダのサイズ
			nullptr,							// クラスリンクインターフェイスへのポインタ
			pSr[n].pVertexShader.GetAddressOf()	// ID3D11VertexShaderインターフェイスへのポインタのアドレス
		);
		if (FAILED(hr))	return false;	// エラーチェック

		// インプットレイアウトの作成
		hr = App::GetInstance()->GetDevice().Get()->CreateInputLayout(
			vertexDesc,							// 入力データ型の配列
			ARRAYSIZE(vertexDesc),				// 配列内の入力データ型の数
			pCsoBin,							// コンパイル済みシェーダへのポインタ
			CsoSize,							// コンパイル済みシェーダのサイズ
			pSr[n].pVertexLayout.GetAddressOf()	// 作成される入力レイアウトオブジェクトへのポインタ
		);
		if (FAILED(hr))	return false;	// エラーチェック
	}
	return true;
}

// ピクセルシェーダ作成
int32_t Shader::CreatePS(const std::string& psStr, int32_t n)
{
	// 生成が正常に行われたか判断する為に用意
	HRESULT hr = S_OK;
	// ファイルの拡張子を取得する
	size_t num = psStr.find_last_of('.');
	std::string extension = psStr.substr(num + 1);
	// stringからconst wchar_tに変換する
	wchar_t* ps = new wchar_t[psStr.length() + 1];
	size_t value = 0;
	mbstowcs_s(&value, ps, 20, psStr.c_str(), psStr.length() + 1);


	// 読み込む拡張子がhlsl?
	if (extension == "hlsl")
	{
		Microsoft::WRL::ComPtr<ID3DBlob> pPSBlob = nullptr;	// オブジェクトコードとエラーメッセージを返す

		// シェーダーファイル読み込み
		hr = CompileShaderFromFile(
			ps,						// ファイルパス
			"main",					// エントリーポイントの名前
			"ps_5_0",				// シェーダーのバージョン
			pPSBlob.GetAddressOf()	// 保存する変数 
		);
		if (FAILED(hr))	return false;	// エラーチェック

		// ピクセルシェーダの作成
		hr = App::GetInstance()->GetDevice().Get()->CreatePixelShader(
			pPSBlob.Get()->GetBufferPointer(),	// コンパイル済みシェーダへのポインタ
			pPSBlob.Get()->GetBufferSize(),		// コンパイル済みシェーダのサイズ
			nullptr,							// クラスリンクインターフェイスへのポインタ
			pSr[n].pPixelShader.GetAddressOf()	// ID3D11PixelShaderインターフェイスへのポインタのアドレス
		);
		pPSBlob.Reset();				// 解放		
		if (FAILED(hr))	return false;	// エラーチェック
	}
	// 読み込む拡張子がcso?
	else if (extension == "cso")
	{
		ReadCSOFile(ps);

		// ピクセルシェーダの作成
		hr = App::GetInstance()->GetDevice().Get()->CreatePixelShader(
			pCsoBin,						// コンパイル済みシェーダへのポインタ
			CsoSize,						// コンパイル済みシェーダのサイズ
			nullptr,						// クラスリンクインターフェイスへのポインタ
			pSr[n].pPixelShader.GetAddressOf()// ID3D11PixelShaderインターフェイスへのポインタのアドレス
		);
		if (FAILED(hr))	return false;	// エラーチェック
	}
	return true;
}

// シェーダーファイル読み込み
HRESULT Shader::CompileShaderFromFile(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	// コンパイラがHLSLコードをコンパイルする方法を指定
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;// 厳密なコンパイルを強制
#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;				// デバッグファイル/行/型/シンボル情報を出力コードに挿入するようにコンパイラに指示

	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;	// コード生成中に最適化ステップをスキップするようにコンパイラに指示
#endif

	Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob = nullptr;	// コンパイル後のバイナリデータを取得する
													// ファイルからシェーダをコンパイル
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, pErrorBlob.GetAddressOf());

	// エラーチェック
	if (FAILED(hr))
	{
		if (pErrorBlob.Get())
		{
			// 文字列をデバッガに送る
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob.Get()->GetBufferPointer()));
			pErrorBlob.Reset();
		}
		return hr;
	}
	pErrorBlob.Reset();
	return hr;
}

// CSOファイルを読み込む
void  Shader::ReadCSOFile(LPCWSTR szFileName)
{
	std::ifstream binfile(szFileName, std::ios::in | std::ios::binary);

	if (binfile.is_open()) {
		int32_t fsize = static_cast<int32_t>(binfile.seekg(0, std::ios::end).tellg());
		binfile.seekg(0, std::ios::beg);
		uint8_t* code(new uint8_t[fsize]);
		binfile.read((char*)code, fsize);
		CsoSize = fsize;
		pCsoBin = std::move(code);
	}
}
