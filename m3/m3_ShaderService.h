#pragma once
#ifndef _M3_SHADERSERVICE_H_
#define _M3_SHADERSERVICE_H_

#include <windows.h>
#include <Cg/Cg.h>
#include <Cg/cgGL.h>
#include <stdlib.h>
//#include <fstream>
#include <map>
#include <vector>
#include <string>


struct m3_CgShader
{
	CGcontext   cg_Context;
	CGprofile   cg_VertexProfile;
	CGprofile   cg_PixelProfile;
	CGprogram   cg_VertexProgram;
	CGprogram   cg_PixelProgram;
};

struct m3_CgShaderBasicParams
{
	CGparameter cg_Texture;

	CGparameter cg_mWorldViewProjection;
	CGparameter cg_mWorldViewInverse;

	CGparameter cg_vCamera;
	CGparameter cg_vLight;

	CGparameter cg_bLightFlag;
};

class m3_ShaderService
{
	private :
		std::map<std::string,m3_CgShader*> _shaderContainer;
		std::vector<std::string> _requestShader;
		
		HANDLE _thread;
		CRITICAL_SECTION shaderRequestCriticalSection;

		void _insertShader(std::string key);
	public :
		void _shaderLoadThread();
		m3_CgShader* GetShader(std::string value);
		m3_ShaderService();	
};

#endif