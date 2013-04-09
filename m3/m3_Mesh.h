#pragma once
#ifndef _M3_MESH_H_
#define _M3_MESH_H_
#pragma warning(disable:4786)

#include <string>
//#include <fstream>
#include <math.h>
#include "m3_Core.h"
#include "m3_DDSLoader.h"
#include "m3_FileFormat.h"
#include "m3_Input.h"
#include "m3_Resource.h"
#include "Matrix4x4.h"

#define GL_ARRAY_BUFFER_ARB 0x8892
#define GL_STATIC_DRAW_ARB 0x88E4
typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, int size, const GLvoid *data, GLenum usage);

struct m3_Vertex
{
	float u,v;
	float normal[3];
	float x, y, z;
};

class m3_Mesh
{
	// VARIBLES
private :
	m3_ComplexMeshData *_complexMeshData;
	m3_VertexBufferPtr *_vertexBufferPtr;
	m3_VertexBufferID *_vertexBufferId;	
	m3_CgShader       *_cgShader;
	m3_CgShaderBasicParams _cgShaderParams;
	Vector3d Rotation;
	// FUNCTION
private :
	void _CalculateNormals();	
public :
	std::string meshOriginalName;
	std::string _meshFileName;
	std::string _textureFileName;
	GLuint textureColorId;
	GLuint textureNormalId;
	void _RenderBox();
	// FUNCTION
public :
	m3_Mesh();
	void Load(const char* fileName,const char* meshName);
	void Convert();
	void Update();
	void Render();
	void RenderNormals();
	void RenderBone();
};

#endif