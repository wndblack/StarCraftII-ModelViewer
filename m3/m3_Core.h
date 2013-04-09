#pragma once
#ifndef _M3_CORE_H_
#define _M3_CORE_H_
#include "m3_Camera.h"
//#include "m3_GUIService.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

class m3_Core
{
	private:
		static m3_Camera* _camera;
		static bool _cameraSingleFlag;

		//static m3_GUIService* _GUIService;
		//static  bool _GUISingleFlag;
	public:
		static m3_Camera* GetCamera();
		//static m3_GUIService* GetGUI();

};
#endif