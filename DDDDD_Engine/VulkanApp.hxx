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
class Vulkan:public SingletonBase
{
public:
	SINGLETON_MAKES(Vulkan)

	Vulkan();
	~Vulkan();

	/// @brief 初期化処理
	/// @return 成功したらtrue
	bool Init();

	/// @brief 更新処理
	void Update();

	/// @brief 終了処理
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