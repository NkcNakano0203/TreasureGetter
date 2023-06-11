#pragma once
#include <Windows.h>		// Windowsプログラムで必要になるものが入っている
#include <D3D11.h>			// DirectX11SDKを使うために必要なもの
#include <d3dcompiler.h>	// シェーダーファイルをコンパイルするために必要
#include <directxcolors.h>	// DirectX::ライブラリを利用するために必要
#include "App.h"

enum STATUS
{
	INIT,		// 初期化
	UPDATE,		// 更新
	TERM,		// 終了
	STATUS_MAX,
};

enum SCENE
{
	START,		// タイトルシーン
	GAME,		// ゲームシーン
	CLEAR,		// クリアシーン
	SCENE_MAX,
};

class SceneManager
{
public:
	SceneManager() {};
	virtual void Init() {};
	virtual SCENE Update() { return SCENE_MAX; };
	virtual void Render() {};
	virtual void Term() {};
	virtual void SetViewProj(DirectX::XMMATRIX v, DirectX::XMMATRIX p) {};// 仮で viewとprojがほしい

};