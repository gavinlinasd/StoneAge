#include "version.h"
#include <time.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "npc_fmwarpman.h"
#include "npc_scheduleman.h"
#include "readmap.h"
#include "log.h"
#include "battle.h"
#include "handletime.h"
#include "family.h"
#include "errno.h"

#define FMWARPMAN_INIT_LOOPTIME		600	// 0.1鏃
#define FMWARPMAN_FREE_LOOPTIME		9000	// 1.5鏃
#define FMWARPMAN_BUSY_LOOPTIME		3000	// 0.5鏃
#define FMWARPMAN_WAIT_LOOPTIME		18000	// 3鏃
#define FMWARPMAN_CLEANPLACE		59
//#define FMWARPMAN_CLEANPLACE		9
#define	TRUE	1
#define FALSE	0
// CoolFish Rem 2002/2/25
// #define MANOR	4

enum {
	NPC_WORK_MODEFLAG =  CHAR_NPCWORKINT1,
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT2,
	NPC_WORK_WARPFLOOR = CHAR_NPCWORKINT3,
	NPC_WORK_FMNUMI = CHAR_NPCWORKINT4,
	NPC_WORK_FMNUMII = CHAR_NPCWORKINT5,
	NPC_WORK_ID = CHAR_NPCWORKINT6,
	NPC_WORK_TIMEFLAG = CHAR_NPCWORKINT9,
	NPC_WORK_TALKFLAG = CHAR_NPCWORKINT10,
	NPC_WORK_CleanTime = CHAR_NPCWORKINT11,
};

enum {
	NPC_STATEINIT,
	NPC_STATEFREE,
	NPC_STATEBUSY,
	NPC_STATEWAIT,
};

enum {
	NPC_WORK_WINFMNAME = CHAR_NPCWORKCHAR1,
};

void NPC_ERR_FMDiSP(int meindex, int talker, int errNO);
static void NPC_FMWarpMan_selectWindow(int meindex, int toindex, int num, int select);
void NPC_FMBATTLESET(int floor, int index1, int index2, int flag);
void NPC_WarpFamily(int floor, int index1, int index2, int fl, int x, int y);
void NPC_BattleOut(int fmindex, int fmindex1);
void NPC_CleanPK(int floor, int meindex);
int NPC_FMFloorUse(int floor);
void NPC_talkToFloor(int floor, int index1, int index2, char *data);
void CheckLeavePK(int npcindex, int floor, int index1, int index2);

void CHECK_FMPknumInFloor( int meindex)
{
	int fmnum1 = 0, fmnum2 = 0;
	int fmpks_pos;

	fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
	// Nuke 20040920: Bug fix 蚽埶�侕�雛奀猁潰脤軞�侕�
	//print("<<%d %d>>",fmpks_pos,MAX_SCHEDULEMAN);
	//if( fmpks_pos < 0 || fmpks_pos >= MAX_SCHEDULEMAN ) return;
	if( fmpks_pos < 0 || fmpks_pos >= MAX_SCHEDULEMAN * MAX_SCHEDULE ) return;
	NPC_GetPKFMNum(CHAR_getWorkInt(meindex, NPC_WORK_WARPFLOOR),
		         		fmpks[fmpks_pos].host_index,
		         		fmpks[fmpks_pos].guest_index,
		         		&fmnum1, &fmnum2 );

	CHAR_setWorkInt( meindex, NPC_WORK_FMNUMI , fmnum1);
	CHAR_setWorkInt( meindex, NPC_WORK_FMNUMII, fmnum2);
}

extern int familyMemberIndex[FAMILY_MAXNUM][FAMILY_MAXMEMBER];

BOOL NPC_FMWarpManInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buff2[256];
	char buf[1024];
	int fl, x, y, meid;
	// shan
	int fl1, x1, y1;

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("FMWarpMan:GetArgStrErr");
		return FALSE;
	}

	if(NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP1", buf, sizeof( buf))==NULL){
	        print("FMWarpman Err is %s",npcarg);
		print("FMWarpman Err");
		return FALSE;
	}

	getStringFromIndexWithDelim(buf,",",1,buff2,sizeof(buff2));
	fl=atoi(buff2);
	CHAR_setWorkInt(meindex, NPC_WORK_WARPFLOOR, fl);
	getStringFromIndexWithDelim(buf,",",2,buff2,sizeof(buff2));
	x=atoi(buff2);
	getStringFromIndexWithDelim(buf,",",3,buff2,sizeof(buff2));
	y=atoi(buff2);

	if( MAP_IsValidCoordinate( fl,x,y )== FALSE ){
		print( "FMWarp NPC:Invalid warpman ERR" );
		return FALSE;
	}

    // shan begin
	if(NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP2", buf, sizeof( buf))==NULL){
	        print("FMWarpman Err is %s",npcarg);
		print("FMWarpman Err");
		return FALSE;
	}
	getStringFromIndexWithDelim(buf,",",1,buff2,sizeof(buff2));
	fl1=atoi(buff2);
	CHAR_setWorkInt(meindex, NPC_WORK_WARPFLOOR, fl);
	getStringFromIndexWithDelim(buf,",",2,buff2,sizeof(buff2));
	x1=atoi(buff2);
	getStringFromIndexWithDelim(buf,",",3,buff2,sizeof(buff2));
	y1=atoi(buff2);
	if( MAP_IsValidCoordinate( fl1,x1,y1 )== FALSE ){
		print( "FMWarp NPC:Invalid warpman ERR" );
		return FALSE;
	}
	// shan end
	
	meid = NPC_Util_GetNumFromStrWithDelim(npcarg, "ID");
	if ((meid < 0) || (meid >= MAX_SCHEDULEMAN))
	{
		print("FMWARP NPC: Init error invalid ID:%d\n", meid);
		return FALSE;
	}

	/*--淏騷鏤优擭--*/
	CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_INIT_LOOPTIME);
   	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEWARPMAN );
	CHAR_setWorkInt(meindex, NPC_WORK_MODEFLAG, NPC_STATEINIT);
	CHAR_setWorkInt(meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	CHAR_setWorkInt(meindex, NPC_WORK_TIMEFLAG, 0);
	CHAR_setWorkInt(meindex, NPC_WORK_TALKFLAG, -1);
	CHAR_setWorkInt(meindex, NPC_WORK_ID, meid);
	CHAR_setWorkInt(meindex, NPC_WORK_FMNUMI, 0);
	CHAR_setWorkInt(meindex, NPC_WORK_FMNUMII, 0);

	CHAR_setWorkInt( meindex, NPC_WORK_CleanTime, 6*10);
	
    return TRUE;
}

void NPC_FMWarpManTalked(int meindex, int talkerindex, char *szMes, int color)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	
	CHAR_setWorkInt( talkerindex, CHAR_WORKWARPCHECK, FALSE );
	
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }

	if(NPC_Util_isFaceToFace( meindex ,talkerindex ,2)==FALSE){
		if(NPC_Util_isFaceToChara(talkerindex,meindex,1 )==FALSE) return;
	}

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return ;
	}

	if(CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE){
		NPC_ERR_FMDiSP( meindex, talkerindex, 1);
	}

	CHAR_setWorkInt( talkerindex , CHAR_WORKSHOPRELEVANTTRD , 0 );
	NPC_FMWarpMan_selectWindow( meindex, talkerindex, 0, -1);
}

void NPC_FMWarpManLoop(int meindex)
{
	struct tm tm1;
	struct tm *tm2;
	int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
	int floor = CHAR_getWorkInt(meindex, NPC_WORK_WARPFLOOR);
#ifdef _DEATH_FAMILY_GM_COMMAND	// WON ADD 模逜桵GM硌鍔
	int i;
	int playernum = CHAR_getPlayerMaxNum();
#endif
	
	// WON ADD 党淏snprintf頗絳祡絞儂腔bug
	if( (tm2=localtime((time_t *)&NowTime.tv_sec) ) == NULL ){
		print("\n won ==> time err !! ");
		return;
	}
	memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
	
#ifdef _DEATH_FAMILY_GM_COMMAND	// WON ADD 模逜桵GM硌鍔
	{
		static int next_time[5] = {10,20,30,40,50}, flag[5] = {0};
		int now_time = time(NULL);
		int j, ID = 0;
		 
		ID = CHAR_getWorkInt(meindex, NPC_WORK_ID) - MANORNUM ;
		 
		if( flag[ ID - 1 ] == 0 ){
			next_time[ ID - 1 ] += now_time;
			flag[ ID -1 ] = 1;
		}
		 
		if( now_time > next_time[ID - 1] ){	
			next_time[ID -1] = now_time + (60 * 5);
			 
			for( i=0; i<MAX_SCHEDULE; i++ ){
				if( fmpks[ fmpks_pos + i ].flag == FMPKS_FLAG_SCHEDULED &&
					CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEFREE ){
					 
					char msg[128] = {0};
					int pk_min = fmpks[fmpks_pos + i ].prepare_time;
					 
					sprintf(msg, "[%s] VS [%s] 黺(%d:%d) 婓部華 %d 羲湖", 				
									fmpks[ fmpks_pos + i ].host_name,
									fmpks[ fmpks_pos + i ].guest_name,
									tm1.tm_hour + i, pk_min,  
									CHAR_getWorkInt(meindex, NPC_WORK_ID) - 9 );
					 
					for( j=0; j< playernum; j++ ){
						if( CHAR_CHECKINDEX( j ) != FALSE )	
						CHAR_talkToCli( j, -1, msg, CHAR_COLORYELLOW);
					}
				}
			} 
		}
	}
#endif
		
	if (tm1.tm_min == 0)
   	CHAR_setWorkInt(meindex, NPC_WORK_TIMEFLAG, tm1.tm_min);
	if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEINIT)
	{
		if (tm1.tm_sec == 0)
		{
			CHAR_setWorkInt(meindex, NPC_WORK_MODEFLAG, NPC_STATEFREE);
			CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_FREE_LOOPTIME);
		}
	}
	if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEFREE)
	{
		// �△藥褲區腴傘掉銫畏蠙褲動掉� >= 袧掘奀潔寀扢隅�瞿掁茛椕棧�
		
		if (tm1.tm_min > CHAR_getWorkInt(meindex, NPC_WORK_TALKFLAG)
			&& tm1.tm_min == 0)
		{
			// 籵眭傖埜輛部
			if (fmpks[fmpks_pos].flag == FMPKS_FLAG_SCHEDULED)
				NPC_talkToFloor(CHAR_getInt(meindex, CHAR_FLOOR),
				fmpks[fmpks_pos].host_index,
				fmpks[fmpks_pos].guest_index, "褫眕袧掘輛部賸ㄐ");
			CHAR_setWorkInt(meindex, NPC_WORK_TALKFLAG, tm1.tm_min);
		}
		if ((fmpks[fmpks_pos].prepare_time > 0) && (fmpks[fmpks_pos].flag == FMPKS_FLAG_SCHEDULED))
		{
			int clock = 0;
			
			NPC_FMBATTLESET(floor, fmpks[fmpks_pos].host_index,
				fmpks[fmpks_pos].guest_index, -1);
			if (tm1.tm_hour - (fmpks[fmpks_pos].dueltime / 100) < 0)
				clock = (fmpks[fmpks_pos].dueltime % 100) + fmpks[fmpks_pos].prepare_time - 60;
			else
				clock = fmpks[fmpks_pos].dueltime - (tm1.tm_hour * 100) + fmpks[fmpks_pos].prepare_time;
			
			if ((tm1.tm_min >= clock) && (fmpks[fmpks_pos].prepare_time > 0))
			{
				
				NPC_talkToFloor(floor, fmpks[fmpks_pos].host_index,
					fmpks[fmpks_pos].guest_index, "羲桵蹕‵ㄐㄐ");
				CHAR_setWorkInt(meindex, NPC_WORK_MODEFLAG, NPC_STATEBUSY);
				CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_BUSY_LOOPTIME);
				NPC_FMBATTLESET(floor, fmpks[fmpks_pos].host_index,
					fmpks[fmpks_pos].guest_index, 1);
				
				print("\n won ==> set fmwaperman state busy !!");
				
				
#ifdef _DEATH_FAMILY_GM_COMMAND	// WON ADD 模逜桵GM硌鍔
				{
					int num1 = 0, num2 = 0;
					int winflag = 0;
					int floor = CHAR_getWorkInt(meindex, NPC_WORK_WARPFLOOR);
					
					NPC_GetPKFMNum(floor, fmpks[fmpks_pos].host_index,
						fmpks[fmpks_pos].guest_index, &num1, &num2);
					
					if( num2 < 20 ){
						NPC_WarpFamily(floor, -1,
							fmpks[fmpks_pos].guest_index,
							CHAR_getInt(meindex, CHAR_FLOOR),
							CHAR_getInt(meindex, CHAR_X),
							CHAR_getInt(meindex, CHAR_Y));
						NPC_talkToFloor( floor,  -1, fmpks[fmpks_pos].guest_index, "秪帤雛媼坋�侀衋貐�" );
#ifdef _DEATH_FAMILY_STRUCT		// WON ADD 模逜桵湔溫吨蛹訧蹋
						{
							char out[256];
							sprintf( out, "[%s](%d) 秪帤雛媼坋�侔���",  fmpks[fmpks_pos].guest_name, num2 );
							saacproto_FM_PK_STRUCT_send( acfd, out );
						}
#endif
					}
					
					if( num1 < 20 ){
						NPC_WarpFamily(floor, fmpks[fmpks_pos].host_index,
							-1,
							CHAR_getInt(meindex, CHAR_FLOOR),
							CHAR_getInt(meindex, CHAR_X),
							CHAR_getInt(meindex, CHAR_Y));
						NPC_talkToFloor( floor,  fmpks[fmpks_pos].host_index, -1, "秪帤雛媼坋�侀衋貐�" );
#ifdef _DEATH_FAMILY_STRUCT		// WON ADD 模逜桵湔溫吨蛹訧蹋
						{
							char out[256];
							sprintf( out, "[%s](%d) 秪帤雛媼坋�侔���",  fmpks[fmpks_pos].host_name/* .host_index*/, num1 );
							saacproto_FM_PK_STRUCT_send( acfd, out );
						}
#endif		
					}
				}
#endif
			}
			else if (tm1.tm_min > CHAR_getWorkInt(meindex, NPC_WORK_TIMEFLAG))
			{
				char buf[256];
				int clock = 0;
				CHAR_setWorkInt(meindex, NPC_WORK_TIMEFLAG, tm1.tm_min);
				clock = ((fmpks[fmpks_pos].dueltime % 100) + fmpks[fmpks_pos].prepare_time) - tm1.tm_min;
				if (clock >= 60)	clock = clock - 60;
				sprintf(buf, "擒燭勤桵奀潔遜呁狟ㄩ%4d煦笘", clock);
				NPC_talkToFloor(floor, fmpks[fmpks_pos].host_index,
					fmpks[fmpks_pos].guest_index, buf);
			}
		}
	}else if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEBUSY){
		// 潰脤部囀�侕�﹜瓚剿吨蛹
		int num1 = 0, num2 = 0;
		int winflag = 0;
		int floor = CHAR_getWorkInt(meindex, NPC_WORK_WARPFLOOR);
		int meid = CHAR_getWorkInt(meindex, NPC_WORK_ID);
		
		print("\n won ==> check_winner : npc_meid(%d)", meid );
		
		NPC_GetPKFMNum(floor, fmpks[fmpks_pos].host_index,
			fmpks[fmpks_pos].guest_index, &num1, &num2);
		NPC_FMBATTLESET(floor, fmpks[fmpks_pos].host_index,
			fmpks[fmpks_pos].guest_index, 1);
		CheckLeavePK(meindex, floor, fmpks[fmpks_pos].host_index,
			fmpks[fmpks_pos].guest_index);
		
		if (meid > MANORNUM){
			if (tm1.tm_min == FMWARPMAN_CLEANPLACE){
				// 瓚剿吨蛹﹜WARP燭部﹜扢隅�峉苺譙棧�
				NPC_BattleOut(fmpks[fmpks_pos].host_index, fmpks[fmpks_pos].guest_index);
				NPC_FMBATTLESET(floor, fmpks[fmpks_pos].host_index,
					fmpks[fmpks_pos].guest_index, -1);
				if (fmpks[fmpks_pos].win == 0){// 汜湔桵
					if (num1 > num2)
						winflag = 1;
					else if(num1 < num2)
						winflag = 2;
					else
						winflag = 3;
				}else if (fmpks[fmpks_pos].win == 1){ // 湖滄桵
					if ((CHAR_getWorkInt(meindex, NPC_WORK_FMNUMI) - num1) < (CHAR_getWorkInt(meindex, NPC_WORK_FMNUMII) - num2))
						winflag = 1;
					else if ((CHAR_getWorkInt(meindex, NPC_WORK_FMNUMI) - num1) > (CHAR_getWorkInt(meindex, NPC_WORK_FMNUMII) - num2))
						winflag = 2;
					else
						winflag = 3;
				}
			}
		}
		if ((num1 == 0) && (num2 != 0)){
			winflag = 2;
		}else if ((num2 == 0) && (num1 != 0)){
			winflag = 1;
		}else if ((num1 == 0) && (num2 == 0)){
			winflag = 3;
		}      
		if (winflag > 0 && winflag < 3){
#ifdef _MANOR_PKRULE      
			if(meid > MANORNUM){
#endif
#ifdef _DEATH_FAMILY_GM_COMMAND	// WON ADD 模逜桵GM硌鍔
				{
					int i, win_index = -1, loser_index = -1;
					char *win_name, *loser_name, msg[256] = {0}, token[256] = {0};
					struct tm tm1;
					int n1 = 0, n2 = 0;
					//				FILE *f;
					
					if(winflag == 1){
						win_index = fmpks[fmpks_pos].host_index;
						win_name = fmpks[fmpks_pos].host_name;
						n1 = num1;
						loser_index = fmpks[fmpks_pos].guest_index;
						loser_name = fmpks[fmpks_pos].guest_name;
						n2 = num2;
					}else if(winflag == 2){
						win_index = fmpks[fmpks_pos].guest_index;
						win_name = fmpks[fmpks_pos].guest_name;
						n1 = num2;
						loser_index = fmpks[fmpks_pos].host_index;
						loser_name = fmpks[fmpks_pos].host_name;
						n2 = num1;
					}else{
						win_index = fmpks[fmpks_pos].host_index;
						win_name = fmpks[fmpks_pos].host_name;
						loser_index = fmpks[fmpks_pos].guest_index;
						loser_name = fmpks[fmpks_pos].guest_name;					
					}
					
					memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));
					
					if(winflag == 1 || winflag == 2){
						sprintf( msg, "[%s](%d)�� 吨 [%s](%d)��", win_name, n1, loser_name, n2 );
						sprintf( token, "桵錶畦惆ㄩ %s", msg );
					}else if( winflag == 3 ){				
						sprintf( msg, "[%s] す忒 [%s]", win_name, loser_name );
						sprintf( token, "桵錶畦惆ㄩ %s", msg );
					}
					
#ifdef _DEATH_FAMILY_STRUCT		// WON ADD 模逜桵湔溫吨蛹訧蹋
					{
						char out[256] = {0};
						sprintf( out, "%d/%d/%d:%d %s", tm1.tm_mon+1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, msg );
						saacproto_FM_PK_STRUCT_send( acfd, out );
					}
#endif
					
					for( i=0 ; i< playernum ; i++ ){
						if( CHAR_CHECKINDEX( i ) == FALSE )	continue;
						CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW);
					}				
					/*
					if( (f = fopen( "d_fm_pk_log.txt", "a+" ) ) ){
					fprintf( f, "%d/%d/%d:%d %s\n", tm1.tm_mon+1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, msg );
					fclose( f );
					}else{
					print("\n open file (d_fm_pk_log.txt) Err !!");
					}
					*/
				}
#else
				// 霜檢模逜pk 徹汒咡
				if (winflag == 1){	 		 
					saacproto_ACFixFMPK_send(acfd,
						fmpks[fmpks_pos].host_name,
						fmpks[fmpks_pos].host_index + 1,
						fmpks[fmpks_pos].host_index,
						fmpks[fmpks_pos].guest_name,
						fmpks[fmpks_pos].guest_index + 1,
						fmpks[fmpks_pos].guest_index);
				} else {			 
					saacproto_ACFixFMPK_send(acfd,
						fmpks[fmpks_pos].guest_name,
						fmpks[fmpks_pos].guest_index + 1,
						fmpks[fmpks_pos].guest_index,
						fmpks[fmpks_pos].host_name,
						fmpks[fmpks_pos].host_index + 1,
						fmpks[fmpks_pos].host_index);
				}  
#endif
				
#ifdef _MANOR_PKRULE      
			}
#endif
			
			{
				if (meid > 0 && meid <= MANORNUM){// CoolFish 2002/2/25 Change MANOR -> MANORNUM
#ifdef _NEW_MANOR_LAW
					int i,iFmIndex1,iFmIndex2,iCharindex;
#endif
					fmpks[fmpks_pos + 1].flag = FMPKS_FLAG_MANOR_BATTLEEND;
					if (winflag == 1){
						char token[256];
						sprintf( token, " (%d:%d) %d/%d/%d",
							tm1.tm_hour, tm1.tm_min,
							tm1.tm_year+1900, tm1.tm_mon+1, tm1.tm_mday);
						saacproto_ACFixFMPoint_send(acfd, 
							fmpks[fmpks_pos].host_name,
							fmpks[fmpks_pos].host_index + 1,
							fmpks[fmpks_pos].host_index,
							fmpks[fmpks_pos].guest_name,
							fmpks[fmpks_pos].guest_index + 1,
							fmpks[fmpks_pos].guest_index, meid);
						//Syu 崝樓蚽埶桵吨蛹Log
						Logfmpk(
							fmpks[fmpks_pos].host_name,
							fmpks[fmpks_pos].host_index,
							num1,
							fmpks[fmpks_pos].guest_name,
							fmpks[fmpks_pos].guest_index,
							num2, token, "", "", 2);
#ifdef _NEW_MANOR_LAW
						// 埻模逜忐蛂賸蚽埶,模逜傖埜褫腕善坒啟
						iFmIndex1 = fmpks[fmpks_pos].host_index;
						for(i=0;i<FAMILY_MAXMEMBER;i++){
							iCharindex = familyMemberIndex[iFmIndex1][i];
							// �譧硨葋炰顫醾�
							if(iCharindex >= 0 && CHAR_getCharUse(iCharindex)){
								// 鳳腕踢ヴ = 跺�侔鑫� * 5000
								int iAddGold = ((float)CHAR_getInt(iCharindex,CHAR_MOMENTUM)/100.0f) * 5000.0f;
								int iGold = CHAR_getInt(iCharindex,CHAR_BANKGOLD),iMaxGold;
								// 珂溫�賳鶲刳靇�
								if(iGold + iAddGold > CHAR_MAXBANKGOLDHAVE){
									CHAR_setInt(iCharindex,CHAR_BANKGOLD,CHAR_MAXBANKGOLDHAVE);
									// 跺�刳靇郱鑑銨臏�,溫善跺�侂篽�
									iAddGold = iGold + iAddGold - CHAR_MAXBANKGOLDHAVE;
									iGold = CHAR_getInt(iCharindex,CHAR_GOLD);
									iMaxGold = CHAR_getMaxHaveGold(iCharindex);
									if(iGold + iAddGold > iMaxGold) CHAR_setInt(iCharindex,CHAR_GOLD,iMaxGold);
									else CHAR_setInt(iCharindex,CHAR_GOLD,iGold + iAddGold);
									LogFMPKGetMomey(CHAR_getChar(iCharindex,CHAR_FMNAME),
																	CHAR_getChar(iCharindex,CHAR_CDKEY),
																	CHAR_getChar(iCharindex,CHAR_NAME),
																	CHAR_getInt(iCharindex,CHAR_MOMENTUM),iAddGold,0);
								}
								else{
									CHAR_setInt(iCharindex,CHAR_BANKGOLD,iGold + iAddGold);
									LogFMPKGetMomey(CHAR_getChar(iCharindex,CHAR_FMNAME),
																	CHAR_getChar(iCharindex,CHAR_CDKEY),
																	CHAR_getChar(iCharindex,CHAR_NAME),
																	CHAR_getInt(iCharindex,CHAR_MOMENTUM),iAddGold,1);
								}
								CHAR_talkToCli(iCharindex,-1,"釓賴賸!忐誘蛂蚽埶腔蔣踢眒颯�踽蒫譫鶲刳靇�",CHAR_COLORRED);
							}
						}
#endif
					} 
					else if (winflag == 2){	
						char token[256];
						sprintf( token, " (%d:%d) %d/%d/%d",
							tm1.tm_hour, tm1.tm_min,
							tm1.tm_year+1900, tm1.tm_mon+1, tm1.tm_mday);
						saacproto_ACFixFMPoint_send(acfd, 
							fmpks[fmpks_pos].guest_name,
							fmpks[fmpks_pos].guest_index + 1,
							fmpks[fmpks_pos].guest_index,
							fmpks[fmpks_pos].host_name,
							fmpks[fmpks_pos].host_index + 1,
							fmpks[fmpks_pos].host_index, meid);
						//Syu 崝樓蚽埶桵吨蛹Log
						Logfmpk(
							fmpks[fmpks_pos].guest_name,
							fmpks[fmpks_pos].guest_index,
							num2,
							fmpks[fmpks_pos].host_name,
							fmpks[fmpks_pos].host_index,
							num1, token, "", "", 2);
					}
					CHAR_setWorkInt(meindex, NPC_WORK_MODEFLAG, NPC_STATEFREE);
					CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_FREE_LOOPTIME);
					print("FMWarpMan State:%d\n", CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG));
#ifdef _NEW_MANOR_LAW
					// 邧源腔跺�佷側眢暲鑫げ模疚橧�
					iFmIndex1 = fmpks[fmpks_pos].host_index;
					iFmIndex2 = fmpks[fmpks_pos].guest_index;
					for(i=0;i<FAMILY_MAXMEMBER;i++){
						iCharindex = familyMemberIndex[iFmIndex1][i];
						// �譥俶硨葋珆藱撜�,祥婓盄奻腔婓腎�邆恘滔掏撜�
						if(iCharindex >= 0 && CHAR_getCharUse(iCharindex)){
							CHAR_setInt(iCharindex,CHAR_MOMENTUM,0);
							CHAR_talkToCli(iCharindex,-1,"蚽埶桵摽跺�佷側眢暲鑫さ橧�",CHAR_COLORRED);
						}
						iCharindex = familyMemberIndex[iFmIndex2][i];
						if(iCharindex >= 0 && CHAR_getCharUse(iCharindex)){
							CHAR_setInt(iCharindex,CHAR_MOMENTUM,0);
							CHAR_talkToCli(iCharindex,-1,"蚽埶桵摽跺�佷側眢暲鑫さ橧�",CHAR_COLORRED);
						}
					}
#endif
				}
			}
			if (winflag == 1)
			{
				CHAR_setWorkChar(meindex, NPC_WORK_WINFMNAME, fmpks[fmpks_pos].host_name);
			}
			else if (winflag == 2)
			{
				CHAR_setWorkChar(meindex, NPC_WORK_WINFMNAME, fmpks[fmpks_pos].guest_name);
				
			}
			NPC_WarpFamily(floor, fmpks[fmpks_pos].host_index,
				fmpks[fmpks_pos].guest_index,
				CHAR_getInt(meindex, CHAR_FLOOR),
				CHAR_getInt(meindex, CHAR_X),
				CHAR_getInt(meindex, CHAR_Y));
			if (meid > MANORNUM){// CoolFish 2002/2/25 Change MANOR -> MANORNUM
				CHAR_setWorkInt(meindex, NPC_WORK_MODEFLAG, NPC_STATEWAIT);
				CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_WAIT_LOOPTIME);
			}
      }else if( winflag == 3 ){
				if (meid > 0 && meid <= MANORNUM) // CoolFish 2002/2/25 Change MANOR -> MANORNUM
					
					fmpks[fmpks_pos + 1].flag = FMPKS_FLAG_MANOR_BATTLEEND;
				NPC_talkToFloor(floor, fmpks[fmpks_pos].host_index,
         	fmpks[fmpks_pos].guest_index, "邧源す忒ㄛ祥煦吨蛹ㄐㄐ");
				NPC_WarpFamily(floor, fmpks[fmpks_pos].host_index,
         	fmpks[fmpks_pos].guest_index,
         	CHAR_getInt(meindex, CHAR_FLOOR),
         	CHAR_getInt(meindex, CHAR_X),
         	CHAR_getInt(meindex, CHAR_Y));
				CHAR_setWorkInt(meindex, NPC_WORK_MODEFLAG, NPC_STATEWAIT);
				CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_WAIT_LOOPTIME);  
      }
   }else if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEWAIT){
		 
		 //	andy_edit 2002/07/29
		 if (tm1.tm_min == FMWARPMAN_CLEANPLACE)
			 CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_FREE_LOOPTIME);
		 
		 NPC_CleanPK(floor, meindex);
		 
		 if (tm1.tm_min == 0){
			 CHAR_setWorkInt(meindex, NPC_WORK_MODEFLAG, NPC_STATEFREE);
			 // shan add
			 NPC_FMBATTLESET(floor, fmpks[fmpks_pos].host_index, fmpks[fmpks_pos].guest_index, -1);
			 CHAR_setInt(meindex, CHAR_LOOPINTERVAL, FMWARPMAN_FREE_LOOPTIME);
			 // 蔚 fmwarpman 場宎趙
			 CHAR_setWorkInt(meindex, NPC_WORK_FMNUMI, 0);
			 CHAR_setWorkInt(meindex, NPC_WORK_FMNUMII, 0);
			 CHAR_setWorkInt(meindex, NPC_WORK_TIMEFLAG, 0);
			 CHAR_setWorkInt(meindex, NPC_WORK_TALKFLAG, -1);
			 CHAR_setWorkChar(meindex, NPC_WORK_WINFMNAME, "");
		 }
   }
}

void NPC_FMWarpManWindowTalked( int meindex, int talkerindex, 
		int seqno, int select, char *data)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024], buff2[256], tmpbuf[256];
	int fl, x, y, fmpks_pos, fd;

	if( !CHAR_CHECKINDEX( talkerindex) )return;
	fd = getfdFromCharaIndex(talkerindex);
	if (fd == -1)	return;

	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) {
		return;
	}
	if(select==WINDOW_BUTTONTYPE_OK){
		return;
	}
   	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return ;
	}
	// shan add
	fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
   
	if (CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].host_index){
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP1", buf, sizeof( buf)) == NULL ){
			print("FM WARPMAN Can't Read WARP1!\n");
			return;
		}
	}else if (CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].guest_index){
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP2", buf, sizeof( buf)) == NULL ){
			print("FM WARPMAN Can't Read WARP2!\n");
			return;
		}
	}else{
		CHAR_talkToCli( talkerindex, meindex, "斕甜準勤桵邧源垀扽�扂遞眢撜尕悵�", CHAR_COLORYELLOW);
		return;
	}
	getStringFromIndexWithDelim(buf,",",1,buff2,sizeof(buff2));
	fl=atoi(buff2);
	getStringFromIndexWithDelim(buf,",",2,buff2,sizeof(buff2));
	x=atoi(buff2);
	getStringFromIndexWithDelim(buf,",",3,buff2,sizeof(buff2));
	y=atoi(buff2);


	switch (seqno){
	case CHAR_WINDOWTYPE_WINDOWWARPMAN_MAIN:
		if(select==WINDOW_BUTTONTYPE_YES){
			if( CHAR_getWorkInt( talkerindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return;
			if(CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE){
				CHAR_talkToCli( talkerindex, meindex, "③賤汃芶勦ㄐ", CHAR_COLORYELLOW);
				return;
			}

			if(CHAR_getWorkInt( talkerindex , CHAR_WORKSHOPRELEVANTTRD) <= 0){
				if( CHAR_getWorkInt( talkerindex, CHAR_WORKWARPCHECK ) == TRUE )  {
					if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEFREE){
         		if (CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].host_index){
         			int tmpnum1 = CHAR_getWorkInt(meindex, NPC_WORK_FMNUMI) + 1;
	         		CHAR_setWorkInt(meindex, NPC_WORK_FMNUMI, tmpnum1);
		       	}else if (CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].guest_index){
			     		int tmpnum2 = CHAR_getWorkInt(meindex, NPC_WORK_FMNUMII) + 1;
				   		CHAR_setWorkInt(meindex, NPC_WORK_FMNUMII, tmpnum2);
						}else{
							CHAR_talkToCli( talkerindex, meindex, "斕甜準勤桵邧源垀扽�扂遞眢撜尕悵�", CHAR_COLORYELLOW);
						}
	         	CHAR_setWorkInt(talkerindex, CHAR_WORKFMPKFLAG, 1);
		       	CHAR_setWorkInt(talkerindex, CHAR_WORKFMMANINDEX, meindex);
			     	CHAR_setWorkInt( talkerindex, CHAR_WORKWARPCHECK, FALSE );
				   	CHAR_warpToSpecificPoint(talkerindex, fl, x,y);
/*						{
							FILE *fp;
							struct tm tm1;
							char szFileName[128];

							memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
							memset(szFileName,0,sizeof(szFileName));
							sprintf(szFileName,"FMPkWarp.%d%d%d.log",tm1.tm_mon,tm1.tm_hour,tm1.tm_min);
							fp = fopen(szFileName,"a+");
							if(fp != NULL){
								fprintf(fp,"FMName:%s\tName:%s\tWarpHost:%d\tWarpGuest:%d\tFloor:%d,%d,%d\tMaxPlayer:%d\n",
														CHAR_getChar(talkerindex,CHAR_FMNAME),CHAR_getChar(talkerindex,CHAR_NAME),
														CHAR_getWorkInt(meindex,NPC_WORK_FMNUMI),
														CHAR_getWorkInt(meindex,NPC_WORK_FMNUMII),
														fl,x,y,
														fmpks[fmpks_pos].max_player);
								fclose(fp);
							}
							else printf("%s(errno:%x)\n",sys_errlist[errno],errno);
						}
*/
					}
					else return;
					{
						struct tm tm1;
						int clock = 0;
						int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
						memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
						clock = ((fmpks[fmpks_pos].dueltime % 100) + fmpks[fmpks_pos].prepare_time) - tm1.tm_min;
						if (clock >= 60)	clock = clock - 60;
						sprintf(tmpbuf, "\n③脹緊邧源袧掘俇救摽婬羲桵ㄐ"
							"\n桵須奀③祥猁呴砩腎堤麼燭羲﹝"
							"\n荇腕吨瞳腔模逜珩③珂尕摽珨狟ㄛ"
							"\n脹渾笛瓚腔哫票摽婬燭部ㄐ"
							"\n郅郅蠟腔磁釬ㄐ"
							"\n擒燭勤桵奀潔遜呁狟ㄩ%4d煦笘",
							clock);
						lssproto_WN_send(fd, 
			         	WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
								-1, -1,
								makeEscapeString(tmpbuf, buff2, sizeof(buff2)));
					}
				}
			}
			else{
				if(CHAR_getInt(talkerindex,CHAR_GOLD) < CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)){
					NPC_ERR_FMDiSP( meindex, talkerindex, 2);
					return ;
				}
				CHAR_DelGold( talkerindex, CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD ));
				CHAR_setWorkInt( talkerindex, CHAR_WORKWARPCHECK, FALSE );
				CHAR_warpToSpecificPoint(talkerindex, fl, x,y);
			}
		}
		break;
	}
}

void NPC_ERR_FMDiSP(int meindex,int talker,int errNO)
{
	
	char token[1024];
	int i=0;
	int otherindex;
	int fd = getfdFromCharaIndex( talker);
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return ;
	}
	
	if(errNO==1){
		/*--蚕↓  贖↓ぁ  媃凝崹倜--*/
		if(NPC_Util_GetStrFromStrWithDelim( npcarg, "PartyMsg",token, sizeof( token))==NULL) {
			/*--优擭踏躂趙埵崹躂�氿�き摯隍蚗掛↓喇禱窅竣--*/
			sprintf(token, "\n\n拸楊樓�輹韃茛甭輴�賤汃芶勦ㄐ");
		}

		if(CHAR_getWorkInt(talker,CHAR_WORKPARTYMODE)==CHAR_PARTY_CLIENT){
					
		}else{
			
		/*--惉↓譫↓誑  媃凝崹倜鳹譴摯陑--*/
		/*--蚕↓  贖↓蠈劂勗隍蚗掛↓喇禱③埰--*/		
		for( i=0 ; i < CHAR_PARTYMAX ;i++)
		{
			otherindex=CHAR_getWorkInt(talker,CHAR_WORKPARTYINDEX1+i);

			if(otherindex != -1){
				 fd = getfdFromCharaIndex( otherindex);

				/*-喫喫ぁ邞嚭埰堎--*/
				lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
							WINDOW_BUTTONTYPE_OK,
							CHAR_WINDOWTYPE_WINDOWWARPMAN_ERR, 
							CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							token);
				}
			}
			return ;
		}
	
	}else if (errNO==2){
		/*--鉣誑倜堇埵笢ぁ埰��--*/
		if(NPC_Util_GetStrFromStrWithDelim( npcarg, "MoneyMsg", token, sizeof( token))==NULL){
			/*--堁鉣誑鵿堇埵笢摯勗��优擭踏躂趙埵凝埱倜�掁�き摯隍蚗掛↓喇禱  渦堇勗③埰--*/
			sprintf(token,"\n\n踢ヴ侔綱祥逋遄ㄛ③湔疑ヴ摽婬徹懂﹝");
			}
		}

		/*-喫喫ぁ邞嚭埰堎--*/
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK,
						CHAR_WINDOWTYPE_WINDOWWARPMAN_ERR, 
						CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
						token);
}

static void NPC_FMWarpMan_selectWindow( int meindex, int toindex, int num,int select)
{
	struct tm tm1;
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[1024];
	char buf[256];
	char buf2[1024];
	char buf3[256];
	int fl = 0, num1 = 0, num2 = 0;
	int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
	int floor = CHAR_getWorkInt(meindex, NPC_WORK_WARPFLOOR);
	int fd = getfdFromCharaIndex( toindex);	
	if( fd == -1 ) {
		fprint( "getfd err\n");
		return;
	}
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return ;
	}
	/*--优擭啞栝騷鳴摯  勗啞痲囮硞倳誑炩擭踏躂趙笢堎凝踏躂趙笢躂�仱袚藡我俴捊啤帟鉸醓�*/
	if(strstr(npcarg,"%4d")!=NULL){
		NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP1", buf, sizeof( buf));
		getStringFromIndexWithDelim(buf,",",1,buf3,sizeof(buf3));
		fl = NPC_FMFloorUse(atoi(buf3));
	}
	CHAR_setWorkInt(toindex, CHAR_WORKWARPCHECK, TRUE);
	CHAR_setWorkInt(toindex, NPC_WORK_WARPFLOOR, fl);
	memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
	
	if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEFREE)
	{
		if (((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].host_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos].host_name) == 0))
			|| ((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].guest_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos].guest_name) == 0)))
		{
			if ((fmpks[fmpks_pos].host_index != -1)
		      && (CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].host_index))
			{
				// WON ADD 党淏模逜pk部腔埮桵恀枙
				
				if(fmpks[fmpks_pos].flag == -1)		return;
				
				if (fmpks[fmpks_pos].flag != FMPKS_FLAG_SCHEDULED)
					
				{
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK, -1, -1,
						makeEscapeString("\n涴部桵須勤源疑砉遜羶肮砩儸ㄐ\n狟棒③暮腕珂肂埮勤源摽婬懂腎暮勘‵", token, sizeof(token)));
					return;
				}
#ifdef _FMVER21		   	
				if (CHAR_getInt(toindex, CHAR_FMLEADERFLAG) == FMMEMBER_APPLY)
#else
				if (CHAR_getInt(toindex, CHAR_FMLEADERFLAG) == 0)
#endif		   	
				{	
					char	tmpbuf[256];
					sprintf(tmpbuf, "\n斕遜羶衄淏宒樓��%s模逜ㄛ垀眕祥夔輛部ㄐ", fmpks[fmpks_pos].host_name);
					lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK, -1, -1,
						makeEscapeString(tmpbuf, token, sizeof(token)));
					return;
				}
				//andy_add 2003/06/17
				CHECK_FMPknumInFloor( meindex);
				if (CHAR_getWorkInt(meindex, NPC_WORK_FMNUMI) > (fmpks[fmpks_pos].max_player - 1)){
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK, -1, -1, makeEscapeString("\n眒冪祥夔婬輛�氐煄咯﹏n模逜輛部�侕�眒冪善湛扢隅奻癹賸ㄐ", token, sizeof(token)));
					return;
				}
			}
			else if ((fmpks[fmpks_pos].guest_index != -1)
		      && (CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].guest_index))
			{
				// WON ADD 党淏模逜pk部腔埮桵恀枙
				
				if(fmpks[fmpks_pos].flag == -1)		return;
				
				if (fmpks[fmpks_pos].flag != FMPKS_FLAG_SCHEDULED)
					
				{
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK, -1, -1,
						makeEscapeString("\n涴部桵須斕疑砉遜羶肮砩儸ㄐ\n狟棒③暮腕枑婌懂躲扂�溜洁�", token, sizeof(token)));
					return;
				}
#ifdef _FMVER21		   	
				if (CHAR_getInt(toindex, CHAR_FMLEADERFLAG) == FMMEMBER_APPLY)
#else
					if (CHAR_getInt(toindex, CHAR_FMLEADERFLAG) == 0)
#endif		   	
				{
					char	tmpbuf[256];
					sprintf(tmpbuf, "\n斕遜羶衄淏宒樓��%s模逜ㄛ垀眕祥夔輛部ㄐ", fmpks[fmpks_pos].guest_name);
					lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK, -1, -1,
						makeEscapeString(tmpbuf, token, sizeof(token)));
					return;
				}
				//andy_add 2003/06/17
				CHECK_FMPknumInFloor( meindex);
				if( CHAR_getWorkInt( meindex, NPC_WORK_FMNUMII) > (fmpks[fmpks_pos].max_player - 1)){
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK, -1, -1,
						makeEscapeString("眒冪祥夔婬輛�氐煄咯﹏n模逜輛部�侕�眒冪善湛扢隅奻癹賸ㄐ", token, sizeof(token)));
					return;
				}
			}
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "FreeMsg", buf2, sizeof( buf2)) == NULL) return;
			CONNECT_set_pass(fd, TRUE);
			CONNECT_set_first_warp(fd, TRUE);
			sprintf(token, buf2, fmpks[fmpks_pos].guest_name, fmpks[fmpks_pos].host_name,
				tm1.tm_hour, tm1.tm_min);
			CHAR_setWorkInt( toindex , CHAR_WORKFMMANINDEX, meindex );
			CHAR_setWorkInt( toindex , CHAR_WORKSHOPRELEVANTTRD , -1 );
		} else if ((((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos + 1].host_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos + 1].host_name) == 0))
			|| ((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos + 1].guest_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos + 1].guest_name) == 0)))
			&& (((fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_SCHEDULED)
			   || (fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_MANOR_PREPARE)))
				 )
		{
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "NextMsg", buf2, sizeof( buf2)) == NULL) return;
			sprintf(token, buf2, fl, tm1.tm_hour, tm1.tm_min);
			CONNECT_set_pass(fd, FALSE);
			CONNECT_set_first_warp(fd, FALSE);
		}
		else if ((strcmp(fmpks[fmpks_pos].host_name, "") ==  0) || (strcmp(fmpks[fmpks_pos].guest_name, "") == 0))
		{
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "TalkMsg", buf2, sizeof( buf2)) == NULL) return;
			sprintf(token, buf2);
			CONNECT_set_pass(fd, FALSE);
			CONNECT_set_first_warp(fd, FALSE);
		}
		else
		{
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "AskMsg", buf2, sizeof( buf2)) == NULL) return;
			CONNECT_set_pass(fd, FALSE);
			CONNECT_set_first_warp(fd, FALSE);
			sprintf(token, buf2, fmpks[fmpks_pos].guest_name, fmpks[fmpks_pos].host_name);
		}
	}
	else if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEBUSY) // 謗模逜桵須笢
	{
		if (((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].host_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos].host_name) == 0))
			|| ((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].guest_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos].guest_name) == 0)))
		{
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "BusyMsg", buf2, sizeof( buf2)) == NULL){
				print("\nGet BusyMsg Message Error");
				return;
			}
			NPC_GetPKFMNum(floor,
				fmpks[fmpks_pos].host_index,
				fmpks[fmpks_pos].guest_index,
				&num1, &num2);
			sprintf(token, buf2, 
				fmpks[fmpks_pos].host_name, num1,
				fmpks[fmpks_pos].guest_name, num2,
				tm1.tm_hour, tm1.tm_min);
		} else if ((((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos + 1].host_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos + 1].host_name) == 0))
			|| ((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos + 1].guest_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos + 1].guest_name) == 0)))
			&& (((fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_SCHEDULED)
			   || (fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_MANOR_PREPARE)))
				 )
		{
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "NextMsg", buf2, sizeof( buf2)) == NULL) return;
			sprintf(token, buf2, fl, tm1.tm_hour, tm1.tm_min);
		} else {
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "AskMsg", buf2, sizeof( buf2)) == NULL) return;
			sprintf(token, buf2, fmpks[fmpks_pos].guest_name, fmpks[fmpks_pos].host_name);
		}
		CONNECT_set_pass(fd, FALSE);
		CONNECT_set_first_warp(fd, FALSE);
	}
	else if (CHAR_getWorkInt(meindex, NPC_WORK_MODEFLAG) == NPC_STATEWAIT) // 謗模逜桵須賦旰
	{
		if (((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].host_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos].host_name) == 0))
			|| ((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos].guest_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos].guest_name) == 0)))
		{
			char tmpbuf[256];
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "EndMsg", buf2, sizeof( buf2)) == NULL){
				print("\nGet EndMsg Message Error");
				return;
			}
			NPC_GetPKFMNum(floor,
				fmpks[fmpks_pos].host_index,
				fmpks[fmpks_pos].guest_index,
				&num1, &num2);
			if (strcmp(CHAR_getWorkChar(meindex, NPC_WORK_WINFMNAME), "") == 0)
				sprintf(tmpbuf, "邧源す忒ㄐ");
			else sprintf(tmpbuf, "%s鳳吨賸ㄐㄐ", CHAR_getWorkChar(meindex, NPC_WORK_WINFMNAME));
			sprintf(token, buf2,  fmpks[fmpks_pos].guest_name,
				fmpks[fmpks_pos].host_name, tmpbuf);
		} else if ((((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos + 1].host_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos + 1].host_name) == 0))
			|| ((CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI) == fmpks[fmpks_pos + 1].guest_index)
			&& (strcmp(CHAR_getChar(toindex, CHAR_FMNAME), fmpks[fmpks_pos + 1].guest_name) == 0)))
			&& (((fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_SCHEDULED)
			   || (fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_MANOR_PREPARE)))
				 )
		{
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "NextMsg", buf2, sizeof( buf2)) == NULL) return;
			sprintf(token, buf2, fl, tm1.tm_hour, tm1.tm_min);
		} else {
			if(NPC_Util_GetStrFromStrWithDelim( npcarg, "AskMsg", buf2, sizeof( buf2)) == NULL) return;
			sprintf(token, buf2, fmpks[fmpks_pos].guest_name, fmpks[fmpks_pos].host_name);
		}
		CONNECT_set_pass(fd, FALSE);
		CONNECT_set_first_warp(fd, FALSE);
	}
	/*-喫喫ぁ邞嚭埰堎--*/
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_YESNO,
		CHAR_WINDOWTYPE_WINDOWWARPMAN_MAIN, 
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
		token);
	
}

int NPC_FMFloorUse(int floor)
{
	int	i;
	int	players = 0;
	int     playernum = CHAR_getPlayerMaxNum();

	/* 鏤畛騷媕↓鏍攝蚗袲 */
	for( i=0 ; i< playernum ; i++ ){
		if( CHAR_getCharUse(i) == FALSE )continue;
			if(CHAR_getInt(i,CHAR_FLOOR)==floor){
				players++;
			}
	}
	return players;
}

void NPC_GetPKFMNum(int floor, int index1, int index2, int *num1, int *num2)
{
	int	i = 0, charindex;

	*num1 = 0; *num2 = 0;

	/* 鏤畛騷媕↓鏍攝蚗袲 */
	for (i = 0 ; i < FAMILY_MAXMEMBER; i++ ){
		charindex = familyMemberIndex[ index1][i];
		if( CHAR_getCharUse( charindex) ){
			if (CHAR_getInt(charindex, CHAR_FLOOR) == floor)
				*num1 = *num1 + 1;
		}else
			familyMemberIndex[ index1][i] = -1;

		charindex = familyMemberIndex[index2][i];
		if (CHAR_getCharUse(charindex)){
			if (CHAR_getInt(charindex, CHAR_FLOOR) == floor)
				*num2 = *num2 + 1;
		}else
			familyMemberIndex[ index2][i] = -1;
	}

}

void NPC_FMBATTLESET(int floor, int index1, int index2, int flag)
{
	int	i = 0, charindex;

	for (i = 0; i < FAMILY_MAXMEMBER; i++)
	{
		charindex = familyMemberIndex[index1][i];
		if (CHAR_getCharUse(charindex))
		{
			if (CHAR_getInt(charindex ,CHAR_FLOOR) == floor)
			{
			        CHAR_setWorkInt( charindex, CHAR_WORKBATTLEFLAG, flag);
			        if(flag==1)
    				    CHAR_setFlg(charindex, CHAR_ISDUEL, 1);
		        }
		}
		else
			familyMemberIndex[index1][i] = -1;
		charindex = familyMemberIndex[index2][i];
		if (CHAR_getCharUse(charindex))
		{
			if (CHAR_getInt(charindex ,CHAR_FLOOR) == floor)
			{
			        CHAR_setWorkInt( charindex, CHAR_WORKBATTLEFLAG, flag);
			        if(flag==1)
			            CHAR_setFlg(charindex, CHAR_ISDUEL, 1);
			}
		}
		else
			familyMemberIndex[index2][i] = -1;
	}
}

void NPC_CleanPK(int floor, int meindex)
{
	int	i;
	int     playernum = CHAR_getPlayerMaxNum();
	int	fl = CHAR_getInt(meindex, CHAR_FLOOR);
	int	x = CHAR_getInt(meindex, CHAR_X);
	int	y = CHAR_getInt(meindex, CHAR_Y);

	for( i=0 ; i< playernum ; i++ )
	{
	   if( CHAR_getCharUse(i) == FALSE )continue;
	   	if(CHAR_getInt(i,CHAR_FLOOR)==floor)
	   		CHAR_warpToSpecificPoint(i, fl, x, y);
	}
}

void NPC_talkToFloor(int floor, int index1, int index2, char *data)
{
	int 	i = 0, charindex;

	for (i = 0; i < FAMILY_MAXMEMBER; i++)
	{
		charindex = familyMemberIndex[index1][i];
		if (CHAR_getCharUse(charindex))
		{
			print("charname:%s fmname:%s\n", 
				CHAR_getChar(charindex, CHAR_NAME),
				CHAR_getChar(charindex, CHAR_FMNAME));
			if (CHAR_getInt(charindex, CHAR_FLOOR) == floor)
				CHAR_talkToCli(charindex, -1, data, CHAR_COLORYELLOW);
		}
		else
			familyMemberIndex[index1][i] = -1;
		charindex = familyMemberIndex[index2][i];
		if (CHAR_getCharUse(charindex))
		{
			print("charname:%s fmname:%s\n", 
				CHAR_getChar(i, CHAR_NAME),
				CHAR_getChar(i, CHAR_FMNAME));
			if (CHAR_getInt(charindex, CHAR_FLOOR) == floor)
				CHAR_talkToCli(charindex, -1, data, CHAR_COLORRED);
		}
		else
			familyMemberIndex[index2][i] = -1;
	}
}

void NPC_WarpFamily(int floor, int index1, int index2, int fl, int x, int y)
{
        int     i, charindex1, charindex2;

        for (i = 0; i < FAMILY_MAXMEMBER; i++){
                charindex1 = familyMemberIndex[index1][i];
                charindex2 = familyMemberIndex[index2][i];
                if (CHAR_getCharUse(charindex1))
                {
                   if (CHAR_getInt(charindex1, CHAR_FLOOR) == floor)
                   {
                   	CHAR_setWorkInt(charindex1, CHAR_WORKBATTLEFLAG, FALSE);
                   	CHAR_warpToSpecificPoint(charindex1, fl, x, y);
                   }
                }
                else
                         familyMemberIndex[index1][i] = -1;
                if (CHAR_getCharUse(charindex2))
                {
                   if (CHAR_getInt(charindex2, CHAR_FLOOR) == floor)
                   {
                   	CHAR_setWorkInt(charindex2, CHAR_WORKBATTLEFLAG, FALSE);
                   	CHAR_warpToSpecificPoint(charindex2, fl, x, y);
                   }
                }
                else
                         familyMemberIndex[index1][i] = -1;
        }
}

void NPC_BattleOut(int index1, int index2)
{
        int i, charindex1, charindex2;

        for (i = 0; i < FAMILY_MAXMEMBER; i++)
        {
                charindex1 = familyMemberIndex[index1][i];
                charindex2 = familyMemberIndex[index2][i];
                if(CHAR_getCharUse(charindex1)){
                         if(CHAR_getWorkInt( charindex1, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
                                 BATTLE_WatchStop(charindex1);
                }
                else
                         familyMemberIndex[index1][i] = -1;

                if(CHAR_getCharUse(charindex2)){
                         if(CHAR_getWorkInt( charindex2, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
                                 BATTLE_WatchStop(charindex2);
                }
                else
                         familyMemberIndex[index2][i] = -1;
        }   
}

void CheckLeavePK(int npcindex, int floor, int index1, int index2)
{
	int 	i = 0, charindex, fl = 0, x = 0, y = 0;
	fl = CHAR_getInt(npcindex, CHAR_FLOOR);
	x = CHAR_getInt(npcindex, CHAR_X);
	y = CHAR_getInt(npcindex, CHAR_Y);

	for (i = 0; i < FAMILY_MAXMEMBER; i++)
	{
	   charindex = familyMemberIndex[index1][i];
	   if (CHAR_getCharUse(charindex)){
	      if (CHAR_getInt(charindex, CHAR_FLOOR) == floor){
	         if (CHAR_getWorkInt(charindex, CHAR_WORKFMPKFLAG) < 0){
	            if (CHAR_getWorkInt(charindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
	            	CHAR_DischargeParty( charindex, 0);
	            CHAR_setWorkInt(charindex, CHAR_WORKWARPCHECK, FALSE);
	            CHAR_setWorkInt(charindex, CHAR_WORKBATTLEFLAG, FALSE);
	            CHAR_warpToSpecificPoint(charindex, fl, x, y);
	            CHAR_talkToCli(charindex, -1, "桵啖燭部", CHAR_COLORRED);
	         }
	      }
	   }else
	      familyMemberIndex[index1][i] = -1;
	   charindex = familyMemberIndex[index2][i];
	   if (CHAR_getCharUse(charindex)){
	      if (CHAR_getInt(charindex, CHAR_FLOOR) == floor){
	         if (CHAR_getWorkInt(charindex, CHAR_WORKFMPKFLAG) < 0){
	            if (CHAR_getWorkInt(charindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
	            	CHAR_DischargeParty(charindex, 0);
	            CHAR_setWorkInt(charindex, CHAR_WORKWARPCHECK, FALSE);
	            CHAR_setWorkInt(charindex, CHAR_WORKBATTLEFLAG, FALSE);
	            CHAR_warpToSpecificPoint(charindex, fl, x, y);
	            CHAR_talkToCli(charindex, -1, "桵啖燭部", CHAR_COLORRED);
	         }
	      }
	   }else
	      familyMemberIndex[index2][i] = -1;
	}
}
