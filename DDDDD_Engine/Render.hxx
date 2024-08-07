//	ファイル名	：Render.hxx
//	  概  要	：
//	作	成	者	：daigo
//	 作成日時	：2024/06/11 20:42:52
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____Render_HXX_____
#define _____Render_HXX_____

// =-=-= インクルード部 =-=-=
#include "RenderMgr.hxx"
#include <vector>

class Render
{
	friend class RenderMgr;
public:
	Render();
	virtual ~Render();

	
protected:
	std::vector<Vertex> m_Vertex;
};

#endif // !_____Render_HXX_____