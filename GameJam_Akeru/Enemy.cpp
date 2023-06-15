#include "Enemy.h"
#include "Camera.h"
#include "Texture.h"

Enemy::Enemy()
{
	// 使わない時はカメラの範囲外に置いておく
	position = DirectX::XMFLOAT3(0, 0, 0);
	matrix.SetPos(position);
	scale = DirectX::XMFLOAT3(1, 1, 1);
	currentStatus = Disable;
	texture = Texture::Enemy;
}

void Enemy::ChangeStatus(EnemyStatus nextStatus)
{
	currentStatus = nextStatus;
}

void Enemy::SetScale(DirectX::XMFLOAT3 value)
{
	scale = value;
	matrix.Identity();
	matrix.SetScale(scale);
}

void Enemy::Identity()
{
	matrix.Identity();
}

void Enemy::CameraUpdate(Camera& camera)
{
	camera.Update(matrix.GetView(), matrix.GetProjection());
	Identity();
}

void Enemy::Render(CPolygon& object)
{
	object.Render(matrix.GetCB(), Texture::GetInstance()->GetTextureResource(texture));
}