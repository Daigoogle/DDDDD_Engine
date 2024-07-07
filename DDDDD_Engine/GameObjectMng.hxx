//	�t�@�C����	�FGameObjectMng.hxx
//	  �T  �v 	�F
//	��	��	��	�Fdaigo
//	 �쐬����	�F2024/06/16 11:24:35
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= �C���N���[�h�K�[�h�� =-=-=
#ifndef _____GameObjectMng_HXX_____
#define _____GameObjectMng_HXX_____

// =-=-= �C���N���[�h�� =-=-=
#include <memory>
#include <deque>
#include <map>

#include "SingletonsMng.hxx"

class GameObjectInst;
class GameObject;
class SceneBase;

class GameObjectMng: public SingletonBase
{
public:
	// =-=-= �V���O���g���� =-=-=
	SINGLETON_MAKES(GameObjectMng)

	/// @brief �R���X�g���N�^
	GameObjectMng();

	/// @brief �f�X�g���N�^
	~GameObjectMng();

	/// @brief ����������
	/// @return ����������true
	bool Init() override;
	/// @brief �X�V����
	void Update() override;

	/// @brief �����I�u�W�F�N�g�̏������Ɠo�^
	void InitObjects();
	/// @brief ���V�[���̃I�u�W�F�N�g�̍X�V
	/// @param pScene ���݂̃V�[��
	void UpdateObjects(SceneBase* pScene);
	/// @brief �V�[���̃I�u�W�F�N�g�̉��
	/// @param pScene �������V�[��
	void DeleteObjects(SceneBase* pScene);

	/// @brief �I�u�W�F�N�g�̍쐬
	/// @param pScene �쐬����I�u�W�F�N�g�̏����V�[��
	/// @return �쐬�����I�u�W�F�N�g
	GameObject MakeObject(SceneBase* pScene);

private:
	/// @brief �I�u�W�F�N�g�̃��[�h�L���[
	std::map<SceneBase* ,std::deque<GameObjectInst*>> m_ObjectsLoadQueue;
	/// @brief �I�u�W�F�N�g�̍X�V�L���[
	std::map<SceneBase* ,std::deque<GameObjectInst*>> m_ObjectsQueue;
	unsigned __int64 m_LastObjectID;// �Ō�Ɋ��蓖�Ă�ID
};

#endif // !_____GameObjectMng_HXX_____