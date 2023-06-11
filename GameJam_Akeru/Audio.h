#pragma once
#include <wrl/client.h>		// DirectXのスマートポインタ
#include <xaudio2.h>
#include <string>



class Audio
{
public:
	static Audio* GetInstance()
	{
		static Audio instance;
		return &instance;
	}
	bool Init();
	void Play();
	void Term();

	HRESULT LoadAudio(const std::string& path);

private:
	HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);


	Microsoft::WRL::ComPtr<IXAudio2> pXAudio2 = nullptr;
	IXAudio2MasteringVoice* pMasterVoice = nullptr;
	WAVEFORMATEXTENSIBLE wfx = { 0 };
	XAUDIO2_BUFFER buffer = { 0 };
	int32_t cnt;
	const std::string& PATH = "snd//";// オーディオファイル格納先

};