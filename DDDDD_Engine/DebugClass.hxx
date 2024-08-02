//	ファイル名	：DebugClass.hxx
//	  概  要	：
//	作	成	者	：daigo
//	 作成日時	：2024/06/11 0:20:25
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____DebugClass_HXX_____
#define _____DebugClass_HXX_____

// =-=-= インクルード部 =-=-=
#include "SingletonsMng.hxx"

class DebugClass :public Singleton<DebugClass>
{
	friend class Singleton<DebugClass>;
public:

	bool Init() override;
	void Update() override;
private:
	DebugClass();
	~DebugClass();

};

#endif // !_____DebugClass_HXX_____