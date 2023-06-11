#pragma once
#include <D3D11.h>			// DirectX11SDKを使うために必要なもの
#include <directxcolors.h>	// DirectX::ライブラリを利用するために必要

class Camera
{
private:
	D3D11_VIEWPORT		viewPort;	// 描画範囲
	DirectX::XMFLOAT4	cameraLook;	// カメラの向いている方向
	DirectX::XMFLOAT3	eye, focus, up;
public:

	Camera() :eye(0.0f, 0.0f, -5.0f), focus(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f) {}
	void SetViewPort();
	void Update(DirectX::XMMATRIX& view, DirectX::XMMATRIX& proj);
	DirectX::XMFLOAT4 GetCameraLook()const { return cameraLook; }
	void MoveEye(DirectX::XMFLOAT3& move);
	void MoveFocus(DirectX::XMFLOAT3& move);
};