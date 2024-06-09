#include "Vulkan.hxx"

#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL_syswm.h>
#include <iostream>

SDL_Window* Vulkan::m_Window;
unsigned int Vulkan::m_extension_count;
std::vector<const char*>* Vulkan::m_extensions;
std::vector<const char*> Vulkan::m_layers;
vk::ApplicationInfo Vulkan::m_appInfo;
vk::InstanceCreateInfo Vulkan::m_instInfo;
vk::Instance Vulkan::m_instance;
VkSurfaceKHR Vulkan::m_surface;
vk::SurfaceKHR* Vulkan::m_pSurface;
bool Vulkan::m_stillRunning = true;
SDL_Event Vulkan::m_event;
unsigned int Vulkan::m_DeltaTime = 0;

/// @brief 初期化処理
/// @return 成功したらtrue
bool Vulkan::Init()
{
    // Create an SDL window that supports Vulkan rendering.
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "Could not initialize SDL." << std::endl;
        return false;
    }
    m_Window = SDL_CreateWindow("Vulkan Window", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_VULKAN);
    if (m_Window == NULL) {
        std::cout << "Could not create SDL window." << std::endl;
        return false;
    }

    // Get WSI extensions from SDL (we can add more if we like - we just can't remove these)
    if (!SDL_Vulkan_GetInstanceExtensions(m_Window, &m_extension_count, NULL)) {
        std::cout << "Could not get the number of required instance extensions from SDL." << std::endl;
        return false;
    }
    m_extensions = new std::vector<const char*>(m_extension_count);
    if (!SDL_Vulkan_GetInstanceExtensions(m_Window, &m_extension_count, m_extensions->data())) {
        std::cout << "Could not get the names of required instance extensions from SDL." << std::endl;
        return false;
    }

    // Use validation layers if this is a debug build
#if defined(_DEBUG)
    m_layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    // vk::ApplicationInfo allows the programmer to specifiy some basic information about the
    // program, which can be useful for layers and tools to provide more debug information.
    m_appInfo = vk::ApplicationInfo()
        .setPApplicationName("Vulkan C++ Windowed Program Template")
        .setApplicationVersion(1)
        .setPEngineName("LunarG SDK")
        .setEngineVersion(1)
        .setApiVersion(VK_API_VERSION_1_0);

    // vk::InstanceCreateInfo is where the programmer specifies the layers and/or extensions that
    // are needed.
    m_instInfo = vk::InstanceCreateInfo()
        .setFlags(vk::InstanceCreateFlags())
        .setPApplicationInfo(&m_appInfo)
        .setEnabledExtensionCount(static_cast<uint32_t>(m_extensions->size()))
        .setPpEnabledExtensionNames(m_extensions->data())
        .setEnabledLayerCount(static_cast<uint32_t>(m_layers.size()))
        .setPpEnabledLayerNames(m_layers.data());

    // Create the Vulkan instance.
    try {
        m_instance = vk::createInstance(m_instInfo);
    }
    catch (const std::exception& e) {
        std::cout << "Could not create a Vulkan instance: " << e.what() << std::endl;
        return false;
    }

    // Create a Vulkan surface for rendering
    if (!SDL_Vulkan_CreateSurface(m_Window, static_cast<VkInstance>(m_instance), &m_surface)) {
        std::cout << "Could not create a Vulkan surface." << std::endl;
        return false;
    }
    m_pSurface = new vk::SurfaceKHR(m_surface);

    return true;
}
/// @brief 更新処理
void Vulkan::Update()
{
    m_DeltaTime = SDL_GetTicks() - m_DeltaTime;

    // イベントを取得
    while (SDL_PollEvent(&m_event)) {

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

/// @brief 終了処理
void Vulkan::UnInit()
{
    m_instance.destroySurfaceKHR(*m_pSurface);// サーフェスの破棄
    SDL_DestroyWindow(m_Window);// ウィンドウの破棄
    SDL_Quit();// SDLの終了
    m_instance.destroy();// インスタンスの破棄
}