#include <windows.h>

#include "d3dx9.h"
#include <vector>

#include "m2/videosetting.h"
#include "m3/m3_setup.h"

// Function prototypes
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
BOOL DoInit(HWND hWnd);
LRESULT CALLBACK WndProc( HWND hWnd,     UINT uMsg,     WPARAM wParam,     LPARAM lParam) ;

HINSTANCE hInstance;
HWND hWnd;
HDC hDC;
HGLRC hRC;
extern bool supportVBO;

PIXELFORMATDESCRIPTOR pfd=     // /pfd ���ߴ���������ϣ���Ķ�����������ʹ�õ����ظ�ʽ
{
  sizeof(PIXELFORMATDESCRIPTOR),     // ������ʽ�������Ĵ�С
  1,        // �汾��
  PFD_DRAW_TO_WINDOW |      // ��ʽ֧�ִ���
  PFD_SUPPORT_OPENGL |      // ��ʽ����֧��OpenGL
  PFD_DOUBLEBUFFER,      // ����֧��˫����
  PFD_TYPE_RGBA,       // ���� RGBA ��ʽ
  16,        // ѡ��ɫ�����
  0, 0, 0, 0, 0, 0,      // ���Ե�ɫ��λ
  1,        // ��Alpha����
  0,        // ����Shift Bit
  0,        // ���ۼӻ���
  0, 0, 0, 0,       // ���Ծۼ�λ
  16,        // 16λ Z-���� (��Ȼ���)
  0,        // ���ɰ建��
  0,        // �޸�������
  PFD_MAIN_PLANE,       // ����ͼ��
  0,        // Reserved
  0, 0, 0        // ���Բ�����
};



GLvoid ReSizeGLScene(GLsizei width, GLsizei height)    // ����OpenGL���ڴ�С
{
	if (height==0)        // ��ֹ�����
	{
		height=1;       // ��Height��Ϊ1
	}

	glViewport(0, 0, width, height);     // ���õ�ǰ���ӿ�

	glMatrixMode(GL_PROJECTION);      // ѡ��ͶӰ����
	glLoadIdentity();       // ����ͶӰ����

	// �����ӿڵĴ�С
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,128.0f*5);
	gluLookAt(0.0,0.0,50,0.0,0.0,0.0,0.0,1.0,0.0);

	glMatrixMode(GL_MODELVIEW);      // ѡ��ģ�͹۲����
	glLoadIdentity();       // ����ģ�͹۲����
}


BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
  GLuint  PixelFormat;      // �������ƥ��Ľ��
  WNDCLASS wc;       // ������ṹ
  DWORD  dwExStyle;      // ��չ���ڷ��
  DWORD  dwStyle;      // ���ڷ��

  RECT WindowRect;       // ȡ�þ��ε����ϽǺ����½ǵ�����ֵ
  WindowRect.left=(long)0;      // ��Left   ��Ϊ 0
  WindowRect.right=(long)width;      // ��Right  ��ΪҪ��Ŀ��
  WindowRect.top=(long)0;       // ��Top    ��Ϊ 0
  WindowRect.bottom=(long)height;      // ��Bottom ��ΪҪ��ĸ߶�

  hInstance  = GetModuleHandle(NULL);   // ȡ�����Ǵ��ڵ�ʵ��
  wc.style  = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;  // �ƶ�ʱ�ػ�����Ϊ����ȡ��DC
  wc.lpfnWndProc  = (WNDPROC) WndProc;    // WndProc������Ϣ
  wc.cbClsExtra  = 0;      // �޶��ⴰ������
  wc.cbWndExtra  = 0;      // �޶��ⴰ������
  wc.hInstance  = hInstance;     // ����ʵ��
  wc.hIcon  = LoadIcon(NULL, IDI_WINLOGO);   // װ��ȱʡͼ��
  wc.hCursor  = LoadCursor(NULL, IDC_ARROW);   // װ�����ָ��
  wc.hbrBackground = NULL;      // GL����Ҫ����
  wc.lpszMenuName  = NULL;      // ����Ҫ�˵�
  wc.lpszClassName = "OpenG";     // �趨������
  if (!RegisterClass(&wc))      // ����ע�ᴰ����
  {
    MessageBox(NULL,"ע�ᴰ��ʧ��","����",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;       // �˳�������FALSE
  }

  {
  dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;   // ��չ������
  dwStyle=WS_OVERLAPPEDWINDOW;     //  ������
  }
  AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);  // �������ڴﵽ����Ҫ��Ĵ�С
  if (!(hWnd=CreateWindowEx( dwExStyle,    // ��չ������
     "OpenG",    // ������
     title,     // ���ڱ���
     WS_CLIPSIBLINGS |   // ����Ĵ���������
     WS_CLIPCHILDREN |   // ����Ĵ���������
     dwStyle,    // ѡ��Ĵ�������
     0, 0,     // ����λ��
     WindowRect.right-WindowRect.left, // ��������õĴ��ڿ��
     WindowRect.bottom-WindowRect.top, // ��������õĴ��ڸ߶�
     NULL,     // �޸�����
     NULL,     // �޲˵�
     hInstance,    // ʵ��
     NULL)))     // ����WM_CREATE�����κζ���
  {
    MessageBox(NULL,"���ܴ���һ�������豸������","����",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;       // ���� FALSE
  }

  wglMakeCurrent(NULL, NULL); 

  if (!(hDC=GetDC(hWnd)))       // ȡ���豸��������ô?
  {
    MessageBox(NULL,"���ܴ���һ����ƥ������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;       // ���� FALSE
  }
  if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))    // Windows �ҵ���Ӧ�����ظ�ʽ����?
  {
    MessageBox(NULL,"�����������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;       // ���� FALSE
  }
  if(!SetPixelFormat(hDC,PixelFormat,&pfd))    // �ܹ��������ظ�ʽô?
  {
    MessageBox(NULL,"�����������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;       // ���� FALSE
  }
  if (!(hRC=wglCreateContext(hDC)))     // �ܷ�ȡ����ɫ������?
  {
    MessageBox(NULL,"���ܴ���OpenGL��Ⱦ������","����",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;       // ���� FALSE
  }
  if(!wglMakeCurrent(hDC,hRC))      // ���Լ�����ɫ������
  {
    MessageBox(NULL,"���ܼ��ǰ��OpenGL��Ȼ������","����",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;       // ���� FALSE
  }
  ShowWindow(hWnd,SW_SHOW);      // ��ʾ����
  SetForegroundWindow(hWnd);      // ����������ȼ�
  SetFocus(hWnd);        // ���ü��̵Ľ������˴���
  ReSizeGLScene(width, height);      // ����͸�� GL ��Ļ

  return TRUE;        // �ɹ�
}



float modelsize;

#include "m2/quaternion.h"

Vec3D	m_vPosition = Vec3D(0.0f, 0.0f, 1.0f);
Vec3D	m_vViewDir = Vec3D(0.0f, 0.0f, -1.0f);
Vec3D	m_vRightVector = Vec3D(1.0f, 0.0f, 0.0f);
Vec3D	m_vUpVector = Vec3D(0.0f, 1.0f, 0.0f);

#define GL_BUFFER_OFFSET(i) ((char *)(0) + (i))
extern size_t globalTime;
extern DWORD lastTime;



BOOL DoInit(HWND hWnd)
{
  //TestModel();
  m3_init();
  return 1;
}

#include "wx/display.h"
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
  //wxDisplay *disp = new wxDisplay(0);
	//int bpp = disp->GetCurrentMode().bpp;
	MSG msg;        // Windowsx��Ϣ�ṹ
	if (!CreateGLWindow("OpenGL������",640,480,16,0))
	{
		return 0;       // ʧ���˳�
	}
  InitGLSetting();
  InitGL();
  DoInit(0);


  while(1)        // ����ѭ��ֱ�� done=TRUE
	{
     m3_render();
		//}
	}

  return 0;
}

LRESULT CALLBACK WndProc( HWND hWnd,     // ���ڵľ��
    UINT uMsg,     // ���ڵ���Ϣ
    WPARAM wParam,     // ���ӵ���Ϣ����
    LPARAM lParam)     // ���ӵ���Ϣ����
{
	switch (uMsg)        // ���Windows��Ϣ
	{
		case WM_ACTIVATE:      // ���Ӵ��ڼ�����Ϣ
		{
			return 0;      // ������Ϣѭ��
		}
		case WM_SYSCOMMAND:      // ϵͳ�ж�����
		{
			switch (wParam)      // ���ϵͳ����
			{
				case SC_SCREENSAVE:    // ����Ҫ����?
				case SC_MONITORPOWER:    // ��ʾ��Ҫ����ڵ�ģʽ?
				return 0;     // ��ֹ����
			}
			break;       // �˳�
		}
		case WM_CLOSE:       // �յ�Close��Ϣ?
		{
			PostQuitMessage(0);     // �����˳���Ϣ
			return 0;      // ����
		}
		case WM_KEYDOWN:      // �м�����ô?
		{
			return 0;      // ����
		}
		case WM_KEYUP:       // �м��ſ�ô?
		{
			return 0;      // ����
		}
	}
	// �� DefWindowProc��������δ�������Ϣ��
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}


