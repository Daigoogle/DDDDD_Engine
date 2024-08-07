//	ファイル名	：Vulkan.hxx
//	  概  要		：Vulkanの初期化、更新、終了処理を行うクラス
//	作	成	者	：daigo
//	 作成日時	：2024/06/09 16:24:36
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____Vulkan_HXX_____
#define _____Vulkan_HXX_____

// =-=-= インクルード部 =-=-=
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <vulkan/vulkan.hpp>
#include <vector>
#include "SingletonsMng.hxx"

// =-=-= クラス定義部 =-=-=
class VulkanApp:public Singleton<VulkanApp>
{
	friend class Singleton<VulkanApp>;
public:
	/// @brief 初期化処理
	/// @return 成功したらtrue
	bool Init() override;
	/// @brief 更新処理
	void Update() override;

	/// @brief デバイスの取得
	/// @return デバイス
	vk::Device GetDevice() { return m_Device.get(); }
private:
	/// @brief コンストラクタ
	VulkanApp();
	/// @brief デストラクタ
	~VulkanApp();

	/// @brief スワップチェインの再作成
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
	std::vector<vk::UniqueDescriptorSet>	descSets;
	std::vector<vk::UniqueDescriptorSet>	descSets2;
	vk::Queue								m_graphicsQueue;

	vk::UniqueInstance			m_Instance;
	vk::UniqueSurfaceKHR		m_uSurface;
	vk::UniqueDevice			m_Device;
	vk::UniqueFence				m_SwapchainImgFence;
	vk::UniqueFence				imgRenderedFence;
	vk::UniqueSwapchainKHR		m_Swapchain;
	vk::UniqueRenderPass		m_Renderpass;
	vk::UniquePipelineLayout	pipelineLayout;
	vk::UniquePipeline			m_Pipeline;
	vk::UniqueCommandPool		cmdPool;
	vk::UniqueCommandPool		tmpCmdPool;
	vk::UniqueShaderModule		vertShader;
	vk::UniqueShaderModule		fragShader;
	vk::UniqueSemaphore			imgRenderedSemaphorne;
	vk::UniqueSemaphore			m_swapchainImgSemaphore;
	vk::UniqueBuffer			vertexBuf;
	vk::UniqueBuffer			indexBuf;
	vk::UniqueBuffer			stagingBuf;
	vk::UniqueBuffer			uniformBuf;
	vk::UniqueBuffer			uniformBuf2;
	vk::UniqueDeviceMemory		vertexBufMemory;
	vk::UniqueDeviceMemory		indexBufMemory;
	vk::UniqueDeviceMemory		stagingBufMemory;
	vk::UniqueDeviceMemory		uniformBufMemory;
	vk::UniqueDeviceMemory		uniformBufMemory2;
	vk::UniqueDescriptorSetLayout descSetLayout;
	vk::UniqueDescriptorPool	descPool;
	vk::UniqueDescriptorSetLayout descSetLayout2;
	vk::UniqueDescriptorPool	descPool2;

	void* pUniformBufMem;
	void* pUniformBufMem2;
};

#endif // !_____Vulkan_HXX_____