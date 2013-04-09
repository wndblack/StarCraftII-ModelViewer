#include "videosetting.h"
#include <wx/txtstrm.h>

bool init; 
bool render;
bool refresh;

// OpenGL Settings
int xRes, yRes;
int pixelFormat;
float fov;
int desktopBPP;

// Card capabilities
std::vector<VideoCaps> capsList;
VideoCaps curCap;
int capIndex;

int AnisofilterLevel;
int numTextureUnits;

// Card Info
char *vendor;
char *version;
char *renderer;

// Is there hardware support?
bool hasHardware;
bool secondPass;
// Video card support for OGL Extensions
bool supportFragProg;
bool supportVertexProg;
bool supportGLSL;
bool supportCompression;// = false;
bool supportMultiTex;//  = false;
bool supportDrawRangeElements;//  = false;
bool supportPointSprites;//  = false;
bool supportShaders;//  = false;
bool supportAntiAlias;//  = false;
bool supportVBO;//  = false;
bool supportPBO;//  = false;
bool supportFBO;//  = false;
bool supportNPOT;//  = false;
bool supportOGL20;//  = false;
bool supportWGLPixelFormat; 
bool supportTexRects;

extern HWND hWnd;
extern HDC hDC;
extern HGLRC hRC;
extern PIXELFORMATDESCRIPTOR pfd;


void EnumDisplayModes()
{
#ifdef _WINDOWS
	if (!hasHardware)
		return;

	// Error check
	if (!hDC) {
		return;
	}

	// This is where we do the grunt work of checking the caps
	// find out how many pixel formats we have
	size_t num_pfd = 0;
	std::vector<int> iAttributes;
	std::vector<int> results;

	iAttributes.push_back(WGL_NUMBER_PIXEL_FORMATS_ARB);
	results.resize(2);	// 2 elements
	wglGetPixelFormatAttribivARB(hDC, 0, 0, 1, &iAttributes[0], &results[0]);	// get the number of contexts we can create
	num_pfd = results[0];
	iAttributes.clear();
	results.clear();

	// error check
	if (num_pfd == 0) {
		hasHardware = false;
		return;
	}

	iAttributes.push_back(WGL_DRAW_TO_WINDOW_ARB);
	iAttributes.push_back(WGL_STENCIL_BITS_ARB);
	iAttributes.push_back(WGL_ACCELERATION_ARB);
	iAttributes.push_back(WGL_DEPTH_BITS_ARB);
	iAttributes.push_back(WGL_SUPPORT_OPENGL_ARB);
	iAttributes.push_back(WGL_DOUBLE_BUFFER_ARB);
	iAttributes.push_back(WGL_PIXEL_TYPE_ARB);
	iAttributes.push_back(WGL_COLOR_BITS_ARB);
	iAttributes.push_back(WGL_ALPHA_BITS_ARB);
	iAttributes.push_back(WGL_ACCUM_BITS_ARB);

	if(glewIsSupported("GL_ARB_multisample")) {
		iAttributes.push_back(WGL_SAMPLE_BUFFERS_ARB);
		iAttributes.push_back(WGL_SAMPLES_ARB);
	}

	results.resize(iAttributes.size());

	// Reset our list of card capabilities.
	capsList.clear();

	for (size_t i=0; i<num_pfd; i++) {
		if(!wglGetPixelFormatAttribivARB(hDC, (int)i+1,0, (UINT)iAttributes.size(), &iAttributes[0], &results[0]))
			return;
		
		// once we make it here we can look at the pixel data to make sure this is a context we want to use
		// results[0] is WGL_DRAW_TO_WINDOW,  since we only work in windowed mode,  no need to return results for fullscreen mode
		// results[4] is WGL_SUPPORT_OPENGL,  obvious if the mode doesn't support opengl then its useless to us.
		// results[3] is bitdepth,  if it has a 0 bitdepth then its useless to us
		if(results[0] == GL_TRUE && results[4] == GL_TRUE && results[3]>0) {
			// what we have here is a contect which is drawable to a window, is in rgba format and is accelerated in some way
			// so now we pull the infomation, fill a structure and shove it into our map
			VideoCaps caps;

			if(glewIsSupported("GL_ARB_multisample")) {
				caps.sampleBuffer = results[10];
				caps.aaSamples = results[11];
			} else {
				caps.sampleBuffer = false;
				caps.aaSamples = 0;
			}

			caps.accum = results[9];
			caps.alpha = results[8];
			caps.colour = results[7];
			//caps.pixelType = results[6]; /* WGL_TYPE_RGBA_ARB*/
			caps.doubleBuffer = results[5];
			caps.zBuffer = results[3];
			caps.hwAcc = results[2]; /*WGL_FULL_ACCELERATION_ARB / WGL_GENERIC_ACCELERATION_ARB / WGL_NO_ACCELERATION_ARB;*/
			caps.stencil = results[1];
			capsList.push_back(caps);	// insert into the map
		}
	}
#endif
}
bool GetCompatibleWinMode(VideoCaps caps)
{
#ifdef _WINDOWS
	if (!hasHardware)
		return false;

	// Ask the WGL for the clostest match to this pixel format
	if (!hDC) {
		//throw std::runtime_error("VideoSettings::GetCompatibleWinMode() : Attempted to Get a Compatible Window Mode Without a Valid Window");
		return false;
	}

	std::vector<int> iAttributes;
	std::vector<int> results;
	unsigned int numformats;
	float fAtrributes[] = { 0,0};

	iAttributes.push_back(WGL_DRAW_TO_WINDOW_ARB);
	iAttributes.push_back(GL_TRUE);
	iAttributes.push_back(WGL_ACCELERATION_ARB);
	iAttributes.push_back(caps.hwAcc);
	iAttributes.push_back(WGL_COLOR_BITS_ARB);
	iAttributes.push_back(caps.colour);		// min color required
	iAttributes.push_back(WGL_ALPHA_BITS_ARB);
	iAttributes.push_back(caps.alpha);		// min alpha bits
	iAttributes.push_back(WGL_DEPTH_BITS_ARB);
	iAttributes.push_back(caps.zBuffer);
	iAttributes.push_back(WGL_STENCIL_BITS_ARB);
	iAttributes.push_back(caps.stencil);
	if(caps.aaSamples != 0) {
		iAttributes.push_back(WGL_SAMPLE_BUFFERS_ARB);
		iAttributes.push_back(caps.sampleBuffer);
		iAttributes.push_back(WGL_SAMPLES_ARB);
		iAttributes.push_back(caps.aaSamples);
	}
	iAttributes.push_back(WGL_DOUBLE_BUFFER_ARB);
	iAttributes.push_back(caps.doubleBuffer);
	if(caps.accum != 0) {
		iAttributes.push_back(WGL_ACCUM_BITS_ARB);
		iAttributes.push_back(caps.accum);
	}
	iAttributes.push_back(0);
	iAttributes.push_back(0);
	
	int status = wglChoosePixelFormatARB(hDC, &iAttributes[0], fAtrributes, 1, &pixelFormat, &numformats);	// find a matching format
	if (status == GL_TRUE && numformats) {
		// we have a matching format, extract the details
		iAttributes.clear();
		iAttributes.push_back(WGL_COLOR_BITS_ARB);
		iAttributes.push_back(WGL_ALPHA_BITS_ARB);
		iAttributes.push_back(WGL_DEPTH_BITS_ARB);
		iAttributes.push_back(WGL_STENCIL_BITS_ARB);
		iAttributes.push_back(WGL_SAMPLE_BUFFERS_ARB);
		iAttributes.push_back(WGL_SAMPLES_ARB);
		iAttributes.push_back(WGL_DOUBLE_BUFFER_ARB);
		iAttributes.push_back(WGL_ACCUM_BITS_ARB);
		iAttributes.push_back(WGL_ACCELERATION_ARB);
		results.resize(iAttributes.size());

		if(!wglGetPixelFormatAttribivARB(hDC, pixelFormat, PFD_MAIN_PLANE, (UINT)iAttributes.size(), &iAttributes[0], &results[0])) {
			//int i = GetLastError();
			return false;
		}

		curCap.aaSamples = results[5];
		curCap.accum = results[7];
		curCap.alpha = results[1];
		curCap.colour = results[0];
		curCap.doubleBuffer = results[6];
		curCap.sampleBuffer = results[4];
		curCap.stencil = results[3];
		curCap.zBuffer = results[2];
		curCap.hwAcc = results[8];

		for (size_t i=0; i<capsList.size(); i++) {
			if (capsList[i].colour == curCap.colour && 
				capsList[i].zBuffer == curCap.zBuffer && 
				capsList[i].alpha == curCap.alpha && 
				capsList[i].hwAcc == curCap.hwAcc && 
				capsList[i].doubleBuffer==curCap.doubleBuffer &&
				capsList[i].sampleBuffer==curCap.sampleBuffer &&
				capsList[i].aaSamples==curCap.aaSamples) {
				capIndex = (int)i;
				break;
			}
		}

		return true;
	}
#endif
	return false;
}

void SetCurrent()
{
#ifdef _WINDOWS
	if(!wglMakeCurrent(hDC, hRC)) {					// Try To Activate The Rendering Context
		//wxMessageBox("Can't Activate The GL Rendering Context.","ERROR");
		//render = false;
	} else {
		render = true;
	}
#endif
}
bool GetAvailableMode()
{
#ifdef _WINDOWS
	render = false;

	if (!hWnd)
		return false;

	// Clear the rendering context
	wglMakeCurrent(NULL, NULL); 

	if (hRC) {
		wglDeleteContext(hRC);
		hRC = NULL;
	}

	if (hDC) {
		ReleaseDC(hWnd,hDC);
		hDC = NULL;
	}

	hDC = GetDC(hWnd);						// Grab A Device Context For This Window
	if (hDC == 0) {							// Did We Get A Device Context?
		// Failed
		//throw std::runtime_error("Failed To Get Device Context");
		return false;
	}

	pixelFormat = ChoosePixelFormat(hDC, &pfd);				// Find A Compatible Pixel Format
	if (pixelFormat == 0) {									// Did We Find A Compatible Format?
		// Failed
		//throw std::runtime_error("Failed To Acuire PixelFormat");
		return false;
	}

	if (SetPixelFormat(hDC, pixelFormat, &pfd) == false) {		// Try To Set The Pixel Format
		// Failed
		//throw std::runtime_error("Failed To SetPixelFormat");
		return false;
	}

	hRC = wglCreateContext(hDC);						// Try To Get A Rendering Context
	if (hRC == 0) {								// Did We Get A Rendering Context?
		// Failed
		//throw std::runtime_error("Failed To Get OpenGL Context");
		return false;
	}

	SetCurrent();
	render = true;
#endif
	return true;
}
void SetMode()
{
#ifdef _WINDOWS
	if (!hWnd)
		return;

	// Clear the rendering context
	wglMakeCurrent(NULL, NULL); 

	if (hRC) {
		wglDeleteContext(hRC);
		hRC = NULL;
	}

	if (hDC) {
		ReleaseDC(hWnd,hDC);
		hDC = NULL;
	}

	hDC = GetDC(hWnd);
	if (!hDC) {						// Did We Get A Device Context?
		return;
	}
	
	GLboolean status;
	unsigned int numFormats;
	float fAttributes[] = { 0,0};
	std::vector<int> AttributeList;

	AttributeList.push_back(WGL_DRAW_TO_WINDOW_ARB);
	AttributeList.push_back(GL_TRUE);
	AttributeList.push_back(WGL_SUPPORT_OPENGL_ARB);
	AttributeList.push_back(GL_TRUE);
	AttributeList.push_back(WGL_ACCELERATION_ARB);
	AttributeList.push_back(curCap.hwAcc);
	AttributeList.push_back(WGL_COLOR_BITS_ARB);
	AttributeList.push_back(curCap.colour);
	AttributeList.push_back(WGL_ALPHA_BITS_ARB);
	AttributeList.push_back(curCap.alpha);
	AttributeList.push_back(WGL_DEPTH_BITS_ARB);
	AttributeList.push_back(curCap.zBuffer);
	//AttributeList.push_back(WGL_STENCIL_BITS_ARB);
	//AttributeList.push_back(curCap.stencil);
	AttributeList.push_back(WGL_DOUBLE_BUFFER_ARB);
	AttributeList.push_back(curCap.doubleBuffer);

	// 8229 being software mode
	if (curCap.hwAcc!=8229  && curCap.accum!=0) {
		AttributeList.push_back(WGL_ACCUM_BITS_ARB);
		AttributeList.push_back(curCap.accum);
	}

	if (curCap.aaSamples != 0 && supportAntiAlias) {
		AttributeList.push_back(WGL_SAMPLE_BUFFERS_ARB);
		AttributeList.push_back(GL_TRUE);
		AttributeList.push_back(WGL_SAMPLES_ARB);
		AttributeList.push_back(curCap.aaSamples);
	}
	AttributeList.push_back(0);
	AttributeList.push_back(0);
    

	status = wglChoosePixelFormatARB(hDC, &AttributeList[0], fAttributes, 1, &pixelFormat, &numFormats);
	if (status == GL_TRUE && numFormats) {
		if (SetPixelFormat(hDC, pixelFormat, &pfd) == FALSE) {
			//wxLogMessage(wxT("Error: Failed To Set Requested Pixel Format."));
			secondPass = true;
			return;
		}

		hRC = wglCreateContext(hDC);
		if (!hRC) {
			return;
		}
		
		render = true;
		SetCurrent();
	} else {
		// Error
		// Something went wrong,  fall back to the basic opengl setup
		GetAvailableMode();
	}
#endif
}

bool InitGLSetting()
{	
	int glewErr = glewInit();
  if (glewErr != GLEW_OK)
  {
  		return false;
	}

	if(glewIsSupported("GL_EXT_texture_filter_anisotropic"))
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLint*)&AnisofilterLevel);
	else
		AnisofilterLevel = 0;

	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, (GLint*)&numTextureUnits);
	vendor = ((char *)glGetString(GL_VENDOR));
	version = ((char *)glGetString(GL_VERSION));
	renderer = ((char *)glGetString(GL_RENDERER));

	double num = atof((char *)glGetString(GL_VERSION));
	supportOGL20 = (num >= 2.0);
	if (supportOGL20)
		supportNPOT = true;
	else
		supportNPOT = glewIsSupported("GL_ARB_texture_non_power_of_two") == GL_TRUE ? true : false;

	supportFragProg = glewIsSupported("GL_ARB_fragment_program") == GL_TRUE ? true : false;
	supportVertexProg = glewIsSupported("GL_ARB_vertex_program") == GL_TRUE ? true : false;
	supportGLSL = glewIsSupported("GL_ARB_shading_language_100") == GL_TRUE ? true : false;
	supportShaders = (supportFragProg && supportVertexProg);

	supportDrawRangeElements = glewIsSupported("GL_EXT_draw_range_elements") == GL_TRUE ? true : false;
	supportMultiTex = glewIsSupported("GL_ARB_multitexture") == GL_TRUE ? true : false;
	supportVBO = glewIsSupported("GL_ARB_vertex_buffer_object") == GL_TRUE ? true : false;
	supportCompression = glewIsSupported("GL_ARB_texture_compression GL_ARB_texture_cube_map GL_EXT_texture_compression_s3tc") == GL_TRUE ? true : false;
	supportPointSprites = glewIsSupported("GL_ARB_point_sprite GL_ARB_point_parameters") == GL_TRUE ? true : false;

  supportPBO = wglewIsSupported("WGL_ARB_pbuffer WGL_ARB_render_texture") == GL_TRUE ? true : false;
	supportAntiAlias = wglewIsSupported("WGL_ARB_multisample") == GL_TRUE ? true : false;
	supportWGLPixelFormat = wglewIsSupported("WGL_ARB_pixel_format") == GL_TRUE ? true : false;

  supportFBO = glewIsSupported("GL_EXT_framebuffer_object") == GL_TRUE ? true : false;
	supportTexRects = glewIsSupported("GL_ARB_texture_rectangle") == GL_TRUE ? true : false;

	if (wxString(renderer, wxConvUTF8).IsSameAs(wxT("GDI Generic"), false)) 
  {
		// bloody oath - wtb a graphics card
		hasHardware = false;
	} 
  else 
  {
		hasHardware = true;
	}
  // Max texture sizes
	GLint texSize; 
	// Rectangle
	if (glewIsSupported("GL_ARB_texture_rectangle")) 
  {
		glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB, &texSize); 
	}
	// Square
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize); 
	
	init = true;

		if (supportWGLPixelFormat) 
    {
			EnumDisplayModes();
			GetCompatibleWinMode(curCap);
		}
	if (hasHardware && supportWGLPixelFormat)
		SetMode();

	return true;
}

void InitGL()
{
	if (!init)
		return;

	GLenum err = 0;
	err = glGetError();

	if (supportDrawRangeElements && supportVBO) 
  {
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
  else 
  {
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	
	if(supportAntiAlias && curCap.aaSamples>0)
		glEnable(GL_MULTISAMPLE_ARB);

	// Colour and materials
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
	glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
	
	// For environmental mapped meshes
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 18.0f);


	// TODO: Implement a scene graph, or scene manager to optimise OpenGL?
	// Default texture settings.
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glAlphaFunc(GL_GEQUAL, 0.9f);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_SWAP_BYTES, false);
	glPixelStorei(GL_PACK_LSB_FIRST, false);

	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


/*
	glEnable(GL_TEXTURE_2D);
  glShadeModel(GL_SMOOTH); 
	glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);       // 所作深度测试的类型

  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);   // 设置s方向的纹理自动生成
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);   // 设置t方向的纹理自动生成
*/

	err = glGetError();
}