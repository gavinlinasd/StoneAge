#ifndef __NPC_ROOMADMINNEW_H__
#define __NPC_ROOMADMINNEW_H__


void NPC_RoomAdminNewTalked( int meindex , int talkerindex , char *msg , int color );
void NPC_RoomAdminNewLoop( int meindex );
BOOL NPC_RoomAdminNewInit( int meindex );

BOOL NPC_RankingInit( int meindex );
void NPC_RankingTalked( int meindex , int talkerindex , char *msg ,int color );

BOOL NPC_PrintpassmanInit( int meindex );
void NPC_PrintpassmanTalked( int meindex , int talkerindex , char *msg ,int color );

#if 0
typedef struct roomadmin_tag
{
    int index;              /*   碟摯index */
    
    char doorname[256];     /* 鼠摯  碟摯迶囮摯  鞳 */
    char explanation[256];  /*      */
    char passwd[256];       /* 蚕筒極↓迶 */
    int expire_time_mod;    /* 痾Ъ */
    time_t expire_time;     /* 趵藦憍躂摯鄹蹬 */
    int least_cost;         /*   黖鉣滌 */
    
}NPC_ROOMINFO;
#endif

typedef struct npc_roomadminnew_tag {
	int		expire;
	char	cdkey[CDKEYLEN];
	char	charaname[32];
	char	passwd[9];
}NPC_ROOMINFO;

typedef struct npc_roomadminnew_ranking_tag {
	int		gold;				/*   睊鉣滌 */
	int		biddate;			/*   睊鄹蹬 */
	char	cdkey[CDKEYLEN];	/*   敁媃倜硞摯    す↓ */
	char	charaname[32];		/*   敁媃倜硞摯  鞳 */
	char	owntitle[32];		/* 措蠕*/
	
}NPC_RANKING_INFO;

#endif /*__NPC_ROOMADMINNEW_H__*/

BOOL NPC_RoomAdminNew_ReadFile( char *roomname, NPC_ROOMINFO *data);
