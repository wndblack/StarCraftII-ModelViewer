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

PIXELFORMATDESCRIPTOR pfd=     // /pfd 告诉窗口我们所希望的东东，即窗口使用的像素格式
{
  sizeof(PIXELFORMATDESCRIPTOR),     // 上述格式描述符的大小
  1,        // 版本号
  PFD_DRAW_TO_WINDOW |      // 格式支持窗口
  PFD_SUPPORT_OPENGL |      // 格式必须支持OpenGL
  PFD_DOUBLEBUFFER,      // 必须支持双缓冲
  PFD_TYPE_RGBA,       // 申请 RGBA 格式
  16,        // 选定色彩深度
  0, 0, 0, 0, 0, 0,      // 忽略的色彩位
  1,        // 无Alpha缓存
  0,        // 忽略Shift Bit
  0,        // 无累加缓存
  0, 0, 0, 0,       // 忽略聚集位
  16,        // 16位 Z-缓存 (深度缓存)
  0,        // 无蒙板缓存
  0,        // 无辅助缓存
  PFD_MAIN_PLANE,       // 主绘图层
  0,        // Reserved
  0, 0, 0        // 忽略层遮罩
};



GLvoid ReSizeGLScene(GLsizei width, GLsizei height)    // 重置OpenGL窗口大小
{
	if (height==0)        // 防止被零除
	{
		height=1;       // 将Height设为1
	}

	glViewport(0, 0, width, height);     // 重置当前的视口

	glMatrixMode(GL_PROJECTION);      // 选择投影矩阵
	glLoadIdentity();       // 重置投影矩阵

	// 设置视口的大小
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,128.0f*5);
	gluLookAt(0.0,0.0,50,0.0,0.0,0.0,0.0,1.0,0.0);

	glMatrixMode(GL_MODELVIEW);      // 选择模型观察矩阵
	glLoadIdentity();       // 重置模型观察矩阵
}


BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
  GLuint  PixelFormat;      // 保存查找匹配的结果
  WNDCLASS wc;       // 窗口类结构
  DWORD  dwExStyle;      // 扩展窗口风格
  DWORD  dwStyle;      // 窗口风格

  RECT WindowRect;       // 取得矩形的左上角和右下角的坐标值
  WindowRect.left=(long)0;      // 将Left   设为 0
  WindowRect.right=(long)width;      // 将Right  设为要求的宽度
  WindowRect.top=(long)0;       // 将Top    设为 0
  WindowRect.bottom=(long)height;      // 将Bottom 设为要求的高度

  hInstance  = GetModuleHandle(NULL);   // 取得我们窗口的实例
  wc.style  = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;  // 移动时重画，并为窗口取得DC
  wc.lpfnWndProc  = (WNDPROC) WndProc;    // WndProc处理消息
  wc.cbClsExtra  = 0;      // 无额外窗口数据
  wc.cbWndExtra  = 0;      // 无额外窗口数据
  wc.hInstance  = hInstance;     // 设置实例
  wc.hIcon  = LoadIcon(NULL, IDI_WINLOGO);   // 装入缺省图标
  wc.hCursor  = LoadCursor(NULL, IDC_ARROW);   // 装入鼠标指针
  wc.hbrBackground = NULL;      // GL不需要背景
  wc.lpszMenuName  = NULL;      // 不需要菜单
  wc.lpszClassName = "OpenG";     // 设定类名字
  if (!RegisterClass(&wc))      // 尝试注册窗口类
  {
    MessageBox(NULL,"注册窗口失败","错误",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;       // 退出并返回FALSE
  }

  {
  dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;   // 扩展窗体风格
  dwStyle=WS_OVERLAPPEDWINDOW;     //  窗体风格
  }
  AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);  // 调整窗口达到真正要求的大小
  if (!(hWnd=CreateWindowEx( dwExStyle,    // 扩展窗体风格
     "OpenG",    // 类名字
     title,     // 窗口标题
     WS_CLIPSIBLINGS |   // 必须的窗体风格属性
     WS_CLIPCHILDREN |   // 必须的窗体风格属性
     dwStyle,    // 选择的窗体属性
     0, 0,     // 窗口位置
     WindowRect.right-WindowRect.left, // 计算调整好的窗口宽度
     WindowRect.bottom-WindowRect.top, // 计算调整好的窗口高度
     NULL,     // 无父窗口
     NULL,     // 无菜单
     hInstance,    // 实例
     NULL)))     // 不向WM_CREATE传递任何东东
  {
    MessageBox(NULL,"不能创建一个窗口设备描述表","错误",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;       // 返回 FALSE
  }

  wglMakeCurrent(NULL, NULL); 

  if (!(hDC=GetDC(hWnd)))       // 取得设备描述表了么?
  {
    MessageBox(NULL,"不能创建一种相匹配的像素格式","错误",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;       // 返回 FALSE
  }
  if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))    // Windows 找到相应的象素格式了吗?
  {
    MessageBox(NULL,"不能设置像素格式","错误",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;       // 返回 FALSE
  }
  if(!SetPixelFormat(hDC,PixelFormat,&pfd))    // 能够设置象素格式么?
  {
    MessageBox(NULL,"不能设置像素格式","错误",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;       // 返回 FALSE
  }
  if (!(hRC=wglCreateContext(hDC)))     // 能否取得着色描述表?
  {
    MessageBox(NULL,"不能创建OpenGL渲染描述表","错误",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;       // 返回 FALSE
  }
  if(!wglMakeCurrent(hDC,hRC))      // 尝试激活着色描述表
  {
    MessageBox(NULL,"不能激活当前的OpenGL渲然描述表","错误",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;       // 返回 FALSE
  }
  ShowWindow(hWnd,SW_SHOW);      // 显示窗口
  SetForegroundWindow(hWnd);      // 略略提高优先级
  SetFocus(hWnd);        // 设置键盘的焦点至此窗口
  ReSizeGLScene(width, height);      // 设置透视 GL 屏幕

  return TRUE;        // 成功
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
	MSG msg;        // Windowsx消息结构
	if (!CreateGLWindow("OpenGL程序框架",640,480,16,0))
	{
		return 0;       // 失败退出
	}
  InitGLSetting();
  InitGL();
  DoInit(0);


  while(1)        // 保持循环直到 done=TRUE
	{
     m3_render();
		//}
	}

  return 0;
}

LRESULT CALLBACK WndProc( HWND hWnd,     // 窗口的句柄
    UINT uMsg,     // 窗口的消息
    WPARAM wParam,     // 附加的消息内容
    LPARAM lParam)     // 附加的消息内容
{
	switch (uMsg)        // 检查Windows消息
	{
		case WM_ACTIVATE:      // 监视窗口激活消息
		{
			return 0;      // 返回消息循环
		}
		case WM_SYSCOMMAND:      // 系统中断命令
		{
			switch (wParam)      // 检查系统调用
			{
				case SC_SCREENSAVE:    // 屏保要运行?
				case SC_MONITORPOWER:    // 显示器要进入节电模式?
				return 0;     // 阻止发生
			}
			break;       // 退出
		}
		case WM_CLOSE:       // 收到Close消息?
		{
			PostQuitMessage(0);     // 发出退出消息
			return 0;      // 返回
		}
		case WM_KEYDOWN:      // 有键按下么?
		{
			return 0;      // 返回
		}
		case WM_KEYUP:       // 有键放开么?
		{
			return 0;      // 返回
		}
	}
	// 向 DefWindowProc传递所有未处理的消息。
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}


