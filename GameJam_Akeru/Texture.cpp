#include "Texture.h"
#include "App.h"

void Texture::Term()
{
	// 要素番号1以降のデータを全部リセット
	for (auto tr = pTr.begin() + 1; tr != pTr.end(); ++tr)
	{
		tr->pTexture.Reset();
		tr->pSampler.Reset();
	}
	cnt = 1;
}

int32_t Texture::LoadTexture(const std::string& path)
{
	// stringからconst wchar_tに変換する
	std::string str = PATH + path;
	wchar_t* wc = new wchar_t[str.length() + 1];
	size_t value = 0;
	mbstowcs_s(&value, wc, 20, str.c_str(), str.length() + 1);

	pTr.emplace_back();
	// テクスチャ読み込み
	DirectX::CreateWICTextureFromFile(App::GetInstance()->GetDevice().Get(), wc, nullptr, pTr[cnt].pTexture.GetAddressOf());
	// サンプラー作成
	
	D3D11_SAMPLER_DESC descSampler;
	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	App::GetInstance()->GetDevice().Get()->CreateSamplerState(
		&descSampler,
		pTr[cnt].pSampler.GetAddressOf()
	);

	return cnt++;
}

TextureResources* Texture::GetTextureResource(int32_t num)
{
	return &pTr[num];
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Texture::GetTexture(int32_t num)
{
	return pTr[num].pTexture.Get();
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> Texture::GetSampler(int32_t num)
{
	return pTr[num].pSampler.Get();
}

