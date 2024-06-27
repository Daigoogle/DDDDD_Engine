//	ファイル名	：GameObjectInst.hxx
//	  概  要	：
//	作	成	者	：daigo
//	 作成日時	：2024/06/16 11:52:00
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____GameObjectInst_HXX_____
#define _____GameObjectInst_HXX_____

// =-=-= インクルード部 =-=-=
#include <list>
#include <string>
#include <memory>
#include "GameObjectMng.hxx"

class Component;

class GameObjectInst
{
	friend class GameObjectMng;
	friend class GameObject;
private:
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
		std::unique_ptr<TypeComp> pComp(new TypeComp);
		m_Components.push_back(std::move(pComp));
		return pComp.get();
	}

	template<typename TypeComp>
	TypeComp* GetComponent()
	{
		for (const auto elem : m_Components)
		{
			TypeComp* pComp = dynamic_cast<TypeComp*>(elem.get());
			if (pComp != nullptr)
				return pComp;
		}
		return nullptr;
	}

private:
	std::string m_Name;					// 名前
	unsigned m_Tag;						// タグ
	bool m_IsActive;					// アクティブかどうか
	std::list<std::unique_ptr<Component>> m_Components;	// コンポーネント
	unsigned __int64 m_ObjectID;		// ID
	std::list<GameObjectInst*> m_childs;// 子オブジェクト
	GameObjectInst* m_pParent;			// 親オブジェクト
	SceneBase* m_pScene;				// シーン
};

#endif // !_____GameObjectInst_HXX_____