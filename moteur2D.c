#ifdef _DEBUG
#define DEBUGMODE
#endif

#include <allegro.h>
#include <math.h>
#include "3DSnake.h"

extern BITMAP *Buffer;

extern struct persoinfos Perso;

extern int CouleurBas;
extern int CouleurNoir;
extern int CouleurVert;

extern int MapWidth;
extern int MapHeight;
extern BITMAP *Map[];

extern struct sprite Sprite[];
extern int NbrSprites;
extern int Status;

extern volatile int Frames;

void Moteur2D() {
	//On prépare l'écran
	{
		double CaseWidth=(double)SCREEN_W/(double)MapWidth;
		double CaseHeight=(double)SCREEN_H/(double)MapHeight;
		double mX;
		double mY;
		double mDist;
		double SinPersoRot;
		double CosPersoRot;
		int x,y;
		clear_bitmap(Buffer);
		for(x=0;x<MapWidth;x++)
			for(y=0;y<MapHeight;y++)
				if(Map[y*MapWidth+x]==NULL)
					rectfill(Buffer,x*CaseWidth,y*CaseHeight,(x+1)*CaseWidth-1,(y+1)*CaseHeight-1,CouleurBas);
				else {
					BITMAP *Text=Map[y*MapWidth+x];
					stretch_blit(Text,Buffer,0,0,Text->w,Text->h,x*CaseWidth,y*CaseHeight,CaseWidth,CaseHeight);
				}
		{
			int iSprite;
			for(iSprite=0;iSprite<NbrSprites;iSprite++) {
				double SpriteWidth=(Sprite[iSprite].Img->w*SCREEN_W)/(Sprite[iSprite].Img->h*MapWidth);
				double SpriteHeight=Sprite[iSprite].Img->h*SpriteWidth/Sprite[iSprite].Img->w;
				stretch_sprite(Buffer,Sprite[iSprite].Img,Sprite[iSprite].PosX*SCREEN_W/(64*MapWidth)-SpriteWidth/2,Sprite[iSprite].PosY*SCREEN_H/(64*MapHeight)-SpriteHeight,SpriteWidth,SpriteHeight);
			}
		}

		mDist=0;
		SinPersoRot=sin(Perso.Rot);
		CosPersoRot=cos(Perso.Rot);
		do {
			mDist++;
			mX=Perso.Left+mDist*SinPersoRot;
			mY=Perso.Top+mDist*CosPersoRot;
		} while(MAP(mX,mY)==NULL);
		line(Buffer,SCREEN_W*Perso.Left/(MapWidth*64),SCREEN_H*Perso.Top/(MapHeight*64),SCREEN_W*mX/(MapWidth*64),SCREEN_H*mY/(MapHeight*64),makecol(255,255,255));

		mDist=0;
		SinPersoRot=sin(Perso.Rot-0.523598776);
		CosPersoRot=cos(Perso.Rot-0.523598776);
		do {
			mDist++;
			mX=Perso.Left+mDist*SinPersoRot;
			mY=Perso.Top+mDist*CosPersoRot;
		} while(MAP(mX,mY)==NULL);
		line(Buffer,SCREEN_W*Perso.Left/(MapWidth*64),SCREEN_H*Perso.Top/(MapHeight*64),SCREEN_W*mX/(MapWidth*64),SCREEN_H*mY/(MapHeight*64),makecol(0,255,0));

		mDist=0;
		SinPersoRot=sin(Perso.Rot+0.523598776);
		CosPersoRot=cos(Perso.Rot+0.523598776);
		do {
			mDist++;
			mX=Perso.Left+mDist*SinPersoRot;
			mY=Perso.Top+mDist*CosPersoRot;
		} while(MAP(mX,mY)==NULL);
		line(Buffer,SCREEN_W*Perso.Left/(MapWidth*64),SCREEN_H*Perso.Top/(MapHeight*64),SCREEN_W*mX/(MapWidth*64),SCREEN_H*mY/(MapHeight*64),makecol(0,255,0));
		circlefill(Buffer,SCREEN_W*Perso.Left/(MapWidth*64),SCREEN_H*Perso.Top/(MapHeight*64),10,makecol(128,128,128));
	}
	if(mouse_b & 1) {
		circlefill(Buffer,mouse_x,mouse_y,5,CouleurNoir);
		textprintf_ex(Buffer,font,mouse_x,mouse_y,CouleurVert,CouleurNoir,"(%ld;%ld)",(int)mouse_x*MapWidth/SCREEN_W,(int)mouse_y*MapHeight/SCREEN_H);
	}

	//On affiche
	vsync();
	blit(Buffer,screen,0,0,0,0,SCREEN_W,SCREEN_H);

	{
		double RunSpeed=32;
		double RotSpeed=0.314159265;

		if(key[KEY_LEFT] && Perso.Left-RunSpeed>=0 && MAP(Perso.Left-RunSpeed,Perso.Top)==NULL)
			Perso.Left-=RunSpeed;
		else if(key[KEY_RIGHT] && Perso.Left+RunSpeed<MapWidth*64.0 && MAP(Perso.Left+RunSpeed,Perso.Top)==NULL)
			Perso.Left+=RunSpeed;
		if(key[KEY_UP] && Perso.Top-RunSpeed>=0 && MAP(Perso.Left,Perso.Top-RunSpeed)==NULL)
			Perso.Top-=RunSpeed;
		else if(key[KEY_DOWN] && Perso.Top+RunSpeed<MapHeight*64.0 && MAP(Perso.Left,Perso.Top+RunSpeed)==NULL)
			Perso.Top+=RunSpeed;
		if(key[KEY_X]) {
			Perso.Rot-=RotSpeed;
			if(Perso.Rot<0)
				Perso.Rot+=6.28318531;
		}
		else if(key[KEY_Z]) {
			Perso.Rot+=RotSpeed;
			if(Perso.Rot>6.28318531)
				Perso.Rot-=6.28318531;
		}
		if(Status>=0) {
			if(key[KEY_F2])
				Status=STATUS_JOUER3D;
			else if(key[KEY_F3])
				Status=STATUS_PAUSE;
		}
		if(key[KEY_ESC] && !Menu(1))
			Status=STATUS_QUITTER;
	}
	Frames++;
}