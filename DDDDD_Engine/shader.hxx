//	ファイル名	：shader.hxx
//	  概  要	：
//	作	成	者	：daigo
//	 作成日時	：2024/07/14 22:23:09
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____shader_HXX_____
#define _____shader_HXX_____

// =-=-= インクルード部 =-=-=
#include <vulkan/vulkan.hpp>
#include <vector>
#include <string>

class shader
{
public:
	shader();
	~shader();

	void LoadShader(std::string fileName);

private:

};

#endif // !_____shader_HXX_____