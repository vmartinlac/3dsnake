#ifdef _DEBUG
#define DEBUGMODE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>
#include "3DSnake.h"

extern BITMAP *Fond;
extern FONT *Police;
extern BITMAP *Locked;

extern struct level Level[];
extern int NbrLevels;
extern int iLevel;

extern struct slot Slot[];

int n_button_proc(int msg, DIALOG *d, int c);
int level_proc(int msg, DIALOG *d, int c);
int slot_proc(int msg, DIALOG *d, int c);

DIALOG GuiA[6];
DIALOG GuiB[10];
DIALOG GuiC[4];

int UsePolice;

int Menu(int Loaded) {
	if(Loaded) {
		GuiA[0].flags=D_EXIT;
		GuiA[3].flags=D_EXIT;
	}
	else {
		GuiA[0].flags=D_DISABLED;
		GuiA[3].flags=D_DISABLED;
	}
	if(Slot[0].Existing||Slot[1].Existing||Slot[2].Existing||Slot[3].Existing||Slot[4].Existing||Slot[5].Existing)
		GuiA[2].flags=D_EXIT;
	else
		GuiA[2].flags=D_DISABLED;
	clear_keybuf();
	show_mouse(screen);
DebutMenu:
	blit(Fond,screen,0,0,0,0,SCREEN_W,SCREEN_H);
	switch(do_dialog(GuiA,-1)) {
		case -1:
			show_mouse(NULL);
			position_mouse(SCREEN_W/2,SCREEN_H/2);

			if(Loaded) {
				key[KEY_ESC]=0;
				return 1;
			}
			else
				return 0;
		case 0:
			position_mouse(SCREEN_W/2,SCREEN_H/2);
			show_mouse(NULL);
			return 1;
		case 1:
			{
				int i;
				for(i=0;i<NbrLevels;i++) {
					GuiB[i+1].proc=level_proc;
					if(Level[i].Locked)
						GuiB[i+1].flags=D_DISABLED;
					else
						GuiB[i+1].flags=D_EXIT;
				}
				GuiB[i+1].proc=NULL;
				blit(Fond,screen,0,0,0,0,SCREEN_W,SCREEN_H);
				switch(i=do_dialog(GuiB,-1)) {
					case -1:
					case 0:
						goto DebutMenu;
					default:
						iLevel=i-1;
				}
			}

			show_mouse(NULL);
			position_mouse(SCREEN_W/2,SCREEN_H/2);

			if(Loaded)
				UnloadData();

			if(LoadDataFromLF(Level[iLevel].Path))
				return 1;
			else
				return 0;
		case 2:
			blit(Fond,screen,0,0,0,0,SCREEN_W,SCREEN_H);
			GuiC[1].dp="Charger";
			GuiC[2].d2=0;
			if(Slot[0].Existing)
				GuiC[2].d1=0;
			else if(Slot[1].Existing)
				GuiC[2].d1=1;
			else if(Slot[2].Existing)
				GuiC[2].d1=2;
			else if(Slot[3].Existing)
				GuiC[2].d1=3;
			else if(Slot[4].Existing)
				GuiC[2].d1=4;
			else
				GuiC[2].d1=5;
			switch(do_dialog(GuiC,-1)) {
				case 0:
				case -1:
					goto DebutMenu;
			}

			show_mouse(NULL);
			position_mouse(SCREEN_W/2,SCREEN_H/2);

			if(Loaded)
				UnloadData();

			if(LoadDataFromSF(GuiC[2].d1))
				return 1;
			else
				return 0;
		case 3:
			blit(Fond,screen,0,0,0,0,SCREEN_W,SCREEN_H);
			GuiC[1].dp="Sauvegarder";
			GuiC[2].d2=1;
			GuiC[2].d1=0;

			switch(do_dialog(GuiC,-1)) {
				case 0:
				case -1:
					goto DebutMenu;
			}

			show_mouse(NULL);
			position_mouse(SCREEN_W/2,SCREEN_H/2);

			if(SaveDataToSF(GuiC[2].d1))
				return 1;
			else
				return 0;
		case 4:
		default:
			show_mouse(NULL);
			return 0;
	}
}

int n_button_proc(int msg, DIALOG *d, int c) {
	if(msg==MSG_DRAW) {
		BITMAP *Ecran=gui_get_screen();
		FONT *FontToUse;
		int FontColour;
		int TextWidth;
		if(d->flags & D_SELECTED) {
			if(UsePolice) {
				FontToUse=Police;
				FontColour=-1;
				TextWidth=text_length(Police,d->dp);
			}
			else {
				FontToUse=font;
				FontColour=d->bg;
				TextWidth=text_length(font,d->dp);
			}

			if(d->flags & D_DISABLED)
				FontColour=makecol(128,128,128);

			rectfill(Ecran,d->x,d->y,d->x+d->w,d->y+d->h,d->fg);
			textout_centre_ex(Ecran,FontToUse,d->dp,d->x+d->w/2,d->y+d->h/2-text_height(FontToUse)/2,FontColour,d->fg);
			rect(Ecran,d->x,d->y,d->x+d->w,d->y+d->h,d->bg);
			if(d->flags & D_GOTFOCUS) {
				int DividedTextWidth=TextWidth/2;
				int StartX=d->x+d->w/2;
				triangle(Ecran,StartX-DividedTextWidth-8,d->y+d->h/2,StartX-DividedTextWidth-22,d->y+5,StartX-DividedTextWidth-22,d->y+d->h-5,d->bg);
				triangle(Ecran,StartX+DividedTextWidth+8,d->y+d->h/2,StartX+DividedTextWidth+22,d->y+5,StartX+DividedTextWidth+22,d->y+d->h-5,d->bg);
			}
		}
		else {
			if(UsePolice) {
				FontToUse=Police;
				FontColour=-1;
				TextWidth=text_length(Police,d->dp);
			}
			else {
				FontToUse=font;
				FontColour=d->fg;
				TextWidth=text_length(font,d->dp);
			}

			if(d->flags & D_DISABLED)
				FontColour=makecol(128,128,128);

			rectfill(Ecran,d->x,d->y,d->x+d->w,d->y+d->h,d->bg);
			textout_centre_ex(Ecran,FontToUse,d->dp,d->x+d->w/2,d->y+d->h/2-text_height(FontToUse)/2,FontColour,d->bg);
			rect(Ecran,d->x,d->y,d->x+d->w,d->y+d->h,d->fg);
			if(d->flags & D_GOTFOCUS) {
				int DividedTextWidth=TextWidth/2;
				int StartX=d->x+d->w/2;
				triangle(Ecran,StartX-DividedTextWidth-8,d->y+d->h/2,StartX-DividedTextWidth-22,d->y+5,StartX-DividedTextWidth-22,d->y+d->h-5,d->fg);
				triangle(Ecran,StartX+DividedTextWidth+8,d->y+d->h/2,StartX+DividedTextWidth+22,d->y+5,StartX+DividedTextWidth+22,d->y+d->h-5,d->fg);
			}
		}
		if(d->flags & D_DISABLED) {
			int x,y;
			for(x=1;x<d->w-1;x+=2)
				for(y=1;y<d->h-1;y+=2)
					putpixel(Ecran,x+d->x,y+d->y,0xDDDDDD);
		}
		return D_O_K;
	}
	else
		return d_button_proc(msg,d,c);
}

int level_proc(int msg, DIALOG *d, int c) {
	if(msg==MSG_DRAW) {
		BITMAP *Ecran=gui_get_screen();
		BITMAP *Miniature=(Level[d->d1].Locked)?Locked:Level[d->d1].Miniature;
		char *Texte=Level[d->d1].Name;
		int TextWidth=text_length(font,Texte);
		int TextHeight=text_height(font);

		if(d->flags & D_SELECTED) {
			rectfill(Ecran,d->x,d->y+d->h-TextHeight-2,d->x+d->w,d->y+d->h,d->fg);
			stretch_blit(Miniature,Ecran,0,0,Miniature->w,Miniature->h,d->x,d->y,d->w,d->h-TextHeight-2);
			textout_centre_ex(Ecran,font,Texte,d->x+d->w/2,d->y+d->h-TextHeight,d->bg,d->fg);
			rect(Ecran,d->x,d->y,d->x+d->w,d->y+d->h,d->bg);
			if(d->flags & D_GOTFOCUS) {
				triangle(Ecran,d->x+d->w/2-TextWidth/2-2,d->y+d->h-TextHeight/2-1,d->x+d->w/2-TextWidth/2-5,d->y+d->h-TextHeight/2-4,d->x+d->w/2-TextWidth/2-5,d->y+d->h-TextHeight/2+3,d->bg);
				triangle(Ecran,d->x+d->w/2+TextWidth/2+1,d->y+d->h-TextHeight/2-1,d->x+d->w/2+TextWidth/2+4,d->y+d->h-TextHeight/2-4,d->x+d->w/2+TextWidth/2+4,d->y+d->h-TextHeight/2+3,d->bg);
			}
		}
		else {
			rectfill(Ecran,d->x,d->y+d->h-TextHeight-2,d->x+d->w,d->y+d->h,d->bg);
			stretch_blit(Miniature,Ecran,0,0,Miniature->w,Miniature->h,d->x,d->y,d->w,d->h-TextHeight-2);
			textout_centre_ex(Ecran,font,Texte,d->x+d->w/2,d->y+d->h-TextHeight,d->fg,d->bg);
			rect(Ecran,d->x,d->y,d->x+d->w,d->y+d->h,d->fg);
			if(d->flags & D_GOTFOCUS) {
				triangle(Ecran,d->x+d->w/2-TextWidth/2-2,d->y+d->h-TextHeight/2-1,d->x+d->w/2-TextWidth/2-5,d->y+d->h-TextHeight/2-4,d->x+d->w/2-TextWidth/2-5,d->y+d->h-TextHeight/2+3,d->fg);
				triangle(Ecran,d->x+d->w/2+TextWidth/2+1,d->y+d->h-TextHeight/2-1,d->x+d->w/2+TextWidth/2+4,d->y+d->h-TextHeight/2-4,d->x+d->w/2+TextWidth/2+4,d->y+d->h-TextHeight/2+3,d->fg);
			}
		}
		return D_O_K;
	}
	else
		return d_button_proc(msg,d,c);
}

int slot_proc(int msg, DIALOG *d, int c) {
	switch(msg) {
		case MSG_WANTFOCUS:
			return D_WANTFOCUS;
		case MSG_CHAR:
			switch(c >>8) {
				case KEY_UP:
					{
						int i;
						for(i=d->d1-1;i>=0;i--) {
							if(Slot[i].Existing) {
								d->d1=i;
								object_message(d,MSG_DRAW,0);
								return D_USED_CHAR;
							}
						}
						return D_O_K;
					}
				case KEY_DOWN:
					{
						int i;
						for(i=d->d1+1;i<5;i++) {
							if(Slot[i].Existing) {
								d->d1=i;
								object_message(d,MSG_DRAW,0);
								return D_USED_CHAR;
							}
						}
						return D_O_K;
					}
				default:
					return D_O_K;
			}
		case MSG_CLICK:
			{
				int iRow=(gui_mouse_y()-d->y-2)/(d->h/5);
				int Row[5];
				int NbrRows=0;
				int i;
				for(i=0;i<5;i++) {
					if(Slot[i].Existing || d->d2) {
						Row[NbrRows]=i;
						NbrRows++;
					}
				}
				if(iRow>=0 && iRow<NbrRows) {
					d->d1=Row[iRow];
					return D_REDRAWME;
				}
			}
			return D_O_K;
		case MSG_DCLICK:
			if(d->flags & D_EXIT)
				return D_CLOSE;
			else
				return D_O_K;
		case MSG_DRAW:
			{
				BITMAP *Ecran=gui_get_screen();
				int i;
				int FontHeight=(UsePolice)?text_height(Police):text_height(font);
				int RowHeight=(d->h-4)/5;
				int PosY=d->y+2;

				rectfill(Ecran,d->x,d->y,d->x+d->w,d->y+d->h,d->bg);
				rect(Ecran,d->x,d->y,d->x+d->w,d->y+d->h,d->fg);

				for(i=0;i<5;i++) {
					if(Slot[i].Existing || d->d2) {
						char StrTime[230];

						if(i==d->d1)
							rectfill(Ecran,d->x+2,PosY,d->x+d->w-2,PosY+RowHeight,makecol(64,64,255));

						{
							struct tm LocalTime;
							localtime_r(&Slot[i].Time, &LocalTime);
							strftime(StrTime,230,"%x %X",&LocalTime);
						}
						
						if(UsePolice) {
							textprintf_ex(Ecran,Police,d->x+2,PosY+RowHeight/2-FontHeight/2,-1,-1,"Slot%ld:",i+1);
							if(Slot[i].Existing) {
								textout_ex(Ecran,font,Level[Slot[i].iLevel].Name,d->x+110,PosY+RowHeight/2-text_height(font),0,-1);
								textout_ex(Ecran,font,StrTime,d->x+110,PosY+RowHeight/2,0,-1);
							}
							else
								textout_ex(Ecran,font,"Non utilise",d->x+110,PosY+RowHeight/2-text_height(font)/2,0,-1);
						}
						else {
							textprintf_ex(Ecran,font,d->x+2,PosY+RowHeight/2-FontHeight/2,0,-1,"Slot%ld:",i+1);
							if(Slot[i].Existing) {
								textout_ex(Ecran,font,Level[Slot[i].iLevel].Name,d->x+50,PosY+RowHeight/2-text_height(font),0,-1);
								textout_ex(Ecran,font,StrTime,d->x+50,PosY+RowHeight/2,0,-1);
							}
							else
								textout_ex(Ecran,font,"Non utilise",d->x+50,PosY+RowHeight/2-text_height(font)/2,0,-1);
						}
						PosY+=RowHeight;
					}
				}
			}
			return D_O_K;
		default:
			return D_O_K;
	}
}

void InitGui() {
	UsePolice=(SCREEN_W>=640);

	//GuiA
	GuiA[4].bg=GuiA[3].bg=GuiA[2].bg=GuiA[1].bg=GuiA[0].bg=makecol(255,255,0);
	GuiA[4].fg=GuiA[3].fg=GuiA[2].fg=GuiA[1].fg=GuiA[0].fg=makecol(255,0,0);
	GuiA[4].flags=GuiA[3].flags=GuiA[2].flags=GuiA[1].flags=GuiA[0].flags=D_EXIT;
	GuiA[4].key=GuiA[3].key=GuiA[2].key=GuiA[1].key=GuiA[0].key=0;
	GuiA[4].proc=GuiA[3].proc=GuiA[2].proc=GuiA[1].proc=GuiA[0].proc=n_button_proc;
	GuiA[0].dp="Jouer";
	GuiA[1].dp="Nouvelle partie";
	GuiA[2].dp="Charger";
	GuiA[3].dp="Sauvegarder";
	GuiA[4].dp="Quitter";
	if(SCREEN_W>=640) {
		GuiA[4].w=GuiA[3].w=GuiA[2].w=GuiA[1].w=GuiA[0].w=330;
		GuiA[4].h=GuiA[3].h=GuiA[2].h=GuiA[1].h=GuiA[0].h=40;
		GuiA[4].x=GuiA[3].x=GuiA[2].x=GuiA[1].x=GuiA[0].x=SCREEN_W/2-GuiA[0].w/2;
		GuiA[0].y=SCREEN_H/3;
		GuiA[1].y=GuiA[0].y+GuiA[0].h+10;
		GuiA[2].y=GuiA[1].y+GuiA[1].h+10;
		GuiA[3].y=GuiA[2].y+GuiA[2].h+10;
		GuiA[4].y=GuiA[3].y+GuiA[3].h+10;
	}
	else {
		GuiA[4].w=GuiA[3].w=GuiA[2].w=GuiA[1].w=GuiA[0].w=175;
		GuiA[4].h=GuiA[3].h=GuiA[2].h=GuiA[1].h=GuiA[0].h=15;
		GuiA[4].x=GuiA[3].x=GuiA[2].x=GuiA[1].x=GuiA[0].x=SCREEN_W/2-GuiA[0].w/2;
		GuiA[0].y=SCREEN_H/3;
		GuiA[1].y=GuiA[0].y+GuiA[0].h+4;
		GuiA[2].y=GuiA[1].y+GuiA[1].h+4;
		GuiA[3].y=GuiA[2].y+GuiA[2].h+4;
		GuiA[4].y=GuiA[3].y+GuiA[3].h+4;
	}
	GuiA[5].proc=NULL;

	//GuiB
	GuiB[8].bg=GuiB[7].bg=GuiB[6].bg=GuiB[5].bg=GuiB[4].bg=GuiB[3].bg=GuiB[2].bg=GuiB[1].bg=GuiB[0].bg=makecol(255,255,0);
	GuiB[8].fg=GuiB[7].fg=GuiB[6].fg=GuiB[5].fg=GuiB[4].fg=GuiB[3].fg=GuiB[2].fg=GuiB[1].fg=GuiB[0].fg=makecol(255,0,0);
	GuiB[8].flags=GuiB[7].flags=GuiB[6].flags=GuiB[5].flags=GuiB[4].flags=GuiB[3].flags=GuiB[2].flags=GuiB[1].flags=GuiB[0].flags=D_EXIT;
	GuiB[8].key=GuiB[7].key=GuiB[6].key=GuiB[5].key=GuiB[4].key=GuiB[3].key=GuiB[2].key=GuiB[1].key=GuiB[0].key=0;
	GuiB[8].proc=GuiB[7].proc=GuiB[6].proc=GuiB[5].proc=GuiB[4].proc=GuiB[3].proc=GuiB[2].proc=GuiB[1].proc=level_proc;
	GuiB[0].dp="Precedent";
	GuiB[0].proc=n_button_proc;

	if(SCREEN_W>=640) {
		GuiB[0].w=270;
		GuiB[0].h=40;
	}
	else {
		GuiB[0].w=136;
		GuiB[0].h=15;
	}
	GuiB[0].x=SCREEN_W-GuiB[0].w-16;
	GuiB[0].y=SCREEN_H-GuiB[0].h-16;

	GuiB[5].x=GuiB[1].x=SCREEN_W*16/640;
	GuiB[6].x=GuiB[2].x=SCREEN_W*176/640;
	GuiB[7].x=GuiB[3].x=SCREEN_W*336/640;
	GuiB[8].x=GuiB[4].x=SCREEN_W*496/640;

	GuiB[1].y=GuiB[2].y=GuiB[3].y=GuiB[4].y=SCREEN_H*160/480;
	GuiB[5].y=GuiB[6].y=GuiB[7].y=GuiB[8].y=SCREEN_H*288/480;

	GuiB[1].w=GuiB[2].w=GuiB[3].w=GuiB[4].w=GuiB[5].w=GuiB[6].w=GuiB[7].w=GuiB[8].w=SCREEN_W*128/640;
	GuiB[1].h=GuiB[2].h=GuiB[3].h=GuiB[4].h=GuiB[5].h=GuiB[6].h=GuiB[7].h=GuiB[8].h=SCREEN_H*96/480;
	
	GuiB[1].d1=0;
	GuiB[2].d1=1;
	GuiB[3].d1=2;
	GuiB[4].d1=3;
	GuiB[5].d1=4;
	GuiB[6].d1=5;
	GuiB[7].d1=6;
	GuiB[8].d1=7;

	GuiB[9].proc=NULL;

	//GuiC
	GuiC[2].bg=GuiC[1].bg=GuiC[0].bg=makecol(255,255,0);
	GuiC[2].fg=GuiC[1].fg=GuiC[0].fg=makecol(255,0,0);
	GuiC[2].flags=GuiC[1].flags=GuiC[0].flags=D_EXIT;
	GuiC[2].key=GuiC[1].key=GuiC[0].key=0;

	if(SCREEN_W>=640) {
		GuiC[1].w=GuiC[0].w=270;
		GuiC[1].h=GuiC[0].h=40;
		GuiC[2].w=320;
		GuiC[2].h=240;
	}
	else {
		GuiC[1].w=GuiC[0].w=136;
		GuiC[1].h=GuiC[0].h=15;
		GuiC[2].w=200;
		GuiC[2].h=90;
	}
	GuiC[0].x=SCREEN_W-GuiC[0].w-16;
	GuiC[1].x=16;
	GuiC[1].y=GuiC[0].y=SCREEN_H-GuiC[0].h-16;
	GuiC[1].proc=GuiC[0].proc=n_button_proc;
	GuiC[0].dp="Precedent";

	GuiC[2].proc=slot_proc;
	GuiC[2].x=SCREEN_W/2-GuiC[2].w/2;
	GuiC[2].y=SCREEN_H*160/480;

	GuiC[3].proc=NULL;
}
