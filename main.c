/****************************************\
* Tout le projet 3DSnake, dont           *
* ce fichier est la source               *
* principale contenant la fonction main, *
* a été conçu et programmé par Victor    *
* Martin Lac.                             *
\****************************************/

#ifdef _DEBUG
#define DEBUGMODE
#endif

#include <stdlib.h>
#include <allegro.h>
#include <math.h>
#include "3DSnake.h"

extern BITMAP *Logo;
extern BITMAP *Buffer;

double DistancePersoEcran;
double *RowToDist;

extern struct level Level[];
extern int NbrLevels;

extern struct slot Slot[];

int main() {
	allegro_init();
	install_keyboard();
	install_timer();
	install_mouse();
	install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,NULL);
	{
		int Card=GFX_AUTODETECT_WINDOWED;
		int Width=640;
		int Height=480;
		int i;

		set_color_depth(32);
#ifndef _DEBUG
		set_gfx_mode(GFX_SAFE,320,200,0,0);
		if(!gfx_mode_select(&Card,&Width,&Height))
			return 1;
#endif
		if(set_gfx_mode(Card,Width,Height,0,0))
			return 1;
		RowToDist=malloc(SCREEN_H*sizeof(*RowToDist));
		if(RowToDist==NULL)
			return 1;
		DistancePersoEcran=SCREEN_W/2.0/tan(0.523598776);
		for(i=0;i<SCREEN_H;i++)
			if((double)i>(SCREEN_H-1.0)/2.0)
				RowToDist[i]=32*tan(1.57079633-atan((i-(SCREEN_H-1.0)/2.0)/DistancePersoEcran));
			else
				RowToDist[i]=32*tan(1.57079633-atan(((SCREEN_H-1.0)/2.0-i)/DistancePersoEcran));
		set_color_conversion(COLORCONV_TOTAL|COLORCONV_KEEP_TRANS);
	}

	textout_centre(screen,font,"Chargement en cours ...",SCREEN_W/2,SCREEN_H/2,makecol(0,255,0));
	if(!LoadCommon())
		return 1;
	InitGui();

#ifndef _DEBUG
	FadeInBitmap(Logo,1200);
	FadeOutBitmap(Logo,1200);
#endif

	while(Jouer());

	UnloadCommon();

	return 0;
} END_OF_MAIN()
