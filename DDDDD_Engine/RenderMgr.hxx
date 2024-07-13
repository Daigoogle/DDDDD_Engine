//	�t�@�C����	�FRenderMgr.hxx
//	  �T  �v	�F
//	��	��	��	�Fdaigo
//	 �쐬����	�F2024/06/11 20:35:52
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= �C���N���[�h�K�[�h�� =-=-=
#ifndef _____RenderMgr_HXX_____
#define _____RenderMgr_HXX_____

// =-=-= �C���N���[�h�� =-=-=
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