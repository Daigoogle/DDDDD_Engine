#include "SingletonsMng.hxx"

namespace 
{
	std::array<std::vector<SingletonBase*>, static_cast<int>(UPDATE_ORDER::LAST_UPDATE) + 1> g_Updaters;//更新処理
	std::vector<void(*)()> g_finalizers;//終了処理
	std::mutex gMutex;// 排他制御用
}

/// @brief コンストラクタ
SingletonBase::SingletonBase(UPDATE_ORDER Order)
{
	Supervision::addUpdater(this, Order);// 更新処理を追加
}

/// @brief 終了処理を保存
/// @param func 終了処理
void Supervision::addFinalizer(void(*func)())
{
	std::lock_guard<std::mutex> lock(gMutex);// 排他制御
	g_finalizers.push_back(func);// 終了処理を追加
}

/// @brief 更新処理を保存
/// @param pSingleton 処理するシングルトン
/// @param order 更新順
void Supervision::addUpdater(SingletonBase* pSingleton, UPDATE_ORDER order)
{
	if(order == UPDATE_ORDER::NO_UPDATE)// 更新しない場合は追加しない
		return;
	g_Updaters[static_cast<int>(order)].push_back(pSingleton);// 更新処理を追加
}

/// @brief 更新処理を行う
void Supervision::Updater()
{
	for (auto& updaters : g_Updaters)
		for (auto& updater : updaters)
			updater->Update();// 更新処理を実行
}

/// @brief 終了処理を行う
void Supervision::Finalize()
{
	std::lock_guard<std::mutex> lock(gMutex);// 排他制御
	for (auto& elem : g_Updaters)// 更新処理をクリア
		elem.clear();
	for (int i = g_finalizers.size() - 1; i >= 0;i--)// 終了処理を逆順に実行
		g_finalizers[i]();
}