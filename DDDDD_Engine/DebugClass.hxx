//	�t�@�C����	�FDebugClass.hxx
//	  �T  �v	�F
//	��	��	��	�Fdaigo
//	 �쐬����	�F2024/06/11 0:20:25
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= �C���N���[�h�K�[�h�� =-=-=
#ifndef _____DebugClass_HXX_____
#define _____DebugClass_HXX_____

// =-=-= �C���N���[�h�� =-=-=
#include "SingletonsMng.hxx"

class DebugClass:public SingletonBase
{
public:
	SINGLETON_MAKES(DebugClass)

	DebugClass();
	~DebugClass();

	bool Init() override;
	void Update() override;
private:

};

#endif // !_____DebugClass_HXX_____