#include "SingletonsMng.hxx"

#include "SDLApp.hxx"
#include "VulkanApp.hxx"
#include "SceneMng.hxx"
#include "GameObjectMng.hxx"

#include "DebugClass.hxx" 

/// @brief 初期化処理を行う
/// @return 成功したらtrue
bool Supervision::Initialize()
{
	//bool result = true;
	//result = SDLApp::CreateInstance().Init();
	//result = VulkanApp::CreateInstance().Init();
	//result = SceneMng::CreateInstance().Init();
	//result = DebugClass::CreateInstance().Init();
	//result = GameObjectMng::CreateInstance().Init();
	// 初期化処理はこの中へ
	if(	          SDLApp::CreateInstance().Init() 
		&&     VulkanApp::CreateInstance().Init() 
		&&      SceneMng::CreateInstance().Init()
		&&    DebugClass::CreateInstance().Init()
		&& GameObjectMng::CreateInstance().Init()
	)
		return true;
	return false;
}

/// @brief 描画処理を行う
void Supervision::Drawing()
{
	// 描画処理はこの中へ

}