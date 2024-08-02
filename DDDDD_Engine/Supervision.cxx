#include "SingletonsMng.hxx"

#include "SDLApp.hxx"
#include "VulkanApp.hxx"
#include "SceneMng.hxx"
#include "GameObjectMng.hxx"

#include "DebugClass.hxx" 

/// @brief �������������s��
/// @return ����������true
bool Supervision::Initialize()
{
	// �����������͂��̒���
	if(	          SDLApp::CreateInstance().Init() 
		&&     VulkanApp::CreateInstance().Init() 
		&&      SceneMng::CreateInstance().Init()
		&&    DebugClass::CreateInstance().Init()
		&& GameObjectMng::CreateInstance().Init()
	)
		return true;
	return false;
}

/// @brief �`�揈�����s��
void Supervision::Drawing()
{
	// �`�揈���͂��̒���

}