//	ファイル名	：RenderTest.hxx
//	  概  要	：
//	作	成	者	：daigo
//	 作成日時	：2024/07/09 16:58:51
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____RenderTest_HXX_____
#define _____RenderTest_HXX_____

// =-=-= インクルード部 =-=-=
#include "Component.hxx"
#include "Render.hxx"

class RenderTest:public Component, public Render
{
public:
	RenderTest();
	~RenderTest();


private:

};

#endif // !_____RenderTest_HXX_____