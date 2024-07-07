#include "VulkanApp.hxx"
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL_syswm.h>
#include <iostream>
#include <fstream>
#include <vulkan/vulkan.h>
#include "SDLApp.hxx"


VulkanApp::VulkanApp()
    : SingletonBase(UPDATE_ORDER::SECOND_UPDATE)
    , m_Window(nullptr)
    , m_extension_count(0)
{

}

VulkanApp::~VulkanApp()
{
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

    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(m_uSurface.get());
    std::vector<vk::SurfaceFormatKHR> surfaceFormats = physicalDevice.getSurfaceFormatsKHR(m_uSurface.get());
    std::vector<vk::PresentModeKHR> surfacePresentModes = physicalDevice.getSurfacePresentModesKHR(m_uSurface.get());

    swapchainFormat = surfaceFormats[0];
    swapchainPresentMode = surfacePresentModes[0];

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

    swapchainImages = m_Device->getSwapchainImagesKHR(m_Swapchain.get());



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

/// @brief 更新処理
void VulkanApp::Update()
{
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
    m_CmdBufs[0]->draw(3, 1, 0, 0);

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
    viewports[0].width = static_cast<float>( screenWidth);
    viewports[0].height = static_cast<float>(screenHeight);

    vk::Rect2D scissors[1];
    scissors[0].offset = vk::Offset2D{ 0, 0 };
    scissors[0].extent = vk::Extent2D{ screenWidth, screenHeight };

    vk::PipelineViewportStateCreateInfo viewportState;
    viewportState.viewportCount = 1;
    viewportState.pViewports = viewports;
    viewportState.scissorCount = 1;
    viewportState.pScissors = scissors;

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;

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

    vk::PipelineLayoutCreateInfo layoutCreateInfo;
    layoutCreateInfo.setLayoutCount = 0;
    layoutCreateInfo.pSetLayouts = nullptr;

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