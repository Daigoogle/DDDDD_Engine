#include "SingletonsMng.hxx"

#include "SDLApp.hxx"
#include "VulkanApp.hxx"
#include "SceneMng.hxx"
#include "GameObjectMng.hxx"

#include "DebugClass.hxx" 

/// @brief ‰Šú‰»ˆ—‚ğs‚¤
/// @return ¬Œ÷‚µ‚½‚çtrue
bool Supervision::Initialize()
{
	// ‰Šú‰»ˆ—‚Í‚±‚Ì’†‚Ö
	if(	          SDLApp::CreateInstance().Init() 
		&&     VulkanApp::CreateInstance().Init() 
		&&      SceneMng::CreateInstance().Init()
		&&    DebugClass::CreateInstance().Init()
		&& GameObjectMng::CreateInstance().Init()
	)
		return true;
	return false;
}

/// @brief •`‰æˆ—‚ğs‚¤
void Supervision::Drawing()
{
	// •`‰æˆ—‚Í‚±‚Ì’†‚Ö

}