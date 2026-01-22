#ifndef _NPCS_HANDLE_H
#define _NPCS_HANDLE_H
#include "version.h"
#include "util.h"

#ifdef _NPCSERVER_NEW
//沭璃瓚剿
BOOL NPCS_HandleCheckFreeMess( int npcobjindex, int charaindex, int charobjindex,
				char *CheckfreeMess);

BOOL NPCS_FreePassCheck( int charaindex, char *buf);

//掀誕瓚剿
BOOL NPCS_FreeBigSmallCheck( int charaindex,char* buf);
//統杅瓚剿
BOOL NPCS_ArgumentFreeCheck( int charaindex, char* Argument, int amount, int temp, int probjID);
//統杅湮苤瓚剿
BOOL NPCS_ArgumentBigSmallCheck( int point, int mypoint, int flg);
//�△譆葆沉D唾昜杅	*reAmount 呁牄戲弇
int NPCS_getUserPetAmount( int charaindex, int objID, int flg);
//�△譆葆沉D耋撿杅	*reAmount 呁牄戲弇
int NPCS_getUserItemAmount( int charaindex, int objID, int flg);
//�△襖韃蚧侕�
int NPCS_getUserPartyAmount( int charaindex );
BOOL NPCS_NpcstalkToCli( int charaindex,int npcobjindex, char* message, CHAR_COLOR color );
//岈璃揭燴
BOOL NPCS_RunDoEventAction( int charaindex, char *buf1);
#endif
#endif
