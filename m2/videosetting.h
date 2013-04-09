#pragma once
#include <gl/glew.h>
#include "GL\wglew.h"
#include <gl\glaux.h>
#include <gl/gl.h>
#include <gl/glu.h>


#include <vector>
struct VideoCaps
{
	int	colour;
	int alpha;
	int zBuffer;
	int accum;
	int stencil;
	int aaSamples;	// how many AA samples can this mode do?
	int hwAcc;	// Hardware Acceleration mode?
	GLboolean sampleBuffer;	// do we have an AA sample buffer?
	GLboolean doubleBuffer;	// double buffered?
};


bool InitGLSetting();
void InitGL();