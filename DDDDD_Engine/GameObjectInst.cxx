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
	//for (auto& elem:m_Components)
	//	elem.release();
	m_Components.clear();
}

bool GameObjectInst::Init()
{
	for (auto& elem : m_Components)
		if(!elem->Init())// ‰Šú‰»
			return false;// ‰Šú‰»Ž¸”s
	return true;// ‰Šú‰»¬Œ÷
}

void GameObjectInst::Update()
{
	if (!IsActive())
		return;
	for (auto& elem : m_Components)
		elem->Update();// XV
}

const bool GameObjectInst::IsActive()
{
	return m_IsActive && (m_pParent ? m_pParent->IsActive() : true);
}
