#include "m3_Camera.h"

m3_Camera::m3_Camera()
{
	_moveCameraFlag = false;
	Position = Vector3d(0.0f,0.0f,0.0f);
	Rotation = Vector3d(0.0f,0.0f,0.0f);
}

void m3_Camera::ApplyViewMatrix()
{
	mView = MatrixView(Vector3d(0.0f,0.0f,0.0f),Vector3d(0.0f,0.0f,m3_Input::mWheel),Vector3d(0.0f,1.0f,0.0f));
}

void m3_Camera::Update()
{
	float xrotrad, yrotrad;

	if(m3_Input::ePressForward && _moveCameraFlag)
	{
		yrotrad = (Rotation.y / 180 * 3.141592654f);
		xrotrad = (Rotation.x / 180 * 3.141592654f);
		Position.x += (float)(sin(yrotrad));
		Position.z -= (float)(cos(yrotrad));
		Position.y -= (float)(sin(xrotrad));
	}
	if(m3_Input::ePressBackward && _moveCameraFlag)
	{
		yrotrad = (Rotation.y / 180 * 3.141592654f);
		xrotrad = (Rotation.x / 180 * 3.141592654f);
		Position.x -= (float)(sin(yrotrad));
		Position.z += (float)(cos(yrotrad));
		Position.y += (float)(sin(xrotrad));
	}
	if(m3_Input::ePressLeft && _moveCameraFlag)
	{
		yrotrad = (Rotation.y / 180 * 3.141592654f);
		Position.x -= (float)(cos(yrotrad)) * 0.2f;
		Position.z -= (float)(sin(yrotrad)) * 0.2f;
	}
	if(m3_Input::ePressRight && _moveCameraFlag)
	{
		yrotrad = (Rotation.y / 180 * 3.141592654f);
		Position.x += (float)(cos(yrotrad)) * 0.2f;
		Position.z += (float)(sin(yrotrad)) * 0.2f;
	}

	if(m3_Input::ePressRotateLeft && _moveCameraFlag)
	{
		Rotation.x += 1.0f;
		if ( Rotation.x > 360.0f ) Rotation.x -= 360.0f;
	}

	if( m3_Input::ePressRotateRight && _moveCameraFlag)
	{
		Rotation.x -= 1.0f;
		if ( Rotation.x < -360.0f ) Rotation.x += 360.0f;
	}

	/*if( m3_Input::mLeftDown )
	{
		int diffx = m3_Input::mCurrentPosition.x - m3_Input::mLastPosition.x; 
		int diffy = m3_Input::mCurrentPosition.y - m3_Input::mLastPosition.y; 

		m3_Input::mLastPosition.x = m3_Input::mCurrentPosition.x;
		m3_Input::mLastPosition.y = m3_Input::mCurrentPosition.y;

		Rotation.y += (float) diffx/100.0f;
		Rotation.x += (float) diffy/100.0f;
	}*/
}