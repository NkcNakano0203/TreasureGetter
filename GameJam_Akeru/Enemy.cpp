#include "Enemy.h"

Enemy::Enemy()
{
	// �g��Ȃ����̓J�����͈̔͊O�ɒu���Ă���
	position = DirectX::XMFLOAT3(0, 0, 0);
	matrix.SetPos(position);
	scale = DirectX::XMFLOAT3(1, 1, 1);
	currentStatus = Disable;
}

void Enemy::Move(DirectX::XMFLOAT3 value, bool relative)
{
	if (relative)
	{
		position = DirectX::XMFLOAT3(
			position.x + value.x,
			position.y + value.y,
			position.z + value.z);
	}
	else
	{
		position = value;
	}
	matrix.Identity();
	matrix.SetPos(position);
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
