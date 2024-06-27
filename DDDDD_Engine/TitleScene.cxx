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
	obj.AddComponent<int>(); // Component‚ğŒp³‚µ‚Ä‚¢‚È‚¢ƒNƒ‰ƒX‚Í’Ç‰Á‚Å‚«‚È‚¢
	
	return true;
}