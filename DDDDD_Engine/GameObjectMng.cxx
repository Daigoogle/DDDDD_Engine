#include "GameObjectMng.hxx"
#include "GameObjectInst.hxx"
#include "GameObject.hxx"
#include "SceneBase.hxx"

GameObjectMng::GameObjectMng()
	: SingletonBase(UPDATE_ORDER::SECOND_UPDATE)
{

}

GameObjectMng::~GameObjectMng()
{

}

bool GameObjectMng::Init()
{
	return true;
}

void GameObjectMng::Update()
{

}

void GameObjectMng::InitObjects()
{

}

void GameObjectMng::UpdateObjects()
{

}

void GameObjectMng::DeleteObjects()
{

}

GameObject GameObjectMng::MakeObject(SceneBase* pScene)
{
	m_ObjectsLoadQueue[typeid(*pScene)].push(new GameObjectInst());
	return ;
}
