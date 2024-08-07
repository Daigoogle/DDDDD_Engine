//	ファイル名	：FragmentShader.hxx
//	  概  要	：
//	作	成	者	：daigo
//	 作成日時	：2024/08/02 16:22:35
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____FragmentShader_HXX_____
#define _____FragmentShader_HXX_____

// =-=-= インクルード部 =-=-=
#include "Shader.hxx"

class FragmentShader: public shader
{
public:
	FragmentShader();
	~FragmentShader();

	void Bind() override;
private:

};

#endif // !_____FragmentShader_HXX_____