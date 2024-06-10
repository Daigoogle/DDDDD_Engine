#include "SingletonsMng.hxx"

#include "SDLApp.hxx"
#include "Vulkan.hxx"

#include "DebugClass.hxx" 

/// @brief ‰Šú‰»ˆ—‚ğs‚¤
/// @return ¬Œ÷‚µ‚½‚çtrue
bool Supervision::Initialize()
{
	// ‰Šú‰»ˆ—‚Í‚±‚Ì’†‚Ö
	if(	SDLApp::GetInstance().Init() 
		&& Vulkan::GetInstance().Init() 
		&& DebugClass::GetInstance().Init()
	)
		return true;
	return false;
}