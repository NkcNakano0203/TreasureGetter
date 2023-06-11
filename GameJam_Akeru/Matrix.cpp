#include "Matrix.h"

void Matrix::Identity()
{
	cb.world = DirectX::XMMatrixIdentity();
}

void Matrix::SetPos(const DirectX::XMFLOAT3& pos)
{
	cb.world *= DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void Matrix::SetRotationX(float rotX)
{
	cb.world *= DirectX::XMMatrixRotationX(rotX);
}

void Matrix::SetRotationY(float rotY)
{
	cb.world *= DirectX::XMMatrixRotationY(rotY);
}

void Matrix::SetRotationZ(float rotZ)
{
	cb.world *= DirectX::XMMatrixRotationZ(rotZ);
}

void Matrix::SetRotationAxis(const DirectX::XMVECTOR& axis, float rot)
{
	cb.world *= DirectX::XMMatrixRotationAxis(axis, rot);
}

void Matrix::SetScale(const DirectX::XMFLOAT3& scale)
{
	cb.world *= DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
}

