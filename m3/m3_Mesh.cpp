#include "m3_Mesh.h"

m3_Mesh::m3_Mesh()
{
	_complexMeshData = NULL;
	_cgShader = NULL;
	_meshFileName = "";
	_textureFileName = "";
	textureColorId = -1;
	textureNormalId = -1;
	Rotation = Vector3d(0.0f,0.0f,0.0f);
}

void m3_Mesh::Load(const char* fileName,const char* meshName)
{
	meshOriginalName = meshName;
	_meshFileName = fileName;
	std::string _textureFilePath   = "m3\\";
	std::string _textureName = meshName;
	std::string extension = "";
	unsigned int extensionCutSize = 0;

	for( int i = (_meshFileName.length() - 1); i > -1; --i)
		if(_meshFileName[i] != '.')
			extension += _meshFileName[i];
		else
		{
			if(extension == "3m")
				extensionCutSize = 3;
			if(extension == "sd3")
				extensionCutSize = 4;
		}

	_textureName.erase(_textureName.length() - extensionCutSize,extensionCutSize);
	_textureFilePath.insert(_textureFilePath.length(),_textureName);
	_textureFilePath.insert(_textureFilePath.length(),"_diffuse.dds");
	_textureFileName = _textureFilePath;
}


/*void m3_Mesh::_CalculateNormals()
{
	float v1[3];
	float v2[3];
	float normal[3];

	for( int i = 0; i < nIndeces; i += 3 )
	{
		if(d_Indeces[i] > nVerteces) 
			continue;

		v1[0] = d_Verteces[d_Indeces[i+1]].x - d_Verteces[d_Indeces[i]].x;
		v1[1] = d_Verteces[d_Indeces[i+1]].y - d_Verteces[d_Indeces[i]].y;
		v1[2] = d_Verteces[d_Indeces[i+1]].z - d_Verteces[d_Indeces[i]].z;

		v2[0] = d_Verteces[d_Indeces[i+2]].x - d_Verteces[d_Indeces[i]].x;
		v2[1] = d_Verteces[d_Indeces[i+2]].y - d_Verteces[d_Indeces[i]].y;
		v2[2] = d_Verteces[d_Indeces[i+2]].z - d_Verteces[d_Indeces[i]].z;

		normal[0] =  v1[1] * v2[2] - v1[2] * v2[1];
		normal[1] =  v2[0] * v1[2] - v2[2] * v2[0];
		normal[2] =  v1[0] * v2[1] - v1[1] * v2[0];

		float vLength = sqrt(pow(normal[0],2) + pow(normal[1],2) + pow(normal[2],2));
		normal[0] /= vLength;
		normal[1] /= vLength;
		normal[2] /= vLength;

		d_Verteces[d_Indeces[i]].normal[0] = normal[0];
		d_Verteces[d_Indeces[i]].normal[1] = normal[1];
		d_Verteces[d_Indeces[i]].normal[2] = normal[2];

		d_Verteces[d_Indeces[i + 1]].normal[0] = normal[0];
		d_Verteces[d_Indeces[i + 1]].normal[1] = normal[1];
		d_Verteces[d_Indeces[i + 1]].normal[2] = normal[2];

		d_Verteces[d_Indeces[i + 2]].normal[0] = normal[0];
		d_Verteces[d_Indeces[i + 2]].normal[1] = normal[1];
		d_Verteces[d_Indeces[i + 2]].normal[2] = normal[2];

		data_normal[d_Indeces[i]].x = normal[0];
		data_normal[d_Indeces[i]].y = normal[1];
		data_normal[d_Indeces[i]].z = normal[2];

		data_normal[d_Indeces[i + 1]].x = normal[0];
		data_normal[d_Indeces[i + 1]].y = normal[1];
		data_normal[d_Indeces[i + 1]].z = normal[2];

		data_normal[d_Indeces[i + 2]].x = normal[0];
		data_normal[d_Indeces[i + 2]].y = normal[1];
		data_normal[d_Indeces[i + 2]].z = normal[2];
	}
}*/

void m3_Mesh::Convert()
{/*
	if(_complexMeshData == NULL) return;

	std::string fileName = "Content\\Export\\";
	fileName += meshOriginalName;

	fileName.erase(fileName.length() - 3,3);
	fileName.insert(fileName.length(),".obj");

	char vertexTag = 'v';
	char indexTag  = 'f';
	char texCoordTag[2] =  { 'v','t' };
	char normalTag[2] = { 'v', 'n' };
	char space = ' ';
	char separator = '/';

	FILE *file = NULL;
	fopen_s(&file,fileName.c_str(),"w");

	for(unsigned int i = 0; i < _complexMeshData->vertexDataPtr->nVerteces; ++i)
		fprintf_s(file, "v %f %f %f\n", _complexMeshData->vertexDataPtr->vertexDataPtr[i].x, _complexMeshData->vertexDataPtr->vertexDataPtr[i].y, _complexMeshData->vertexDataPtr->vertexDataPtr[i].z);

	for(unsigned int i = 0; i < _complexMeshData->vertexDataPtr->nVerteces; ++i)
		fprintf_s(file, "vt %f %f\n", _complexMeshData->vertexDataPtr->textureCoordPtr[i].u, 1 - _complexMeshData->vertexDataPtr->textureCoordPtr[i].v);

	for(unsigned int i = 0; i < _complexMeshData->vertexDataPtr->nVerteces; ++i)
		fprintf_s(file, "vn %f %f %f\n", _complexMeshData->vertexDataPtr->normalDataPtr[i].x,_complexMeshData->vertexDataPtr->normalDataPtr[i].y,_complexMeshData->vertexDataPtr->normalDataPtr[i].z);

	for(unsigned int i = 0; i < _complexMeshData->vertexDataPtr->nIndeces; i+=3 )
		fprintf_s(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", _complexMeshData->vertexDataPtr->indecesPtr[i]+1, _complexMeshData->vertexDataPtr->indecesPtr[i]+1, _complexMeshData->vertexDataPtr->indecesPtr[i]+1,
                                                          _complexMeshData->vertexDataPtr->indecesPtr[i + 1]+1, _complexMeshData->vertexDataPtr->indecesPtr[i + 1]+1, _complexMeshData->vertexDataPtr->indecesPtr[i + 1]+1,
                                                          _complexMeshData->vertexDataPtr->indecesPtr[i + 2]+1, _complexMeshData->vertexDataPtr->indecesPtr[i + 2]+1, _complexMeshData->vertexDataPtr->indecesPtr[i + 2]+1);

	fclose(file);*/
}

void m3_Mesh::Update()
{
	if(!_complexMeshData)
	{
		_complexMeshData = m3_Resource::MeshContainer()->GetMesh(_meshFileName);
		if(_complexMeshData)
		{
			_vertexBufferId = _complexMeshData->vertexDataId;
			_vertexBufferPtr = _complexMeshData->vertexDataPtr;
		}
	}

	if(!_cgShader)
	{
		_cgShader = m3_Resource::ShaderContainer()->GetShader("basic");
		if(_cgShader)
		{
			_cgShaderParams.cg_Texture				   = cgGetNamedParameter(_cgShader->cg_PixelProgram, "colorTexture");
			_cgShaderParams.cg_mWorldViewProjection    = cgGetNamedParameter( _cgShader->cg_PixelProgram, "mWorldViewProjection" );
		    _cgShaderParams.cg_mWorldViewInverse       = cgGetNamedParameter( _cgShader->cg_PixelProgram, "mWorldViewInverse" );
	        _cgShaderParams.cg_vCamera		           = cgGetNamedParameter( _cgShader->cg_PixelProgram, "vCamera" );
	        _cgShaderParams.cg_vLight                  = cgGetNamedParameter( _cgShader->cg_PixelProgram, "vLight");
			_cgShaderParams.cg_bLightFlag			   = cgGetNamedParameter( _cgShader->cg_PixelProgram, "bLightFlag");
		}
	}
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	if( m3_Input::mLeftDown )
	{
		int diffx = m3_Input::mCurrentPosition.x - m3_Input::mLastPosition.x; 
		int diffy = m3_Input::mCurrentPosition.y - m3_Input::mLastPosition.y; 

		m3_Input::mLastPosition.x = m3_Input::mCurrentPosition.x;
		m3_Input::mLastPosition.y = m3_Input::mCurrentPosition.y;

		Rotation.y += (float) diffx/100.0f;
		Rotation.x += (float) diffy/100.0f;
	}
	Matrix4x4 rotX = RotationX(Rotation.x);
	Matrix4x4 rotY = RotationY(Rotation.y);
	Matrix4x4 trans = Translation(Vector3d(0.0f,0.0f,m3_Input::mWheel));
	Matrix4x4 world = rotX*rotY*trans;
	m3_Core::GetCamera()->ApplyViewMatrix();
	Matrix4x4 worldView = world*m3_Core::GetCamera()->mView;
	glLoadMatrixf(worldView.m);
}


void m3_Mesh::RenderBone()
{
	if(!_complexMeshData || !_cgShader) return;
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	static float tFrames = 0;
	static float rFrames = 0;
	float  tSpeed = 0.0f;
	float  rSpeed = 0.0f; 
	float animSpeed = 60.0f;
	glPointSize(2.0f);
  unsigned int i;
	for( i = 0; i < _complexMeshData->vertexDataPtr->nBones; ++i)
	{
		m3_BoneData *parentPtr = &_complexMeshData->vertexDataPtr->boneDataPtr[i];
		Matrix4x4 mBone;
    uint32 seq_index;
		while(parentPtr != NULL)
		{
      int nSeq = 0;
			for( seq_index = 0; seq_index < _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].nTransformationIds; ++seq_index)
			{
				if(parentPtr->translationId.animid == _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].transformationId[seq_index])
				{
					uint16 tId = _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].transformationIndex[seq_index].tranformationDataIndex;
					uint16 tIndex = _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].transformationIndex[seq_index].sequenceDataIndex;
					if(tIndex == 2)
					{
            animSpeed = _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].frequency;
						tSpeed = _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].tSequenceData[tId].nFrames/animSpeed;
						if(tFrames >= _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].tSequenceData[tId].nFrames)
							tFrames = 0;
						parentPtr->_startPosition = _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].tSequenceData[tId].keys[(int)tFrames];
					}
				}
			}

			for( seq_index = 0; seq_index < _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].nTransformationIds; ++seq_index)
			{
				if(parentPtr->rotationId.animid == _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].transformationId[seq_index])
				{
					uint16 tId = _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].transformationIndex[seq_index].tranformationDataIndex;
					uint16 tIndex = _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].transformationIndex[seq_index].sequenceDataIndex;
					if(tIndex == 3)
					{
            animSpeed = _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].frequency;
						rSpeed = _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].rSequenceData[tId].nFrames/animSpeed;
						if(rFrames >= _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].rSequenceData[tId].nFrames)
							rFrames = 0;

						parentPtr->_startRotation[0] = _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].rSequenceData[tId].keys[(int)rFrames].x;
						parentPtr->_startRotation[1] = _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].rSequenceData[tId].keys[(int)rFrames].y;
						parentPtr->_startRotation[2] = _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].rSequenceData[tId].keys[(int)rFrames].z;
						parentPtr->_startRotation[3] = _complexMeshData->vertexDataPtr->_sequenceBlock[nSeq].rSequenceData[tId].keys[(int)rFrames].w;
					}
				}
			}

			Matrix4x4 mRotation = Quaternion(parentPtr->_startRotation);
			Matrix4x4 mTranslation = Translation(parentPtr->_startPosition);
			Matrix4x4 mScale = Scale(parentPtr->_startScale);
			Matrix4x4 mLocalBone = mScale*mRotation*mTranslation;
			mBone = mBone*mLocalBone; 
			parentPtr = parentPtr->parentPtr;
		}

		_complexMeshData->vertexDataPtr->boneDataPtr[i].matrix = mBone;
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		Matrix4x4 mRotationX = RotationX(Rotation.x);
		Matrix4x4 mRotationY = RotationY(Rotation.y);
		Matrix4x4 mTranslation = Translation(Vector3d(0.0f,0.0f,m3_Input::mWheel));
		Matrix4x4 mWorld = mRotationX*mRotationY*mTranslation;
		m3_Core::GetCamera()->ApplyViewMatrix();
		Matrix4x4 worldView = mBone*mWorld*m3_Core::GetCamera()->mView;

		glLoadMatrixf(worldView.m);
		
    if((i==0x0b||i==0x10||i==0x12))
    {
    glBegin(GL_POINTS);
			glColor4f(1.0f,0.5f,1.0f,1.0f);			
			glVertex3f(0.0f, 0.0f, 0.0f);	
		glEnd();
    }

	}

	m3_M3Loader::glBindBufferARB( GL_ARRAY_BUFFER_ARB, _vertexBufferId->vertexId);
	m3_VertexData *vertexDataPtr = (m3_VertexData*)m3_M3Loader::glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB);
	for( i = 0; i < _vertexBufferPtr->nVerteces; ++i)
	{
		uint8 bIndex_01 = _vertexBufferPtr->globalBonesIndecesList[_vertexBufferPtr->boneVertexDataPtr[i].boneIndex[0]];
		uint8 bIndex_02 = _vertexBufferPtr->globalBonesIndecesList[_vertexBufferPtr->boneVertexDataPtr[i].boneIndex[1]];
		uint8 bIndex_03 = _vertexBufferPtr->globalBonesIndecesList[_vertexBufferPtr->boneVertexDataPtr[i].boneIndex[2]];
		uint8 bIndex_04 = _vertexBufferPtr->globalBonesIndecesList[_vertexBufferPtr->boneVertexDataPtr[i].boneIndex[3]];

		float fWeight_01 = _vertexBufferPtr->boneVertexDataPtr[i].boneWeight[0]/255.0f;
		float fWeight_02 = _vertexBufferPtr->boneVertexDataPtr[i].boneWeight[1]/255.0f;
		float fWeight_03 = _vertexBufferPtr->boneVertexDataPtr[i].boneWeight[2]/255.0f;
		float fWeight_04 = _vertexBufferPtr->boneVertexDataPtr[i].boneWeight[3]/255.0f;

		Matrix4x4 *mBoneAnimated_01 = &_complexMeshData->vertexDataPtr->boneDataPtr[bIndex_01].matrix;
		Matrix4x4 *mBoneAnimated_02 = &_complexMeshData->vertexDataPtr->boneDataPtr[bIndex_02].matrix;
		Matrix4x4 *mBoneAnimated_03 = &_complexMeshData->vertexDataPtr->boneDataPtr[bIndex_03].matrix;
		Matrix4x4 *mBoneAnimated_04 = &_complexMeshData->vertexDataPtr->boneDataPtr[bIndex_04].matrix;

		Matrix4x4 *mBoneStatic_01 = &_complexMeshData->vertexDataPtr->staticBonesMatrices[bIndex_01];
		Matrix4x4 *mBoneStatic_02 = &_complexMeshData->vertexDataPtr->staticBonesMatrices[bIndex_02];
		Matrix4x4 *mBoneStatic_03 = &_complexMeshData->vertexDataPtr->staticBonesMatrices[bIndex_03];
		Matrix4x4 *mBoneStatic_04 = &_complexMeshData->vertexDataPtr->staticBonesMatrices[bIndex_04];

		Vector3d vVertexStatic = Vector3d(-_vertexBufferPtr->vertexDataPtr[i].x,_vertexBufferPtr->vertexDataPtr[i].y,_vertexBufferPtr->vertexDataPtr[i].z);

		Vector3d vVertexAnimated = Vector3d(0.0f,0.0f,0.0f);

		vVertexAnimated += ((*mBoneAnimated_01)*((*mBoneStatic_01) * vVertexStatic))*fWeight_01;
		vVertexAnimated += ((*mBoneAnimated_02)*((*mBoneStatic_02) * vVertexStatic))*fWeight_02;
		vVertexAnimated += ((*mBoneAnimated_03)*((*mBoneStatic_03) * vVertexStatic))*fWeight_03;
		vVertexAnimated += ((*mBoneAnimated_04)*((*mBoneStatic_04) * vVertexStatic))*fWeight_04;

		vertexDataPtr[i].x = vVertexAnimated.x; 
		vertexDataPtr[i].y = vVertexAnimated.y;
		vertexDataPtr[i].z = vVertexAnimated.z;
	}
	m3_M3Loader::glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glColor4f(1.0f,1.0f,1.0f,1.0f);	

	tFrames+= tSpeed;
	rFrames+= rSpeed;
}

void m3_Mesh::RenderNormals()
{
	if(!_complexMeshData || !_cgShader) return;
	
	glDisable(GL_TEXTURE_2D);
	
	int normalLengthDiv = 4.0f;
	glBegin(GL_LINES);
	for(unsigned int i = 0; i < _complexMeshData->vertexDataPtr->nVerteces; ++i)
	{
		glColor4f(1.0f,1.0f,1.0f,0.7f);			
		glVertex3f(_complexMeshData->vertexDataPtr->vertexDataPtr[i].x, _complexMeshData->vertexDataPtr->vertexDataPtr[i].y, _complexMeshData->vertexDataPtr->vertexDataPtr[i].z);	
		glColor4f(0.0f,0.0f,1.0f,0.1f);			
		glVertex3f(_complexMeshData->vertexDataPtr->vertexDataPtr[i].x +_complexMeshData->vertexDataPtr->normalDataPtr[i].x/normalLengthDiv, _complexMeshData->vertexDataPtr->vertexDataPtr[i].y  + _complexMeshData->vertexDataPtr->normalDataPtr[i].y/normalLengthDiv, _complexMeshData->vertexDataPtr->vertexDataPtr[i].z + _complexMeshData->vertexDataPtr->normalDataPtr[i].z/normalLengthDiv);	
	}
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f,1.0f,1.0f,1.0f);		
}

void m3_Mesh::Render()
{
	if(!_complexMeshData || !_cgShader) return;

	//cgGLSetTextureParameter( _cgShaderParams.cg_Texture, m3_Resource::TextureContainer()->GetTexture(_textureFileName) );
  glBindTexture(GL_TEXTURE_2D, m3_Resource::TextureContainer()->GetTexture(_textureFileName) );

  /*
	cgGLEnableProfile( _cgShader->cg_VertexProfile );
	cgGLEnableProfile( _cgShader->cg_PixelProfile );
	//cgGLEnableTextureParameter(_cgShaderParams.cg_Texture);

	cgGLSetStateMatrixParameter(_cgShaderParams.cg_mWorldViewProjection,
								CG_GL_MODELVIEW_PROJECTION_MATRIX,
								CG_GL_MATRIX_IDENTITY );

	cgGLSetStateMatrixParameter(_cgShaderParams.cg_mWorldViewInverse,
								CG_GL_MODELVIEW_MATRIX,
								CG_GL_MATRIX_INVERSE_TRANSPOSE );
  */
	float fEyePosition[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static float fLightVector[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static float fAngle = 0.0f;
	fAngle += 0.05f;
	fLightVector[0] = cos(fAngle)*15.0f;
	fLightVector[2] = sin(fAngle)*15.0f;
	fLightVector[1] = 1.0f;

	float fLength = sqrtf( fLightVector[0]*fLightVector[0] +
						   fLightVector[1]*fLightVector[1] +
						   fLightVector[2]*fLightVector[2] );
	fLightVector[0] /= fLength;
	fLightVector[1] /= fLength;
	fLightVector[2] /= fLength;

  /*
	cgGLSetParameter4fv( _cgShaderParams.cg_vCamera, fEyePosition );
	cgGLSetParameter4fv( _cgShaderParams.cg_vLight, fLightVector );

	cgGLSetParameter1f( _cgShaderParams.cg_bLightFlag, false);*/

	m3_M3Loader::glBindBufferARB( GL_ARRAY_BUFFER_ARB, _vertexBufferId->vertexId);
	glVertexPointer( 3, GL_FLOAT, 0,  NULL );	
	m3_M3Loader::glBindBufferARB( GL_ARRAY_BUFFER_ARB, _vertexBufferId->textureCoordId );
	glTexCoordPointer( 2, GL_FLOAT, 0,NULL );	
	m3_M3Loader::glBindBufferARB( GL_ARRAY_BUFFER_ARB, _vertexBufferId->normalId );
	glNormalPointer( GL_FLOAT, 0,NULL );

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	for(uint32 i = 0; i < _vertexBufferPtr->nRegions; ++i)
	{
		glDrawElements( GL_TRIANGLES, _vertexBufferPtr->regionDataPtr[i].nIndices, GL_UNSIGNED_INT, &_vertexBufferPtr->indecesPtr[_vertexBufferPtr->regionDataPtr[i].ofsIndices]);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

  /*
	cgGLDisableTextureParameter( _cgShaderParams.cg_Texture );

	cgGLDisableProfile( _cgShader->cg_VertexProfile );
	cgGLDisableProfile( _cgShader->cg_PixelProfile );*/

	RenderBone();

	if(m3_Input::enableNormal)
		RenderNormals();

}