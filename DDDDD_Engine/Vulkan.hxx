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

// =-=-= クラス定義部 =-=-=
class Vulkan
{
public:
	/// @brief 初期化処理
	/// @return 成功したらtrue
	static bool Init();

	/// @brief 更新処理
	static void Update();

	/// @brief 終了処理
	static void UnInit();

	/// @brief ウィンドウが開いているか
	/// @return 開いているならtrue
	static bool IsRunning() { return m_stillRunning; }

	/// @brief イベントを取得
	/// @return イベント
	static SDL_Event GetEvent() { return m_event; }

	/// @brief 1フレーム当たりの時間(ミリ秒)を取得
	/// @return 1フレーム当たりの時間(ミリ秒)
	static unsigned int DeltaTime() { return m_DeltaTime; }
private:
	static SDL_Window*				m_Window;
	static unsigned int				m_extension_count;
	static std::vector<const char*>*m_extensions;
	static std::vector<const char*> m_layers;
	static vk::ApplicationInfo		m_appInfo;
	static vk::InstanceCreateInfo	m_instInfo;
	static vk::Instance				m_instance;
	static VkSurfaceKHR				m_surface;
	static vk::SurfaceKHR*			m_pSurface;
	static bool						m_stillRunning;
	static SDL_Event				m_event;
	static unsigned int				m_DeltaTime;
};

#endif // !_____Vulkan_HXX_____