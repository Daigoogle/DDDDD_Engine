#include "GameObjectMng.hxx"
#include "GameObjectInst.hxx"
#include "GameObject.hxx"
#include "SceneMng.hxx"
#include "SceneBase.hxx"

GameObjectMng::GameObjectMng()
	: SingletonBase(UPDATE_ORDER::SECOND_UPDATE)
	, m_LastObjectID(0)
{

}

GameObjectMng::~GameObjectMng()
{

}

bool GameObjectMng::Init()
{
	return true;
}

void GameObjectMng::Update()
{
	InitObjects();// まずは初期化
	UpdateObjects(SceneMng::GetInstance().GetNowScene());// 更新
}

void GameObjectMng::InitObjects()
{
	// キューの中身を全て初期化
	for (auto& elem: m_ObjectsLoadQueue){
		while (elem.second.empty()){
			 elem.second.front()->Init();
			 m_ObjectsQueue[elem.first].push(std::move(elem.second.front()));// InitからUpdateに移動
			 elem.second.pop();
		}
	}
}

void GameObjectMng::UpdateObjects(SceneBase* pScene)
{
	// シーンキューの中身を更新
	while (m_ObjectsQueue[pScene].empty())
		m_ObjectsQueue[pScene].front()->Update();
}

void GameObjectMng::DeleteObjects(SceneBase* pScene)
{
	// シーンキューの中身を解放
	while (m_ObjectsQueue[pScene].empty())
		m_ObjectsQueue[pScene].front().release();
}

GameObject GameObjectMng::MakeObject(SceneBase* pScene)
{
	std::unique_ptr<GameObjectInst> pInst = std::make_unique<GameObjectInst>();// インスタンスの生成
	pInst->m_pScene = pScene;		// シーンの設定
	pInst->m_ObjectID = ++m_LastObjectID;// オブジェクトIDの設定
	GameObject obj;					// オブジェクトの生成
	obj.SetInstance(pInst.get());	// インスタンスを設定
	m_ObjectsLoadQueue[pScene].push(std::move(pInst));// キューに追加
	return obj;
}
