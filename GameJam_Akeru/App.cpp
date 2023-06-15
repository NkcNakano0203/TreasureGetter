#include "App.h"
#include "Shader.h"
#include "Texture.h"
#include "Matrix.h"

// コンストラクタ
App::App()
	:hWnd(nullptr)
	,hInstance(nullptr)
{
}

// デストラクタ
App::~App()
{
}

// 初期化処理管理
bool App::InitApp()
{
	// ウィンドウ初期化
	if (!InitWnd())	return false;

	// D3Dデバイス初期化
	if (!InitD3D())	return false;

	// ウィンドウを表示.
	ShowWindow(hWnd, SW_SHOWNORMAL);

	// ウィンドウを更新.
	UpdateWindow(hWnd);

	// ウィンドウにフォーカスを設定.
	SetFocus(hWnd);

	// 正常終了
	return true;
}

// 終了処理管理
void App::TermApp()
{
	// D3Dデバイス終了
	TermD3D();

	// ウィンドウ終了
	TermWnd();
}

// 描画開始
void App::RenderBegin(float r, float g, float b, float a)
{
	float color[4] = { r, g, b, a };
	// 背景の色を設定 レンダーターゲットクリア
	pImmediateContext.Get()->ClearRenderTargetView(pRenderTargetView.Get(), color);
	// 深度/ステンシルバッファクリア
	pImmediateContext.Get()->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

// 描画終了
void App::RenderEnd()
{
	pSwapChain.Get()->Present(0, 0);
}

// ウィンドウ初期化
bool App::InitWnd()
{
	// ウィンドウクラス登録
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;	// ウィンドウスタイル
	wcex.lpfnWndProc = WndProc;				// ウィンドウのメッセージを処理するコールバック関数へのポインタ
	wcex.cbClsExtra = 0;					// ウィンドウクラス構造体の後ろに割り当てる補足のバイト数
	wcex.cbWndExtra = 0;					// ウィンドウインスタンスの後ろに割り当てる補足のバイト数
	wcex.hInstance = hInstance;				// このクラスのためのウィンドウプロシージャがあるハンドル
	wcex.hIcon = nullptr;					// アイコン
	wcex.hCursor = nullptr;					// マウスカーソル
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);// ウィンドウ背景色
	wcex.lpszMenuName = nullptr;			// デフォルトメニュー名
	wcex.lpszClassName = CLASS_NAME;		// ウィンドウクラス名
	wcex.hIconSm = nullptr;					// 小さいアイコン
	RegisterClassEx(&wcex);					// ウィンドウクラス登録

	// ウィンドウ生成 成功時はウィンドウハンドル、失敗時はnullが返る
	hWnd = CreateWindow(
		CLASS_NAME,					// RegisterClass()で登録したクラスの名前
		WINDOW_TITLE,				// タイトルバー
		WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,// ウィンドウスタイル
		CW_USEDEFAULT,				// ウィンドウ左上x座標
		CW_USEDEFAULT,				// ウィンドウ左上y座標
		WINDOW_W,						// ウィンドウ幅
		WINDOW_H,						// ウィンドウ高さ
		nullptr,					// 親ウィンドウのハンドル
		nullptr,					// メニューのハンドル
		wcex.hInstance,				// ウィンドウを作成するモジュールのハンドル
		nullptr						// WM_CREATEでLPARAMに渡したい値
	);

	// 正常終了
	return true;
}

// ウィンドウ終了
void App::TermWnd()
{	
	// ウィンドウの登録を解除
	if (hInstance != nullptr)
	{
		UnregisterClass(CLASS_NAME, hInstance);
	}

	hInstance = nullptr;
	hWnd = nullptr;
}

// D3Dデバイス初期化
bool App::InitD3D()
{
	// 生成が正常に行われたか判断する為に用意
	HRESULT hr = S_OK;

	// デバイスとスワップチェインの初期化、設定
	{
		DXGI_SWAP_CHAIN_DESC desc;			// スワップ チェーンを記述
		ZeroMemory(&desc, sizeof(desc));	// 初期化
		desc.BufferCount = 1;				// バックバッファの数
		desc.BufferDesc.Width = WINDOW_W;	// バックバッファの幅
		desc.BufferDesc.Height = WINDOW_H;	// バックバッファの高さ
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// フォーマット
		desc.BufferDesc.RefreshRate.Numerator = 60;			// リフレッシュレート（分子）
		desc.BufferDesc.RefreshRate.Denominator = 1;		// リフレッシュレート（分母）
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;// 走査線描画
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;// スケーリング
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// バックバッファの使用方法
		desc.OutputWindow = hWnd;		// 出力ウィンドウへの関連付け
		desc.SampleDesc.Count = 1;		// ピクセル単位のマルチサンプリングの数
		desc.SampleDesc.Quality = 0;	// マルチサンプリングのクオリティ
		desc.Windowed = TRUE;			// TRUE:ウィンドウモード FALES:フルスクリーンモード
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // スワップ チェーンの動作のオプション

		D3D_FEATURE_LEVEL level;
		// デバイスとスワップチェインの作成
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,					// デバイスの作成時に使用するビデオアダプタへのポインタ
			D3D_DRIVER_TYPE_HARDWARE,	// 作成するデバイスの種類を表す D3D_DRIVER_TYPE 
			nullptr,					// ソフトウェアラスタライザーを実装するDLLのハンドル
			0,							// 有効にするランタイムレイヤー
			nullptr,					// 作成を試みる機能レベルの順序を指定
			0,							// pFeatureLevelsの要素数
			D3D11_SDK_VERSION,			// SDKのバージョン
			&desc,						// スワップ チェーンの初期化パラメーターを格納するスワップチェインの記述へのポインタ
			pSwapChain.GetAddressOf(),	// レンダリングに使用するスワップチェインを表すIDXGISwapChainオブジェクトへのポインタのアドレスを返す
			pDevice.GetAddressOf(),		// 作成されたデバイスを表すID3D11Deviceオブジェクトへのポインタのアドレスを返す
			&level,						// このデバイスでサポートされている機能レベルの配列にある最初の要素を表すD3D_FEATURE_LEVELへのポインタを返す
			pImmediateContext.GetAddressOf());// デバイス コンテキストを表すID3D11DeviceContextオブジェクトへのポインタのアドレスを返す

		// エラーチェック
		if (FAILED(hr))	return false;
	}

	// バックバッファを描画ターゲットに設定		
	{
		// スワップチェインから最初のバックバッファを取得する
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;	// バッファのアクセスに使うインターフェイス(バッファを2Dテクスチャとして扱う)
		hr = pSwapChain.Get()->GetBuffer(
			0,									// バックバッファの番号
			__uuidof(ID3D11Texture2D),			// バッファにアクセスするインターフェイス
			(LPVOID*)pBackBuffer.GetAddressOf()	// バッファを受け取る変数
		);

		// エラーチェック
		if (FAILED(hr))	return false;

		// バックバッファの描画ターゲットビューを作る
		hr = pDevice->CreateRenderTargetView(
			pBackBuffer.Get(),				// ビューでアクセスするリソース
			nullptr,						// 描画ターゲットビューの定義
			pRenderTargetView.GetAddressOf()// 描画ターゲットビューを受け取る変数
		);

		// エラーチェック
		if (FAILED(hr))	return false;
	}

	// 深度/ステンシルテクスチャの作成
	{
		D3D11_TEXTURE2D_DESC descDepth;
		descDepth.Width = WINDOW_W;						// 幅
		descDepth.Height = WINDOW_H;					// 高さ
		descDepth.MipLevels = 1;						// ミップマップ レベル数
		descDepth.ArraySize = 1;						// 配列サイズ
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;		// フォーマット(深度のみ)
		descDepth.SampleDesc.Count = 1;					// マルチサンプリングの設定
		descDepth.SampleDesc.Quality = 0;				// マルチサンプリングの品質
		descDepth.Usage = D3D11_USAGE_DEFAULT;			// 使用方法 デフォルト
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// 深度/ステンシルとして使用
		descDepth.CPUAccessFlags = 0;					// CPUからアクセスしない
		descDepth.MiscFlags = 0;						// その他の設定なし

		// 2Dテクスチャーの配列を作成
		hr = pDevice->CreateTexture2D(
			&descDepth,					// 作成する2Dテクスチャ
			nullptr,					// サブリソースの記述の配列へのポインタ
			pDepthStencil.GetAddressOf()// 作成したテクスチャを受け取る
		);

		// エラーチェック
		if (FAILED(hr))	return false;

		// 深度/ステンシルビューの作成
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		descDSV.Format = descDepth.Format;	// リソース データのフォーマット
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;// リソースのタイプ
		descDSV.Flags = 0;					// テクスチャーが読み取り専用かどうか
		descDSV.Texture2D.MipSlice = 0;		// 2Dテクスチャーのサブリソースを指定
		hr = pDevice->CreateDepthStencilView(
			pDepthStencil.Get(),			// 深度/ステンシルビューを作るテクスチャ
			&descDSV,						// 深度/ステンシルビューの設定
			pDepthStencilView.GetAddressOf()// 作成したビューを受け取る
		);

		// エラーチェック
		if (FAILED(hr))	return false;

		// 描画ターゲットビューを出力マージャーの描画ターゲットとして設定
		pImmediateContext.Get()->OMSetRenderTargets(
			1,									// 描画ターゲットの数
			pRenderTargetView.GetAddressOf(),	// 描画ターゲットビューの配列
			/*pDepthStencilView.Get()*/
			nullptr								// 深度/ステンシルビューを設定しない
		);
	}

	// ラスタライズステートオブジェクトの設定
	{
		D3D11_RASTERIZER_DESC descRast = {
			D3D11_FILL_SOLID,	// レンダリング時に使用する描画モードを設定
			D3D11_CULL_NONE,	// カリングの設定
			FALSE,				// 三角形が前向きか後ろ向きかを設定する
			0,					// 指定のピクセルに加算する深度値
			0.0f,				// ピクセルの最大深度バイアス
			0.0f,				// ピクセルの傾斜に基づいてバイアスをかける
			FALSE,				// 距離に基づいてクリッピングを有効にする
			FALSE,				// シザー矩形カリングを有効にする
			FALSE,				// マルチサンプリングのアンチエイリアシングを有効にする
			FALSE				// 線のアンチエイリアシングを有効にする
		};

		// ラスタライズステートオブジェクトの作成
		pDevice.Get()->CreateRasterizerState(
			&descRast,						// ラスタライザステートの記述へのポインタ
			pRasterizerState.GetAddressOf()	// 作成されるラスタライザステートオブジェクトへのポインタのアドレス
		);

		//ラスタライザーをコンテキストに設定
		pImmediateContext.Get()->RSSetState(pRasterizerState.Get());
	}

	// ブレンディングステートの作成
	{
		D3D11_BLEND_DESC descBlend;
		ZeroMemory(&descBlend, sizeof(descBlend));
		descBlend.AlphaToCoverageEnable = FALSE;		// ピクセルをレンダーターゲットに設定するときに、アルファトゥカバレッジをマルチサンプリングテクニックとして使用するかどうかを決定
		descBlend.IndependentBlendEnable = FALSE;		// 同時処理のレンダーターゲットで独立したブレンディングを有効にするかどうか
		descBlend.RenderTarget[0].BlendEnable = TRUE;	// ブレンディングを有効にする
		descBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;		// 最初のRGBデータソースを指定
		descBlend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;// 2番目のRGBデータソースを指定
		descBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;			// RGBデータソースの組合せ方法を定義
		descBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;		// 最初のアルファデータソースを指定
		descBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;	// 2番目のアルファデータソースを指定
		descBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;	// アルファデータソースの組合せ方法を定義
		descBlend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // 書き込みマスク
		pDevice.Get()->CreateBlendState(
			&descBlend,					// ブレンディングステートの記述へのポインタ
			pBlendState.GetAddressOf()	// 作成されるオブジェクトへのポインタのアドレス
		);

		// ブレンディングステートの設定
		float blendFactor[4] = {
			D3D11_BLEND_ZERO,
			D3D11_BLEND_ZERO,
			D3D11_BLEND_ZERO,
			D3D11_BLEND_ZERO
		};

		// ブレンディングステートの設定
		pImmediateContext.Get()->OMSetBlendState(
			pBlendState.Get(),	// ブレンディングステートへのポインタ
			blendFactor,	// ブレンディング係数の配列
			0xffffffff		// サンプルマスク
		);
	}

	// 座標変換行列
	{
		D3D11_BUFFER_DESC descCBuffer;
		// 初期化
		ZeroMemory(&descCBuffer, sizeof(descCBuffer));
		// バッファで想定されている読み込み及び書き込みの方法を識別
		descCBuffer.Usage = D3D11_USAGE_DEFAULT;
		// バッファのサイズ
		descCBuffer.ByteWidth = sizeof(ConstantBuffer);
		// バッファをどのようにグラフィックスパイプラインにバインドするかを識別
		descCBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		// CPUのアクセスフラグ
		descCBuffer.CPUAccessFlags = 0;

		// バッファの作成
		hr = pDevice.Get()->CreateBuffer(
			// バッファの記述へのポインタ
			&descCBuffer,
			// 初期化データへのポインタ
			nullptr,
			// 作成されるバッファへのポインタのアドレス
			pConstantBuffer.GetAddressOf()
		);

		// エラーチェック
		if (FAILED(hr))	return false;
	}

	// 固定パイプライン用にシェーダーデータ読み込み
	{
		Shader::GetInstance()->LoadShader("VertexShader.cso", "PixelShader.cso");
	}
	// 画像読み込み
	{
		Texture::GetInstance()->LoadTexture("TitleLogo.png");
		Texture::GetInstance()->LoadTexture("StartUI.png");
		Texture::GetInstance()->LoadTexture("Player.png");
		Texture::GetInstance()->LoadTexture("Enemy.png");
		Texture::GetInstance()->LoadTexture("BoxClose.png");
		Texture::GetInstance()->LoadTexture("BoxOpen.png");
		Texture::GetInstance()->LoadTexture("Treasure.png");
		Texture::GetInstance()->LoadTexture("ClearImage.png");
	}

	// 正常終了
	return true;
}

// D3Dデバイス終了
void App::TermD3D()
{
	pRasterizerState.Reset();
	pBlendState.Reset();
	pDepthStencilView.Reset();
	pDepthStencil.Reset();
	pRenderTargetView.Reset();
	pSwapChain.Reset();
	pImmediateContext.Reset();
	pDevice.Reset();
}

LRESULT CALLBACK App::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:		// 閉じるボタンが押されたとき
		PostMessage(hWnd, WM_DESTROY, 0, 0);	// WM_DESTROYへ
		break;
	case WM_DESTROY:	// ウィンドウが終了するとき
		PostQuitMessage(0);	// WM_QUITを送信する
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)// escキーでウィンドウ閉じる
			DestroyWindow(hWnd);
		break;
	}
	// 既定のウィンドウプロシージャを呼び出して処理を返す
	return DefWindowProc(hWnd, message, wParam, lParam);
}
