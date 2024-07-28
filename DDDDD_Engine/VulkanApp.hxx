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
class VulkanApp:public SingletonBase
{
public:
	// =-=-= �V���O���g���� =-=-=
	SINGLETON_MAKES(VulkanApp)

	/// @brief �R���X�g���N�^
	VulkanApp();
	/// @brief �f�X�g���N�^
	~VulkanApp();

	/// @brief ����������
	/// @return ����������true
	bool Init() override;
	/// @brief �X�V����
	void Update() override;

	VkDevice GetDevice() { return m_Device.get(); }
private:
	/// @brief �X���b�v�`�F�C���̍č쐬
	void RecreateSwapchain();

	SDL_Window*		m_Window;
	unsigned int	m_extension_count;

	vk::PhysicalDevice physicalDevice;
	vk::SurfaceFormatKHR swapchainFormat;
	vk::PresentModeKHR swapchainPresentMode;
	std::vector<vk::Image> swapchainImages;
	vk::VertexInputBindingDescription vertexBindingDescription[1];
	vk::VertexInputAttributeDescription vertexInputDescription[2];

	std::vector<const char*>				m_extensions;
	std::vector<const char*>				m_Layers;
	std::vector<vk::UniqueFramebuffer>		m_swapchainFramebufs;
	std::vector<vk::UniqueImageView>		m_swapchainImageViews;
	std::vector<vk::UniqueCommandBuffer>	m_CmdBufs;
	vk::Queue								m_graphicsQueue;

	vk::UniqueInstance			m_Instance;
	vk::UniqueSurfaceKHR		m_uSurface;
	vk::UniqueDevice			m_Device;
	vk::UniqueFence				m_SwapchainImgFence;
	vk::UniqueSwapchainKHR		m_Swapchain;
	vk::UniqueRenderPass		m_Renderpass;
	vk::UniquePipelineLayout	pipelineLayout;
	vk::UniquePipeline			m_Pipeline;
	vk::UniqueCommandPool		cmdPool;
	vk::UniqueShaderModule		vertShader;
	vk::UniqueShaderModule		fragShader;
	vk::UniqueSemaphore			imgRenderedSemaphorne;
	vk::UniqueSemaphore			m_swapchainImgSemaphore;
	vk::UniqueFence				imgRenderedFence;
	vk::UniqueDeviceMemory		vertexBufMemory;
	vk::UniqueDeviceMemory		indexBufMemory;
	vk::UniqueBuffer			vertexBuf;
	vk::UniqueBuffer			indexBuf;
	vk::UniqueDeviceMemory stagingBufMemory;
};

#endif // !_____Vulkan_HXX_____