//	ファイル名	：ShaderList.hxx
//	  概  要	：
//	作	成	者	：daigo
//	 作成日時	：2024/08/02 14:40:08
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____ShaderList_HXX_____
#define _____ShaderList_HXX_____

// =-=-= インクルード部 =-=-=
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