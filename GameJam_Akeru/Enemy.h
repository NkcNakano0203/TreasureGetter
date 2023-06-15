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
	void ChangeStatus(EnemyStatus nextStatus);
	void SetScale(DirectX::XMFLOAT3 value);
	void Identity();
	void CameraUpdate(Camera& camera);
	void Render(CPolygon& object);

	inline void Move(DirectX::XMFLOAT3 value, bool relative = true)
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

	/// <summary>
	/// 現在のステータスを取得する
	/// </summary>
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