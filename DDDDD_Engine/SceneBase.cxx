#include "SceneBase.hxx"

#include "GameObject.hxx"
#include "GameObjectMng.hxx"

GameObject SceneBase::MakeObject()
{
	return GameObjectMng::GetInstance().MakeObject(this);
}