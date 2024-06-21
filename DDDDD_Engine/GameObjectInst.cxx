#include "GameObjectInst.hxx"

#include "Component.hxx"

GameObjectInst::GameObjectInst()
	: m_Name("Nameless")
	, m_Tag(0), m_IsActive(true)
	, m_ObjectID(0)
	, m_pParent(nullptr)
	, m_pScene(nullptr)
{

}

GameObjectInst::~GameObjectInst()
{
	for (auto& elem:m_Components)
		delete elem;
}

bool GameObjectInst::Init()
{
	for (auto& elem : m_Components)
		if(!elem->Init())// ‰Šú‰»
			return false;// ‰Šú‰»¸”s
	return true;// ‰Šú‰»¬Œ÷
}

void GameObjectInst::Update()
{

}
