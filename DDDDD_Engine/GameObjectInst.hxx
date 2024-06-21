//	�t�@�C����	�FGameObjectInst.hxx
//	  �T  �v	�F
//	��	��	��	�Fdaigo
//	 �쐬����	�F2024/06/16 11:52:00
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= �C���N���[�h�K�[�h�� =-=-=
#ifndef _____GameObjectInst_HXX_____
#define _____GameObjectInst_HXX_____

// =-=-= �C���N���[�h�� =-=-=
#include <list>
#include <string>
#include <memory>
#include "GameObjectMng.hxx"

class Component;

class GameObjectInst
{
	friend class GameObjectMng;
	friend class GameObject;
public:
	GameObjectInst();
	~GameObjectInst();

	GameObjectInst& operator=(const GameObjectInst&) = delete;
	GameObjectInst& operator=(GameObjectInst&&) = delete;

	bool Init();
	void Update();

	const bool IsActive();


	template<typename TypeComp>
	TypeComp* AddComponent()
	{
		TypeComp* pComp = new TypeComp;
		m_Components.push_back(pComp);
		return pComp;
	}

	template<typename TypeComp>
	TypeComp* GetComponent()
	{
		for (const auto elem : m_Components)
		{
			TypeComp* pComp = dynamic_cast<TypeComp*>(elem);
			if (pComp != nullptr)
				return pComp;
		}
		return nullptr;
	}
private:
	std::string m_Name;					// ���O
	unsigned m_Tag;						// �^�O
	bool m_IsActive;					// �A�N�e�B�u���ǂ���
	std::list<std::unique_ptr<Component>> m_Components;	// �R���|�[�l���g
	unsigned __int64 m_ObjectID;		// ID
	std::list<GameObjectInst*> m_childs;// �q�I�u�W�F�N�g
	GameObjectInst* m_pParent;			// �e�I�u�W�F�N�g
	SceneBase* m_pScene;				// �V�[��
};

#endif // !_____GameObjectInst_HXX_____