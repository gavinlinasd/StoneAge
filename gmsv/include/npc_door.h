#ifndef __NPC_DOOR_H__
#define __NPC_DOOR_H__

BOOL NPC_DoorInit(int meindex );
void NPC_DoorTalked( int meindex , int talkerindex , char *msg , int color );
void NPC_DoorPreAttacked(int meindex , int attacker , float *dm);
void NPC_DoorPostOver( int meindex , int movedindex );
void NPC_DoorOff( int meindex , int movedindex );
void NPC_DoorWatch( int meindex, int moveindex, CHAR_ACTION act,
                           int x, int y, int dir, int* opt, int optlen );
int NPC_DoorSearchByName( char *name );
int NPC_DoorSearchByPosition( int fl , int x, int y );
void NPC_DoorLooked( int meindex , int lookedindex );
BOOL NPC_DoorRefreshOverFlg( int meind );


/* 眥摯NPC凝�梜鯚蛗鯆戰醫瓊腇PC蹬鏤痲哱昡鳴勗僑笢趙毀 npc_stepswitch.c禱陑Й */
int NPC_DoorSetPasswd( int meindex, char *pas );
int NPC_DoorGetPasswd( int meindex, char *out , int len );
void NPC_DoorFlip(int meindex , int doindex);
void NPC_DoorClose(int meindex , int doindex);
void NPC_DoorOpen(int meindex , int doindex );
void NPC_DoorAddSwitchCounter( int meindex , int i , int talker );
void NPC_DoorSetSwitchCounter( int meindex, int i , int doindex );

#endif





