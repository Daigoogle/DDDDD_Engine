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
	InitObjects();// �܂��͏�����
	UpdateObjects(SceneMng::GetInstance().GetNowScene());// �X�V
}

void GameObjectMng::InitObjects()
{
	// �L���[�̒��g��S�ď�����
	for (auto& elem: m_ObjectsLoadQueue){
		while (elem.second.empty()){
			 elem.second.front()->Init();
			 m_ObjectsQueue[elem.first].push(std::move(elem.second.front()));// Init����Update�Ɉړ�
			 elem.second.pop();
		}
	}
}

void GameObjectMng::UpdateObjects(SceneBase* pScene)
{
	// �V�[���L���[�̒��g���X�V
	while (m_ObjectsQueue[pScene].empty())
		m_ObjectsQueue[pScene].front()->Update();
}

void GameObjectMng::DeleteObjects(SceneBase* pScene)
{
	// �V�[���L���[�̒��g�����
	while (m_ObjectsQueue[pScene].empty())
		m_ObjectsQueue[pScene].front().release();
}

GameObject GameObjectMng::MakeObject(SceneBase* pScene)
{
	std::unique_ptr<GameObjectInst> pInst = std::make_unique<GameObjectInst>();// �C���X�^���X�̐���
	pInst->m_pScene = pScene;		// �V�[���̐ݒ�
	pInst->m_ObjectID = ++m_LastObjectID;// �I�u�W�F�N�gID�̐ݒ�
	GameObject obj;					// �I�u�W�F�N�g�̐���
	obj.SetInstance(pInst.get());	// �C���X�^���X��ݒ�
	m_ObjectsLoadQueue[pScene].push(std::move(pInst));// �L���[�ɒǉ�
	return obj;
}
