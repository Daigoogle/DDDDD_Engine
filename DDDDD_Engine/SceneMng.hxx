//	�t�@�C����	�FSceneMng.hxx
//	  �T  �v	�F
//	��	��	��	�Fdaigo
//	 �쐬����	�F2024/06/14 23:52:43
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= �C���N���[�h�K�[�h�� =-=-=
#ifndef _____SceneMng_HXX_____
#define _____SceneMng_HXX_____

// =-=-= �C���N���[�h�� =-=-=
#include "SingletonsMng.hxx"
#include "SceneBase.hxx"

class SceneMng: public SingletonBase
{
public:
	SINGLETON_MAKES(SceneMng)

	SceneMng();
	~SceneMng();

	bool Init() override;
	void Update() override;

	template<typename TypeScene>
	void ChangeScene()
	{
		m_NextScene = new TypeScene;
	}
private:
	SceneBase* m_NowScene;
	SceneBase* m_NextScene;
	SceneBase* m_LoadScene;
};

#endif // !_____SceneMng_HXX_____