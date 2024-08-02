//	�t�@�C����	�FShaderList.hxx
//	  �T  �v	�F
//	��	��	��	�Fdaigo
//	 �쐬����	�F2024/08/02 14:40:08
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= �C���N���[�h�K�[�h�� =-=-=
#ifndef _____ShaderList_HXX_____
#define _____ShaderList_HXX_____

// =-=-= �C���N���[�h�� =-=-=
#include <vulkan/vulkan.hpp>
#include <vector>

#include "SingletonsMng.hxx"

class ShaderList:public Singleton<ShaderList>
{
	friend class Singleton<ShaderList>;
public:
	void LoadVertexShader(std::string fileName);
	void LoadFragmentShader(std::string fileName);
private:
	ShaderList();
	~ShaderList();

	std::vector<vk::UniqueShaderModule> m_ShaderList;
};

#endif // !_____ShaderList_HXX_____