#include <tchar.h>			// テキストや文字列を扱うために必要なヘッダーファイル
#include <Windows.h>		// Windowsプログラムで必要になるものが入っている
#include <D3D11.h>			// DirectX11SDKを使うために必要なもの
#include <d3dcompiler.h>	// シェーダーファイルをコンパイルするために必要
#include <directxcolors.h>	// DirectX::ライブラリを利用するために必要

#pragma comment(lib, "d3d11.lib")		// DirectX11ライブラリを利用するために必要
#pragma comment(lib, "d3dCompiler.lib")	// シェーダーコードをコンパイルするために必要

#include "ClearScene.h"
#include "Input.h"
#include "Camera.h"
#include "Matrix.h"
#include "Polygon.h"
#include "Shader.h"

using namespace std;
using namespace DirectX;

Camera		clearCamera;
CPolygon	clearObj;

Matrix		treasureMatrix;
Matrix		boxMatrix;
Matrix		clearImageMatrix;

float sinT;

void ClearScene::Init()
{
	treasureMatrix.Identity();
	boxMatrix.Identity();
	clearImageMatrix.Identity();

	clearCamera.SetViewPort();
	int32_t n = Shader::GetInstance()->LoadShader("VertexShader.hlsl", "PixelShader.hlsl");
	clearObj.Init(Shader::GetInstance()->GetShader(n));
}

SCENE ClearScene::Update()
{
	// シーン継続する場合はSCENE_MAXを返す
	SCENE rt = SCENE_MAX;

	// カメラ設定更新
	clearCamera.Update(treasureMatrix.GetView(), treasureMatrix.GetProjection());
	clearCamera.Update(boxMatrix.GetView(), boxMatrix.GetProjection());
	clearCamera.Update(clearImageMatrix.GetView(), clearImageMatrix.GetProjection());

	treasureMatrix.Identity();
	boxMatrix.Identity();
	clearImageMatrix.Identity();

	XMFLOAT3 pos = XMFLOAT3(1, 0, 0);
	sinT += 0.03f;
	pos.y = (0.5f * sin(sinT));
	treasureMatrix.SetPos(pos);

	boxMatrix.SetPos(XMFLOAT3(1, -1, 0));

	clearImageMatrix.SetScale(XMFLOAT3(3, 3, 1));
	clearImageMatrix.SetPos(XMFLOAT3(-1, 1, 0));

	// シーン読み込み
	if (Input::GetInstance()->GetKeyDown(VK_RETURN))
	{
		pos = XMFLOAT3(0, 0, 0);
		rt = START;
	}

	return rt;
}

void ClearScene::Render()
{
	App::GetInstance()->RenderBegin(1, 1, 1, 1);

	clearObj.Render(boxMatrix.GetCB(), Texture::GetInstance()->GetTextureResource(Texture::TreasureBoxOpen));
	clearObj.Render(treasureMatrix.GetCB(), Texture::GetInstance()->GetTextureResource(Texture::Treasure));
	clearObj.Render(clearImageMatrix.GetCB(), Texture::GetInstance()->GetTextureResource(Texture::ClearImage));

	App::GetInstance()->RenderEnd();
}

void ClearScene::Term()
{
	Shader::GetInstance()->Term();
}
