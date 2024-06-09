#include "SingletonsMng.hxx"

std::array<std::vector<SingletonBase*>, 3> SingletonManager::m_Updaters;//更新処理
std::vector<SingletonManager::Function> SingletonManager::m_finalizers;//終了処理

namespace 
{
	std::mutex gMutex;// 排他制御用
}

/// @brief コンストラクタ
SingletonBase::SingletonBase()
{
	SingletonManager::addUpdater(this, m_Order);// 更新処理を追加
}

/// @brief 終了処理を保存
/// @param func 終了処理
void SingletonManager::addFinalizer(Function func)
{
	std::lock_guard<std::mutex> lock(gMutex);// 排他制御
	m_finalizers.push_back(func);// 終了処理を追加
}

/// @brief 更新処理を保存
/// @param pSingleton 処理するシングルトン
/// @param order 更新順
void SingletonManager::addUpdater(SingletonBase* pSingleton, UPDATE_ORDER order)
{
	if(order == UPDATE_ORDER::NO_UPDATE)// 更新しない場合は追加しない
		return;
	m_Updaters[static_cast<int>(order) - 1].push_back(pSingleton);// 更新処理を追加
}

/// @brief 更新処理を行う
void SingletonManager::Updater()
{
	for (auto& updaters : m_Updaters)
		for (auto& updater : updaters)
			updater->Update();// 更新処理を実行
}

/// @brief 終了処理を行う
void SingletonManager::finalize()
{
	std::lock_guard<std::mutex> lock(gMutex);// 排他制御
	for (auto& elem : m_Updaters)// 更新処理をクリア
		elem.clear();
	for (auto& elem : m_finalizers)// 終了処理を実行
		(*elem)();
}