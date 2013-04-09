#include "m3_Input.h"

int		 m3_Input::mWheel		     = -7;
float    m3_Input::mUpPosition       = -0.5f;
POINT    m3_Input::mLastPosition     = POINT();
POINT    m3_Input::mCurrentPosition  = POINT();
POINT    m3_Input::mDeltaPosition    = POINT();
bool	 m3_Input::mRightClick   = false;
bool	 m3_Input::mRightDown	 = false;
bool	 m3_Input::mLeftClick	 = false;
bool	 m3_Input::mLeftDown	 = false;

bool	 m3_Input::enableLight	 = false;
bool	 m3_Input::enableNormal	 = false;
bool	 m3_Input::saveToObj	 = false;
int		 m3_Input::meshIndex = 0;
SDL_Event    m3_Input::event     = SDL_Event(); 

bool     m3_Input::ePressForward  = false;
bool     m3_Input::ePressBackward = false;
bool     m3_Input::ePressLeft     = false;
bool     m3_Input::ePressRight    = false;
bool     m3_Input::ePressRotateLeft  = false;
bool     m3_Input::ePressRotateRight = false;

bool	 m3_Input::exitEvent    	 = false;

void m3_Input::Update()
{
	while ( SDL_PollEvent(&event) )
	{ 
		switch(event.type)
		{
			case SDL_QUIT:
				exit(0);
			break;

			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						exit(0);
					break;

					case SDLK_TAB:
						meshIndex++;
					break;

					case SDLK_SPACE:
						saveToObj = true;
					break;

					case SDLK_w :
						ePressForward = true;
					break;

					case SDLK_s :
						ePressBackward = true;
					break;

					case SDLK_a :
						ePressLeft = true;
					break;

					case SDLK_d :
						ePressRight = true;
					break;

					case SDLK_q :
						ePressRotateLeft = true;
					break;
					
					case SDLK_e :
						ePressRotateRight = true;
					break;

				} 
			break;

			case SDL_KEYUP:
				switch(event.key.keysym.sym)
				{
					case SDLK_w :
						ePressForward = false;
					break;

					case SDLK_s :
						ePressBackward = false;
					break;

					case SDLK_a :
						ePressLeft = false;
					break;

					case SDLK_d :
						ePressRight = false;
					break;

					case SDLK_q :
						ePressRotateLeft = false;
					break;
					
					case SDLK_e :
						ePressRotateRight = false;
					break;
				}
			break;

			case SDL_MOUSEMOTION:
				mCurrentPosition.x = event.motion.x;
				mCurrentPosition.y = event.motion.y;
			break;

			case SDL_MOUSEBUTTONDOWN:
				mLeftDown  = true;
				mLeftClick = true;
				mLastPosition.x = mCurrentPosition.x = event.button.x;
				mLastPosition.y = mCurrentPosition.y = event.button.y;
			break;

			case SDL_MOUSEBUTTONUP:
				switch(event.button.button)
				{
					case SDL_BUTTON_WHEELUP:
						if(mWheel > -15) 
							mWheel -= 1;
                        break;

					case SDL_BUTTON_WHEELDOWN:
						if(mWheel < -1)
							mWheel += 1;
						break;
				}
				mLeftClick = false;
				mLeftDown  = false;
			break;	
		}
	} 
}

