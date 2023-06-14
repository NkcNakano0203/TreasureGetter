#include <tchar.h>			// テキストや文字列を扱うために必要なヘッダーファイル
#include <Windows.h>		// Windowsプログラムで必要になるものが入っている
#include <D3D11.h>			// DirectX11SDKを使うために必要なもの
#include <d3dcompiler.h>	// シェーダーファイルをコンパイルするために必要
#include <directxcolors.h>	// DirectX::ライブラリを利用するために必要

#pragma comment(lib, "d3d11.lib")		// DirectX11ライブラリを利用するために必要
#pragma comment(lib, "d3dCompiler.lib")	// シェーダーコードをコンパイルするために必要

#include "StartScene.h"
#include "Input.h"
#include "Matrix.h"
#include "Camera.h"
#include "Polygon.h"
#include "Shader.h"
#include "Texture.h"
#include "Audio.h"

using namespace std;
using namespace DirectX;

Camera		startCamera;
Matrix		titleLogoMatrix;
CPolygon	obj;

Matrix startUIMatrix;

void MatrixIdentity()
{
	titleLogoMatrix.Identity();
	startUIMatrix.Identity();
}

void StartScene::Init()
{
	MatrixIdentity();
	startCamera.SetViewPort();
	int32_t n = Shader::GetInstance()->LoadShader("VertexShader.hlsl", "PixelShader.hlsl");
	//memo:デフォルト引数でも動くけど自作シェーダーで動かしたいときは引数で渡す必要がある
	//warning:自作シェーダーを使うときは.hlslファイルを実行ファイルフォルダにも入れること
	obj.Init(Shader::GetInstance()->GetShader());

	//Audio::GetInstance()->Init();
	//Audio::GetInstance()->LoadAudio("dummy.wav");
	//Audio::GetInstance()->Play();
}

SCENE StartScene::Update()
{
	// シーン継続する場合はSCENE_MAXを返す
	SCENE rt = SCENE_MAX;

	// カメラ設定更新
	startCamera.Update(titleLogoMatrix.GetView(), titleLogoMatrix.GetProjection());
	startCamera.Update(startUIMatrix.GetView(), startUIMatrix.GetProjection());

	MatrixIdentity();

	static float t = 0;
	t += 0.02f;

	titleLogoMatrix.SetScale(XMFLOAT3(3 + (0.3f * sin(t)), 3 + (0.3f * sin(t)), 1));

	titleLogoMatrix.SetPos(XMFLOAT3(0, 0.5f, 0));
	startUIMatrix.SetPos(XMFLOAT3(0, -1, 0));

	// シーン読み込み
	if (Input::GetInstance()->GetKeyDown(VK_RETURN))
	{
		rt = GAME;
	}

	return rt;
}

void StartScene::Render()
{
	App::GetInstance()->RenderBegin(0.5f, 0.8f, 1.0f, 1.0f);

	obj.Render(titleLogoMatrix.GetCB(), Texture::GetInstance()->GetTextureResource(Texture::TitleLogo));
	obj.Render(startUIMatrix.GetCB(), Texture::GetInstance()->GetTextureResource(Texture::StartUI));

	App::GetInstance()->RenderEnd();
}

void StartScene::Term()
{
}
