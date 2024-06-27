//	ファイル名	：CameraComp.hxx
//	  概  要	：
//	作	成	者	：daigo
//	 作成日時	：2024/06/27 14:24:31
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____CameraComp_HXX_____
#define _____CameraComp_HXX_____

// =-=-= インクルード部 =-=-=
#include "Component.hxx"

class CameraComp: public Component
{
public:
	CameraComp();
	~CameraComp();

	bool Init() override;
	void Update() override;
private:

};

#endif // !_____CameraComp_HXX_____