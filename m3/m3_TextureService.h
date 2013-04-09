#pragma once
#ifndef _M3_TEXTURESERVICE_H_
#define _M3_TEXTURESERVICE_H_
#pragma warning(disable:4786)

#include <string>
#include <map>
#include <vector>

#include "m3_DDSLoader.h"

DWORD __stdcall textureLoadThread(LPVOID value);

class m3_TextureService
{
	private :
		std::map<std::string,GLuint> _textureContainer;
		std::map<std::string,DDS_IMAGE_DATA*> _textureDataContainer;
		std::vector<std::string> _requestTexture;
		
		HANDLE _thread;
		CRITICAL_SECTION textureRequestCriticalSection;

		void _insertTextureData(std::string key);
		void _insertTexture(std::string key, GLuint value);
	public :
		GLint GetTexture(std::string value);
		m3_TextureService();
		void _textureLoadThread();
};

#endif