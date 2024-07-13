//	ファイル名	：RenderMgr.hxx
//	  概  要	：
//	作	成	者	：daigo
//	 作成日時	：2024/06/11 20:35:52
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____RenderMgr_HXX_____
#define _____RenderMgr_HXX_____

// =-=-= インクルード部 =-=-=
#include "SingletonsMng.hxx"
#include <queue>

struct Vertex
{
	float x, y;
};

class RenderMgr: public SingletonBase
{
public:
	SINGLETON_MAKES(RenderMgr)

	RenderMgr();
	~RenderMgr();

	bool Init() override;
	void Update() override;
private:

};

#endif // !_____RenderMgr_HXX_____