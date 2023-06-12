#pragma once
#include "Matrix.h"
#include "Polygon.h"

enum TipName
{
	Green=0,
	Road
};

class MapTip
{
public:
	MapTip(float x, float y, float z, TipName texNum);


private:
	DirectX::XMFLOAT3 position;
	TipName textureNumber;
	Matrix matrix;
};

MapTip::MapTip(float x, float y, float z, TipName texNum) :
	textureNumber(texNum)
{
	position = DirectX::XMFLOAT3(x, y, z);
	matrix.SetPos(position);
}