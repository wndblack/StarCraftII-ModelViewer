#include "m3_M3Loader.h"


#include "vec3d.h"
#include "quaternion.h"
#include "modelexport_m3.h"

  SphereF  m_sphere;

#include <map>

PFNGLGENBUFFERSARBPROC m3_M3Loader::glGenBuffersARB = NULL;
PFNGLBINDBUFFERARBPROC m3_M3Loader::glBindBufferARB = NULL;
PFNGLBUFFERDATAARBPROC m3_M3Loader::glBufferDataARB = NULL;
PFNGLDELETEBUFFERSARBPROC m3_M3Loader::glDeleteBuffersARB = NULL;
PFNGLMAPBUFFERARBPROC     m3_M3Loader::glMapBufferARB = NULL;
PFNGLUNMAPBUFFERARBPROC   m3_M3Loader::glUnmapBufferARB = NULL;


void m3_M3Loader::Initilize()
{
	if(glGenBuffersARB == NULL)
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
	if(glBindBufferARB == NULL)
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
	if(glBufferDataARB == NULL)
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
	if(glDeleteBuffersARB == NULL)
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");
	if(glMapBufferARB == NULL)
		glMapBufferARB = (PFNGLMAPBUFFERARBPROC) wglGetProcAddress("glMapBufferARB");
	if(glUnmapBufferARB == NULL)
		glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC) wglGetProcAddress("glUnmapBufferARB");
}
m3_VertexBufferPtr* m3_M3Loader::LoadMd34FileData(std::string value)
{
	std::ifstream inStream;
	inStream.open(value.c_str(),std::ios::in | std::ios::binary);
	inStream.seekg(0, std::ios::end);
	int fileSize = inStream.tellg();
    inStream.seekg (0, std::ios::beg);
	char *fileData = new char[fileSize];
	inStream.read(fileData,fileSize);
	inStream.close();

  MD34 *header = (MD34*)fileData;
  MODL *mdata = (MODL*)(fileData+sizeof(MD34));
  ReferenceEntry *refEntry = (ReferenceEntry*)(fileData+header->ofsRefs);

	m3_Version _vertexBlockVersion = M3_UNKNOWN;
	m3_VertexBlock_v01 *_vertexBlock_v01;
	m3_VertexBlock_v02 *_vertexBlock_v02;
	m3_VertexBlock_v03 *_vertexBlock_v03;

  uint16 *indexBlock;

  DIV *_indexRegionBlock;
	//m3_RegionBlock *_regionBlock;
  REGN *_regionBlock;

	m3_VertexBufferPtr *vertexBufferPtr = new m3_VertexBufferPtr();
	unsigned int _blockSize = 0;

  //骨骼索引
	m3_TagsCutoffs *_tagsCuttoffs = (m3_TagsCutoffs*)(fileData + refEntry[mdata->mBoneLU.ref].offset);			
	uint16 *_globalBonesIndecesList = (uint16*)(fileData + refEntry[mdata->mBoneLU.ref].offset);
	vertexBufferPtr->globalBonesIndecesList = new uint16[mdata->mBoneLU.nEntries];
	uint32 index;
	for( index = 0; index < mdata->mBoneLU.nEntries; ++index)
		vertexBufferPtr->globalBonesIndecesList[index] = _globalBonesIndecesList[index];

  //骨骼初始
	Matrix4x4* _saticBonesMatrices = (Matrix4x4*)(fileData+refEntry[mdata->mIREF.ref].offset);
	vertexBufferPtr->staticBonesMatrices = new Matrix4x4[refEntry[mdata->mIREF.ref].nEntries];
	for( index = 0; index < refEntry[mdata->mIREF.ref].nEntries; ++index)
		vertexBufferPtr->staticBonesMatrices[index] = _saticBonesMatrices[index];

  //骨骼变换
  char mname[512];
  SEQS34 *_seq = (SEQS34*)(fileData + refEntry[mdata->mSEQS.ref].offset);
  STG *_stg = (STG*)(fileData + refEntry[mdata->mSTG.ref].offset);

  std::map<uint32,std::string> _mapstg;
  for(index = 0;index<mdata->mSTG.nEntries;index++)
  {
    std::string str;
    uint32 id;
    str.resize(_stg[index].name.nEntries);
    //memcpy(&str,fileData + refEntry[_stg[index].name.ref].offset,_stg[index].name.nEntries);
    str = fileData + refEntry[_stg[index].name.ref].offset;
    memcpy(&id,fileData + refEntry[_stg[index].stcID.ref].offset,sizeof(uint32));
    sprintf(mname,"%s  %d\r\n",str.c_str(),id);
    OutputDebugString(mname);
    _mapstg.insert(std::pair<uint32,std::string>(id,str));
  }

  STC* _seqData = (STC*)(fileData + refEntry[mdata->mSTC.ref].offset);
  vertexBufferPtr->_sequenceBlock.resize(mdata->mSTC.nEntries);
  for(uint32 nSeq = 0; nSeq<mdata->mSTC.nEntries;nSeq++)
  {
	  // Transform Id's.
	  uint32 *tranfomationId = (uint32*)(fileData + refEntry[_seqData[nSeq].animid.ref].offset);
	  vertexBufferPtr->_sequenceBlock[nSeq].nTransformationIds = _seqData[nSeq].animid.nEntries;
	  vertexBufferPtr->_sequenceBlock[nSeq].transformationId = new uint32[vertexBufferPtr->_sequenceBlock[nSeq].nTransformationIds];
    sprintf(mname,"%d : \r\n",nSeq);
    OutputDebugString(mname);
    for( index = 0; index < vertexBufferPtr->_sequenceBlock[nSeq].nTransformationIds; ++index)
		{
      vertexBufferPtr->_sequenceBlock[nSeq].transformationId[index] = tranfomationId[index];
      sprintf(mname,"0x%08x               ",tranfomationId[index]);
      OutputDebugString(mname);
    }
    OutputDebugString("\r\n");

	  // Transform Indece's.
	  _m3_TransformationIndex *transformtionIndex = (_m3_TransformationIndex*)(fileData + refEntry[_seqData[nSeq].animindex.ref].offset);
	  vertexBufferPtr->_sequenceBlock[nSeq].nTransformationIndeces = _seqData[nSeq].animindex.nEntries;
	  vertexBufferPtr->_sequenceBlock[nSeq].transformationIndex = new _m3_TransformationIndex[vertexBufferPtr->_sequenceBlock[nSeq].nTransformationIndeces];
	  for( index = 0; index < vertexBufferPtr->_sequenceBlock[nSeq].nTransformationIndeces; ++index)
		{
      vertexBufferPtr->_sequenceBlock[nSeq].transformationIndex[index] = transformtionIndex[index];
      sprintf(mname,"0x%08x-0x%08x    ",transformtionIndex[index].tranformationDataIndex,transformtionIndex[index].sequenceDataIndex);
      OutputDebugString(mname);
    }
    OutputDebugString("\r\n");
	  // Sequence Data's.
    std::map<uint32,std::string>::iterator it = _mapstg.find(nSeq);
    if(it != _mapstg.end())
    {
      for(uint32 in = 0;in<mdata->mSTG.nEntries;in++)
      {
        if(strcmp(it->second.c_str(),fileData + refEntry[_seq[in].name.ref].offset) == 0)
        {
          vertexBufferPtr->_sequenceBlock[nSeq].frequency = _seq[in].frequency;
          break;
        }
      }
    }
    else
    {
      vertexBufferPtr->_sequenceBlock[nSeq].frequency = 60;
    }
    vertexBufferPtr->_sequenceBlock[nSeq].name = new char[refEntry[_seqData[nSeq].name.ref].nEntries];
    strcpy(vertexBufferPtr->_sequenceBlock[nSeq].name,fileData + refEntry[_seqData[nSeq].name.ref].offset);

	  // Translation data's.
	  SD *translationDataPtr = (SD*)(fileData + refEntry[_seqData[nSeq].arVec3D.ref].offset);
	  vertexBufferPtr->_sequenceBlock[nSeq].nTSequenceData = _seqData[nSeq].arVec3D.nEntries;
	  vertexBufferPtr->_sequenceBlock[nSeq].tSequenceData = new _m3_TranslationDataPtr[vertexBufferPtr->_sequenceBlock[nSeq].nTSequenceData];
	  for( index = 0; index < vertexBufferPtr->_sequenceBlock[nSeq].nTSequenceData; ++index)
	  {
		  Vector3d *vTranslationData = (Vector3d*)(fileData + refEntry[translationDataPtr[index].data.ref].offset);
      int32 *iTranslationTime = (int32*)(fileData + refEntry[translationDataPtr[index].timeline.ref].offset);
		  vertexBufferPtr->_sequenceBlock[nSeq].tSequenceData[index].nFrames = translationDataPtr[index].data.nEntries;
		  vertexBufferPtr->_sequenceBlock[nSeq].tSequenceData[index].keys = new Vector3d[vertexBufferPtr->_sequenceBlock[nSeq].tSequenceData[index].nFrames];
		  vertexBufferPtr->_sequenceBlock[nSeq].tSequenceData[index].timelines = new int32[vertexBufferPtr->_sequenceBlock[nSeq].tSequenceData[index].nFrames];
		  for(uint32 index_02 = 0; index_02 < vertexBufferPtr->_sequenceBlock[nSeq].tSequenceData[index].nFrames; ++index_02)
      {
        vertexBufferPtr->_sequenceBlock[nSeq].tSequenceData[index].keys[index_02] = vTranslationData[index_02];
			  vertexBufferPtr->_sequenceBlock[nSeq].tSequenceData[index].timelines[index_02] = iTranslationTime[index_02];
      }
	  }

	  // Rotation data's.
	  SD *rotationDataPtr = (SD*)(fileData + refEntry[_seqData[nSeq].arQuat.ref].offset);
	  vertexBufferPtr->_sequenceBlock[nSeq].nRSequenceData = _seqData[nSeq].arQuat.nEntries;
	  vertexBufferPtr->_sequenceBlock[nSeq].rSequenceData = new _m3_RotationDataPtr[vertexBufferPtr->_sequenceBlock[nSeq].nRSequenceData];
	  for( index = 0; index < vertexBufferPtr->_sequenceBlock[nSeq].nRSequenceData; ++index)
	  {
		  QuaternionStruct *qRotationData = (QuaternionStruct*)(fileData + refEntry[rotationDataPtr[index].data.ref].offset);
      int32 *iTranslationTime = (int32*)(fileData + refEntry[rotationDataPtr[index].timeline.ref].offset);
		  vertexBufferPtr->_sequenceBlock[nSeq].rSequenceData[index].nFrames = rotationDataPtr[index].data.nEntries;
		  vertexBufferPtr->_sequenceBlock[nSeq].rSequenceData[index].keys = new QuaternionStruct[vertexBufferPtr->_sequenceBlock[nSeq].rSequenceData[index].nFrames];
		  vertexBufferPtr->_sequenceBlock[nSeq].rSequenceData[index].timelines = new int32[vertexBufferPtr->_sequenceBlock[nSeq].rSequenceData[index].nFrames];
		  for(uint32 index_02 = 0; index_02 < vertexBufferPtr->_sequenceBlock[nSeq].rSequenceData[index].nFrames; ++index_02)
      {
        vertexBufferPtr->_sequenceBlock[nSeq].rSequenceData[index].keys[index_02] = qRotationData[index_02];
			  vertexBufferPtr->_sequenceBlock[nSeq].rSequenceData[index].timelines[index_02] = iTranslationTime[index_02];
      }
	  }
  }
  uint32 aaa = vertexBufferPtr->_sequenceBlock[4].frequency;
  aaa = vertexBufferPtr->_sequenceBlock[0].frequency;
  aaa = vertexBufferPtr->_sequenceBlock[8].frequency;
  MAT *Mat = (MAT*)(fileData + refEntry[mdata->mMat.ref].offset);
  LAYR34 *Lay = (LAYR34*)(fileData + refEntry[Mat->layers[0].ref].offset);
  for(int a=0;a<mdata->mMat.nEntries;a++)
  {
    sprintf(mname,"%d material   ",a);
    OutputDebugString(mname);
    memcpy(mname,fileData + refEntry[Mat[a].name.ref].offset,Mat[a].name.nEntries);
    mname[Mat[a].name.nEntries] =0;
    OutputDebugString(mname);
    sprintf(mname,"  flags: 0x%02x blendmode: 0x%02x",Mat[a].flags,Mat[a].blendMode);
    OutputDebugString(mname);
    OutputDebugString("\r\n");
    for(int b=0;b<13;b++)
    {
      Lay = (LAYR34*)(fileData + refEntry[Mat[a].layers[b].ref].offset);
      memcpy(mname,fileData + refEntry[Lay[0].name.ref].offset,Lay[0].name.nEntries);
      mname[Lay[0].name.nEntries] =0;
      OutputDebugString("    ");
      OutputDebugString(mname);
      sprintf(mname,"   0x%02x",Lay[0].flags);
      OutputDebugString(mname);
      OutputDebugString("\r\n");
    } 
  }
  m_sphere.min = mdata->boundSphere.min;
  m_sphere.max = mdata->boundSphere.max;
  m_sphere.radius = mdata->boundSphere.radius;
  PAR *par =(PAR*)(fileData + refEntry[mdata->mPar.ref].offset);

  STS *sts = (STS*)(fileData + refEntry[mdata->mSTS.ref].offset);
  STG *stg = (STG*)(fileData + refEntry[mdata->mSTG.ref].offset);
  uint32 i;
  for( i=0;i<mdata->mSTS.nEntries;i++)
  {
    uint32 *tranfomationId = (uint32*)(fileData + refEntry[sts[i].animid.ref].offset);
    OutputDebugString("sts \r\n");
    for(int j=0;j<sts[i].animid.nEntries;j++)
    {
      sprintf(mname,"0x%08x               ",tranfomationId[j]);
      OutputDebugString(mname);
    }
    OutputDebugString(" \r\n");
  }

  //骨骼
	BONE34* _boneblock = (BONE34*)(fileData + refEntry[mdata->mBone.ref].offset);
	vertexBufferPtr->nBones = mdata->mBone.nEntries;
	vertexBufferPtr->boneDataPtr = new m3_BoneData[vertexBufferPtr->nBones];

  for(  i = 0; i < vertexBufferPtr->nBones; ++i)
	{
		vertexBufferPtr->boneDataPtr[i].rotationId = _boneblock[i].initRot.AnimRef;
		vertexBufferPtr->boneDataPtr[i].translationId = _boneblock[i].initTrans.AnimRef;
    sprintf(mname,"0x%02x Bone rid: 0x%08x   tid: 0x%08x\r\n",i,_boneblock[i].initRot.AnimRef.animid,_boneblock[i].initTrans.AnimRef.animid);
    OutputDebugString(mname);

		memcpy(&vertexBufferPtr->boneDataPtr[i]._startPosition ,&_boneblock[i].initTrans.value,sizeof(Vector3d));
		memcpy(&vertexBufferPtr->boneDataPtr[i]._startRotation ,&_boneblock[i].initRot.value,sizeof(Vec4D));
		memcpy(&vertexBufferPtr->boneDataPtr[i]._startScale    ,&_boneblock[i].initScale.value,sizeof(Vector3d));

		vertexBufferPtr->boneDataPtr[i].parent = _boneblock[i].parent;

		vertexBufferPtr->boneDataPtr[i].parentPtr = NULL;
	}

	//for( uint32 j = 0; j < vertexBufferPtr->nBones; ++j)
	for(  i = 0; i < vertexBufferPtr->nBones; ++i)
	{
		if(vertexBufferPtr->boneDataPtr[i].parent != -1)
		{
			vertexBufferPtr->boneDataPtr[i].parentPtr = &vertexBufferPtr->boneDataPtr[vertexBufferPtr->boneDataPtr[i].parent];
		}
	}


	//if(_tagNameStr == "_VID")
	{
		_indexRegionBlock = (DIV*)(fileData + refEntry[mdata->mDIV.ref].offset);
		vertexBufferPtr->nIndeces = _indexRegionBlock->faces.nEntries;
		indexBlock = (uint16*)(fileData + refEntry[_indexRegionBlock->faces.ref].offset);
		_regionBlock = (REGN *)(fileData + refEntry[_indexRegionBlock->REGN.ref].offset);
		
		uint16 *bonelookup =  (uint16*)(fileData + refEntry[mdata->mDIV.ref + 1].offset);
    BAT * pbat = (BAT*)(fileData + refEntry[_indexRegionBlock->BAT.ref].offset);
    MATM *pmatm =(MATM*)(fileData + refEntry[mdata->mMatLU.ref].offset);
    //pmatm[pbat[regn]]
		int c = 0;
	}
	//if(_tagNameStr == "__8U")
	{
		_blockSize = refEntry[mdata->mVert.ref].nEntries % sizeof(m3_VertexBlock_v01);
		_vertexBlock_v01 = (m3_VertexBlock_v01*)(fileData + refEntry[mdata->mVert.ref].offset);
		if((_blockSize == 0) && (_vertexBlock_v01[0].tangent[3] == 255))
		{
			vertexBufferPtr->nVerteces = (refEntry[mdata->mVert.ref].nEntries)/sizeof(m3_VertexBlock_v01);
			  _vertexBlock_v01 = (m3_VertexBlock_v01*)(fileData + refEntry[mdata->mVert.ref].offset);
			_vertexBlockVersion = M3_01;
		}

		_blockSize = refEntry[mdata->mVert.ref].nEntries % sizeof(m3_VertexBlock_v02);
		_vertexBlock_v02 = (m3_VertexBlock_v02*)(fileData + refEntry[mdata->mVert.ref].offset);
		if((_blockSize == 0) && (_vertexBlock_v02[0].tangent[3] == 255))
		{
			vertexBufferPtr->nVerteces = (refEntry[mdata->mVert.ref].nEntries)/sizeof(m3_VertexBlock_v02);
			  _vertexBlock_v02 = (m3_VertexBlock_v02*)(fileData + refEntry[mdata->mVert.ref].offset);
			_vertexBlockVersion = M3_02;
		}

		_blockSize = refEntry[mdata->mVert.ref].nEntries % sizeof(m3_VertexBlock_v03);
		if(_blockSize == 0 && _vertexBlockVersion != M3_02)
		{
			vertexBufferPtr->nVerteces = (refEntry[mdata->mVert.ref].nEntries)/sizeof(m3_VertexBlock_v03);
			  _vertexBlock_v03 = (m3_VertexBlock_v03*)(fileData + refEntry[mdata->mVert.ref].offset);
			_vertexBlockVersion = M3_03;
		}
	}


	vertexBufferPtr->vertexDataPtr = new m3_VertexData[vertexBufferPtr->nVerteces];
	vertexBufferPtr->normalDataPtr = new m3_NormalData[vertexBufferPtr->nVerteces];
	vertexBufferPtr->textureCoordPtr = new m3_TextureCoordData[vertexBufferPtr->nVerteces];
  vertexBufferPtr->decaltextureCoordPtr = new m3_TextureCoordData[vertexBufferPtr->nVerteces];
	vertexBufferPtr->boneVertexDataPtr = new m3_BoneVertexData[vertexBufferPtr->nVerteces];

	float wNormal = 0.0f; 


  Vector3d UVector = Vector3d(1, 0, 0)/(2 * m_sphere.radius);//将世界坐标变换到纹理投影空间坐标并规范化到0-1之间（正投影） 
  Vector3d VVector = Vector3d(0, 0, 1)/(-2 * m_sphere.radius);//同上 
  Vec3D centerV = (m_sphere.max+m_sphere.min)/2;
  Vector3d center = Vector3d(centerV.x,centerV.y,centerV.z);

	for( i = 0; i < vertexBufferPtr->nVerteces; ++i)
	{
		switch(_vertexBlockVersion)
		{
			case M3_01:
				vertexBufferPtr->vertexDataPtr[i].x = _vertexBlock_v01[i].position[0];
				vertexBufferPtr->vertexDataPtr[i].y = _vertexBlock_v01[i].position[1];
				vertexBufferPtr->vertexDataPtr[i].z = _vertexBlock_v01[i].position[2];

				vertexBufferPtr->normalDataPtr[i].x = ((float)2*_vertexBlock_v01[i].normal[0]/255.0f - 1.0f);
				vertexBufferPtr->normalDataPtr[i].y = ((float)2*_vertexBlock_v01[i].normal[1]/255.0f - 1.0f);
				vertexBufferPtr->normalDataPtr[i].z = ((float)2*_vertexBlock_v01[i].normal[2]/255.0f - 1.0f);
				wNormal = (float) _vertexBlock_v01[i].normal[3]/255.0f;
				if(wNormal)
				{
					vertexBufferPtr->normalDataPtr[i].x /= wNormal;
					vertexBufferPtr->normalDataPtr[i].y /= wNormal; 
					vertexBufferPtr->normalDataPtr[i].z /= wNormal;
				}
				vertexBufferPtr->textureCoordPtr[i].u = (float)_vertexBlock_v01[i].uv[0]/2048.0f;
				vertexBufferPtr->textureCoordPtr[i].v = (float)_vertexBlock_v01[i].uv[1]/2048.0f;

        
				vertexBufferPtr->decaltextureCoordPtr[i].u = 
          (float)DotProduct(Vector3d(vertexBufferPtr->vertexDataPtr[i].x,vertexBufferPtr->vertexDataPtr[i].y,vertexBufferPtr->vertexDataPtr[i].z)-center, UVector) + 0.5;
        if(vertexBufferPtr->decaltextureCoordPtr[i].u>1||vertexBufferPtr->decaltextureCoordPtr[i].u<0)
          vertexBufferPtr->decaltextureCoordPtr[i].u = 0;
				vertexBufferPtr->decaltextureCoordPtr[i].v = 
          (float)DotProduct(Vector3d(vertexBufferPtr->vertexDataPtr[i].x,vertexBufferPtr->vertexDataPtr[i].y,vertexBufferPtr->vertexDataPtr[i].z)-center, VVector) + 0.5;
        if(vertexBufferPtr->decaltextureCoordPtr[i].v>1||vertexBufferPtr->decaltextureCoordPtr[i].v<0)
          vertexBufferPtr->decaltextureCoordPtr[i].v = 0;


				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[0] = _vertexBlock_v01[i].boneIndex[0];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[1] = _vertexBlock_v01[i].boneIndex[1];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[2] = _vertexBlock_v01[i].boneIndex[2];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[3] = _vertexBlock_v01[i].boneIndex[3];

				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[0] = _vertexBlock_v01[i].boneWeight[0];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[1] = _vertexBlock_v01[i].boneWeight[1];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[2] = _vertexBlock_v01[i].boneWeight[2];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[3] = _vertexBlock_v01[i].boneWeight[3];

				//printf("%i     ,",_vertexBlock_v03[i].boneWeight[0]);

			break;
			case M3_02:
				vertexBufferPtr->vertexDataPtr[i].x = _vertexBlock_v02[i].position[0];
				vertexBufferPtr->vertexDataPtr[i].y = _vertexBlock_v02[i].position[1];
				vertexBufferPtr->vertexDataPtr[i].z = _vertexBlock_v02[i].position[2];

				vertexBufferPtr->normalDataPtr[i].x = ((float)2*_vertexBlock_v02[i].normal[0]/255.0f - 1.0f);
				vertexBufferPtr->normalDataPtr[i].y = ((float)2*_vertexBlock_v02[i].normal[1]/255.0f - 1.0f);
				vertexBufferPtr->normalDataPtr[i].z = ((float)2*_vertexBlock_v02[i].normal[2]/255.0f - 1.0f);
				wNormal = (float) _vertexBlock_v02[i].normal[3]/255.0f;
				if(wNormal)
				{
					vertexBufferPtr->normalDataPtr[i].x /= wNormal;
					vertexBufferPtr->normalDataPtr[i].y /= wNormal; 
					vertexBufferPtr->normalDataPtr[i].z /= wNormal;
				}
				vertexBufferPtr->textureCoordPtr[i].u = (float)_vertexBlock_v02[i].uv[0]/2048.0f;
				vertexBufferPtr->textureCoordPtr[i].v = (float)_vertexBlock_v02[i].uv[1]/2048.0f;

				//if(_vertexBlock_v02[i].boneIndex[0] >= 12)
				//	int aaa = 0;
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[0] = _vertexBlock_v02[i].boneIndex[0];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[1] = _vertexBlock_v02[i].boneIndex[1];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[2] = _vertexBlock_v02[i].boneIndex[2];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[3] = _vertexBlock_v02[i].boneIndex[3];

				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[0] = _vertexBlock_v02[i].boneWeight[0];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[1] = _vertexBlock_v02[i].boneWeight[1];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[2] = _vertexBlock_v02[i].boneWeight[2];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[3] = _vertexBlock_v02[i].boneWeight[3];

				//printf("%i     ,",_vertexBlock_v01[i].boneWeight[0]);

			break;
			case M3_03:
				vertexBufferPtr->vertexDataPtr[i].x = _vertexBlock_v03[i].position[0];
				vertexBufferPtr->vertexDataPtr[i].y = _vertexBlock_v03[i].position[1];
				vertexBufferPtr->vertexDataPtr[i].z = _vertexBlock_v03[i].position[2];

				vertexBufferPtr->normalDataPtr[i].x = ((float)2*_vertexBlock_v03[i].normal[0]/255.0f - 1.0f);
				vertexBufferPtr->normalDataPtr[i].y = ((float)2*_vertexBlock_v03[i].normal[1]/255.0f - 1.0f);
				vertexBufferPtr->normalDataPtr[i].z = ((float)2*_vertexBlock_v03[i].normal[2]/255.0f - 1.0f);
				wNormal = (float) _vertexBlock_v03[i].normal[3]/255.0f;
				if(wNormal)
				{
					vertexBufferPtr->normalDataPtr[i].x /= wNormal;
					vertexBufferPtr->normalDataPtr[i].y /= wNormal; 
					vertexBufferPtr->normalDataPtr[i].z /= wNormal;
				}
				vertexBufferPtr->textureCoordPtr[i].u = (float)_vertexBlock_v03[i].uv[0]/2048.0f;
				vertexBufferPtr->textureCoordPtr[i].v = (float)_vertexBlock_v03[i].uv[1]/2048.0f;

				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[0] = _vertexBlock_v03[i].boneIndex[0];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[1] = _vertexBlock_v03[i].boneIndex[1];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[2] = _vertexBlock_v03[i].boneIndex[2];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[3] = _vertexBlock_v03[i].boneIndex[3];

				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[0] = _vertexBlock_v03[i].boneWeight[0];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[1] = _vertexBlock_v03[i].boneWeight[1];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[2] = _vertexBlock_v03[i].boneWeight[2];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[3] = _vertexBlock_v03[i].boneWeight[3];

				//printf("%i     ,",_vertexBlock_v03[i].boneWeight[0]);
			break;
		}
	}

	vertexBufferPtr->indecesPtr = new unsigned int[vertexBufferPtr->nIndeces];

	for( i = 0; i < vertexBufferPtr->nIndeces; ++i)
		vertexBufferPtr->indecesPtr[i] = indexBlock[i];

	vertexBufferPtr->nRegions = _indexRegionBlock->REGN.nEntries;
	vertexBufferPtr->regionDataPtr = new m3_RegionData[vertexBufferPtr->nRegions];

	for( i = 0; i < vertexBufferPtr->nRegions; ++i)
	{
		vertexBufferPtr->regionDataPtr[i].nIndices = _regionBlock[i].numFaces;
		vertexBufferPtr->regionDataPtr[i].nVertices = _regionBlock[i].numVert;
		vertexBufferPtr->regionDataPtr[i].ofsIndices = _regionBlock[i].indFaces;
		vertexBufferPtr->regionDataPtr[i].ofsVertices = _regionBlock[i].indVert;
	}

  return vertexBufferPtr;
}
m3_VertexBufferPtr* m3_M3Loader::LoadFileData(std::string value)
{
	std::ifstream inStream;
	inStream.open(value.c_str(),std::ios::in | std::ios::binary);
	inStream.seekg(0, std::ios::end);
	int fileSize = inStream.tellg();
    inStream.seekg (0, std::ios::beg);
	char *fileData = new char[fileSize];
	inStream.read(fileData,fileSize);
	inStream.close();

	m3_Header *header = (m3_Header*)fileData;
	m3_Tag *tags =(m3_Tag*)(fileData + header->tagsOffset);

	m3_Version _vertexBlockVersion = M3_UNKNOWN;
	m3_VertexBlock_v01 *_vertexBlock_v01;
	m3_VertexBlock_v02 *_vertexBlock_v02;
	m3_VertexBlock_v03 *_vertexBlock_v03;

	uint16 *indexBlock;

	m3_IndexRegionBlock *_indexRegionBlock;
	m3_RegionBlock *_regionBlock;

	m3_VertexBufferPtr *vertexBufferPtr = new m3_VertexBufferPtr();
	

	bool firstIndexBlockFlag = false;
	unsigned int _blockSize = 0;
	std::string _tagNameStr = "none";

	//int a = sizeof(BONE);
	//int b = sizeof(BONE_02);

	//int c = sizeof(m3_TagReference);

  unsigned int i;
	for( i = 0; i < header->nTags; ++i)
	{
		_tagNameStr = tags[i].tagName;
		_tagNameStr.resize(4);

		if(_tagNameStr == "LDOM") // Read global indeces for bones.
		{
			m3_TagsCutoffs *_tagsCuttoffs = (m3_TagsCutoffs*)(fileData + tags[i].blockOffset);			
			uint16 *_globalBonesIndecesList = (uint16*)(fileData + tags[_tagsCuttoffs->globalBonesIndecesList.reference].blockOffset);
			vertexBufferPtr->globalBonesIndecesList = new uint16[_tagsCuttoffs->globalBonesIndecesList.nEntries];
			for(uint32 index = 0; index < _tagsCuttoffs->globalBonesIndecesList.nEntries; ++index)
				vertexBufferPtr->globalBonesIndecesList[index] = _globalBonesIndecesList[index];
		}

		if(_tagNameStr == "FERI") // Read static matrix transform for bones.
		{
			Matrix4x4* _saticBonesMatrices = (Matrix4x4*)(fileData + tags[i].blockOffset);
			vertexBufferPtr->staticBonesMatrices = new Matrix4x4[tags[i].blockSize];
			for(uint32 index = 0; index < tags[i].blockSize; ++index)
				vertexBufferPtr->staticBonesMatrices[index] = _saticBonesMatrices[index];
		}

		if(_tagNameStr == "_GTS") // Read animation sequence names.
		{
			m3_TagReference *_animationSequencesNames = (m3_TagReference*)(fileData + tags[i].blockOffset);
			m3_string *_names = (m3_string*)(fileData + tags[_animationSequencesNames->reference].blockOffset);
			//vertexBufferPtr->animationSequencesNames = new std::string[tags[i].blockSize];
			//for(uint32 index = 0; index < tags[i].blockSize; ++index)
			//{		
			//	vertexBufferPtr->animationSequencesNames[index] = _names[index];
			//}
			

			int a = 0;
			//m3_Tags2 *ref = (m3_Tags2*)(fileData + tags[i].blockOffset);
			//uint8 *temp = (uint8*)(fileData + tags[ref[0].a.reference].blockOffset);
			//uint32 *temp2 = (uint32*)(fileData + tags[ref[0].b.reference].blockOffset);
		}

		if(_tagNameStr == "_CTS")
		{
			SeqData* _seqData = (SeqData*)(fileData + tags[i].blockOffset);

      vertexBufferPtr->_sequenceBlock.resize(tags[i].blockSize);
      for(uint32 nSeq = 0; nSeq<tags[i].blockSize;nSeq++)
      {

			  // Transform Id's.
			  uint32 *tranfomationId = (uint32*)(fileData + tags[_seqData[nSeq].animid.reference].blockOffset);
			  vertexBufferPtr->_sequenceBlock[nSeq].nTransformationIds = _seqData[nSeq].animid.nEntries;
			  vertexBufferPtr->_sequenceBlock[nSeq].transformationId = new uint32[vertexBufferPtr->_sequenceBlock[nSeq].nTransformationIds];
			  uint32 index;
        for( index = 0; index < vertexBufferPtr->_sequenceBlock[nSeq].nTransformationIds; ++index)
				  vertexBufferPtr->_sequenceBlock[nSeq].transformationId[index] = tranfomationId[index];

			  // Transform Indece's.
			  _m3_TransformationIndex *transformtionIndex = (_m3_TransformationIndex*)(fileData + tags[_seqData[nSeq].animindex.reference].blockOffset);
			  vertexBufferPtr->_sequenceBlock[nSeq].nTransformationIndeces = _seqData[nSeq].animindex.nEntries;
			  vertexBufferPtr->_sequenceBlock[nSeq].transformationIndex = new _m3_TransformationIndex[vertexBufferPtr->_sequenceBlock[nSeq].nTransformationIndeces];
			  for( index = 0; index < vertexBufferPtr->_sequenceBlock[nSeq].nTransformationIndeces; ++index)
				  vertexBufferPtr->_sequenceBlock[nSeq].transformationIndex[index] = transformtionIndex[index];

			  // Sequence Data's.

				  // Translation data's.
			  m3_AnimationData *translationDataPtr = (m3_AnimationData*)(fileData + tags[_seqData[nSeq]._seqData[2].reference].blockOffset);
			  vertexBufferPtr->_sequenceBlock[nSeq].nTSequenceData = _seqData[nSeq]._seqData[2].nEntries;
			  vertexBufferPtr->_sequenceBlock[nSeq].tSequenceData = new _m3_TranslationDataPtr[vertexBufferPtr->_sequenceBlock[nSeq].nTSequenceData];
			  for( index = 0; index < vertexBufferPtr->_sequenceBlock[nSeq].nTSequenceData; ++index)
			  {
				  Vector3d *vTranslationData = (Vector3d*)(fileData + tags[translationDataPtr[index].keys.reference].blockOffset);
				  vertexBufferPtr->_sequenceBlock[nSeq].tSequenceData[index].nFrames = translationDataPtr[index].frames.nEntries;
				  vertexBufferPtr->_sequenceBlock[nSeq].tSequenceData[index].keys = new Vector3d[vertexBufferPtr->_sequenceBlock[nSeq].tSequenceData[index].nFrames];
				  for(uint32 index_02 = 0; index_02 < vertexBufferPtr->_sequenceBlock[nSeq].tSequenceData[index].nFrames; ++index_02)
					  vertexBufferPtr->_sequenceBlock[nSeq].tSequenceData[index].keys[index_02] = vTranslationData[index_02];
			  }
				  // Rotation data's.
			  m3_AnimationData *rotationDataPtr = (m3_AnimationData*)(fileData + tags[_seqData[nSeq]._seqData[3].reference].blockOffset);
			  vertexBufferPtr->_sequenceBlock[nSeq].nRSequenceData = _seqData[nSeq]._seqData[3].nEntries;
			  vertexBufferPtr->_sequenceBlock[nSeq].rSequenceData = new _m3_RotationDataPtr[vertexBufferPtr->_sequenceBlock[nSeq].nRSequenceData];
			  for( index = 0; index < vertexBufferPtr->_sequenceBlock[nSeq].nRSequenceData; ++index)
			  {
				  QuaternionStruct *qRotationData = (QuaternionStruct*)(fileData + tags[rotationDataPtr[index].keys.reference].blockOffset);
				  vertexBufferPtr->_sequenceBlock[nSeq].rSequenceData[index].nFrames = rotationDataPtr[index].frames.nEntries;
				  vertexBufferPtr->_sequenceBlock[nSeq].rSequenceData[index].keys = new QuaternionStruct[vertexBufferPtr->_sequenceBlock[nSeq].rSequenceData[index].nFrames];
				  for(uint32 index_02 = 0; index_02 < vertexBufferPtr->_sequenceBlock[nSeq].rSequenceData[index].nFrames; ++index_02)
					  vertexBufferPtr->_sequenceBlock[nSeq].rSequenceData[index].keys[index_02] = qRotationData[index_02];
			  }
      }
		}

		if(_tagNameStr == "ENOB")
		{
			BONE* _boneblock = (BONE*)(fileData + tags[i].blockOffset);
			vertexBufferPtr->nBones = tags[i].blockSize;
			vertexBufferPtr->boneDataPtr = new m3_BoneData[vertexBufferPtr->nBones];
      uint32 i;
			for(  i = 0; i < vertexBufferPtr->nBones; ++i)
			{
				vertexBufferPtr->boneDataPtr[i].rotationId = _boneblock[i].rotationId;
				vertexBufferPtr->boneDataPtr[i].translationId = _boneblock[i].translationId;

				vertexBufferPtr->boneDataPtr[i]._startPosition = _boneblock[i].position;
				vertexBufferPtr->boneDataPtr[i]._startRotation[0] = _boneblock[i].rotation[0];
				vertexBufferPtr->boneDataPtr[i]._startRotation[1] = _boneblock[i].rotation[1];
				vertexBufferPtr->boneDataPtr[i]._startRotation[2] = _boneblock[i].rotation[2];
				vertexBufferPtr->boneDataPtr[i]._startRotation[3] = _boneblock[i].rotation[3];
				vertexBufferPtr->boneDataPtr[i]._startScale = _boneblock[i].scale;

				vertexBufferPtr->boneDataPtr[i].parent = _boneblock[i].parent;

				vertexBufferPtr->boneDataPtr[i].parentPtr = NULL;
			}
		
			//for( uint32 j = 0; j < vertexBufferPtr->nBones; ++j)
			for(  i = 0; i < vertexBufferPtr->nBones; ++i)
			{
				if(vertexBufferPtr->boneDataPtr[i].parent != -1)
				{
					vertexBufferPtr->boneDataPtr[i].parentPtr = &vertexBufferPtr->boneDataPtr[vertexBufferPtr->boneDataPtr[i].parent];
					//vertexBufferPtr->boneDataPtr[i]._startPosition += vertexBufferPtr->boneDataPtr[vertexBufferPtr->boneDataPtr[i].parent]._startPosition;
				}
			}
			//----int test01 = 0;
		}

		if(_tagNameStr == "Q4DS")
		{
			//----m3_AnimationData* _animblock = (m3_AnimationData*)(fileData + tags[i].blockOffset);
			//----int test01 = 0;
		}
		
		if(_tagNameStr == "V3DS")
		{
			//----m3_AnimationData* _animblock = (m3_AnimationData*)(fileData + tags[i].blockOffset);
			//----Vector3d *_vec = (Vector3d*)(fileData + tags[_animblock->keys.reference].blockOffset);
			//----int test01 = 0;
		}

		if(_tagNameStr == "3RDS")
		{
			//----m3_AnimationData* _animblock = (m3_AnimationData*)(fileData + tags[i].blockOffset);
			//----int test01 = 0;
		}

		if(_tagNameStr == "SQES")
		{
			//----SEQS* seqs = (SEQS*)(fileData + tags[i].blockOffset);
			//----int test01 = 0;
		}
		
		if(_tagNameStr == "RYAL")
		{
			//----LAYR* layr = (LAYR*)(fileData + tags[i].blockOffset);
			//----std::string _textureInFileName = (std::string)(fileData + tags[layr->name.reference].blockOffset);
		}

		if(_tagNameStr == "_VID")
		{
			_indexRegionBlock = (m3_IndexRegionBlock*)(fileData + tags[i].blockOffset);
			vertexBufferPtr->nIndeces = _indexRegionBlock->faces.nEntries;
			indexBlock = (uint16*)(fileData + tags[_indexRegionBlock->faces.reference].blockOffset);
			_regionBlock = (m3_RegionBlock*)(fileData + tags[_indexRegionBlock->regions.reference].blockOffset);
			
			uint16 *bonelookup =  (uint16*)(fileData + tags[i + 1].blockOffset);
			int c = 0;
		}

		if(_tagNameStr == "__8U")
		{
			_blockSize = tags[i].blockSize % sizeof(m3_VertexBlock_v01);
			_vertexBlock_v01 = (m3_VertexBlock_v01*)(fileData + tags[i].blockOffset);
			if((_blockSize == 0) && (_vertexBlock_v01[0].tangent[3] == 255))
			{
				vertexBufferPtr->nVerteces = (tags[i].blockSize)/sizeof(m3_VertexBlock_v01);
			    _vertexBlock_v01 = (m3_VertexBlock_v01*)(fileData + tags[i].blockOffset);
				_vertexBlockVersion = M3_01;
			}

			_blockSize = tags[i].blockSize % sizeof(m3_VertexBlock_v02);
			_vertexBlock_v02 = (m3_VertexBlock_v02*)(fileData + tags[i].blockOffset);
			if((_blockSize == 0) && (_vertexBlock_v02[0].tangent[3] == 255))
			{
				vertexBufferPtr->nVerteces = (tags[i].blockSize)/sizeof(m3_VertexBlock_v02);
			    _vertexBlock_v02 = (m3_VertexBlock_v02*)(fileData + tags[i].blockOffset);
				_vertexBlockVersion = M3_02;
			}

			_blockSize = tags[i].blockSize % sizeof(m3_VertexBlock_v03);
			if(_blockSize == 0 && _vertexBlockVersion != M3_02)
			{
				vertexBufferPtr->nVerteces = (tags[i].blockSize)/sizeof(m3_VertexBlock_v03);
			    _vertexBlock_v03 = (m3_VertexBlock_v03*)(fileData + tags[i].blockOffset);
				_vertexBlockVersion = M3_03;
			}
		}
	}

	vertexBufferPtr->vertexDataPtr = new m3_VertexData[vertexBufferPtr->nVerteces];
	vertexBufferPtr->normalDataPtr = new m3_NormalData[vertexBufferPtr->nVerteces];
	vertexBufferPtr->textureCoordPtr = new m3_TextureCoordData[vertexBufferPtr->nVerteces];
	vertexBufferPtr->boneVertexDataPtr = new m3_BoneVertexData[vertexBufferPtr->nVerteces];

	float wNormal = 0.0f; 

	for( i = 0; i < vertexBufferPtr->nVerteces; ++i)
	{
		switch(_vertexBlockVersion)
		{
			case M3_01:
				vertexBufferPtr->vertexDataPtr[i].x = _vertexBlock_v01[i].position[0];
				vertexBufferPtr->vertexDataPtr[i].y = _vertexBlock_v01[i].position[1];
				vertexBufferPtr->vertexDataPtr[i].z = _vertexBlock_v01[i].position[2];

				vertexBufferPtr->normalDataPtr[i].x = ((float)2*_vertexBlock_v01[i].normal[0]/255.0f - 1.0f);
				vertexBufferPtr->normalDataPtr[i].y = ((float)2*_vertexBlock_v01[i].normal[1]/255.0f - 1.0f);
				vertexBufferPtr->normalDataPtr[i].z = ((float)2*_vertexBlock_v01[i].normal[2]/255.0f - 1.0f);
				wNormal = (float) _vertexBlock_v01[i].normal[3]/255.0f;
				if(wNormal)
				{
					vertexBufferPtr->normalDataPtr[i].x /= wNormal;
					vertexBufferPtr->normalDataPtr[i].y /= wNormal; 
					vertexBufferPtr->normalDataPtr[i].z /= wNormal;
				}
				vertexBufferPtr->textureCoordPtr[i].u = (float)_vertexBlock_v01[i].uv[0]/2048.0f;
				vertexBufferPtr->textureCoordPtr[i].v = (float)_vertexBlock_v01[i].uv[1]/2048.0f;

				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[0] = _vertexBlock_v01[i].boneIndex[0];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[1] = _vertexBlock_v01[i].boneIndex[1];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[2] = _vertexBlock_v01[i].boneIndex[2];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[3] = _vertexBlock_v01[i].boneIndex[3];

				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[0] = _vertexBlock_v01[i].boneWeight[0];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[1] = _vertexBlock_v01[i].boneWeight[1];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[2] = _vertexBlock_v01[i].boneWeight[2];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[3] = _vertexBlock_v01[i].boneWeight[3];

				//printf("%i     ,",_vertexBlock_v03[i].boneWeight[0]);

			break;
			case M3_02:
				vertexBufferPtr->vertexDataPtr[i].x = _vertexBlock_v02[i].position[0];
				vertexBufferPtr->vertexDataPtr[i].y = _vertexBlock_v02[i].position[1];
				vertexBufferPtr->vertexDataPtr[i].z = _vertexBlock_v02[i].position[2];

				vertexBufferPtr->normalDataPtr[i].x = ((float)2*_vertexBlock_v02[i].normal[0]/255.0f - 1.0f);
				vertexBufferPtr->normalDataPtr[i].y = ((float)2*_vertexBlock_v02[i].normal[1]/255.0f - 1.0f);
				vertexBufferPtr->normalDataPtr[i].z = ((float)2*_vertexBlock_v02[i].normal[2]/255.0f - 1.0f);
				wNormal = (float) _vertexBlock_v02[i].normal[3]/255.0f;
				if(wNormal)
				{
					vertexBufferPtr->normalDataPtr[i].x /= wNormal;
					vertexBufferPtr->normalDataPtr[i].y /= wNormal; 
					vertexBufferPtr->normalDataPtr[i].z /= wNormal;
				}
				vertexBufferPtr->textureCoordPtr[i].u = (float)_vertexBlock_v02[i].uv[0]/2048.0f;
				vertexBufferPtr->textureCoordPtr[i].v = (float)_vertexBlock_v02[i].uv[1]/2048.0f;

				//if(_vertexBlock_v02[i].boneIndex[0] >= 12)
				//	int aaa = 0;
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[0] = _vertexBlock_v02[i].boneIndex[0];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[1] = _vertexBlock_v02[i].boneIndex[1];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[2] = _vertexBlock_v02[i].boneIndex[2];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[3] = _vertexBlock_v02[i].boneIndex[3];

				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[0] = _vertexBlock_v02[i].boneWeight[0];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[1] = _vertexBlock_v02[i].boneWeight[1];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[2] = _vertexBlock_v02[i].boneWeight[2];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[3] = _vertexBlock_v02[i].boneWeight[3];

				//printf("%i     ,",_vertexBlock_v01[i].boneWeight[0]);

			break;
			case M3_03:
				vertexBufferPtr->vertexDataPtr[i].x = _vertexBlock_v03[i].position[0];
				vertexBufferPtr->vertexDataPtr[i].y = _vertexBlock_v03[i].position[1];
				vertexBufferPtr->vertexDataPtr[i].z = _vertexBlock_v03[i].position[2];

				vertexBufferPtr->normalDataPtr[i].x = ((float)2*_vertexBlock_v03[i].normal[0]/255.0f - 1.0f);
				vertexBufferPtr->normalDataPtr[i].y = ((float)2*_vertexBlock_v03[i].normal[1]/255.0f - 1.0f);
				vertexBufferPtr->normalDataPtr[i].z = ((float)2*_vertexBlock_v03[i].normal[2]/255.0f - 1.0f);
				wNormal = (float) _vertexBlock_v03[i].normal[3]/255.0f;
				if(wNormal)
				{
					vertexBufferPtr->normalDataPtr[i].x /= wNormal;
					vertexBufferPtr->normalDataPtr[i].y /= wNormal; 
					vertexBufferPtr->normalDataPtr[i].z /= wNormal;
				}
				vertexBufferPtr->textureCoordPtr[i].u = (float)_vertexBlock_v03[i].uv[0]/2048.0f;
				vertexBufferPtr->textureCoordPtr[i].v = (float)_vertexBlock_v03[i].uv[1]/2048.0f;

				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[0] = _vertexBlock_v03[i].boneIndex[0];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[1] = _vertexBlock_v03[i].boneIndex[1];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[2] = _vertexBlock_v03[i].boneIndex[2];
				vertexBufferPtr->boneVertexDataPtr[i].boneIndex[3] = _vertexBlock_v03[i].boneIndex[3];

				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[0] = _vertexBlock_v03[i].boneWeight[0];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[1] = _vertexBlock_v03[i].boneWeight[1];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[2] = _vertexBlock_v03[i].boneWeight[2];
				vertexBufferPtr->boneVertexDataPtr[i].boneWeight[3] = _vertexBlock_v03[i].boneWeight[3];

				//printf("%i     ,",_vertexBlock_v03[i].boneWeight[0]);
			break;
		}
	}

	vertexBufferPtr->indecesPtr = new unsigned int[vertexBufferPtr->nIndeces];

	for( i = 0; i < vertexBufferPtr->nIndeces; ++i)
		vertexBufferPtr->indecesPtr[i] = indexBlock[i];

	vertexBufferPtr->nRegions = _indexRegionBlock->regions.nEntries;
	vertexBufferPtr->regionDataPtr = new m3_RegionData[vertexBufferPtr->nRegions];

	for( i = 0; i < vertexBufferPtr->nRegions; ++i)
	{
		vertexBufferPtr->regionDataPtr[i].nIndices = _regionBlock[i].nIndices;
		vertexBufferPtr->regionDataPtr[i].nVertices = _regionBlock[i].nVertices;
		vertexBufferPtr->regionDataPtr[i].ofsIndices = _regionBlock[i].ofsIndices;
		vertexBufferPtr->regionDataPtr[i].ofsVertices = _regionBlock[i].ofsVertices;
	}

	//delete[] fileData;
	return vertexBufferPtr;
}

m3_VertexBufferID*  m3_M3Loader::Load(m3_VertexBufferPtr* value)
{
	if(glGenBuffersARB == NULL)
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
	if(glBindBufferARB == NULL)
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
	if(glBufferDataARB == NULL)
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
	if(glDeleteBuffersARB == NULL)
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");
	if(glMapBufferARB == NULL)
		glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
	if(glUnmapBufferARB == NULL)
		glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC) wglGetProcAddress("glUnmapBufferARB");

	m3_VertexBufferID* vertexBufferId = new m3_VertexBufferID();

	glGenBuffersARB( 1, &vertexBufferId->vertexId );							
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertexBufferId->vertexId );		
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, value->nVerteces*3*sizeof(float), value->vertexDataPtr, GL_STREAM_DRAW_ARB );

	glGenBuffersARB( 1, &vertexBufferId->textureCoordId );							
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertexBufferId->textureCoordId );		
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, value->nVerteces*2*sizeof(float), value->textureCoordPtr, GL_STREAM_DRAW_ARB );

	glGenBuffersARB( 1, &vertexBufferId->decaltextureCoordId );		
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertexBufferId->decaltextureCoordId);		
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, value->nVerteces*2*sizeof(float), value->decaltextureCoordPtr, GL_STREAM_DRAW_ARB );

	glGenBuffersARB( 1, &vertexBufferId->normalId );							
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertexBufferId->normalId );		
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, value->nVerteces*3*sizeof(float), value->normalDataPtr, GL_STREAM_DRAW_ARB );

	return vertexBufferId;
}