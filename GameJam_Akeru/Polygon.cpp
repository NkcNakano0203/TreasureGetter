#include "Polygon.h"
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>


void CPolygon::Init(ShaderResources* sr)
{
	pSr = sr;
	for (int32_t i = 0; i < 4; i++)
	{
		// 頂点の位置
		vertexList[i].pos.x = i < 2 ? -0.5f : 0.5f;
		vertexList[i].pos.y = i % 2 == 0 ? -0.5f : 0.5f;
		vertexList[i].pos.z = 0.0f;
		// 頂点の色
		vertexList[i].col.x = 1.0f;
		vertexList[i].col.y = 1.0f;
		vertexList[i].col.z = 1.0f;
		vertexList[i].col.w = 1.0f;
		// uv
		vertexList[i].uv.x = i < 2 ? 0.0f : 1.0f;
		vertexList[i].uv.y = i % 2 == 0 ? 1.0f : 0.0f;
		// normal
		vertexList[i].normal.x = 0.0f;
		vertexList[i].normal.y = 0.0f;
		vertexList[i].normal.z = 1.0f;
	}

	// バッファリソースについて記述
	D3D11_BUFFER_DESC descBuffer;
	// 初期化
	ZeroMemory(&descBuffer, sizeof(descBuffer));
	// バッファで想定されている読み込み及び書き込みの方法を識別
	descBuffer.Usage = D3D11_USAGE_DEFAULT;
	// バッファのサイズ
	descBuffer.ByteWidth = sizeof(vertex) * 4;
	// バッファをどのようにグラフィックスパイプラインにバインドするかを識別
	descBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// CPUのアクセスフラグ
	descBuffer.CPUAccessFlags = 0;

	// サブリソースの初期化に使用されるデータを指定
	D3D11_SUBRESOURCE_DATA InitData;
	// 初期化
	ZeroMemory(&InitData, sizeof(InitData));
	// 初期化データへのポインタ
	InitData.pSysMem = vertexList;
	// テクスチャにある1本の線の先端から隣の線までの距離
	InitData.SysMemPitch = 0;
	// 3Dテクスチャに関連する値
	InitData.SysMemSlicePitch = 0;
	// バッファの作成
	App::GetInstance()->GetDevice().Get()->CreateBuffer(
		// バッファの記述へのポインタ
		&descBuffer,
		// 初期化データへのポインタ
		&InitData,
		// 作成されるバッファへのポインタのアドレス
		pSr->pVertexBuffer.GetAddressOf()
	);
}

void CPolygon::Render(ConstantBuffer cb, TextureResources* tex)
{

	// ワールド座標行列を転置
	constantBuffer.world = DirectX::XMMatrixTranspose(cb.world);
	// ビュー座標行列を転置
	constantBuffer.view = DirectX::XMMatrixTranspose(cb.view);
	// プロジェクション座標行列を転置
	constantBuffer.projection = DirectX::XMMatrixTranspose(cb.projection);
	// データをコピーしてConstantBufferの内容を書き換える（更新する）
	App::GetInstance()->GetDeviceContext()->UpdateSubresource(App::GetInstance()->GetConstantBuffer().Get(), 0, nullptr, &constantBuffer, 0, 0);


	// サブリソースの更新
	App::GetInstance()->GetDeviceContext()->UpdateSubresource(
		pSr->pVertexBuffer.Get(),
		0,
		nullptr,
		vertexList,
		0,
		0
	);
	// 入力アセンブラステージに入力レイアウトオブジェクトをバインド
	App::GetInstance()->GetDeviceContext()->IASetInputLayout(pSr->pVertexLayout.Get());
	// 入力アセンブラステージに頂点バッファの配列をバインド
	UINT strides = sizeof(vertex);
	UINT offsets = 0;
	App::GetInstance()->GetDeviceContext()->IASetVertexBuffers(
		0,					// バインドに使用する最初の入力スロット
		1,					// 配列内の頂点バッファの数
		pSr->pVertexBuffer.GetAddressOf(),	// 頂点バッファの配列へのポインタ
		&strides,			// ストライド値
		&offsets			// オフセット値
	);
	// 頂点シェーダをデバイスに設定
	App::GetInstance()->GetDeviceContext()->VSSetShader(
		pSr->pVertexShader.Get(),// 頂点シェーダへのポインタ
		nullptr,			// クラスインスタンスインターフェイスの配列へのポインタ
		0					// 配列のクラスインスタンスインターフェイスの数
	);
	// 頂点シェーダのパイプラインステージで使用される定数バッファを設定
	App::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(
		0,					// デバイスの配列の中で定数バッファの設定を開始する位置
		1,					// 設定するバッファの数
		App::GetInstance()->GetConstantBuffer().GetAddressOf()	// デバイスに設定する定数バッファの配列
	);

	// ピクセルシェーダをデバイスに設定
	App::GetInstance()->GetDeviceContext()->PSSetShader(
		pSr->pPixelShader.Get(),// ピクセルシェーダへのポインタ
		nullptr, 				// クラスインスタンスインターフェイスの配列へのポインタ
		0						// 配列のクラスインスタンスインターフェイスの数
	);
	//テクスチャーをシェーダーに渡す
	App::GetInstance()->GetDeviceContext()->PSSetSamplers(
		0,
		1,
		tex->pSampler.GetAddressOf()
	);
	App::GetInstance()->GetDeviceContext()->PSSetShaderResources(
		0,
		1,
		tex->pTexture.GetAddressOf()
	);
	// 入力アセンブラステージにプリミティブの形状をバインド
	App::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(
		// 頂点データを三角形のリストとして解釈
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		// 複数の三角形を出したいときはTRIANGLESTRIPにすると頂点を節約できる
	);
	// プリミティブを描画
	App::GetInstance()->GetDeviceContext()->Draw(
		4,		// 描画する頂点の数
		0		// 最初の頂点のインデックス
	);

}

void CPolygon::Term()
{
}

void CPolygon::SetColor(float r, float g, float b, float a, int32_t v)
{
	vertexList[v].col.x = r;
	vertexList[v].col.y = g;
	vertexList[v].col.z = b;
	vertexList[v].col.w = a;
}

void CPolygon::SetUV(float cx, float cy, float nx, float ny)
{
	float w, h;
	// 縦横を指定した数に分割する
	w = 1.0f / cx;
	h = 1.0f / cy;

	vertexList[0].uv.x = w * nx;
	vertexList[0].uv.y = h * (ny + 1);

	vertexList[1].uv.x = w * nx;
	vertexList[1].uv.y = h * ny;

	vertexList[2].uv.x = w * (nx + 1);
	vertexList[2].uv.y = h * (ny + 1);

	vertexList[3].uv.x = w * (nx + 1);
	vertexList[3].uv.y = h * ny;
}
