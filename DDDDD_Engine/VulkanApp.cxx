#include "VulkanApp.hxx"
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL_syswm.h>
#include <iostream>
#include <fstream>
#include <vulkan/vulkan.h>
#include "SDLApp.hxx"
#include "Vectors.hxx"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const char* TextureFilePath = "Assets/Texture/UnionSorcerer_thumbnail.png";

struct Vertex {
    fVec2 Pos;
    fVec3 Color;
};
std::vector<Vertex> vertices = {
    Vertex{fVec2{-0.5f, 0.5f}, fVec3{ 0.0f, 0.0f, 1.0f}},
    Vertex{fVec2{-0.5f,-0.5f}, fVec3{ 0.0f, 1.0f, 0.0f}},
    Vertex{fVec2{ 0.5f,-0.5f}, fVec3{ 1.0f, 0.0f, 0.0f}},
    Vertex{fVec2{ 0.5f, 0.5f}, fVec3{ 1.0f, 1.0f, 1.0f}},
};
std::vector<uint16_t> indices = { 0,1,2,0,2,3 };

struct SceneData {
    fVec2 rectCenter;
};

SceneData sceneData = { fVec2{ 0.3f, -0.2f } };

VulkanApp::VulkanApp()
    : Singleton(UPDATE_ORDER::SECOND_UPDATE)
    , m_Window(nullptr)
    , swapchainPresentMode()
    , m_extension_count(0)
    , pUniformBufMem(nullptr)
{

}

VulkanApp::~VulkanApp()
{
    m_Device->unmapMemory(uniformBufMemory.get());

    descSets.clear();
    m_graphicsQueue.waitIdle();
    m_CmdBufs.clear();
    m_swapchainImageViews.clear();
    m_swapchainFramebufs.clear();
}

/// @brief 初期化処理
/// @return 成功したらtrue
bool VulkanApp::Init()
{
    // ウィンドウの取得
    SDLApp& sdlApp = SDLApp::GetInstance();
    m_Window = sdlApp.GetWindow();
    uint32_t screenWidth = sdlApp.GetWndWidth(), screenHeight = sdlApp.GetWndHeight();

    // Get WSI extensions from SDL (we can add more if we like - we just can't remove these)
    if (!SDL_Vulkan_GetInstanceExtensions(m_Window, &m_extension_count, NULL)) {
        std::cout << "Could not get the number of required instance extensions from SDL." << std::endl;
        return false;
    }
    m_extensions = std::vector<const char*>(m_extension_count);
    if (!SDL_Vulkan_GetInstanceExtensions(m_Window, &m_extension_count, m_extensions.data())) {
        std::cout << "Could not get the names of required instance extensions from SDL." << std::endl;
        return false;
    }

    // Use validation layers if this is a debug build
#if defined(_DEBUG)
    m_Layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    vk::ApplicationInfo			m_AppInfo;
    vk::InstanceCreateInfo		m_InstInfo;
    // vk::ApplicationInfo allows the programmer to specifiy some basic information about the
    // program, which can be useful for layers and tools to provide more debug information.
    m_AppInfo = vk::ApplicationInfo()
        .setPApplicationName("Vulkan C++ Windowed Program Template")
        .setApplicationVersion(1)
        .setPEngineName("LunarG SDK")
        .setEngineVersion(1)
        //.setApiVersion(VK_API_VERSION_1_0);
        .setApiVersion(VK_API_VERSION_1_3);

    // vk::InstanceCreateInfo is where the programmer specifies the layers and/or extensions that
    // are needed.
    m_InstInfo = vk::InstanceCreateInfo()
        .setFlags(vk::InstanceCreateFlags())
        .setPApplicationInfo(&m_AppInfo)
        .setEnabledExtensionCount(static_cast<uint32_t>(m_extensions.size()))
        .setPpEnabledExtensionNames(m_extensions.data())
        .setEnabledLayerCount(static_cast<uint32_t>(m_Layers.size()))
        .setPpEnabledLayerNames(m_Layers.data());

    // Create the Vulkan instance.
    try {
        m_Instance = vk::createInstanceUnique(m_InstInfo);
    }
    catch (const std::exception& e) {
        std::cout << "Could not create a Vulkan instance: " << e.what() << std::endl;
        return false;
    }

    VkSurfaceKHR c_surface;
    // Create a Vulkan surface for rendering
    if (!SDL_Vulkan_CreateSurface(m_Window, static_cast<VkInstance>(*m_Instance), &c_surface)) {
        std::cout << "Could not create a Vulkan surface." << std::endl;
        return false;
    }
    m_uSurface = vk::UniqueSurfaceKHR(c_surface,m_Instance.get());

    // 物理デバイスの取得
    std::vector<vk::PhysicalDevice> physicalDevices = m_Instance->enumeratePhysicalDevices();
    physicalDevice = physicalDevices[0];//本来は適切なデバイスを選択するロジックを実装する


    // 論理デバイスの作成
    vk::DeviceCreateInfo devCreateInfo;

    // グラフィックスキューの取得
    bool existsSuitablePhysicalDevice = false;
    uint32_t graphicsQueueFamilyIndex;

    // 物理デバイスの列挙
    for (uint32_t i = 0; i < physicalDevices.size(); i++) {
        std::vector<vk::QueueFamilyProperties> queueProps = physicalDevices[i].getQueueFamilyProperties();
        bool existsGraphicsQueue = false;

        for (uint32_t j = 0; j < queueProps.size(); j++) {
            if (queueProps[j].queueFlags & vk::QueueFlagBits::eGraphics &&
                physicalDevices[i].getSurfaceSupportKHR(j, m_uSurface.get())) {
                existsGraphicsQueue = true;
                graphicsQueueFamilyIndex = j;
                break;
            }
        }

        std::vector<vk::ExtensionProperties> extProps = physicalDevices[i].enumerateDeviceExtensionProperties();
        bool supportsSwapchainExtension = false;

        for (uint32_t j = 0; j < extProps.size(); j++) {
            if (std::string(extProps[j].extensionName.data()) == VK_KHR_SWAPCHAIN_EXTENSION_NAME) {
                supportsSwapchainExtension = true;
                break;
            }
        }

        if (existsGraphicsQueue && supportsSwapchainExtension) {
            physicalDevice = physicalDevices[i];
            existsSuitablePhysicalDevice = true;
            break;
        }
    }

    if (!existsSuitablePhysicalDevice) {
        std::cerr << "使用可能な物理デバイスがありません。" << std::endl;
        return false;
    }

    auto devRequiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    devCreateInfo.enabledExtensionCount = static_cast<uint32_t>(devRequiredExtensions.size());
    devCreateInfo.ppEnabledExtensionNames = devRequiredExtensions.begin();

    vk::DeviceQueueCreateInfo queueCreateInfo[1];
    queueCreateInfo[0].queueFamilyIndex = graphicsQueueFamilyIndex;
    queueCreateInfo[0].queueCount = 1;

    float queuePriorities[1] = { 1.0 };

    queueCreateInfo[0].pQueuePriorities = queuePriorities;

    devCreateInfo.pQueueCreateInfos = queueCreateInfo;
    devCreateInfo.queueCreateInfoCount = 1;

    m_Device = physicalDevice.createDeviceUnique(devCreateInfo);

    m_graphicsQueue = m_Device->getQueue(graphicsQueueFamilyIndex, 0);

    /// 頂点バッファの作成
    vk::BufferCreateInfo vertBufferCreateInfo;
    vertBufferCreateInfo.size = sizeof(Vertex) * vertices.size();
    vertBufferCreateInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    vertBufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    vk::PhysicalDeviceMemoryProperties memProps = physicalDevice.getMemoryProperties();

    vertexBuf = m_Device->createBufferUnique(vertBufferCreateInfo);

    vk::MemoryRequirements vertexBufMemReq = m_Device->getBufferMemoryRequirements(vertexBuf.get());

    vk::MemoryAllocateInfo vertexBufMemAllocInfo;
    vertexBufMemAllocInfo.allocationSize = vertexBufMemReq.size;

    bool suitableMemoryTypeFound = false;
    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
        if (vertexBufMemReq.memoryTypeBits & (1 << i) &&
            (memProps.memoryTypes[i].propertyFlags & (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))) {
            vertexBufMemAllocInfo.memoryTypeIndex = i;
            suitableMemoryTypeFound = true;
            break;
        }
    }
    if (!suitableMemoryTypeFound) {
        std::cerr << "適切なメモリタイプが存在しません。" << std::endl;
        return false;
    }

    vertexBufMemory = m_Device->allocateMemoryUnique(vertexBufMemAllocInfo);
    m_Device->bindBufferMemory(vertexBuf.get(), vertexBufMemory.get(), 0);    
    {
        vk::BufferCreateInfo stagingBufferCreateInfo;
        stagingBufferCreateInfo.size = sizeof(Vertex) * vertices.size();
        stagingBufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
        stagingBufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

        vk::UniqueBuffer stagingBuf = m_Device->createBufferUnique(stagingBufferCreateInfo);

        vk::MemoryRequirements stagingBufMemReq = m_Device->getBufferMemoryRequirements(stagingBuf.get());

        vk::MemoryAllocateInfo stagingBufMemAllocInfo;
        stagingBufMemAllocInfo.allocationSize = stagingBufMemReq.size;

        suitableMemoryTypeFound = false;
        for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
            if (stagingBufMemReq.memoryTypeBits & (1 << i) && (memProps.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible)) {
                stagingBufMemAllocInfo.memoryTypeIndex = i;
                suitableMemoryTypeFound = true;
                break;
            }
        }
        if (!suitableMemoryTypeFound) {
            std::cerr << "適切なメモリタイプが存在しません。" << std::endl;
            return false;
        }

        vk::UniqueDeviceMemory stagingBufMemory = m_Device->allocateMemoryUnique(stagingBufMemAllocInfo);

        m_Device->bindBufferMemory(stagingBuf.get(), stagingBufMemory.get(), 0);

        void* pStagingBufMem = m_Device->mapMemory(stagingBufMemory.get(), 0, sizeof(Vertex) * vertices.size());

        std::memcpy(pStagingBufMem, vertices.data(), sizeof(Vertex) * vertices.size());

        vk::MappedMemoryRange flushMemoryRange;
        flushMemoryRange.memory = stagingBufMemory.get();
        flushMemoryRange.offset = 0;
        flushMemoryRange.size = sizeof(Vertex) * vertices.size();

        m_Device->flushMappedMemoryRanges({ flushMemoryRange });

        m_Device->unmapMemory(stagingBufMemory.get());

        vk::CommandPoolCreateInfo tmpCmdPoolCreateInfo;
        tmpCmdPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
        tmpCmdPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
        vk::UniqueCommandPool tmpCmdPool = m_Device->createCommandPoolUnique(tmpCmdPoolCreateInfo);

        vk::CommandBufferAllocateInfo tmpCmdBufAllocInfo;
        tmpCmdBufAllocInfo.commandPool = tmpCmdPool.get();
        tmpCmdBufAllocInfo.commandBufferCount = 1;
        tmpCmdBufAllocInfo.level = vk::CommandBufferLevel::ePrimary;
        std::vector<vk::UniqueCommandBuffer> tmpCmdBufs = m_Device->allocateCommandBuffersUnique(tmpCmdBufAllocInfo);

        vk::BufferCopy bufCopy;
        bufCopy.srcOffset = 0;
        bufCopy.dstOffset = 0;
        bufCopy.size = sizeof(Vertex) * vertices.size();

        vk::CommandBufferBeginInfo cmdBeginInfo;
        cmdBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        tmpCmdBufs[0]->begin(cmdBeginInfo);
        tmpCmdBufs[0]->copyBuffer(stagingBuf.get(), vertexBuf.get(), { bufCopy });
        tmpCmdBufs[0]->end();

        vk::CommandBuffer submitCmdBuf[1] = { tmpCmdBufs[0].get() };
        vk::SubmitInfo submitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = submitCmdBuf;

        m_graphicsQueue.submit({ submitInfo });
        m_graphicsQueue.waitIdle();
    }
    //// バッファへの書き込み
    //void* vertexBufMem = m_Device->mapMemory(vertexBufMemory.get(), 0, sizeof(Vertex) * vertices.size());
    //
    //std::memcpy(vertexBufMem, vertices.data(), sizeof(Vertex) * vertices.size());
    //
    //m_Device->flushMappedMemoryRanges({ vk::MappedMemoryRange(vertexBufMemory.get(), 0, sizeof(Vertex) * vertices.size()) });
    //m_Device->unmapMemory(vertexBufMemory.get());
    vertexBindingDescription[0].binding = 0;
    vertexBindingDescription[0].stride = sizeof(Vertex);
    vertexBindingDescription[0].inputRate = vk::VertexInputRate::eVertex;
    
    vertexInputDescription[0].binding = 0;
    vertexInputDescription[0].location = 0;
    vertexInputDescription[0].format = vk::Format::eR32G32Sfloat;
    vertexInputDescription[0].offset = offsetof(Vertex,Pos);
    vertexInputDescription[1].binding = 0;
    vertexInputDescription[1].location = 1;
    vertexInputDescription[1].format = vk::Format::eR32G32B32Sfloat;
    vertexInputDescription[1].offset = offsetof(Vertex,Color);
    //vertexInputDescription[2].binding = 0;
    //vertexInputDescription[2].location = 2;
    //vertexInputDescription[2].format = vk::Format::eR32G32B32Sfloat;
    //vertexInputDescription[2].offset = offsetof(Vertex, Normal);

    /// ステージングバッファの作成
    vk::BufferCreateInfo stagingBufferCreateInfo;
    stagingBufferCreateInfo.size = sizeof(Vertex) * vertices.size();
    stagingBufferCreateInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst; // eTransferDstを追加
    stagingBufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    stagingBuf = m_Device->createBufferUnique(stagingBufferCreateInfo);

    vk::MemoryRequirements stagingBufMemReq = m_Device->getBufferMemoryRequirements(stagingBuf.get());

    vk::MemoryAllocateInfo stagingBufMemAllocInfo;
    stagingBufMemAllocInfo.allocationSize = stagingBufMemReq.size;

    suitableMemoryTypeFound = false;
    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
        if (stagingBufMemReq.memoryTypeBits & (1 << i) &&
            (memProps.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)) {   // eDeviceLocalに変更
            stagingBufMemAllocInfo.memoryTypeIndex = i;
            suitableMemoryTypeFound = true;
            break;
        }
    }
    if (!suitableMemoryTypeFound) {
        std::cerr << "適切なメモリタイプが存在しません。" << std::endl;
        return false;
    }
    stagingBufMemory = m_Device->allocateMemoryUnique(stagingBufMemAllocInfo);
    m_Device->bindBufferMemory(stagingBuf.get(), stagingBufMemory.get(), 0);

    /// インデックスバッファの作成
    vk::BufferCreateInfo indexBufferCreateInfo;
    indexBufferCreateInfo.size = sizeof(uint16_t) * indices.size();
    indexBufferCreateInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;    // ここだけ注意
    indexBufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    indexBuf = m_Device->createBufferUnique(indexBufferCreateInfo);

    vk::MemoryRequirements indexBufMemReq = m_Device->getBufferMemoryRequirements(indexBuf.get());

    vk::MemoryAllocateInfo indexBufMemAllocInfo;
    indexBufMemAllocInfo.allocationSize = indexBufMemReq.size;

    suitableMemoryTypeFound = false;
    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
        if (indexBufMemReq.memoryTypeBits & (1 << i) &&
            (memProps.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible)) {
            indexBufMemAllocInfo.memoryTypeIndex = i;
            suitableMemoryTypeFound = true;
            break;
        }
    }
    if (!suitableMemoryTypeFound) {
        std::cerr << "適切なメモリタイプが存在しません。" << std::endl;
        return false;
    }

    indexBufMemory = m_Device->allocateMemoryUnique(indexBufMemAllocInfo);

    m_Device->bindBufferMemory(indexBuf.get(), indexBufMemory.get(), 0);
    {
        vk::BufferCreateInfo stagingBufferCreateInfo;
        stagingBufferCreateInfo.size = sizeof(uint16_t) * indices.size();
        stagingBufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
        stagingBufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

        vk::UniqueBuffer stagingBuf = m_Device->createBufferUnique(stagingBufferCreateInfo);

        vk::MemoryRequirements stagingBufMemReq = m_Device->getBufferMemoryRequirements(stagingBuf.get());

        vk::MemoryAllocateInfo stagingBufMemAllocInfo;
        stagingBufMemAllocInfo.allocationSize = stagingBufMemReq.size;

        suitableMemoryTypeFound = false;
        for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
            if (stagingBufMemReq.memoryTypeBits & (1 << i) && (memProps.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible)) {
                stagingBufMemAllocInfo.memoryTypeIndex = i;
                suitableMemoryTypeFound = true;
                break;
            }
        }
        if (!suitableMemoryTypeFound) {
            std::cerr << "適切なメモリタイプが存在しません。" << std::endl;
            return false;
        }

        vk::UniqueDeviceMemory stagingBufMemory = m_Device->allocateMemoryUnique(stagingBufMemAllocInfo);

        m_Device->bindBufferMemory(stagingBuf.get(), stagingBufMemory.get(), 0);

        void* pStagingBufMem = m_Device->mapMemory(stagingBufMemory.get(), 0, sizeof(uint16_t) * indices.size());

        std::memcpy(pStagingBufMem, indices.data(), sizeof(uint16_t) * indices.size());

        vk::MappedMemoryRange flushMemoryRange;
        flushMemoryRange.memory = stagingBufMemory.get();
        flushMemoryRange.offset = 0;
        flushMemoryRange.size = sizeof(uint16_t) * indices.size();

        m_Device->flushMappedMemoryRanges({ flushMemoryRange });

        m_Device->unmapMemory(stagingBufMemory.get());

        vk::CommandPoolCreateInfo tmpCmdPoolCreateInfo;
        tmpCmdPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
        tmpCmdPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
        vk::UniqueCommandPool tmpCmdPool = m_Device->createCommandPoolUnique(tmpCmdPoolCreateInfo);

        vk::CommandBufferAllocateInfo tmpCmdBufAllocInfo;
        tmpCmdBufAllocInfo.commandPool = tmpCmdPool.get();
        tmpCmdBufAllocInfo.commandBufferCount = 1;
        tmpCmdBufAllocInfo.level = vk::CommandBufferLevel::ePrimary;
        std::vector<vk::UniqueCommandBuffer> tmpCmdBufs = m_Device->allocateCommandBuffersUnique(tmpCmdBufAllocInfo);

        vk::BufferCopy bufCopy;
        bufCopy.srcOffset = 0;
        bufCopy.dstOffset = 0;
        bufCopy.size = sizeof(uint16_t) * indices.size();

        vk::CommandBufferBeginInfo cmdBeginInfo;
        cmdBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        tmpCmdBufs[0]->begin(cmdBeginInfo);
        tmpCmdBufs[0]->copyBuffer(stagingBuf.get(), indexBuf.get(), { bufCopy });
        tmpCmdBufs[0]->end();

        vk::CommandBuffer submitCmdBuf[1] = { tmpCmdBufs[0].get() };
        vk::SubmitInfo submitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = submitCmdBuf;

        m_graphicsQueue.submit({ submitInfo });
        m_graphicsQueue.waitIdle();
    }

    //// バッファへの書き込み（インデックス編）
    //void* indexBufMem = m_Device->mapMemory(indexBufMemory.get(), 0, sizeof(uint16_t) * indices.size());
    //
    //std::memcpy(indexBufMem, indices.data(), sizeof(uint16_t) * indices.size());
    //
    //m_Device->flushMappedMemoryRanges(vk::MappedMemoryRange{ indexBufMemory.get(), 0, sizeof(uint16_t) * indices.size() });
    //m_Device->unmapMemory(indexBufMemory.get());

    /// ディスクリプタセットの作成
    vk::BufferCreateInfo uniformBufferCreateInfo;
    uniformBufferCreateInfo.size = sizeof(SceneData);
    uniformBufferCreateInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    uniformBufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    uniformBuf = m_Device->createBufferUnique(uniformBufferCreateInfo);

    vk::MemoryRequirements uniformBufMemReq = m_Device->getBufferMemoryRequirements(uniformBuf.get());

    vk::MemoryAllocateInfo uniformBufMemAllocInfo;
    uniformBufMemAllocInfo.allocationSize = uniformBufMemReq.size;

    suitableMemoryTypeFound = false;
    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
        if (uniformBufMemReq.memoryTypeBits & (1 << i) &&
            (memProps.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible)) {
            uniformBufMemAllocInfo.memoryTypeIndex = i;
            suitableMemoryTypeFound = true;
            break;
        }
    }
    if (!suitableMemoryTypeFound) {
        std::cerr << "適切なメモリタイプが存在しません。" << std::endl;
        return false;
    }

    uniformBufMemory = m_Device->allocateMemoryUnique(uniformBufMemAllocInfo);

    m_Device->bindBufferMemory(uniformBuf.get(), uniformBufMemory.get(), 0);

    pUniformBufMem = m_Device->mapMemory(uniformBufMemory.get(), 0, sizeof(SceneData));


    // テクスチャの読み込み
    int imgWidth, imgHeight, imgCh;
    auto pImgData = stbi_load("image.jpg", &imgWidth, &imgHeight, &imgCh, STBI_rgb_alpha);
    if (pImgData == nullptr) {
        std::cerr << "画像ファイルの読み込みに失敗しました。" << std::endl;
        return false;
    }

    vk::ImageCreateInfo texImgCreateInfo;
    texImgCreateInfo.imageType = vk::ImageType::e2D;
    texImgCreateInfo.extent = vk::Extent3D(imgWidth, imgHeight, 1);
    texImgCreateInfo.mipLevels = 1;
    texImgCreateInfo.arrayLayers = 1;
    texImgCreateInfo.format = vk::Format::eR8G8B8A8Unorm;
    texImgCreateInfo.tiling = vk::ImageTiling::eOptimal;
    texImgCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
    texImgCreateInfo.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    texImgCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    texImgCreateInfo.samples = vk::SampleCountFlagBits::e1;

    vk::UniqueImage texImage = m_Device->createImageUnique(texImgCreateInfo);

    vk::MemoryRequirements texImgMemReq = m_Device->getImageMemoryRequirements(texImage.get());

    vk::MemoryAllocateInfo texImgMemAllocInfo;
    texImgMemAllocInfo.allocationSize = texImgMemReq.size;

    suitableMemoryTypeFound = false;
    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
        if (texImgMemReq.memoryTypeBits & (1 << i)) {
            texImgMemAllocInfo.memoryTypeIndex = i;
            suitableMemoryTypeFound = true;
            break;
        }
    }

    if (!suitableMemoryTypeFound) {
        std::cerr << "使用可能なメモリタイプがありません。" << std::endl;
        return false;
    }

    vk::UniqueDeviceMemory texImgMem = m_Device->allocateMemoryUnique(texImgMemAllocInfo);

    m_Device->bindImageMemory(texImage.get(), texImgMem.get(), 0);

    {
        size_t imgDataSize = 4 * imgWidth * imgHeight;

        vk::BufferCreateInfo imgStagingBufferCreateInfo;
        imgStagingBufferCreateInfo.size = imgDataSize;
        imgStagingBufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
        imgStagingBufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

        vk::UniqueBuffer imgStagingBuf = m_Device->createBufferUnique(imgStagingBufferCreateInfo);

        vk::MemoryRequirements imgStagingBufMemReq = m_Device->getBufferMemoryRequirements(imgStagingBuf.get());

        vk::MemoryAllocateInfo imgStagingBufMemAllocInfo;
        imgStagingBufMemAllocInfo.allocationSize = imgStagingBufMemReq.size;

        suitableMemoryTypeFound = false;
        for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
            if (imgStagingBufMemReq.memoryTypeBits & (1 << i) &&
                (memProps.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible)) {
                imgStagingBufMemAllocInfo.memoryTypeIndex = i;
                suitableMemoryTypeFound = true;
                break;
            }
        }
        if (!suitableMemoryTypeFound) {
            std::cerr << "適切なメモリタイプが存在しません。" << std::endl;
            return false;
        }

        vk::UniqueDeviceMemory imgStagingBufMemory = m_Device->allocateMemoryUnique(imgStagingBufMemAllocInfo);

        m_Device->bindBufferMemory(imgStagingBuf.get(), imgStagingBufMemory.get(), 0);

        void* pImgStagingBufMem = m_Device->mapMemory(imgStagingBufMemory.get(), 0, imgDataSize);

        std::memcpy(pImgStagingBufMem, pImgData, imgDataSize);

        vk::MappedMemoryRange flushMemoryRange;
        flushMemoryRange.memory = imgStagingBufMemory.get();
        flushMemoryRange.offset = 0;
        flushMemoryRange.size = imgDataSize;

        m_Device->flushMappedMemoryRanges({ flushMemoryRange });

        m_Device->unmapMemory(imgStagingBufMemory.get());

        stbi_image_free(pImgData);

        vk::CommandPoolCreateInfo tmpCmdPoolCreateInfo;
        tmpCmdPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
        tmpCmdPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
        vk::UniqueCommandPool tmpCmdPool = m_Device->createCommandPoolUnique(tmpCmdPoolCreateInfo);

        vk::CommandBufferAllocateInfo tmpCmdBufAllocInfo;
        tmpCmdBufAllocInfo.commandPool = tmpCmdPool.get();
        tmpCmdBufAllocInfo.commandBufferCount = 1;
        tmpCmdBufAllocInfo.level = vk::CommandBufferLevel::ePrimary;
        std::vector<vk::UniqueCommandBuffer> tmpCmdBufs = m_Device->allocateCommandBuffersUnique(tmpCmdBufAllocInfo);

        vk::CommandBufferBeginInfo cmdBeginInfo;
        cmdBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        tmpCmdBufs[0]->begin(cmdBeginInfo);

        {
            vk::ImageMemoryBarrier barrior;
            barrior.oldLayout = vk::ImageLayout::eUndefined;
            barrior.newLayout = vk::ImageLayout::eTransferDstOptimal;
            barrior.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrior.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrior.image = texImage.get();
            barrior.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            barrior.subresourceRange.baseMipLevel = 0;
            barrior.subresourceRange.levelCount = 1;
            barrior.subresourceRange.baseArrayLayer = 0;
            barrior.subresourceRange.layerCount = 1;
            barrior.srcAccessMask = {};
            barrior.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
            tmpCmdBufs[0]->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, { barrior });
        }

        vk::BufferImageCopy imgCopyRegion;
        imgCopyRegion.bufferOffset = 0;
        imgCopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        imgCopyRegion.imageSubresource.mipLevel = 0;
        imgCopyRegion.imageSubresource.baseArrayLayer = 0;
        imgCopyRegion.imageSubresource.layerCount = 1;
        imgCopyRegion.imageOffset = vk::Offset3D{ 0, 0, 0 };
        imgCopyRegion.imageExtent = vk::Extent3D{ uint32_t(imgWidth), uint32_t(imgHeight), 1 };

        imgCopyRegion.bufferRowLength = 0;
        imgCopyRegion.bufferImageHeight = 0;

        tmpCmdBufs[0]->copyBufferToImage(imgStagingBuf.get(), texImage.get(), vk::ImageLayout::eTransferDstOptimal, { imgCopyRegion });

        {
            vk::ImageMemoryBarrier barrior;
            barrior.oldLayout = vk::ImageLayout::eTransferDstOptimal;
            barrior.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            barrior.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrior.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrior.image = texImage.get();
            barrior.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            barrior.subresourceRange.baseMipLevel = 0;
            barrior.subresourceRange.levelCount = 1;
            barrior.subresourceRange.baseArrayLayer = 0;
            barrior.subresourceRange.layerCount = 1;
            barrior.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrior.dstAccessMask = vk::AccessFlagBits::eShaderRead;
            tmpCmdBufs[0]->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, { barrior });
        }
        tmpCmdBufs[0]->end();

        vk::CommandBuffer submitCmdBuf[1] = { tmpCmdBufs[0].get() };
        vk::SubmitInfo submitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = submitCmdBuf;

        m_graphicsQueue.submit({ submitInfo });
        m_graphicsQueue.waitIdle();
    }

    vk::SamplerCreateInfo samplerCreateInfo;
    samplerCreateInfo.magFilter = vk::Filter::eLinear;
    samplerCreateInfo.minFilter = vk::Filter::eLinear;
    samplerCreateInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    samplerCreateInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
    samplerCreateInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
    samplerCreateInfo.anisotropyEnable = false;
    samplerCreateInfo.maxAnisotropy = 1.0f;
    samplerCreateInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerCreateInfo.unnormalizedCoordinates = false;
    samplerCreateInfo.compareEnable = false;
    samplerCreateInfo.compareOp = vk::CompareOp::eAlways;
    samplerCreateInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.minLod = 0.0f;
    samplerCreateInfo.maxLod = 0.0f;
    vk::UniqueSampler sampler = m_Device->createSamplerUnique(samplerCreateInfo);


    vk::DescriptorSetLayoutBinding descSetLayoutBinding[2];
    descSetLayoutBinding[0].binding = 0;
    descSetLayoutBinding[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    descSetLayoutBinding[0].descriptorCount = 1;
    descSetLayoutBinding[0].stageFlags = vk::ShaderStageFlagBits::eVertex; 
    descSetLayoutBinding[1].binding = 1;
    descSetLayoutBinding[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    descSetLayoutBinding[1].descriptorCount = 1;
    descSetLayoutBinding[1].stageFlags = vk::ShaderStageFlagBits::eFragment;

    vk::DescriptorSetLayoutCreateInfo descSetLayoutCreateInfo{};
    descSetLayoutCreateInfo.bindingCount = std::size(descSetLayoutBinding);
    descSetLayoutCreateInfo.pBindings = descSetLayoutBinding;

    descSetLayout = m_Device->createDescriptorSetLayoutUnique(descSetLayoutCreateInfo);

    vk::DescriptorPoolSize descPoolSize[2];
    descPoolSize[0].type = vk::DescriptorType::eUniformBuffer;
    descPoolSize[0].descriptorCount = 1;
    descPoolSize[1].type = vk::DescriptorType::eCombinedImageSampler;
    descPoolSize[1].descriptorCount = 1;

    vk::DescriptorPoolCreateInfo descPoolCreateInfo;
    descPoolCreateInfo.poolSizeCount = std::size(descPoolSize);
    descPoolCreateInfo.pPoolSizes = descPoolSize;
    descPoolCreateInfo.maxSets = 1;

    descPool = m_Device->createDescriptorPoolUnique(descPoolCreateInfo);

    vk::DescriptorSetAllocateInfo descSetAllocInfo;

    auto descSetLayouts = { descSetLayout.get() };

    descSetAllocInfo.descriptorPool = descPool.get();
    descSetAllocInfo.descriptorSetCount = static_cast<uint32_t>(descSetLayouts.size());
    descSetAllocInfo.pSetLayouts = descSetLayouts.begin();

    descSets = m_Device->allocateDescriptorSetsUnique(descSetAllocInfo);

    vk::WriteDescriptorSet writeDescSet;
    writeDescSet.dstSet = descSets[0].get();
    writeDescSet.dstBinding = 0;
    writeDescSet.dstArrayElement = 0;
    writeDescSet.descriptorType = vk::DescriptorType::eUniformBuffer;

    vk::DescriptorBufferInfo descBufInfo[1];
    descBufInfo[0].buffer = uniformBuf.get();
    descBufInfo[0].offset = 0;
    descBufInfo[0].range = sizeof(SceneData);

    writeDescSet.descriptorCount = 1;
    writeDescSet.pBufferInfo = descBufInfo;

    m_Device->updateDescriptorSets({ writeDescSet }, {});




    std::vector<vk::SurfaceFormatKHR> surfaceFormats = physicalDevice.getSurfaceFormatsKHR(m_uSurface.get());
    std::vector<vk::PresentModeKHR> surfacePresentModes = physicalDevice.getSurfacePresentModesKHR(m_uSurface.get());
    
    swapchainFormat = surfaceFormats[0];
    swapchainPresentMode = surfacePresentModes[0];

    RecreateSwapchain();


    vk::CommandPoolCreateInfo cmdPoolCreateInfo;
    vk::FenceCreateInfo fenceCreateInfo;

    cmdPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    cmdPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    cmdPool = m_Device->createCommandPoolUnique(cmdPoolCreateInfo);

    vk::CommandBufferAllocateInfo cmdBufAllocInfo;
    cmdBufAllocInfo.commandPool = cmdPool.get();
    cmdBufAllocInfo.commandBufferCount = 1;
    cmdBufAllocInfo.level = vk::CommandBufferLevel::ePrimary;
    m_CmdBufs =
        m_Device->allocateCommandBuffersUnique(cmdBufAllocInfo);

    m_SwapchainImgFence = m_Device->createFenceUnique(fenceCreateInfo);

    vk::SemaphoreCreateInfo semaphoreCreateInfo;

    m_swapchainImgSemaphore = m_Device->createSemaphoreUnique(semaphoreCreateInfo);
    imgRenderedSemaphorne = m_Device->createSemaphoreUnique(semaphoreCreateInfo);

    fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    imgRenderedFence = m_Device->createFenceUnique(fenceCreateInfo);

    return true;
}

void VulkanApp::RecreateSwapchain()
{   // 破棄処理
    m_swapchainFramebufs.clear();
    m_swapchainImageViews.clear();
    m_swapchainImageViews.clear();
    m_Renderpass.reset();
    m_Pipeline.reset();
    m_Swapchain.reset();

    // 新しいサイズの取得
    SDLApp& sdlApp = SDLApp::GetInstance();
    uint32_t screenWidth = sdlApp.GetWndWidth(), screenHeight = sdlApp.GetWndHeight();

    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(m_uSurface.get());

    // スワップチェーンの作成
    vk::SwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.surface = m_uSurface.get();
    swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount + 1;
    swapchainCreateInfo.imageFormat = swapchainFormat.format;
    swapchainCreateInfo.imageColorSpace = swapchainFormat.colorSpace;
    swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
    swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainCreateInfo.presentMode = swapchainPresentMode;
    swapchainCreateInfo.clipped = VK_TRUE;

    m_Swapchain = m_Device->createSwapchainKHRUnique(swapchainCreateInfo);

    // スワップチェーンのイメージの取得
    swapchainImages = m_Device->getSwapchainImagesKHR(m_Swapchain.get());

    vk::AttachmentDescription attachments[1];
    attachments[0].format = swapchainFormat.format;
    attachments[0].samples = vk::SampleCountFlagBits::e1;
    attachments[0].loadOp = vk::AttachmentLoadOp::eClear;
    attachments[0].storeOp = vk::AttachmentStoreOp::eStore;
    attachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachments[0].initialLayout = vk::ImageLayout::eUndefined;
    attachments[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference subpass0_attachmentRefs[1];
    subpass0_attachmentRefs[0].attachment = 0;
    subpass0_attachmentRefs[0].layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpasses[1];
    subpasses[0].pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpasses[0].colorAttachmentCount = 1;
    subpasses[0].pColorAttachments = subpass0_attachmentRefs;

    vk::RenderPassCreateInfo renderpassCreateInfo;
    renderpassCreateInfo.attachmentCount = 1;
    renderpassCreateInfo.pAttachments = attachments;
    renderpassCreateInfo.subpassCount = 1;
    renderpassCreateInfo.pSubpasses = subpasses;
    renderpassCreateInfo.dependencyCount = 0;
    renderpassCreateInfo.pDependencies = nullptr;

    m_Renderpass = m_Device->createRenderPassUnique(renderpassCreateInfo);

    vk::Viewport viewports[1];
    viewports[0].x = 0.0;
    viewports[0].y = 0.0;
    viewports[0].minDepth = 0.0;
    viewports[0].maxDepth = 1.0;
    viewports[0].width = static_cast<float>(screenWidth);
    viewports[0].height = static_cast<float>(screenHeight);

    // シザーの設定
    vk::Rect2D scissors[1];
    scissors[0].offset = vk::Offset2D{ 0, 0 };
    scissors[0].extent = vk::Extent2D{ screenWidth, screenHeight };

    // ビューポートの設定
    vk::PipelineViewportStateCreateInfo viewportState;
    viewportState.viewportCount = 1;
    viewportState.pViewports = viewports;
    viewportState.scissorCount = 1;
    viewportState.pScissors = scissors;

    // 頂点入力の設定
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = vertexBindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexAttributeDescriptions = vertexInputDescription;

    // 入力アセンブリの設定
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = false;

    vk::PipelineRasterizationStateCreateInfo rasterizer;
    rasterizer.depthClampEnable = false;
    rasterizer.rasterizerDiscardEnable = false;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = false;

    vk::PipelineMultisampleStateCreateInfo multisample;
    multisample.sampleShadingEnable = false;
    multisample.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendAttachmentState blendattachment[1];
    blendattachment[0].colorWriteMask =
        vk::ColorComponentFlagBits::eA |
        vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB;
    blendattachment[0].blendEnable = false;

    vk::PipelineColorBlendStateCreateInfo blend;
    blend.logicOpEnable = false;
    blend.attachmentCount = 1;
    blend.pAttachments = blendattachment;

    auto pipelineDescSetLayouts = { descSetLayout.get() };

    vk::PipelineLayoutCreateInfo layoutCreateInfo;
    layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(pipelineDescSetLayouts.size());
    layoutCreateInfo.pSetLayouts = pipelineDescSetLayouts.begin();

    pipelineLayout = m_Device->createPipelineLayoutUnique(layoutCreateInfo);

    size_t vertSpvFileSz;
    std::ifstream vertSpvFile("Assets/Shader/shader.vert.spv", std::ios_base::binary | std::ifstream::ate);
    if (!vertSpvFile.is_open()) {
        std::cerr << "ファイルが開けませんでした。" << std::endl;
        return;
    }

    vertSpvFileSz = static_cast<size_t>(vertSpvFile.tellg());
    vertSpvFile.seekg(0, std::ios_base::beg);
    std::vector<char> vertSpvFileData(vertSpvFileSz);
    vertSpvFile.read(vertSpvFileData.data(), vertSpvFileSz);

    vk::ShaderModuleCreateInfo vertShaderCreateInfo;
    vertShaderCreateInfo.codeSize = vertSpvFileSz;
    vertShaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vertSpvFileData.data());

    vertShader = m_Device->createShaderModuleUnique(vertShaderCreateInfo);

    size_t fragSpvFileSz;

    std::ifstream fragSpvFile("Assets/Shader/shader.frag.spv", std::ios_base::binary | std::ifstream::ate);
    if (!fragSpvFile.is_open()) {
        std::cerr << "ファイルが開けませんでした。" << std::endl;
        return;
    }

    fragSpvFileSz = static_cast<size_t>(fragSpvFile.tellg());
    fragSpvFile.seekg(0, std::ifstream::beg);
    std::vector<char> fragSpvFileData(fragSpvFileSz);
    fragSpvFile.read(fragSpvFileData.data(), fragSpvFileSz);

    vk::ShaderModuleCreateInfo fragShaderCreateInfo;
    fragShaderCreateInfo.codeSize = fragSpvFileSz;
    fragShaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fragSpvFileData.data());

    fragShader = m_Device->createShaderModuleUnique(fragShaderCreateInfo);

    vk::PipelineShaderStageCreateInfo shaderStage[2];
    shaderStage[0].stage = vk::ShaderStageFlagBits::eVertex;
    shaderStage[0].module = vertShader.get();
    shaderStage[0].pName = "main";
    shaderStage[1].stage = vk::ShaderStageFlagBits::eFragment;
    shaderStage[1].module = fragShader.get();
    shaderStage[1].pName = "main";

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
    pipelineCreateInfo.pRasterizationState = &rasterizer;
    pipelineCreateInfo.pMultisampleState = &multisample;
    pipelineCreateInfo.pColorBlendState = &blend;
    pipelineCreateInfo.layout = pipelineLayout.get();
    pipelineCreateInfo.renderPass = m_Renderpass.get();
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStage;

    m_Pipeline = m_Device->createGraphicsPipelineUnique(nullptr, pipelineCreateInfo).value;

    // イメージビューの作成
    m_swapchainImageViews.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); i++) {
        vk::ImageViewCreateInfo imgViewCreateInfo;
        imgViewCreateInfo.image = swapchainImages[i];
        imgViewCreateInfo.viewType = vk::ImageViewType::e2D;
        imgViewCreateInfo.format = swapchainFormat.format;
        imgViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
        imgViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
        imgViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
        imgViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
        imgViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        imgViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imgViewCreateInfo.subresourceRange.levelCount = 1;
        imgViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imgViewCreateInfo.subresourceRange.layerCount = 1;

        m_swapchainImageViews[i] = m_Device->createImageViewUnique(imgViewCreateInfo);
    }

    // フレームバッファの作成
    m_swapchainFramebufs.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); i++) {
        vk::ImageView frameBufAttachments[1];
        frameBufAttachments[0] = m_swapchainImageViews[i].get();

        vk::FramebufferCreateInfo frameBufCreateInfo;
        frameBufCreateInfo.width = surfaceCapabilities.currentExtent.width;
        frameBufCreateInfo.height = surfaceCapabilities.currentExtent.height;
        frameBufCreateInfo.layers = 1;
        frameBufCreateInfo.renderPass = m_Renderpass.get();
        frameBufCreateInfo.attachmentCount = 1;
        frameBufCreateInfo.pAttachments = frameBufAttachments;

        m_swapchainFramebufs[i] = m_Device->createFramebufferUnique(frameBufCreateInfo);
    }
}

/// @brief 更新処理
void VulkanApp::Update()
{
    static float angle = 0.0f;
    angle += SDLApp::GetInstance().DeltaTime();
    sceneData.rectCenter.x = sinf(angle * 0.002f)* 0.5f;
    sceneData.rectCenter.y = cosf(angle * 0.002f)* 0.5f;

    m_Device->waitForFences({ imgRenderedFence.get() }, VK_TRUE, UINT64_MAX);

    vk::ResultValue<uint32_t> acquireImgResult = m_Device->acquireNextImageKHR(m_Swapchain.get(), 1'000'000'000, m_swapchainImgSemaphore.get());
    if (acquireImgResult.result == vk::Result::eSuboptimalKHR || acquireImgResult.result == vk::Result::eErrorOutOfDateKHR) {
        std::cerr << "スワップチェーンを再作成します。" << std::endl;
        RecreateSwapchain();
        Update();
        return;
    }
    if (acquireImgResult.result != vk::Result::eSuccess) {
        std::cerr << "次フレームの要求に失敗しました。" << std::endl;
        return ;
    }

    m_Device->resetFences({ imgRenderedFence.get() });

    std::memcpy(pUniformBufMem, &sceneData, sizeof(SceneData));

    vk::MappedMemoryRange flushMemoryRange;
    flushMemoryRange.memory = uniformBufMemory.get();
    flushMemoryRange.offset = 0;
    flushMemoryRange.size = sizeof(SceneData);

    //m_Device->flushMappedMemoryRanges({ flushMemoryRange });

    uint32_t imgIndex = acquireImgResult.value;

    m_CmdBufs[0]->reset();

    vk::CommandBufferBeginInfo cmdBeginInfo;
    m_CmdBufs[0]->begin(cmdBeginInfo);

    vk::ClearValue clearVal[1];
    clearVal[0].color.float32[0] = 0.0f;
    clearVal[0].color.float32[1] = 0.0f;
    clearVal[0].color.float32[2] = 0.0f;
    clearVal[0].color.float32[3] = 1.0f;

    vk::RenderPassBeginInfo renderpassBeginInfo;
    renderpassBeginInfo.renderPass = m_Renderpass.get();
    renderpassBeginInfo.framebuffer = m_swapchainFramebufs[imgIndex].get();
    renderpassBeginInfo.renderArea = vk::Rect2D({ 0,0 }, { SDLApp::GetInstance().GetWndWidth() , SDLApp::GetInstance().GetWndHeight() });
    renderpassBeginInfo.clearValueCount = 1;
    renderpassBeginInfo.pClearValues = clearVal;

    m_CmdBufs[0]->beginRenderPass(renderpassBeginInfo, vk::SubpassContents::eInline);
    
    m_CmdBufs[0]->bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline.get());
    m_CmdBufs[0]->bindVertexBuffers(0, { vertexBuf.get() }, { 0 }); // 追加
    m_CmdBufs[0]->bindIndexBuffer(indexBuf.get(), 0, vk::IndexType::eUint16);
    m_CmdBufs[0]->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout.get(), 0, { descSets[0].get() }, {});
    m_CmdBufs[0]->drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    
    m_CmdBufs[0]->endRenderPass();

    m_CmdBufs[0]->end();

    vk::CommandBuffer submitCmdBuf[1] = { m_CmdBufs[0].get() };
    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = submitCmdBuf;

    vk::Semaphore renderwaitSemaphores[] = { m_swapchainImgSemaphore.get() };
    vk::PipelineStageFlags renderwaitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = renderwaitSemaphores;
    submitInfo.pWaitDstStageMask = renderwaitStages;

    vk::Semaphore renderSignalSemaphores[] = { imgRenderedSemaphorne.get() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = renderSignalSemaphores;

    m_graphicsQueue.submit({ submitInfo }, imgRenderedFence.get());

    vk::PresentInfoKHR presentInfo;

    auto presentSwapchains = { m_Swapchain.get() };
    auto imgIndices = { imgIndex };

    presentInfo.swapchainCount = static_cast<uint32_t>(presentSwapchains.size());
    presentInfo.pSwapchains = presentSwapchains.begin();
    presentInfo.pImageIndices = imgIndices.begin();

    vk::Semaphore presenWaitSemaphores[] = { imgRenderedSemaphorne.get() };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = presenWaitSemaphores;
    
    m_graphicsQueue.presentKHR(presentInfo);
}