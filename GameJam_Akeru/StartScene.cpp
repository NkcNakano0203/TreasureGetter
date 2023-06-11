#include <vector>
#include <tchar.h>			// テキストや文字列を扱うために必要なヘッダーファイル
#include <Windows.h>		// Windowsプログラムで必要になるものが入っている
#include <D3D11.h>			// DirectX11SDKを使うために必要なもの
#include <d3dcompiler.h>	// シェーダーファイルをコンパイルするために必要
#include <directxcolors.h>	// DirectX::ライブラリを利用するために必要

#include <math.h>
#include <random>
#include <iostream>

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

Matrix		startMatrix;
Camera		startCamera;
CPolygon	startObj;

const int length = 100;

constexpr int MaxBulletNum = 20;

vector<Matrix> enemyMatrixs;
vector<XMFLOAT3> enemyPos;

void StartScene::Init()
{
	startMatrix.Identity();
	startCamera.SetViewPort();
	startObj.Init(Shader::GetInstance()->GetShader());

	//Audio::GetInstance()->Init();
	//Audio::GetInstance()->LoadAudio("dummy.wav");
	//Audio::GetInstance()->Play();
}

XMFLOAT3 currentPos = XMFLOAT3(0, 0, 0);

void InstantiateEnemy()
{
	random_device rnd;
	mt19937 mt(rnd());
	uniform_int_distribution<> rand100(-1, 1);
	XMFLOAT3 position = XMFLOAT3(5, rand100(mt), 0);

	// 行列とポリゴンを生成
	Matrix enemyMatrix;
	enemyMatrix.Identity();
	startObj.Init(Shader::GetInstance()->GetShader());
	enemyMatrix.SetPos(position);

	// 弾の行列とポリゴンを配列に追加
	enemyMatrixs.push_back(enemyMatrix);
	enemyPos.push_back(position);
}

/// <summary>
/// 2-1
/// </summary>
bool isHit(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	// ベクトルを取得
	XMFLOAT3 vec =
		XMFLOAT3(pos2.x - pos1.x,
			pos2.y - pos1.y,
			pos2.z - pos1.z);
	// 判定を取りやすくするために長さを取得
	float vecLength = std::sqrt((std::pow(vec.x, 2) + std::pow(vec.y, 2)));
	return vecLength <= 1.0f;
}

SCENE StartScene::Update()
{
	// シーン継続する場合はSCENE_MAXを返す
	SCENE rt = SCENE_MAX;

	// カメラ設定更新
	startCamera.Update(startMatrix.GetView(), startMatrix.GetProjection());
	startMatrix.Identity();
	for (size_t i = 0; i < enemyMatrixs.size(); i++)
	{
		startCamera.Update(enemyMatrixs[i].GetView(), enemyMatrixs[i].GetProjection());
		enemyMatrixs[i].Identity();
	}

	static XMFLOAT3 pos;
	static float f;
	f += 0.05f;
	pos.y = sin(f);

	static XMFLOAT3 movePos = XMFLOAT3(1, 0, 0);
	movePos.x += 0.1f;
	if (movePos.x >= 6)
	{
		movePos.x = -5;
	}

	float moveSpeed = 0.1f;
	// 矢印とWASDに対応
	XMFLOAT3 inputVec = XMFLOAT3(0, 0, 0);
	if (Input::GetInstance()->GetKey(VK_UP) || Input::GetInstance()->GetKey(0x57)) inputVec.y = 1;
	if (Input::GetInstance()->GetKey(VK_DOWN) || Input::GetInstance()->GetKey(0x53)) inputVec.y = -1;
	if (Input::GetInstance()->GetKey(VK_RIGHT) || Input::GetInstance()->GetKey(0x44)) inputVec.x = 1;
	if (Input::GetInstance()->GetKey(VK_LEFT) || Input::GetInstance()->GetKey(0x41)) inputVec.x = -1;
	// 画面外に出たら反対から出てくる
	if (currentPos.x >= 6)currentPos.x = -5;
	if (currentPos.x <= -6)currentPos.x = 5;
	if (currentPos.y >= 4)currentPos.y = -3;
	if (currentPos.y <= -4)currentPos.y = 3;

	// 移動
	XMStoreFloat3(&inputVec, XMVector3Normalize(XMLoadFloat3(&inputVec)));
	currentPos = XMFLOAT3(currentPos.x + moveSpeed * inputVec.x, currentPos.y + moveSpeed * inputVec.y, 0);
	startMatrix.SetPos(currentPos);

	// 敵の生成

	static float t = 0;
	t += 1;
	static float insSpan = 60;
	if (t >= insSpan)
	{
		t = 0;
		InstantiateEnemy();
	}

	// 敵の移動
	for (size_t i = 0; i < enemyMatrixs.size(); i++)
	{
		// 移動
		enemyPos[i].x -= 0.05f;
		enemyMatrixs[i].SetPos(enemyPos[i]);
	}

	auto enemyPosItr = enemyPos.begin();
	auto enemyMatItr = enemyMatrixs.begin();
	while (enemyPosItr != enemyPos.end())
	{
		if (((*enemyPosItr).x < -3))
		{
			enemyPosItr = enemyPos.erase(enemyPosItr);
			enemyMatItr = enemyMatrixs.erase(enemyMatItr);
		}
		else
		{
			enemyPosItr++;
			enemyMatItr++;
		}
	}

	auto bulletPosItr = enemyPos.begin();
	auto bulletMatItr = enemyMatrixs.begin();
	while (bulletPosItr != enemyPos.end())
	{
		if (((*bulletPosItr).x > 5))
		{
			bulletPosItr = enemyPos.erase(bulletPosItr);
			bulletMatItr = enemyMatrixs.erase(bulletMatItr);
		}
		else
		{
			bulletPosItr++;
			bulletMatItr++;
		}
	}

	// シーン読み込み
	if (Input::GetInstance()->GetKeyDown(VK_RETURN))
	{
		pos = XMFLOAT3(0, 0, 0);
		rt = GAME;
	}

	return rt;
}

void StartScene::Render()
{
	//memo:深度ステンシルビューを使用しないときは後ろに書いた物から描画される
	App::GetInstance()->RenderBegin(0.5f, 0.8f, 1.0f, 1.0f);

	startObj.Render(startMatrix.GetCB(), Texture::GetInstance()->GetTextureResource(0));

	if (enemyMatrixs.size() > 0)
	{
		for (size_t i = 0; i < enemyMatrixs.size(); i++)
		{
			startObj.Render(enemyMatrixs[i].GetCB(), Texture::GetInstance()->GetTextureResource(2));
		}
	}

	App::GetInstance()->RenderEnd();
}

void StartScene::Term() {}