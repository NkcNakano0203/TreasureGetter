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

#include "GameScene.h"
#include "Input.h"
#include "Matrix.h"
#include "Camera.h"
#include "Polygon.h"
#include "Shader.h"
#include "Texture.h"
#include "Enemy.h"

using namespace std;
using namespace DirectX;

Camera		mainCamera;
CPolygon	gameObj;

Matrix		playerMatrix;
Matrix		treasureBoxMatrix;

// 敵の最大数
constexpr int32_t MaxEnemyIndex = 10;
// オブジェクトプール用配列
Enemy enemy[MaxEnemyIndex];
float t = 60;
constexpr float SpawnSpan = 20;

constexpr float PlayerMoveSpeed = 0.1f;
constexpr float EnemyMoveSpeed = 0.11f;
XMFLOAT3 playerPos = XMFLOAT3(0, 0, 0);
constexpr XMFLOAT3 StartPos = XMFLOAT3(0, -1.7, 0);
constexpr XMFLOAT3 BoxPos = XMFLOAT3(0, 1.5f, 0);

void GameScene::Init()
{
	playerMatrix.Identity();
	treasureBoxMatrix.Identity();
	for (size_t i = 0; i < MaxEnemyIndex; i++)
	{
		enemy[i].Identity();
	}
	mainCamera.SetViewPort();
	gameObj.Init(Shader::GetInstance()->GetShader());

	playerPos = StartPos;
}


void SpawnEnemy()
{
	for (size_t i = 0; i < MaxEnemyIndex; i++)
	{
		// 非アクティブな敵を見つける
		if (enemy[i].GetCurrentStatus() != Enemy::Disable)continue;
		random_device rnd;
		mt19937 mt(rnd());
		uniform_int_distribution<> rand10(0, 10);
		// 乱数で行動パターンを決定する
		Enemy::EnemyStatus status = rand10(mt) >= 5 ? Enemy::ActiveRightMove : Enemy::ActiveLeftMove;
		XMFLOAT3 position;
		if (status == Enemy::ActiveRightMove)
		{
			position = XMFLOAT3(-4, 0.5f, 0);
		}
		else if (status == Enemy::ActiveLeftMove)
		{
			position = XMFLOAT3(4, 0, 0);
		}
		enemy[i].ChangeStatus(status);
		enemy[i].Move(position, false);
		break;
	}
}

/// <summary>
/// 2-1
/// </summary>
bool isHit(XMFLOAT3 pos1, XMFLOAT3 pos2, float radius)
{
	// ベクトルを取得
	XMFLOAT3 vec =
		XMFLOAT3(pos2.x - pos1.x,
			pos2.y - pos1.y,
			pos2.z - pos1.z);
	// 判定を取りやすくするために長さを取得
	float vecLength = std::sqrt((std::pow(vec.x, 2) + std::pow(vec.y, 2)));
	return vecLength <= radius;
}

// 矢印とWASDに対応
void PlayerMove()
{
	XMFLOAT3 inputVec = XMFLOAT3(0, 0, 0);
	if (Input::GetInstance()->GetKey(VK_UP) || Input::GetInstance()->GetKey(0x57)) inputVec.y = 1;
	if (Input::GetInstance()->GetKey(VK_DOWN) || Input::GetInstance()->GetKey(0x53)) inputVec.y = -1;
	if (Input::GetInstance()->GetKey(VK_RIGHT) || Input::GetInstance()->GetKey(0x44)) inputVec.x = 1;
	if (Input::GetInstance()->GetKey(VK_LEFT) || Input::GetInstance()->GetKey(0x41)) inputVec.x = -1;
	// 画面外には出さない
	constexpr float MaxPosX = 3.5f;
	constexpr float MaxPosY = 2;
	if (playerPos.x > MaxPosX)playerPos.x = MaxPosX;
	if (playerPos.x < -MaxPosX)playerPos.x = -MaxPosX;
	if (playerPos.y > MaxPosY)playerPos.y = MaxPosY;
	if (playerPos.y < -MaxPosY)playerPos.y = -MaxPosY;

	// 移動
	XMStoreFloat3(&inputVec, XMVector3Normalize(XMLoadFloat3(&inputVec)));
	playerPos = XMFLOAT3(playerPos.x + PlayerMoveSpeed * inputVec.x, playerPos.y + PlayerMoveSpeed * inputVec.y, 0);
	playerMatrix.SetPos(playerPos);
}

SCENE GameScene::Update()
{
	// シーン継続する場合はSCENE_MAXを返す
	SCENE rt = SCENE_MAX;

	// カメラ設定更新
	mainCamera.Update(playerMatrix.GetView(), playerMatrix.GetProjection());
	playerMatrix.Identity();
	playerMatrix.SetScale(XMFLOAT3(0.5f, 0.5f, 1));

	for (size_t i = 0; i < MaxEnemyIndex; i++)
	{
		enemy[i].CameraUpdate(mainCamera);
		enemy[i].SetScale(XMFLOAT3(0.2f, 0.2f, 1));
	}

	// 箱
	mainCamera.Update(treasureBoxMatrix.GetView(), treasureBoxMatrix.GetProjection());
	treasureBoxMatrix.Identity();
	treasureBoxMatrix.SetPos(BoxPos);

	PlayerMove();

	// 敵の生成
	t += 1;
	if (t >= SpawnSpan)
	{
		t = 0;
		SpawnEnemy();
	}

	// 敵の移動
	for (size_t i = 0; i < MaxEnemyIndex; i++)
	{
		if (enemy[i].GetCurrentStatus() == Enemy::Disable)continue;
		// 移動
		XMFLOAT3 movePos = XMFLOAT3(EnemyMoveSpeed * (enemy[i].GetCurrentStatus() == Enemy::ActiveRightMove ? 1 : -1), 0, 0);
		enemy[i].Move(movePos);

		if (abs(enemy[i].GetPosition().x) > 6)
		{
			enemy[i].ChangeStatus(Enemy::Disable);
		}
	}

	for (size_t i = 0; i < MaxEnemyIndex; i++)
	{
		if (enemy[i].GetCurrentStatus() == Enemy::Disable)continue;
		if (isHit(enemy[i].GetPosition(), playerPos, 0.8f))
		{
			playerPos = StartPos;
		}
		else if (isHit(BoxPos, playerPos, 0.6f))
		{
			playerMatrix.SetPos(XMFLOAT3(0, 0, 0));
			rt = CLEAR;
		}
	}

	return rt;
}

void GameScene::Render()
{
	//memo:深度ステンシルビューを使用しないときは後ろに書いた物から描画される
	App::GetInstance()->RenderBegin(0.59f, 0.98f, 0.59f, 1.0f);

	gameObj.Render(playerMatrix.GetCB(), Texture::GetInstance()->GetTextureResource(Texture::Player));
	gameObj.Render(treasureBoxMatrix.GetCB(), Texture::GetInstance()->GetTextureResource(Texture::TreasureBoxClose));

	for (size_t i = 0; i < MaxEnemyIndex; i++)
	{
		if (enemy[i].GetCurrentStatus() == Enemy::Disable)continue;
		enemy[i].Render(gameObj);
	}

	App::GetInstance()->RenderEnd();
}

void GameScene::Term()
{
	Shader::GetInstance()->Term();
}
