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

// =-=-= �N���X��`�� =-=-=
class Vulkan
{
public:
	/// @brief ����������
	/// @return ����������true
	static bool Init();

	/// @brief �X�V����
	static void Update();

	/// @brief �I������
	static void UnInit();

	/// @brief �E�B���h�E���J���Ă��邩
	/// @return �J���Ă���Ȃ�true
	static bool IsRunning() { return m_stillRunning; }

	/// @brief �C�x���g���擾
	/// @return �C�x���g
	static SDL_Event GetEvent() { return m_event; }

	/// @brief 1�t���[��������̎���(�~���b)���擾
	/// @return 1�t���[��������̎���(�~���b)
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