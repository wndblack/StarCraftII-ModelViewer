#include "m3_Core.h"

m3_Camera* m3_Core::_camera = 0;
bool m3_Core::_cameraSingleFlag = false;

m3_Camera* m3_Core::GetCamera()
{
	if(!_cameraSingleFlag)
	{
		_camera = new m3_Camera();
		_cameraSingleFlag = true;
		return _camera;
	}
	else
		return _camera;
}

/*
m3_GUIService* m3_Core::_GUIService = NULL;
bool m3_Core::_GUISingleFlag = false;

m3_GUIService* m3_Core::GetGUI()
{
	if(!_GUISingleFlag)
	{
		_GUIService = new m3_GUIService();
		_GUISingleFlag = true;
		return _GUIService;
	}
	else
		return _GUIService;
}*/