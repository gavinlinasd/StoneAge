#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "net.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "npc_duelranking.h"
#include "handletime.h"

#ifdef _DEATH_CONTEND
#include "deathcontend.h"
#endif

#ifdef _DEATH_FAMILY_GM_COMMAND	// WON ADD 家族战GM指令
#include "family.h"
#endif

static void NPC_Duelranking_selectWindow( int meindex, int toindex, int num, int select );
#define		NPC_DUELRANKING_WINDOWLINENUM	10

#ifdef _DEATH_FAMILYCONTEND
BOOL NPC_ReadFamilyPK_List( void);
BOOL NPC_BackupFamilyPK_List( void);
BOOL NPC_getFamilyPK_List( int ti, char *buf, int sizes);
BOOL NPC_CheckJoin_FamilyPK_List( int meindex, int toindex);
#endif

enum{
	WINDOWTYPE_PKLIST_START=10,
	WINDOWTYPE_PKLIST_SELECT,
	WINDOWTYPE_PKLIST_SELECT1,
	WINDOWTYPE_PKLIST_SELECT2,

	WINDOWTYPE_FAMILYCONTENDSTART,
	WINDOWTYPE_FAMILYCONTENDSELECT,
	WINDOWTYPE_FAMILYCONTENDLIST,
	WINDOWTYPE_FAMILYCONTENDJOIN,

	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT1,
	NPC_WORK_BATTLELOOP = CHAR_NPCWORKINT2,
};

BOOL NPC_DuelrankingInit( int meindex )
{
#ifdef _DEATH_CONTEND
	char buf1[256];
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];

	CHAR_setWorkInt( meindex, NPC_WORK_BATTLELOOP, 0);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);

	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg) ) != NULL &&
		NPC_Util_GetStrFromStrWithDelim( npcarg, "PKLoop", buf1, sizeof( buf1)) != NULL ){
		CHAR_setInt( meindex, CHAR_LOOPINTERVAL, atoi(buf1));
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "PKLType", buf1, sizeof( buf1)) != NULL ){
			CHAR_setWorkInt( meindex, NPC_WORK_BATTLELOOP, atoi( buf1));
		}
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "PKLDelay", buf1, sizeof( buf1)) != NULL ){
			CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec + (atoi( buf1)*60) );
		}
	}
#endif

#ifdef _DEATH_FAMILYCONTEND
	{
		char buf1[256];
		char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
		CHAR_setWorkInt( meindex, NPC_WORK_BATTLELOOP, 0);
		CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
		if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg) ) != NULL &&
			NPC_Util_GetStrFromStrWithDelim( npcarg, "LoadFMList", buf1, sizeof( buf1)) != NULL ){
			if( strstr( buf1, "ON") != NULL )
				NPC_ReadFamilyPK_List();
		}
	}
#endif

	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEDUELRANKING );
	return TRUE;
}

void NPC_DuelrankingLooked( int meindex , int lookedindex)
{

#ifdef _DEATH_FAMILYCONTEND
	char buf1[256];
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];

	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg) ) != NULL &&
		NPC_Util_GetStrFromStrWithDelim( npcarg, "FmPkJoin", buf1, sizeof( buf1)) != NULL ){

		if( strstr( buf1, "ON") !=NULL ){
			CHAR_setWorkInt( lookedindex, CHAR_WORKSHOPRELEVANT, 0);
			NPC_Duelranking_selectWindow( meindex, lookedindex, 2, -1);
			return;
		}else{
			return;
		}
	}
#endif

#ifdef _DEATH_CONTEND
	char buf1[256];
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	//static int first = 1;
	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg) ) != NULL &&
		NPC_Util_GetStrFromStrWithDelim( npcarg, "PKCharts", buf1, sizeof( buf1)) != NULL ){
		if( strstr( buf1, "ON") !=NULL ){
			CHAR_setWorkInt( lookedindex, CHAR_WORKSHOPRELEVANT, 0);
			NPC_Duelranking_selectWindow( meindex, lookedindex, 2, -1);
			//if( first == 1 ) saacproto_PKLISTCHARTS_send( acfd, 2, 1);
			//first = 0;
			return;
		}else{
			return;
		}
	}
#endif

#ifdef _DEATH_FAMILY_GM_COMMAND	// WON ADD 家族战GM指令
	NPC_Duelranking_selectWindow( meindex, lookedindex, 2, -1);
#endif

	NPC_Duelranking_selectWindow( meindex, lookedindex, 1, -1);
}

static void NPC_Duelranking_selectWindow( int meindex, int toindex, int num, int select)
{
	
	int		fd;
	int		button;
	if( CHAR_getInt( toindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
		return;
	}
	if( NPC_Util_CharDistance( toindex, meindex ) > 1) return;
	if( (fd = getfdFromCharaIndex( toindex)) == -1 ) return;

	switch( num){
	case 1:
		{
			char	message[1024];
			char	buf[2048];
			strcpy( message, 
				"1\n"
				"             要看哪一个？              \n\n\n"
				"             ＜强者们＞              \n\n"
				"             ＜ 自己 ＞              \n\n"
				"             ＜都不看＞              \n\n" );
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT, 
							WINDOW_BUTTONTYPE_NONE,
							CHAR_WINDOWTYPE_DUELRANKING_START,
							CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							makeEscapeString( message, buf, sizeof(buf)));
		}
		break;
#ifdef _DEATH_CONTEND
	case 2:
		{
			char	message[1024];
			char	buf[2048];
			strcpy( message, 
				"1\n"
				"             要看哪一个？           \n\n"
//				"           ＜资格赛排行＞              \n"
				"          ＜ 正式赛程表 ＞   \n"
				"           ＜ 队伍资料 ＞  \n"
//				"           ＜ 参加赛程 ＞   \n"
				"           ＜ 赛程纪录 ＞   \n" );

			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT,
							WINDOW_BUTTONTYPE_CANCEL,
							WINDOWTYPE_PKLIST_START,
							CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							makeEscapeString( message, buf, sizeof(buf)));
		}
		break;
/*	case 3:
		{
			char token[4096], buf[256], buf1[256],buf2[256];
			char teamname[256];
			int win, lose, battle, score;
			int page, i, pagecount=10;
			page = CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT);
			button = WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_NEXT;

			if( select == WINDOW_BUTTONTYPE_PREV || select == WINDOW_BUTTONTYPE_NEXT) {
				if( select == WINDOW_BUTTONTYPE_NEXT ){
					if( page < 40 ) page++;
				}else if( select == WINDOW_BUTTONTYPE_PREV ){
					if( page > 0 )	page--;
				}
			}else if( select == WINDOW_BUTTONTYPE_OK ){
				CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 0);
				return;
			}
			if( page < 0 || page >= 40 ) page = 0;
			if( page == 0  ) button = WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_NEXT;
			if( page == 39 ) button = WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_PREV;
			CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, page);
			snprintf( token, sizeof( token), 
				"                 资格赛排行榜\n\n"
				"排名    队伍               胜  负  场 积分\n\n" );

			for( i=0; i<pagecount; i++ ){
				if( PKLIST_GetChartsListData( (page*pagecount)+1+i, buf, sizeof(buf) ) == FALSE )continue;

				if( !getStringFromIndexWithDelim( buf, "|", 1, teamname, sizeof( teamname)) ) continue;
				if( !getStringFromIndexWithDelim( buf, "|", 2, buf1, sizeof( buf1)) ) continue;
				win =atoi( buf1);
				if( !getStringFromIndexWithDelim( buf, "|", 3, buf1, sizeof( buf1)) ) continue;
				lose =atoi( buf1);
				if( !getStringFromIndexWithDelim( buf, "|", 4, buf1, sizeof( buf1)) ) continue;
				battle =atoi( buf1);
				if( !getStringFromIndexWithDelim( buf, "|", 5, buf1, sizeof( buf1)) ) continue;
				score =atoi( buf1);

				strcpy( buf2, "－");
				if( battle>= 30 && score > 0 ) strcpy( buf2, "★");
				sprintf( buf1, "%4dth %-18s %3d %3d %3d %3d %s\n", (page*pagecount)+1+i,
					teamname, win, lose, battle, score, buf2);

				strcat( token, buf1);
			}
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT, 
							button, WINDOWTYPE_PKLIST_SELECT, CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							token );
		}
		break;
*/
	case 5://晋级队伍
	case 3://正式赛程
		{
			char token[4096], buf1[256];
			char teamname1[256], teamname2[256], typestring[256];
			int tindex, dtimes, stimes=0;
			int page, i, pagecount=10;
			int winno = WINDOWTYPE_PKLIST_SELECT;

			page = CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT);
			button = WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_NEXT;

			if( select == WINDOW_BUTTONTYPE_PREV || select == WINDOW_BUTTONTYPE_NEXT) {
				if( select == WINDOW_BUTTONTYPE_NEXT ){
					if( page < 11 ) page++;
				}else if( select == WINDOW_BUTTONTYPE_PREV ){
					if( page > 0 )	page--;
				}
			}else if( select == WINDOW_BUTTONTYPE_OK ){
				CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 0);
				return;
			}
			if( page < 0 || page >= 11 ) page = 0;
			if( page == 0  ) button = WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_NEXT;
			if( page == 10 ) button = WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_PREV;
			CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, page);

			switch( num){
			case 3:
				snprintf( token, sizeof( token), 
					"         赛程表\n\n"
					"场次  队伍                         剩馀时间 状态\n\n" );
				break;
			case 5:
				snprintf( token, sizeof( token),	"  队伍名单&赛程纪录\n\n"
													"纪录  \n\n" );
				winno = WINDOWTYPE_PKLIST_SELECT2;
				break;
			}

			for( i=0; i<pagecount; i++ ){
				memset( teamname1, 0, sizeof(teamname1));
				memset( teamname2, 0, sizeof(teamname2));
				if( PKLIST_GetABattlelistDataString(
					(page*pagecount)+1+i, &tindex, &stimes, teamname1, teamname2, typestring, num) == FALSE )
					continue;

				switch( num){
				case 3:
					dtimes = (stimes-(int)time(NULL));
					if( dtimes<1 ) dtimes = 1;
					sprintf( buf1, "场%2d %-12s VS %-12s %2d分%2d秒 %s \n", tindex,
						teamname1, teamname2, (int)(dtimes/60), dtimes%60, typestring);
					break;
				case 5:
					if( strstr(teamname2, "NULL") != NULL ){
						sprintf( buf1, "%4d %-12s \n", tindex, teamname1);
					}else{
						sprintf( buf1, "%-12s vs %12s\n", teamname1, teamname2);
					}
					break;
				}
				strcat( token, buf1);
			}
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT, 
							button, winno, CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							token );
		}
		break;
	case 4:
		{
			char token[4096];
			int teanum = CHAR_getInt( toindex, CHAR_PKLISTTEAMNUM);

			button = WINDOW_BUTTONTYPE_OK;
			if( PKLIST_GetMyPKListTeamData( teanum, token, sizeof(token) ) == FALSE ) return;

			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, button, -1,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token );
		}
		break;
/*	case 6://加入赛程
		{
			int teamnum;
			{
				char buf1[256];
				char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
				if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg) ) != NULL &&
					NPC_Util_GetStrFromStrWithDelim( npcarg, "ServerType", buf1, sizeof( buf1)) != NULL ){
					if( strstr( buf1, "OFF") !=NULL ){
						return;
					}
				}else{
					return;
				}
			}
			if( (teamnum = CHAR_getInt( toindex, CHAR_PKLISTTEAMNUM)) < 0 ){
				//andy_log
				print("toindex:%d CHAR_PKLISTTEAMNUM:%d err !!\n", toindex, teamnum);
				return;
			}
			if( CHAR_getInt( toindex, CHAR_PKLISTLEADER) != 1 ) {
				CHAR_talkToCli( toindex, -1, "只有队长可以代表队伍加入赛程。", CHAR_COLORYELLOW);
				return;
			}
			if( PKLIST_JoinPKProcedures( toindex) == FALSE ){
				CHAR_talkToCli( toindex, -1, "加入赛程失败！。", CHAR_COLORYELLOW);
			}

		}
		break;
*/
#endif

#ifdef _DEATH_FAMILYCONTEND
	case 2:
		{
			char	message[1024];
			char	buf[2048];
			strcpy( message, 
				"1\n"
				"             要看哪一个？           \n\n"
				"          ＜家族资格排行＞              \n"
				"       ＜ 跨星系家族战报名 ＞   \n" );

			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT,
							WINDOW_BUTTONTYPE_CANCEL,
							WINDOWTYPE_FAMILYCONTENDSTART,
							CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							makeEscapeString( message, buf, sizeof(buf)));
		}
		break;
	case 3:
		{
			char token[4096], buf[512], buf1[256];
			char familyname[256], mname[256];
			int famindex=-1, joinE = 0;

			int page, i, pagecount=10;

			page = CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT);
			button = WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_NEXT;

			if( select == WINDOW_BUTTONTYPE_PREV || select == WINDOW_BUTTONTYPE_NEXT) {
				if( select == WINDOW_BUTTONTYPE_NEXT ){
					if( page < 4 ) page++;
				}else if( select == WINDOW_BUTTONTYPE_PREV ){
					if( page > 0 )	page--;
				}
			}else if( select == WINDOW_BUTTONTYPE_OK ){
				CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 0);
				return;
			}
			if( page < 0 || page >= 4 ) page = 0;
			if( page == 0  ) button = WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_NEXT;
			if( page == 3 ) button = WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_PREV;

			CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, page);
			snprintf( token, sizeof( token), 
				"          跨星系家族赛资格排行榜\n\n"
				"排名    家族名称          家族族长名称\n\n" );
			for( i=0; i<pagecount; i++ ){
				joinE = 0;

				if( NPC_getFamilyPK_List( (page*pagecount)+i, buf, sizeof(buf)) == FALSE ) continue;
//andy_log
print("FPKList[%d]:%s\n", i, buf);

				if( !getStringFromIndexWithDelim( buf, "|", 1, buf1, sizeof( buf1)) ) continue;
				famindex =atoi( buf1);
				if( !getStringFromIndexWithDelim( buf, "|", 2, familyname, sizeof( familyname)) ) continue;
				if( !getStringFromIndexWithDelim( buf, "|", 4, mname, sizeof( mname)) ) continue;
				if( !getStringFromIndexWithDelim( buf, "|", 5, buf1, sizeof( buf1)) ) continue;
				joinE = atoi( buf1);

				sprintf( buf1, "%2dth %-18s %-24s %s\n", (page*pagecount)+1+i,
					familyname, mname, (joinE == 1)?"★":"");


				strcat( token, buf1);
			}
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT, 
							button, WINDOWTYPE_FAMILYCONTENDSELECT, CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							token );
		}
		break;

	case 4:	//报名
		NPC_CheckJoin_FamilyPK_List( meindex, toindex);
		break;
#endif


#ifdef _DEATH_FAMILY_GM_COMMAND	// WON ADD 家族战GM指令
	case 2:
		{
			char	message[1024] = {0};
			char	buf[2048]= {0};
			strcpy( message, 
				"1\n"
				"             要看哪一个？           \n\n"
				"       ＜ 跨星系家族战排行 ＞   \n" );

			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT,
							WINDOW_BUTTONTYPE_CANCEL,
							WINDOWTYPE_PKLIST_START,
							CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							makeEscapeString( message, buf, sizeof(buf)));
		}
		break;
	case 3:
		{
            char token[4096];
            char buf1[256];
            int page, i, pagecount=10;
            char all_msg[400][128];
            FILE *fp=NULL;

			memset( token, 0, sizeof( token));
			page = CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT);
			button = WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_NEXT;
			if( select == WINDOW_BUTTONTYPE_PREV || select == WINDOW_BUTTONTYPE_NEXT) {
				if( select == WINDOW_BUTTONTYPE_NEXT ){
					if( page < 40 ) page++;
				}else if( select == WINDOW_BUTTONTYPE_PREV ){
					if( page > 0 )	page--;
				}
			}else if( select == WINDOW_BUTTONTYPE_OK ){
				CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 0);
				return;
			}
			if( page < 0 || page >= 40 ) page = 0;
			if( page == 0  ) button = WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_NEXT;
			if( page == 39 ) button = WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_PREV;
			CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, page);
			snprintf( token, sizeof( token), 
				"          跨星系家族战排行榜\n\n" );			


			for( i=0; i<40*10; i++){
				memset( all_msg[i] ,  0, sizeof( all_msg[i]));
			}

			if( (fp = fopen( "d_fm_pk_log.txt", "r" )) != NULL ){
				i=0;
				//andy_log
				print( "read d_fm_pk_log.txt !!\n");
				while( fgets( buf1, sizeof( buf1)-1, fp) ){
					//andy_log
					print( "read buf1:%s !!\n", buf1);
					if( buf1[0] == 0 || buf1[0] == '#' ||strlen(buf1) <= 0 ) continue;
					buf1[ strlen(buf1) -1] = 0;
					snprintf( all_msg[i], sizeof( all_msg[i]), "%s", buf1);
					i++;
				}
				fclose( fp );
			}else{
				//andy_log
				print( "Can't read d_fm_pk_log.txt !!\n");
			}
			
			for( i=0; i<pagecount; i++ ){		
				sprintf( buf1, "%s\n", all_msg[ (page*pagecount) + i ]);
				strcat( token, buf1);
			}
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT, 
							button, WINDOWTYPE_PKLIST_SELECT1, CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							token );
		}
		break;
#endif


	}
}

void NPC_DuelrankingWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{
	int fdid = getFdidFromCharaIndex( talkerindex);
	if( fdid == -1 ) return;

	if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;
	switch( seqno){
	case CHAR_WINDOWTYPE_DUELRANKING_START:
		if( atoi( data) == 3 ) {
			saacproto_DBGetEntryByCount_send( acfd, 
											DB_DUELPOINT, 0, 
								NPC_DUELRANKING_WINDOWLINENUM, fdid, 
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));
		}
		if( atoi( data) == 5 ) {
			char dbkey[256];
			CHAR_makeDBKey( talkerindex, dbkey, sizeof( dbkey));
			saacproto_DBGetEntryRank_send( acfd, DB_DUELPOINT, dbkey, fdid,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));
		}
		break;
	case CHAR_WINDOWTYPE_DUELRANKING_TOPRANKING:
		if( select == WINDOW_BUTTONTYPE_PREV || select == WINDOW_BUTTONTYPE_NEXT) {
			int	count = CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT);
			if( select == WINDOW_BUTTONTYPE_NEXT ) {
				count += NPC_DUELRANKING_WINDOWLINENUM;
			}else {
				count -= NPC_DUELRANKING_WINDOWLINENUM;
			}
	
			if( count < 0) count = 0;
			saacproto_DBGetEntryByCount_send( acfd, 
											DB_DUELPOINT, 
											count,
								NPC_DUELRANKING_WINDOWLINENUM, fdid,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));
		}else if( select == WINDOW_BUTTONTYPE_OK ) {
			if( strlen( data) != 0 ) {
				int count;
				count = atoi( data);
				if( count < 1 ) count = 1;
				saacproto_DBGetEntryByCount_send( acfd, 
												DB_DUELPOINT, 
												count-1,
									NPC_DUELRANKING_WINDOWLINENUM, fdid,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));
			}
		}
		break;
#ifdef _DEATH_CONTEND
	case WINDOWTYPE_PKLIST_START:
		{
			switch( atoi( data)){
			case 2://资格赛排行
				NPC_Duelranking_selectWindow( meindex, talkerindex, 3, select);
				break;
			case 3://正式赛程
				NPC_Duelranking_selectWindow( meindex, talkerindex, 4, select);
				break;
			case 4://队伍资料
				NPC_Duelranking_selectWindow( meindex, talkerindex, 5, select);
				break;
			case 5://加入赛程
				NPC_Duelranking_selectWindow( meindex, talkerindex, 6, -1);
				break;
			case 6://晋级队伍
				NPC_Duelranking_selectWindow( meindex, talkerindex, 7, select);
				break;
			}
		}
		break;
	case WINDOWTYPE_PKLIST_SELECT:
		NPC_Duelranking_selectWindow( meindex, talkerindex, 3, select);
		break;
	case WINDOWTYPE_PKLIST_SELECT1:
		NPC_Duelranking_selectWindow( meindex, talkerindex, 4, select);
		break;
	case WINDOWTYPE_PKLIST_SELECT2:
		NPC_Duelranking_selectWindow( meindex, talkerindex, 5, select);
		break;
#endif
#ifdef _DEATH_FAMILYCONTEND
	case WINDOWTYPE_FAMILYCONTENDSTART:
		{
			switch( atoi( data)){
			case 2://资格排行
				NPC_Duelranking_selectWindow( meindex, talkerindex, 3, select);
				break;
			case 3://报名
				NPC_Duelranking_selectWindow( meindex, talkerindex, 4, select);
				break;
			}
		}
		break;
	case WINDOWTYPE_FAMILYCONTENDSELECT:
		NPC_Duelranking_selectWindow( meindex, talkerindex, 3, select);
		break;
#endif

#ifdef _DEATH_FAMILY_GM_COMMAND	// WON ADD 家族战GM指令
	case WINDOWTYPE_PKLIST_START:
		{
			switch( atoi( data ) ){
			case 2:
				NPC_Duelranking_selectWindow( meindex, talkerindex, 3, select);
				break;
			}
		}
		break;
	case WINDOWTYPE_PKLIST_SELECT1:
		NPC_Duelranking_selectWindow( meindex, talkerindex, 3, select);
		break;
#endif


	}
}

void NPC_Duelranking_PrintRanking( char *data, int count_start, int fdid, int msgid2)
{
	char message[1024];
	char buf[2048];
	int		i;
	int	 cnt = 0;
	int	button = 0;	
	int fd = getfdFromFdid( fdid);
	if( fd == -1 ) return;
	message[0] = '\0';
	snprintf( message, sizeof( message), 
			"                        世界的强者们\n\n"
			"      Rank       DuelPoint      Lv     Name\n\n" );
	for( i = 1; ; i ++ ) {
		char	rankdata[256];
		char	cdandname[64];
		char	charaname[32];
		char	rank[16];
		char	score[64];
		char	msgbuf[512];
		char	cdkey[32];
		char	lv[16];
		char	hosi[6];
		
		if( !getStringFromIndexWithDelim( data, "|", i, rankdata, sizeof( rankdata))) {
			break;
		}
		if( !getStringFromIndexWithDelim( rankdata, ",", 2, rank, sizeof( rank)) ){
			continue;
		}
		if( !getStringFromIndexWithDelim( rankdata, ",", 3, cdandname, sizeof( cdandname)) ){			continue;
		}
		makeStringFromEscaped( cdandname);
		if( !getStringFromIndexWithDelim( cdandname, "_", 1, cdkey, sizeof( cdkey)) ){
			continue;
		}
		if( !getStringFromIndexWithDelim( cdandname, "_", 2, charaname, sizeof( charaname)) ){
			continue;
		}
		if( !getStringFromIndexWithDelim( rankdata, ",", 4, score, sizeof( score)) ){
			continue;
		}
		if( !getStringFromIndexWithDelim( rankdata, ",", 5, lv, sizeof( lv)) ){
			lv[0] = '\0';
		}
		{
			char *mycdkey = CHAR_getChar( CONNECT_getCharaindex(fd), CHAR_CDKEY);
			char *myname  = CHAR_getChar( CONNECT_getCharaindex(fd), CHAR_NAME);
			char engnum[4][3] = { "st","nd","rd","th"};
			int  dsprank = atoi(rank);
			if( dsprank > 3) dsprank = 3;
			if( mycdkey != NULL && myname != NULL && strcmp( cdkey, mycdkey) == 0 && 
				strcmp( charaname, myname) == 0 ) {
				strcpy( hosi, "★");
			}else {
				strcpy( hosi, "　");
			}
			snprintf( msgbuf, sizeof( msgbuf), "%s %5d%s      %10s     %3s     %s\n",
											hosi,
											atoi(rank)+1, engnum[dsprank],
											score, lv,
											charaname );
		}
		strcatsafe( message, sizeof( message), msgbuf);
		cnt ++;
	}
	if( cnt == 0 ) {
		strcpy( message, "\n      没有其他的资料了。");
		button = WINDOW_BUTTONTYPE_PREV|WINDOW_BUTTONTYPE_OK;
	}else if( cnt < NPC_DUELRANKING_WINDOWLINENUM ) {
		if( count_start >= NPC_DUELRANKING_WINDOWLINENUM) {
			button = WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_PREV;
		}else {
			button = WINDOW_BUTTONTYPE_OK;
		}
	}else if( count_start == 0 ) {
		button = WINDOW_BUTTONTYPE_NEXT|WINDOW_BUTTONTYPE_OK;
	}else {
		button = WINDOW_BUTTONTYPE_NEXT|WINDOW_BUTTONTYPE_PREV|WINDOW_BUTTONTYPE_OK;
	}
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT, 
					button,
					CHAR_WINDOWTYPE_DUELRANKING_TOPRANKING,
					msgid2,
					makeEscapeString( message, buf, sizeof(buf)));

	if( cnt == 0 ) {
		CHAR_setWorkInt( CONNECT_getCharaindex(fd), CHAR_WORKSHOPRELEVANT, -1);
	}else {
		CHAR_setWorkInt( CONNECT_getCharaindex(fd), CHAR_WORKSHOPRELEVANT, count_start);
	}
}

void NPC_Duelranking_CallMyRanking( int count, int msgid, int msgid2)
{
	count -= 5;
	if( count < 0 ) count = 0;
	saacproto_DBGetEntryByCount_send( acfd,  DB_DUELPOINT, 
									count, NPC_DUELRANKING_WINDOWLINENUM, 
									msgid, msgid2 );
}

#ifdef _DEATH_CONTEND
void NPC_Duelrankingloop( int meindex)
{
	static int side = 0;
	int i;

	if( CHAR_getWorkInt( meindex, NPC_WORK_CURRENTTIME) < NowTime.tv_sec ){
		char buf1[256];
		char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];

		CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);

		if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg) ) != NULL &&
			NPC_Util_GetStrFromStrWithDelim( npcarg, "PKLoop", buf1, sizeof( buf1)) != NULL ){
			CHAR_setInt( meindex, CHAR_LOOPINTERVAL, atoi(buf1));
			if( NPC_Util_GetStrFromStrWithDelim( npcarg, "PKLType", buf1, sizeof( buf1)) != NULL ){
				CHAR_setWorkInt( meindex, NPC_WORK_BATTLELOOP, atoi( buf1));
			}
			if( NPC_Util_GetStrFromStrWithDelim( npcarg, "PKLDelay", buf1, sizeof( buf1)) != NULL ){
				CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec + (atoi( buf1)*60) );
			}
		}
	}
	if( CHAR_getWorkInt( meindex, NPC_WORK_BATTLELOOP) == 0 ) return;

	for( i=0; i<MAXBATTLEPAGE; i++){
		ABATTLE_CheckInABattle( i);
	}
	if( (++side)%3 == 0 ){
		side = 0;
		//ABATTLE_CheckBattlefromFl();
		ABATTLE_GetRecordBattle();
	}
	ABATTLE_MakeInABattleString();
}
#endif

#ifdef _DEATH_FAMILYCONTEND
#define MAXJOINFM 35
typedef struct _tagProFamilyList
{
	int fmindex;
	char fmname[256];
	char ucdkey[256];
	char uname[256];
	int join;
	int use;

}ProFamilyList;

ProFamilyList JoinFamilyList[MAXJOINFM];

BOOL NPC_ReadFamilyPK_List( void)
{
	FILE *fp=NULL;
	int i, k=0;
	char arg[1024], buf1[256];

	for( i=0; i<MAXJOINFM; i++){
		JoinFamilyList[i].use = 0;
		JoinFamilyList[i].join = 0;
	}

	if( (fp=fopen("fmlist.txt", "r+")) == NULL ) return FALSE;

	while( fgets( arg, sizeof(arg), fp) ){
		if( arg[0] == '#' || strlen( arg) <= 0 ) continue;

		if( !getStringFromIndexWithDelim( arg, "|", 1, buf1, sizeof( buf1)) ) continue; //fmindex
		JoinFamilyList[k].fmindex = atoi( buf1);

		if( !getStringFromIndexWithDelim( arg, "|", 2, buf1, sizeof( buf1)) ) continue;
		snprintf( JoinFamilyList[k].fmname, sizeof(JoinFamilyList[k].fmname), "%s", buf1);

		if( !getStringFromIndexWithDelim( arg, "|", 3, buf1, sizeof( buf1)) ) continue;
		snprintf( JoinFamilyList[k].uname, sizeof(JoinFamilyList[k].uname), "%s", buf1);

		if( !getStringFromIndexWithDelim( arg, "|", 4, buf1, sizeof( buf1)) ) continue;
		snprintf( JoinFamilyList[k].ucdkey, sizeof(JoinFamilyList[k].ucdkey), "%s", buf1);

		if( !getStringFromIndexWithDelim( arg, "|", 5, buf1, sizeof( buf1)) ) continue;
		JoinFamilyList[k].join = atoi( buf1);

		JoinFamilyList[k].use = 1;

print( "JFM[%d]:%d|%s|%s|%s|%d|\n",
	  k, JoinFamilyList[k].fmindex, JoinFamilyList[k].fmname,
	  JoinFamilyList[k].ucdkey, JoinFamilyList[k].uname, JoinFamilyList[k].join );

		k++;
		if( k >= MAXJOINFM ) break;
	}
	fclose( fp);
	return TRUE;
}

BOOL NPC_BackupFamilyPK_List( void)
{
	FILE *fp=NULL;
	int i;

	if( (fp=fopen("fmlist.txt", "w+")) == NULL ) return FALSE;
	for( i=0; i<MAXJOINFM; i++){
		if( JoinFamilyList[i].use == 0 ) continue;
		fprintf( fp, "%d|%s|%s|%s|%d|\n",
			JoinFamilyList[i].fmindex , JoinFamilyList[i].fmname ,
			JoinFamilyList[i].uname , JoinFamilyList[i].ucdkey , 
			JoinFamilyList[i].join );
	}

	fclose( fp);
	return TRUE;
}

BOOL NPC_getFamilyPK_List( int ti, char *buf, int sizes)
{
	//andy_log
	print("ti:%d\n", ti);
	if( ti < 0 || ti > MAXJOINFM ) return FALSE;
	if( JoinFamilyList[ti].use == 0 ) return FALSE;

	snprintf( buf, sizes, "%d|%s|%s|%s|%d|",
	  JoinFamilyList[ti].fmindex, JoinFamilyList[ti].fmname,
	  JoinFamilyList[ti].ucdkey, JoinFamilyList[ti].uname, JoinFamilyList[ti].join );

	return TRUE;
}

BOOL NPC_CheckJoin_FamilyPK_List( int meindex, int toindex)
{
	int i;

	//andy_log
	print( "  *check: [%d,%s,%s]\n", CHAR_getInt( toindex, CHAR_FMINDEX),
		CHAR_getChar( toindex, CHAR_CDKEY), CHAR_getChar( toindex, CHAR_NAME) );

	for( i=0; i<MAXJOINFM; i++){
		if( JoinFamilyList[i].use == 0 ) continue;

	//andy_log
	print( "check[%d]: [%d,%s,%s]\n",
		i, JoinFamilyList[i].fmindex, 
		JoinFamilyList[i].ucdkey, JoinFamilyList[i].uname );

		if( JoinFamilyList[i].fmindex !=  CHAR_getInt( toindex, CHAR_FMINDEX) ) continue;

		if( CHAR_getInt( toindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER ) {
			if( strcmp( CHAR_getChar( toindex, CHAR_CDKEY), JoinFamilyList[i].ucdkey ) ||
				strcmp( CHAR_getChar( toindex, CHAR_NAME), JoinFamilyList[i].uname ) )
				continue;
		}

		if( JoinFamilyList[i].join == 1 ){
			CHAR_talkToCli( toindex, -1, "已经报名参加了。", CHAR_COLORYELLOW);
			return FALSE;
		}else{
			CHAR_talkToCli( toindex, -1, "报名完成。", CHAR_COLORYELLOW);
			JoinFamilyList[i].join = 1;
			NPC_BackupFamilyPK_List();
			return TRUE;
		}
	}
	CHAR_talkToCli( toindex, -1, "无法报名，请确认你是否为资格名单上之家族族长。", CHAR_COLORYELLOW);
	return FALSE;
}
//CHAR_FMINDEX 
#endif
