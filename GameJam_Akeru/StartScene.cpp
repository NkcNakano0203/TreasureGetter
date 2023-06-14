#include <tchar.h>			// �e�L�X�g�╶������������߂ɕK�v�ȃw�b�_�[�t�@�C��
#include <Windows.h>		// Windows�v���O�����ŕK�v�ɂȂ���̂������Ă���
#include <D3D11.h>			// DirectX11SDK���g�����߂ɕK�v�Ȃ���
#include <d3dcompiler.h>	// �V�F�[�_�[�t�@�C�����R���p�C�����邽�߂ɕK�v
#include <directxcolors.h>	// DirectX::���C�u�����𗘗p���邽�߂ɕK�v

#pragma comment(lib, "d3d11.lib")		// DirectX11���C�u�����𗘗p���邽�߂ɕK�v
#pragma comment(lib, "d3dCompiler.lib")	// �V�F�[�_�[�R�[�h���R���p�C�����邽�߂ɕK�v

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

Camera		startCamera;
Matrix		titleLogoMatrix;
CPolygon	obj;

Matrix startUIMatrix;

void MatrixIdentity()
{
	titleLogoMatrix.Identity();
	startUIMatrix.Identity();
}

void StartScene::Init()
{
	MatrixIdentity();
	startCamera.SetViewPort();
	int32_t n = Shader::GetInstance()->LoadShader("VertexShader.hlsl", "PixelShader.hlsl");
	//memo:�f�t�H���g�����ł��������ǎ���V�F�[�_�[�œ����������Ƃ��͈����œn���K�v������
	//warning:����V�F�[�_�[���g���Ƃ���.hlsl�t�@�C�������s�t�@�C���t�H���_�ɂ�����邱��
	obj.Init(Shader::GetInstance()->GetShader());

	//Audio::GetInstance()->Init();
	//Audio::GetInstance()->LoadAudio("dummy.wav");
	//Audio::GetInstance()->Play();
}

SCENE StartScene::Update()
{
	// �V�[���p������ꍇ��SCENE_MAX��Ԃ�
	SCENE rt = SCENE_MAX;

	// �J�����ݒ�X�V
	startCamera.Update(titleLogoMatrix.GetView(), titleLogoMatrix.GetProjection());
	startCamera.Update(startUIMatrix.GetView(), startUIMatrix.GetProjection());

	MatrixIdentity();

	static float t = 0;
	t += 0.02f;

	titleLogoMatrix.SetScale(XMFLOAT3(3 + (0.3f * sin(t)), 3 + (0.3f * sin(t)), 1));

	titleLogoMatrix.SetPos(XMFLOAT3(0, 0.5f, 0));
	startUIMatrix.SetPos(XMFLOAT3(0, -1, 0));

	// �V�[���ǂݍ���
	if (Input::GetInstance()->GetKeyDown(VK_RETURN))
	{
		rt = GAME;
	}

	return rt;
}

void StartScene::Render()
{
	App::GetInstance()->RenderBegin(0.5f, 0.8f, 1.0f, 1.0f);

	obj.Render(titleLogoMatrix.GetCB(), Texture::GetInstance()->GetTextureResource(Texture::TitleLogo));
	obj.Render(startUIMatrix.GetCB(), Texture::GetInstance()->GetTextureResource(Texture::StartUI));

	App::GetInstance()->RenderEnd();
}

void StartScene::Term()
{
}
