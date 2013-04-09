#include "m3_ShaderService.h"

m3_ShaderService::m3_ShaderService()
{

}

void m3_ShaderService::_insertShader(std::string key)
{
	m3_CgShader *_cgShader = new m3_CgShader();

	if(cgGLIsProfileSupported(CG_PROFILE_ARBVP1) )
        _cgShader->cg_VertexProfile = CG_PROFILE_ARBVP1;
    else if( cgGLIsProfileSupported(CG_PROFILE_VP40) )
        _cgShader->cg_VertexProfile = CG_PROFILE_VP40;
	
	if( cgGLIsProfileSupported(CG_PROFILE_ARBFP1) )
        _cgShader->cg_PixelProfile = CG_PROFILE_ARBFP1;
    else if( cgGLIsProfileSupported(CG_PROFILE_FP30) )
        _cgShader->cg_PixelProfile = CG_PROFILE_FP30;
	else if( cgGLIsProfileSupported(CG_PROFILE_FP20) )
        _cgShader->cg_PixelProfile = CG_PROFILE_FP20;


	_cgShader->cg_Context = cgCreateContext();
	
	_cgShader->cg_VertexProgram = cgCreateProgramFromFile(_cgShader->cg_Context,
										          CG_SOURCE,
										         "Content\\shaders\\vertex_shader.cg",
										         _cgShader->cg_VertexProfile,
										         NULL, 
										         NULL );

	_cgShader->cg_PixelProgram = cgCreateProgramFromFile( _cgShader->cg_Context,
										         CG_SOURCE,
										         "Content\\shaders\\pixel_shader.cg",
										         _cgShader->cg_PixelProfile,
										         NULL, 
										         NULL );

	cgGLLoadProgram( _cgShader->cg_VertexProgram );
	cgGLLoadProgram( _cgShader->cg_PixelProgram );

	cgGLBindProgram( _cgShader->cg_VertexProgram );
	cgGLBindProgram( _cgShader->cg_PixelProgram );
	
	_shaderContainer.insert(std::pair<std::string,m3_CgShader*>(key,_cgShader));
}

m3_CgShader* m3_ShaderService::GetShader(std::string value)
{
	std::map<std::string,m3_CgShader*>::iterator shaderIterator = _shaderContainer.find(value);
	if(shaderIterator != _shaderContainer.end())
		return shaderIterator->second;
	else
	{
		_insertShader(value);
		shaderIterator = _shaderContainer.find(value);
		if(shaderIterator != _shaderContainer.end())
			return shaderIterator->second;
		else
			return NULL;
	}
}