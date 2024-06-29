#include "Component.hxx"

#include "GameObject.hxx"

GameObject Component::GetGameObject()
{
	GameObject obj;
	obj.SetInstance(m_pGameObjectInst);
	return obj;
}