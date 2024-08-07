//	ファイル名	：VertexShader.hxx
//	  概  要	：
//	作	成	者	：daigo
//	 作成日時	：2024/08/02 16:21:48
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= インクルードガード部 =-=-=
#ifndef _____VertexShader_HXX_____
#define _____VertexShader_HXX_____

// =-=-= インクルード部 =-=-=
#include "Shader.hxx"

class VertexShader : public shader
{
public:
	VertexShader();
	~VertexShader();

	void Bind() override;
private:

};

#endif // !_____VertexShader_HXX_____