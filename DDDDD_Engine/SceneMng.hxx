//	�t�@�C����	�FSceneMng.hxx
//	  �T  �v	�F
//	��	��	��	�Fdaigo
//	 �쐬����	�F2024/06/14 23:52:43
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= �C���N���[�h�K�[�h�� =-=-=
#ifndef _____SceneMng_HXX_____
#define _____SceneMng_HXX_____

// =-=-= �C���N���[�h�� =-=-=
#include <deque>
#include <type_traits>
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

	template<typename TypeScene, typename = std::enable_if_t<std::is_base_of_v<SceneBase, TypeScene>>>
	void StartLoading()
	{
		
		m_LoadScenes.push_back(std::make_unique<TypeScene>());
	}

	template<typename TypeScene, typename = std::enable_if_t<std::is_base_of_v<SceneBase, TypeScene>>>
	void ChangeScene()
	{
		m_NextScene = std::make_unique<TypeScene>();
	}

	SceneBase* GetNowScene() const { return m_NowScene.get(); }
private:
	std::unique_ptr<SceneBase> m_NowScene;
	std::unique_ptr<SceneBase> m_NextScene;
	std::deque<std::unique_ptr<SceneBase>> m_LoadScenes;
};

#endif // !_____SceneMng_HXX_____