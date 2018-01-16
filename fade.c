#ifdef _DEBUG
#define DEBUGMODE
#endif

#include <allegro.h>
#include "3DSnake.h"

volatile int FadeElaspedTime;
void FadeTimer(void);

int FadeInBitmap(BITMAP *Bmp,int Duration) {
	BITMAP *ScrBuffer;
	int x,y;
	double Pourcentage;

	ScrBuffer=create_bitmap(SCREEN_W,SCREEN_H);
	if(ScrBuffer==NULL)
		return 0;
	FadeElaspedTime=0;
	if(install_int(FadeTimer,1))
		return 0;
	clear_bitmap(screen);
	while(FadeElaspedTime<Duration) {
		Pourcentage=(double)FadeElaspedTime/(double)Duration;
		clear_bitmap(ScrBuffer);
		for(x=0;x<SCREEN_W;x++) {
			for(y=0;y<SCREEN_H;y++) {
				int Pixel=((long *)Bmp->line[(int)y])[(int)x];
				((long *)ScrBuffer->line[(int)y])[(int)x]=LITPIXEL(Pixel,Pourcentage);
			}
		}
		vsync();
		blit(ScrBuffer,screen,0,0,0,0,SCREEN_W,SCREEN_H);
	}
	remove_int(FadeTimer);
	destroy_bitmap(ScrBuffer);
	return 1;
}

int FadeOutBitmap(BITMAP *Bmp,int Duration) {
	BITMAP *ScrBuffer;
	int x,y;
	double Pourcentage;

	ScrBuffer=create_bitmap(SCREEN_W,SCREEN_H);
	if(ScrBuffer==NULL)
		return 0;
	FadeElaspedTime=0;
	if(install_int(FadeTimer,1))
		return 0;
	while(FadeElaspedTime<Duration) {
		Pourcentage=1.0-(double)FadeElaspedTime/(double)Duration;
		clear_bitmap(ScrBuffer);
		for(x=0;x<SCREEN_W;x++) {
			for(y=0;y<SCREEN_H;y++) {
				int Pixel=((long *)Bmp->line[(int)y])[(int)x];
				((long *)ScrBuffer->line[(int)y])[(int)x]=LITPIXEL(Pixel,Pourcentage);
			}
		}
		vsync();
		blit(ScrBuffer,screen,0,0,0,0,SCREEN_W,SCREEN_H);
	}
	remove_int(FadeTimer);
	clear_bitmap(screen);
	destroy_bitmap(ScrBuffer);
	return 1;
}

void FadeTimer(void) {
	FadeElaspedTime++;
}