#pragma once
#ifndef _M3_MESHSERVICE_H_
#define _M3_MESHSERVICE_H_
#pragma warning(disable:4786)

#include <stdlib.h>
//#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "m3_M3Loader.h"

DWORD __stdcall meshLoadThread(LPVOID value);

class m3_MeshService
{
	private :
		std::map<std::string,m3_ComplexMeshData*> _meshComplexContainer;
		std::map<std::string,m3_VertexBufferID*>  _meshIdContainer;
		std::map<std::string,m3_VertexBufferPtr*> _meshDataContainer;
		std::vector<std::string> _requestMesh;
		
		HANDLE _thread;
		CRITICAL_SECTION meshRequestCriticalSection;

		void _insertMeshData(std::string key);
		void _insertMesh(std::string key, m3_VertexBufferID *value);

		m3_MeshExtension _checkFileExtension(std::string value);
	public :
		m3_ComplexMeshData* GetMesh(std::string value);
		m3_MeshService();
		void _meshLoadThread();
};

#endif