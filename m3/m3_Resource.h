#pragma once
#ifndef _M3_RESOURCE_H_
#define _M3_RESOURCE_H_
#pragma warning(disable:4786)

#include "m3_TextureService.h"
#include "m3_MeshService.h"
#include "m3_ShaderService.h"

class m3_Resource
{
	private :
		static m3_TextureService *_textureServiceInstance;
		static bool _textureServiceSingleFlag;

		static m3_MeshService *_meshServiceInstance;
		static bool _meshServiceSingleFlag;

		static m3_ShaderService *_shaderServiceInstance;
		static bool _shaderServiceSingleFlag;
	public :

		static m3_MeshService    * MeshContainer();
		static m3_TextureService * TextureContainer();
		static m3_ShaderService  * ShaderContainer();
};

#endif