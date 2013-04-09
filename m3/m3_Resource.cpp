#include "m3_Resource.h"


m3_TextureService *m3_Resource::_textureServiceInstance = NULL;
bool m3_Resource::_textureServiceSingleFlag = false;

m3_TextureService *m3_Resource::TextureContainer()
{
	if(_textureServiceSingleFlag)
		return _textureServiceInstance;
	else
	{
		_textureServiceSingleFlag = true;
		_textureServiceInstance = new m3_TextureService();
		return _textureServiceInstance;
	}
}

m3_MeshService *m3_Resource::_meshServiceInstance = NULL;
bool m3_Resource::_meshServiceSingleFlag = false;

m3_MeshService *m3_Resource::MeshContainer()
{
	if(_meshServiceSingleFlag)
		return _meshServiceInstance;
	else
	{
		_meshServiceSingleFlag = true;
		_meshServiceInstance = new m3_MeshService();
		return _meshServiceInstance;
	}
}


m3_ShaderService *m3_Resource::_shaderServiceInstance = NULL;
bool m3_Resource::_shaderServiceSingleFlag = false;

m3_ShaderService *m3_Resource::ShaderContainer()
{
	if(_shaderServiceSingleFlag)
		return _shaderServiceInstance;
	else
	{
		_shaderServiceSingleFlag = true;
		_shaderServiceInstance = new m3_ShaderService();
		return _shaderServiceInstance;
	}
}

