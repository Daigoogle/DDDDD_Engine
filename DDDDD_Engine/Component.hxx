//	�t�@�C����	�FComponent.hxx
//	  �T  �v		�F
//	��	��	��	�Fdaigo
//	 �쐬����	�F2024/06/16 9:43:49
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= �C���N���[�h�K�[�h�� =-=-=
#ifndef _____Component_HXX_____
#define _____Component_HXX_____

// =-=-= �C���N���[�h�� =-=-=

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