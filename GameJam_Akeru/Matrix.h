#pragma once
#include <directxcolors.h>	// DirectX::���C�u�����𗘗p���邽�߂ɕK�v

#pragma comment(lib, "d3d11.lib")		// DirectX11���C�u�����𗘗p���邽�߂ɕK�v


// ���W�ϊ��\����
struct ConstantBuffer
{
	DirectX::XMMATRIX world;		// ���[���h�ϊ��s��
	DirectX::XMMATRIX view;			// �r���[�ϊ��s��
	DirectX::XMMATRIX projection;	// �v���W�F�N�V�����ϊ��s��
};

class Matrix
{
public:
	void Identity();
	DirectX::XMMATRIX& GetWorld() { return cb.world; }
	DirectX::XMMATRIX& GetView() { return cb.view; }
	DirectX::XMMATRIX& GetProjection() { return cb.projection; }
	ConstantBuffer& GetCB() { return cb; }

	void SetPos(const DirectX::XMFLOAT3& pos);
	void SetRotationX(float rotX);
	void SetRotationY(float rotY);
	void SetRotationZ(float rotZ);
	void SetRotationAxis(const DirectX::XMVECTOR& axis, float rot);
	void SetScale(const DirectX::XMFLOAT3& scale);
private:
	ConstantBuffer cb;
};