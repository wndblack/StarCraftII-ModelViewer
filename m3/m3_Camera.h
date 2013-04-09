#pragma once
#ifndef _M3_CAMERA_H_
#define _M3_CAMERA_H_
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "Vector3d.h"
#include "Matrix4x4.h"
#include "m3_Input.h"

class m3_Camera
{
	private :
		bool _moveCameraFlag;
	public :
		Vector3d Position;
		Vector3d Rotation;
		Matrix4x4 mView;

		m3_Camera();
		
		void ApplyViewMatrix();
		void Update();
};

#endif