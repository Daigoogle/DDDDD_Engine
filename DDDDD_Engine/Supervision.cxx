#include "SingletonsMng.hxx"

#include "SDLApp.hxx"
#include "Vulkan.hxx"

#include "DebugClass.hxx" 

/// @brief �������������s��
/// @return ����������true
bool Supervision::Initialize()
{
	// �����������͂��̒���
	if(	SDLApp::GetInstance().Init() 
		&& Vulkan::GetInstance().Init() 
		&& DebugClass::GetInstance().Init()
	)
		return true;
	return false;
}