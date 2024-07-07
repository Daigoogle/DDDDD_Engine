//	ファイル名	：GameObjectMng.hxx
//	  概  要 	：
//	作	成	者	：daigo
//	 作成日時	：2024/06/16 11:24:35
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____GameObjectMng_HXX_____
#define _____GameObjectMng_HXX_____

// =-=-= インクルード部 =-=-=
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
	// =-=-= シングルトン化 =-=-=
	SINGLETON_MAKES(GameObjectMng)

	/// @brief コンストラクタ
	GameObjectMng();

	/// @brief デストラクタ
	~GameObjectMng();

	/// @brief 初期化処理
	/// @return 成功したらtrue
	bool Init() override;
	/// @brief 更新処理
	void Update() override;

	/// @brief 生成オブジェクトの初期化と登録
	void InitObjects();
	/// @brief 現シーンのオブジェクトの更新
	/// @param pScene 現在のシーン
	void UpdateObjects(SceneBase* pScene);
	/// @brief シーンのオブジェクトの解放
	/// @param pScene 解放するシーン
	void DeleteObjects(SceneBase* pScene);

	/// @brief オブジェクトの作成
	/// @param pScene 作成するオブジェクトの所属シーン
	/// @return 作成したオブジェクト
	GameObject MakeObject(SceneBase* pScene);

private:
	/// @brief オブジェクトのロードキュー
	std::map<SceneBase* ,std::deque<GameObjectInst*>> m_ObjectsLoadQueue;
	/// @brief オブジェクトの更新キュー
	std::map<SceneBase* ,std::deque<GameObjectInst*>> m_ObjectsQueue;
	unsigned __int64 m_LastObjectID;// 最後に割り当てたID
};

#endif // !_____GameObjectMng_HXX_____