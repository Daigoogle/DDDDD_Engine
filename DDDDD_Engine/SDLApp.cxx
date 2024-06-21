#include "SDLApp.hxx"

#include <iostream>

SDLApp::SDLApp()
    :SingletonBase(UPDATE_ORDER::FIRST_UPDATE)
    ,m_Window(nullptr)
    ,m_event()
	,m_stillRunning(true)
	,m_DeltaTime(0)
    ,m_lastTime(0)
{

}

SDLApp::~SDLApp()
{

}

bool SDLApp::Init()
{
    // Create an SDL window that supports Vulkan rendering.
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "Could not initialize SDL." << std::endl;
        return false;
    }
    m_Window = SDL_CreateWindow("Vulkan Window", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, m_WindowWidth, m_WindowHeight, SDL_WINDOW_VULKAN);
    if (m_Window == NULL) {
        std::cout << "Could not create SDL window." << std::endl;
        return false;
    }

    m_lastTime = SDL_GetTicks(); // ���݂̎��Ԃ��擾

	return true;
}

void SDLApp::Update()
{
    unsigned int currentTick = SDL_GetTicks(); // ���݂̎��Ԃ��擾
    m_DeltaTime = currentTick - m_lastTime;   // �P�t���[��������̎��Ԃ��擾
    m_lastTime = currentTick;                 // ���݂̎��Ԃ�ۑ�

    // �C�x���g���P���擾���郋�[�v
    while (SDL_PollEvent(&m_event)) 
    {
        switch (m_event.type) {

        case SDL_QUIT:
            m_stillRunning = false;
            break;

        default:
            // Do nothing.
            break;
        }
    }
}

void SDLApp::UnInit()
{
    SDL_DestroyWindow(m_Window);    // �E�B���h�E�̔j��
    SDL_Quit(); // SDL�̏I��
}