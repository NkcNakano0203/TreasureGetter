#pragma once
#include <Windows.h>		// Windowsプログラムで必要になるものが入っている
#include <cstdint>

constexpr int32_t KEY_MAX = 256;

class Input
{
public:
	static Input* GetInstance()
	{
		static Input instance;
		return &instance;
	}
	void SetInputState();
	void SetInputStateOld();
	bool GetKey(int32_t v_key);
	bool GetKeyDown(int32_t v_key);
	bool GetKeyUp(int32_t v_key);

private:
	Input();
	~Input();
	int8_t key[KEY_MAX];
	int8_t keyOld[KEY_MAX];
};
