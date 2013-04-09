#include "m3_setup.h"
#include <Cg/Cg.h>
#include <Cg/cgGL.h>
#include "m3_DDSLoader.h"

#include <SDL.h>
#include "m3_Mesh.h"


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480


std::vector<m3_Mesh*> meshList;
Uint32 my_callbackfunc(Uint32 interval, void *param)
{
 return interval;
}
void* my_callback_param;



void init( void )
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glEnable( GL_DEPTH_TEST );
	glShadeModel(GL_SMOOTH);

	GLfloat LightPosition[] = { 0.0f, 0.0f, 0.0f,  1.0f };
	GLfloat LightAmbient[]  = { 0.7f, 0.7f, 0.7f,  1.0f };
	GLfloat LightDiffuse[]  = { 1.0f, 1.0f, 1.0f,  1.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);	
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	
	glEnable(GL_LIGHT1);	

	glDisable(GL_LIGHTING);

	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	Matrix4x4 mProjection = MatrixProjection(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f,1000.0f);
	glLoadMatrixf(mProjection.m);

	/////////////////////////////////////////////////////////////////////////////

	/*WIN32_FIND_DATA FindFileData; 
	HANDLE hFind = INVALID_HANDLE_VALUE; 
	char *DirSpec = "Content\\models\\*.m3";
	hFind = FindFirstFile(DirSpec, &FindFileData);

	if(hFind != INVALID_HANDLE_VALUE)
	{*/
	    m3_Mesh *pMesh = new m3_Mesh;
		std::string fileName = "m3\\wasp01.m3";//"m3\\Banshee.m3";EngineeringBay
        std::string meshname = "wasp01.m3";//"Banshee.m3";
		//fileName = "m3\\Banshee.m3";
		pMesh->Load("m3\\Banshee.m3","Banshee.m3");
		meshList.push_back(pMesh);
		//meshList[meshList.size() - 1]->Load(fileName.c_str(),meshname.c_str());
		/*while(FindNextFile(hFind, &FindFileData) != 0)
		{
			fileName = "Content\\models\\";
			fileName += FindFileData.cFileName;
			meshList.push_back(new m3_Mesh());
			meshList[meshList.size() - 1]->Load(fileName.c_str(),FindFileData.cFileName);
		}
	}*/

	//meshList.push_back(new m3_Mesh());
	//meshList[meshList.size() - 1]->Load("Content\\models\\Panter.3ds","Panter.3ds");

	//map = new m3_Heightmap();
	//map->Load("Content\\maps\\map.raw");

	m3_DDSLoader::glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)wglGetProcAddress("glCompressedTexImage2DARB");

	//m3_PostService::Load();
	//m3_Core::GetGUI()->SetCurrentMenu("main_menu");

}
void m3_init()
{
  if ( SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO) < 0 )
  { 
   printf("Unable to init SDL: %s\n", SDL_GetError()); 
   exit(1); 
  } 
  atexit(SDL_Quit); 
  if ( SDL_SetVideoMode(WINDOW_WIDTH,WINDOW_HEIGHT,32,SDL_OPENGL | !SDL_FULLSCREEN) == NULL )
  { 
    printf("Unable to set 640x480 video: %s\n", SDL_GetError()); 
    exit(1); 
  }

  //SDL_Surface* icon = SDL_LoadBMP("icon.bmp");
  //SDL_WM_SetIcon(icon, NULL);

  SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 0 );
  SDL_WM_SetCaption("m3_viewer", NULL);
  SDL_TimerID my_timer_id = SDL_AddTimer(1000, my_callbackfunc, my_callback_param);


  init();

}
void render( void )
{
	//m3_PostService::Update();
	m3_Core::GetCamera()->Update();

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if(m3_Input::meshIndex < 0)
		m3_Input::meshIndex = meshList.size() - 1;

	if(m3_Input::meshIndex >=  meshList.size())
		m3_Input::meshIndex = 0;

	if(m3_Input::saveToObj)
	{
		m3_Input::saveToObj = false;
		meshList[m3_Input::meshIndex]->Convert();
	}

	//m3_PostService::EnableRenderToTexture();
	if(m3_Input::meshIndex >=0 && m3_Input::meshIndex < meshList.size())
	{
		meshList[m3_Input::meshIndex]->Update();
		meshList[m3_Input::meshIndex]->Render();
	}
	//m3_PostService::DisableRenderToTexture();
	
	//m3_PostService::Render();

	glFlush();
  SDL_GL_SwapBuffers();
}
void m3_render()
{
  m3_Input::Update();
  render();
}