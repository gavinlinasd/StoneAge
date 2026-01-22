#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "handletime.h"
#include "readmap.h"
#include "log.h"
#include "npc_eventaction.h"

#ifdef _GAMBLE_ROULETTE

#include "npc_gamblemaster.h"
void defPlayerGold( int meindex, int flg);
void NPC_GAMBLEMASTER_RESET( int meindex);
void Codef_Gold( int meindex, int toindex, int stone, int flg, char *token);
void NPC_GambleRoulette_selectWindow(int meindex,int toindex,int num,int select);

enum
{
	ROULETTE_START=0,
	ROULETTE_SELECT,
	ROULETTE_LOOK,
	ROULETTE_END,
};

enum
{
	roulette1 = 11,	roulette2, roulette3, roulette4, roulette5,	roulette6, roulette7, 
	roulette8, roulette9, roulette10, roulette11, roulette12, roulette13, roulette14,
	roulette15,	roulette16,	roulette17,	roulette18,	roulette19,	roulette20,	

	roulette21 = 41, roulette22, roulette23, roulette24, roulette25,	roulette26,	
	roulette27,	roulette28,	roulette29, roulette30, roulette31, roulette32,
	roulette33,	roulette34,	roulette35,	roulette36,	roulette37,	roulette38,
	roulette39,	roulette40,
	roulette41 = 71, roulette42, roulette43, roulette44, roulette45,

	roulette51 = 101, roulette52=102, roulette53=103,
	roulette61 = 111, roulette62=112,
};

typedef struct tagGambleMaster
{
	int m_fx;
	int m_fy;
	int m_type;	//瘍鎢 
	int m_RG;	//綻蟯1 2
	int m_EO;	//等邧1 2
	int m_SI;
	int m_IN;
	char str_type[56];
	int m_nums;
}GambleEndType;
GambleEndType EndTypedef[60]={
{22, 8 , roulette1 , roulette41, roulette43, roulette51, roulette61, "瘍鎢綻ㄠ", 0},
{7 , 8 , roulette1 , roulette41, roulette43, roulette51, roulette61, "瘍鎢綻ㄠ", 0},
{22, 10, roulette3 , roulette41, roulette43, roulette51, roulette61, "瘍鎢綻ㄢ", 0},
{7 , 6 , roulette3 , roulette41, roulette43, roulette51, roulette61, "瘍鎢綻ㄢ", 0},
{20, 11, roulette5 , roulette41, roulette43, roulette51, roulette61, "瘍鎢綻ㄤ", 0},
{9 , 5 , roulette5 , roulette41, roulette43, roulette51, roulette61, "瘍鎢綻ㄤ", 0},
{19, 5 , roulette6 , roulette41, roulette44, roulette53, roulette61, "瘍鎢綻ㄥ", 0},
{10, 11, roulette6 , roulette41, roulette44, roulette53, roulette61, "瘍鎢綻ㄥ", 0},
{17, 5 , roulette8 , roulette41, roulette44, roulette53, roulette61, "瘍鎢綻ㄧ", 0},
{12, 11, roulette8 , roulette41, roulette44, roulette53, roulette61, "瘍鎢綻ㄧ", 0},
{16, 5 , roulette9 , roulette41, roulette43, roulette53, roulette61, "瘍鎢綻ㄨ", 0},
{13, 11, roulette9 , roulette41, roulette43, roulette53, roulette61, "瘍鎢綻ㄨ", 0},
{15, 5 , roulette10, roulette41, roulette44, roulette53, roulette61, "瘍鎢綻ㄠㄟ", 0},
{14, 11, roulette10, roulette41, roulette44, roulette53, roulette61, "瘍鎢綻ㄠㄟ", 0},
{12, 5 , roulette13, roulette41, roulette43, roulette52, roulette62, "瘍鎢綻ㄠㄢ", 0},
{17, 11, roulette13, roulette41, roulette43, roulette52, roulette62, "瘍鎢綻ㄠㄢ", 0},
{22, 6 , roulette17, roulette41, roulette43, roulette53, roulette62, "瘍鎢綻ㄠㄦ", 0},
{7 , 10, roulette17, roulette41, roulette43, roulette53, roulette62, "瘍鎢綻ㄠㄦ", 0},
{22, 5 , roulette18, roulette41, roulette44, roulette52, roulette62, "瘍鎢綻ㄠㄧ", 0},
{7 , 11, roulette18, roulette41, roulette44, roulette52, roulette62, "瘍鎢綻ㄠㄧ", 0},

{22, 9 , roulette22, roulette42, roulette44, roulette51, roulette61, "瘍鎢蟯ㄡ", 0},
{7 , 7 , roulette22, roulette42, roulette44, roulette51, roulette61, "瘍鎢蟯ㄡ", 0},
{21, 11, roulette24, roulette42, roulette44, roulette51, roulette61, "瘍鎢蟯ㄣ", 0},
{8 , 5 , roulette24, roulette42, roulette44, roulette51, roulette61, "瘍鎢蟯ㄣ", 0},
{18, 5 , roulette27, roulette42, roulette43, roulette53, roulette61, "瘍鎢蟯ㄦ", 0},
{11, 11, roulette27, roulette42, roulette43, roulette53, roulette61, "瘍鎢蟯ㄦ", 0},
{19, 11, roulette31, roulette42, roulette43, roulette51, roulette62, "瘍鎢蟯ㄠㄠ", 0},
{10, 5 , roulette31, roulette42, roulette43, roulette51, roulette62, "瘍鎢蟯ㄠㄠ", 0},
{18, 11, roulette32, roulette42, roulette44, roulette51, roulette62, "瘍鎢蟯ㄠㄡ", 0},
{11, 5 , roulette32, roulette42, roulette44, roulette51, roulette62, "瘍鎢蟯ㄠㄡ", 0},
{16, 11, roulette34, roulette42, roulette44, roulette52, roulette62, "瘍鎢蟯ㄠㄣ", 0},
{14, 5 , roulette34, roulette42, roulette44, roulette52, roulette62, "瘍鎢蟯ㄠㄣ", 0},
{15, 11, roulette35, roulette42, roulette43, roulette52, roulette62, "瘍鎢蟯ㄠㄤ", 0},
{13, 5 , roulette35, roulette42, roulette43, roulette52, roulette62, "瘍鎢蟯ㄠㄤ", 0},
{22, 7 , roulette36, roulette42, roulette44, roulette53, roulette62, "瘍鎢蟯ㄠㄥ", 0},
{7 , 9 , roulette36, roulette42, roulette44, roulette53, roulette62, "瘍鎢蟯ㄠㄥ", 0},
{21, 5 , roulette39, roulette42, roulette43, roulette52, roulette62, "瘍鎢蟯ㄠㄨ", 0},
{8 , 11, roulette39, roulette42, roulette43, roulette52, roulette62, "瘍鎢蟯ㄠㄨ", 0},
{20, 5 , roulette40, roulette42, roulette44, roulette52, roulette62, "瘍鎢蟯ㄡㄟ", 0},
{9 , 11, roulette40, roulette42, roulette44, roulette52, roulette62, "瘍鎢蟯ㄡㄟ", 0},
{22,11, roulette45, 0, 0, roulette52, 0, "瘍鎢ㄟㄟ"},

{ 7, 5, -1, -1, -1, -1, -1, "瘍鎢��", 0},
{ -1, -1, -1, -1, -1, -1, -1, "拸", 0},
};

enum {
	/*
	NPC_WORK_ROUTETOX = CHAR_NPCWORKINT1,
	NPC_WORK_ROUTETOY = CHAR_NPCWORKINT2,
	NPC_WORK_ROUTEPOINT = CHAR_NPCWORKINT3,
*/
	NPC_WORK_NPCAI = CHAR_NPCWORKINT3,
	NPC_WORK_NPCACTION = CHAR_NPCWORKINT3,
	NPC_WORK_MASTERSTONE = CHAR_NPCWORKINT4,
	NPC_WORK_MODE = CHAR_NPCWORKINT5,
	NPC_WORK_SYSTIME = CHAR_NPCWORKINT6,
	NPC_WORK_GAMEFLG = CHAR_NPCWORKINT7,
	NPC_WORK_GAMBLECODE = CHAR_NPCWORKINT8,
	NPC_WORK_MASTERFLG = CHAR_NPCWORKINT9,
	NPC_WORK_ENDPOINT = CHAR_NPCWORKINT10,
};

#define		ROULETTE_ERRTIME	500
#define		ROULETTE_STANDBY1	40
#define		ROULETTE_LOOPTIME	3000

BOOL NPC_Gamble_MasterInit( int meindex )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf1[256];

	if( NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL )	{
		print("\n not found argstr ");
		return FALSE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( argstr, "gamble_code",buf1, sizeof(buf1)) == NULL ) {
		print("\n gamble_code error: not found !!");
		return FALSE;
	}
	CHAR_setWorkInt( meindex, NPC_WORK_GAMBLECODE, atoi( buf1));
	//蚔牁論僇
	CHAR_setWorkInt( meindex, NPC_WORK_GAMEFLG, 0);
	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_GAMBLEMASTER );
	// NPC_WORK_MODE 翋厥�匋棧� 0: 脹渾 1:GAME START 2:GAME END
	CHAR_setWorkInt( meindex, NPC_WORK_MODE, 0);
	CHAR_setWorkInt( meindex, NPC_WORK_MASTERFLG, 30);
	CHAR_setWorkInt( meindex, NPC_WORK_MASTERSTONE, 0);
	CHAR_setWorkInt( meindex, NPC_WORK_NPCAI, 0);
	//扢隅LOOP TIMER
	CHAR_setInt( meindex, CHAR_LOOPINTERVAL, ROULETTE_LOOPTIME);
	//蚚懂暮翹奀潔萸
	CHAR_setWorkInt( meindex, NPC_WORK_SYSTIME, NowTime.tv_sec);
    return TRUE;
}

void NPC_Gamble_MasterTalked( int meindex , int talkerindex , char *szMes , int color )
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];

	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ){
    		return;
	}
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE) return;
	}

	if( NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL )	{
		print("\n not found argstr ");
		return;
	}
	// 0 恁等 11 - 19 寞寀 21 - 29 儅煦
	CHAR_setWorkInt(talkerindex , CHAR_WORKSHOPRELEVANT,0);
	NPC_GambleRoulette_selectWindow( meindex, talkerindex, ROULETTE_START, -1 );
}

void NPC_GambleRoulette_selectWindow(int meindex,int toindex,int num,int select)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf1[256],buf2[256];
	char token[256];
	int buttontype = 0, windowtype = 0, windowno = 0;
	int fd = getfdFromCharaIndex( toindex);
	int pagenum=0, i;
	windowtype = WINDOW_MESSAGETYPE_MESSAGE;

	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg)) == NULL )	{
		print("\n not found npcarg ");
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT,0);
		return;
	}
	switch( num)	{
	case ROULETTE_START:
		i = 1;
		strcpy( token, "\0");
		sprintf( buf2,"%s%d", "gamble_msg0", i);
		while( NPC_Util_GetStrFromStrWithDelim( npcarg, buf2, buf1, sizeof( buf1) ) != NULL )	{
			if( strstr( buf1, "NULL" ) == NULL )	{
				strcat( token , buf1);
				strcat( token, "\n");
			}else	{
				strcat( token, "\n");
			}
			i++;
			sprintf( buf2,"%s%d", "gamble_msg0", i);
		}
	  	windowtype=WINDOW_MESSAGETYPE_SELECT;
	  	windowno = WINDOWTYPE_GAMBLEROULETTE_START; 
		break;
	break;
	case ROULETTE_SELECT:
		//珜 select
		pagenum = CHAR_getWorkInt( toindex , CHAR_WORKSHOPRELEVANT);
		strcpy( token, "\0");
		if( select >= 0 )	{
			sprintf( buf2,"%s%2d", "page_num", select);
			CHAR_setWorkInt( toindex , CHAR_WORKSHOPRELEVANT, select+1);
		}else	{
			sprintf( buf2,"%s%2d", "page_num", pagenum );
			pagenum+=1;
			CHAR_setWorkInt( toindex , CHAR_WORKSHOPRELEVANT, pagenum);
		}
		
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, buf2, token, sizeof( token) ) == NULL )	{
			print("\n error: not found token:%s", token);
			CHAR_setWorkInt( toindex , CHAR_WORKSHOPRELEVANT,0);
			return;
		}

		sprintf( buf2,"%s%2d", "page_num", CHAR_getWorkInt( toindex , CHAR_WORKSHOPRELEVANT));
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, buf2, buf1, sizeof( buf1) ) == NULL )	{
			windowno = WINDOWTYPE_GAMBLEROULETTE_END;
			buttontype = WINDOW_BUTTONTYPE_OK;
		}else	{
			windowno = WINDOWTYPE_GAMBLEROULETTE_SELECT;
			buttontype = WINDOW_BUTTONTYPE_NEXT;
		}
	break;
	case ROULETTE_LOOK:
		{	//珆尨奀硃隙儅煦
			int G_num = CHAR_getInt( toindex, CHAR_GAMBLENUM );

			if( CHAR_getWorkInt( toindex, CHAR_WORKSTAKEFLAG) != FALSE)	{
				for( i=0; i<5; i++)	{
					if( CHAR_getWorkInt( toindex, i+CHAR_WORKSTAKETYPE1) > 0 )
						G_num++;
				}
			}
			if( G_num < 0 ) G_num = 0;
			sprintf( token,"斕腔蚔氈部儅煦峈%d﹝", G_num );
			windowno = WINDOWTYPE_GAMBLEROULETTE_END;
			buttontype = WINDOW_BUTTONTYPE_OK;
		}
		break;
	case ROULETTE_END:
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "page_endmsg", token, sizeof( token) ) == NULL )	{
			print("\n error: not found end_msg");
			CHAR_setWorkInt( toindex , CHAR_WORKSHOPRELEVANT,0);
			return;
		}

		windowno = WINDOWTYPE_GAMBLEROULETTE_END;
		buttontype = WINDOW_BUTTONTYPE_OK;
	break;
	}

	lssproto_WN_send( fd, windowtype, buttontype, windowno, 
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
}

void NPC_Gamble_MasterWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{
	int pagenum=0;
	
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ){
    		return;
	}
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE) return;
	}    

	switch( seqno )	{
	case WINDOWTYPE_GAMBLEROULETTE_START:
		pagenum = atoi( data);
		if( pagenum == 4 )	{
			NPC_GambleRoulette_selectWindow( meindex, talkerindex, ROULETTE_END, -1 );
		}else	{
			if( pagenum == 3 )	{	//ROULETTE_LOOK
				NPC_GambleRoulette_selectWindow( meindex, talkerindex, ROULETTE_LOOK, -1 );
			}else	{
				pagenum = (pagenum*10)+1;
				NPC_GambleRoulette_selectWindow( meindex, talkerindex, ROULETTE_SELECT, pagenum );
			}
		}
	break;
	case WINDOWTYPE_GAMBLEROULETTE_SELECT:
			NPC_GambleRoulette_selectWindow( meindex, talkerindex, ROULETTE_SELECT, -1 );
	break;
	case WINDOWTYPE_GAMBLEROULETTE_END:
			CHAR_setWorkInt(talkerindex , CHAR_WORKSHOPRELEVANT, 0);
	break;
	}
	return;
}

void NPC_Gamble_MasterLoop( int meindex)
{
	int	objmeindex = -1;
	int full_time;
	//int Master_Stone = 0;
	int timeNum = 0;
	//int time_run=0;
	//int act;
	objmeindex = CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX);
	switch( CHAR_getWorkInt( meindex, NPC_WORK_MODE )) {
	  case 0:	// 脹渾
		  full_time = ROULETTE_STANDBY1;
		  timeNum = CHAR_getWorkInt( meindex, NPC_WORK_MASTERFLG );
		  if( ( CHAR_getWorkInt( meindex, NPC_WORK_SYSTIME) + full_time) < NowTime.tv_sec )	{
			  if( timeNum == 30 )	{	//給杅20鏃
				//0 null 1 袧掘 2 變 3 礿
				CHAR_setWorkInt( meindex, NPC_WORK_GAMEFLG, 1);
				showString( meindex, "狟蛁奀潔呁狟ㄡㄟ鏃﹝", 0);
			  }else	if( timeNum == 10 )	{	
				  showString( meindex, "standby_msg", 0);	//嫘畦
				  SetCasinoMap( meindex, 0, 0);	//扢隅華芞祥褫狟蛁
			  }else	if( timeNum <= 6 )	{	//羲宎
				  //0 null 1 袧掘 2 變 3 礿
				  CHAR_setWorkInt( meindex, NPC_WORK_GAMEFLG, 2);
				  CHAR_setWorkInt( meindex, NPC_WORK_MODE, 1);
				  showString( meindex, "start_msg", 0);	//嫘畦
			  }
			  CHAR_setWorkInt( meindex, NPC_WORK_MASTERFLG, timeNum - 2 );
		  }
		break;
	  case 1:	// GAME START
		  if( CHAR_getWorkInt( meindex, NPC_WORK_GAMEFLG) == 3 )	{
			  CHAR_setWorkInt( meindex, NPC_WORK_MODE, 2);
		  }
		break;
	  case 2:	// GAME END
			CHAR_setWorkInt( meindex, NPC_WORK_MODE, 3);
			defPlayerGold( meindex , TRUE);	//數呾怀荇
			CHAR_setWorkInt( meindex, NPC_WORK_ENDPOINT, 0);
			NPC_MAPCLEANGOLD( meindex , CHAR_getInt( meindex, CHAR_FLOOR ));

			LogGamble(
				CHAR_getChar( meindex, CHAR_NAME ), "master",
				"ROULETTE",	CHAR_getInt( meindex, CHAR_FLOOR),
				CHAR_getInt( meindex, CHAR_X ),
				CHAR_getInt( meindex, CHAR_Y ) ,
				CHAR_getWorkInt( meindex, NPC_WORK_MASTERSTONE), 0, 0, 0, 2
			);
		break;
	  case 3:
		  CHAR_setWorkInt( meindex, NPC_WORK_MODE, 0);
		  CHAR_setWorkInt( meindex, NPC_WORK_GAMEFLG, 0);
		  showString( meindex, "end_msg", 0);	//嫘畦
		  SetCasinoMap( meindex, 0, 1);	//扢隅華芞褫狟蛁
		  CHAR_setWorkInt( meindex, NPC_WORK_MASTERFLG, 30 );
		  CHAR_setWorkInt( meindex, NPC_WORK_SYSTIME, NowTime.tv_sec);
		break;
	  default:
		  NPC_GAMBLEMASTER_RESET( meindex);
	    break;
	}
}

void defPlayerGold( int meindex , int flg)
{
	int X,Y;
	int endpoint;
	int End_type = 0;
	int toindex=0, i, master_floor;
	int player_type;
	int gamble_num;
	//float def;
	char token[256], buff[256];
	char buf1[56],buf2[56];
	BOOL GAMBLE_YES = FALSE;
	endpoint = CHAR_getWorkInt( meindex, NPC_WORK_ENDPOINT);
	
	X = (endpoint >> 16 );
	Y = (endpoint & 0xffff);

	for( End_type = 0; End_type < arraysizeof(EndTypedef) ; End_type++ )	{
		if( (X == EndTypedef[ End_type].m_fx) && (Y == EndTypedef[ End_type].m_fy ))	{
			break;
		}
	}

	if( End_type == arraysizeof(EndTypedef) )	{
		print("\n\n **********GAMBLE MASTER ERROR !! **************");
		print("\n NOT FOUND : X=%d Y=%d EndTypedef[I] !!", X, Y);
		return;
	}
/*Log=====================================
	{	//暮翹蚚
		FILE *fp;
		int kp=0,pn=0;;
		fp = fopen("./data/npc/roulette/lookgamble.txt","w+");
		if( fp != NULL )	{
			if( EndTypedef[ End_type].m_nums < 10000 )
				EndTypedef[ End_type].m_nums = EndTypedef[ End_type].m_nums+1;
			for( kp=0; kp<arraysizeof( EndTypedef)-1; kp++ )	{
				if( EndTypedef[ kp].m_nums > 0 && EndTypedef[ kp].m_type > 0 )	{
					fprintf( fp," %s 笢蔣棒杅 [%d] ", EndTypedef[ kp].str_type, EndTypedef[ kp].m_nums );
					if( (pn+1)%2 == 0 )	{
						fprintf( fp," \n");
					}
					pn ++;
				}
			}
			fclose( fp);
		}
	}
//========================================*/

	if( EndTypedef[ End_type].m_type <= 0 )	{
		showString( meindex, "瘍鎢�堧疤享悵�", 0);
	}else	{
		sprintf( token ,"笢蔣瘍鎢 ㄩ %s ", EndTypedef[End_type].str_type );
		showString( meindex, token, 0 );
	}
	if( EndTypedef[ End_type].m_EO == roulette43 )	{
		sprintf( buf1,"等");
	}else	{
		sprintf( buf1,"邧");
	}
	if( EndTypedef[ End_type].m_RG == roulette41 )	{
		sprintf( buf2,"綻");
	}else	{
		sprintf( buf2,"蟯");
	}
	//梑婓部腔��
	toindex = -1;
	master_floor = CHAR_getInt( meindex , CHAR_FLOOR);
	while( toindex < 10000 )	{
		toindex++;
		if( !CHAR_CHECKINDEX( toindex) )
			continue;
		if( CHAR_getInt( toindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
			continue;
		if( master_floor != CHAR_getInt( toindex, CHAR_FLOOR ) )	//岆瘁婓傭部
			continue;
		if( CHAR_getWorkInt( toindex, CHAR_WORKSTAKEFLAG) == FALSE )	//岆瘁衄狟蛁
			continue;
		//岆瘁傭笢
		GAMBLE_YES = FALSE;
		CHAR_setWorkInt( toindex, CHAR_WORKSTAKEFLAG, FALSE);
		for( i=0; i<5; i++)	{
			player_type = CHAR_getWorkInt( toindex, i+CHAR_WORKSTAKETYPE1);
			gamble_num = 0;
			if( player_type > 0 )	{
				if( player_type == EndTypedef[End_type].m_type )	{	//笢賸瘍鎢
					if( EndTypedef[End_type].m_type == roulette45 )	{
						gamble_num += 40;
					}else	{
						gamble_num += 20;
					}
					GAMBLE_YES = TRUE;
					//呾ヴ
					Codef_Gold( meindex, toindex, gamble_num, 0, EndTypedef[End_type].str_type );
				}else if( player_type == EndTypedef[End_type].m_RG )    {	//笢賸綻蟯
					gamble_num += 1;
					GAMBLE_YES = TRUE;
					Codef_Gold( meindex, toindex, gamble_num, 0, buf2 );
				}else if( player_type == EndTypedef[End_type].m_EO )	{	//笢賸等邧
					gamble_num += 1;
					GAMBLE_YES = TRUE;
					Codef_Gold( meindex, toindex, gamble_num, 0, buf1 );
				}else if( player_type == EndTypedef[End_type].m_SI   )	{	//筵蹈					
					gamble_num += 2;
					GAMBLE_YES = TRUE;
					sprintf( token,"%s%d", "筵蹈", EndTypedef[End_type].m_SI-100 );
					Codef_Gold( meindex, toindex, gamble_num, 0, token );
				}else if( player_type == EndTypedef[End_type].m_IN   )	{	//1-10 11-20
					gamble_num += 1;
					GAMBLE_YES = TRUE;
					if( (EndTypedef[End_type].m_IN - 100 ) == 11 )	{
						snprintf( buff, sizeof( buff),"ㄠㄜㄠㄟ");
					}else if( (EndTypedef[End_type].m_IN - 100 ) == 12 )	{
						snprintf( buff, sizeof( buff),"ㄠㄠㄜㄡㄟ");
					}
					sprintf( token,"%s%s", "  峓", buff );
					Codef_Gold( meindex, toindex, gamble_num, 0, token );
				}else	{	//羶笢	諶儅煦
					if( (player_type>>16) == EndTypedef[End_type].m_type || 
						(player_type&0xffff) == EndTypedef[End_type].m_type
						)	{	//笢邧瘍
						gamble_num += 10;
						GAMBLE_YES = TRUE;
						sprintf( token,"%s%s", "邧瘍ㄜ", EndTypedef[End_type].str_type );
						Codef_Gold( meindex, toindex, gamble_num, 0, token );
					}else if( player_type > 0 ) {	//72 73 74 75 羶笢	諶儅煦
						strcpy( token, "\0");
						if( player_type >= roulette41 && player_type <= roulette44 )	{	//綻蟯等邧
							sprintf( token,"%s", "挹蛁 綻蟯等邧 羶笢");
						}else if( player_type == roulette45 )	{	//杅趼00
							sprintf( token,"%s", "挹蛁 杅趼00 羶笢");
						}else if( player_type >= roulette51 && player_type <= roulette53 )	{	//筵蹈
							sprintf( token,"%s", "挹蛁 筵蹈 羶笢");
						}else if( player_type >= roulette61 && player_type <= roulette62 )	{	//  峓
							sprintf( token,"%s", "挹蛁   峓 羶笢");
						}else if( player_type > (1<<16) )	{	//邧瘍
							sprintf( token,"%s", "挹蛁 邧瘍 羶笢");
						}else	{	//杅趼
							sprintf( token,"%s", "挹蛁 杅趼 羶笢");
						}
						gamble_num -= 1;
						Codef_Gold( meindex, toindex, gamble_num, 1, token );
					}
				}
			}
			//ь諾
			CHAR_setWorkInt( toindex, i+CHAR_WORKSTAKETYPE1, 0);
		}
		if( CHAR_getInt( toindex, CHAR_GAMBLENUM) < 0 )	{
			CHAR_setInt( toindex, CHAR_GAMBLENUM, 0 );
		}else if( CHAR_getInt( toindex, CHAR_GAMBLENUM) > 10000 )	{
			CHAR_setInt( toindex, CHAR_GAMBLENUM, 10000 );
		}
		sprintf( token, "斕腔蚔氈部儅煦濛數峈%d煦﹝", CHAR_getInt( toindex, CHAR_GAMBLENUM));
		CHAR_talkToCli( toindex, meindex, token,  CHAR_COLORYELLOW);

		if( GAMBLE_YES == FALSE )	{
			sprintf( token, "斕垀衄挹蛁飲羶笢蔣ㄐ");
			CHAR_talkToCli( toindex, meindex, token,  CHAR_COLORYELLOW);		
            continue;
		}

	}
	return;
}

void Codef_Gold( int meindex, int toindex, int stone,int flg, char *token)
{
	char buf1[256];
	int dnum=-1;
	int Master_gnum=0;
	Master_gnum = CHAR_getWorkInt( meindex, NPC_WORK_MASTERSTONE);
	Master_gnum += stone;
	if( !flg )	{
		sprintf( buf1,"鳩炰斕笢賸ㄩ%sㄛ褫鏽腕 %d 萸儅煦", token, stone);
		CHAR_talkToCli( toindex, meindex, buf1,  CHAR_COLORYELLOW);
#ifdef _FIX_GAMBLENUM
		stone += 1;	//樓奻珨羲宎狟蛁諶腔儅煦
#endif
		CHAR_setInt( toindex, CHAR_GAMBLENUM, CHAR_getInt( toindex, CHAR_GAMBLENUM) + stone);

	}else	{
		dnum = stone;
		if( stone < 0 )	{
			dnum = (dnum*(-1));
		}

		sprintf( buf1,"%sㄛ諶 %d 萸儅煦", token, dnum);
		CHAR_talkToCli( toindex, meindex, buf1,  CHAR_COLORYELLOW);
#ifdef _FIX_GAMBLENUM
#else
		CHAR_setInt( toindex, CHAR_GAMBLENUM, CHAR_getInt( toindex, CHAR_GAMBLENUM) + stone);
#endif
	}
	
	LogGamble(
		CHAR_getChar( toindex, CHAR_NAME ),
		CHAR_getChar( toindex, CHAR_CDKEY ),
		"ROULETTE",
		CHAR_getInt( toindex, CHAR_FLOOR),
		CHAR_getInt( toindex, CHAR_X ),
		CHAR_getInt( toindex, CHAR_Y ),
		CHAR_getInt( toindex, CHAR_GOLD),
		stone,
		0,
		CHAR_getInt( toindex, CHAR_GAMBLENUM ),
		1
	);
	
	//槨翹荅鋸
	if( Master_gnum > 5000000 )	Master_gnum = 5000000;
	if( Master_gnum < 0 ) Master_gnum = 0;
	CHAR_setWorkInt( meindex, NPC_WORK_MASTERSTONE, Master_gnum);
	return;
}
//笭离 渣昫揭燴
void NPC_GAMBLEMASTER_RESET( int meindex)
{
	//渣昫揭燴
	print("\n 渣昫揭燴");
	CHAR_setWorkInt( meindex, NPC_WORK_GAMEFLG, 0);//0 null 1 袧掘 2 變
	CHAR_setWorkInt( meindex, NPC_WORK_MODE, 0);
	showString( meindex, "掛隙磁祥呾ㄐ脹ㄤ煦笘摽笭陔羲宎ㄐㄐ", 1);
	NPC_MAPCLEANGOLD( meindex , CHAR_getInt( meindex, CHAR_FLOOR ));
	//defPlayerGold( meindex , FALSE);
	CHAR_setWorkInt( meindex, NPC_WORK_SYSTIME, NowTime.tv_sec + ROULETTE_ERRTIME);
}

#endif



