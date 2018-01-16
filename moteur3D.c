#ifdef _DEBUG
#define DEBUGMODE
#endif

#include <stdlib.h>
#include <allegro.h>
#include <math.h>
#include <string.h>
#include "3DSnake.h"

int ColorFlash[NBRCOLORFLASHESMAX];
int NbrColorFlashes=0;

extern double DistancePersoEcran;
extern double *RowToDist;

struct sprite *VisiSprite[NBRSPRITESMAX];
int NbrVisiSprites;

extern BITMAP *Buffer;
extern FONT *Police;
extern double *zBuffer;

extern struct persoinfos Perso;

extern int CouleurHaut;
extern int CouleurNoir;
extern int CouleurBlanc;
extern int CouleurRouge;
extern int CouleurVert;
extern int CouleurBleu;

extern int MapWidth;
extern int MapHeight;
extern BITMAP *Map[];

extern struct sprite Sprite[];
extern int NbrSprites;

extern struct arme Arme[];
extern int NbrArmes;

extern volatile int FramesPerSecond;
extern volatile int Frames;
extern volatile int RefreshDecors;

int LastMouseX;
extern int ColorFlash[];
extern int NbrColorFlashes;
extern int Status;

int DDA(double Left,double Top,double Rot);
double DDAInterX;
double DDAInterY;
double DDADist;

double Angle;
double SinAngle;
double CosAngle;
double TanAngle;
double TanDifferenceDroitAngle;
double CosDifferencePersoRotAngle;

void Moteur3D() {
	int iSprite;
	double x,y;
	long **BufferLine=(long **)Buffer->line;
	BITMAP *Canon;

	//Intelligence artificielle
	{
		double AnglePersoInf=Perso.Rot-0.523598776;
		double AnglePersoSup=Perso.Rot+0.523598776;
		double AngleSpriteInf;
		double AngleSpriteSup;
		int RefreshDecorsInFact;

		if(RefreshDecors) {
			RefreshDecorsInFact=1;
			RefreshDecors=0;
		}
		else
			RefreshDecorsInFact=0;

		NbrVisiSprites=0;
		iSprite=0;
		while(iSprite<NbrSprites) {
			int SupprimerSprite=0;
			double SinSpriteRot;
			double CosSpriteRot;
			Sprite[iSprite].Distance=hypot(Perso.Left-Sprite[iSprite].PosX,Perso.Top-Sprite[iSprite].PosY);
			if(Sprite[iSprite].AutoRotating)
				Sprite[iSprite].Rot=Perso.Rot-1.57079633;
			SinSpriteRot=sin(Sprite[iSprite].Rot);
			CosSpriteRot=cos(Sprite[iSprite].Rot);
			Sprite[iSprite].AX=Sprite[iSprite].PosX-SinSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
			Sprite[iSprite].AY=Sprite[iSprite].PosY-CosSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
			Sprite[iSprite].ZX=Sprite[iSprite].PosX+SinSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
			Sprite[iSprite].ZY=Sprite[iSprite].PosY+CosSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
			AngleSpriteInf=AngleVers(Perso.Left,Perso.Top,Sprite[iSprite].AX,Sprite[iSprite].AY);
			AngleSpriteSup=AngleVers(Perso.Left,Perso.Top,Sprite[iSprite].ZX,Sprite[iSprite].ZY);
			if(AngleSpriteInf>AngleSpriteSup) {
				double TmpAngle=AngleSpriteInf;
				AngleSpriteInf=AngleSpriteSup;
				AngleSpriteSup=TmpAngle;
			}
			if(AngleAppartient(AnglePersoInf,AngleSpriteInf,AnglePersoSup) || AngleAppartient(AnglePersoInf,AngleSpriteSup,AnglePersoSup) || AngleAppartient(AngleSpriteInf,AnglePersoInf,AngleSpriteSup) || AngleAppartient(AngleSpriteInf,AnglePersoSup,AngleSpriteSup)) {
				VisiSprite[NbrVisiSprites]=&Sprite[iSprite];
				NbrVisiSprites++;
			}

			//On gère les sprites
			switch(Sprite[iSprite].Type) {
				case SPRITE_TYPE_DECOR:
					if(RefreshDecorsInFact) {
						double SinSpriteRot;
						double CosSpriteRot;
						Sprite[iSprite].Informations.Decor.iFrame=(Sprite[iSprite].Informations.Decor.iFrame+1)%Sprite[iSprite].Informations.Decor.Class->NbrFrames;
						Sprite[iSprite].Img=Sprite[iSprite].Informations.Decor.Class->Frame[Sprite[iSprite].Informations.Decor.iFrame];
						SinSpriteRot=sin(Sprite[iSprite].Rot);
						CosSpriteRot=cos(Sprite[iSprite].Rot);
						Sprite[iSprite].AX=Sprite[iSprite].PosX-SinSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
						Sprite[iSprite].AY=Sprite[iSprite].PosY-CosSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
						Sprite[iSprite].ZX=Sprite[iSprite].PosX+SinSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
						Sprite[iSprite].ZY=Sprite[iSprite].PosY+CosSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
					}
					break;
				case SPRITE_TYPE_OBJET:
					if(Sprite[iSprite].Distance<64.0) {
						switch(Sprite[iSprite].Informations.Objet.Type) {
							case OBJET_TYPE_LAURIERS:
								Status=STATUS_GAGNER;
								play_sample(Perso.SonTrouvant,255,128,1200,0);
								break;
							case OBJET_TYPE_ARME:
								Sprite[iSprite].Informations.Objet.Arme->Owned=1;
								Perso.DrawnArme=Sprite[iSprite].Informations.Objet.Arme;
								if(NbrColorFlashes<NBRCOLORFLASHESMAX) {
									ColorFlash[NbrColorFlashes]=CouleurNoir;
									NbrColorFlashes++;
								}
								play_sample(Perso.SonChargeant,255,128,1200,0);
								SupprimerSprite=1;
								break;
							case OBJET_TYPE_AMMOS:
								Sprite[iSprite].Informations.Objet.Arme->Ammos+=Sprite[iSprite].Informations.Objet.Amount;
								if(NbrColorFlashes<NBRCOLORFLASHESMAX) {
									ColorFlash[NbrColorFlashes]=CouleurBleu;
									NbrColorFlashes++;
								}
								play_sample(Perso.SonChargeant,255,128,1200,0);
								SupprimerSprite=1;
								break;
							case OBJET_TYPE_MEDIKIT:
								Perso.Life+=Sprite[iSprite].Informations.Objet.Amount;
								if(NbrColorFlashes<NBRCOLORFLASHESMAX) {
									ColorFlash[NbrColorFlashes]=CouleurVert;
									NbrColorFlashes++;
								}
								play_sample(Perso.SonSoigne,255,128,1200,0);
								SupprimerSprite=1;
								break;
						}
					}
					break;
				case SPRITE_TYPE_ENNEMI:
					{
						int PersoVisible;

						//On détermine si perso est visible depuis le sprite. (en fait on fait l'inverse)
						DDA(Perso.Left,Perso.Top,AngleVers(Perso.Left,Perso.Top,Sprite[iSprite].PosX,Sprite[iSprite].PosY));
						if(Sprite[iSprite].Distance<DDADist) {
							int i;
							PersoVisible=1;
							for(i=0;i<NbrSprites;i++) {
								if(Sprite[i].Type==SPRITE_TYPE_PORTE && Intersection(Perso.Left,Perso.Top,Sprite[iSprite].PosX,Sprite[iSprite].PosY,Sprite[i].AX,Sprite[i].AY,Sprite[i].ZX,Sprite[i].ZY,NULL,NULL)) {
									PersoVisible=0;
									break;
								}
							}
						}
						else
							PersoVisible=0;

						switch(Sprite[iSprite].Informations.Ennemi.Phase) {
							case ENNEMI_PHASE_ATTENDANT:
								if(PersoVisible) {
									if(!(rand()%4) || Sprite[iSprite].Distance<96.0) {
										Sprite[iSprite].Img=Sprite[iSprite].Informations.Ennemi.Class->ImgTirant;
										Sprite[iSprite].Informations.Ennemi.Phase=ENNEMI_PHASE_TIRANT;
										Sprite[iSprite].Informations.Ennemi.Delayer=Sprite[iSprite].Informations.Ennemi.Class->DelayerTirant;
									}
									else
										Marchant(iSprite);
									play_sample(Sprite[iSprite].Informations.Ennemi.Class->SonVoyant,(Sprite[iSprite].Distance<800)?128+(800-Sprite[iSprite].Distance)*128/800:128,128,1200,0);
								}
								break;
							case ENNEMI_PHASE_CHERCHANT:
								if(PersoVisible && (!(rand()%4) || Sprite[iSprite].Distance<96.0)) {
									Sprite[iSprite].Img=Sprite[iSprite].Informations.Ennemi.Class->ImgTirant;
									Sprite[iSprite].Informations.Ennemi.Phase=ENNEMI_PHASE_TIRANT;
									Sprite[iSprite].Informations.Ennemi.Delayer=Sprite[iSprite].Informations.Ennemi.Class->DelayerTirant;
								}
								else
									Marchant(iSprite);
								break;
							case ENNEMI_PHASE_MARCHANT:
								if(Sprite[iSprite].Informations.Ennemi.Class->Speed>=Sprite[iSprite].Informations.Ennemi.DistDeMarche) {
									Sprite[iSprite].PosX+=sin(Sprite[iSprite].Informations.Ennemi.AngleDeMarche)*Sprite[iSprite].Informations.Ennemi.DistDeMarche;
									Sprite[iSprite].PosY+=cos(Sprite[iSprite].Informations.Ennemi.AngleDeMarche)*Sprite[iSprite].Informations.Ennemi.DistDeMarche;
									if(PersoVisible && (!(rand()%4) || Sprite[iSprite].Distance<96.0)) {
										Sprite[iSprite].Img=Sprite[iSprite].Informations.Ennemi.Class->ImgTirant;
										Sprite[iSprite].Informations.Ennemi.Phase=ENNEMI_PHASE_TIRANT;
										Sprite[iSprite].Informations.Ennemi.Delayer=Sprite[iSprite].Informations.Ennemi.Class->DelayerTirant;
									}
									else
										Marchant(iSprite);
								}
								else {
									Sprite[iSprite].PosX+=sin(Sprite[iSprite].Informations.Ennemi.AngleDeMarche)*Sprite[iSprite].Informations.Ennemi.Class->Speed;
									Sprite[iSprite].PosY+=cos(Sprite[iSprite].Informations.Ennemi.AngleDeMarche)*Sprite[iSprite].Informations.Ennemi.Class->Speed;
									Sprite[iSprite].Informations.Ennemi.DistDeMarche-=Sprite[iSprite].Informations.Ennemi.Class->Speed;
								}

								if(Sprite[iSprite].Informations.Ennemi.Phase==ENNEMI_PHASE_MARCHANT) {
									Sprite[iSprite].Informations.Ennemi.Delayer--;
									if(Sprite[iSprite].Informations.Ennemi.Delayer<0) {
										if(Sprite[iSprite].Img==Sprite[iSprite].Informations.Ennemi.Class->ImgMarchantA)
											Sprite[iSprite].Img=Sprite[iSprite].Informations.Ennemi.Class->ImgMarchantB;
										else
											Sprite[iSprite].Img=Sprite[iSprite].Informations.Ennemi.Class->ImgMarchantA;
										Sprite[iSprite].Informations.Ennemi.Delayer=Sprite[iSprite].Informations.Ennemi.Class->DelayerMarchant;
									}
								}
								break;
							case ENNEMI_PHASE_TIRANT:
								if(PersoVisible) {
									Sprite[iSprite].Informations.Ennemi.Delayer--;
									if(Sprite[iSprite].Informations.Ennemi.Delayer<0) {
										Sprite[NbrSprites].PosX=Sprite[iSprite].PosX;
										Sprite[NbrSprites].PosY=Sprite[iSprite].PosY;
										Sprite[NbrSprites].Img=Sprite[iSprite].Informations.Ennemi.Class->Tir->ImgTrajet;
										Sprite[NbrSprites].Type=SPRITE_TYPE_TIR;
										Sprite[NbrSprites].Informations.Tir.Class=Sprite[iSprite].Informations.Ennemi.Class->Tir;
										Sprite[NbrSprites].Informations.Tir.RicochesAvantDestruction=Sprite[iSprite].Informations.Ennemi.Class->Tir->Ricoches;
										Sprite[NbrSprites].Informations.Tir.Angle=AngleVers(Sprite[iSprite].PosX,Sprite[iSprite].PosY,Perso.Left,Perso.Top);
										Sprite[NbrSprites].Informations.Tir.ToDelete=0;
										Sprite[NbrSprites].Distance=hypot(Perso.Left-Sprite[NbrSprites].PosX,Perso.Top-Sprite[NbrSprites].PosY);
										Sprite[NbrSprites].AX=Sprite[NbrSprites].PosX-sin(Sprite[NbrSprites].Rot)*64.0*Sprite[NbrSprites].Img->w/Sprite[NbrSprites].Img->h;
										Sprite[NbrSprites].AY=Sprite[NbrSprites].PosY-cos(Sprite[NbrSprites].Rot)*64.0*Sprite[NbrSprites].Img->w/Sprite[NbrSprites].Img->h;
										Sprite[NbrSprites].ZX=Sprite[NbrSprites].PosX+sin(Sprite[NbrSprites].Rot)*64.0*Sprite[NbrSprites].Img->w/Sprite[NbrSprites].Img->h;
										Sprite[NbrSprites].ZY=Sprite[NbrSprites].PosY+cos(Sprite[NbrSprites].Rot)*64.0*Sprite[NbrSprites].Img->w/Sprite[NbrSprites].Img->h;
										Sprite[NbrSprites].AutoRotating=1;
										NbrSprites++;
										if(!(rand()%4) || Sprite[iSprite].Distance<96.0) {
											Sprite[iSprite].Img=Sprite[iSprite].Informations.Ennemi.Class->ImgTirant;
											Sprite[iSprite].Informations.Ennemi.Phase=ENNEMI_PHASE_TIRANT;
											Sprite[iSprite].Informations.Ennemi.Delayer=Sprite[iSprite].Informations.Ennemi.Class->DelayerTirant;
										}
										else
											Marchant(iSprite);
										play_sample(Sprite[iSprite].Informations.Ennemi.Class->SonTirant,(Sprite[iSprite].Distance<800)?128+(800-Sprite[iSprite].Distance)*128/800:128,128,1200,0);
									}
								}
								else
									Marchant(iSprite);
								break;
							case ENNEMI_PHASE_BLESSE:
								Sprite[iSprite].Informations.Ennemi.Delayer--;
								if(Sprite[iSprite].Informations.Ennemi.Delayer<0) {
									if(PersoVisible && (!(rand()%4) || Sprite[iSprite].Distance<96.0)) {
										Sprite[iSprite].Img=Sprite[iSprite].Informations.Ennemi.Class->ImgTirant;
										Sprite[iSprite].Informations.Ennemi.Phase=ENNEMI_PHASE_TIRANT;
										Sprite[iSprite].Informations.Ennemi.Delayer=Sprite[iSprite].Informations.Ennemi.Class->DelayerTirant;
									}
									else
										Marchant(iSprite);
								}
								break;
							case ENNEMI_PHASE_MOURANT:
								Sprite[iSprite].Informations.Ennemi.Delayer--;
								if(Sprite[iSprite].Informations.Ennemi.Delayer<0) {
									Sprite[iSprite].Img=Sprite[iSprite].Informations.Ennemi.Class->ImgMort;
									Sprite[iSprite].Informations.Ennemi.Phase=ENNEMI_PHASE_MORT;
								}
								break;
							case ENNEMI_PHASE_MORT:
								break;
						}
					}
					break;
				case SPRITE_TYPE_TIR:
					if(Sprite[iSprite].Informations.Tir.ToDelete)
						SupprimerSprite=1;
					else if(Sprite[iSprite].Distance<16.0) {
#ifndef MEPRISERTIRS
						Perso.Life-=Sprite[iSprite].Informations.Tir.Class->HealthImpact;
						if(Perso.Life<0) {
							Status=STATUS_PERDRE;
							play_sample(Perso.SonMourant,255,128,1200,0);
						}
						else
							play_sample(Perso.SonBlesse,255,128,1200,0);
						if(NbrColorFlashes<NBRCOLORFLASHESMAX) {
							ColorFlash[NbrColorFlashes]=CouleurRouge;
							NbrColorFlashes++;
						}
#endif
						Sprite[iSprite].Img=Sprite[iSprite].Informations.Tir.Class->ImgExplosion;
						Sprite[iSprite].Informations.Tir.ToDelete=1;
					}
					else {
						int iSpriteSome;
						int iSpriteClosest=-1;
						double ClosestSpriteDist;
						double TmpSpriteX;
						double TmpSpriteY;
						double SpriteWorldRayon=Sprite[iSprite].Img->w*32/Sprite[iSprite].Img->h;

						SinAngle=sin(Sprite[iSprite].Informations.Tir.Angle);
						CosAngle=cos(Sprite[iSprite].Informations.Tir.Angle);
						TmpSpriteX=Sprite[iSprite].PosX+SinAngle*Sprite[iSprite].Informations.Tir.Class->Speed;
						TmpSpriteY=Sprite[iSprite].PosY+CosAngle*Sprite[iSprite].Informations.Tir.Class->Speed;

						for(iSpriteSome=0;iSpriteSome<NbrSprites;iSpriteSome++) {
							double InterX,InterY,InterDist;
							if(Sprite[iSpriteSome].Type==SPRITE_TYPE_PORTE && Intersection(Sprite[iSpriteSome].AX,Sprite[iSpriteSome].AY,Sprite[iSpriteSome].ZX,Sprite[iSpriteSome].ZY,Sprite[iSprite].PosX,Sprite[iSprite].PosY,TmpSpriteX,TmpSpriteY,&InterX,&InterY)) {
								InterDist=hypot(Sprite[iSprite].PosX-InterX,Sprite[iSprite].PosY-InterY);
								if(iSpriteClosest<0 || InterDist<ClosestSpriteDist) {
									ClosestSpriteDist=InterDist;
									iSpriteClosest=iSpriteSome;
								}
							}
						}
						if(iSpriteClosest>=0) {
							Sprite[iSprite].Informations.Tir.RicochesAvantDestruction--;
							if(Sprite[iSprite].Informations.Tir.RicochesAvantDestruction<0) {
								Sprite[iSprite].Img=Sprite[iSprite].Informations.Tir.Class->ImgExplosion;
								Sprite[iSprite].Informations.Tir.ToDelete=1;
							}
							else {
								Sprite[iSprite].PosX+=SinAngle*(ClosestSpriteDist-SpriteWorldRayon);
								Sprite[iSprite].PosY+=CosAngle*(ClosestSpriteDist-SpriteWorldRayon);
								Sprite[iSprite].Informations.Tir.Angle=ANGLEREBOND(Sprite[iSprite].Informations.Tir.Angle,Sprite[iSpriteClosest].Rot);
							}
						}
						else if(MAP(TmpSpriteX-SpriteWorldRayon,TmpSpriteY) || MAP(TmpSpriteX+SpriteWorldRayon,TmpSpriteY) || MAP(TmpSpriteX,TmpSpriteY-SpriteWorldRayon) || MAP(TmpSpriteX,TmpSpriteY+SpriteWorldRayon)) {
							Sprite[iSprite].Informations.Tir.RicochesAvantDestruction--;
							if(Sprite[iSprite].Informations.Tir.RicochesAvantDestruction<0) {
								Sprite[iSprite].Img=Sprite[iSprite].Informations.Tir.Class->ImgExplosion;
								Sprite[iSprite].Informations.Tir.ToDelete=1;
							}
							else {
								double HoriX,HoriY,HoriDist;
								double VertiX,VertiY,VertiDist;
								TanAngle=tan(Sprite[iSprite].Informations.Tir.Angle);
								TanDifferenceDroitAngle=tan(1.57079633-Sprite[iSprite].Informations.Tir.Angle);
								if(CosAngle>0) //Bas
									HoriY=(floor(Sprite[iSprite].PosY/64.0)+1)*64.0-Sprite[iSprite].Img->w*32/Sprite[iSprite].Img->h;
								else //Haut
									HoriY=floor(Sprite[iSprite].PosY/64.0)*64.0-1+Sprite[iSprite].Img->w*32/Sprite[iSprite].Img->h;
								HoriX=Sprite[iSprite].PosX+TanAngle*(HoriY-Sprite[iSprite].PosY);
								if(SinAngle>0) //Droite
									VertiX=(floor(Sprite[iSprite].PosX/64.0)+1)*64.0-Sprite[iSprite].Img->w*32/Sprite[iSprite].Img->h;
								else //Gauche
									VertiX=floor(Sprite[iSprite].PosX/64.0)*64.0-1+Sprite[iSprite].Img->w*32/Sprite[iSprite].Img->h;
								VertiY=Sprite[iSprite].PosY-TanDifferenceDroitAngle*(Sprite[iSprite].PosX-VertiX);
								HoriDist=hypot(Sprite[iSprite].PosX-HoriX,Sprite[iSprite].PosY-HoriY);
								VertiDist=hypot(Sprite[iSprite].PosX-VertiX,Sprite[iSprite].PosY-VertiY);
								if(HoriDist<VertiDist) {
									Sprite[iSprite].PosX=HoriX;
									Sprite[iSprite].PosY=HoriY;
									Sprite[iSprite].Informations.Tir.Angle=ANGLEREBOND(Sprite[iSprite].Informations.Tir.Angle,1.57079633);
								}
								else {
									Sprite[iSprite].PosX=VertiX;
									Sprite[iSprite].PosY=VertiY;
									Sprite[iSprite].Informations.Tir.Angle=ANGLEREBOND(Sprite[iSprite].Informations.Tir.Angle,0);
								}
							}
						}
						else {
							Sprite[iSprite].PosX=TmpSpriteX;
							Sprite[iSprite].PosY=TmpSpriteY;
						}
					}
					break;
				case SPRITE_TYPE_PORTE:
					switch(Sprite[iSprite].Informations.Porte.Phase) {
						case PORTE_PHASE_FERME:
							break;
						case PORTE_PHASE_OUVRANT:
							Sprite[iSprite].Informations.Porte.Delayer--;
							if(Sprite[iSprite].Informations.Porte.Delayer<0) {
								Sprite[iSprite].Informations.Porte.Phase=PORTE_PHASE_OUVERT;
								Sprite[iSprite].Informations.Porte.Delayer=Sprite[iSprite].Informations.Porte.Class->DelayerOuvert;
								Sprite[iSprite].PosX=Sprite[iSprite].Informations.Porte.xOpen;
								Sprite[iSprite].PosY=Sprite[iSprite].Informations.Porte.yOpen;
							}
							else {
								Sprite[iSprite].PosX=Sprite[iSprite].Informations.Porte.xClosed+(Sprite[iSprite].Informations.Porte.xOpen-Sprite[iSprite].Informations.Porte.xClosed)*(Sprite[iSprite].Informations.Porte.Class->DelayerGlissant-Sprite[iSprite].Informations.Porte.Delayer)/Sprite[iSprite].Informations.Porte.Class->DelayerGlissant;
								Sprite[iSprite].PosY=Sprite[iSprite].Informations.Porte.yClosed+(Sprite[iSprite].Informations.Porte.yOpen-Sprite[iSprite].Informations.Porte.yClosed)*(Sprite[iSprite].Informations.Porte.Class->DelayerGlissant-Sprite[iSprite].Informations.Porte.Delayer)/Sprite[iSprite].Informations.Porte.Class->DelayerGlissant;
							}
							{
								double SinSpriteRot=sin(Sprite[iSprite].Rot);
								double CosSpriteRot=cos(Sprite[iSprite].Rot);
								Sprite[iSprite].AX=Sprite[iSprite].PosX-SinSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
								Sprite[iSprite].AY=Sprite[iSprite].PosY-CosSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
								Sprite[iSprite].ZX=Sprite[iSprite].PosX+SinSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
								Sprite[iSprite].ZY=Sprite[iSprite].PosY+CosSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
							}
							break;
						case PORTE_PHASE_OUVERT:
							if(Sprite[iSprite].Informations.Porte.Class->DelayerOuvert>=0) {
								Sprite[iSprite].Informations.Porte.Delayer--;
								if(Sprite[iSprite].Informations.Porte.Delayer<0) {
									Sprite[iSprite].Informations.Porte.Phase=PORTE_PHASE_FERMANT;
									Sprite[iSprite].Informations.Porte.Delayer=Sprite[iSprite].Informations.Porte.Class->DelayerGlissant;
								}
							}
							break;
						case PORTE_PHASE_FERMANT:
							Sprite[iSprite].Informations.Porte.Delayer--;
							if(Sprite[iSprite].Informations.Porte.Delayer<0) {
								Sprite[iSprite].Informations.Porte.Phase=PORTE_PHASE_FERME;
								Sprite[iSprite].PosX=Sprite[iSprite].Informations.Porte.xClosed;
								Sprite[iSprite].PosY=Sprite[iSprite].Informations.Porte.yClosed;
							}
							else {
								Sprite[iSprite].PosX=Sprite[iSprite].Informations.Porte.xOpen+(Sprite[iSprite].Informations.Porte.xClosed-Sprite[iSprite].Informations.Porte.xOpen)*(Sprite[iSprite].Informations.Porte.Class->DelayerGlissant-Sprite[iSprite].Informations.Porte.Delayer)/Sprite[iSprite].Informations.Porte.Class->DelayerGlissant;
								Sprite[iSprite].PosY=Sprite[iSprite].Informations.Porte.yOpen+(Sprite[iSprite].Informations.Porte.yClosed-Sprite[iSprite].Informations.Porte.yOpen)*(Sprite[iSprite].Informations.Porte.Class->DelayerGlissant-Sprite[iSprite].Informations.Porte.Delayer)/Sprite[iSprite].Informations.Porte.Class->DelayerGlissant;
							}
							{
								double SinSpriteRot=sin(Sprite[iSprite].Rot);
								double CosSpriteRot=cos(Sprite[iSprite].Rot);
								Sprite[iSprite].AX=Sprite[iSprite].PosX-SinSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
								Sprite[iSprite].AY=Sprite[iSprite].PosY-CosSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
								Sprite[iSprite].ZX=Sprite[iSprite].PosX+SinSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
								Sprite[iSprite].ZY=Sprite[iSprite].PosY+CosSpriteRot*(32.0*Sprite[iSprite].Img->w/Sprite[iSprite].Img->h);
							}
							break;
					}
					break;
			}
			if(SupprimerSprite) {
				if(iSprite!=NbrSprites-1)
					memcpy(&Sprite[iSprite],&Sprite[NbrSprites-1],sizeof(struct sprite));
				NbrSprites--;
			}
			else
				iSprite++;
		}
	}

	//Joueur
	{
		static int FiringDelayer=0;
		double RunSpeed;
		double RotSpeed;

		int iSpriteCtrled=-1;
		int iSpriteSpaced=-1;
		double CtrledSpriteDist;
		double SpacedSpriteDist;

		DDA(Perso.Left,Perso.Top,Perso.Rot);

		for(iSprite=0;iSprite<NbrSprites;iSprite++) {
			double InterX,InterY,TmpDist;
			if(Intersection(Sprite[iSprite].AX,Sprite[iSprite].AY,Sprite[iSprite].ZX,Sprite[iSprite].ZY,Perso.Left,Perso.Top,DDAInterX,DDAInterY,&InterX,&InterY)) {
				TmpDist=hypot(Perso.Left-InterX,Perso.Top-InterY);
				switch(Sprite[iSprite].Type) {
					case SPRITE_TYPE_ENNEMI:
						if(Sprite[iSprite].Informations.Ennemi.Phase!=ENNEMI_PHASE_MORT && Sprite[iSprite].Informations.Ennemi.Phase!=ENNEMI_PHASE_MOURANT && (iSpriteCtrled<0 || TmpDist<CtrledSpriteDist)) {
							CtrledSpriteDist=TmpDist;
							iSpriteCtrled=iSprite;
						}
						break;
					case SPRITE_TYPE_INTERRUPTEUR:
					case SPRITE_TYPE_PORTE:
						if((iSpriteSpaced<0 || TmpDist<SpacedSpriteDist) && TmpDist<96.0) {
							iSpriteSpaced=iSprite;
							SpacedSpriteDist=TmpDist;
						}
						break;
				}
			}
		}
		if(iSpriteCtrled>=0 && iSpriteSpaced>=0 && Sprite[iSpriteSpaced].Type==SPRITE_TYPE_PORTE && SpacedSpriteDist<CtrledSpriteDist)
			iSpriteCtrled=-1;

		{
			int i;
			for(i=0;i<NbrArmes;i++) {
				if(key[KEY_1+i]) {
					if(Arme[i].Owned)
						Perso.DrawnArme=&Arme[i];
					break;
				}
			}
		}

		if(FiringDelayer) {
			FiringDelayer--;
			Canon=Perso.DrawnArme->ImgVisant;
		}
		else if((mouse_b & 1 || key[KEY_LCONTROL]) && Perso.DrawnArme->Ammos>0) {
			FiringDelayer=Perso.DrawnArme->Delayer;
			Canon=Perso.DrawnArme->ImgTirant;
			Perso.DrawnArme->Ammos--;
			play_sample(Perso.DrawnArme->SonTirant,255,128,1411,0);

			if(iSpriteCtrled>=0) {
				VisiSprite[iSpriteCtrled]->Informations.Ennemi.Life-=35;
				if(VisiSprite[iSpriteCtrled]->Informations.Ennemi.Life<0) {
					VisiSprite[iSpriteCtrled]->Informations.Ennemi.Phase=ENNEMI_PHASE_MOURANT;
					VisiSprite[iSpriteCtrled]->Informations.Ennemi.Delayer=VisiSprite[iSpriteCtrled]->Informations.Ennemi.Class->DelayerMourant;
					VisiSprite[iSpriteCtrled]->Img=VisiSprite[iSpriteCtrled]->Informations.Ennemi.Class->ImgMourant;
					play_sample(VisiSprite[iSpriteCtrled]->Informations.Ennemi.Class->SonMourant,(VisiSprite[iSpriteCtrled]->Distance<800.0)?128+(800-VisiSprite[iSpriteCtrled]->Distance)*128/800:128,128,1200,0);
				}
				else {
					VisiSprite[iSpriteCtrled]->Informations.Ennemi.Phase=ENNEMI_PHASE_BLESSE;
					VisiSprite[iSpriteCtrled]->Informations.Ennemi.Delayer=VisiSprite[iSpriteCtrled]->Informations.Ennemi.Class->DelayerBlesse;
					VisiSprite[iSpriteCtrled]->Img=VisiSprite[iSpriteCtrled]->Informations.Ennemi.Class->ImgBlesse;
				}
			}
		}
		else
			Canon=Perso.DrawnArme->ImgVisant;

		{
			static int DejaPresse=0;
			if(key[KEY_SPACE]) {
				if(!DejaPresse) {
					if(iSpriteSpaced>=0) {
						switch(VisiSprite[iSpriteSpaced]->Type) {
							case SPRITE_TYPE_INTERRUPTEUR:
								if(VisiSprite[iSpriteSpaced]->Informations.Interrupteur.AutoFlipping || VisiSprite[iSpriteSpaced]->Informations.Interrupteur.State+1<VisiSprite[iSpriteSpaced]->Informations.Interrupteur.NbrStates) {
									VisiSprite[iSpriteSpaced]->Informations.Interrupteur.State=(VisiSprite[iSpriteSpaced]->Informations.Interrupteur.State+1)%VisiSprite[iSpriteSpaced]->Informations.Interrupteur.NbrStates;
									VisiSprite[iSpriteSpaced]->Img=VisiSprite[iSpriteSpaced]->Informations.Interrupteur.Img[VisiSprite[iSpriteSpaced]->Informations.Interrupteur.State];
									if(VisiSprite[iSpriteSpaced]->Informations.Interrupteur.Script[VisiSprite[iSpriteSpaced]->Informations.Interrupteur.State])
										InterpreterScript(VisiSprite[iSpriteSpaced]->Informations.Interrupteur.Script[VisiSprite[iSpriteSpaced]->Informations.Interrupteur.State]);
								}
								break;
							case SPRITE_TYPE_PORTE:
								if(VisiSprite[iSpriteSpaced]->Informations.Porte.Phase==PORTE_PHASE_FERME) {
									VisiSprite[iSpriteSpaced]->Informations.Porte.Phase=PORTE_PHASE_OUVRANT;
									VisiSprite[iSpriteSpaced]->Informations.Porte.Delayer=VisiSprite[iSpriteSpaced]->Informations.Porte.Class->DelayerGlissant;
									if(VisiSprite[iSpriteSpaced]->Informations.Porte.Class->SonGlissant)
										play_sample(VisiSprite[iSpriteSpaced]->Informations.Porte.Class->SonGlissant,255,128,1200,0);
								}
								else if(VisiSprite[iSpriteSpaced]->Informations.Porte.Phase==PORTE_PHASE_FERMANT) {
									VisiSprite[iSpriteSpaced]->Informations.Porte.Phase=PORTE_PHASE_OUVRANT;
									VisiSprite[iSpriteSpaced]->Informations.Porte.Delayer=VisiSprite[iSpriteSpaced]->Informations.Porte.Class->DelayerGlissant-VisiSprite[iSpriteSpaced]->Informations.Porte.Delayer;
									if(VisiSprite[iSpriteSpaced]->Informations.Porte.Class->SonGlissant)
										play_sample(VisiSprite[iSpriteSpaced]->Informations.Porte.Class->SonGlissant,255,128,1200,0);
								}
								break;
						}
					}
					DejaPresse=1;
				}
			}
			else if(DejaPresse)
				DejaPresse=0;
		}

		if(LastMouseX!=mouse_x) {
			Perso.Rot+=(double)(LastMouseX-mouse_x)/100.0;
			position_mouse(SCREEN_W/2,SCREEN_H/2);
			LastMouseX=mouse_x;
		}

		if(key[KEY_LSHIFT]) {
			RunSpeed=5;
			RotSpeed=0.01;
		}
		else {
			RunSpeed=17;
			RotSpeed=0.09;
		}

		if(key[KEY_LEFT]) {
			if(key[KEY_ALT]) {
				DeplacerPerso(Perso.Left+RunSpeed*sin(Perso.Rot+1.57079633),Perso.Top+RunSpeed*cos(Perso.Rot+1.57079633));
				if(iSpriteCtrled>=0)
					Perso.Rot=AngleVers(Perso.Left,Perso.Top,Sprite[iSpriteCtrled].PosX,Sprite[iSpriteCtrled].PosY);
			}
			else
				Perso.Rot=SIMPLIFIERANGLE(Perso.Rot+RotSpeed);
		}
		else if(key[KEY_RIGHT]) {
			if(key[KEY_ALT]) {
				DeplacerPerso(Perso.Left+RunSpeed*sin(Perso.Rot-1.57079633),Perso.Top+RunSpeed*cos(Perso.Rot-1.57079633));
				if(iSpriteCtrled>=0)
					Perso.Rot=AngleVers(Perso.Left,Perso.Top,Sprite[iSpriteCtrled].PosX,Sprite[iSpriteCtrled].PosY);
			}
			else
				Perso.Rot=SIMPLIFIERANGLE(Perso.Rot-RotSpeed);
		}

		if(key[KEY_UP])
			DeplacerPerso(Perso.Left+RunSpeed*sin(Perso.Rot),Perso.Top+RunSpeed*cos(Perso.Rot));
		else if(key[KEY_DOWN])
			DeplacerPerso(Perso.Left-RunSpeed*sin(Perso.Rot),Perso.Top-RunSpeed*cos(Perso.Rot));
	}

	//Dessin
	for(x=0;x<SCREEN_W;x++) {
		double HauteurTrait;

		int InMap=DDA(Perso.Left,Perso.Top,Perso.Rot+0.523598776-x*1.04719755/(double)SCREEN_W);

		HauteurTrait=DistancePersoEcran*64.0/(DDADist*CosDifferencePersoRotAngle);

		//On dessine la colonne
		{
			BITMAP *Text=MAP(DDAInterX,DDAInterY);
			int yStart=SCREEN_H/2.0-HauteurTrait/2.0;
			int yEnd=SCREEN_H/2.0+HauteurTrait/2.0;
			double PixelDist;
			int Pixel;
			for(y=0;y<SCREEN_H;y++) {
				if(y<=yStart) {
					PixelDist=RowToDist[(int)y]/CosDifferencePersoRotAngle;
					Pixel=CouleurHaut;
				}
				else if(y>=yEnd) {
					int Antecedent,Image;
					PixelDist=RowToDist[(int)y]/CosDifferencePersoRotAngle;
					Antecedent=(((int)(Perso.Left+SinAngle*PixelDist)%32)+((int)(Perso.Top+CosAngle*PixelDist)%32)-32);
					Image=Antecedent*Antecedent/8+64;
					Pixel=(Image<<_rgb_r_shift_32)+(Image<<_rgb_g_shift_32)+(Image<<_rgb_b_shift_32);
				}
				else {
					PixelDist=DDADist;
					Pixel=(InMap)?((long *)Text->line[(int)(Text->h*(y-yStart)/HauteurTrait)])[(int)((DDAInterX+DDAInterY)*Text->w/64.0)%Text->w] :CouleurNoir;
				}

				zBuffer[(int)y]=PixelDist;
				if(PixelDist<700.0)
					BufferLine[(int)y][(int)x]=Pixel;
				else if(PixelDist>1200.0)
					BufferLine[(int)y][(int)x]=0;
				else {
					double Pourcentage=(1200.0-PixelDist)/500.0;
					BufferLine[(int)y][(int)x]=LITPIXEL(Pixel,Pourcentage);
				}
			}
		}

		//On dessine les sprites
		for(iSprite=0;iSprite<NbrVisiSprites;iSprite++) {
			long **SpriteLine;
			double Distance;
			double HauteurSprite;
			double InterX,InterY;
			int Col;
			double yStart,yEnd;
			int Pixel;
			double VarA,VarB;

			if(!Intersection(Perso.Left,Perso.Top,DDAInterX,DDAInterY,VisiSprite[iSprite]->AX,VisiSprite[iSprite]->AY,VisiSprite[iSprite]->ZX,VisiSprite[iSprite]->ZY,&InterX,&InterY))
				continue;
			Col=hypot(InterX-VisiSprite[iSprite]->AX,InterY-VisiSprite[iSprite]->AY)*VisiSprite[iSprite]->Img->w/(VisiSprite[iSprite]->Img->w*64.0/VisiSprite[iSprite]->Img->h);
			if(Col<0 || Col>=VisiSprite[iSprite]->Img->w)
				continue;
			Distance=hypot(InterX-Perso.Left,InterY-Perso.Top);
			HauteurSprite=DistancePersoEcran*64.0/(Distance*CosDifferencePersoRotAngle);

			if(HauteurSprite>SCREEN_H) {
				yStart=0;
				yEnd=SCREEN_H;
			}
			else {
				yStart=(SCREEN_H-HauteurSprite)/2;
				yEnd=(SCREEN_H+HauteurSprite)/2;
			}

			SpriteLine=(long **)VisiSprite[iSprite]->Img->line;

			VarA=VisiSprite[iSprite]->Img->h/HauteurSprite;
			VarB=VarA*(HauteurSprite-SCREEN_H)/2.0;

			for(y=yStart;y<yEnd;y++) {
				if(Distance<zBuffer[(int)y]) {
					Pixel=SpriteLine[(int)(y*VarA+VarB)][Col];
					if(Pixel!=MASK_COLOR_32) {
						zBuffer[(int)y]=Distance;
						BufferLine[(int)y][(int)x]=Pixel;
					}
				}
			}
		}
	}

	//Affichage de l'arme
	{
		double CanonWidth=SCREEN_W*Canon->w/640;
		double CanonHeight=Canon->h*CanonWidth/Canon->w;
		stretch_sprite(Buffer,Canon,SCREEN_W/2-CanonWidth/2,SCREEN_H-CanonHeight,CanonWidth,CanonHeight);
	}

	//On s'interésse à l'affichage hypothétique du plan 2D par transparence
	{
		static int Actif=0;
		static int Delayer=0;
		if(key[KEY_RSHIFT]) {
			if(!Delayer) {
				Actif=!Actif;
				Delayer=1;
			}
		}
		else if(Delayer)
			Delayer=0;
		if(Actif) {
			int x,y;
			double SinPersoRot=sin(Perso.Rot);
			double CosPersoRot=cos(Perso.Rot);
			double DeltaX;
			double DeltaY;
			int MapX;
			int MapY;
			long *Line;
			for(y=0;y<SCREEN_H;y++) {
				Line=(long *)Buffer->line[y];
				for(x=0;x<SCREEN_W;x++) {
					DeltaX=4*x-2*SCREEN_W;
					DeltaY=4*y-2*SCREEN_H;
					MapX=((int)(Perso.Left-DeltaX*CosPersoRot-DeltaY*SinPersoRot))>>6;
					MapY=((int)(Perso.Top+DeltaX*SinPersoRot-DeltaY*CosPersoRot))>>6;
					if(MapX<0 || MapX>=MapWidth || MapY<0 || MapY>=MapHeight || Map[MapY*MapWidth+MapX])
						Line[x]=LITPIXEL(Line[x],0.25);
				}
			}
		}
	}

	//On trace le viseur, on donne quelques infos puis on envoie à l'écran
	vline(Buffer,SCREEN_W/2,SCREEN_H/2-4,SCREEN_H/2+4,CouleurVert);
	hline(Buffer,SCREEN_W/2-4,SCREEN_H/2,SCREEN_W/2+4,CouleurVert);
	if(SCREEN_W>=640)
		textprintf_centre_ex(Buffer,Police,SCREEN_W/2,SCREEN_H-text_height(Police),-1,-1,"Life:%ld Ammos:%ld Fps:%ld",Perso.Life,Perso.DrawnArme->Ammos,FramesPerSecond);
	else
		textprintf_centre_ex(Buffer,font,SCREEN_W/2,SCREEN_H-text_height(font),CouleurVert,CouleurNoir,"Life:%ld Ammos:%ld Fps:%ld",Perso.Life,Perso.DrawnArme->Ammos,FramesPerSecond);

	if(NbrColorFlashes) {
		NbrColorFlashes--;
		clear_to_color(screen,ColorFlash[NbrColorFlashes]);
	}
	else
		blit(Buffer,screen,0,0,0,0,SCREEN_W,SCREEN_H);

	if(Status>=0) {
		if(key[KEY_F1])
			Status=STATUS_JOUER2D;
		else if(key[KEY_F3])
			Status=STATUS_PAUSE;
	}
	if(key[KEY_ESC] && !Menu(1))
		Status=STATUS_QUITTER;

	//
	if(key[KEY_Z]) {
		extern struct tirclass TirClass[];
		Sprite[NbrSprites].PosX=Perso.Left+sin(Perso.Rot)*32.0;
		Sprite[NbrSprites].PosY=Perso.Top+cos(Perso.Rot)*32.0;
		Sprite[NbrSprites].Img=TirClass[0].ImgTrajet;
		Sprite[NbrSprites].AutoRotating=1;
		Sprite[NbrSprites].Type=SPRITE_TYPE_TIR;
		Sprite[NbrSprites].Informations.Tir.Angle=Perso.Rot;
		Sprite[NbrSprites].Informations.Tir.Class=&TirClass[0];
		Sprite[NbrSprites].Informations.Tir.RicochesAvantDestruction=0;
		Sprite[NbrSprites].Informations.Tir.ToDelete=0;
		NbrSprites++;
	}//

	Frames++;
}

double AngleVers(double FromX,double FromY,double ToX,double ToY) {
	if(FromY<ToY) {
		if(FromX<ToX)
			return atan((ToX-FromX)/(ToY-FromY));
		else if(FromX>ToX)
			return 4.71238898+atan((ToY-FromY)/(FromX-ToX));
		else
			return 0;
	}
	if(FromY>ToY) {
		if(FromX<ToX)
			return 1.57079633+atan((FromY-ToY)/(ToX-FromX));
		else if(FromX>ToX)
			return 3.14159265+atan((ToX-FromX)/(ToY-FromY));
		else
			return 3.14159265;
	}
	else {
		if(FromX<ToX)
			return 1.57079633;
		else if(FromX>ToX)
			return 4.71238898;
		else
			return 0; //Cas de To==From
	}
}

int CollisionAvecSprite(double xFrom,double yFrom,double xTo,double yTo) {
	int iSprite;
	for(iSprite=0;iSprite<NbrSprites;iSprite++)
		if(((Sprite[iSprite].Type==SPRITE_TYPE_ENNEMI && Sprite[iSprite].Informations.Ennemi.Phase!=ENNEMI_PHASE_MORT) || Sprite[iSprite].Type==SPRITE_TYPE_DECOR || Sprite[iSprite].Type==SPRITE_TYPE_PORTE) && Intersection(xFrom,yFrom,xTo,yTo,Sprite[iSprite].AX,Sprite[iSprite].AY,Sprite[iSprite].ZX,Sprite[iSprite].ZY,NULL,NULL))
			return 1;
	return 0;
}

void DeplacerPerso(double xTo,double yTo) {
	if(xTo>=0 && xTo<64.0*MapWidth && yTo>=0 && yTo<64.0*MapHeight && MAP(xTo,yTo)==NULL && !CollisionAvecSprite(Perso.Left,Perso.Top,xTo,yTo)) {
		Perso.Left=xTo;
		Perso.Top=yTo;
	}
	else if(xTo>=0 && xTo<64.0*MapWidth && MAP(xTo,Perso.Top)==NULL && !CollisionAvecSprite(Perso.Left,Perso.Top,xTo,Perso.Top))
		Perso.Left=xTo;
	else if(yTo>=0 && yTo<64.0*MapHeight && MAP(Perso.Left,yTo)==NULL && !CollisionAvecSprite(Perso.Left,Perso.Top,Perso.Left,yTo))
		Perso.Top=yTo;
}

int AngleAppartient(double AngleInf,double AngleMed,double AngleSup) {
	double AngleA=SIMPLIFIERANGLE(AngleInf);
	double AngleB=SIMPLIFIERANGLE(AngleMed);
	double AngleC=SIMPLIFIERANGLE(AngleSup);
	
	if(AngleA>AngleC) {
		if(AngleB>AngleA && AngleB<AngleC)
			return 1;
		else
			return 0;
	}
	else {
		if(AngleB>AngleA || AngleB<AngleC)
			return 1;
		else
			return 0;
	}
}

int DDA(double Left,double Top,double Rot) {
	double HoriDirX,HoriDirY,HoriInterX,HoriInterY,HoriFound;
	double VertiDirX,VertiDirY,VertiInterX,VertiInterY,VertiFound;
	double WorldWidth=MapWidth*64.0;
	double WorldHeight=MapHeight*64.0;

	Angle=Rot;
	SinAngle=sin(Angle);
	CosAngle=cos(Angle);
	TanAngle=tan(Angle);
	TanDifferenceDroitAngle=tan(1.57079633-Angle);
	CosDifferencePersoRotAngle=cos(Perso.Rot-Angle);

	//On prépare les variables
	if(CosAngle>0) {
		//Bas
		HoriDirX=64.0*TanAngle;
		HoriDirY=64.0;
		HoriInterY=(floor(Top/64.0)+1)*64.0;
		HoriInterX=Left+TanAngle*(HoriInterY-Top);
	}
	else {
		//Haut
		HoriDirX=-64.0*TanAngle;
		HoriDirY=-64.0;
		HoriInterY=floor(Top/64.0)*64.0-.1;
		HoriInterX=Left+TanAngle*(HoriInterY-Top);
	}

	if(SinAngle>0) {
		//Droite
		VertiDirX=64.0;
		VertiDirY=64.0*TanDifferenceDroitAngle;
		VertiInterX=(floor(Left/64.0)+1)*64.0;
		VertiInterY=Top-TanDifferenceDroitAngle*(Left-VertiInterX);
	}
	else {
		//Gauche
		VertiDirX=-64.0;
		VertiDirY=-64.0*TanDifferenceDroitAngle;
		VertiInterX=floor(Left/64.0)*64.0-.1;
		VertiInterY=Top-TanDifferenceDroitAngle*(Left-VertiInterX);
	}

	//On teste les intersections
	HoriFound=1;
	if(HoriInterX<0 || HoriInterX>=WorldWidth || HoriInterY<0 || HoriInterY>=WorldHeight)
		HoriFound=0;
	else while(MAP(HoriInterX,HoriInterY)==NULL) {
		HoriInterX+=HoriDirX;
		HoriInterY+=HoriDirY;
		if(HoriInterX<0 || HoriInterX>=WorldWidth || HoriInterY<0 || HoriInterY>=WorldHeight) {
			HoriFound=0;
			break;
		}
	}
	VertiFound=1;
	if(VertiInterX<0 || VertiInterX>=WorldWidth || VertiInterY<0 || VertiInterY>=WorldHeight)
		VertiFound=0;
	else while(MAP(VertiInterX,VertiInterY)==NULL) {
		VertiInterX+=VertiDirX;
		VertiInterY+=VertiDirY;
		if(VertiInterX<0 || VertiInterX>=WorldWidth || VertiInterY<0 || VertiInterY>=WorldHeight) {
			VertiFound=0;
			break;
		}
	}

	//On déduit la distance la plus courte
	if(HoriFound && VertiFound) {
		double HoriDist=hypot(Left-HoriInterX,Top-HoriInterY);
		double VertiDist=hypot(Left-VertiInterX,Top-VertiInterY);

		if(HoriDist<VertiDist) {
			DDAInterX=HoriInterX;
			DDAInterY=HoriInterY;
			DDADist=HoriDist;
		}
		else {
			DDAInterX=VertiInterX;
			DDAInterY=VertiInterY;
			DDADist=VertiDist;
		}
		return 1;
	}
	else if(HoriFound) {
		DDAInterX=HoriInterX;
		DDAInterY=HoriInterY;
		DDADist=hypot(Left-HoriInterX,Top-HoriInterY);
		return 1;
	}
	else if(VertiFound) {
		DDAInterX=VertiInterX;
		DDAInterY=VertiInterY;
		DDADist=hypot(Left-VertiInterX,Top-VertiInterY);
		return 1;
	}
	else {
		double HoriDist=hypot(Left-HoriInterX,Top-HoriInterY);
		double VertiDist=hypot(Left-VertiInterX,Top-VertiInterY);

		if(HoriDist<VertiDist) {
			DDAInterX=HoriInterX;
			DDAInterY=HoriInterY;
			DDADist=HoriDist;
		}
		else {
			DDAInterX=VertiInterX;
			DDAInterY=VertiInterY;
			DDADist=VertiDist;
		}
		return 0;
	}
}

int Intersection(double AX,double AY,double BX,double BY,double CX,double CY,double DX,double DY,double *InterX,double *InterY) {
	double d;
	double ua;
	double ub;

	d=(DY-CY)*(BX-AX)-(DX-CX)*(BY-AY);
	if(d==0)
		return 0;
	ua=((DX-CX)*(AY-CY)-(DY-CY)*(AX-CX))/d;
	ub=((BX-AX)*(AY-CY)-(BY-AY)*(AX-CX))/d;

	if(ua>=0 && ua<=1 && ub>=0 && ub<=1) {
		if(InterX)
			*InterX=AX+(ua*(BX-AX));
		if(InterY)
			*InterY=AY+(ua*(BY-AY));
		return 1;
	}
	return 0;
}
