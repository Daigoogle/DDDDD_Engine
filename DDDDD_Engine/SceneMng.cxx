#include "SceneMng.hxx"

SceneMng::SceneMng()
	: SingletonBase(UPDATE_ORDER::SECOND_UPDATE)
	, m_NowScene(nullptr)
	, m_NextScene(nullptr)
	, m_LoadScene(nullptr)
{

}

SceneMng::~SceneMng()
{
	if (m_NextScene){
		delete m_NextScene;
		m_NextScene = nullptr;
	}
	if(m_LoadScene){
		delete m_LoadScene;
		m_LoadScene = nullptr;
	}
	if (m_NowScene){
		m_NowScene->UnInit();
		delete m_NowScene;
		m_NowScene = nullptr;
	}
}

bool SceneMng::Init()
{
	return true;
}

void SceneMng::Update()
{
	if (m_NextScene){
		m_NowScene->UnInit();
		delete m_NowScene;
		m_NowScene = m_NextScene;
		m_NowScene->Init();
		m_NextScene = nullptr;
	}
	if(m_NowScene)
		m_NowScene->Update();
}
