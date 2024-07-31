//	�t�@�C����	�Fshader.hxx
//	  �T  �v	�F
//	��	��	��	�Fdaigo
//	 �쐬����	�F2024/07/14 22:23:09
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= �C���N���[�h�K�[�h�� =-=-=
#ifndef _____shader_HXX_____
#define _____shader_HXX_____

// =-=-= �C���N���[�h�� =-=-=
#include <vulkan/vulkan.hpp>
#include <vector>
#include <string>

class shader
{
public:
	shader();
	~shader();

	void LoadShader(std::string fileName);

	void CreateBuffer();

	void WriteBuffer(uint16_t slot,void* data);
	void SetTexture(uint16_t slot, vk::ImageView texture);

	virtual void Bind() = 0;
private:
	vk::Device m_Device;
	std::vector<void*> m_DeviceBuffer;
};

#endif // !_____shader_HXX_____