//	ファイル名	：TitleScene.hxx
//	  概  要	：
//	作	成	者	：daigo
//	 作成日時	：2024/06/27 12:48:37
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____TitleScene_HXX_____
#define _____TitleScene_HXX_____

// =-=-= インクルード部 =-=-=
#include "SceneBase.hxx"

class TitleScene : public SceneBase
{
public:
	TitleScene();
	~TitleScene();

	bool Init() override;

private:

};

#endif // !_____TitleScene_HXX_____