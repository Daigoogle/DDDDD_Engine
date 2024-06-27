//	ファイル名	：Component.hxx
//	  概  要		：
//	作	成	者	：daigo
//	 作成日時	：2024/06/16 9:43:49
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____Component_HXX_____
#define _____Component_HXX_____

// =-=-= インクルード部 =-=-=

class GameObjectInst;

class Component
{
public:
	Component() {}
	virtual ~Component() {}

	virtual bool Init() = 0;
	virtual void Update() = 0;
};

#endif // !_____Component_HXX_____