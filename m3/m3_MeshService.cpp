#include "m3_MeshService.h"


DWORD __stdcall meshLoadThread(LPVOID value)
{
	m3_M3Loader::Initilize();
	((m3_MeshService*)value)->_meshLoadThread();
	return 0;
}

m3_MeshService::m3_MeshService()
{
	InitializeCriticalSection( &meshRequestCriticalSection );
	_thread = CreateThread(NULL,NULL,meshLoadThread,this,NULL,NULL);
	SetThreadPriority(_thread,THREAD_PRIORITY_NORMAL);
}

m3_MeshExtension m3_MeshService::_checkFileExtension(std::string value)
{
	std::string extension = "";
	for( int i = (value.length() - 1); i > -1; --i)
		if(value[i] != '.')
			extension += value[i];
		else
		{
			if(extension == "3m")
      {
	      std::ifstream inStream;
	      inStream.open(value.c_str(),std::ios::in | std::ios::binary);
	      inStream.seekg(0, std::ios::end);
	      int fileSize = inStream.tellg();
          inStream.seekg (0, std::ios::beg);
        if(fileSize>4)
          fileSize = 4;
	      char fileData[16];
	      inStream.read(fileData,fileSize);
	      inStream.close();
        if(fileData[1] == '3'&&fileData[0] =='4')
          return MESH_M34;
        else
          return MESH_M3;
      }
			if(extension == "sd3")
				return MESH_3DS;
			return MESH_NONE;
		}
	return MESH_NONE;
}

void m3_MeshService::_insertMeshData(std::string key)
{
	std::map<std::string,m3_VertexBufferPtr*>::iterator meshDataIterator = _meshDataContainer.find(key);
	if(meshDataIterator == _meshDataContainer.end())
	{
		switch(_checkFileExtension(key))
		{
      case MESH_M34:
				_meshDataContainer.insert(std::pair<std::string,m3_VertexBufferPtr*>(key,m3_M3Loader::LoadMd34FileData(key)));
        break;
			case MESH_M3:
				_meshDataContainer.insert(std::pair<std::string,m3_VertexBufferPtr*>(key,m3_M3Loader::LoadFileData(key)));
			break;

			break;
		}
	}
}


void m3_MeshService::_insertMesh(std::string key, m3_VertexBufferID *value)
{
	_meshIdContainer.insert(std::pair<std::string,m3_VertexBufferID*>(key,value));

	m3_ComplexMeshData* complexData = new m3_ComplexMeshData();
	complexData->vertexDataId = value;
	complexData->vertexDataPtr = _meshDataContainer.find(key)->second;
	_meshComplexContainer.insert(std::pair<std::string,m3_ComplexMeshData*>(key,complexData));
	std::vector<std::string>::iterator _requestIterator = _requestMesh.begin();
	while(_requestIterator != _requestMesh.end())
	{
		if(key == (*_requestIterator))
		{
			EnterCriticalSection( &meshRequestCriticalSection );
			_requestMesh.erase(_requestIterator);
			LeaveCriticalSection( &meshRequestCriticalSection );
			return;
		}
		++_requestIterator;
	}
}

m3_ComplexMeshData* m3_MeshService::GetMesh(std::string value)
{
	std::map<std::string,m3_ComplexMeshData*>::iterator meshIterator = _meshComplexContainer.find(value);
	if(meshIterator != _meshComplexContainer.end())
		return meshIterator->second;
	else
	{
		std::vector<std::string>::iterator _requestIterator = _requestMesh.begin();
		while(_requestIterator != _requestMesh.end())
		{
			if(value == (*_requestIterator))
			{
				std::map<std::string,m3_VertexBufferPtr*>::iterator meshDataIterator = _meshDataContainer.find(value);
				if(meshDataIterator != _meshDataContainer.end())
				{
					m3_VertexBufferID *meshVertexBufferId = NULL;
					switch(_checkFileExtension(value))
					{
            case MESH_M34:
						case MESH_M3:
							meshVertexBufferId = m3_M3Loader::Load(_meshDataContainer.find(value)->second);
						break;

						break;
					}
					
					_insertMesh(value,meshVertexBufferId);
					return _meshComplexContainer[value];
				}
				else
					return NULL;
			}
			++_requestIterator;
		}
		EnterCriticalSection( &meshRequestCriticalSection );
		_requestMesh.push_back(value);	
		LeaveCriticalSection( &meshRequestCriticalSection );		

		return NULL;
	}
}

void m3_MeshService::_meshLoadThread()
{
	while(TRUE)
	{
		Sleep(1000);
		if(!_requestMesh.size())
			continue;
		EnterCriticalSection( &meshRequestCriticalSection );
		std::vector<std::string>::iterator _requestIterator = _requestMesh.begin();
		while(_requestIterator != _requestMesh.end())
		{
			_insertMeshData(*_requestIterator);
			++_requestIterator;
		}
		LeaveCriticalSection( &meshRequestCriticalSection );		
	}
}