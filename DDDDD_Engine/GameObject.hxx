//	�t�@�C����	�FGameObject.hxx
//	  �T  �v	�F
//	��	��	��	�Fdaigo
//	 �쐬����	�F2024/06/16 11:24:03
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= �C���N���[�h�K�[�h�� =-=-=
#ifndef _____GameObject_HXX_____
#define _____GameObject_HXX_____

// =-=-= �C���N���[�h�� =-=-=
#include <type_traits>
#include "GameObjectInst.hxx"

class GameObject
{
public:
	GameObject();
	~GameObject();

	void SetInstance(GameObjectInst* pInstance);
	bool IsNull() const;

	/// @brief �R���|�[�l���g�̒ǉ�
	/// @tparam TypeComp �ǉ�����R���|�[�l���g�̌^
	/// @return �ǉ������R���|�[�l���g�̃|�C���^
	template<typename TypeComp, typename = std::enable_if_t<std::is_base_of_v<Component, TypeComp>>>
	TypeComp* AddComponent()
	{
		if (!m_pInstance)
			return nullptr;
		return m_pInstance->AddComponent<TypeComp>();
	}

	/// @brief �R���|�[�l���g�̎擾
	/// @tparam TypeComp �擾����R���|�[�l���g�̌^
	/// @return �擾�����R���|�[�l���g�̃|�C���^
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
	GameObjectInst* m_pInstance;// ����
};

#endif // !_____GameObject_HXX_____