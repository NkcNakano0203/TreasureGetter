#include <D3D11.h>			// DirectX11SDKを使うために必要なもの
#include <directxcolors.h>	// DirectX::ライブラリを利用するために必要
#include "Camera.h"
#include "App.h"

using namespace DirectX;

void Camera::SetViewPort()
{
	{
		// ビューポートの設定(描画領域)-1.0～1.0の範囲で作られたワールド座標をスクリーン座標に変換するための情報
		viewPort.TopLeftX = 0.0f;		// ビューポート領域の左上X座標
		viewPort.TopLeftY = 0.0f;		// ビューポート領域の左上Y座標
		viewPort.Width = WINDOW_W;	// ビューポート領域の幅
		viewPort.Height = WINDOW_H;	// ビューポート領域の高さ
		viewPort.MinDepth = 0.0f;		// ビューポート領域の深度値の最小値
		viewPort.MaxDepth = 1.0f;		// ビューポート領域の深度値の最大値

		// ラスタライザにビューポートを設定
		App::GetInstance()->GetDeviceContext().Get()->RSSetViewports(
			1,				// 設定するビューポートの数
			&viewPort		// 設定するD3D11_VIEWPORT構造体の配列
		);
	}
}

void Camera::Update(DirectX::XMMATRIX& view, DirectX::XMMATRIX& proj)
{
	// カメラの位置
	XMVECTOR e = XMVectorSet(eye.x, eye.y, eye.z, 0.0f);
	// カメラの焦点
	XMVECTOR f = XMVectorSet(focus.x, focus.y, focus.z, 0.0f);
	XMStoreFloat4(&cameraLook, XMVector4Normalize(f - e));
	// 現在のワールド座標における上方向の定義
	XMVECTOR u = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	// カメラの位置、上方向、および焦点を使用して、左手座標系のビュー行列を作成
	view = XMMatrixLookAtLH(e, f, u);
	// 視野角の設定
	float    fov = XMConvertToRadians(45.0f);
	// アスペクト比の設定
	float    aspect = viewPort.Width / viewPort.Height;
	// ニア（見える範囲の前方）の設定
	float    nearZ = 0.1f;
	// ファー（見える範囲の後方）の設定
	float    farZ = 1000.0f;
	// 視野に基づいて、左手座標系のパースペクティブ射影行列を作成
	proj = XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);

}

void Camera::MoveEye(DirectX::XMFLOAT3& move)
{
	eye.x += move.x;
	eye.y += move.y;
	eye.z += move.z;
}
void Camera::MoveFocus(DirectX::XMFLOAT3& move)
{
	focus.x += move.x;
	focus.y += move.y;
	focus.z += move.z;
}