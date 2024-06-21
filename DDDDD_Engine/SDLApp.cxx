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

    m_lastTime = SDL_GetTicks(); // 現在の時間を取得

	return true;
}

void SDLApp::Update()
{
    unsigned int currentTick = SDL_GetTicks(); // 現在の時間を取得
    m_DeltaTime = currentTick - m_lastTime;   // １フレーム当たりの時間を取得
    m_lastTime = currentTick;                 // 現在の時間を保存

    // イベントを１つずつ取得するループ
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
    SDL_DestroyWindow(m_Window);    // ウィンドウの破棄
    SDL_Quit(); // SDLの終了
}