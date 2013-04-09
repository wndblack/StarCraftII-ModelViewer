#include "m3_DDSLoader.h"

PFNGLCOMPRESSEDTEXIMAGE2DARBPROC m3_DDSLoader::glCompressedTexImage2DARB = NULL;

DDS_IMAGE_DATA *m3_DDSLoader::LoadImageData(std::string value)
{
    DDS_IMAGE_DATA *pDDSImageData;
    DDSURFACEDESC2 ddsd;
    char filecode[4];
    FILE *pFile;
    int factor;
    int bufferSize;

    pFile = fopen( value.c_str(), "rb" );

    if( pFile == NULL )
    {
        char str[255];
		sprintf( str, "loadDDSTextureFile couldn't find, or failed to load \"%s\"", value.c_str() );
        MessageBox( NULL, str, "ERROR", MB_OK|MB_ICONEXCLAMATION );
        return NULL;
    }

    // Verify the file is a true .dds file
    fread( filecode, 1, 4, pFile );

    if( strncmp( filecode, "DDS ", 4 ) != 0 )
    {
        char str[255];
        sprintf( str, "The file \"%s\" doesn't appear to be a valid .dds file!", value.c_str() );
        MessageBox( NULL, str, "ERROR", MB_OK|MB_ICONEXCLAMATION );
        fclose( pFile );
        return NULL;
    }

    // Get the surface descriptor
    fread( &ddsd, sizeof(ddsd), 1, pFile );

    pDDSImageData = (DDS_IMAGE_DATA*) malloc(sizeof(DDS_IMAGE_DATA));

    memset( pDDSImageData, 0, sizeof(DDS_IMAGE_DATA) );

    //
    // This .dds loader supports the loading of compressed formats DXT1, DXT3 
    // and DXT5.
    //

    switch( ddsd.ddpfPixelFormat.dwFourCC )
    {
        case FOURCC_DXT1:
            // DXT1's compression ratio is 8:1
            pDDSImageData->format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            factor = 2;
            break;

        case FOURCC_DXT3:
            // DXT3's compression ratio is 4:1
            pDDSImageData->format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            factor = 4;
            break;

        case FOURCC_DXT5:
            // DXT5's compression ratio is 4:1
            pDDSImageData->format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            factor = 4;
            break;

        default:
            char str[255];
            sprintf( str, "The file \"%s\" doesn't appear to be compressed "
                "using DXT1, DXT3, or DXT5!", value.c_str() );
            MessageBox( NULL, str, "ERROR", MB_OK|MB_ICONEXCLAMATION );
            return NULL;
    }

    //
    // How big will the buffer need to be to load all of the pixel data 
    // including mip-maps?
    //

    if( ddsd.dwLinearSize == 0 )
    {
        MessageBox( NULL, "dwLinearSize is 0!","ERROR",
            MB_OK|MB_ICONEXCLAMATION);
    }

    if( ddsd.dwMipMapCount > 1 )
        bufferSize = ddsd.dwLinearSize * factor;
    else
        bufferSize = ddsd.dwLinearSize;

    pDDSImageData->pixels = (unsigned char*)malloc(bufferSize * sizeof(unsigned char));

    fread( pDDSImageData->pixels, 1, bufferSize, pFile );

    fclose( pFile );

    pDDSImageData->width      = ddsd.dwWidth;
    pDDSImageData->height     = ddsd.dwHeight;
    pDDSImageData->numMipMaps = ddsd.dwMipMapCount;

    if( ddsd.ddpfPixelFormat.dwFourCC == FOURCC_DXT1 )
        pDDSImageData->components = 3;
    else
        pDDSImageData->components = 4;

    return pDDSImageData;
}


GLuint m3_DDSLoader::Load(DDS_IMAGE_DATA *value)
{
    //DDS_IMAGE_DATA *pDDSImageData = _LoadImageData(fileName);
	GLuint textureId = -1;
    if(value != NULL)
    {
        //int nHeight     = pDDSImageData->height;
       // int nWidth      = pDDSImageData->width;
        //int nNumMipMaps = pDDSImageData->numMipMaps;

        int nBlockSize = -1;

        if( value->format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )
            nBlockSize = 8;
        else
            nBlockSize = 16;

        glGenTextures( 1, &textureId );
        glBindTexture( GL_TEXTURE_2D, textureId );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        int nSize = -1;
        int nOffset = 0;

		for( int i = 0; i < value->numMipMaps; ++i )
        {
			if( value->width  == 0 ) value->width  = 1;
			if( value->height == 0 ) value->height = 1;

            nSize = ((value->width + 3)/4) * ((value->height + 3)/4) * nBlockSize;

            glCompressedTexImage2DARB( GL_TEXTURE_2D,
                                       i,
                                       value->format,
                                       value->width,
                                       value->height,
                                       0,
                                       nSize,
                                       value->pixels + nOffset );

            nOffset += nSize;

            // Half the image size for the next mip-map level...
            value->width  = (value->width  / 2);
            value->height = (value->height / 2);
        }
    }

    if( value != NULL )
    {
        if( value->pixels != NULL )
            free( value->pixels );

        free( value );
    }

	return textureId;
}
