#include "ShaderList.hxx"

#include <iostream>
#include <fstream>
#include "SDLApp.hxx"
#include "VulkanApp.hxx"


const std::string shaderPath = "Assets/Shader/";

enum class VertInputDesc {
	Float1,
    Float2,
    Float3,
    Float4,
    Int1,
    Int2,
    Int3,
    Int4,
};

ShaderList::ShaderList()
	:Singleton(UPDATE_ORDER::NO_UPDATE)
{

}

ShaderList::~ShaderList()
{

}

void ShaderList::LoadShader(const std::string& path,ShaderType type)
{
    size_t vertSpvFileSz;
    std::ifstream vertSpvFile(shaderPath + path , std::ios_base::binary | std::ifstream::ate);
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

    m_ShaderList.emplace_back(VulkanApp::GetInstance().GetDevice().createShaderModuleUnique(vertShaderCreateInfo));

    m_ShaderStages.emplace_back();
    m_ShaderStages.back().module = m_ShaderList.back().get();
    m_ShaderStages.back().pName = "main";

    if (type == ShaderType::Vertex)
    {
        m_ShaderStages.back().stage = vk::ShaderStageFlagBits::eVertex;
    }
    else if (type == ShaderType::Fragment)
    {
        m_ShaderStages.back().stage = vk::ShaderStageFlagBits::eFragment;
    }
    else if (type == ShaderType::Geometry)
    {
        m_ShaderStages.back().stage = vk::ShaderStageFlagBits::eGeometry;
    }
    else
        return;
}

bool ShaderList::Init()
{
#if 0
    // 画面サイズの取得
    SDLApp& sdlApp = SDLApp::GetInstance();
    uint32_t screenWidth = sdlApp.GetWndWidth(), screenHeight = sdlApp.GetWndHeight();

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

    vk::UniqueRenderPass m_Renderpass = m_Device.createRenderPassUnique(renderpassCreateInfo);

    // ビューポートの設定
    std::vector<vk::Viewport> viewports;
    viewports[0].x = 0.0;
    viewports[0].y = 0.0;
    viewports[0].minDepth = 0.0;
    viewports[0].maxDepth = 1.0;
    viewports[0].width = static_cast<float>(screenWidth);
    viewports[0].height = static_cast<float>(screenHeight);

    // シザーの設定
    std::vector<vk::Rect2D> scissors;
    scissors[0].offset = vk::Offset2D{ 0, 0 };
    scissors[0].extent = vk::Extent2D{ screenWidth, screenHeight };

    // ビューポートの作成
    vk::PipelineViewportStateCreateInfo viewportState;
    viewportState.viewportCount = viewports.size();
    viewportState.pViewports = viewports.data();
    viewportState.scissorCount = scissors.size();
    viewportState.pScissors = scissors.data();

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
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(m_ShaderStages.size());
    pipelineCreateInfo.pStages = m_ShaderStages.data();
    vk::UniquePipeline m_Pipeline = m_Device.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo).value;
#endif
	return true;
}

void ShaderList::ReSize()
{
}