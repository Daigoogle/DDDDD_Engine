#include "Vulkan.hxx"

#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL_syswm.h>
#include <iostream>

#include "SDLApp.hxx"

Vulkan::Vulkan()
    : SingletonBase(UPDATE_ORDER::NO_UPDATE)
    , m_Window(nullptr)
    , m_extension_count(0)
    , m_extensions(nullptr)
    , m_pSurface(nullptr)
    , m_surface(VK_NULL_HANDLE)
{

}

Vulkan::~Vulkan()
{
    UnInit();
}

/// @brief 初期化処理
/// @return 成功したらtrue
bool Vulkan::Init()
{
    // ウィンドウの取得
    m_Window = SDLApp::GetInstance().GetWindow();

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

}

/// @brief 終了処理
void Vulkan::UnInit()
{
    m_instance.destroySurfaceKHR(*m_pSurface);  // サーフェスの破棄
    SDL_DestroyWindow(m_Window);    // ウィンドウの破棄
    SDL_Quit(); // SDLの終了
    m_instance.destroy();   // インスタンスの破棄
}