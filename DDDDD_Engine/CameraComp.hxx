//	�t�@�C����	�FCameraComp.hxx
//	  �T  �v	�F
//	��	��	��	�Fdaigo
//	 �쐬����	�F2024/06/27 14:24:31
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= �C���N���[�h�K�[�h�� =-=-=
#ifndef _____CameraComp_HXX_____
#define _____CameraComp_HXX_____

// =-=-= �C���N���[�h�� =-=-=
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