#define STATUS_QUITTER -3
#define STATUS_PERDRE -2
#define STATUS_GAGNER -1
#define STATUS_JOUER3D 0
#define STATUS_JOUER2D 1
#define STATUS_PAUSE 2

#define MAPSIZEMAX 10000
#define NBRLEVELSMAX 8
#define NBRTEXTURESMAX 200
#define NBRSCRIPTSMAX 25
#define NBRSONSMAX 200
#define NBRINSTRUCTIONSPERSCRIPTMAX 10
#define NBRARMESMAX 10
#define NBRDECORCLASSESMAX 40
#define NBRTIRCLASSESMAX 40
#define NBRENNEMICLASSESMAX 40
#define NBRPORTECLASSESMAX 20
#define NBRSPRITESMAX 200
#define NBRCOLORFLASHESMAX 200

#define FPSMAX 25
//#define MEPRISERTIRS
#define MAP(x,y) (Map[((int)(y)>>6)*MapWidth+(((int)(x))>>6)])
#define MINI(a,b) ((a<b)?a:b)
#define MAXI(a,b) ((a>b)?a:b)
#define SIMPLIFIERANGLE(Angle) ((Angle)-6.28318531*floor((Angle)/6.28318531))
#define ANGLEREBOND(AngleImpactant,AngleImpacte) (3.14159265+2*SIMPLIFIERANGLE(AngleImpacte)-SIMPLIFIERANGLE(AngleImpactant+3.14159265))
#define LITPIXEL(Pixel,Coef) (\
	(int)((((Pixel) >> _rgb_r_shift_32) & 0xFF)*(Coef))<<_rgb_r_shift_32 | \
	(int)((((Pixel) >> _rgb_g_shift_32) & 0xFF)*(Coef))<<_rgb_g_shift_32 | \
	(int)((((Pixel) >> _rgb_b_shift_32) & 0xFF)*(Coef))<<_rgb_b_shift_32 \
	)

struct script {
	int NbrInstructions;
	struct instruction {
		union param {
			BITMAP *Bitmap;
			double Double;
			int Int;
			struct ennemiclass *EnnemiClass;
			struct decorclass *DecorClass;
		} Param[6];
		int (*OpFn)(union param Param[]);
	} Instruction[NBRINSTRUCTIONSPERSCRIPTMAX];
};

struct arme {
	int Ammos;
	int Delayer;
	int Impact;
	int Owned;
	BITMAP *ImgVisant;
	BITMAP *ImgTirant;
	SAMPLE *SonTirant;
};

struct ennemiclass {
	BITMAP *ImgAttendant;
	BITMAP *ImgCherchant;
	BITMAP *ImgMarchantA;
	BITMAP *ImgMarchantB;
	BITMAP *ImgTirant;
	BITMAP *ImgBlesse;
	BITMAP *ImgMourant;
	BITMAP *ImgMort;
	int DelayerTirant;
	int DelayerBlesse;
	int DelayerMourant;
	int DelayerMarchant;
	SAMPLE *SonVoyant;
	SAMPLE *SonTirant;
	SAMPLE *SonMourant;
	int StartLife;
	double Speed;
	struct tirclass *Tir;
};

struct decorclass {
	int NbrFrames;
	BITMAP *Frame[10];
};

struct tirclass {
	double Speed;
	BITMAP *ImgTrajet;
	BITMAP *ImgExplosion;
	int HealthImpact;
	int Ricoches;
};

struct porteclass {
	int DelayerGlissant;
	int DelayerOuvert;
	SAMPLE *SonGlissant;
};

struct sprite {
	//Variables générales
	double PosX;
	double PosY;
	double Rot;
	int AutoRotating;
	BITMAP *Img;
	enum spritetype {
		SPRITE_TYPE_DECOR,SPRITE_TYPE_OBJET,SPRITE_TYPE_ENNEMI,SPRITE_TYPE_TIR,SPRITE_TYPE_INTERRUPTEUR,SPRITE_TYPE_PORTE
	} Type;
	//Variables initialisées à chaque frame
	double Distance;
	double AX;
	double AY;
	double ZX;
	double ZY;
	//Variables spécifiques au type de sprite
	union {
		struct {
			struct decorclass *Class;
			int iFrame;
		} Decor;
		struct {
			enum objettype {
				OBJET_TYPE_MEDIKIT,OBJET_TYPE_AMMOS,OBJET_TYPE_LAURIERS,OBJET_TYPE_ARME
			} Type;
			struct arme *Arme;
			int Amount;
		} Objet;
		struct {
			struct ennemiclass *Class;
			int Life;
			enum ennemiphase {
				ENNEMI_PHASE_ATTENDANT,ENNEMI_PHASE_CHERCHANT,ENNEMI_PHASE_MARCHANT,ENNEMI_PHASE_TIRANT,ENNEMI_PHASE_BLESSE,ENNEMI_PHASE_MOURANT,ENNEMI_PHASE_MORT
			} Phase;
			int Delayer;
			double AngleDeMarche;
			double DistDeMarche;
		} Ennemi;
		struct {
			struct tirclass *Class;
			int RicochesAvantDestruction;
			double Angle;
			int ToDelete;
		} Tir;
		struct {
			int NbrStates;
			BITMAP *Img[5];
			struct script *Script[5];
			int State;
			int AutoFlipping;
		} Interrupteur;
		struct {
			double xOpen;
			double yOpen;
			double xClosed;
			double yClosed;
			enum portephase {
				PORTE_PHASE_FERME,PORTE_PHASE_OUVRANT,PORTE_PHASE_OUVERT,PORTE_PHASE_FERMANT
			} Phase;
			int Delayer;
			struct porteclass *Class;
		} Porte;
	} Informations;
};

struct persoinfos {
	double Left;
	double Top;
	double Rot;
	int Life;
	struct arme *DrawnArme;
	SAMPLE *SonTrouvant;
	SAMPLE *SonBlesse;
	SAMPLE *SonMourant;
	SAMPLE *SonSoigne;
	SAMPLE *SonChargeant;
};

struct astarlist {
	int FData;
	int GData;
	int HData;
	int iParent;
	int PosX;
	int PosY;
};

struct level {
	BITMAP *Miniature;
	char Name[20];
	char Path[230];
	int Locked;
};

struct slot {
	int Existing;
	int iLevel;
	time_t Time;
};

int Jouer();

void Moteur2D();
void Moteur3D();
void MoteurPause();

void MainTimer(void);
void FrameTimer(void);

int LoadCommon();
int UnloadCommon();
int LoadDataFromLF(char *Filename);
void UnloadData();
int LoadDataFromSF(int iSlot);
int SaveDataToSF(int iSlot);

int Marchant(int iSprite);
double AngleVers(double FromX,double FromY,double ToX,double ToY);
void DeplacerPerso(double xTo,double yTo);
int CollisionAvecSprite(double xFrom,double yFrom,double xTo,double yTo);
int AngleAppartient(double AngleInf,double AngleMed,double AngleSup);
int Intersection(double AX,double AY,double BX,double BY,double CX,double CY,double DX,double DY,double *InterX,double *InterY);

void InitGui();
int Menu(int Loaded);

int FadeInBitmap(BITMAP *Bmp,int Duration);
int FadeOutBitmap(BITMAP *Bmp,int Duration);

int InterpreterScript(struct script *Script);
int OpFnNewEnnemi(union param Param[]);
int OpFnNewDecor(union param Param[]);
int OpFnSetMap(union param Param[]);
