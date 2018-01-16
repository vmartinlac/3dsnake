#ifdef _DEBUG
#define DEBUGMODE
#endif

#include <allegro.h>
#include "3DSnake.h"

int CouleurHaut;
int CouleurBas;
int CouleurNoir;
int CouleurBlanc;
int CouleurRouge;
int CouleurVert;
int CouleurBleu;

BITMAP *Buffer;
BITMAP *Logo;
BITMAP *Pause;
BITMAP *Fond;
BITMAP *Perdre;
BITMAP *Gagner;
BITMAP *Fin;
BITMAP *Locked;
FONT *Police;
double *zBuffer;

int MapWidth;
int MapHeight;
BITMAP *Map[MAPSIZEMAX];

int NbrTextures;
BITMAP *Texture[NBRTEXTURESMAX];
int NbrSons;
SAMPLE *Son[NBRSONSMAX];
int NbrArmes;
struct arme Arme[NBRARMESMAX];
int NbrEnnemiClasses;
struct ennemiclass EnnemiClass[NBRENNEMICLASSESMAX];
int NbrDecorClasses;
struct decorclass DecorClass[NBRDECORCLASSESMAX];
int NbrTirClasses;
struct tirclass TirClass[NBRTIRCLASSESMAX];
int NbrPorteClasses;
struct porteclass PorteClass[NBRPORTECLASSESMAX];
int NbrScripts;
struct script Script[NBRSCRIPTSMAX];
int NbrSprites;
struct sprite Sprite[NBRSPRITESMAX];

struct persoinfos Perso;

extern int LastMouseX;
extern volatile int FrameSignal;
extern volatile int FramesPerSecond;
extern volatile int Frames;
extern int NbrColorFlashes;

int Status;
void (*Moteur[3])()={Moteur3D,Moteur2D,MoteurPause};

struct level Level[NBRLEVELSMAX];
int NbrLevels;
int iLevel=0;

struct slot Slot[6];

//Renvoyer 1 pour rejouer et 0 pour quitter
int Jouer() {
	if(!Menu(0))
		return 0;

	NbrColorFlashes=0;
	Frames=0;
	FramesPerSecond=0;
	position_mouse(SCREEN_W/2,SCREEN_H/2);
	LastMouseX=mouse_x;
	install_int(MainTimer,1000);
	install_int(FrameTimer,1000/FPSMAX);
	for(Status=STATUS_JOUER3D;Status>=0;Moteur[Status]()) {
		while(!FrameSignal)
			rest(0);
		FrameSignal=0;
	}
	remove_int(MainTimer);
	remove_int(FrameTimer);

	UnloadData();

	switch(Status) {
		case STATUS_GAGNER:
			FadeOutBitmap(Buffer,1200);
			FadeInBitmap(Gagner,1200);
			clear_keybuf();
			readkey();
			FadeOutBitmap(Gagner,1200);
			if(iLevel+1<NbrLevels)
				Level[iLevel+1].Locked=0;
			else {
				FadeInBitmap(Fin,1200);
				clear_keybuf();
				readkey();
				FadeOutBitmap(Fin,1200);
			}
			return 1;
		case STATUS_PERDRE:
			FadeOutBitmap(Buffer,1200);
			FadeInBitmap(Perdre,1200);
			clear_keybuf();
			readkey();
			FadeOutBitmap(Perdre,1200);
			return 1;
		case STATUS_QUITTER:
			return 0;
		default:
			return 1;
	}
}