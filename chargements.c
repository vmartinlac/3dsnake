#ifdef _DEBUG
#define DEBUGMODE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <allegro.h>
#include "3DSnake.h"

#define GLOBALFILE "global.txt"
#define LOCALFILE "local.txt"

#define SF_1 "slot1.txt"
#define SF_2 "slot2.txt"
#define SF_3 "slot3.txt"
#define SF_4 "slot4.txt"
#define SF_5 "slot5.txt"

#define TXTBUFFERSIZE 2048

int ResLstAnalyser(char *Line);
int LevelAnalyser(char *Line);
int Slot0Analyser(char *Line);
int Slot1Analyser(char *Line);
int Slot2Analyser(char *Line);
int Slot3Analyser(char *Line);
int Slot4Analyser(char *Line);
int SlotAnalyserHelper(char *Line,int iSlot);
int ProgressAnalyser(char *Line);
int TextureAnalyser(char *Line);
int ScriptAnalyser(char *Line);
int ArmeAnalyser(char *Line);
int SonAnalyser(char *Line);
int MapAnalyser(char *Line);
int DecorClassAnalyser(char *Line);
int TirClassAnalyser(char *Line);
int PorteClassAnalyser(char *Line);
int EnnemiClassAnalyser(char *Line);
int SpriteAnalyser(char *Line);
int PersoInfosAnalyser(char *Line);

enum portephase StrToPortePhase(char *Str);
enum spritetype StrToSpriteType(char *Str);
enum objettype StrToObjetType(char *Str);
enum ennemiphase StrToEnnemiPhase(char *Str);
char *PortePhaseToStr(enum portephase PortePhase);
char *SpriteTypeToStr(enum spritetype SpriteType);
char *ObjetTypeToStr(enum objettype ObjetType);
char *EnnemiPhaseToStr(enum ennemiphase EnnemiPhase);

extern int MapWidth;
extern int MapHeight;
extern BITMAP *Map[MAPSIZEMAX];

extern int NbrTextures;
extern BITMAP *Texture[];
extern int NbrScripts;
extern struct script Script[];
extern int NbrSons;
extern SAMPLE *Son[];
extern int NbrArmes;
extern struct arme Arme[NBRARMESMAX];
extern int NbrEnnemiClasses;
extern struct ennemiclass EnnemiClass[];
extern int NbrDecorClasses;
extern struct decorclass DecorClass[];
extern int NbrTirClasses;
extern struct tirclass TirClass[];
extern int NbrPorteClasses;
extern struct porteclass PorteClass[];
extern int NbrSprites;
extern struct sprite Sprite[];
extern int NbrLevels;
extern int iLevel;
extern struct level Level[];
extern struct persoinfos Perso;

extern struct slot Slot[5];

extern BITMAP *Buffer;
extern BITMAP *Logo;
extern BITMAP *Pause;
extern BITMAP *Fond;
extern BITMAP *Perdre;
extern BITMAP *Gagner;
extern BITMAP *Locked;
extern BITMAP *Fin;
extern FONT *Police;
extern double *zBuffer;

extern int CouleurHaut;
extern int CouleurBas;
extern int CouleurNoir;
extern int CouleurBlanc;
extern int CouleurRouge;
extern int CouleurVert;
extern int CouleurBleu;

int LoadCommon() {
	int LineNumber=0;
	char Line[TXTBUFFERSIZE];
	PACKFILE *Fichier;
	int (*Analyser)(char *);

	//Global datas
	Fichier=pack_fopen(GLOBALFILE,"r");
	if(Fichier==NULL)
		return 0;
	NbrLevels=0;
	NbrArmes=0;
	Analyser=NULL;
	while(pack_fgets(Line,TXTBUFFERSIZE,Fichier)) {
		LineNumber++;
		if(Line[0]=='#' || Line[0]==0)
			continue;
		else if(!strcmp(Line,"[Level]")) {
			NbrLevels++;
			if(NbrLevels>NBRLEVELSMAX) {
				pack_fclose(Fichier);
				return 0;
			}
			Analyser=LevelAnalyser;
		}
		else if(!strcmp(Line,"[ResLst]"))
			Analyser=ResLstAnalyser;
		else if(!strcmp(Line,"[Arme]")) {
			NbrArmes++;
			if(NbrArmes>NBRARMESMAX) {
				pack_fclose(Fichier);
				return 0;
			}
			Analyser=ArmeAnalyser;
		}
		else if(Analyser) {
			if(!Analyser(Line)) {
				pack_fclose(Fichier);
				return 0;
			}
		}
		else {
			pack_fclose(Fichier);
			return 0;
		}
	}
	pack_fclose(Fichier);

	//Local datas
	Fichier=pack_fopen(LOCALFILE,"r");
	if(Fichier==NULL) {
		int i;
		for(i=1;i<NbrLevels;i++)
			Level[i].Locked=1;
		if(0<NbrLevels)
			Level[0].Locked=0;
		for(i=0;i<5;i++)
			Slot[i].Existing=0;
	}
	else {
		Analyser=NULL;
		while(pack_fgets(Line,TXTBUFFERSIZE,Fichier)) {
			if(Line[0]=='#' || Line[0]==0)
				continue;
			else if(!strcmp(Line,"[Progress]"))
				Analyser=ProgressAnalyser;
			else if(!strcmp(Line,"[Slot1]"))
				Analyser=Slot0Analyser;
			else if(!strcmp(Line,"[Slot2]"))
				Analyser=Slot1Analyser;
			else if(!strcmp(Line,"[Slot3]"))
				Analyser=Slot2Analyser;
			else if(!strcmp(Line,"[Slot4]")) 
				Analyser=Slot3Analyser;
			else if(!strcmp(Line,"[Slot5]"))
				Analyser=Slot4Analyser;
			else if(Analyser) {
				if(!Analyser(Line)) {
					pack_fclose(Fichier);
					return 0;
				}
			}
			else {
				pack_fclose(Fichier);
				return 0;
			}	
		}
	}
	pack_fclose(Fichier);

	//Variables générales
	zBuffer=malloc(SCREEN_H*sizeof(*zBuffer));
	if(zBuffer==NULL)
		return 0;
	Buffer=create_bitmap(SCREEN_W,SCREEN_H);
	if(Buffer==NULL)
		return 0;
	CouleurHaut=makecol(128,128,128);
	CouleurBas=makecol(192,192,192);
	CouleurBlanc=makecol(224,224,224);
	CouleurNoir=makecol(0,0,0);
	CouleurRouge=makecol(255,0,0);
	CouleurVert=makecol(0,255,0);
	CouleurBleu=makecol(0,0,255);
	srand(time(NULL));

	return 1;
}

int UnloadCommon() {
	PACKFILE *Fichier;
	int i;
	char TmpStr[TXTBUFFERSIZE];

	Fichier=pack_fopen(LOCALFILE,"w");
	if(Fichier==NULL)
		return 0;
	pack_fputs("[Progress]\n",Fichier);
	for(i=0;i<NbrLevels;i++) {
		snprintf(TmpStr,TXTBUFFERSIZE,"Level%ld=%ld\n",i,!Level[i].Locked);
		pack_fputs(TmpStr,Fichier);
	}
	for(i=0;i<5;i++) {
		snprintf(TmpStr,TXTBUFFERSIZE,"[Slot%ld]\n",i+1);
		pack_fputs(TmpStr,Fichier);
		if(Slot[i].Existing) {
			pack_fputs("Existing=1\n",Fichier);
			snprintf(TmpStr,TXTBUFFERSIZE,"iLevel=%ld\n",Slot[i].iLevel);
			pack_fputs(TmpStr,Fichier);
			snprintf(TmpStr,TXTBUFFERSIZE,"Time=%ld\n",Slot[i].Time);
			pack_fputs(TmpStr,Fichier);
		}
		else
			pack_fputs("Existing=0\n",Fichier);
	}
	pack_fclose(Fichier);

	destroy_bitmap(Locked);
	destroy_bitmap(Pause);
	destroy_bitmap(Fond);
	destroy_bitmap(Logo);
	destroy_bitmap(Gagner);
	destroy_bitmap(Perdre);
	destroy_bitmap(Fin);
	destroy_font(Police);
	destroy_sample(Perso.SonTrouvant);
	destroy_sample(Perso.SonBlesse);
	destroy_sample(Perso.SonSoigne);
	destroy_sample(Perso.SonChargeant);
	destroy_sample(Perso.SonMourant);
	destroy_bitmap(Buffer);
	free(zBuffer);

	for(i=0;i<NbrLevels;i++)
		destroy_bitmap(Level[i].Miniature);

	for(i=0;i<NbrArmes;i++) {
		destroy_bitmap(Arme[i].ImgTirant);
		destroy_bitmap(Arme[i].ImgVisant);
		destroy_sample(Arme[i].SonTirant);
	}

	return 1;
}

int LoadDataFromLF(char *Filename) {
	int (*Analyser)(char *);
	char Line[TXTBUFFERSIZE];
	PACKFILE *Fichier;

	Fichier=pack_fopen(Filename,"r");
	if(Fichier==NULL)
		return 0;
	NbrTextures=0;
	NbrSons=0;
	MapWidth=MapHeight=0;
	NbrDecorClasses=0;
	NbrTirClasses=0;
	NbrEnnemiClasses=0;
	NbrPorteClasses=0;
	NbrSprites=0;
	NbrScripts=0;
	Analyser=0;
	while(pack_fgets(Line,230,Fichier)) {
		if(Line[0]=='#' || Line[0]==0)
			continue;
		else if(!strcmp(Line,"[Texture]"))
			Analyser=TextureAnalyser;
		else if(!strcmp(Line,"[Son]"))
			Analyser=SonAnalyser;
		else if(!strcmp(Line,"[Map]"))
			Analyser=MapAnalyser;
		else if(!strcmp(Line,"[DecorClass]")) {
			if(NbrDecorClasses>=NBRDECORCLASSESMAX)
				return 0;
			Analyser=DecorClassAnalyser;
			NbrDecorClasses++;
		}
		else if(!strcmp(Line,"[TirClass]")) {
			if(NbrTirClasses>=NBRTIRCLASSESMAX)
				return 0;
			Analyser=TirClassAnalyser;
			NbrTirClasses++;
		}
		else if(!strcmp(Line,"[EnnemiClass]")) {
			if(NbrEnnemiClasses>=NBRENNEMICLASSESMAX)
				return 0;
			Analyser=EnnemiClassAnalyser;
			NbrEnnemiClasses++;
		}
		else if(!strcmp(Line,"[PorteClass]")) {
			if(NbrPorteClasses>=NBRPORTECLASSESMAX)
				return 0;
			Analyser=PorteClassAnalyser;
			NbrPorteClasses++;
		}
		else if(!strcmp(Line,"[Script]")) {
			if(NbrScripts>=NBRSCRIPTSMAX)
				return 0;
			Analyser=ScriptAnalyser;
			Script[NbrScripts].NbrInstructions=0;
			NbrScripts++;
		}
		else if(!strcmp(Line,"[Sprite]")) {
			if(NbrSprites>=NBRSPRITESMAX)
				return 0;
			Analyser=SpriteAnalyser;
			NbrSprites++;
		}
		else if(!strcmp(Line,"[PersoInfos]"))
			Analyser=PersoInfosAnalyser;
		else if(Analyser) {
			if(!Analyser(Line)) {
				pack_fclose(Fichier);
				return 0;
			}
		}
		else {
			pack_fclose(Fichier);
			return 0;
		}
	}

	return 1;
}

int LoadDataFromSF(int iSlot) {
	PACKFILE *Fichier;
	int (*Analyser)(char *);
	char Line[TXTBUFFERSIZE];

	if(!LoadDataFromLF(Level[Slot[iSlot].iLevel].Path))
		return 0;

	switch(iSlot) {
		case 0:
			Fichier=pack_fopen(SF_1,"r");
			break;
		case 1:
			Fichier=pack_fopen(SF_2,"r");
			break;
		case 2:
			Fichier=pack_fopen(SF_3,"r");
			break;
		case 3:
			Fichier=pack_fopen(SF_4,"r");
			break;
		case 4:
			Fichier=pack_fopen(SF_5,"r");
			break;
		default:
			Fichier=NULL;
	}
	if(Fichier==NULL)
		return 0;
	NbrSprites=0;
	MapWidth=MapHeight=0;
	Analyser=NULL;
	while(pack_fgets(Line,TXTBUFFERSIZE,Fichier)) {
		if(Line[0]=='#' || Line[0]==0)
			continue;
		else if(!strcmp(Line,"[Map]")) {
			MapWidth=MapHeight=0;
			Analyser=MapAnalyser;
		}
		else if(!strcmp(Line,"[Sprite]")) {
			if(NbrSprites>=NBRSPRITESMAX)
				return 0;
			Analyser=SpriteAnalyser;
			NbrSprites++;
		}
		else if(!strcmp(Line,"[PersoInfos]"))
			Analyser=PersoInfosAnalyser;
		else if(Analyser) {
			if(!Analyser(Line)) {
				pack_fclose(Fichier);
				return 0;
			}
		}
		else {
			pack_fclose(Fichier);
			return 0;
		}
	}
	return 1;
}

int SaveDataToSF(int iSlot) {
	PACKFILE *Fichier;
	int i,j,k;
	char TmpStr[TXTBUFFERSIZE];

	switch(iSlot) {
		case 0:
			Fichier=pack_fopen(SF_1,"w");
			break;
		case 1:
			Fichier=pack_fopen(SF_2,"w");
			break;
		case 2:
			Fichier=pack_fopen(SF_3,"w");
			break;
		case 3:
			Fichier=pack_fopen(SF_4,"w");
			break;
		case 4:
			Fichier=pack_fopen(SF_5,"w");
			break;
		default:
			Fichier=NULL;
	}
	if(Fichier==NULL)
		return 0;
	pack_fputs("[Map]\n",Fichier);
	for(i=0;i<MapHeight;i++) {
		char LineBuffer[TXTBUFFERSIZE];
		for(j=0;j<MapWidth;j++) {
			if(Map[MapWidth*i+j]) {
				for(k=0;Texture[k]!=Map[MapWidth*i+j];k++) {
					if(k>9 || k>=NbrTextures) {
						pack_fclose(Fichier);
						return 0;
					}
				}
				LineBuffer[j]='0'+k;
			}
			else
				LineBuffer[j]=' ';
		}
		LineBuffer[j++]='\n';
		LineBuffer[j]=0;
		pack_fputs(LineBuffer,Fichier);
	}
	for(i=0;i<NbrSprites;i++) {
		pack_fputs("[Sprite]\n",Fichier);
		for(j=0;Texture[j]!=Sprite[i].Img;j++) {
			if(j>=NbrTextures) {
				pack_fclose(Fichier);
				return 0;
			}
		}
		snprintf(TmpStr,TXTBUFFERSIZE,"Img=%ld\nPosX=%lf\nPosY=%lf\nRot=%lf\nAutoRotating=%ld\nType=%s\n",j,Sprite[i].PosX,Sprite[i].PosY,Sprite[i].Rot,Sprite[i].AutoRotating,SpriteTypeToStr(Sprite[i].Type));
		pack_fputs(TmpStr,Fichier);
		switch(Sprite[i].Type) {
			case SPRITE_TYPE_DECOR:
				for(j=0;&DecorClass[j]!=Sprite[i].Informations.Decor.Class;j++) {
					if(j>=NbrDecorClasses) {
						pack_fclose(Fichier);
						return 0;
					}
				}
				snprintf(TmpStr,TXTBUFFERSIZE,"DecorClass=%ld\nDecoriFrame=%ld\n",j,Sprite[i].Informations.Decor.iFrame);
				pack_fputs(TmpStr,Fichier);
				break;
			case SPRITE_TYPE_ENNEMI:
				for(j=0;&EnnemiClass[j]!=Sprite[i].Informations.Ennemi.Class;j++) {
					if(j>=NbrEnnemiClasses) {
						pack_fclose(Fichier);
						return 0;
					}
				}
				snprintf(TmpStr,TXTBUFFERSIZE,"EnnemiAngleDeMarche=%lf\nEnnemiClass=%ld\nEnnemiDelayer=%ld\nEnnemiDistDeMarche=%lf\nEnnemiLife=%ld\nEnnemiPhase=%s\n",Sprite[i].Informations.Ennemi.AngleDeMarche,j,Sprite[i].Informations.Ennemi.Delayer,Sprite[i].Informations.Ennemi.DistDeMarche,Sprite[i].Informations.Ennemi.Life,EnnemiPhaseToStr(Sprite[i].Informations.Ennemi.Phase));
				pack_fputs(TmpStr,Fichier);
				break;
			case SPRITE_TYPE_TIR:
				for(j=0;&TirClass[j]!=Sprite[i].Informations.Tir.Class;j++) {
					if(j>=NbrTirClasses) {
						pack_fclose(Fichier);
						return 0;
					}
				}
				snprintf(TmpStr,TXTBUFFERSIZE,"TirAngle=%lf\nTirClass=%ld\nTirRicochesAvantDestruction=%ld\nTirToDelete=%ld\n",Sprite[i].Informations.Tir.Angle,j,Sprite[i].Informations.Tir.RicochesAvantDestruction,Sprite[i].Informations.Tir.ToDelete);
				pack_fputs(TmpStr,Fichier);
				break;
			case SPRITE_TYPE_OBJET:
				snprintf(TmpStr,TXTBUFFERSIZE,"ObjetType=%s\n",ObjetTypeToStr(Sprite[i].Informations.Objet.Type));
				pack_fputs(TmpStr,Fichier);
				break;
			case SPRITE_TYPE_INTERRUPTEUR:
				snprintf(TmpStr,TXTBUFFERSIZE,"InterrupteurAutoFlipping=%ld\nInterrupteurNbrStates=%ld\nInterrupteurState=%ld\n",Sprite[i].Informations.Interrupteur.AutoFlipping,Sprite[i].Informations.Interrupteur.NbrStates,Sprite[i].Informations.Interrupteur.State);
				pack_fputs(TmpStr,Fichier);
				for(j=0;j<Sprite[i].Informations.Interrupteur.NbrStates;j++) {
					for(k=0;Texture[k]!=Sprite[i].Informations.Interrupteur.Img[j];k++) {
						if(k>=NbrTextures) {
							pack_fclose(Fichier);
							return 0;
						}
					}
					snprintf(TmpStr,TXTBUFFERSIZE,"InterrupteurState%ldImg=%ld\n",j,k);
					pack_fputs(TmpStr,Fichier);
					if(Sprite[i].Informations.Interrupteur.Script[j]==NULL)
						snprintf(TmpStr,TXTBUFFERSIZE,"InterrupteurState%ldScript=-1\n",j);
					else {
						for(k=0;&Script[k]!=Sprite[i].Informations.Interrupteur.Script[j];k++) {
							if(k>=NbrScripts) {
								pack_fclose(Fichier);
								return 0;
							}
						}
						snprintf(TmpStr,TXTBUFFERSIZE,"InterrupteurState%ldScript=%ld\n",j,k);
					}
					pack_fputs(TmpStr,Fichier);
				}
				break;
			case SPRITE_TYPE_PORTE:
				for(j=0;&PorteClass[j]!=Sprite[i].Informations.Porte.Class;j++) {
					if(j>=NbrPorteClasses) {
						pack_fclose(Fichier);
						return 0;
					}
				}
				snprintf(TmpStr,TXTBUFFERSIZE,"PorteClass=%ld\nPorteDelayer=%ld\nPortePhase=%s\nPortexClosed=%lf\nPorteyClosed=%lf\nPortexOpen=%lf\nPorteyOpen=%lf\n",j,Sprite[i].Informations.Porte.Delayer,PortePhaseToStr(Sprite[i].Informations.Porte.Phase),Sprite[i].Informations.Porte.xClosed,Sprite[i].Informations.Porte.yClosed,Sprite[i].Informations.Porte.xOpen,Sprite[i].Informations.Porte.yOpen);
				pack_fputs(TmpStr,Fichier);
				break;
		}
	}
	snprintf(TmpStr,TXTBUFFERSIZE,"[PersoInfos]\nLife=%ld\nLeft=%lf\nTop=%lf\nRot=%lf\n",Perso.Life,Perso.Left,Perso.Top,Perso.Rot);
	pack_fputs(TmpStr,Fichier);
	for(i=0;i<NbrArmes;i++) {
		snprintf(TmpStr,TXTBUFFERSIZE,"Arme%ldAmmos=%ld\nArme%ldOwned=%ld\n",i,Arme[i].Ammos,i,Arme[i].Owned);
		pack_fputs(TmpStr,Fichier);
	}
	for(i=0;&Arme[i]!=Perso.DrawnArme;i++) {
		if(i>=NbrArmes) {
			pack_fclose(Fichier);
			return 0;
		}
	}
	snprintf(TmpStr,TXTBUFFERSIZE,"ArmeDrawn=%ld\n",i);
	pack_fputs(TmpStr,Fichier);

	pack_fclose(Fichier);
	Slot[iSlot].Existing=1;
	Slot[iSlot].iLevel=iLevel;
	Slot[iSlot].Time=time(NULL);
	return 1;
}

void UnloadData() {
	int i;
	for(i=0;i<NbrTextures;i++)
		destroy_bitmap(Texture[i]);
	for(i=0;i<NbrSons;i++)
		destroy_sample(Son[i]);
}

int LevelAnalyser(char *Line) {
	char *StrVal=strchr(Line,'=');
	if(StrVal==NULL)
		return 0;
	*StrVal=0;
	StrVal++;
	if(!strcmp("Name",Line))
		strncpy(Level[NbrLevels-1].Name,StrVal,20);
	else if(!strcmp("Miniature",Line)) {
		Level[NbrLevels-1].Miniature=load_bitmap(StrVal,NULL);
		if(Level[NbrLevels-1].Miniature==NULL)
			return 0;
	}
	else if(!strcmp("Path",Line))
		strncpy(Level[NbrLevels-1].Path,StrVal,230);
	else
		return 0;
	return 1;
}

int ResLstAnalyser(char *Line) {
	char *StrVal=strchr(Line,'=');
	if(StrVal==NULL)
		return 0;
	*StrVal=0;
	StrVal++;
	if(!strcmp("ImgGagner",Line)) {
		Gagner=load_bitmap(StrVal,NULL);
		if(Gagner==NULL)
			return 0;
		if(Gagner->w!=SCREEN_W || Gagner->h!=SCREEN_H) {
			BITMAP *NewBmp=create_bitmap(SCREEN_W,SCREEN_H);
			if(NewBmp==NULL)
				return 0;
			stretch_blit(Gagner,NewBmp,0,0,Gagner->w,Gagner->h,0,0,NewBmp->w,NewBmp->h);
			destroy_bitmap(Gagner);
			Gagner=NewBmp;
		}
	}
	else if(!strcmp("ImgPerdre",Line)) {
		Perdre=load_bitmap(StrVal,NULL);
		if(Perdre==NULL)
			return 0;
		if(Perdre->w!=SCREEN_W || Perdre->h!=SCREEN_H) {
			BITMAP *NewBmp=create_bitmap(SCREEN_W,SCREEN_H);
			if(NewBmp==NULL)
				return 0;
			stretch_blit(Perdre,NewBmp,0,0,Perdre->w,Perdre->h,0,0,NewBmp->w,NewBmp->h);
			destroy_bitmap(Perdre);
			Perdre=NewBmp;
		}
	}
	else if(!strcmp("ImgFin",Line)) {
		Fin=load_bitmap(StrVal,NULL);
		if(Fin==NULL)
			return 0;
		if(Fin->w!=SCREEN_W || Fin->h!=SCREEN_H) {
			BITMAP *NewBmp=create_bitmap(SCREEN_W,SCREEN_H);
			if(NewBmp==NULL)
				return 0;
			stretch_blit(Fin,NewBmp,0,0,Fin->w,Fin->h,0,0,NewBmp->w,NewBmp->h);
			destroy_bitmap(Fin);
			Fin=NewBmp;
		}
	}
	else if(!strcmp("ImgLogo",Line)) {
		Logo=load_bitmap(StrVal,NULL);
		if(Logo==NULL)
			return 0;
		if(Logo->w!=SCREEN_W || Logo->h!=SCREEN_H) {
			BITMAP *NewBmp=create_bitmap(SCREEN_W,SCREEN_H);
			if(NewBmp==NULL)
				return 0;
			stretch_blit(Logo,NewBmp,0,0,Logo->w,Logo->h,0,0,NewBmp->w,NewBmp->h);
			destroy_bitmap(Logo);
			Logo=NewBmp;
		}
	}
	else if(!strcmp("ImgPause",Line)) {
		Pause=load_bitmap(StrVal,NULL);
		if(Pause==NULL)
			return 0;
	}
	else if(!strcmp("ImgFond",Line)) {
		Fond=load_bitmap(StrVal,NULL);
		if(Fond==NULL)
			return 0;
		if(Fond->w!=SCREEN_W || Fond->h!=SCREEN_H) {
			BITMAP *NewBmp=create_bitmap(SCREEN_W,SCREEN_H);
			if(NewBmp==NULL)
				return 0;
			stretch_blit(Fond,NewBmp,0,0,Fond->w,Fond->h,0,0,NewBmp->w,NewBmp->h);
			destroy_bitmap(Fond);
			Fond=NewBmp;
		}
	}
	else if(!strcmp("ImgLocked",Line)) {
		Locked=load_bitmap(StrVal,NULL);
		if(Locked==NULL)
			return 0;
	}
	else if(!strcmp("SonPersoTrouvant",Line)) {
		Perso.SonTrouvant=load_sample(StrVal);
		if(Perso.SonTrouvant==NULL)
			return 0;
	}
	else if(!strcmp("SonPersoSoigne",Line)) {
		Perso.SonSoigne=load_sample(StrVal);
		if(Perso.SonSoigne==NULL)
			return 0;
	}
	else if(!strcmp("SonPersoChargeant",Line)) {
		Perso.SonChargeant=load_sample(StrVal);
		if(Perso.SonChargeant==NULL)
			return 0;
	}
	else if(!strcmp("SonPersoBlesse",Line)) {
		Perso.SonBlesse=load_sample(StrVal);
		if(Perso.SonBlesse==NULL)
			return 0;
	}
	else if(!strcmp("SonPersoMourant",Line)) {
		Perso.SonMourant=load_sample(StrVal);
		if(Perso.SonMourant==NULL)
			return 0;
	}
	else if(!strcmp("Police",Line)) {
		Police=load_font(StrVal,NULL,NULL);
		if(Police==NULL)
			return 0;
	}
	else
		return 0;
	return 1;
}

int ProgressAnalyser(char *Line) {
	char *StrVal=strchr(Line,'=');
	if(StrVal==NULL)
		return 0;
	*StrVal=0;
	StrVal++;
	if(!strncmp(Line,"Level",5))
		Level[atoi(&Line[5])].Locked=!atoi(StrVal);
	else
		return 0;
	return 1;
}

int Slot0Analyser(char *Line) {
	return SlotAnalyserHelper(Line,0);
}

int Slot1Analyser(char *Line) {
	return SlotAnalyserHelper(Line,1);
}

int Slot2Analyser(char *Line) {
	return SlotAnalyserHelper(Line,2);
}

int Slot3Analyser(char *Line) {
	return SlotAnalyserHelper(Line,3);
}

int Slot4Analyser(char *Line) {
	return SlotAnalyserHelper(Line,4);
}

int SlotAnalyserHelper(char *Line,int iSlot) {
	char *StrVal=strchr(Line,'=');
	if(StrVal==NULL)
		return 0;
	*StrVal=0;
	StrVal++;
	if(!strcmp("Existing",Line))
		Slot[iSlot].Existing=atoi(StrVal);
	else if(!strcmp("iLevel",Line))
		Slot[iSlot].iLevel=atoi(StrVal);
	else if(!strcmp("Time",Line))
		Slot[iSlot].Time=atoi(StrVal);
	else
		return 0;
	return 1;
}

int TextureAnalyser(char *Line) {
	if(NbrTextures>=NBRTEXTURESMAX)
		return 0;
	Texture[NbrTextures]=load_bitmap(Line,NULL);
	if(Texture[NbrTextures]==NULL)
		return 0;
	NbrTextures++;
	return 1;
}

int ScriptAnalyser(char *Line) {
	char *Arg=strchr(Line,'(');
	char *NextArg;
	if(Arg==NULL)
		return 0;
	*Arg=0;
	Arg++;

	if(!strcmp(Line,"NewEnnemi")) {
		Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].OpFn=OpFnNewEnnemi;
		NextArg=strchr(Arg,',');
		if(NextArg==NULL)
			return 0;
		*NextArg=0;
		NextArg++;
		Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].Param[0].Double=atof(Arg);
		Arg=NextArg;
		NextArg=strchr(Arg,',');
		if(NextArg==NULL)
			return 0;
		*NextArg=0;
		NextArg++;
		Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].Param[1].Double=atof(Arg);
		Arg=NextArg;
		NextArg=strchr(Arg,',');
		if(NextArg==NULL)
			return 0;
		*NextArg=0;
		NextArg++;
		Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].Param[2].EnnemiClass=&EnnemiClass[atoi(Arg)];
		Arg=NextArg;
		NextArg=strchr(Arg,')');
		if(NextArg==NULL)
			return 0;
		*NextArg=0;
		NextArg++;
		if(*NextArg)
			return 0;
		Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].Param[3].Int=atoi(Arg);
	}
	else if(!strcmp(Line,"NewDecor")) {
		Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].OpFn=OpFnNewDecor;
		NextArg=strchr(Arg,',');
		if(NextArg==NULL)
			return 0;
		*NextArg=0;
		NextArg++;
		Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].Param[0].Double=atof(Arg);
		Arg=NextArg;
		NextArg=strchr(Arg,',');
		if(NextArg==NULL)
			return 0;
		*NextArg=0;
		NextArg++;
		Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].Param[1].Double=atof(Arg);
		Arg=NextArg;
		NextArg=strchr(Arg,',');
		if(NextArg==NULL)
			return 0;
		*NextArg=0;
		NextArg++;
		Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].Param[2].DecorClass=&DecorClass[atoi(Arg)];
		Arg=NextArg;
		NextArg=strchr(Arg,',');
		if(NextArg==NULL)
			return 0;
		*NextArg=0;
		NextArg++;
		Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].Param[3].Int=atoi(Arg);
		Arg=NextArg;
		NextArg=strchr(Arg,')');
		if(NextArg==NULL)
			return 0;
		*NextArg=0;
		NextArg++;
		if(*NextArg)
			return 0;
		Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].Param[4].Double=atof(Arg);
	}
	else if(!strcmp(Line,"SetMap")) {
		Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].OpFn=OpFnSetMap;
		NextArg=strchr(Arg,',');
		if(NextArg==NULL)
			return 0;
		*NextArg=0;
		NextArg++;
		Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].Param[0].Int=atoi(Arg);
		Arg=NextArg;
		NextArg=strchr(Arg,',');
		if(NextArg==NULL)
			return 0;
		*NextArg=0;
		NextArg++;
		Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].Param[1].Int=atoi(Arg);
		Arg=NextArg;
		NextArg=strchr(Arg,')');
		if(NextArg==NULL)
			return 0;
		*NextArg=0;
		NextArg++;
		if(*NextArg)
			return 0;
		{
			int iTexture=atoi(Arg);
			if(iTexture<0)
				Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].Param[2].Bitmap=NULL;
			else
				Script[NbrScripts-1].Instruction[Script[NbrScripts-1].NbrInstructions].Param[2].Bitmap=Texture[iTexture];
		}
	}
	else
		return 0;
	Script[NbrScripts-1].NbrInstructions++;
	return 1;
}

int SonAnalyser(char *Line) {
	if(NbrSons>=NBRSONSMAX)
		return 0;
	Son[NbrSons]=load_sample(Line);
	if(Son[NbrSons]==NULL)
		return 0;
	NbrSons++;
	return 1;
}

int MapAnalyser(char *Line) {
	int i;
	MapWidth=strlen(Line);
	if(MapWidth*MapHeight+MapHeight>MAPSIZEMAX)
		return 0;
	for(i=0;i<MapWidth;i++)
		Map[MapHeight*MapWidth+i]=(Line[i]==' ')?NULL:Texture[Line[i]-'0'];
	MapHeight++;
	return 1;
}

int DecorClassAnalyser(char *Line) {
	char *StrVal=strchr(Line,'=');
	if(StrVal==NULL)
		return 0;
	*StrVal=0;
	StrVal++;
	if(!strcmp("NbrFrames",Line))
		DecorClass[NbrDecorClasses-1].NbrFrames=atoi(StrVal);
	else if(!strcmp("Frame0",Line))
		DecorClass[NbrDecorClasses-1].Frame[0]=Texture[atoi(StrVal)];
	else if(!strcmp("Frame1",Line))
		DecorClass[NbrDecorClasses-1].Frame[1]=Texture[atoi(StrVal)];
	else if(!strcmp("Frame2",Line))
		DecorClass[NbrDecorClasses-1].Frame[2]=Texture[atoi(StrVal)];
	else if(!strcmp("Frame3",Line))
		DecorClass[NbrDecorClasses-1].Frame[3]=Texture[atoi(StrVal)];
	else if(!strcmp("Frame4",Line))
		DecorClass[NbrDecorClasses-1].Frame[4]=Texture[atoi(StrVal)];
	else if(!strcmp("Frame5",Line))
		DecorClass[NbrDecorClasses-1].Frame[5]=Texture[atoi(StrVal)];
	else if(!strcmp("Frame6",Line))
		DecorClass[NbrDecorClasses-1].Frame[6]=Texture[atoi(StrVal)];
	else if(!strcmp("Frame7",Line))
		DecorClass[NbrDecorClasses-1].Frame[7]=Texture[atoi(StrVal)];
	else if(!strcmp("Frame8",Line))
		DecorClass[NbrDecorClasses-1].Frame[8]=Texture[atoi(StrVal)];
	else if(!strcmp("Frame9",Line))
		DecorClass[NbrDecorClasses-1].Frame[9]=Texture[atoi(StrVal)];
	else
		return 0;
	return 1;
}

int TirClassAnalyser(char *Line) {
	char *StrVal=strchr(Line,'=');
	if(StrVal==NULL)
		return 0;
	*StrVal=0;
	StrVal++;
	if(!strcmp("HealthImpact",Line))
		TirClass[NbrTirClasses-1].HealthImpact=atoi(StrVal);
	else if(!strcmp("ImgExplosion",Line))
		TirClass[NbrTirClasses-1].ImgExplosion=Texture[atoi(StrVal)];
	else if(!strcmp("ImgTrajet",Line))
		TirClass[NbrTirClasses-1].ImgTrajet=Texture[atoi(StrVal)];
	else if(!strcmp("Ricoches",Line))
		TirClass[NbrTirClasses-1].Ricoches=atoi(StrVal);
	else if(!strcmp("Speed",Line))
		TirClass[NbrTirClasses-1].Speed=atof(StrVal);
	else
		return 0;
	return 1;
}

int EnnemiClassAnalyser(char *Line) {
	char *StrVal=strchr(Line,'=');
	if(StrVal==NULL)
		return 0;
	*StrVal=0;
	StrVal++;
	if(!strcmp("DelayerBlesse",Line))
		EnnemiClass[NbrEnnemiClasses-1].DelayerBlesse=atoi(StrVal);
	else if(!strcmp("DelayerMarchant",Line))
		EnnemiClass[NbrEnnemiClasses-1].DelayerMarchant=atoi(StrVal);
	else if(!strcmp("DelayerMourant",Line))
		EnnemiClass[NbrEnnemiClasses-1].DelayerMourant=atoi(StrVal);
	else if(!strcmp("DelayerTirant",Line))
		EnnemiClass[NbrEnnemiClasses-1].DelayerTirant=atoi(StrVal);
	else if(!strcmp("ImgAttendant",Line))
		EnnemiClass[NbrEnnemiClasses-1].ImgAttendant=Texture[atoi(StrVal)];
	else if(!strcmp("ImgCherchant",Line))
		EnnemiClass[NbrEnnemiClasses-1].ImgCherchant=Texture[atoi(StrVal)];
	else if(!strcmp("ImgBlesse",Line))
		EnnemiClass[NbrEnnemiClasses-1].ImgBlesse=Texture[atoi(StrVal)];
	else if(!strcmp("ImgMarchantA",Line))
		EnnemiClass[NbrEnnemiClasses-1].ImgMarchantA=Texture[atoi(StrVal)];
	else if(!strcmp("ImgMarchantB",Line))
		EnnemiClass[NbrEnnemiClasses-1].ImgMarchantB=Texture[atoi(StrVal)];
	else if(!strcmp("ImgMort",Line))
		EnnemiClass[NbrEnnemiClasses-1].ImgMort=Texture[atoi(StrVal)];
	else if(!strcmp("ImgMourant",Line))
		EnnemiClass[NbrEnnemiClasses-1].ImgMourant=Texture[atoi(StrVal)];
	else if(!strcmp("ImgTirant",Line))
		EnnemiClass[NbrEnnemiClasses-1].ImgTirant=Texture[atoi(StrVal)];
	else if(!strcmp("Speed",Line))
		EnnemiClass[NbrEnnemiClasses-1].Speed=atof(StrVal);
	else if(!strcmp("StartLife",Line))
		EnnemiClass[NbrEnnemiClasses-1].StartLife=atoi(StrVal);
	else if(!strcmp("Tir",Line))
		EnnemiClass[NbrEnnemiClasses-1].Tir=&TirClass[atoi(StrVal)];
	else if(!strcmp("SonMourant",Line))
		EnnemiClass[NbrEnnemiClasses-1].SonMourant=Son[atoi(StrVal)];
	else if(!strcmp("SonTirant",Line))
		EnnemiClass[NbrEnnemiClasses-1].SonTirant=Son[atoi(StrVal)];
	else if(!strcmp("SonVoyant",Line))
		EnnemiClass[NbrEnnemiClasses-1].SonVoyant=Son[atoi(StrVal)];
	else
		return 0;
	return 1;
}

int SpriteAnalyser(char *Line) {
	char *StrVal=strchr(Line,'=');
	if(StrVal==NULL)
		return 0;
	*StrVal=0;
	StrVal++;
	if(!strcmp("DecorClass",Line))
		Sprite[NbrSprites-1].Informations.Decor.Class=&DecorClass[atoi(StrVal)];
	else if(!strcmp("DecoriFrame",Line))
		Sprite[NbrSprites-1].Informations.Decor.iFrame=atoi(StrVal);
	else if(!strcmp("EnnemiAngleDeMarche",Line))
		Sprite[NbrSprites-1].Informations.Ennemi.AngleDeMarche=atof(StrVal);
	else if(!strcmp("EnnemiClass",Line))
		Sprite[NbrSprites-1].Informations.Ennemi.Class=&EnnemiClass[atoi(StrVal)];
	else if(!strcmp("EnnemiDelayer",Line))
		Sprite[NbrSprites-1].Informations.Ennemi.Delayer=atoi(StrVal);
	else if(!strcmp("EnnemiDistDeMarche",Line))
		Sprite[NbrSprites-1].Informations.Ennemi.DistDeMarche=atof(StrVal);
	else if(!strcmp("EnnemiLife",Line))
		Sprite[NbrSprites-1].Informations.Ennemi.Life=atoi(StrVal);
	else if(!strcmp("EnnemiPhase",Line)) {
		Sprite[NbrSprites-1].Informations.Ennemi.Phase=StrToEnnemiPhase(StrVal);
		if(Sprite[NbrSprites-1].Informations.Ennemi.Phase<0)
			return 0;
	}
	else if(!strcmp("Img",Line))
		Sprite[NbrSprites-1].Img=Texture[atoi(StrVal)];
	else if(!strcmp("ObjetType",Line)) {
		Sprite[NbrSprites-1].Informations.Objet.Type=StrToObjetType(StrVal);
		if(Sprite[NbrSprites-1].Informations.Objet.Type<0)
			return 0;
	}
	else if(!strcmp("ObjetArme",Line))
		Sprite[NbrSprites-1].Informations.Objet.Arme=&Arme[atoi(StrVal)];
	else if(!strcmp("ObjetAmount",Line))
		Sprite[NbrSprites-1].Informations.Objet.Amount=atoi(StrVal);
	else if(!strcmp("PosX",Line))
		Sprite[NbrSprites-1].PosX=atof(StrVal);
	else if(!strcmp("PosY",Line))
		Sprite[NbrSprites-1].PosY=atof(StrVal);
	else if(!strcmp("Rot",Line))
		Sprite[NbrSprites-1].Rot=atof(StrVal);
	else if(!strcmp("AutoRotating",Line))
		Sprite[NbrSprites-1].AutoRotating=atoi(StrVal);
	else if(!strcmp("TirAngle",Line))
		Sprite[NbrSprites-1].Informations.Tir.Angle=atof(StrVal);
	else if(!strcmp("TirClass",Line))
		Sprite[NbrSprites-1].Informations.Tir.Class=&TirClass[atoi(StrVal)];
	else if(!strcmp("TirRicochesAvantDestruction",Line))
		Sprite[NbrSprites-1].Informations.Tir.RicochesAvantDestruction=atoi(StrVal);
	else if(!strcmp("TirToDelete",Line))
		Sprite[NbrSprites-1].Informations.Tir.ToDelete=atoi(StrVal);
	else if(!strcmp("Type",Line)) {
		Sprite[NbrSprites-1].Type=StrToSpriteType(StrVal);
		if(Sprite[NbrSprites-1].Type<0)
			return 0;
	}
	else if(!strcmp("InterrupteurNbrStates",Line))
		Sprite[NbrSprites-1].Informations.Interrupteur.NbrStates=atoi(StrVal);
	else if(!strcmp("InterrupteurState",Line))
		Sprite[NbrSprites-1].Informations.Interrupteur.State=atoi(StrVal);
	else if(!strncmp("InterrupteurState",Line,17)) {
		char strState[40];
		int i;
		for(i=0;Line[i+17]>='0' && Line[i+17]<='9';i++)
			strState[i]=Line[i+17];
		strState[i]=0;
		if(!strcmp("Img",&Line[i+17]))
			Sprite[NbrSprites-1].Informations.Interrupteur.Img[atoi(strState)]=Texture[atoi(StrVal)];
		else if(!strcmp("Script",&Line[i+17])) {
			int iScript=atoi(StrVal);
			if(iScript<0)
				Sprite[NbrSprites-1].Informations.Interrupteur.Script[atoi(strState)]=NULL;
			else
				Sprite[NbrSprites-1].Informations.Interrupteur.Script[atoi(strState)]=&Script[iScript];
		}
		else
			return 0;
	}
	else if(!strcmp("InterrupteurAutoFlipping",Line))
		Sprite[NbrSprites-1].Informations.Interrupteur.AutoFlipping=atoi(StrVal);
	else if(!strcmp("PorteClass",Line))
		Sprite[NbrSprites-1].Informations.Porte.Class=&PorteClass[atoi(StrVal)];
	else if(!strcmp("PorteDelayer",Line))
		Sprite[NbrSprites-1].Informations.Porte.Delayer=atoi(StrVal);
	else if(!strcmp("PortePhase",Line))
		Sprite[NbrSprites-1].Informations.Porte.Phase=StrToPortePhase(StrVal);
	else if(!strcmp("PortexClosed",Line))
		Sprite[NbrSprites-1].Informations.Porte.xClosed=atof(StrVal);
	else if(!strcmp("PorteyClosed",Line))
		Sprite[NbrSprites-1].Informations.Porte.yClosed=atof(StrVal);
	else if(!strcmp("PortexOpen",Line))
		Sprite[NbrSprites-1].Informations.Porte.xOpen=atof(StrVal);
	else if(!strcmp("PorteyOpen",Line))
		Sprite[NbrSprites-1].Informations.Porte.yOpen=atof(StrVal);
	else
		return 0;
	return 1;
}

int PersoInfosAnalyser(char *Line) {
	char *StrVal=strchr(Line,'=');
	if(StrVal==NULL)
		return 0;
	*StrVal=0;
	StrVal++;
	if(!strcmp("Life",Line))
		Perso.Life=atoi(StrVal);
	else if(!strcmp("Rot",Line))
		Perso.Rot=atof(StrVal);
	else if(!strcmp("Left",Line))
		Perso.Left=atof(StrVal);
	else if(!strcmp("Top",Line))
		Perso.Top=atof(StrVal);
	else if(!strcmp("ArmeDrawn",Line))
		Perso.DrawnArme=&Arme[atoi(StrVal)];
	else if(!strncmp("Arme",Line,4)) {
		char striArme[40];
		int i;
		for(i=0;Line[i+4]>='0' && Line[i+4]<='9';i++)
			striArme[i]=Line[i+4];
		striArme[i]=0;
		if(!strcmp("Ammos",&Line[i+4]))
			Arme[atoi(striArme)].Ammos=atoi(StrVal);
		else if(!strcmp("Owned",&Line[i+4]))
			Arme[atoi(striArme)].Owned=atoi(StrVal);
		else
			return 0;
	}
	else
		return 0;
	return 1;
}

int PorteClassAnalyser(char *Line) {
	char *StrVal=strchr(Line,'=');
	if(StrVal==NULL)
		return 0;
	*StrVal=0;
	StrVal++;
	if(!strcmp("DelayerGlissant",Line))
		PorteClass[NbrPorteClasses-1].DelayerGlissant=atoi(StrVal);
	else if(!strcmp("DelayerOuvert",Line))
		PorteClass[NbrPorteClasses-1].DelayerOuvert=atoi(StrVal);
	else if(!strcmp("SonGlissant",Line)) {
		int iSon=atoi(StrVal);
		if(iSon<0)
			PorteClass[NbrPorteClasses-1].SonGlissant=NULL;
		else
			PorteClass[NbrPorteClasses-1].SonGlissant=Son[iSon];
	}
	else
		return 0;
	return 1;
}

int ArmeAnalyser(char *Line) {
	char *StrVal=strchr(Line,'=');
	if(StrVal==NULL)
		return 0;
	*StrVal=0;
	StrVal++;
	if(!strcmp("ImgVisant",Line))
		Arme[NbrArmes-1].ImgVisant=load_bitmap(StrVal,NULL);
	else if(!strcmp("ImgTirant",Line))
		Arme[NbrArmes-1].ImgTirant=load_bitmap(StrVal,NULL);
	else if(!strcmp("SonTirant",Line))
		Arme[NbrArmes-1].SonTirant=load_sample(StrVal);
	else if(!strcmp("Impact",Line))
		Arme[NbrArmes-1].Impact=atoi(StrVal);
	else if(!strcmp("Delayer",Line))
		Arme[NbrArmes-1].Delayer=atoi(StrVal);
	else
		return 0;
	return 1;
}

enum spritetype StrToSpriteType(char *Str) {
	if(!strcmp(Str,"SPRITE_TYPE_DECOR"))
		return SPRITE_TYPE_DECOR;
	else if(!strcmp(Str,"SPRITE_TYPE_ENNEMI"))
		return SPRITE_TYPE_ENNEMI;
	else if(!strcmp(Str,"SPRITE_TYPE_OBJET"))
		return SPRITE_TYPE_OBJET;
	else if(!strcmp(Str,"SPRITE_TYPE_TIR"))
		return SPRITE_TYPE_TIR;
	else if(!strcmp(Str,"SPRITE_TYPE_INTERRUPTEUR"))
		return SPRITE_TYPE_INTERRUPTEUR;
	else if(!strcmp(Str,"SPRITE_TYPE_PORTE"))
		return SPRITE_TYPE_PORTE;
	else
		return -1;
}

enum objettype StrToObjetType(char *Str) {
	if(!strcmp(Str,"OBJET_TYPE_LAURIERS"))
		return OBJET_TYPE_LAURIERS;
	else if(!strcmp(Str,"OBJET_TYPE_AMMOS"))
		return OBJET_TYPE_AMMOS;
	else if(!strcmp(Str,"OBJET_TYPE_MEDIKIT"))
		return OBJET_TYPE_MEDIKIT;
	else if(!strcmp(Str,"OBJET_TYPE_ARME"))
		return OBJET_TYPE_ARME;
	else
		return -1;
}

enum ennemiphase StrToEnnemiPhase(char *Str) {
	if(!strcmp(Str,"ENNEMI_PHASE_ATTENDANT"))
		return ENNEMI_PHASE_ATTENDANT;
	else if(!strcmp(Str,"ENNEMI_PHASE_BLESSE"))
		return ENNEMI_PHASE_BLESSE;
	else if(!strcmp(Str,"ENNEMI_PHASE_CHERCHANT"))
		return ENNEMI_PHASE_CHERCHANT;
	else if(!strcmp(Str,"ENNEMI_PHASE_MARCHANT"))
		return ENNEMI_PHASE_MARCHANT;
	else if(!strcmp(Str,"ENNEMI_PHASE_MORT"))
		return ENNEMI_PHASE_MORT;
	else if(!strcmp(Str,"ENNEMI_PHASE_MOURANT"))
		return ENNEMI_PHASE_MOURANT;
	else if(!strcmp(Str,"ENNEMI_PHASE_TIRANT"))
		return ENNEMI_PHASE_TIRANT;
	else
		return -1;
}

char *SpriteTypeToStr(enum spritetype SpriteType) {
	switch(SpriteType) {
		case SPRITE_TYPE_DECOR:
			return "SPRITE_TYPE_DECOR";
		case SPRITE_TYPE_PORTE:
			return "SPRITE_TYPE_PORTE";
		case SPRITE_TYPE_ENNEMI:
			return "SPRITE_TYPE_ENNEMI";
		case SPRITE_TYPE_OBJET:
			return "SPRITE_TYPE_OBJET";
		case SPRITE_TYPE_TIR:
			return "SPRITE_TYPE_TIR";
		case SPRITE_TYPE_INTERRUPTEUR:
			return "SPRITE_TYPE_INTERRUPTEUR";
		default:
			return "";
	}
}

char *ObjetTypeToStr(enum objettype ObjetType) {
	switch(ObjetType) {
		case OBJET_TYPE_AMMOS:
			return "OBJET_TYPE_AMMOS";
		case OBJET_TYPE_MEDIKIT:
			return "OBJET_TYPE_MEDIKIT";
		case OBJET_TYPE_LAURIERS:
			return "OBJET_TYPE_LAURIERS";
		case OBJET_TYPE_ARME:
			return "OBJET_TYPE_ARME";
		default:
			return "";
	}
}

char *EnnemiPhaseToStr(enum ennemiphase EnnemiPhase) {
	switch(EnnemiPhase) {
		case ENNEMI_PHASE_ATTENDANT:
			return "ENNEMI_PHASE_ATTENDANT";
		case ENNEMI_PHASE_CHERCHANT:
			return "ENNEMI_PHASE_CHERCHANT";
		case ENNEMI_PHASE_BLESSE:
			return "ENNEMI_PHASE_BLESSE";
		case ENNEMI_PHASE_MARCHANT:
			return "ENNEMI_PHASE_MARCHANT";
		case ENNEMI_PHASE_MORT:
			return "ENNEMI_PHASE_MORT";
		case ENNEMI_PHASE_MOURANT:
			return "ENNEMI_PHASE_MOURANT";
		case ENNEMI_PHASE_TIRANT:
			return "ENNEMI_PHASE_TIRANT";
		default:
			return "";
	}
}

enum portephase StrToPortePhase(char *Str) {
	if(!strcmp(Str,"PORTE_PHASE_FERMANT"))
		return PORTE_PHASE_FERMANT;
	else if(!strcmp(Str,"PORTE_PHASE_FERME"))
		return PORTE_PHASE_FERME;
	else if(!strcmp(Str,"PORTE_PHASE_OUVERT"))
		return PORTE_PHASE_OUVERT;
	else if(!strcmp(Str,"PORTE_PHASE_OUVRANT"))
		return PORTE_PHASE_OUVRANT;
	else
		return -1;
}

char *PortePhaseToStr(enum portephase PortePhase) {
	switch(PortePhase) {
		case PORTE_PHASE_FERMANT:
			return "PORTE_PHASE_FERMANT";
		case PORTE_PHASE_FERME:
			return "PORTE_PHASE_FERME";
		case PORTE_PHASE_OUVERT:
			return "PORTE_PHASE_OUVERT";
		case PORTE_PHASE_OUVRANT:
			return "PORTE_PHASE_OUVRANT";
		default:
			return "";
	}
}
