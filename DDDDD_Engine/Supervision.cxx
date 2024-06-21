#include "SingletonsMng.hxx"

#include "SDLApp.hxx"
#include "Vulkan.hxx"
#include "SceneMng.hxx"

#include "DebugClass.hxx" 

/// @brief 初期化処理を行う
/// @return 成功したらtrue
bool Supervision::Initialize()
{
	// 初期化処理はこの中へ
	if(	SDLApp::GetInstance().Init() 
		&& Vulkan::GetInstance().Init() 
		&& SceneMng::GetInstance().Init()
		&& DebugClass::GetInstance().Init()
	)
		return true;
	return false;
}

/// @brief 描画処理を行う
void Supervision::Drawing()
{
	// 描画処理はこの中へ

}