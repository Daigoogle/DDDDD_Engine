//	ファイル名	：SceneMng.hxx
//	  概  要	：
//	作	成	者	：daigo
//	 作成日時	：2024/06/14 23:52:43
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____SceneMng_HXX_____
#define _____SceneMng_HXX_____

// =-=-= インクルード部 =-=-=
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