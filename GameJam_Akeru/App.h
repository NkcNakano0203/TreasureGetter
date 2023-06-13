#pragma once

#include <Windows.h>		// Windowsプログラムで必要になるものが入っている
#include <D3D11.h>			// DirectX11SDKを使うために必要なもの
#include <d3dcompiler.h>	// シェーダーファイルをコンパイルするために必要
#include <directxcolors.h>	// DirectX::ライブラリを利用するために必要
#include <wrl/client.h>		// DirectXのスマートポインタ

#pragma comment(lib, "d3d11.lib")		// DirectX11ライブラリを利用するために必要
#pragma comment(lib, "d3dCompiler.lib")	// シェーダーコードをコンパイルするために必要


constexpr auto WINDOW_TITLE = L"GAMEAWEEK";	// ウィンドウタイトル
constexpr auto CLASS_NAME = L"DIRECTXSAMPLE";	// ウィンドウクラス名
constexpr int32_t WINDOW_W = 1280;				// ウィンドウサイズ幅
constexpr int32_t WINDOW_H = 720;				// ウィンドウサイズ高さ
constexpr float MIN_FRAME_TIME = 1.0f / 60.0f;	// FPS

class App
{
public:
	static App* GetInstance()
	{
		static App instance;
		return &instance;
	}
	bool InitApp();			// 初期化処理管理
	void TermApp();			// 終了処理管理
	void RenderBegin(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);// 描画開始
	void RenderEnd();		// 描画終了

	Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() {return pDevice;}
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetDeviceContext() {return pImmediateContext;}
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetConstantBuffer() {return pConstantBuffer;}

private:
	App();					// コンストラクタ
	~App();					// デストラクタ

	HWND hWnd;				// ウィンドウハンドル
	HINSTANCE hInstance;	// インスタンスハンドル

	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;						// デバイス
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pImmediateContext = nullptr;	// デバイスコンテキスト
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain = nullptr;				// スワップチェイン
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView = nullptr;	// 描画するためのリソースを作成し、実際の描画操作を行う
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil = nullptr;			// 深度/ステンシルリソースとして使用する
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView = nullptr;	// 深度/ステンシルリソースにアクセスする
	Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState = nullptr;				// アルファブレンディング
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizerState = nullptr;	// ラスタライザ
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer = nullptr;				// 座標系データを保存

	bool InitWnd();			// ウィンドウ初期化
	void TermWnd();			// ウィンドウ終了
	bool InitD3D();			// D3Dデバイス初期化
	void TermD3D();			// D3Dデバイス終了

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};