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

enum class VSDefine
{
	Default,
};
enum class FSDefine
{
	Default,
};

class ShaderList:public Singleton<ShaderList>
{
	friend class Singleton<ShaderList>;
	enum class ShaderType {
		Vertex,
		Fragment,
		Geometry,
	};
public:
	bool Init() override;
	void ReSize();
private:
	ShaderList();
	~ShaderList();

	void LoadShader(const std::string& path, ShaderType type);

	std::vector<vk::PipelineShaderStageCreateInfo> m_ShaderStages;
	std::vector<vk::UniqueShaderModule> m_ShaderList;

	vk::Device m_Device;
};

#endif // !_____ShaderList_HXX_____