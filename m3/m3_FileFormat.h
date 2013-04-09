#pragma once
#ifndef _M3_FILEFORMAT_H_
#define _M3_FILEFORMAT_H_
#include <vector>
#define SAFE_DELETE(v) if (v) delete []v;
#include "m3_TypeDef.h"
#include "Vector3d.h"
#include "Matrix4x4.h"



struct m3_CheckVersion 
{
	uint32 values[17];
	uint32 version;
};

struct m3_Header
{
    char id[4];
    uint32 tagsOffset;
    uint32 nTags;
    uint32 unknown_01;
    uint32 unknown_02;
};

struct m3_Tag 
{
	char tagName[4];
	uint32 blockOffset;
	uint32 blockSize;
	uint32 version;
};

struct m3_TagReference 
{
    uint32 nEntries;
    uint32 reference;
};

struct m3_Tags2
{
	m3_TagReference a;
	m3_TagReference b;
};


enum m3_Version { M3_01 = 0, M3_02, M3_03, M3_UNKNOWN };

struct m3_VertexBlock_v01
{
	float position[3];
	uint8 boneWeight[4];
    uint8 boneIndex[4];
    uint8 normal[4];
    int16 uv[2];
	uint32 unk;
	uint8 tangent[4];
};

struct m3_VertexBlock_v02
{
    float position[3];
    uint8 boneWeight[4];
    uint8 boneIndex[4];
    uint8 normal[4];  
    int16 uv[2];
    uint8 tangent[4];
};

struct m3_VertexBlock_v03
{
    float position[3];
    uint8 boneWeight[4];
    uint8 boneIndex[4];
    uint8 normal[4];  
    int16 uv[2];
    uint8 unknValues[10];
};

struct m3_IndexRegionBlock
{
     m3_TagReference faces; 
     m3_TagReference regions; 
     m3_TagReference BAT;
     m3_TagReference MSEC;
};


struct m3_TagsCutoffs
{
	uint8 skip_01[88];
    m3_TagReference globalBonesIndecesList;
};

struct m3_RegionBlock 
{
	/*0x00*/ uint32 unk; //always 0?
    /*0x04*/ uint16 ofsVertices;
    /*0x06*/ uint16 nVertices;
    /*0x08*/ uint32 ofsIndices;
    /*0x0C*/ uint32 nIndices;
    /*0x10*/ uint16 boneCount; //boneLookup total count (redundant?)
    /*0x12*/ uint16 indBone; //indice into boneLookup
    /*0x14*/ uint16 numBone; //number of bones used from boneLookup
    /*0x16*/ uint16 s1[3]; //flags? vital for sc2 engine rendering the geometry
};

struct m3_RegionData 
{
     uint16 ofsVertices;
     uint16 nVertices;
     uint32 ofsIndices;
     uint32 nIndices; 
};

struct m3_CalcNomralBlock
{
	float normal[3];
};


struct m3_VertexData
{
	float x,y,z;
};

struct m3_NormalData
{
	float x,y,z;
};

struct m3_TextureCoordData
{
	float u,v;
};

struct m3_BoneVertexData
{
	uint8 boneWeight[4];
    uint8 boneIndex[4];
};

struct QuaternionStruct
{
	float x,y,z,w;
};

struct AnimationReference
{
    /*0x00*/ uint16 flags; //usually 1
    /*0x02*/ uint16 animflag; //6 indicates animation data present
    /*0x04*/ uint32 animid; //a unique uint32 value referenced in STC.animid and STS.animid
};

struct AnimsId
{
	uint16 animId;
	uint16 sanimId;
};

struct m3_VertexBufferID
{
	unsigned int vertexId;
	unsigned int normalId;	
	unsigned int textureCoordId;
  unsigned int decaltextureCoordId;
};

struct m3_BoneData
{
	AnimationReference translationId;
	AnimationReference rotationId;

	Vector3d _startPosition;
	float    _startRotation[4];
	Vector3d _startScale;

	int16 parent;

	Matrix4x4 matrix;

	m3_BoneData *parentPtr;
};

struct SeqData
{
    /*0x00*/ m3_TagReference name;
    /*0x08*/ uint32 d1;
    /*0x0C*/ uint16 indSEQ[2]; //two points to the sequence in SEQS chunk, possibly one for SEQS and one for STG
    /*0x10*/ m3_TagReference animid; //list of unique uint32s used in chunks with animation. The index of these correspond with the data in the next reference.
    /*0x18*/ m3_TagReference animindex; //lookup table, connects animid with it's animation data, nEntries of AnimationIndex reference using U32_ id
    /*0x20*/ uint32 d2;
    /*0x24*/ m3_TagReference _seqData[13]; //SD3V - Trans, SD4Q - Rotation, SDR3 - Scale?, SDFG - Flags, SDMB - Bounding Boxes?
};

/*struct SeqData
{
	m3_TagReference name;
	uint8 unk_01[8];
	m3_TagReference animid;
	m3_TagReference animoffs;
	uint8 unk_02[20];
	m3_TagReference trans;
	m3_TagReference rot;
	uint8 unk_03[72];
};*/

struct m3_SequenceData
{
	uint32 *transformsId;
	uint32 nTransforms;
	Vector3d *vTranslate;
	uint32 nTranslate;
	float *qRotate[4];
	uint32 nRotate;
};

struct m3_AnimationData 
{
	m3_TagReference frames; 
	unsigned long flags;
	unsigned long fend;
	m3_TagReference keys;
};

struct m3_AnimationTranslationData
{
	uint32 nFrames;
	Vector3d *vTranslation;
};

struct m3_AnimationRotationData
{
	uint32 nFrames;
	QuaternionStruct *vRotation;
};

struct _m3_TransformationIndex
{
	uint16 tranformationDataIndex;
	uint16 sequenceDataIndex;
};

struct _m3_TranslationDataPtr
{
	uint32 nFrames;
	Vector3d *keys;
  int32 *timelines;
};

struct _m3_RotationDataPtr
{
	uint32 nFrames;
	QuaternionStruct *keys;
  int32 *timelines;
};

struct _m3_SequenceBlock
{
	uint32 nTransformationIds;
	uint32 nTransformationIndeces;

	uint32 *transformationId;
	_m3_TransformationIndex *transformationIndex;

	uint32 nTSequenceData;
	uint32 nRSequenceData;

	_m3_TranslationDataPtr *tSequenceData;
	_m3_RotationDataPtr *rSequenceData;
  uint32 frequency;
  char * name;
};

struct m3_string
{
	char* name;
};

struct m3_VertexBufferPtr
{
	m3_VertexData *vertexDataPtr;
	m3_NormalData *normalDataPtr;
	m3_TextureCoordData *textureCoordPtr;
  m3_TextureCoordData *decaltextureCoordPtr;
	unsigned int *indecesPtr;

	unsigned int nVerteces;
	unsigned int nIndeces;

	m3_RegionData *regionDataPtr;
	unsigned int nRegions;

	m3_BoneData *boneDataPtr;
	unsigned int nBones;

	m3_BoneVertexData *boneVertexDataPtr;

	m3_SequenceData *sequenceData;

	m3_AnimationData *translationAnimation;
	uint32 nTranslationAnimation;

	m3_AnimationData *rotationAnimation;
	uint32 nRotationAnimation;

	m3_AnimationTranslationData *translationData;
	m3_AnimationRotationData *rotationData;

	//_m3_SequenceBlock _sequenceBlock;
  std::vector<_m3_SequenceBlock> _sequenceBlock;
  unsigned int nSequence;

	Matrix4x4 *staticBonesMatrices;

	uint16 *globalBonesIndecesList;

	std::string *animationSequencesNames;

	m3_VertexBufferPtr()
	{
		vertexDataPtr = NULL;
		normalDataPtr = NULL;
		textureCoordPtr = NULL;
		indecesPtr = NULL;
		regionDataPtr = NULL;

		nVerteces = 0;
		nIndeces = 0;
		nRegions = 0;
	}
  ~m3_VertexBufferPtr()
  {
    for(uint32 i=0;i<_sequenceBlock.size();i++)
    {
      SAFE_DELETE(_sequenceBlock[i].transformationId);
      SAFE_DELETE(_sequenceBlock[i].transformationIndex);
      SAFE_DELETE(_sequenceBlock[i].tSequenceData);
      uint32 index;
      for( index = 0; index < _sequenceBlock[i].nTSequenceData; ++index)
      {
        SAFE_DELETE(_sequenceBlock[i].tSequenceData[index].keys);
        SAFE_DELETE(_sequenceBlock[i].tSequenceData[index].timelines);
      }
      SAFE_DELETE(_sequenceBlock[i].rSequenceData);
      for( index = 0; index < _sequenceBlock[i].nRSequenceData; ++index)
      {
        SAFE_DELETE(_sequenceBlock[i].rSequenceData[index].keys);
        SAFE_DELETE(_sequenceBlock[i].rSequenceData[index].timelines);
      }
    }
    _sequenceBlock.clear();
  }
};

struct m3_ComplexMeshData
{
	m3_VertexBufferID*  vertexDataId;
	m3_VertexBufferPtr* vertexDataPtr;
};

enum m3_MeshExtension { MESH_M3 = 0, MESH_3DS , MESH_M34,MESH_NONE};


typedef struct{
    int unk;
    m3_TagReference name;
    int unk2[85];
}LAYR;

 struct SEQS
    {
        /*0x00*/ int32 d1;
        /*0x04*/ int32 d2;
        /*0x08*/ m3_TagReference name;
        /*0x10*/ int32 d3;
        /*0x14*/ uint32 length;
        /*0x18*/ int32 d4;
        /*0x1C*/ uint32 flags;
        /*0x20*/ int32 unk[5];
        /*0x34*/ float extents[6];
        /*0x4C*/ float radius;
        /*0x50*/ int32 d5;
        /*0x54*/ int32 d6;
    };


struct BONE_02
 {
        int32 d1; // Keybone?
        m3_TagReference name;
        uint32 flags;
        int16 parent;
        int16 s1;

        float floats[34];
};


struct BONE
{
    /*0x00*/ int32 d1; // Keybone?
    /*0x04*/ m3_TagReference name;
    /*0x0C*/ uint32 flags; //2560 = Weighted vertices rendered, 512 = not rendered
    /*0x10*/ int16 parent;
    /*0x12*/ uint16 s1;
    /*0x14*/ AnimationReference translationId; //unique animation ID ref
    /*0x1C*/ Vector3d position; //bone position is relative to parent bone and its rotation
    /*0x28*/ int32 d2[4];
    /*0x38*/ AnimationReference rotationId;
    /*0x40*/ float rotation[4]; //initial bone rotation
    /*0x50*/ int32 d3[3];
    /*0x5C*/ float f1;
    /*0x60*/ int32 d4;
    /*0x64*/ AnimationReference scaleid;
    /*0x6C*/ Vector3d scale; //initial scale
    /*0x78*/ Vector3d v1;
    /*0x84*/ int32 d5[6]; //appears to have another unknown animation ref at 0x84
};

/*struct BONE
{
    int32 d1; 
    m3_TagReference name;
    uint32 flags;
    int16 parent;
    uint8 unk_01[6];
	uint32 transId;
	//double translationId;
	Vector3d position;
	uint8 unk_02[20];
	uint32 rotId;;
	//double rotationId;
	float rotation[4];
	uint8 unk_03[28];
	Vector3d scale;
	uint8 unk_04[36];
};*/



/*		SkipBytes 4
		b.name = ReadRef()
		--echo ("Bone_" + i as string + ": " + b.name)
		SkipBytes 4
		b.parent = ReadShort bstream #signed + 1
		SkipBytes 6
		b.transid = ReadLong bstream #unsigned
		b.pos = ReadVec #v3d
		SkipBytes 20
		b.rotid = ReadLong bstream #unsigned
		b.rot = ReadVec #quat
		SkipBytes 28
		b.scale = ReadVec #v3d
		SkipBytes 36*/



#endif