//	ファイル名	：SingletonsMng.hxx
//	  概  要		：Mozc式をベースとしたシングルトンを管理するクラス
//	作	成	者	：daigo
//	 作成日時	：2024/06/09 20:37:20
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____SingletonsMng_HXX_____
#define _____SingletonsMng_HXX_____

// =-=-= インクルード部 =-=-=
#include <array>
#include <deque>
#include <mutex>
#include <type_traits>


// =-=-= 定数定義部 =-=-=
enum class UPDATE_ORDER {
	NO_UPDATE = -1,	//更新しない
	FIRST_UPDATE,	//１番に更新
	SECOND_UPDATE,	//２番に更新
	THIRD_UPDATE,	//３番に更新
	LAST_UPDATE		//最後に更新
};

/// @brief シングルトンの基底クラス
class _SingletonBase
{
public:	
	/// @brief 初期化処理 ※オーバーライドしてください
	virtual bool Init() = 0;
	/// @brief 更新処理 ※オーバーライドしてください
	virtual void Update() {};

protected:
	/// @brief コンストラクタを禁止
	_SingletonBase() = delete;
	/// @brief コンストラクタ
	_SingletonBase(UPDATE_ORDER Order);
	/// @brief デストラクタ
	virtual ~_SingletonBase() {};
};

/// @brief シングルトンの最終処理を行うクラス
class Supervision final
{
	template<typename Type> friend class Singleton;
	friend _SingletonBase;
public:
	/// @brief 初期化処理を行う
	/// @return 成功したらtrue
	static bool Initialize();
	/// @brief 更新処理を行う
	static void Updater();
	/// @brief 描画処理を行う
	static void Drawing();
	/// @brief 終了処理を行う
	static void Finalize();

private:
	/// @brief 終了処理を追加する
	/// @param func 終了処理
	static void addFinalizer(void(*func)());
	/// @brief 更新処理を追加する
	/// @param pSingleton 処理するシングルトン
	/// @param order 更新順
	static void addUpdater(_SingletonBase* pSingleton, UPDATE_ORDER order);

private:
	static std::array<std::deque<_SingletonBase*>, static_cast<int>(UPDATE_ORDER::LAST_UPDATE) + 1> m_Updaters;//更新処理
	static std::deque<void(*)()> m_finalizers;//終了処理
};

/// @brief シングルトンのインスタンスを生成・保持するクラス
template<typename Type>
class Singleton :public _SingletonBase
{
public:
	/// @brief インスタンスを取得する
	/// @return インスタンス
	static inline Type& CreateInstance()
	{
		//初めて呼び出されたならインスタンスの生成
		std::call_once(initFlag, Create);
		return *instance;
	}
	
	/// @brief インスタンスを取得する
	/// @return インスタンス
	static inline Type& GetInstance()
	{
		return *instance;
	}

protected:
	/// @brief コンストラクタを禁止
	Singleton() = delete;
	/// @brief コンストラクタ
	Singleton(UPDATE_ORDER Order) :_SingletonBase(Order) {}
	/// @brief デストラクタ
	virtual ~Singleton() = default;

	// コピー・代入禁止
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

private:
	/// @brief インスタンスを生成する
	static void Create()
	{
		instance = new Type;
		Supervision::addFinalizer(&Singleton<Type>::destroy);
	}

	/// @brief インスタンスを破棄する
	static void destroy()
	{
		delete instance;
		instance = nullptr;
	}

	static std::once_flag initFlag;	//作ったかのフラグ(排他制御)
	static Type* instance;			//自らのインスタンス
};

// 静的メンバを定義
template<typename Type> std::once_flag Singleton<Type>::initFlag;
template <typename Type> Type* Singleton<Type>::instance = nullptr;

#endif // !_____SingletonsMng_HXX_____