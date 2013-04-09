#pragma once
#ifndef _M3_M3LOADER_H_
#define _M3_M3LOADER_H_
#pragma warning(disable:4786)

#include <windows.h>
#include <string>
#include <fstream>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glext.h"
#include <vector>
#include "m3_FileFormat.h"


class m3_M3Loader
{
	private :
					
	public :
		static void Initilize();

		static PFNGLGENBUFFERSARBPROC glGenBuffersARB;					
		static PFNGLBINDBUFFERARBPROC glBindBufferARB;					
		static PFNGLBUFFERDATAARBPROC glBufferDataARB;					
		static PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
		static PFNGLMAPBUFFERARBPROC     glMapBufferARB;
		static PFNGLUNMAPBUFFERARBPROC   glUnmapBufferARB;

		static m3_VertexBufferPtr* LoadFileData(std::string value);
    static m3_VertexBufferPtr* LoadMd34FileData(std::string value);
		static m3_VertexBufferID*  Load(m3_VertexBufferPtr* value);
};

#endif