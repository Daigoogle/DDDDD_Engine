//	�t�@�C����	�FVulkan.hxx
//	  �T  �v		�FVulkan�̏������A�X�V�A�I���������s���N���X
//	��	��	��	�Fdaigo
//	 �쐬����	�F2024/06/09 16:24:36
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= �C���N���[�h�K�[�h�� =-=-=
#ifndef _____Vulkan_HXX_____
#define _____Vulkan_HXX_____

// =-=-= �C���N���[�h�� =-=-=
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <vulkan/vulkan.hpp>
#include <vector>
#include "SingletonsMng.hxx"

// =-=-= �N���X��`�� =-=-=
class Vulkan:public SingletonBase
{
public:
	SINGLETON_MAKES(Vulkan)

	Vulkan();
	~Vulkan();

	/// @brief ����������
	/// @return ����������true
	bool Init();

	/// @brief �X�V����
	void Update();

	/// @brief �I������
	void UnInit();
private:
	SDL_Window*					m_Window;
	unsigned int				m_extension_count;
	std::vector<const char*>	m_extensions;
	std::vector<const char*>	m_Layers;
	vk::ApplicationInfo			m_AppInfo;
	vk::InstanceCreateInfo		m_InstInfo;
	vk::UniqueInstance			m_Instance;
	vk::UniqueSurfaceKHR		m_uSurface;
	vk::UniqueDevice m_Device;
	vk::UniqueFence m_SwapchainImgFence;
	vk::UniqueSwapchainKHR m_Swapchain;
	std::vector<vk::UniqueCommandBuffer> m_CmdBufs;
	vk::UniqueRenderPass m_Renderpass;
	std::vector<vk::UniqueFramebuffer> swapchainFramebufs;
	std::vector<vk::UniqueImageView> swapchainImageViews;
	vk::UniquePipelineLayout pipelineLayout;
	vk::UniquePipeline m_Pipeline;
	vk::Queue graphicsQueue;
	vk::CommandBufferAllocateInfo cmdBufAllocInfo;
	vk::UniqueCommandPool cmdPool;
	vk::UniqueShaderModule vertShader;
	vk::UniqueShaderModule fragShader;
};

#endif // !_____Vulkan_HXX_____