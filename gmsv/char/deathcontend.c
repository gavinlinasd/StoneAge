#include "version.h"
#include <string.h>
#include <math.h>
#include "util.h"
#include "char.h"
#include "char_data.h"
#include "char_base.h"
#include "battle.h"
#include "configfile.h"
#include "saacproto_cli.h"
#include "buf.h"
#include "log.h"

#ifdef _DEATH_CONTEND

#include "deathcontend.h"

PkTeamLists *PkTeamList=NULL;

PKProcedures PKProcedure[MAXJOINTEAM];

static char chartslist[65536*3];

static char ABattlelist[65536*2];
static char ABLevelup[65536*2];

static int maxteam = 0;

static int PKListSort[550];

//正式赛
ArrangeBattleC *NowABattle[MAXBAHEAD][MAXBAFLOOR][MAXNOWBATTLE];
ArrangeBattleC *headABattle[MAXBAHEAD];
ArrangeBattleC *InBattle[MAXBATTLEPAGE];

static int maxfloor=0;
static int maxbati=0;
int ABFloor[MAXBAHEAD];

/*
struct {
	int floor;
	int x;
	int y;
	int use;
}PK_WATCHMAP[MAXJOINTEAM][MAXWATCHMAP]={
	{},


};
*/
struct {
	int floor;
	int x;
	int y;
	int use;
}PK_BATTLEMAP[MAXJOINTEAM]={
//	{ 8240, 13, 14, 0}, { 8240, 13, 27, 0}, { 8240, 26, 14, 0}, { 8240, 26, 27, 0},

	{ 8240, 13, 14, 0}, { 8240, 13, 27, 0}, { 8240, 26, 14, 0}, { 8240, 26, 27, 0},
	{ 8241, 13, 14, 0}, { 8241, 13, 27, 0}, { 8241, 26, 14, 0}, { 8241, 26, 27, 0},
	{ 8242, 13, 14, 0}, { 8242, 13, 27, 0}, { 8242, 26, 14, 0}, { 8242, 26, 27, 0},
	{ 8243, 13, 14, 0}, { 8243, 13, 27, 0}, { 8243, 26, 14, 0}, { 8243, 26, 27, 0},
	{ 8244, 13, 14, 0}, { 8244, 13, 27, 0}, { 8244, 26, 14, 0}, { 8244, 26, 27, 0},

	{ 8245, 13, 14, 0}, { 8245, 13, 27, 0}, { 8245, 26, 14, 0}, { 8245, 26, 27, 0},
	{ 8246, 13, 14, 0}, { 8246, 13, 27, 0}, { 8246, 26, 14, 0}, { 8246, 26, 27, 0},
	{ 8247, 13, 14, 0}, { 8247, 13, 27, 0}, { 8247, 26, 14, 0}, { 8247, 26, 27, 0},
	{ 8248, 13, 14, 0}, { 8248, 13, 27, 0}, { 8248, 26, 14, 0}, { 8248, 26, 27, 0},
	{ 8249, 13, 14, 0}, { 8249, 13, 27, 0}, { 8249, 26, 14, 0}, { 8249, 26, 27, 0},

};

void PKLIST_shoutToAll( char *token, int teamnum, int floor)
{
	int i;
    int playernum = CHAR_getPlayerMaxNum();

	LogPkContend( token, "shoutToAll", 0, 0, 0, 1);

    for( i=0 ; i<playernum ; i++) {
        if( CHAR_getCharUse(i) == FALSE ) continue;
		//if( floor == -1 || floor == CHAR_getInt( i, CHAR_FLOOR) )
		//if( teamnum == -1 || teamnum == CHAR_getInt( i, CHAR_PKLISTTEAMNUM) )
			CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW);
	}
}

void del_rn( char *s )
{
	int i;
	for(i=0;;i++){
		if( s[i] == '\r' || s[i] == '\n' ) s[i] = 0;
		if( s[i] == 0 )break;
	}
}

//勿动-------------------------------------------------------
void PKLIST_ResetOneTeamMan( int ti )
{
	int k;
	if( ti < 0 || ti >= maxteam ) return;
	for( k=0;k<MAXTEAMMANNUM;k++){
		PkTeamList[ti].MyTeamMans[k].use = 0;
		memset( PkTeamList[ti].MyTeamMans[k].cdkey, 0,
			sizeof( PkTeamList[ti].MyTeamMans[k].cdkey) );
		memset( PkTeamList[ti].MyTeamMans[k].name, 0,
			sizeof( PkTeamList[ti].MyTeamMans[k].name) );
	}
}

void PKLIST_ResetOneBHistory( int ti )
{
	int k;
	for( k=0; k<MAXBATTLENUM; k++){
		PkTeamList[ti].BHistory[k].use = 0;
		PkTeamList[ti].BHistory[k].teamnum = -1;
		PkTeamList[ti].BHistory[k].flg = 1;
	}
}

void PKLIST_ResetOnePkTeamList( int ti )
{
	if( ti < 0 || ti >= maxteam ) return;
	PkTeamList[ti].use = 0;
	PkTeamList[ti].teamnum = -1;
	memset( PkTeamList[ti].teamname, 0, sizeof( PkTeamList[ti].teamname ) );
	memset( PkTeamList[ti].pathdir, 0, sizeof( PkTeamList[ti].pathdir) );
	memset( PkTeamList[ti].leadercdkey, 0, sizeof( PkTeamList[ti].leadercdkey ) );
	PkTeamList[ti].win = 0;
	PkTeamList[ti].lost = 0;
	PkTeamList[ti].battleplay = 0;
	PkTeamList[ti].score = 0;
	PkTeamList[ti].inside = 1;
	PkTeamList[ti].read = 1;
}
//-----------------------------------------------------------

int PKLIST_InitPkTeamList( int teamnum )
{
	int i;
	if( PkTeamList != NULL || teamnum <= 0 ) return -1;
	maxteam = teamnum;
	PkTeamList = (PkTeamLists *)calloc( 1, sizeof( struct _tagPkTeamLists) * maxteam );
	if( PkTeamList == NULL ){
		print( "ANDY err Can't calloc PkTeamList :%d !!\n",
			sizeof( struct _tagPkTeamLists) * maxteam );
		return -1;
	}
	print( "PkTeamList calloc size :%d ..\n",
		sizeof( struct _tagPkTeamLists) * maxteam );

	for( i=0; i<maxteam; i++ ){
		PKLIST_ResetOnePkTeamList( i );
		PKLIST_ResetOneTeamMan( i);
		PKLIST_ResetOneBHistory( i);
	}

	PKLIST_LoadInitPkTeamListData();

	memset( PKListSort, -1, sizeof(PKListSort));
	memset( chartslist, 0, sizeof(chartslist));
	//saacproto_LoadPkTeamListData_send( acfd , 0,  1000);

	for( i=0; i<MAXJOINTEAM; i++ ){
		PKProcedure[i].use = 0;
		PKProcedure[i].type = PKTYPE_NONE;
		PKProcedure[i].time = 0;


		PKProcedure[i].Team[0].use = 0;
		PKProcedure[i].Team[0].fd = -1;
		PKProcedure[i].Team[0].teamnum = -1;
		PKProcedure[i].Team[1].use = 0;
		PKProcedure[i].Team[1].teamnum = -1;
		PKProcedure[i].Team[1].fd = -1;
	}
#ifdef _DEATH_CONTENDAB
	ABATTLE_InitABattle(19);
#endif
	PKLIST_LoadPkTeamListData();	// 本来向ac要,改为直接读档
	PKLIST_Sort_PKListSort();

	return 1;
}


int PKLIST_GetPkTeamListArrayFromNum( int teamnum)
{
	int i;
	for( i=0; i<maxteam; i++ ){
		if( PkTeamList[i].use != 1 ) continue;
		if( PkTeamList[i].teamnum == teamnum )return i;
	}
	return -1;
}

int PKLIST_GetPkTeamListArray( int teamnum, char *cdkey)
{
	int i;
	for( i=0; i<maxteam; i++ ){
		if( PkTeamList[i].use != 1 ) continue;
		if( PkTeamList[i].teamnum != teamnum ) continue;
		if( !strcmp( PkTeamList[i].leadercdkey, cdkey) ) return i;
	}
	return -1;
}

//确认约战
BOOL PKLIST_CHECKPkTeamSame( int teamnum, int charaindex, char *cdkey, int toteamnum )
{
	int k;
	int array;
	if( !CHAR_CHECKINDEX( charaindex ) ) return FALSE;
	array = PKLIST_GetPkTeamListArray( teamnum, cdkey);
	if( array == -1 ) return FALSE;
	for( k=0; k<MAXBATTLENUM; k++){
		if( k >= DEFMAXBATTLENUM ){
			CHAR_talkToCli( charaindex, -1, "对战场次已满！", CHAR_COLORYELLOW);
			return FALSE;
		}
		if( PkTeamList[array].BHistory[k].use == 0 ) continue;
		if( PkTeamList[array].BHistory[k].teamnum == toteamnum ) return FALSE;
	}
	return TRUE;
}

int PKLIST_CHECKPkTeamNew( int teamnum, int charaindex, char *cdkey )
{
	int k;
	int array;
	array = PKLIST_GetPkTeamListArray( teamnum, cdkey);
	if( array == -1 ){
		CHAR_talkToCli( charaindex, -1, "无法取得队伍资料！", CHAR_COLORYELLOW);
		return -1;
	}
	for( k=0; k<MAXBATTLENUM; k++){
		if( PkTeamList[array].BHistory[k].use == 1 ) continue;
		break;
	}
	if( k>= DEFMAXBATTLENUM ){
		CHAR_talkToCli( charaindex, -1, "对战场次已满！", CHAR_COLORYELLOW);
		return -1;
	}
	return k;
}

BOOL PKLIST_LoadPkTeamListDataSub( int ti, char *data)
{
	int oldinside = -1;
	char buf[512];
	if( getStringFromIndexWithDelim( data, "|", 1, buf, sizeof( buf)) == FALSE  ) return FALSE;
		PkTeamList[ti].teamnum = atoi( buf);
	if( getStringFromIndexWithDelim( data, "|", 2, buf, sizeof( buf)) == FALSE  ) return FALSE;
		sprintf( PkTeamList[ti].pathdir, "%s", buf);
	if( getStringFromIndexWithDelim( data, "|", 3, buf, sizeof( buf)) == FALSE  ) return FALSE;
		sprintf( PkTeamList[ti].teamname, "%s", buf);
	if( getStringFromIndexWithDelim( data, "|", 4, buf, sizeof( buf)) == FALSE  ) return FALSE;
		sprintf( PkTeamList[ti].leadercdkey, "%s", buf);
	if( getStringFromIndexWithDelim( data, "|", 5, buf, sizeof( buf)) == FALSE  ) return FALSE;
		PkTeamList[ti].win = atoi( buf);
	if( getStringFromIndexWithDelim( data, "|", 6, buf, sizeof( buf)) == FALSE  ) return FALSE;
		PkTeamList[ti].lost = atoi( buf);
	if( getStringFromIndexWithDelim( data, "|", 7, buf, sizeof( buf)) == FALSE  ) return FALSE;
		PkTeamList[ti].battleplay = atoi( buf);
	if( getStringFromIndexWithDelim( data, "|", 8, buf, sizeof( buf)) == FALSE  ) return FALSE;
		PkTeamList[ti].score = atoi( buf);

	oldinside = PkTeamList[ti].inside;
	if( getStringFromIndexWithDelim( data, "|", 9, buf, sizeof( buf)) == FALSE  ) return FALSE;
		PkTeamList[ti].inside = atoi( buf);

	if( PkTeamList[ti].inside == 2 && PkTeamList[ti].inside != oldinside ){//广播
		char token[256];
		int i;
		int playernum = CHAR_getPlayerMaxNum();
		sprintf( token, "%s 队，达成比赛规定积分，积分：%d。",
			PkTeamList[ti].teamname, PkTeamList[ti].score );

		for( i=0; i<playernum; i++){
			if( CHAR_getCharUse(i) &&
				CHAR_getInt( i , CHAR_WHICHTYPE)  == CHAR_TYPEPLAYER  &&
				CHAR_getInt( i , CHAR_FLOOR ) == 8250 ){
				CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
			}
		}
	}

	PKLIST_UNLOCKTeam( PkTeamList[ti].teamnum);
	PkTeamList[ti].use = 1;

	return TRUE;
}

BOOL PKLIST_LoadPkTeamListDataMyTeamMans( int ti, char *data)
{
	int k=1, count=0;
	char buf[256], buf1[64];

	while( getStringFromIndexWithDelim( data, "|", k, buf, sizeof( buf)) !=FALSE )	{
		k++;
		if( getStringFromIndexWithDelim( buf, ",", 1, buf1, sizeof( buf1)) == FALSE  ) continue;
		sprintf( PkTeamList[ti].MyTeamMans[count].cdkey, "%s", buf1);
		if( getStringFromIndexWithDelim( buf, ",", 2, buf1, sizeof( buf1)) == FALSE  ) continue;
		sprintf( PkTeamList[ti].MyTeamMans[count].name, "%s", buf1);
		PkTeamList[ti].MyTeamMans[count].use = 1;
		count++;
		if( count >= MAXTEAMMANNUM ) break;
	}
	if( count <= 0 ) return FALSE;

	return TRUE;
}

BOOL PKLIST_LoadPkTeamListDataBHistory( int ti, char *data)
{
	int k=1, count=0;
	char buf[256], buf1[64];
	while( getStringFromIndexWithDelim( data, "|", k, buf, sizeof( buf)) !=FALSE )	{
		k++;
		if( getStringFromIndexWithDelim( buf, ",", 1, buf1, sizeof( buf1)) == FALSE  ) continue;
		PkTeamList[ti].BHistory[count].teamnum = atoi( buf1);
		if( getStringFromIndexWithDelim( buf, ",", 2, buf1, sizeof( buf1)) == FALSE  ) continue;
		PkTeamList[ti].BHistory[count].flg = atoi( buf1);

		PkTeamList[ti].BHistory[count].use = 1;
		count++;
		if( count >= MAXBATTLENUM ) break;
	}
	return TRUE;
}

// teamlist.ini 内容
// teamnum|teamname|leadercdkeky|leadername|cdkey|name|cdkey|name|cdkey|name|cdkey|name

void PKLIST_LoadInitPkTeamListData(void)
{
	FILE *fp = NULL;
	int count = 0,i;
	char szReadData[1024],buf[1024],buf1[1024];

	if((fp = fopen("data//pkdata//teamlist.txt","r")) != NULL){
		fclose(fp);
		return;
	}
	if((fp = fopen("data//pkdata//teamlist.ini","r")) == NULL){
		print("can't find data/pkdata/teamlist.ini\n");
		return;
	}
	while(1){
		memset(szReadData,0,sizeof(szReadData));
		memset(buf,0,sizeof(buf));
		if(fgets(szReadData,1024,fp) == NULL) break;
		if(getStringFromIndexWithDelim(szReadData,"|",1,buf,sizeof(buf)) == FALSE){
			print("err get teamindex data:%s\n",szReadData);
			continue;
		}
		PkTeamList[count].teamnum = atoi(buf);
		sprintf(PkTeamList[count].pathdir,"NOUSE");
		if(getStringFromIndexWithDelim(szReadData,"|",2,buf,sizeof(buf)) == FALSE){
			print("err get teamindex data:%s\n",szReadData);
			continue;
		}
		sprintf(PkTeamList[count].teamname,"%s",buf);
		if(getStringFromIndexWithDelim(szReadData,"|",3,buf,sizeof(buf)) == FALSE){
			print("err get leadercdkey data:%s\n",szReadData);
			continue;
		}
		sprintf(PkTeamList[count].leadercdkey,"%s",buf);
		for(i=0;i<MAXTEAMMANNUM;i++){
			if(getStringFromIndexWithDelim(szReadData,"|",3+i*2,buf,sizeof(buf)) == FALSE){
				if(i == 0) print("err team has no leader data:%s\n",szReadData);
				break;
			}
			if(getStringFromIndexWithDelim(szReadData,"|",4+i*2,buf1,sizeof(buf1)) == FALSE) break;
			sprintf(PkTeamList[count].MyTeamMans[i].cdkey,"%s",buf);
			sprintf(PkTeamList[count].MyTeamMans[i].name,"%s",buf1);
			PkTeamList[count].MyTeamMans[i].use = 1;
		}
		
		PKLIST_UNLOCKTeam(PkTeamList[count].teamnum);
		PkTeamList[count].use = 1;
		count++;
	}
	fclose(fp);
	printf("get %d teamlist.ini data\n",count);
	PKLIST_SavePkTeamListData();
}

// teamlist.txt 内容
// teamindex_teamnum|pathdir|teamname|leadercdkeky|win|lost|battleplay|score|inside_cdkey,name|...|cdkey,name|_teamnum,flg|...|teamnum,flg
//																																									|--------最多五笔--------| |--------最多一百笔-------|
BOOL PKLIST_LoadPkTeamListData(void) // 改成读档
{
	FILE *fp = NULL;
	int array = -1,count = 0;
	char szReadData[10240],buf[10240];
	char teamindex[8];

	if((fp = fopen("data//pkdata//teamlist.txt","r")) == NULL){
		print("can't find data/pkdata/teamlist.txt\n");
		return FALSE;
	}
	while(1){
		memset(szReadData,0,sizeof(szReadData));
		memset(buf,0,sizeof(buf));
		if(fgets(szReadData,10240,fp) == NULL) break;
		if(getStringFromIndexWithDelim(szReadData,"_",1,teamindex,sizeof(teamindex)) == FALSE){
			print("err get teamindex data:%s line:%d\n",szReadData,count++);
			continue;
		}
		array = atoi(teamindex);
		getStringFromIndexWithDelim(szReadData,"_",2,buf,sizeof(buf));
		if(PKLIST_LoadPkTeamListDataSub(array,buf) == FALSE){
			print("err set PkTeamList data:%s line:%d\n",szReadData,count++);
			continue;
		}
		getStringFromIndexWithDelim(szReadData,"_",3,buf,sizeof(buf));
		if( PKLIST_LoadPkTeamListDataMyTeamMans(array,buf) == FALSE){
			print("err set PkTeamList.MyTeamMans data:%s line:%d\n",szReadData,count++);
			continue;
		}
		if(getStringFromIndexWithDelim(szReadData,"_",4,buf,sizeof(buf)) != FALSE){
			if(PKLIST_LoadPkTeamListDataBHistory(array,buf) == FALSE){
				print("err set PkTeamList.BHistory data:%s line:%d\n",szReadData,count++);
				continue;
			}
		}
		count++;
	}
	fclose(fp);
	printf("get %d teamlist data\n",count);

	return TRUE;
}

void PKLIST_SavePkTeamListData(void)
{
	FILE *fp = NULL;
	int i,j;
	char szWriteData[1024],szWriteData2[2048],szTemp[128];

	if((fp = fopen("data//pkdata//teamlist.txt","w")) == NULL){
		print("can't open data/pkdata/teamlist.txt\n");
		return;
	}
	for(i=0;i<MAXTEAMNUM;i++){
		if(PkTeamList[i].use != 0){
			memset(szWriteData,0,sizeof(szWriteData));
			for(j=0;j<MAXTEAMMANNUM;j++){
				if(PkTeamList[i].MyTeamMans[j].use != 0){
					strcat(szWriteData,PkTeamList[i].MyTeamMans[j].cdkey);
					strcat(szWriteData,",");
					strcat(szWriteData,PkTeamList[i].MyTeamMans[j].name);
					strcat(szWriteData,"|");
				}
			}
			szWriteData[strlen(szWriteData)-1] = 0;
			memset(szWriteData2,0,sizeof(szWriteData2));
			memset(szTemp,0,sizeof(szTemp));
			for(j=0;j<MAXBATTLENUM;j++){
				if(PkTeamList[i].BHistory[j].use != 0){
					sprintf(szTemp,"%d,%d|",PkTeamList[i].BHistory[j].teamnum,PkTeamList[i].BHistory[j].flg);
					strcat(szWriteData2,szTemp);
				}
			}
			fprintf(fp,"%d_%d|%s|%s|%s|%d|%d|%d|%d|%d|_%s_%s\n",
							i,PkTeamList[i].teamnum,"NOUSE",PkTeamList[i].teamname,PkTeamList[i].leadercdkey,
							PkTeamList[i].win,PkTeamList[i].lost,PkTeamList[i].battleplay,PkTeamList[i].score,
							PkTeamList[i].inside,szWriteData,szWriteData2);
		}
	}
	fclose(fp);
}

/*BOOL PKLIST_LoadPkTeamListData( char *data)
{
	int k=1, array=-1, teamcount=0;
	char buf[10240], buf1[512];

	if( PkTeamList == NULL || data == NULL ){
		//andy_log
		print(" ANDY PkTeamList=NULL || data = NULL \n");
		return FALSE;
	}
		
	while( getStringFromIndexWithDelim( data, " ", k, buf, sizeof( buf)) !=FALSE )	{
		k++;
		print("\n read buf:%s\n", buf);
		if( getStringFromIndexWithDelim( buf, "_", 1, buf1, sizeof( buf1)) == FALSE  ) continue;
		array = atoi( buf1);
		if( array<0 || array>=maxteam ){
			print("err 1 buf:%s\n", buf);
			continue;
		}
		if( getStringFromIndexWithDelim( buf, "_", 2, buf1, sizeof( buf1)) == FALSE  ){
			print("err 2 buf:%s\n", buf);
			continue;
		}
		if( PKLIST_LoadPkTeamListDataSub( array, buf1 ) == FALSE ){
			print("err 3 buf:%s\n", buf);
			continue;
		}
		if( getStringFromIndexWithDelim( buf, "_", 3, buf1, sizeof( buf1)) == FALSE  ){
			print("err 4 buf:%s\n", buf);
			continue;
		}
		if( PKLIST_LoadPkTeamListDataMyTeamMans( array, buf1) == FALSE ){
			print("err 5 buf:%s\n", buf);
			continue;
		}
		if( getStringFromIndexWithDelim( buf, "_", 4, buf1, sizeof( buf1)) != FALSE  ){
			if( PKLIST_LoadPkTeamListDataBHistory( array, buf1) == FALSE ){
				print("err 7 buf:%s\n", buf);
				continue;
			}
		}

		teamcount++;
	}
	//andy_log
	print("\nteamcount:%d \n", teamcount);
	if( teamcount <= 0 ) return TRUE;
	return TRUE;
}
*/

void PKLIST_InsertTeamNum( int charaindex )
{
	int i, k;
	char *cdkey, *name;
	if( !CHAR_CHECKINDEX( charaindex) ) return;
	cdkey = CHAR_getChar( charaindex, CHAR_CDKEY);
	name = CHAR_getChar( charaindex, CHAR_NAME);

	for( i=0; i<maxteam; i++ ){
		if( PkTeamList[i].use == 0 ) continue;
		for( k=0;k<MAXTEAMMANNUM;k++){
			if( PkTeamList[i].MyTeamMans[k].use == 0 ) continue;
			if( strcmp( PkTeamList[i].MyTeamMans[k].cdkey, cdkey) )continue;
//			if( !strcmp( PkTeamList[i].MyTeamMans[k].name, name) ){

				CHAR_setInt( charaindex, CHAR_PKLISTTEAMNUM, PkTeamList[i].teamnum );
				if( !strcmp( PkTeamList[i].leadercdkey, cdkey) ){
					CHAR_setInt( charaindex, CHAR_PKLISTLEADER, 1);
				}else{
					CHAR_setInt( charaindex, CHAR_PKLISTLEADER, 0);
				}
				//andy_log
				print("teamnum:%d leadercdkey:%d ok\n",
					PkTeamList[i].teamnum, CHAR_getInt( charaindex, CHAR_PKLISTLEADER));
				return;
//			}
		}
	}

	return;
}

void PKLIST_ShowPkListTeamData( void)
{
	int i;
	{
		int k;
		print( "\n");
		for( i=0; i<maxteam; i++ ){
			if( PkTeamList[i].use != 1 ) continue;
			print("PkTeamList[%d] [%d|%d|%s|%s|%s|%d|%d|%d|%d|%d] \n", i,
				PkTeamList[i].use,
				PkTeamList[i].teamnum,
				PkTeamList[i].teamname,
				PkTeamList[i].pathdir,
				PkTeamList[i].leadercdkey,
				PkTeamList[i].win,
				PkTeamList[i].lost,
				PkTeamList[i].battleplay,
				PkTeamList[i].score,
				PkTeamList[i].inside
			);
			for( k=0; k<MAXTEAMMANNUM; k++){
				if( PkTeamList[i].MyTeamMans[k].use == 0 ) continue;
				print( "[%s,%s]," , PkTeamList[i].MyTeamMans[k].cdkey,
						PkTeamList[i].MyTeamMans[k].name );
			}
			print( "\n");
			for( k=0; k<MAXBATTLENUM; k++){
				if( PkTeamList[i].BHistory[k].use == 0 ) continue;
				if( k!=0 && k%10 == 0 ) print( "\n");
				print( "%d,%d|" , PkTeamList[i].BHistory[k].teamnum,
						PkTeamList[i].BHistory[k].flg );
			}
			print( "\n");
		}
		print( "\n");
	}

}

BOOL PKLIST_GetTeamLeaderCdkey( int teamnum, char *buf)
{
	int i;
	for( i=0; i<maxteam; i++ ){
		if( PkTeamList[i].use == 0 ) continue;
		if( PkTeamList[i].teamnum != teamnum ) continue;
		sprintf( buf, "%s", PkTeamList[i].leadercdkey );
		return TRUE;
	}
	return FALSE;
}

void NPC_PKLIST_PlayerLogout_Exit( int charaindex )
{
	char token[256];
	int menum, tonum;

#ifdef _DEATH_CONTENDAB
	{
		int i,j,side;
		char tocdkey[256], mecdkey[256];
		int mearray, toarray,battlearray;
		ArrangeBattleC *aBo=NULL;
		ArrangeBattleC *aB1=NULL;
		ArrangeBattleC *aB2=NULL;

		if( (menum=CHAR_getInt( charaindex, CHAR_PKLISTTEAMNUM)) < 0 ) return;
		if( (mearray = PKLIST_GetPkTeamListArrayFromNum( menum)) == -1 ||
			PKLIST_GetTeamLeaderCdkey( menum, mecdkey ) == FALSE ){
			print("Can't Get Data menum:%d err !!\n", menum);
			return;
		}
		if( (battlearray = PKLIST_GetPKProcedureArray( menum)) < 0 ) return;

		for( j=0; j<2; j++ ){
			if( PKProcedure[battlearray].Team[j].use == 0 ) continue;
			if( PKProcedure[battlearray].Team[j].teamnum != menum ) continue;
			
			if( PKProcedure[battlearray].type != PKTYPE_STANDBY ) return;
			if( PKProcedure[battlearray].time+1 >= (int)time(NULL) ) return;
			side = (j+1)%2;
			
			if( PKProcedure[battlearray].Team[side].use == 0 ) return;
			if( (tonum = PKProcedure[battlearray].Team[side].teamnum ) < 0 ) return;
			
			if( (toarray = PKLIST_GetPkTeamListArrayFromNum( tonum)) == -1 ||
				PKLIST_GetTeamLeaderCdkey( tonum, tocdkey ) == FALSE ){
				print("Can't Get Data tonum:%d err !!\n", tonum);
				return;
			}
			break;
		}
		//andy_log
		print("NPC_PKLIST_PlayerLogout_Exit( %d) \n", charaindex);
		if( (menum=CHAR_getInt( charaindex, CHAR_PKLISTTEAMNUM)) < 0 ) return;

		if( (aB1=ABATTLE_getInBattle( menum))==NULL ) return;
		if( (aBo=aB1->top) == NULL ) return;
		if( (aB1=aBo->next[0]) == NULL || (aB2=aBo->next[1]) == NULL ) return;

		if( aB1->teamnum == menum ){
			sprintf( token, "%s 队 胜 %s 队。", aB2->teamname, aB1->teamname );
			LogPkContend( aB2->teamname , aB1->teamname, 0, 0, 0, 0);
			ABATTLE_RecordBattle( -1, aB1->teamname, "", aB2->teamname, "胜");
			tonum = aB2->teamnum;
			ABATTLE_EnterBattle( aB2);
		}else{
			sprintf( token, "%s 队 胜 %s 队。", aB1->teamname, aB2->teamname );
			LogPkContend( aB1->teamname , aB2->teamname, 0, 0, 0, 0);
			ABATTLE_RecordBattle( -1, aB1->teamname, "胜", aB2->teamname, "");
			tonum = aB1->teamnum;
			ABATTLE_EnterBattle( aB1);
		}
		for( i=0; i<MAXBATTLEPAGE; i++){
			if( InBattle[i] == NULL) continue;
			if( InBattle[i] == aBo ) InBattle[i] = NULL;
		}
		{ //广播胜负
			int i;
			int playernum = CHAR_getPlayerMaxNum();
			for( i=0; i<playernum; i++){
				if( !CHAR_getCharUse(i) || CHAR_getInt( i , CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ) continue;
				CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
				if( CHAR_getInt( i , CHAR_PKLISTTEAMNUM ) == menum ||
					CHAR_getInt( i , CHAR_PKLISTTEAMNUM ) == tonum ){
					CHAR_warpToSpecificPoint( i, 8250, 20, 20);
				}
			}
		}
		PKLIST_LOCKTeam( menum);
		PKLIST_LOCKTeam( tonum);
		PKLIST_UpData(tocdkey,mecdkey,tonum,menum,0,0);

		PKLIST_Sort_PKListSort();
		PKLIST_SavePkTeamListData();
	}
#else
	char tocdkey[256], mecdkey[256];
	int battlearray;
	int j, side, mearray, toarray;

	if( (menum=CHAR_getInt( charaindex, CHAR_PKLISTTEAMNUM)) < 0 ) return;
	if( (mearray = PKLIST_GetPkTeamListArrayFromNum( menum)) == -1 ||
		PKLIST_GetTeamLeaderCdkey( menum, mecdkey ) == FALSE ){
		print("Can't Get Data menum:%d err !!\n", menum);
		return;
	}

	if( (battlearray = PKLIST_GetPKProcedureArray( menum)) < 0 ) return;

	for( j=0; j<2; j++ ){
		if( PKProcedure[battlearray].Team[j].use == 0 ) continue;
		if( PKProcedure[battlearray].Team[j].teamnum != menum ) continue;

		if( PKProcedure[battlearray].type != PKTYPE_STANDBY ) return;
		if( PKProcedure[battlearray].time+1 >= (int)time(NULL) ) return;
		side = (j+1)%2;

		if( PKProcedure[battlearray].Team[side].use == 0 ) return;
		if( (tonum = PKProcedure[battlearray].Team[side].teamnum ) < 0 ) return;

		if( (toarray = PKLIST_GetPkTeamListArrayFromNum( tonum)) == -1 ||
			PKLIST_GetTeamLeaderCdkey( tonum, tocdkey ) == FALSE ){
			print("Can't Get Data tonum:%d err !!\n", tonum);
			return;
		}

		PKLIST_LOCKTeam( menum);
		PKLIST_LOCKTeam( tonum);
		//saacproto_PkListUpDate_send( acfd , tocdkey, mecdkey,tonum, menum, 0, 0);
		PKLIST_UpData(tocdkey,mecdkey,tonum,menum,0,0);

		sprintf( token, "%s 队 胜 %s 队。",
			PkTeamList[toarray].teamname, PkTeamList[mearray].teamname);

		LogPkContend( PkTeamList[toarray].teamname, PkTeamList[mearray].teamname,
					CHAR_getInt( charaindex, CHAR_FLOOR),
					CHAR_getInt( charaindex, CHAR_X), CHAR_getInt( charaindex, CHAR_Y), 0);

		PKLIST_DelPKProcedures( battlearray, 0, PKTYPE_WAIT);
		PKLIST_DelPKProcedures( battlearray, 1, PKTYPE_WAIT);

		{ //广播胜负
			int i;
			int playernum = CHAR_getPlayerMaxNum();
			for( i=0; i<playernum; i++){
				if( !CHAR_getCharUse(i) || CHAR_getInt( i , CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ) continue;
				CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );

				if( CHAR_getInt( i , CHAR_PKLISTTEAMNUM ) == menum ||
					CHAR_getInt( i , CHAR_PKLISTTEAMNUM ) == tonum ){
					CHAR_warpToSpecificPoint( i, 8250, 20, 20);
				}
			}
		}
		break;
	}
	PKLIST_Sort_PKListSort();
	PKLIST_SavePkTeamListData();
#endif
}

int NPC_PKLIST_Finish_Exit( int menum, int tonum, int winside, int battlemap)
{
	char token[256];


#ifdef _DEATH_CONTENDAB
	{
		int i;
		char mecdkey[256], tocdkey[256];
		int mearray, toarray;
		ArrangeBattleC *aB1=NULL;
		ArrangeBattleC *aB2=NULL;

		if( (mearray = PKLIST_GetPkTeamListArrayFromNum( menum)) == -1 ||
			PKLIST_GetTeamLeaderCdkey( menum, mecdkey ) == FALSE ){
			print("Can't Get Data menum:%d err !!\n", menum);
			return -1;
		}
		if( (toarray = PKLIST_GetPkTeamListArrayFromNum( tonum)) == -1 ||
			PKLIST_GetTeamLeaderCdkey( tonum, tocdkey ) == FALSE ){
			print("Can't Get Data tonum:%d err !!\n", tonum);
			return -1;
		}
		//andy_log
		print("NPC_PKLIST_Finish_Exit( %d, %d, %d, %d) \n", menum, tonum, winside, battlemap);
		if( (aB1=ABATTLE_getInBattle( menum))==NULL ) return 0;
		if( (aB2=ABATTLE_getInBattle( tonum))==NULL ) return 0;
		if( aB1->top == NULL || aB2->top == NULL ||
			aB1->top != aB2->top ) return 0;

		if( winside == 0 ){
			sprintf( token, "%s 队 胜 %s 队。", aB1->teamname, aB2->teamname );
			LogPkContend( aB1->teamname , aB2->teamname, 0, 0, 0, 0);
			ABATTLE_RecordBattle( -1, aB1->teamname, "胜", aB2->teamname, "");
			ABATTLE_EnterBattle( aB1);
		}else{
			sprintf( token, "%s 队 胜 %s 队。", aB2->teamname, aB1->teamname );
			LogPkContend( aB2->teamname , aB1->teamname, 0, 0, 0, 0);
			ABATTLE_RecordBattle( -1, aB1->teamname, "", aB2->teamname, "胜");
			ABATTLE_EnterBattle( aB2);
		}

		for( i=0; i<MAXBATTLEPAGE; i++){
			if( InBattle[i] == NULL) continue;
			if( InBattle[i] == aB1->top ) InBattle[i] = NULL;
		}

		{ //广播胜负
			int i;
			int playernum = CHAR_getPlayerMaxNum();
			for( i=0; i<playernum; i++){
				if( !CHAR_getCharUse(i) || CHAR_getInt( i , CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ) continue;
				CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
				if( CHAR_getInt( i , CHAR_PKLISTTEAMNUM ) == menum ||
					CHAR_getInt( i , CHAR_PKLISTTEAMNUM ) == tonum ){
					CHAR_warpToSpecificPoint( i, 8250, 20, 20);
				}
			}
		}
		PKLIST_LOCKTeam( menum);
		PKLIST_LOCKTeam( tonum);
		PKLIST_UpData(mecdkey,tocdkey,menum,tonum,winside,0);

		PKLIST_Sort_PKListSort();
		PKLIST_SavePkTeamListData();
	}
#else
	char mecdkey[256], tocdkey[256];
	int mearray, toarray, battlearray;

	if( (mearray = PKLIST_GetPkTeamListArrayFromNum( menum)) == -1 ||
		PKLIST_GetTeamLeaderCdkey( menum, mecdkey ) == FALSE ){
		print("Can't Get Data menum:%d err !!\n", menum);
		return -1;
	}
	if( (toarray = PKLIST_GetPkTeamListArrayFromNum( tonum)) == -1 ||
		PKLIST_GetTeamLeaderCdkey( tonum, tocdkey ) == FALSE ){
		print("Can't Get Data tonum:%d err !!\n", tonum);
		return -1;
	}

	PKLIST_LOCKTeam( menum);
	PKLIST_LOCKTeam( tonum);

	//saacproto_PkListUpDate_send( acfd , mecdkey, tocdkey, menum, tonum, winside, 0);
	PKLIST_UpData(mecdkey,tocdkey,menum,tonum,winside,0);

	sprintf( token, "%s 队 胜 %s 队。",
		(winside==0)?PkTeamList[mearray].teamname:PkTeamList[toarray].teamname,
		(winside==0)?PkTeamList[toarray].teamname:PkTeamList[mearray].teamname );

	LogPkContend(
		(winside==0)?PkTeamList[mearray].teamname:PkTeamList[toarray].teamname ,
		(winside==0)?PkTeamList[toarray].teamname:PkTeamList[mearray].teamname,
		0, 0, 0 ,0);

	{ //广播胜负
		int i;
		int playernum = CHAR_getPlayerMaxNum();
		for( i=0; i<playernum; i++){
			if( !CHAR_getCharUse(i) || CHAR_getInt( i , CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ) continue;
			CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );

			if( CHAR_getInt( i , CHAR_PKLISTTEAMNUM ) == menum ||
				CHAR_getInt( i , CHAR_PKLISTTEAMNUM ) == tonum ){
				CHAR_warpToSpecificPoint( i, 8250, 20, 20);
			}
		}
	}
	if( (battlearray = PKLIST_GetPKProcedureArray( menum)) < 0 ) return 1;
	PKLIST_DelPKProcedures( battlearray, 0, PKTYPE_WAIT);
	PKLIST_DelPKProcedures( battlearray, 1, PKTYPE_WAIT);
	PKLIST_Sort_PKListSort();
	PKLIST_SavePkTeamListData();
#endif
	return 1;
}

BOOL PKLIST_HandleChartsMess( int fd, char *data, int type, int flg)
{
	int datalen=0;
	switch(type){
	case 1:
		break;
	case 2:
		datalen = strlen(data);
		if( data==NULL || datalen <= 0 ) return FALSE;
		memset( chartslist, 0, sizeof(chartslist) );
		memcpy( chartslist, data, datalen+1 );
		chartslist[datalen+1]=0;
		break;
	}
	return TRUE;
}

BOOL PKLIST_GetChartsListData( int ti, char *data, int sizes )
{
	if( getStringFromIndexWithDelim( chartslist, " ", ti, data, sizes) == FALSE  ){
		//andy_log
		print("err chartslist:%s.. !!\n", data);
		return FALSE;
	}
	return TRUE;
}

BOOL PKLIST_GetMyPKListTeamData( int teamnum, char *data, int sizes )
{
	int ti, k;
	char buf[256];
	if( (ti=PKLIST_GetPkTeamListArrayFromNum( teamnum)) == -1 ) return FALSE;
	if( PkTeamList[ti].use == 0 ) return FALSE;
	if( PkTeamList[ti].MyTeamMans[0].use == 0 ) return FALSE;

	/*snprintf( data, sizes, "队伍：%s\n胜：%d 负：%d 场：%d 积：%d分 \n",
			PkTeamList[ti].teamname, PkTeamList[ti].win, PkTeamList[ti].lost,
			PkTeamList[ti].battleplay, PkTeamList[ti].score );*/
	snprintf( data, sizes, "队伍：%s\n胜：%d 负：%d 场：%d\n",
			PkTeamList[ti].teamname, PkTeamList[ti].win, PkTeamList[ti].lost,
			PkTeamList[ti].battleplay);

	sprintf( buf, "队长：%s \n", PkTeamList[ti].MyTeamMans[0].name );
	strcat( data, buf);
	strcat( data, "队员：");

	for( k=1; k<MAXTEAMMANNUM; k++){
		if( PkTeamList[ti].MyTeamMans[k].use == 0 ) continue;
		sprintf( buf, "%s ", PkTeamList[ti].MyTeamMans[k].name );
		strcat( data, buf);
	}
	strcat( data, "\n");

	return TRUE;
}
//赛程
void PKLIST_DelPKProcedures( int ti, int side, int type)
{
	if( ti<0 || ti>=40 ) return;
	if( side<0 || side >1 ) return;

	PKProcedure[ti].use = 0;
	PKProcedure[ti].type = type;
	PKProcedure[ti].Team[side].use = 0;
	PKProcedure[ti].Team[side].teamnum = -1;
	PKProcedure[ti].Team[side].fd = -1;

}

BOOL PKLIST_CheckPklistInServerMap( int ti, int side)
{
	if( !CHAR_CHECKINDEX( PKProcedure[ti].Team[side].toindex) ||
		PKLIST_GetPkTeamListArrayFromNum( PKProcedure[ti].Team[side].teamnum ) == -1 ||
		PKProcedure[ti].Team[side].fd != getfdFromCharaIndex( PKProcedure[ti].Team[side].toindex) ||
		CHAR_getInt( PKProcedure[ti].Team[side].toindex, CHAR_FLOOR ) == 8250 ){
		return FALSE;
	}
	return TRUE;
}

BOOL PKLIST_CheckPKSameTeam( int charaindex )
{
	int teamnum, i, meti;
	char szMsg[128];

	if( !CHAR_CHECKINDEX( charaindex) ){
		print( "CHAR_CHECKINDEX err !!\n");
		return FALSE;
	}

	if( (teamnum=CHAR_getInt( charaindex, CHAR_PKLISTTEAMNUM)) < 0 ){
		print( "CHAR_PKLISTTEAMNUM err !!\n");
		return FALSE;
	}
	if( (meti=PKLIST_GetPkTeamListArrayFromNum( teamnum )) == -1 ){
		print( "GetPkTeamListArray err !!\n");
		return FALSE;
	}
#ifdef _DEATH_CONTENDAB
	if( CHAR_getInt( charaindex, CHAR_PKLISTLEADER) != 1 ){
		return FALSE;
	}
#else
	if( PkTeamList[meti].battleplay >= DEFMAXBATTLENUM ){
		CHAR_talkToCli( charaindex, -1, "对战场数已满！", CHAR_COLORYELLOW);
		return FALSE;
	}

	if( CHAR_getInt( charaindex, CHAR_PKLISTLEADER) != 1 ){
		CHAR_talkToCli( charaindex, -1, "只有参赛队伍的队长才能加入赛程喔！", CHAR_COLORYELLOW);
		return FALSE;
	}
#endif
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_LEADER ){
		return TRUE;
	}

	for( i=0; i<5; i++){
		int pindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1+i);
		if( !CHAR_CHECKINDEX( pindex) ) continue;
		if( teamnum != CHAR_getInt( pindex, CHAR_PKLISTTEAMNUM) ){
			CHAR_talkToCli( charaindex, -1, "你的队里怎会有不同队伍的人呢？", CHAR_COLORYELLOW);
			return FALSE;
		}
	}
	for( i=0; i<5; i++){
		int pindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1+i);
		if(!CHAR_CHECKINDEX(pindex)) continue;
		if(CHAR_getInt(pindex,CHAR_LV) < 120){
			sprintf(szMsg,"你的队里 %s 等级 %d 未达比赛标准喔！",CHAR_getChar(pindex,CHAR_NAME),CHAR_getInt(pindex,CHAR_LV));
			CHAR_talkToCli(charaindex,-1,szMsg,CHAR_COLORYELLOW);
			return FALSE;
		}
		if(CHAR_getInt(pindex,CHAR_TRANSMIGRATION) < 3){
			sprintf(szMsg,"你的队里 %s 转生数 %d 转，未达比赛标准喔！",CHAR_getChar(pindex,CHAR_NAME),CHAR_getInt(pindex,CHAR_TRANSMIGRATION));
			CHAR_talkToCli(charaindex,-1,szMsg,CHAR_COLORYELLOW);
			return FALSE;
		}
	}
	return TRUE;
}


BOOL PKLIST_CheckPKReapetTeam( int menum, int tonum)
{
	int meti, k, toti;
	if( (meti=PKLIST_GetPkTeamListArrayFromNum( menum )) == -1 ){
		return FALSE;
	}
	for( k=0; k<MAXBATTLENUM; k++){
		if( PkTeamList[meti].BHistory[k].use == 0 ) continue;
		if( PkTeamList[meti].BHistory[k].teamnum == tonum ){
			return FALSE;
		}
	}
	if( (toti=PKLIST_GetPkTeamListArrayFromNum( tonum )) == -1 ){
		return FALSE;
	}
	for( k=0; k<MAXBATTLENUM; k++){
		if( PkTeamList[toti].BHistory[k].use == 0 ) continue;
		if( PkTeamList[toti].BHistory[k].teamnum == menum ){
			return FALSE;
		}
	}

	return TRUE;
}


BOOL PKLIST_JoinPKProcedures( int charaindex )
{
	int i, j, teamnum, side=-1;
	BOOL Finds = FALSE;

	//andy_log
	print("JoinPKProcedures( %d) \n", charaindex);
	if( PKLIST_CheckPKSameTeam( charaindex) == FALSE ){
		print("CheckPKSameTeam() err !!!\n");
		return FALSE;
	}
	if( (teamnum = CHAR_getInt( charaindex, CHAR_PKLISTTEAMNUM)) < 0 ) return FALSE;
	if( PKLIST_CheckLOCKTeam( teamnum) == FALSE ){
		CHAR_talkToCli( charaindex, -1, "系统忙碌中，请稍後！", CHAR_COLORYELLOW);
		return FALSE;
	}

	for( i=0; i<MAXJOINTEAM; i++ ){
		if( PKProcedure[i].type == PKTYPE_STANDBY ){
			if( PKProcedure[i].time + (5*60) < (int)time(NULL) ){
				print( "太久未战斗\n");
				if( PKProcedure[i].Team[0].use != 0 &&
					PKLIST_CheckPklistInServerMap( i, 0) == TRUE &&
					PKProcedure[i].Team[1].use != 0 &&
					PKLIST_CheckPklistInServerMap( i, 0) == TRUE ){

					if( CHAR_getWorkInt( PKProcedure[i].Team[0].toindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE ){
						//andy_log
						print("强制战斗!!\n");
						BATTLE_CreateVsPlayer( PKProcedure[i].Team[0].toindex,
							PKProcedure[i].Team[1].toindex );
					}
				}else{
					PKLIST_warp( i, 0, 8250, 20, 20);
					PKLIST_warp( i, 1, 8250, 22, 20);
					PKLIST_DelPKProcedures( i, side, PKTYPE_WAIT);
					PKLIST_DelPKProcedures( i, side, PKTYPE_WAIT);
					PKProcedure[i].type = PKTYPE_WAIT;
				}
			}
			continue;
		}

		for( j=0; j<2; j++ ){
			if( PKProcedure[i].Team[j].use != 0 ){
				if( PKLIST_CheckPklistInServerMap( i, j) == FALSE){
					PKLIST_DelPKProcedures( i, j, PKTYPE_WAIT);
				}
				if( PKLIST_CheckPKReapetTeam( PKProcedure[i].Team[j].teamnum, teamnum ) == FALSE )
					break;
				continue;
			}
			side=(j+1)%2;
			if( PKProcedure[i].Team[side].use != 0 ){
				if( PKLIST_CheckPklistInServerMap( i, side) == FALSE){
					PKLIST_DelPKProcedures( i, side, PKTYPE_WAIT);
					continue;
				}
				if( PKLIST_CheckPKReapetTeam( PKProcedure[i].Team[side].teamnum,
						teamnum ) == FALSE )
					continue;

				PKProcedure[i].type = PKTYPE_STANDBY;
				PKProcedure[i].time = (int)time( NULL);
			}else{
				PKProcedure[i].type = PKTYPE_WAIT;
			}
//-----------------------------------------------------------------------------
			//载入队伍资料
			PKProcedure[i].Team[j].teamnum = teamnum;
			snprintf( PKProcedure[i].Team[j].cdkey, sizeof(PKProcedure[i].Team[j].cdkey),"%s",
				CHAR_getChar( charaindex, CHAR_CDKEY) );
			snprintf( PKProcedure[i].Team[j].name, sizeof(PKProcedure[i].Team[j].name),"%s",
				CHAR_getChar( charaindex, CHAR_NAME) );
			PKProcedure[i].Team[j].toindex = charaindex;
			PKProcedure[i].Team[j].fd = getfdFromCharaIndex( charaindex);
			PKProcedure[i].Team[j].use = 1;
//------------------------------------------------------------------------------
			PKLIST_warp( i, j, PK_BATTLEMAP[i].floor, PK_BATTLEMAP[i].x-j, PK_BATTLEMAP[i].y );

			Finds = TRUE;
			break;
		}
		if( Finds == TRUE ) break;
	}

	if( Finds == FALSE ){
		CHAR_talkToCli( charaindex, -1, "目前赛程中之队伍已满请稍後再试！", CHAR_COLORYELLOW);
		return FALSE;
	}
	return TRUE;
}

void PKLIST_CheckTeamBeEnable( void)
{
	int i, j;
	for( i=0; i<MAXJOINTEAM; i++ ){
		for( j=0; j<2; j++ ){
			if( PKProcedure[i].Team[j].use == 0 ) continue;
			PKLIST_CheckPklistInServerMap( i, j);//确认队伍是否成立
		}
	}
}

void PKLIST_warpOne( int charaindex, int fl, int x, int y )
{
	int k;
	if( !CHAR_CHECKINDEX( charaindex) ) return;
	CHAR_warpToSpecificPoint( charaindex, fl, x, y);
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER ){
		for( k = 1; k < CHAR_PARTYMAX; k ++ ){
			int subindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1+k );
			if( !CHAR_CHECKINDEX( subindex ) )continue;
			CHAR_warpToSpecificPoint( subindex, fl, x, y);
		}
	}
}

void PKLIST_warp( int ti, int side, int fl, int x, int y )
{
	int k;
	if( !CHAR_CHECKINDEX(PKProcedure[ti].Team[side].toindex) );
	if( CHAR_getWorkInt( PKProcedure[ti].Team[side].toindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_LEADER ){
		CHAR_warpToSpecificPoint( PKProcedure[ti].Team[side].toindex, fl, x, y);
		CHAR_talkToCli( PKProcedure[ti].Team[side].toindex, -1, "请在此等待对战队伍！", CHAR_COLORYELLOW);
	}else{
		for( k = 0; k < CHAR_PARTYMAX; k ++ ){
			int subindex = CHAR_getWorkInt( PKProcedure[ti].Team[side].toindex,
				CHAR_WORKPARTYINDEX1+k );

			if( !CHAR_CHECKINDEX( subindex ) )continue;
			CHAR_talkToCli( subindex, -1, "请在此等待对战队伍！如时间超过１０分後未有对战队伍，建议玩家可重原登後重新加入赛程。", CHAR_COLORYELLOW);
			CHAR_warpToSpecificPoint( subindex, fl, x, y);
		}
	}
}

int PKLIST_GetPKProcedureArray( int menum )
{
	int i, j;
	for( i=0; i<MAXJOINTEAM; i++ ){
		for( j=0; j<2; j++ ){
			if( PKProcedure[i].Team[j].use == 0 ) continue;
			if( PKProcedure[i].Team[j].teamnum == menum ) return i;
		}
	}
	return -1;
}

BOOL PKLIST_CheckLOCKTeam( int menum)
{
	int meti=-1;
	if( (meti=PKLIST_GetPkTeamListArrayFromNum( menum )) == -1 ){
		return FALSE;
	}

	if(  PkTeamList[meti].read > (int)time(NULL) ){
		return FALSE;
	}
	return TRUE;

}

void PKLIST_LOCKTeam( int menum)
{
	int meti=-1;
	if( (meti=PKLIST_GetPkTeamListArrayFromNum( menum )) == -1 ){
		return;
	}
	PkTeamList[meti].read = (int)time(NULL)+60;
}

void PKLIST_UNLOCKTeam( int menum)
{
	int meti=-1;
	if( (meti=PKLIST_GetPkTeamListArrayFromNum( menum )) == -1 ){
		return;
	}
	PkTeamList[meti].read = 0;
}

void PKLIST_Sort_PKListSort( void)
{
	int i, j, k, last=-1;
	char data[65535*3], buf[512];
	for( i=0; i<MAXTEAMNUM; i++){
		last = -1;
		for( k=549; k>=0; k-- ){
			if( PkTeamList[i].score <= 0 ) continue;
			if( PKListSort[k] == -1 ){
				last = k;
				continue;
			}
			if(	PkTeamList[i].score > PkTeamList[PKListSort[k]].score ){
				last = k;
				continue;
			}
			break;
		}
		if( last == -1 ) continue;
		for( j=548; j>=last; j-- ){
			PKListSort[j+1] = PKListSort[j];
		}
		PKListSort[last]=i;
	}
	memset(data,0,sizeof(data));
	for(i=0;i<550;i++){
		if(PKListSort[i] == -1) continue;
			
		sprintf(buf, "%s|%d|%d|%d|%d ",
			PkTeamList[PKListSort[i]].teamname,
			PkTeamList[PKListSort[i]].win, PkTeamList[PKListSort[i]].lost,
			PkTeamList[PKListSort[i]].battleplay, PkTeamList[PKListSort[i]].score );
		strcat(data,buf);
	}
	PKLIST_HandleChartsMess(0,data,2,0);
}

//正式赛

void ABATTLE_InitABattle( int maxnums )
{
	int i,j,k;
	if( maxnums<1 || maxnums >= MAXBAFLOOR) return;
	memset( ABLevelup, 0, sizeof(ABLevelup));
	for( k=0; k<MAXBAHEAD; k++ ){
		ABFloor[k]=(maxnums);
		for( i=0; i<MAXBAFLOOR; i++){
			for( j=0; j<MAXNOWBATTLE; j++)
				NowABattle[k][i][j]=NULL;
		}
	}

	for( i=0; i<MAXBATTLEPAGE; i++){
		InBattle[i]=NULL;
	}

	for( i=0; i<MAXBAHEAD; i++){
		if( (headABattle[i] = ArrangeBattleC_getNew()) == NULL ){
			print("err can't get headABattle NULL \n");
			return;
		}
		headABattle[i]->top = NULL;
		for( j=0; j<MAXNOWBATTLE; j++){
			if( NowABattle[i][0][j] != NULL )continue;
			NowABattle[i][0][j] = headABattle[i];
			headABattle[i]->fl = 0;
			headABattle[i]->code = j;
			break;
		}
		if( j >= MAXNOWBATTLE ) return;
		ABATTLE_CreateNet( headABattle[i], i, 0, maxnums);
	}
	{
		FILE *fp=NULL;
		int count=0;
		char buf[256], buf1[256], buf2[256], filename[256];
		
		for( i=0; i<MAXBAHEAD; i++ ){
			sprintf( filename, "abattle_%d.txt", i);
			if( (fp=fopen( filename, "r")) == NULL ) continue;
			count=0;
			while( fgets( buf, sizeof(buf)-1, fp) ){
				if( getStringFromIndexWithDelim( buf , "|", 1, buf1, sizeof( buf1)) == FALSE ) continue;
				if( getStringFromIndexWithDelim( buf , "|", 2, buf2, sizeof( buf2)) == FALSE ) continue;

				//andy_log
				print("ti:%d", i);

				NowABattle[i][maxnums][count]->teamnum = atoi( buf1);
				snprintf( NowABattle[i][maxnums][count]->teamname,
					sizeof( NowABattle[i][maxnums][count]->teamname),"%s", buf2);
				if( NowABattle[i][maxnums][count]->teamnum >= 0 )
					NowABattle[i][maxnums][count]->use = 1;

				//andy_log
				print("NowABattle[%d][%d][%d].  %x[%d,%d,%s]\n",
					i, maxnums, count,
					NowABattle[i][maxnums][count],
					NowABattle[i][maxnums][count]->use, 
					NowABattle[i][maxnums][count]->teamnum,
					NowABattle[i][maxnums][count]->teamname );

				count++;
			}
			fclose( fp);
			//remove( filename);
		}
	}

	maxbati = MAXBAHEAD;
	maxfloor = maxnums;
}

int ABATTLE_CreateNet( ArrangeBattleC *now, int ti, int fl, int maxfl)
{
	int i,j;
	ArrangeBattleC *news=NULL;
	//andy_log
	print( "CreateNet( %d,%d)\n", ti, fl);

	if( fl != 0 ){
		for( j=0; j<MAXNOWBATTLE; j++){
			if( NowABattle[ti][fl][j] != NULL )continue;
			//andy_log
			print(" create -> NowABattle[%d][%d][%d]\n", ti, fl, j);
			NowABattle[ti][fl][j] = now;
			now->fl = fl;
			now->code = j;
			break;
		}
		if( j >= MAXNOWBATTLE ) return 0;
	}
	if( fl < maxfl ){
		for( i=0; i<2; i++){
		
			news=NULL;
			if( (news = ArrangeBattleC_getNew()) == NULL ){
				//andy_log
				print("err can't get news NULL\n");
				return 0;
			}
			now->next[i] = news;
			news->top = now;
			if( ABATTLE_CreateNet( news, ti, fl+1, maxfl) == 0 ){
				//andy_log
				print("err net free %x \n", news);
				free( news );
				now->next[i] = NULL;
				return 0;
			}
		}
	}

	return 1;
}

void ABATTLE_ShowNet( ArrangeBattleC *now, int fl)
{
	int i;
	if( now == NULL ) return;

	if( now->fl == fl ){
		print(" [%d,%d,%s] %x,top:%x\n", now->use, now->teamnum, now->teamname, now,
			now->top );
	}

	for( i=0; i<2; i++){
		if( now->next[i] == NULL )continue;
		ABATTLE_ShowNet( now->next[i], fl);
	}
}

void ABATTLE_ShowBattlefromFl( int ti, int fl)
{
	int j, total=0;
	if( fl<0 ) return;
	if( ti<0 || ti >= MAXBAHEAD ) return;
	print("headABattle:%x\n", headABattle);
	ABATTLE_ShowNet( headABattle[ti], fl);
	print("\nNOWBATTLE:\n");
	if( fl<0 || fl >= MAXBAFLOOR) return;


	for( j=0; j<MAXNOWBATTLE; j++){
		if( NowABattle[ti][fl][j] == NULL )continue;
		if( j!=0 && j%8==0) print("\n");
		print(" [%d,%d,%s] %x, top:%x",
			NowABattle[ti][fl][j]->use, NowABattle[ti][fl][j]->teamnum, 
			NowABattle[ti][fl][j]->teamname, NowABattle[ti][fl][j],
			NowABattle[ti][fl][j]->top );
		total++;
	}
	print("\ntotal:%d\n", total);
}

BOOL ABATTLE_InsertBattle( ArrangeBattleC *aB)
{
	ArrangeBattleC *aBo=NULL;
	int i;
	if( aB == NULL ) return FALSE;
	for( i=0; i<MAXBATTLEPAGE; i++){
		if( InBattle[i] != NULL ) continue;
		if( (aBo=aB->next[0])==NULL || aBo->use == 0 || aBo->type > 0 ) return FALSE;
		if( (aBo=aB->next[1])==NULL || aBo->use == 0 || aBo->type > 0 ) return FALSE;
		InBattle[i] = aB;
		InBattle[i]->next[0]->type =1;
		InBattle[i]->next[1]->type =1;

		print("InsertBattle[%d,%s vs %d,%s]\n",
			InBattle[i]->next[0]->teamnum, InBattle[i]->next[0]->teamname,
			InBattle[i]->next[1]->teamnum, InBattle[i]->next[1]->teamname );
#ifdef _DEATH_CONTENDTEST
		aB->time = (int)time(NULL)+30;	//战斗时间
#else
		aB->time = (int)time(NULL)+(5*60);	//战斗时间
#endif
		aB->type = 1; // 战斗旗标
		return TRUE;
	}
	return FALSE;
}

void ABATTLE_EnterBattle( ArrangeBattleC *aB)
{
	ArrangeBattleC *aBtop=NULL;
	ArrangeBattleC *aBo=NULL;

	if( (aBtop=aB->top) == NULL ){
		//andy_log
		print("del (aBtop=aB->top) NULL !\n");
		return;
	}

	aBtop->teamnum = aB->teamnum;
	snprintf( aBtop->teamname, sizeof(aBtop->teamname),"%s", aB->teamname);
	aBtop->type = 0;
	aBtop->use = 1;

	if( (aBo=aBtop->next[0]) != NULL ){
		aBo->use=0;
		aBo->teamnum=-1;
	}
	if( (aBo=aBtop->next[1]) != NULL ){
		aBo->use=0;
		aBo->teamnum=-1;
	}
}

void ABATTLE_EliminateBattlefromFl( ArrangeBattleC *aB)
{
	ArrangeBattleC *aBtop=NULL;
	if( aB == NULL ) return;
	aB->use=0;
	aB->type = 0;
	aB->teamnum = -1;
	free( aB);
}

int ABATTLE_FindBattlefromFl( int ti, int fl)
{
	int i, j, side=0, count=0;
	ArrangeBattleC *aBo=NULL;
	ArrangeBattleC *aB1=NULL;
	ArrangeBattleC *aB2=NULL;

	//andy_log
	print( "FindBattlefromFl(%d,%d)\n", ti, fl);
	for( i=0; i<MAXNOWBATTLE; i++){
		if( NowABattle[ti][fl][i] == NULL )continue;
		aBo = NowABattle[ti][fl][i];

		for( j=0; j<2; j++){
			if( (aB1=aBo->next[j]) == NULL || aB1->use == 0 || aB1->type > 0 )continue;

			side = (j+1)%2;
			aB2=aBo->next[side];

			if( aB2 == NULL || aB2->use == 0 || aB2->teamnum == -1 ){ //没有对手
			}else{
				if( ABATTLE_InsertBattle( aBo) == FALSE ){
					return -1;
				}else{
					char token[256];
					int playernum = CHAR_getPlayerMaxNum();
					sprintf( token, "%s队 VS %s队 ５分钟後决斗。", aB1->teamname, aB2->teamname );
					PKLIST_shoutToAll( token, -1, -1);
					count++;
					// 把排入赛程的人员传入地图 8250
					for(i=0;i<playernum;i++){
						if(CHAR_getCharUse(i) == FALSE) continue;
						printf("%s will be send...",CHAR_getChar(i,CHAR_NAME));
						if((CHAR_getInt(i,CHAR_PKLISTTEAMNUM) == aB1->teamnum || CHAR_getInt(i,CHAR_PKLISTTEAMNUM) == aB2->teamnum) &&
							CHAR_getInt(i,CHAR_FLOOR) != 8250){
							printf("send!!\n");
							CHAR_warpToSpecificPoint(i,8250,15,15);
						}
						else printf("has not send!!(teamnum:%d,floor:%d)\n",CHAR_getInt(i,CHAR_PKLISTTEAMNUM),CHAR_getInt(i,CHAR_FLOOR));
					}
					//if( count >= 2) return count;
					return count;
				}
			}
		}
	}
	return count;
}

void ABATTLE_CheckBattlefromFl(int charindex, int ti,int battleindex)
{
	int i = ti, count = 0,floor=0;
	//ABFloor[MAXBAHEAD];
	//andy_log
	print("ABATTLE_CheckBattlefromFl(%d,%d )..\n",ti,battleindex);
	//for( i=0; i<MAXBAHEAD; i++){
	{
		if( ABFloor[i] == -1 ){
			CHAR_talkToCli( charindex, -1, "组别不存在", CHAR_COLORYELLOW);
			return;
		}
		floor = ABATTLE_CheckBattlefromFl_sub(charindex, i, ABFloor[i],battleindex);
		//if( count < 4 )
		//	count += ABATTLE_FindBattlefromFl( i, ABFloor[i]);
		count = ABATTLE_FindBattlefromFl( i, ABFloor[i]);
		ABFloor[i] = floor;

		if( ABFloor[i] == -1 ){
			FILE *fp=NULL;
			if( headABattle[i]->use == 1 && headABattle[i]->teamnum != -1 ){
				char token[256];
				sprintf( token, "恭喜 %s 队，取得分组优胜，晋级下一阶段。", headABattle[i]->teamname );
				PKLIST_shoutToAll( token, -1, 8250);
				ABATTLE_RecordBattle( i, headABattle[i]->teamname, "晋级８强", "NULL", "");

			}
			if( (fp=fopen( "nbattle.txt", "a+")) != NULL ){
				fprintf( fp, "%d %d|%s|\n",
					i, headABattle[i]->teamnum, headABattle[i]->teamname);
				fclose( fp);
			}
		}
	}

}

int ABATTLE_CheckBattlefromFl_sub(int charindex, int ti, int fl, int battleindex)
{
	int i = battleindex;
	char token[256];
	ArrangeBattleC *aBo=NULL;
	ArrangeBattleC *aB1=NULL;
	ArrangeBattleC *aB2=NULL;
	
	//andy_log
	print("CheckBattlefromFl_sub( %d, %d, %d)\n", ti, fl, battleindex);

	for( i=0; i<MAXNOWBATTLE; i++){
		if( (aBo=NowABattle[ti][fl][i]) == NULL ) continue;
		aB1=aBo->next[0];
		aB2=aBo->next[1];
		if( (aB1 == NULL || aB1->use == 0 ) &&
			(aB2 == NULL || aB2->use == 0 ) ){//两队都不成立
			if( aB1 != NULL ) free( aB1);
			if( aB2 != NULL ) free( aB2);
			aBo->next[0] = NULL;
			aBo->next[1] = NULL;
			continue;
		}else if( aB1 == NULL || aB1->use == 0 || aB1->teamnum == -1 ){ //0不成立
			ABATTLE_EliminateBattlefromFl( aB1);
			aBo->next[0] = NULL;
			if( aB2 != NULL && aB2->use != 0 && aB2->type == 0 ){
				ABATTLE_EnterBattle( aB2);
			}
			return fl;
		}else if( aB2 == NULL || aB2->use == 0 || aB2->teamnum == -1 ){ //1不成立
			ABATTLE_EliminateBattlefromFl( aB2);
			aBo->next[1] = NULL;
			if( aB1 != NULL && aB1->use != 0 && aB1->type == 0 ){
				ABATTLE_EnterBattle( aB1);
			}
			return fl;
		}else{
			//andy_log
			print("两者成立!.%d.%x[%d,%s] %x[%d,%s]\n", i,
				aB1, aB1->teamnum, aB1->teamname,
				aB2, aB2->teamnum, aB2->teamname );
			return fl;
		}
	}

	return (fl-1);
}

//确认赛程战斗状态 包含时间
BOOL ABATTLE_CheckInABattle( int ti)
{
	ArrangeBattleC *aB=NULL;
	ArrangeBattleC *aB1=NULL;
	ArrangeBattleC *aB2=NULL;
	int i;
	int playernum = CHAR_getPlayerMaxNum();
	char token[256];

	if( ti<0 || ti>MAXBATTLEPAGE ) return FALSE;
	if( (aB=InBattle[ti]) == NULL ) return FALSE;

	aB1=aB->next[0];
	aB2=aB->next[1];

	switch( aB->type){
	case 0:
		break;
	case 1:	//已排入赛程 5分钟後进场
		if( aB->time < (int)time(NULL) ){
			if( aB1 == NULL || aB1->use == 0 || 
				aB2 == NULL || aB2->use == 0 ) return FALSE;
			sprintf( token, "%s队 VS %s队，赛程即将开始，请两队做好准备。",
				aB1->teamname, aB2->teamname );
			//PKLIST_shoutToAll( token, aB1->teamnum, 8250);
			PKLIST_shoutToAll( token, aB2->teamnum, 8250);
#ifdef _DEATH_CONTENDTEST
			aB->time=(int)time(NULL)+30;
#else
			aB->time=(int)time(NULL)+(2*60);
#endif
			aB->type=2;
			// 把排入赛程的人员传入地图 8250
			for(i=0;i<playernum;i++){
				if(CHAR_getCharUse(i) == FALSE) continue;
				printf("%s will be send...",CHAR_getChar(i,CHAR_NAME));
				if((CHAR_getInt(i,CHAR_PKLISTTEAMNUM) == aB1->teamnum || CHAR_getInt(i,CHAR_PKLISTTEAMNUM) == aB2->teamnum) &&
					CHAR_getInt(i,CHAR_FLOOR) != 8250){
					printf("send!!\n");
					CHAR_warpToSpecificPoint(i,8250,15,15);
				}
				else printf("has not send!!(teamnum:%d,floor:%d)\n",CHAR_getInt(i,CHAR_PKLISTTEAMNUM),CHAR_getInt(i,CHAR_FLOOR));
			}
		}
		break;
	case 2:	//已排入赛程 进场3分钟 并判断队长是否存在 并广播
		{
			int playernum = CHAR_getPlayerMaxNum();
			for( i=0 ; i<playernum ; i++) {
				if( CHAR_getCharUse(i) == FALSE ) continue;
				if( aB1!=NULL && !CHAR_CHECKINDEX( aB1->toindex) &&
					aB1->teamnum == CHAR_getInt( i, CHAR_PKLISTTEAMNUM) && 
					CHAR_getInt( i, CHAR_PKLISTLEADER) == 1 &&
					PKLIST_CheckPKSameTeam( i) == TRUE ){

						int stime = ((aB->time)-(int)time(NULL));
						sprintf( token, "请快找齐你的所有队员，并组好队，%d秒後将进行传送。", (stime<0)?0:stime);
						CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW);
						aB1->toindex = i;
						continue;

				}
				if( aB2!=NULL && !CHAR_CHECKINDEX( aB2->toindex) &&
					aB2->teamnum == CHAR_getInt( i, CHAR_PKLISTTEAMNUM) &&
					CHAR_getInt( i, CHAR_PKLISTLEADER) == 1 &&
					PKLIST_CheckPKSameTeam( i) == TRUE ){
						int stime = ((aB->time)-(int)time(NULL));
						sprintf( token, "请快找齐你的所有队员，并组好队，%d秒後将进行传送。", (stime<0)?0:stime);
						CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW);
						aB2->toindex = i;
						continue;
				}
			}
		}
		// 把排入赛程的人员传入地图 8250
		for(i=0;i<playernum;i++){
			if(CHAR_getCharUse(i) == FALSE) continue;
			if((CHAR_getInt(i,CHAR_PKLISTTEAMNUM) == aB1->teamnum || CHAR_getInt(i,CHAR_PKLISTTEAMNUM) == aB2->teamnum) &&
				CHAR_getInt(i,CHAR_FLOOR) != 8250) CHAR_warpToSpecificPoint(i,8250,15,15);
		}

		if( aB->time < (int)time(NULL) ){
			char token[256];

			sprintf( token, "%s队 VS %s队，赛程开始。", aB1->teamname, aB2->teamname );
			PKLIST_shoutToAll( token, -1, 8250);

			if( CHAR_CHECKINDEX( aB1->toindex) && CHAR_CHECKINDEX( aB2->toindex) ){
				PKLIST_warpOne( aB2->toindex, PK_BATTLEMAP[ti].floor, PK_BATTLEMAP[ti].x+1, PK_BATTLEMAP[ti].y);
				PKLIST_warpOne( aB1->toindex, PK_BATTLEMAP[ti].floor, PK_BATTLEMAP[ti].x, PK_BATTLEMAP[ti].y);
#ifdef _DEATH_CONTENDTEST
				aB->time=(int)time(NULL)+5;
#else
				aB->time=(int)time(NULL)+(20);
#endif
				aB->type=3;
				return TRUE;
			}else if( !CHAR_CHECKINDEX( aB1->toindex) && !CHAR_CHECKINDEX( aB2->toindex) ){
				sprintf( token, "%s队(%d)，%s队(%d)，因队长未在时间内到场，视为弃权。",aB1->teamname,aB1->toindex,
					aB2->teamname,aB2->toindex);
				ABATTLE_RecordBattle( ti, aB1->teamname, "未", aB2->teamname, "未");
				aB1->use = 0;
				aB2->use = 0;
			}else if( !CHAR_CHECKINDEX( aB1->toindex) ) {
				sprintf( token, "%s队(%d)，因队长未在时间内到场，视为弃权。",aB1->teamname,aB1->toindex);
				aB1->use = 0;
				ABATTLE_RecordBattle( ti, aB1->teamname, "未", "NULL", "");
				ABATTLE_EnterBattle( aB2);
			}else if( !CHAR_CHECKINDEX( aB2->toindex) ){
				sprintf( token, "%s队(%d)，因队长未在时间内到场，视为弃权。",aB2->teamname,aB2->toindex);
				aB2->use = 0;
				ABATTLE_RecordBattle( ti, aB2->teamname, "未", "NULL", "");
				ABATTLE_EnterBattle( aB1);
			}
			PKLIST_shoutToAll( token, -1, 8250);
			InBattle[ti] = NULL;
			return FALSE;
		}
		break;
	case 3:	//已排入赛程 队伍到齐 5分钟内 强制战斗
		aB->type=4;
		aB->time=(int)time(NULL)+(2*60);
		break;
	case 4:
		if( aB->time < (int)time(NULL) ){//强制战斗
			if( CHAR_getWorkInt( aB1->toindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE ){
				//andy_log
				print("强制战斗 %s vs %s \n", aB1->teamname, aB2->teamname);
				BATTLE_CreateVsPlayer( aB1->toindex, aB2->toindex );
				aB->type = 5;
			}
		}
		break;
	}

	return TRUE;
}

void ABATTLE_MakeInABattleString( void)
{
	ArrangeBattleC *aB=NULL;
	ArrangeBattleC *aB1=NULL;
	ArrangeBattleC *aB2=NULL;
	char buf1[256], buf[256];
	int i, fir=1;

	memset( ABattlelist, 0, sizeof(ABattlelist));

	for( i=0; i<MAXBATTLEPAGE; i++){
		if( (aB=InBattle[i]) == NULL )continue;
		if( (aB1=aB->next[0]) == NULL || aB1->use == 0 )continue;
		if( (aB2=aB->next[1]) == NULL || aB2->use == 0 )continue;

		switch( aB->type){
		case 0: continue;
			break;
		case 1://预定
			strcpy( buf, "预定");
			break;
		case 2://准备
			strcpy( buf, "准备");
			break;
		case 3://开始
		case 4:
			strcpy( buf, "开始");
			break;
		}

		sprintf( buf1, "%d|%s|%s|%d|%s",
			i, aB1->teamname, aB2->teamname, aB->time, buf);
		if( fir != 1 ) strcat( ABattlelist, " ");
		fir=0;
		strcat( ABattlelist, buf1);
	}
}

BOOL PKLIST_GetABattlelistDataString( int ti, int *tindex, int *stimes,
									 char *buf1, char *buf2, char *buf3, int flg)
{
	char data[1024], buf[256], buff[256];
	switch( flg){
	case 3:
		{
			memset( data, 0, sizeof(data));
			if( getStringFromIndexWithDelim( ABattlelist, " ", ti, data, sizeof(data)) == FALSE  )
				return FALSE;
			if( getStringFromIndexWithDelim( data, "|", 1, buf, sizeof( buf)) == FALSE ) return FALSE;
			*tindex=atoi( buf);
			if( getStringFromIndexWithDelim( data, "|", 2, buf, sizeof( buf))== FALSE) return FALSE;
			strcpy( buf1, buf);
			if( getStringFromIndexWithDelim( data, "|", 3, buf, sizeof( buf))== FALSE) return FALSE;
			strcpy( buf2, buf);
			if( getStringFromIndexWithDelim( data, "|", 4, buf, sizeof( buf))== FALSE) return FALSE;
			*stimes = atoi( buf);
			if( getStringFromIndexWithDelim( data, "|", 5, buf, sizeof( buf))== FALSE ) return FALSE;
			strcpy( buf3, buf);
		}
		break;
	case 5:
		{
			if( getStringFromIndexWithDelim( ABLevelup, " ", ti, data, sizeof(data)) == FALSE  )
				return FALSE;
			if( getStringFromIndexWithDelim( data, "|", 1, buf, sizeof( buf)) == FALSE ) return FALSE;
			*tindex=atoi( buf);
			if( getStringFromIndexWithDelim( data, "|", 2, buff, sizeof( buff)) == FALSE ) return FALSE;
			if( getStringFromIndexWithDelim( data, "|", 3, buf, sizeof( buf)) == FALSE ) return FALSE;
			sprintf( buf1, "［%s］%s ", buff, buf);
			if( getStringFromIndexWithDelim( data, "|", 4, buff, sizeof( buff)) == FALSE ) return FALSE;
			if( getStringFromIndexWithDelim( data, "|", 5, buf, sizeof( buf)) == FALSE ) return FALSE;
			sprintf( buf2, "［%s］%s ", buff, buf);
		}
		break;
	}
	return TRUE;
}

ArrangeBattleC *ABATTLE_getInBattle( int teamnum)
{
	ArrangeBattleC *aB=NULL;
	ArrangeBattleC *aBx=NULL;
	int i;

	for( i=0; i<MAXBATTLEPAGE; i++){
		if( (aB=InBattle[i]) == NULL )continue;
		if( (aBx=aB->next[0]) != NULL && aBx->use == 1 && aBx->teamnum != -1 ){
			if( aBx->teamnum == teamnum )
				return aBx;
		}
		if( (aBx=aB->next[1]) != NULL && aBx->use == 1 && aBx->teamnum != -1 ){
			if( aBx->teamnum == teamnum )
				return aBx;
		}
	}
	return NULL;
}

ArrangeBattleC *ArrangeBattleC_getNew( void)
{
	ArrangeBattleC *MBUF=NULL;
	MBUF = (ArrangeBattleC *) calloc( 1, sizeof( struct _tagArrangeBattle) );
	if( MBUF == NULL ) return NULL;

	MBUF->use = 0;
	MBUF->fl = -1;
	MBUF->code = -1;
	MBUF->teamnum = -1;
	MBUF->toindex = -1;
	MBUF->type=0;
	MBUF->teamname[0]=0;
	MBUF->next[0] = NULL;
	MBUF->next[1] = NULL;
	MBUF->top = NULL;

	return MBUF;
}

ArrangeBattleC *ArrangeBattleC_getInBattleArray( int ti)
{
	if( ti<0 || ti>MAXBATTLEPAGE ) return NULL;
	return InBattle[ti];
}

void ABATTLE_RecordBattle( int ti, char *buf1, char *tstr1,char *buf2, char *tstr2)
{//ABLevelup
	char strlog[512];
	FILE *fp=NULL;

	if( (fp=fopen( "battle.rec", "a+") )==NULL )return;
	sprintf( strlog, "%d|%s|%s|%s|%s", ti, tstr1, buf1, tstr2, buf2);
	fprintf( fp, "%s\n", strlog);
	fclose( fp);

	ABATTLE_GetRecordBattle();
}

void remove_r( char *s )
{
    int i;
    for(i=0;;i++){
        if( s[i]=='\r' || s[i]=='\n' )s[i] =0;
        if( s[i]==0)break;
    }
}

void ABATTLE_GetRecordBattle( void)
{//ABLevelup
	char data[512];
	FILE *fp=NULL;
	memset( ABLevelup, 0, sizeof(ABLevelup));

	if( (fp=fopen( "battle.rec", "r") )==NULL )return;
	while( fgets( data, sizeof(data)-1, fp) ){
		if( strlen( data) <= 0 ) continue;
		remove_r( data);
		if( strlen( ABLevelup) > 0 ) strcat( ABLevelup, " ");
		strcat( ABLevelup, data);
	}
	fclose( fp);
}

void PKLIST_UpData(char *mycdkey,char *tocdkey,int menum,int tonum,int winer,int flg)
{
	int k,meti,toti;

	if((meti = PKLIST_GetPkTeamListArray(menum,mycdkey)) == -1){
		printf("can't find meteam.teamnum(%d),cdkey(%s)\n",menum,mycdkey);
		return;
	}
	if((toti = PKLIST_GetPkTeamListArray(tonum,tocdkey)) == -1){
		printf("can't find toteam.teamnum(%d),cdkey(%s)\n",tonum,tocdkey);
		return;
	}

	if(PkTeamList[meti].use != 1){
		printf("meteam not in use.teamnum(%d),cdkey(%s)\n",menum,mycdkey);
		return;
	}
	/*for(k=0;k<MAXBATTLENUM;k++){
		if( PkTeamList[meti].BHistory[k].use != 1 ) continue;
		if( PkTeamList[meti].BHistory[k].teamnum == tonum ){
			return;
		}
	}*/
	if( PkTeamList[toti].use != 1 ){
		printf("toteam not in use.teamnum(%d),cdkey(%s)\n",tonum,tocdkey);
		return;
	}
	/*for( k=0; k<MAXBATTLENUM; k++ ){
		if( PkTeamList[toti].BHistory[k].use != 1 ) continue;
		if( PkTeamList[toti].BHistory[k].teamnum == menum ){
			return;
		}
	}*/
	PKLIST_UpdateOnePkTeamData(meti,tonum,(winer==0)?1:0);
	PKLIST_UpdateOnePkTeamData(toti,menum,(winer==1)?1:0);
}

int PKLIST_UpdateOnePkTeamData( int ti, int forti, int winerflg)
{
	int ret=0;
	int battleTi = PKLIST_GetOneBHistory( ti );
	if( battleTi == -1){
		printf( "ti:%d battleTi:%d err !!\n", ti, battleTi);
		return -2;
	}

	if( winerflg == 1 ){
		PkTeamList[ti].win++;
	}else{
		PkTeamList[ti].lost++;
	}
	PkTeamList[ti].battleplay++; //场次
	PkTeamList[ti].score = (PkTeamList[ti].win*3) - (PkTeamList[ti].lost*1);//积分

	if( PkTeamList[ti].score >= 90 ){
		PkTeamList[ti].inside = 2; //录取
	}else if( PkTeamList[ti].lost > 50*0.4){// 0.7 = (1-胜率3成)*最高场次
		PkTeamList[ti].inside = 0;
	}else {
		PkTeamList[ti].inside = 1;
	}

	if( PkTeamList[ti].score < 0 ) PkTeamList[ti].score = 0;

	ret = PKLIST_SetOneBHistory( ti, battleTi, 1, forti, winerflg);
	//PKLIST_SortTheCharts( ti);
	return ret;
}

int PKLIST_GetOneBHistory( int ti )
{
	int i;
	if( ti < 0 || ti >= maxteam ){
		printf( "GetOneBHistory(ti:%d) err return -1 !!\n", ti);
		return -1;
	}
	for( i=0; i<MAXBATTLENUM; i++){
		if( i>=DEFMAXBATTLENUM ) break;
		if( PkTeamList[ti].BHistory[i].use == 0 ){
			printf("return BHistory[%d] \n", i) ;
			return i;
		}
	}

	printf("BATTLEFULL err !! \n") ;
	return -1;
}

int PKLIST_SetOneBHistory( int ti, int hi, int use, int teamnum, int flg )
{
	if( ti < 0 || ti >= maxteam ) return -1;
	if( hi < 0 || hi >= MAXBATTLENUM ) return -1;
	if( PkTeamList[ti].BHistory[hi].use == use ) return -1;
	
	PkTeamList[ti].BHistory[hi].teamnum = teamnum;
	PkTeamList[ti].BHistory[hi].flg = flg;
	PkTeamList[ti].BHistory[hi].use = use;
	return 1;
}

#endif
