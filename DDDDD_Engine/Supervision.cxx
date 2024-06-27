#include "SingletonsMng.hxx"

#include "SDLApp.hxx"
#include "Vulkan.hxx"
#include "SceneMng.hxx"
#include "GameObjectMng.hxx"

#include "DebugClass.hxx" 

/// @brief �������������s��
/// @return ����������true
bool Supervision::Initialize()
{
	// �����������͂��̒���
	if(	SDLApp::GetInstance().Init() 
		&& Vulkan::GetInstance().Init() 
		&& SceneMng::GetInstance().Init()
		&& DebugClass::GetInstance().Init()
		&& GameObjectMng::GetInstance().Init()
	)
		return true;
	return false;
}

/// @brief �`�揈�����s��
void Supervision::Drawing()
{
	// �`�揈���͂��̒���

}