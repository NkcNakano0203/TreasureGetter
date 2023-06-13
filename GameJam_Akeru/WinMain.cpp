#include <Windows.h>		// Windowsプログラムで必要になるものが入っている
#include <time.h>
#include "App.h"
#include "Input.h"
#include "SceneManager.h"
#include "StartScene.h"
#include "GameScene.h"
#include "ClearScene.h"

#pragma comment(lib,"winmm.lib")

// シーン制御
SceneManager* pScene;
STATUS status = INIT;
SCENE  scene = START;
// FPS制御
float frameTime = 0;
LARGE_INTEGER timeStart = { 0 };
LARGE_INTEGER timeEnd = { 0 };
LARGE_INTEGER timeFreq = { 0 };

void GameLoop();
void FpsSleep();

// エントリーポイント
int32_t WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int32_t nCmdShow)
{
	// 初期化
	if (!App::GetInstance()->InitApp())return 1;

	MSG hMsg = {};
	ZeroMemory(&hMsg, sizeof(hMsg));

	// メインループ
	while (hMsg.message != WM_QUIT)
	{
		if (PeekMessage(&hMsg, nullptr, 0, 0, PM_REMOVE))// メッセージを取得する。出来なくても繰り返し処理を行う
		{
			TranslateMessage(&hMsg);// キーボードメッセージから文字メッセージを生成する
			DispatchMessage(&hMsg);	// ウィンドウプロシージャにメッセージを渡す
		}
		else
		{
			Input::GetInstance()->SetInputState();		// キー入力取得
			QueryPerformanceFrequency(&timeFreq);		// FPS制御
			QueryPerformanceCounter(&timeStart);		// FPS制御
			GameLoop();									// ゲーム処理
			FpsSleep();									// FPS制御
			Input::GetInstance()->SetInputStateOld();	// キー入力保存
		}
	}

	// 終了
	App::GetInstance()->TermApp();

	return 0;
}

// ゲームループ
void GameLoop()
{
	switch (status)
	{
	case INIT:

		// シーンの切り替え
		switch (scene)
		{
		case START:
		default:
			pScene = new StartScene();
			break;
		case GAME:
			pScene = new GameScene();
			break;
		case CLEAR:
			pScene = new ClearScene();
			break;
		}
		pScene->Init();
		// 初期化が終わったら更新に
		status = UPDATE;
		break;
	case UPDATE:
		scene = pScene->Update();

		pScene->Render();

		// 同じシーンをループする場合SCENE_MAXを返す。SCENE_MAX以外が返ってきたらシーン遷移
		if (scene != SCENE_MAX)
		{
			status = TERM;
		}
		break;
	case TERM:
		pScene->Term();
		delete pScene;
		status = INIT;
		break;
	}
}

// FPS制御用スリープ処理
void FpsSleep()
{
	// 現在の時間取得
	QueryPerformanceCounter(&timeEnd);
	// (今の時間 - 前フレームの時間) / 周波数 = 経過時間(秒単位)
	frameTime = static_cast<float>(timeEnd.QuadPart - timeStart.QuadPart) / static_cast<float>(timeFreq.QuadPart);

	if (frameTime < MIN_FRAME_TIME) { // 時間に余裕がある
		// ミリ秒に変換
		DWORD sleepTime = static_cast<DWORD>((MIN_FRAME_TIME - frameTime) * 1000);

		timeBeginPeriod(1); // 分解能を上げる(こうしないとSleepの精度はガタガタ)
		Sleep(sleepTime);   // 待機
		timeEndPeriod(1);   // 戻す

		return;
	}
	timeStart = timeEnd; // 入れ替え
}