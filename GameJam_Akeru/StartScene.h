#pragma once
#include "SceneManager.h"

class StartScene :public SceneManager
{
private:
public:
	void Init() override;
	SCENE Update() override;
	void Render() override;
	void Term() override;
	//void SetViewProj(DirectX::XMMATRIX v, DirectX::XMMATRIX p) {};// 仮で viewとprojがほしい
};