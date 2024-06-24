//	�t�@�C����	�FGameObjectMng.hxx
//	  �T  �v	�F
//	��	��	��	�Fdaigo
//	 �쐬����	�F2024/06/16 11:24:35
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= �C���N���[�h�K�[�h�� =-=-=
#ifndef _____GameObjectMng_HXX_____
#define _____GameObjectMng_HXX_____

// =-=-= �C���N���[�h�� =-=-=
#include <memory>
#include <queue>
#include <map>

#include "SingletonsMng.hxx"

class GameObjectInst;
class GameObject;
class SceneBase;

class GameObjectMng: public SingletonBase
{
public:
	SINGLETON_MAKES(GameObjectMng)

	GameObjectMng();
	~GameObjectMng();

	bool Init() override;
	void Update() override;

	void InitObjects();
	void UpdateObjects(SceneBase* pScene);
	void DeleteObjects(SceneBase* pScene);

	GameObject MakeObject(SceneBase* pScene);

private:
	std::map<SceneBase* ,std::queue<std::unique_ptr<GameObjectInst>>> m_ObjectsLoadQueue;
	std::map<SceneBase* ,std::queue<std::unique_ptr<GameObjectInst>>> m_ObjectsQueue;
	unsigned __int64 m_LastObjectID;
};

#endif // !_____GameObjectMng_HXX_____