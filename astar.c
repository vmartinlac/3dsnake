#ifdef _DEBUG
#define DEBUGMODE
#endif

#include <math.h>
#include <allegro.h>
#include "3DSnake.h"

extern struct persoinfos Perso;
extern struct sprite Sprite[];
extern int NbrSprites;

extern int MapWidth;
extern int MapHeight;
extern BITMAP *Map[];

struct astarlist ListeFermee[MAPSIZEMAX],ListeOuverte[MAPSIZEMAX];
int NbrListeOuvertePoints;
int NbrListeFermeePoints;

void VerifierPoint(int PosX,int PosY,int Parent);

int xFrom;
int yFrom;
int xTo;
int yTo;

//Retourne l'indice dans la liste fermée du point (xDest,yDest) ou une valeur négative en cas d'échec
//Penser à remplir xFrom,yFrom,xTo et yTo avant d'appeler la fonction
/*int GetPath() {

	NbrListeOuvertePoints=0;
	NbrListeFermeePoints=0;

	ListeOuverte[NbrListeOuvertePoints].PosX=xFrom;
	ListeOuverte[NbrListeOuvertePoints].PosY=yFrom;
	ListeOuverte[NbrListeOuvertePoints].iParent=-1;
	ListeOuverte[NbrListeOuvertePoints].GData=0;
	ListeOuverte[NbrListeOuvertePoints].HData=(xTo-xFrom)*(xTo-xFrom)+(yTo-yFrom)*(yTo-yFrom);
	ListeOuverte[NbrListeOuvertePoints].FData=ListeOuverte[NbrListeOuvertePoints].GData+ListeOuverte[NbrListeOuvertePoints].HData;
	NbrListeOuvertePoints++;
	for(;;) {
		int iCurrent=0;
		int iSome;
		if(NbrListeOuvertePoints==0)
			return -1;
		for(iSome=0;iSome<NbrListeOuvertePoints;iSome++)
			if(ListeOuverte[iSome].FData<ListeOuverte[iCurrent].FData)
				iCurrent=iSome;
		ListeFermee[NbrListeFermeePoints].PosX=ListeOuverte[iCurrent].PosX;
		ListeFermee[NbrListeFermeePoints].PosY=ListeOuverte[iCurrent].PosY;
		ListeFermee[NbrListeFermeePoints].iParent=ListeOuverte[iCurrent].iParent;
		ListeFermee[NbrListeFermeePoints].FData=ListeOuverte[iCurrent].FData;
		ListeFermee[NbrListeFermeePoints].GData=ListeOuverte[iCurrent].GData;
		ListeFermee[NbrListeFermeePoints].HData=ListeOuverte[iCurrent].HData;
		ListeOuverte[iCurrent].PosX=ListeOuverte[NbrListeOuvertePoints-1].PosX;
		ListeOuverte[iCurrent].PosY=ListeOuverte[NbrListeOuvertePoints-1].PosY;
		ListeOuverte[iCurrent].iParent=ListeOuverte[NbrListeOuvertePoints-1].iParent;
		ListeOuverte[iCurrent].FData=ListeOuverte[NbrListeOuvertePoints-1].FData;
		ListeOuverte[iCurrent].GData=ListeOuverte[NbrListeOuvertePoints-1].GData;
		ListeOuverte[iCurrent].HData=ListeOuverte[NbrListeOuvertePoints-1].HData;
		iCurrent=NbrListeFermeePoints;

		NbrListeOuvertePoints--;
		NbrListeFermeePoints++;

		if(ListeFermee[iCurrent].PosX==xTo && ListeFermee[iCurrent].PosY==yTo)
			return iCurrent;

		VerifierPoint(ListeFermee[iCurrent].PosX-1,ListeFermee[iCurrent].PosY,iCurrent);
		VerifierPoint(ListeFermee[iCurrent].PosX+1,ListeFermee[iCurrent].PosY,iCurrent);
		VerifierPoint(ListeFermee[iCurrent].PosX,ListeFermee[iCurrent].PosY-1,iCurrent);
		VerifierPoint(ListeFermee[iCurrent].PosX,ListeFermee[iCurrent].PosY+1,iCurrent);
		VerifierPoint(ListeFermee[iCurrent].PosX-1,ListeFermee[iCurrent].PosY-1,iCurrent);
		VerifierPoint(ListeFermee[iCurrent].PosX+1,ListeFermee[iCurrent].PosY+1,iCurrent);
		VerifierPoint(ListeFermee[iCurrent].PosX+1,ListeFermee[iCurrent].PosY-1,iCurrent);
		VerifierPoint(ListeFermee[iCurrent].PosX-1,ListeFermee[iCurrent].PosY+1,iCurrent);
	}
	return -1;
}*/

void VerifierPoint(int PosX,int PosY,int Parent) {
	int iSome;
	int FData;
	int GData;
	int HData;
	enum {
		ORTHOGONAL,DIAGONAL
	} Deplacement=(PosX!=ListeFermee[Parent].PosX && PosY!=ListeFermee[Parent].PosY)?DIAGONAL:ORTHOGONAL;

	if(PosX<0 || PosY<0 || PosX>=MapWidth || PosY>=MapHeight || Map[PosY*MapWidth+PosX])
		return;

	if(Deplacement==DIAGONAL && (Map[ListeFermee[Parent].PosY*MapWidth+PosX] || Map[PosY*MapWidth+ListeFermee[Parent].PosX]))
		return;

	for(iSome=0;iSome<NbrListeFermeePoints;iSome++)
		if(ListeFermee[iSome].PosX==PosX && ListeFermee[iSome].PosY==PosY)
			return;

	GData=ListeFermee[Parent].GData+(Deplacement==ORTHOGONAL)?10:14;
	HData=(PosX-xTo)*(PosX-xTo)+(PosY-yTo)*(PosY-yTo);
	FData=GData+HData;

	for(iSome=0;iSome<NbrListeOuvertePoints;iSome++) {
		if(ListeOuverte[iSome].PosX==PosX && ListeOuverte[iSome].PosY==PosY) {
			if(ListeOuverte[iSome].GData>GData) {
				ListeOuverte[iSome].iParent=Parent;
				ListeOuverte[iSome].FData=FData;
				ListeOuverte[iSome].GData=GData;
				ListeOuverte[iSome].HData=HData;
			}
			return;
		}
	}

	{
		int iSprite;
		for(iSprite=0;iSprite<NbrSprites;iSprite++)
			if(((Sprite[iSprite].Type==SPRITE_TYPE_PORTE && Sprite[iSprite].Informations.Porte.Phase==PORTE_PHASE_FERME) || Sprite[iSprite].Type==SPRITE_TYPE_PORTE) && Intersection(ListeFermee[Parent].PosX*64.0+32.0,ListeFermee[Parent].PosY*64.0+32.0,PosX*64.0+32.0,PosY*64.0+32.0,Sprite[iSprite].AX,Sprite[iSprite].AY,Sprite[iSprite].ZX,Sprite[iSprite].ZY,NULL,NULL))
				return;
	}

	ListeOuverte[NbrListeOuvertePoints].PosX=PosX;
	ListeOuverte[NbrListeOuvertePoints].PosY=PosY;
	ListeOuverte[NbrListeOuvertePoints].iParent=Parent;
	ListeOuverte[NbrListeOuvertePoints].FData=FData;
	ListeOuverte[NbrListeOuvertePoints].GData=GData;
	ListeOuverte[NbrListeOuvertePoints].HData=HData;
	NbrListeOuvertePoints++;
}

int Marchant(int iSprite) {
	//Ne faire appel à cette fonction que si Sprite et Perso sont sur des cases différentes !
	double xNextDouble,yNextDouble;
	int iPerso;
	int iSome;

	if((int)Sprite[iSprite].PosX/64==(int)Sprite[iSprite].PosY/64 && (int)Perso.Left/64==(int)Perso.Top/64) {
		TRACE("Utilisation de A* sans nécessité de déplacement !\n");
		Sprite[iSprite].Informations.Ennemi.Phase=ENNEMI_PHASE_CHERCHANT;
		Sprite[iSprite].Img=Sprite[iSprite].Informations.Ennemi.Class->ImgCherchant;
		return 0;
	}

	xFrom=(int)Sprite[iSprite].PosX/64;
	yFrom=(int)Sprite[iSprite].PosY/64;
	xTo=(int)Perso.Left/64;
	yTo=(int)Perso.Top/64;

	NbrListeOuvertePoints=0;
	NbrListeFermeePoints=0;

	ListeOuverte[NbrListeOuvertePoints].PosX=xFrom;
	ListeOuverte[NbrListeOuvertePoints].PosY=yFrom;
	ListeOuverte[NbrListeOuvertePoints].iParent=-1;
	ListeOuverte[NbrListeOuvertePoints].GData=0;
	ListeOuverte[NbrListeOuvertePoints].HData=(xTo-xFrom)*(xTo-xFrom)+(yTo-yFrom)*(yTo-yFrom);
	ListeOuverte[NbrListeOuvertePoints].FData=ListeOuverte[NbrListeOuvertePoints].GData+ListeOuverte[NbrListeOuvertePoints].HData;
	NbrListeOuvertePoints++;

	for(;;) {
		int iCurrent=0;
		if(NbrListeOuvertePoints==0) {
			Sprite[iSprite].Informations.Ennemi.Phase=ENNEMI_PHASE_CHERCHANT;
			Sprite[iSprite].Img=Sprite[iSprite].Informations.Ennemi.Class->ImgCherchant;
			return 0;
		}
		for(iSome=0;iSome<NbrListeOuvertePoints;iSome++)
			if(ListeOuverte[iSome].FData<ListeOuverte[iCurrent].FData)
				iCurrent=iSome;
		ListeFermee[NbrListeFermeePoints].PosX=ListeOuverte[iCurrent].PosX;
		ListeFermee[NbrListeFermeePoints].PosY=ListeOuverte[iCurrent].PosY;
		ListeFermee[NbrListeFermeePoints].iParent=ListeOuverte[iCurrent].iParent;
		ListeFermee[NbrListeFermeePoints].FData=ListeOuverte[iCurrent].FData;
		ListeFermee[NbrListeFermeePoints].GData=ListeOuverte[iCurrent].GData;
		ListeFermee[NbrListeFermeePoints].HData=ListeOuverte[iCurrent].HData;
		ListeOuverte[iCurrent].PosX=ListeOuverte[NbrListeOuvertePoints-1].PosX;
		ListeOuverte[iCurrent].PosY=ListeOuverte[NbrListeOuvertePoints-1].PosY;
		ListeOuverte[iCurrent].iParent=ListeOuverte[NbrListeOuvertePoints-1].iParent;
		ListeOuverte[iCurrent].FData=ListeOuverte[NbrListeOuvertePoints-1].FData;
		ListeOuverte[iCurrent].GData=ListeOuverte[NbrListeOuvertePoints-1].GData;
		ListeOuverte[iCurrent].HData=ListeOuverte[NbrListeOuvertePoints-1].HData;
		iCurrent=NbrListeFermeePoints;

		NbrListeOuvertePoints--;
		NbrListeFermeePoints++;

		if(ListeFermee[iCurrent].PosX==xTo && ListeFermee[iCurrent].PosY==yTo) {
			iPerso=iCurrent;
			break;
		}

		VerifierPoint(ListeFermee[iCurrent].PosX-1,ListeFermee[iCurrent].PosY,iCurrent);
		VerifierPoint(ListeFermee[iCurrent].PosX+1,ListeFermee[iCurrent].PosY,iCurrent);
		VerifierPoint(ListeFermee[iCurrent].PosX,ListeFermee[iCurrent].PosY-1,iCurrent);
		VerifierPoint(ListeFermee[iCurrent].PosX,ListeFermee[iCurrent].PosY+1,iCurrent);
		VerifierPoint(ListeFermee[iCurrent].PosX-1,ListeFermee[iCurrent].PosY-1,iCurrent);
		VerifierPoint(ListeFermee[iCurrent].PosX+1,ListeFermee[iCurrent].PosY+1,iCurrent);
		VerifierPoint(ListeFermee[iCurrent].PosX+1,ListeFermee[iCurrent].PosY-1,iCurrent);
		VerifierPoint(ListeFermee[iCurrent].PosX-1,ListeFermee[iCurrent].PosY+1,iCurrent);
	}

	for(iSome=iPerso;ListeFermee[ListeFermee[iSome].iParent].iParent>=0;iSome=ListeFermee[iSome].iParent);

	xNextDouble=(double)ListeFermee[iSome].PosX*64.0+32.0;
	yNextDouble=(double)ListeFermee[iSome].PosY*64.0+32.0;
	
	Sprite[iSprite].Informations.Ennemi.AngleDeMarche=AngleVers(Sprite[iSprite].PosX,Sprite[iSprite].PosY,xNextDouble,yNextDouble);
	Sprite[iSprite].Informations.Ennemi.DistDeMarche=hypot(Sprite[iSprite].PosX-xNextDouble,Sprite[iSprite].PosY-yNextDouble);

	Sprite[iSprite].Img=Sprite[iSprite].Informations.Ennemi.Class->ImgMarchantA;
	Sprite[iSprite].Informations.Ennemi.Delayer=Sprite[iSprite].Informations.Ennemi.Class->DelayerMarchant;
	Sprite[iSprite].Informations.Ennemi.Phase=ENNEMI_PHASE_MARCHANT;
	return 1;
}
