#ifndef _NPCS_HANDLE_H
#define _NPCS_HANDLE_H
#include "version.h"
#include "util.h"

#ifdef _NPCSERVER_NEW
//条件判断
BOOL NPCS_HandleCheckFreeMess( int npcobjindex, int charaindex, int charobjindex,
				char *CheckfreeMess);

BOOL NPCS_FreePassCheck( int charaindex, char *buf);

//比较判断
BOOL NPCS_FreeBigSmallCheck( int charaindex,char* buf);
//参数判断
BOOL NPCS_ArgumentFreeCheck( int charaindex, char* Argument, int amount, int temp, int probjID);
//参数大小判断
BOOL NPCS_ArgumentBigSmallCheck( int point, int mypoint, int flg);
//取得指定ID宠物数	*reAmount 剩馀栏位
int NPCS_getUserPetAmount( int charaindex, int objID, int flg);
//取得指定ID道具数	*reAmount 剩馀栏位
int NPCS_getUserItemAmount( int charaindex, int objID, int flg);
//取得团队人数
int NPCS_getUserPartyAmount( int charaindex );
BOOL NPCS_NpcstalkToCli( int charaindex,int npcobjindex, char* message, CHAR_COLOR color );
//事件处理
BOOL NPCS_RunDoEventAction( int charaindex, char *buf1);
#endif
#endif
