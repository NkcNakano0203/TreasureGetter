#include <tchar.h>			// �e�L�X�g�╶������������߂ɕK�v�ȃw�b�_�[�t�@�C��
#include <Windows.h>		// Windows�v���O�����ŕK�v�ɂȂ���̂������Ă���
#include <D3D11.h>			// DirectX11SDK���g�����߂ɕK�v�Ȃ���
#include <d3dcompiler.h>	// �V�F�[�_�[�t�@�C�����R���p�C�����邽�߂ɕK�v
#include <directxcolors.h>	// DirectX::���C�u�����𗘗p���邽�߂ɕK�v
#include <math.h>
#include <random>
#include <iostream>

#pragma comment(lib, "d3d11.lib")		// DirectX11���C�u�����𗘗p���邽�߂ɕK�v
#pragma comment(lib, "d3dCompiler.lib")	// �V�F�[�_�[�R�[�h���R���p�C�����邽�߂ɕK�v

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

// �G�̍ő吔
constexpr int32_t MaxEnemyIndex = 10;
// �I�u�W�F�N�g�v�[���p�z��
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
		// ��A�N�e�B�u�ȓG��������
		if (enemy[i].GetCurrentStatus() != Enemy::Disable)continue;
		random_device rnd;
		mt19937 mt(rnd());
		uniform_int_distribution<> rand10(0, 10);
		// �����ōs���p�^�[�������肷��
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
	// �x�N�g�����擾
	XMFLOAT3 vec =
		XMFLOAT3(pos2.x - pos1.x,
			pos2.y - pos1.y,
			pos2.z - pos1.z);
	// ��������₷�����邽�߂ɒ������擾
	float vecLength = std::sqrt((std::pow(vec.x, 2) + std::pow(vec.y, 2)));
	return vecLength <= radius;
}

// ����WASD�ɑΉ�
void PlayerMove()
{
	XMFLOAT3 inputVec = XMFLOAT3(0, 0, 0);
	if (Input::GetInstance()->GetKey(VK_UP) || Input::GetInstance()->GetKey(0x57)) inputVec.y = 1;
	if (Input::GetInstance()->GetKey(VK_DOWN) || Input::GetInstance()->GetKey(0x53)) inputVec.y = -1;
	if (Input::GetInstance()->GetKey(VK_RIGHT) || Input::GetInstance()->GetKey(0x44)) inputVec.x = 1;
	if (Input::GetInstance()->GetKey(VK_LEFT) || Input::GetInstance()->GetKey(0x41)) inputVec.x = -1;
	// ��ʊO�ɂ͏o���Ȃ�
	constexpr float MaxPosX = 3.5f;
	constexpr float MaxPosY = 2;
	if (playerPos.x > MaxPosX)playerPos.x = MaxPosX;
	if (playerPos.x < -MaxPosX)playerPos.x = -MaxPosX;
	if (playerPos.y > MaxPosY)playerPos.y = MaxPosY;
	if (playerPos.y < -MaxPosY)playerPos.y = -MaxPosY;

	// �ړ�
	XMStoreFloat3(&inputVec, XMVector3Normalize(XMLoadFloat3(&inputVec)));
	playerPos = XMFLOAT3(playerPos.x + PlayerMoveSpeed * inputVec.x, playerPos.y + PlayerMoveSpeed * inputVec.y, 0);
	playerMatrix.SetPos(playerPos);
}

SCENE GameScene::Update()
{
	// �V�[���p������ꍇ��SCENE_MAX��Ԃ�
	SCENE rt = SCENE_MAX;

	// �J�����ݒ�X�V
	mainCamera.Update(playerMatrix.GetView(), playerMatrix.GetProjection());
	playerMatrix.Identity();
	playerMatrix.SetScale(XMFLOAT3(0.5f, 0.5f, 1));

	for (size_t i = 0; i < MaxEnemyIndex; i++)
	{
		enemy[i].CameraUpdate(mainCamera);
		enemy[i].SetScale(XMFLOAT3(0.2f, 0.2f, 1));
	}

	// ��
	mainCamera.Update(treasureBoxMatrix.GetView(), treasureBoxMatrix.GetProjection());
	treasureBoxMatrix.Identity();
	treasureBoxMatrix.SetPos(BoxPos);

	PlayerMove();

	// �G�̐���
	t += 1;
	if (t >= SpawnSpan)
	{
		t = 0;
		SpawnEnemy();
	}

	// �G�̈ړ�
	for (size_t i = 0; i < MaxEnemyIndex; i++)
	{
		if (enemy[i].GetCurrentStatus() == Enemy::Disable)continue;
		// �ړ�
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
	//memo:�[�x�X�e���V���r���[���g�p���Ȃ��Ƃ��͌��ɏ�����������`�悳���
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
