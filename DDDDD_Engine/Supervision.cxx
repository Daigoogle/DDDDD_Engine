#include "SingletonsMng.hxx"

#include "SDLApp.hxx"
#include "Vulkan.hxx"
#include "SceneMng.hxx"
#include "GameObjectMng.hxx"

#include "DebugClass.hxx" 

/// @brief ‰Šú‰»ˆ—‚ğs‚¤
/// @return ¬Œ÷‚µ‚½‚çtrue
bool Supervision::Initialize()
{
	// ‰Šú‰»ˆ—‚Í‚±‚Ì’†‚Ö
	if(	SDLApp::GetInstance().Init() 
		&& Vulkan::GetInstance().Init() 
		&& SceneMng::GetInstance().Init()
		&& DebugClass::GetInstance().Init()
		&& GameObjectMng::GetInstance().Init()
	)
		return true;
	return false;
}

/// @brief •`‰æˆ—‚ğs‚¤
void Supervision::Drawing()
{
	// •`‰æˆ—‚Í‚±‚Ì’†‚Ö

}