//	�t�@�C����	�FSDLApp.hxx
//	  �T  �v	�F
//	��	��	��	�Fdaigo
//	 �쐬����	�F2024/06/10 21:51:22
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= �C���N���[�h�K�[�h�� =-=-=
#ifndef _____SDLApp_HXX_____
#define _____SDLApp_HXX_____

// =-=-= �C���N���[�h�� =-=-=
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include "SingletonsMng.hxx"

class SDLApp: public SingletonBase
{
public:
	// �V���O���g����
	SINGLETON_MAKES(SDLApp)

	/// @brief �R���X�g���N�^
	SDLApp();
	/// @brief �f�X�g���N�^
	~SDLApp();

	/// @brief ����������
	/// @return ����������true
	bool Init() override;
	/// @brief �X�V����
	void Update() override;
	/// @brief �I������
	void UnInit();

	/// @brief �E�B���h�E���J���Ă��邩
	/// @return �J���Ă���Ȃ�true
	bool IsRunning() { return m_stillRunning; }
	/// @brief �E�B���h�E���擾
	/// @return �E�B���h�E
	SDL_Window* GetWindow() { return m_Window; }
	/// @brief �E�B���h�E�̕����擾
	/// @return �E�B���h�E�̕�
	unsigned int GetWndWidth() { return m_WindowWidth; }
	/// @brief �E�B���h�E�̍������擾
	/// @return �E�B���h�E�̍���
	unsigned int GetWndHeight() { return m_WindowHeight; }
	/// @brief �C�x���g���擾
	/// @return �C�x���g
	SDL_Event GetEvent() { return m_event; }
	/// @brief 1�t���[��������̎���(�~���b)���擾
	/// @return 1�t���[��������̎���(�~���b)
	unsigned int DeltaTime() { return m_DeltaTime; }
private:
	SDL_Window *m_Window;
	static const unsigned int m_WindowWidth = 1280;
	static const unsigned int m_WindowHeight = 720;
	SDL_Event m_event;
	bool m_stillRunning;
	unsigned int m_DeltaTime;
	unsigned int m_lastTime;
	
};

#endif // !_____SDLApp_HXX_____