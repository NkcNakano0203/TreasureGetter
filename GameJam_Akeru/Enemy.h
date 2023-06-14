#pragma once
#include "Matrix.h"
#include "Polygon.h"
#include "Camera.h"

class Enemy
{
public:
	enum  EnemyStatus
	{
		Disable,
		ActiveRightMove,
		ActiveLeftMove,
	};
	Enemy();
	void Move(DirectX::XMFLOAT3 value, bool relative = true);
	void ChangeStatus(EnemyStatus nextStatus);
	void SetScale(DirectX::XMFLOAT3 value);
	void Identity();
	void CameraUpdate(Camera& camera);
	void Render(CPolygon& object);

	EnemyStatus GetCurrentStatus() const
	{
		return currentStatus;
	}

	DirectX::XMFLOAT3 GetPosition() const
	{
		return position;
	}


private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	Matrix matrix;
	EnemyStatus currentStatus;
	Texture::TextureName texture;
};