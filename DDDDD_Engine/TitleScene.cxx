#include "TitleScene.hxx"
#include "GameObject.hxx"
#include "CameraComp.hxx"

TitleScene::TitleScene()
{

}

TitleScene::~TitleScene()
{

}

bool TitleScene::Init()
{
	GameObject obj = MakeObject();
	
	obj.AddComponent<CameraComp>();
	obj.AddComponent<int>(); // Component���p�����Ă��Ȃ��N���X�͒ǉ��ł��Ȃ�
	
	return true;
}