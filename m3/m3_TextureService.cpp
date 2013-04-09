#include "m3_TextureService.h"

DWORD __stdcall textureLoadThread(LPVOID value)
{
	((m3_TextureService*)value)->_textureLoadThread();
	return 0;
}

m3_TextureService::m3_TextureService()
{
	InitializeCriticalSection( &textureRequestCriticalSection );
	_thread = CreateThread(NULL,NULL,textureLoadThread,this,NULL,NULL);
	SetThreadPriority(_thread,THREAD_PRIORITY_NORMAL);
}


void m3_TextureService::_insertTextureData(std::string key)
{
	std::map<std::string,DDS_IMAGE_DATA*>::iterator textureDataIterator = _textureDataContainer.find(key);
	if(textureDataIterator == _textureDataContainer.end())
		_textureDataContainer.insert(std::pair<std::string,DDS_IMAGE_DATA*>(key,m3_DDSLoader::LoadImageData(key)));
}


void m3_TextureService::_insertTexture(std::string key, GLuint value)
{
	_textureContainer.insert(std::pair<std::string,GLuint>(key,value));
	std::vector<std::string>::iterator _requestIterator = _requestTexture.begin();
	while(_requestIterator != _requestTexture.end())
	{
		if(key == (*_requestIterator))
		{
			EnterCriticalSection( &textureRequestCriticalSection );
			_requestTexture.erase(_requestIterator);
			LeaveCriticalSection( &textureRequestCriticalSection );
			return;
		}
		++_requestIterator;
	}
}

GLint m3_TextureService::GetTexture(std::string value)
{
	std::map<std::string,GLuint>::iterator textureIterator = _textureContainer.find(value);
	if(textureIterator != _textureContainer.end())
		return textureIterator->second;
	else
	{
		std::vector<std::string>::iterator _requestIterator = _requestTexture.begin();
		while(_requestIterator != _requestTexture.end())
		{
			if(value == (*_requestIterator))
			{
				std::map<std::string,DDS_IMAGE_DATA*>::iterator textureDataIterator = _textureDataContainer.find(value);
				if(textureDataIterator != _textureDataContainer.end())
				{
					GLuint textureId =  m3_DDSLoader::Load(_textureDataContainer.find(value)->second);
					_insertTexture(value,textureId);
					_textureDataContainer.erase(textureDataIterator);
					return textureId;
				}
				else
					return 0;
			}
			++_requestIterator;
		}
		EnterCriticalSection( &textureRequestCriticalSection );
		_requestTexture.push_back(value);	
		LeaveCriticalSection( &textureRequestCriticalSection );		

		return 0;
	}
}

void m3_TextureService::_textureLoadThread()
{
	while(TRUE)
	{
		Sleep(1000);
		if(!_requestTexture.size())
			continue;
		EnterCriticalSection( &textureRequestCriticalSection );
		std::vector<std::string>::iterator _requestIterator = _requestTexture.begin();
		while(_requestIterator != _requestTexture.end())
		{
			_insertTextureData(*_requestIterator);
			++_requestIterator;
		}
		LeaveCriticalSection( &textureRequestCriticalSection );		
	}
}