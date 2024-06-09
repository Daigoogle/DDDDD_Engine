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
#include <vector>
#include <mutex>

// =-=-= 定数定義部 =-=-=
enum class INIT_ORDER {
	FIRST_INIT = 0,	//１番に更新
	SECOND_INIT,	//２番に更新
	THIRD_INIT,		//３番に更新
	FOURTH_INIT,	//４番に更新
	FIFTH_INIT,		//５番に更新
};
enum class UPDATE_ORDER {
	NO_UPDATE = 0,	//更新しない
	FIRST_UPDATE,	//１番に更新
	SECOND_UPDATE,	//２番に更新
	THIRD_UPDATE,	//３番に更新
};

/// @brief シングルトンの基底クラス
class SingletonBase
{
public:
	/// @brief コンストラクタ
	SingletonBase();
	/// @brief 初期化処理 ※オーバーライドしてください
	/// @return 成功したらtrue
	virtual bool Init() = 0;
	/// @brief 更新処理 ※必要な場合のみオーバーライドしてください
	virtual void Update() {};
	/// @brief デストラクタ ※オーバーライドしてください
	virtual ~SingletonBase() = 0;
private:
	UPDATE_ORDER m_Order = UPDATE_ORDER::NO_UPDATE;//更新順 ※イニシャライザーで任意の値に初期化してください
};

/// @brief シングルトンの最終処理を行うクラス
class SingletonManager final
{
public:
	/// @brief 終了処理の関数型
	using Function = void(*)();
	using SngltFunc = void(SingletonBase::*)();
	/// @brief 終了処理を追加する
	/// @param func 終了処理
	static void addFinalizer(Function func);
	/// @brief 更新処理を追加する
	/// @param pSingleton 処理するシングルトン
	/// @param order 更新順
	static void addUpdater(SingletonBase* pSingleton, UPDATE_ORDER order);
	/// @brief 更新処理を行う
	static void Updater();
	/// @brief 終了処理を行う
	static void finalize();

private:
	static std::array<std::vector<SngltFunc>,static_cast<int>(INIT_ORDER::FIFTH_INIT)+1> m_Initializers;//初期化処理
	static std::array<std::vector<SngltFunc>, static_cast<int>(UPDATE_ORDER::THIRD_UPDATE)> m_Updaters;//更新処理
	static std::vector<Function> m_finalizers;//終了処理
};

/// @brief シングルトンの最終処理を行うクラス
/// @tparam Type シングルトンの型
template<typename Type>
class Singleton final
{
public:
	/// @brief インスタンスを取得する
	/// @return インスタンス
	static Type& GetInstance()
	{
		//初めて呼び出されたならインスタンスの生成
		std::call_once(initFlag, Create);
		assert(instance);
		return *instance;
	}

	/// @brief インスタンスを取得する
	explicit operator Type&()
	{
		return GetInstance();
	}
private:
	/// @brief インスタンスを生成する
	static void Create()
	{
		static_cast<SingletonBase>(Type);//シングルトンの基底クラスを継承しているかチェック
		instance = new Type;
		SingletonFinalizer::addFinalizer(&Singleton<Type>::destroy);
	}

	/// @brief インスタンスを破棄する
	static void destroy()
	{
		delete instance;
		instance = nullptr;
	}

	static std::once_flag initFlag;	//作ったかのフラグ(並列処理対応)
	static Type* instance;			//自らのインスタンス
};

//	静的メンバを定義
template <typename Type> std::once_flag Singleton<Type>::initFlag;
template <typename Type> Type* Singleton<Type>::instance = nullptr;

#endif // !_____SingletonsMng_HXX_____