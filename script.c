#ifdef _DEBUG
#define DEBUGMODE
#endif

#include <allegro.h>
#include <math.h>
#include "3DSnake.h"

extern struct sprite Sprite[NBRSPRITESMAX];
extern int NbrSprites;
extern struct persoinfos Perso;
extern int MapWidth;
extern BITMAP *Map[MAPSIZEMAX];

int InterpreterScript(struct script *Script) {
	int i;
	for(i=0;i<Script->NbrInstructions;i++)
		if(!Script->Instruction[i].OpFn(Script->Instruction[i].Param))
			return 0;
	return 1;
}

int OpFnNewEnnemi(union param Param[]) {
	if(NbrSprites>=NBRSPRITESMAX)
		return 0;
	Sprite[NbrSprites].PosX=Param[0].Double;
	Sprite[NbrSprites].PosY=Param[1].Double;
	Sprite[NbrSprites].Type=SPRITE_TYPE_ENNEMI;
	Sprite[NbrSprites].AutoRotating=1;
	Sprite[NbrSprites].Informations.Ennemi.Class=Param[2].EnnemiClass;
	Sprite[NbrSprites].Informations.Ennemi.Life=Sprite[NbrSprites].Informations.Ennemi.Class->StartLife;
	if(Param[3].Int) {
		Sprite[NbrSprites].Distance=hypot(Perso.Left-Sprite[NbrSprites].PosX,Perso.Top-Sprite[NbrSprites].PosY);
		play_sample(Sprite[NbrSprites].Informations.Ennemi.Class->SonVoyant,(Sprite[NbrSprites].Distance<800)?128+(800-Sprite[NbrSprites].Distance)*128/800:128,128,1200,0);
		if(!Marchant(NbrSprites))
			return 0;
	}
	else {
		Sprite[NbrSprites].Informations.Ennemi.Phase=ENNEMI_PHASE_ATTENDANT;
		Sprite[NbrSprites].Img=Sprite[NbrSprites].Informations.Ennemi.Class->ImgAttendant;
	}
	NbrSprites++;
	return 1;
}

int OpFnNewDecor(union param Param[]) {
	if(NbrSprites>=NBRSPRITESMAX)
		return 0;
	Sprite[NbrSprites].PosX=Param[0].Double;
	Sprite[NbrSprites].PosY=Param[1].Double;
	Sprite[NbrSprites].Type=SPRITE_TYPE_DECOR;
	Sprite[NbrSprites].Informations.Decor.Class=Param[2].DecorClass;
	Sprite[NbrSprites].Informations.Decor.iFrame=0;
	Sprite[NbrSprites].Img=Sprite[NbrSprites].Informations.Decor.Class->Frame[0];
	Sprite[NbrSprites].AutoRotating=Param[3].Int;
	Sprite[NbrSprites].Rot=Param[4].Double;
	NbrSprites++;
	return 1;
}

int OpFnSetMap(union param Param[]) {
	Map[Param[1].Int*MapWidth+Param[0].Int]=Param[2].Bitmap;
	return 1;
}
