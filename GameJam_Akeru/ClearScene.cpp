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

Matrix		clearMatrix;
Camera		clearCamera;
CPolygon	clearObj;

void ClearScene::Init()
{
	clearMatrix.Identity();
	clearCamera.SetViewPort();
	int32_t n = Shader::GetInstance()->LoadShader("VertexShader.hlsl", "PixelShader.hlsl");
	clearObj.Init(Shader::GetInstance()->GetShader(n));
}

SCENE ClearScene::Update()
{
	// シーン継続する場合はSCENE_MAXを返す
	SCENE rt = SCENE_MAX;

	// カメラ設定更新
	clearCamera.Update(clearMatrix.GetView(), clearMatrix.GetProjection());

	clearMatrix.Identity();

	static XMFLOAT3 pos;
	pos.x = 1.0f;
	clearMatrix.SetPos(pos);

	static float rot;
	rot += 0.04f;
	clearMatrix.SetRotationZ(rot);


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
	App::GetInstance()->RenderBegin(0.8f, 0.8f, 0.3f, 1.0f);

	clearObj.Render(clearMatrix.GetCB());

	App::GetInstance()->RenderEnd();
}

void ClearScene::Term()
{
	Shader::GetInstance()->Term();
}
