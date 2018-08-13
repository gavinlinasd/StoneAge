#ifndef __NPC_RACEMAN_H__
#define __NPC_RACEMAN_H__

#define DEFMAXRANK 100

struct RANKTABLE
{
	char id[20];
	char name[256];
	int  catchcnt;
	char kindcnt[10000];
	int  rank;
}*LPRANKTABLE;

struct ASKTABLE
{
	int  no;
	char petname[32];
	int  bbi;
	int  lowlv;
	int  highlv;
};

extern struct RANKTABLE ranktable1[DEFMAXRANK];
extern struct RANKTABLE ranktable2[DEFMAXRANK];
extern struct RANKTABLE ranktable3[DEFMAXRANK];
extern struct RANKTABLE ranktable4[DEFMAXRANK];
extern struct RANKTABLE ranktable5[DEFMAXRANK];

extern struct RANKTABLE history1[DEFMAXRANK];
extern struct RANKTABLE history2[DEFMAXRANK];
extern struct RANKTABLE history3[DEFMAXRANK];
extern struct RANKTABLE history4[DEFMAXRANK];
extern struct RANKTABLE history5[DEFMAXRANK];

extern struct ASKTABLE  asktable[400];
	   
BOOL NPC_RacemanInit( int meindex);
void NPC_RacemanTalked( int meindex , int talkerindex , char *szMes ,int color );
void NPC_RacemanWindowTalked( int meindex , int talkerindex , int seqno, int select, char *data);
int  Raceman_getokask( int talker , int random );
//void Raceman_awardprize( int meindex , int talker );


#endif
