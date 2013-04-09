#pragma once
#ifndef _M3_DDSLOADER_H_
#define _M3_DDSLOADER_H_

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <ddraw.h> 
#include "glext.h"
#include <stdlib.h>
#include <fstream>


struct DDS_IMAGE_DATA
{
    GLsizei  width;
    GLsizei  height;
    GLint    components;
    GLenum   format;
    int      numMipMaps;
    GLubyte *pixels;
};

class m3_DDSLoader
{
	// VARIBLES
private :
	// FUNCTION
private :
    // VARIBLES
public :
	// FUNCTION
public :
	static GLuint Load(DDS_IMAGE_DATA* value);
	static DDS_IMAGE_DATA* LoadImageData(std::string value);
	static PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB;
};

#endif

