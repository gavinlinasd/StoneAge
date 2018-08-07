#ifndef __NPC_FMWARPMAN_H__
#define __NPC_FMWARPMAN_H__

void NPC_FMWarpManTalked(int meindex, int talkerindex, char *msg, int color);
BOOL NPC_FMWarpManInit(int meindex);
void NPC_FMWarpManWindowTalked(int meindex, int talkerindex, int seqno,
	int select, char *data);
void NPC_FMWarpManLoop(int meindex);
void NPC_GetPKFMNum(int floor, int fmindex1, int fmindex2, int *num1, int *num2);



#ifdef _DEATH_FAMILY_GM_COMMAND	// WON ADD 家族战GM指令
void set_fm_pk_score( int win_index, int loser_index, char *win_name, char *loser_name );
void swap_fm_pk_struct( int no1, int no2 );
int get_fm_pk_index( int win_index );
void read_fm_pk_score();
void save_fm_pk_score();

#endif








#endif 
/*__NPC_FMWARPMAN_H__*/
