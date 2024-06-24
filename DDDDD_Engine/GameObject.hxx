//	ファイル名	：GameObject.hxx
//	  概  要	：
//	作	成	者	：daigo
//	 作成日時	：2024/06/16 11:24:03
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____GameObject_HXX_____
#define _____GameObject_HXX_____

// =-=-= インクルード部 =-=-=
#include <type_traits>
#include "GameObjectInst.hxx"

class GameObject
{
public:
	GameObject();
	~GameObject();

	void SetInstance(GameObjectInst* pInstance);
	bool IsNull() const;

	/// @brief コンポーネントの追加
	/// @tparam TypeComp 追加するコンポーネントの型
	/// @return 追加したコンポーネントのポインタ
	template<typename TypeComp, typename = std::enable_if_t<std::is_base_of_v<Component, TypeComp>>>
	TypeComp* AddComponent()
	{
		if (!m_pInstance)
			return nullptr;
		return m_pInstance->AddComponent<TypeComp>();
	}

	/// @brief コンポーネントの取得
	/// @tparam TypeComp 取得するコンポーネントの型
	/// @return 取得したコンポーネントのポインタ
	template<typename TypeComp, typename = std::enable_if_t<std::is_base_of_v<Component, TypeComp>>>
	TypeComp* GetComponent()
	{
		if (!m_pInstance)
			return nullptr;
		return m_pInstance->GetComponent<TypeComp>();
	}

	void AddChild(GameObject& pChild);
	void RemoveChild(GameObject& pChild);

	const bool IsActive();
	void SetActive(bool isActive);
	const unsigned __int64 GetID();
	void SetID(unsigned __int64 id);
	void SetName(const std::string& name);
	const std::string& GetName();
	GameObject GetParent();
	unsigned GetTag();
	void SetTag(unsigned tag);
	SceneBase* GetScene();
private:
	GameObjectInst* m_pInstance;// 実体
};

#endif // !_____GameObject_HXX_____