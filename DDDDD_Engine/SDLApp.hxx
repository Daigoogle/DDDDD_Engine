//	ファイル名	：SDLApp.hxx
//	  概  要	：
//	作	成	者	：daigo
//	 作成日時	：2024/06/10 21:51:22
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____SDLApp_HXX_____
#define _____SDLApp_HXX_____

// =-=-= インクルード部 =-=-=
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include "SingletonsMng.hxx"

class SDLApp: public SingletonBase
{
public:
	// シングルトン化
	SINGLETON_MAKES(SDLApp)

	/// @brief コンストラクタ
	SDLApp();
	/// @brief デストラクタ
	~SDLApp();

	/// @brief 初期化処理
	/// @return 成功したらtrue
	bool Init() override;
	/// @brief 更新処理
	void Update() override;
	/// @brief 終了処理
	void UnInit();

	/// @brief ウィンドウが開いているか
	/// @return 開いているならtrue
	bool IsRunning() { return m_stillRunning; }
	/// @brief ウィンドウを取得
	/// @return ウィンドウ
	SDL_Window* GetWindow() { return m_Window; }
	/// @brief ウィンドウの幅を取得
	/// @return ウィンドウの幅
	unsigned int GetWndWidth() { return m_WindowWidth; }
	/// @brief ウィンドウの高さを取得
	/// @return ウィンドウの高さ
	unsigned int GetWndHeight() { return m_WindowHeight; }
	/// @brief イベントを取得
	/// @return イベント
	SDL_Event GetEvent() { return m_event; }
	/// @brief 1フレーム当たりの時間(ミリ秒)を取得
	/// @return 1フレーム当たりの時間(ミリ秒)
	unsigned int DeltaTime() { return m_DeltaTime; }
private:
	SDL_Window *m_Window;
	static const unsigned int m_WindowWidth = 1280;
	static const unsigned int m_WindowHeight = 720;
	SDL_Event m_event;
	bool m_stillRunning;
	unsigned int m_DeltaTime;
	unsigned int m_lastTime;
	
};

#endif // !_____SDLApp_HXX_____