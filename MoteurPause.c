#ifdef _DEBUG
#define DEBUGMODE
#endif

#include <allegro.h>
#include "3DSnake.h"

extern BITMAP *Buffer;
extern BITMAP *Pause;
extern BITMAP *Fond;

extern volatile int Frames;
extern int Status;

void MoteurPause() {
	static int PauseXPos=0;
	static int PauseYPos=0;
	static int PauseXDir=5;
	static int PauseYDir=3;
	int PauseWidth=Pause->w*SCREEN_W/640;
	int PauseHeight=Pause->h*SCREEN_H/480;

	stretch_blit(Fond,Buffer,0,0,Fond->w,Fond->h,0,0,SCREEN_W,SCREEN_H);

	PauseXPos+=PauseXDir;
	if(PauseXPos<0) {
		PauseXPos=0;
		PauseXDir=-PauseXDir;
	}
	else if(PauseXPos+PauseWidth>SCREEN_W) {
		PauseXPos=SCREEN_W-PauseWidth;
		PauseXDir=-PauseXDir;
	}

	PauseYPos+=PauseYDir;
	if(PauseYPos<0) {
		PauseYPos=0;
		PauseYDir=-PauseYDir;
	}
	else if(PauseYPos+PauseHeight>SCREEN_H) {
		PauseYPos=SCREEN_H-PauseHeight;
		PauseYDir=-PauseYDir;
	}

	masked_stretch_blit(Pause,Buffer,0,0,Pause->w,Pause->h,PauseXPos,PauseYPos,PauseWidth,PauseHeight);

	vsync();
	blit(Buffer,screen,0,0,0,0,SCREEN_W,SCREEN_H);

	if(Status>=0) {
		if(key[KEY_F1])
			Status=STATUS_JOUER2D;
		else if(key[KEY_F2])
			Status=STATUS_JOUER3D;
	}
	if(key[KEY_ESC] && !Menu(1))
			Status=STATUS_QUITTER;
	Frames++;
}