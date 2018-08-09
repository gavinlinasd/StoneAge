#define _CHAR_C_

// Spock +1 2000/12/6
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "main.h"
#include "char.h"
#include "saacproto_serv.h"
#include "util.h"
#include "saacproto_lserver.h"

// CoolFish: Family 2001/6/12
#include "acfamily.h"
#include "version.h"

// Arminius 7.17 memory lock
#include "lock.h"
#include "recv.h"

#ifdef _SASQL1
#include "sasql.h"
#endif

static void getCharNameFromString(char*str ,char *out );
static void getCharOptionFromString( char *str , char *out );
static void getCharNameFromString(char*str ,char *out );
static void makeCharFileName( char *idstring ,char *output, int outlen,int num);
// Nuke *1*1
static int makeSaveCharString( char *out , int outlen ,
															char *nm , char *opt , char *info );
static int findBlankCharIndex( char *id  );

static int unlinkCharFile( char *id , int num );
#ifdef _SLEEP_CHAR // 取得非活跃人物档名
static void makeSleepCharFileName( char *id,char *output, int outlen, int num);
static void makeSleepCharPoolItemFileName( char *id,char *output, int outlen);
#endif
#ifdef _CHAR_POOLITEM
static void makeCharPoolItemFileName( char *id,char *output, int outlen);
#endif
#define SPACE '|'
#define SPACESTRING "|"

#define IS_2BYTEWORD( _a_ ) ( (char)(0x80) <= (_a_) && (_a_) <= (char)(0xFF) )


#define USERLOG( id , format, args...)    \
{\
	char body[1000];\
	char fn[1000];\
	if(id[0]){\
	snprintf( body ,sizeof(body), "%s.log.%d" , id ,get_rotate_count() );\
	makeDirFilename( fn , sizeof(fn),logdir, getHash(id), body);\
	LOGBASE( fn , "%u " , (unsigned int ) time(NULL) );\
	LOGBASE( fn , format , ## args );\
	}\
}

#ifdef _ANGEL_SUMMON
struct MissionTable missiontable[MAXMISSIONTABLE];
#endif

/*

	
		仇及白央奶伙匹烂聒今木化中月楮醒反兮氏少  据毛勾井丹［
		
			
				char *c0 : ID
				char *c1 : Passwd  手丹  据允氏匹月井日银歹卅中
				char *c2 : Charname
				char *c3 : opt
				Spock fixed, opt change to process
				char *c3 : process
				char *c4 : 勾井歹氏
				int i0 : lock  
				int i1 : mesgid 
*/

void charLoadCallback( int ti , int auth , char *c0, char* c1 ,
											char *c2 , char *c3, char *c4, int i0 , int i1 )
{
	// Spock deleted 2000/11/2
	//static int process_id = 0;
	// CoolFish: Init charindex 2001/10/16
#ifdef _NewSave
	int charindex = -1;
#else
	int charindex;
#endif
#ifndef _LOCK_SERVER
	char loadbuf[CHARDATASIZE];
	char infobuf[CHARDATASIZE];
	int lock = i0;
#endif
	char *process = c3;
	char *id = c0;
	char *passwd = c1;
	char *charname = c2;
	int mesgid = i1;
	
	char *deadline = c4;
	
	// Spock deleted 2000/11/2
	//process_id++;
	
	if( auth != 0 ){
		char data[100];
		
		snprintf( data,  sizeof( data ), "%d" , auth );
#ifdef _NewSave
		saacproto_ACCharLoad_send( ti , FAILED , data, mesgid, charindex );
#else
		saacproto_ACCharLoad_send( ti , FAILED , data, mesgid );
#endif
		
		
#ifdef _FIX_MESSAGE             // WON ADD 修改WORKER封包内容
		// 同一星系重覆登入送 game end
		if( auth == 204 ){
#ifdef _WAEI_KICK		
			log("\n (%s) AP同一星系重覆登入，踢人!! ", id );
			saacproto_ACKick_recv( ti, id, 7, -1);	//踢人
#ifdef _UNLOCKPOOL
			addUnlockPool( ti, id, "", process, ""); // AP解锁
#else
			addWorkerTask( id , "dummy" , "3" , dummyCallback , ti ,
				id , "dummy" , charname , process ,"", mesgid , 0 );
#endif
#endif
		}else 
			log("\n  (%s) 登入异常!!:%d ", id, auth );
#endif
		
		return;
	}
#ifdef _DELAY_UNLOCK
	if( UNlockM_isBe( id ) ){
		saacproto_ACCharLoad_send( ti , FAILED , "locked" , mesgid, charindex );
		return;
	}else
#endif
    if( isLocked( id ) ){
			//int process;
#ifdef _NewSave
			saacproto_ACCharLoad_send( ti , FAILED , "locked" , mesgid, charindex );
#else
			saacproto_ACCharLoad_send( ti , FAILED , "locked" , mesgid );
#endif
			//DeleteMemLock(getHash(id) & 0xff, id, &process); // 如果AP无锁则AC解锁
			log("\n (%s) AC同一星系重覆登入，踢人!! ", id );
			saacproto_ACKick_recv( ti, id, 7, -1);	//踢人
			
			//		checkGSUCheck( id);
			return;
    }
		
#ifdef _LOCK_SERVER
		if( CHECK_LS_BEUSE() == TRUE )	{
			LServer_SACheckLock2_send( ti, id, mesgid, charname, 1, process);//process
			return;
		}else	{
			saacproto_ACCharLoad_send( ti , FAILED , "locked" , mesgid, charindex );
		}
#else
		
    charindex = getCharIndexByName( id , charname );
#ifdef _NewSave
    //log("\n档案装载序号:%d 账号:%s 名字:%s\n", charindex, id, charname);
#endif

#ifdef _SQL_BACKGROUND
		sasql_online(id,charname);
#endif
		
    if( charindex < 0 ){
			/* 平乓仿互绣箕仄卅中及匹巨仿□卅及分 */
#ifdef _NewSave
			saacproto_ACCharLoad_send( ti , FAILED ,
				"char nonexistent" , mesgid , charindex );
#else
			saacproto_ACCharLoad_send( ti , FAILED ,
				"char nonexistent" , mesgid );
#endif
			return;
    }
		
		
//    log( "loadCharOne: id:[%s] char:[%s]\n", id, charname );
    
    if( loadCharOne( id , charindex ,  loadbuf ,sizeof( loadbuf ))<0){
#ifdef _NewSave
			saacproto_ACCharLoad_send( ti , FAILED ,
				"cannot load ( disk i/o error?)", mesgid, charindex );
#else
			saacproto_ACCharLoad_send( ti , FAILED ,
				"cannot load ( disk i/o error?)", mesgid );
#endif
			return;
    }else{
#ifdef _CHAR_POOLITEM
			checkCharPoolItem( id);
#endif
#ifdef _CHAR_POOLITEM
			checkCharPoolPet( id);
#endif
			
		}
    {//ttom
			char *c_ptr;
			c_ptr=loadbuf;
			while(*c_ptr!='\0'){
				if( IS_2BYTEWORD(*c_ptr) ){
					if(*(c_ptr+1)==' '){
						*(c_ptr+1)=0x41;
					}
					c_ptr++;
					if(*c_ptr=='\0') break;
				}
				c_ptr++;
			}
    }//ttom
		
    if( lock ){
			char result[100];
			char retdata[100];

#ifdef _LOCK_ADD_NAME
			if( lockUser( getGSName(ti) , id , charname, passwd , 1 , result , sizeof( result ) ,
				retdata , sizeof( retdata ) , process , deadline) <0 ){
#else
			// Spock 2000/11/2
			if( lockUser( getGSName(ti) , id , passwd , 1 , result , sizeof( result ) ,
				retdata , sizeof( retdata ) , process , deadline) <0 ){
#endif
				saacproto_ACCharLoad_send( ti , FAILED, "lock FAIL!!" , mesgid , charindex );
				return;
			}else	{
#ifdef _TIMEOUTKICK
				//
#endif
			}
    }
		memset( infobuf , 0 , sizeof( infobuf ));
		getCharInfoFromString( loadbuf , infobuf );
		makeStringFromEscaped( infobuf );
    
		
#ifdef _BAD_PLAYER            // WON ADD 送坏玩家去关
		{
			extern int MAX_BAD_PLAYER;
			extern char BadPlayerList[500][20];			// 定义最大 500 名
			int i;
			
			for( i=0; i<=MAX_BAD_PLAYER; i++){	
				if( strcmp( id, BadPlayerList[i] ) == 0){
					saacproto_ACCharLoad_send( ti ,  BADPLAYER , infobuf , mesgid, charindex );
#ifdef _WAEI_KICK			
					saacproto_ACKick_recv( ti, id, 10, -1);
#endif
					return;
				}
			}
		}
#endif
		
#ifdef _NewSave
    saacproto_ACCharLoad_send( ti ,  SUCCESSFUL , infobuf , mesgid, charindex );
#else
    saacproto_ACCharLoad_send( ti ,  SUCCESSFUL , infobuf , mesgid);
#endif
		
#ifdef _WAEI_KICK
		saacproto_ACKick_recv( ti, id, 10, -1);	//踢其他星系
#endif
		
		
#endif
}

#ifdef _NewSave
int charSave( int ti ,  char *id , 
						 char *charname , char *opt , char *charinfo,
						 int unlock , int mesgid , int charindex)
#else
						 int charSave( int ti ,  char *id , 
						 char *charname , char *opt , char *charinfo,
						 int unlock , int mesgid )
#endif
{
#ifdef _NewSave
#else
	int charindex;
#endif
	char savebuf[CHARDATASIZE];
	int ret = -1;
	
	
	memset(savebuf, 0, sizeof(savebuf));
	
	//andy_log
	if( strstr( charinfo, "DATAEND=") == NULL ){
		FILE *fp;
		if( (fp=fopen( "badpetstring.txt", "a+")) != NULL ){
			fprintf( fp, "%s\n", charinfo);
			fclose( fp);
		}
		log( "err add batpetstring.txt:%s[%s] !\n", id, charname);
	}
	
	if( unlock ){
		char result[100];
		char retdata[100];    
#ifdef _LOCK_ADD_NAME    
		if( (ret = lockUser(  getGSName(ti), id , "" , "0" , 0 , result , sizeof( result ) ,
			retdata , sizeof( retdata ) , "0" , "0")) < 0 ){
#else
		if( (ret = lockUser(  getGSName(ti), id , "0" , 0 , result , sizeof( result ) ,
			retdata , sizeof( retdata ) , "0" , "0")) < 0 ){
#endif
			log( "Unlock:%s FAILED err!!\n", id);
		}
#ifdef _LOCK_SERVER
		if( CHECK_LS_BEUSE() == TRUE )	{
			LServer_SAUNLock_send( ti, id, mesgid, 1);
		}
#endif
	}
	// Nuke *1 add escape
	if( makeSaveCharString( savebuf , sizeof( savebuf ) ,
		charname, opt , charinfo )<0){
		log("\n ACCharSave:too long  ");
		saacproto_ACCharSave_send( ti , FAILED , "too long" , mesgid );
		// Spock fixed 2000/11/1
		return ret;
	}
#ifdef _NewSave
	if (charindex == -1)
		charindex = getCharIndexByName( id , charname );
#else
	// Nuke *1-
	charindex = getCharIndexByName( id , charname );
#endif
	if( charindex < 0 ){
		int blankind = findBlankCharIndex( id );
		if( blankind < 0 ){
			log("\n ACCharSave:char full  ");
			saacproto_ACCharSave_send( ti , FAILED , "char full" ,mesgid);
			return ret;
		} else {
			charindex = blankind;
		}
	}
	
	log( "账号:[%s] 人物:[%s]\n", id, charname );
	if( saveCharOne( id , charindex , savebuf ) < 0 ){
		log("\n ACCharSave:disk I/O error or a bug  ");
		saacproto_ACCharSave_send( ti , FAILED , "disk I/O error or a bug", mesgid );
		return ret;
	}
	else
	{
#ifdef _SASQL
		sasql_save_nm( charindex, id, charname );
		sasql_save_opt( charindex, id, opt );
		sasql_save_int_info( charindex, id, charinfo );
		sasql_save_char_info( charindex, id, charinfo );
#endif
	}

	saacproto_ACCharSave_send( ti ,  SUCCESSFUL , "" , mesgid);
	return ret;
}

void  charListCallback( int ti , int auth , char *c0 , char *c1 ,
											 char *c2 , char *c3 , char *c4 , int i0 , int i1 )
{
#ifndef _LOCK_SERVER
	char listbuf[CHARDATASIZE];
#endif
	char *id = c0;
	int mesgid = i0;
	
	//log(" 档案列表回溯:%s:%d \n", id, auth);
	
	if( auth != 0 ){
		char data[100];
		snprintf( data, sizeof( data ) , "%d" , auth );
		saacproto_ACCharList_send( ti , FAILED , data , mesgid );
		total_ng_charlist++;
		return;
	}
	
if (!sasql_query(c0,c1)){
#ifdef _SQL_REGISTER
		if(!sasql_register(c0,c1))
#endif
			{
	    log("密码错误或没有注册\n");
	    saacproto_ACCharList_send( ti , FAILED , "Password error" , mesgid);
	    return;}}


	// 取消下列 unlock 动作
#ifndef _FIX_MESSAGE             // WON ADD 修改WORKER封包内容
#ifdef _DELAY_UNLOCK
	if( UNlockM_isBe( id ) ){
		saacproto_ACCharList_send( ti , FAILED , "locked" , mesgid );
		return;
	}else
#endif
    if( isLocked( id ) ){
			saacproto_ACCharList_send( ti , FAILED , "locked" , mesgid );
			checkGSUCheck( id );
			total_ng_charlist++;
			return;
    }
#endif // end FIX_MESSAGE
		
#ifdef _LOCK_SERVER
		if( CHECK_LS_BEUSE() == TRUE )	{
			LServer_SACheckLock1_send( ti, id, mesgid, 1);
		}else	{
			saacproto_ACCharList_send( ti , FAILED , "locked" , mesgid );
			total_ng_charlist++;
		}
#else
		
#ifdef _SLEEP_CHAR // 如果无新档, 将旧档移至新档
		{
			char fn_old[256], fn_new[256];
			FILE *fp_old, *fp_new;
			int i;
			
			// 移人物档
			for( i=0; i<MAXCHAR_PER_USER; i++) {
				makeCharFileName( id, fn_new, sizeof(fn_new), i);
				fp_new = fopen( fn_new, "r");
				if( fp_new == NULL ) {
					makeSleepCharFileName( id, fn_old, sizeof(fn_old), i);
					fp_old = fopen( fn_old, "r");
					if( fp_old != NULL ) {
						fclose( fp_old);
						rename( fn_old, fn_new); // 搬移
						//filecopy( fn_old, fn_new); // 复制
						log(" 移档_%s ", fn_new);
					}
				}
				else {
					fclose( fp_new);
				}
			}
			
			// 移仓库档
			makeCharPoolItemFileName( id, fn_new, sizeof(fn_new));
			fp_new = fopen( fn_new, "r");
			if( fp_new == NULL ) {
				makeSleepCharPoolItemFileName( id, fn_old, sizeof(fn_old));
				fp_old = fopen( fn_old, "r");
				if( fp_old != NULL ) {
					fclose( fp_old);
					rename( fn_old, fn_new); // 搬移
					//filecopy( fn_old, fn_new); // 复制
					log(" 移档_%s ", fn_new);
				}
			}
			else {
				fclose( fp_new);
			}
			
		}
#endif
		
#ifdef _DEATH_FAMILY_LOGIN_CHECK//无法读取人物资料
		if( loadCharNameAndOption( id , listbuf,sizeof(listbuf)) == 0 ){
			saacproto_ACCharList_send( ti , FAILED , "locked" , mesgid );
			return;
		}
#endif
		
#ifdef _DEATH_CONTEND //无法读取人物资料
		if( loadCharNameAndOption( id , listbuf,sizeof(listbuf)) == 0 ){
			saacproto_ACCharList_send( ti , FAILED , "locked" , mesgid );
			return;
		}
#else
    loadCharNameAndOption( id , listbuf,sizeof(listbuf));
#endif
    // Arminius
    saacproto_ACCharList_send( ti ,  SUCCESSFUL , listbuf , mesgid);
    total_ok_charlist++;
#endif
}

void charDeleteCallback( int ti , int auth ,
												char *c0 , char *c1 , char *c2 , char *c3 ,
												char *c4 , int i0 , int i1 )
{
	int charindex;
	char *id = c0;
	char *charname = c2;
	int mesgid = i0;
	struct tm *now;
	time_t timenow;
	char logfile[128];
	
	if( auth != 0 ){
		char data[100];
		snprintf( data, sizeof( data ) , "%d" , auth );
		saacproto_ACCharDelete_send( ti , FAILED , data, mesgid );
		return;
	}        
	
	
	charindex = getCharIndexByName( id , charname );
	if( charindex < 0 ){
		saacproto_ACCharDelete_send( ti , FAILED , "nochar",  mesgid );
		return;
	}   
	if( unlinkCharFile( id , charindex ) < 0 ){
		saacproto_ACCharDelete_send( ti , FAILED , "fileI/O" , mesgid);
		return;
	}
#ifdef	__FAMILY
	// CoolFish: Family 2001/6/12
	{
		int index = 0, fmindex = 0, fmcharindex = 0;
		char fmname[32];
		index = ACgetFMInfoFromChar(fmname, &fmindex, charname, id, &fmcharindex);
		if (index != -1)
		{
			if (fmcharindex == 0)
			{
#ifdef _FMVER21
				if (ChangeFMLeader(index, fmname, fmindex) >= 0)
				{
					if (ACDelFM(index, fmname, fmindex) >= 0)
						log("ACDelFM index:%d fmname:%s fmindex:%d charname:%s, charid:%s, charindex:%d\n",
						index, fmname, fmindex, charname, id, fmcharindex);
				}
				log("ChangeFMLeader_3 index:%d fmname:%s fmindex:%d\n", index, fmname, fmindex);
#else
				if (ACDelFM(index, fmname, fmindex) >= 0)
					log("ACDelFM index:%d fmname:%s fmindex:%d charname:%s, charid:%s, charindex:%d\n",
					index, fmname, fmindex, charname, id, fmcharindex);
#endif
			}
			else 
			{
#ifdef _FMVER21
				if (ACMemberLeaveFM(index, fmname, fmindex, charname, 0, 0, fmcharindex) >= 0)
					log("ACMemberLeaveFM index:%d fmname:%s fmindex:%d charname:%s, charid:%s, charindex:%d\n",
					index, fmname, fmindex, charname, id, fmcharindex);
#else
				if (ACMemberLeaveFM(index, fmname, fmindex, charname, 0, fmcharindex) >= 0)
					log("ACMemberLeaveFM index:%d fmname:%s fmindex:%d charname:%s, charid:%s, charindex:%d\n",
					index, fmname, fmindex, charname, id, fmcharindex);
#endif
			}
		}
	}
#endif    
	// Spock 2000/12/6
	time( &timenow );
	now = localtime( &timenow );
	sprintf( logfile , "log/chardel/%04d%02d%02d.log" ,
		now->tm_year + 1900 , now->tm_mon + 1 , now->tm_mday );
	LOGBASE( logfile , "%04d/%02d/%02d %02d:%02d:%02d id:[%s] char:[%s] index:[%d]\n" ,
		now->tm_year + 1900 , now->tm_mon + 1 , now->tm_mday ,
		now->tm_hour , now->tm_min , now->tm_sec ,
		id , charname , charindex );
	// Spock end
	USERLOG( id , "chardelete: successful\n" );
	saacproto_ACCharDelete_send( ti ,  SUCCESSFUL , "ok" , mesgid);
}

void dummyCallback( int ti , int auth ,
									 char *c0 , char *c1 , char *c2 , char *c3 ,
									 char *c4 , int i0 , int i1 )
{
	return;
}

static void getCharNameFromString(char*str ,char *out )
{
	int c;
	
	out[0]='\0';
	for(c=0;;c++){
		if( IS_2BYTEWORD( str[c] ) ){
			out[c] = str[c];
			c ++ ;	// 戚及  侬尺
			out[c] = str[c];
			continue;
		}
		
		out[c] = str[c];
		if( str[c] == '\0' )break;
		
		if( str[c] == SPACE ){
			out[c] = '\0';
			break;
		}
	}
}

static void getCharOptionFromString( char *str , char *out )
{
	int c;
	int outc = 0;
	out[outc] = '\0';
	for(c=0;;c++){
		if( str[c]=='\0')break;
		if( IS_2BYTEWORD( str[c] ) ){
			c++;
			if( str[c] == '\0' )break;	// 公及  侬互允匹卞NULL卅日蔽  
			continue;
		}
		
		if( str[c]== SPACE ){
			c++;
			for(;;c++){
				
				if( str[c] == '\0' ){	// 蔽  分匀凶日 NULL   木化蔽歹月
					out[outc] = '\0';
					break;	
				}
				
				// 仇仇手ㄡ田奶玄  侬及ㄠ田奶玄  卅日手丹域田奶玄戊疋□允月
				//----   For 2Byte Code ----
				if( IS_2BYTEWORD( str[c] ) ){
					out[outc++] = str[c++];		// ㄠ田奶玄  戊疋□
					out[outc++] = str[c];		// ㄡ田奶玄  戊疋□
					if( str[c] == '\0' )break;	// 公及  侬互允匹卞NULL卅日蔽  
					continue;
				}
				
				// 骚橘  侬反ㄠ田奶玄戊疋□
				out[outc] = str[c];			// 引内戊疋□
				if( str[c] == '\0' )break;	// 蔽  分匀凶日蔽歹月
				
				if( str[c] == SPACE ){
					out[outc] = '\0';
					break;
				}
				outc++;
			}
			return;
		}
	}
}

void getCharInfoFromString( char *str , char *out )
{
	int c=0,outc=0;
	int space_counter=0;
	
	out[outc] = '\0';
	for(c=0;;c++){
		if( str[c]=='\0')break;
		if( IS_2BYTEWORD( str[c] ) ){
			c++;
			continue;
		}else
			if( str[c]== SPACE ){
				space_counter++;
			}
			if( space_counter==2 && str[c]== SPACE ){
				c++;
				for(;;c++){
					if( IS_2BYTEWORD( str[c] ) ){
						out[outc++] = str[c++];
						out[outc++] = str[c];
						if( str[c] == '\0' ){
							out[outc] = '\0';
							break;
						}
						continue;
					}
					
					out[outc] = str[c];
					if( str[c] == '\0' )break;
					if( str[c] == SPACE ){
						out[outc] = '\0';
						break;
					}
					outc++;
				}
				return;
			}
	}
}

static void makeCharFileName( char *id,char *output, int outlen, int num)
{
	char body[1024];
	if ( strlen( id) < 1)return;
	
	snprintf(body , sizeof(body ) , "%s.%d.char" , id , num );
	makeDirFilename(output , outlen, chardir , getHash(id) ,body);
}

#ifdef _SLEEP_CHAR // 取得非活跃人物档名
static void makeSleepCharFileName( char *id,char *output, int outlen, int num)
{
	char body[1024];
	if ( strlen( id) < 1)return;
	
	snprintf(body , sizeof(body ) , "%s.%d.char" , id , num );
	makeDirFilename(output , outlen, sleepchardir , getHash(id) ,body);
}
#endif

int loadCharNameAndOption( char *id , char *output , int outlen)
{
	char chbuf[MAXCHAR_PER_USER][CHARDATASIZE];
	char namework[CHARDATASIZE];
	char optionwork[CHARDATASIZE];
	char nm_work[CHARDATASIZE];
	
	int i,count=0;
	int so_far_bytes = 0;
	output[0] = 0;
	
	for(i=0;i<MAXCHAR_PER_USER;i++){
		int l;
		char delim[2];
		
		chbuf[i][0] = 0;
		loadCharOne( id , i , chbuf[i] , sizeof( chbuf[i] ));
		
		getCharNameFromString( chbuf[i] , namework );
		getCharOptionFromString( chbuf[i] , optionwork );
		
		if( chbuf[i][0] == 0 )continue;
		if( strlen( output ) > 0 ){
			delim[0] = SPACE;
			delim[1] = 0;
		} else {
			delim[0] = 0;
		}
		
		snprintf( nm_work , sizeof( nm_work ),
			"%s%s|%s" , delim,namework , optionwork );
		l = strlen( nm_work );
		
		if( ( so_far_bytes + l ) < outlen ){
			strcat( output , nm_work );
			so_far_bytes += l;
			count++;
		}
	}
	
	return count;
}
#ifdef _CHECK_BADFILES
int backupBadcharfile( char *filename, char *cdkey, int num)
{
	char file1[256],file2[256];
	char userpath[256];
	FILE *fp;
	
	log("backupBadcharfile( %s) -%s.%d-%s\n", filename, cdkey, num, userpath);
	sprintf( userpath , "%s.%d.char" , cdkey , num );
	sprintf( file1, "char/0x%x/%s", getHash(cdkey) & 0xff, userpath);
	sprintf( file2, "badchar/%s", userpath);
	{
		char buf[1024];
		int f;
		bzero(buf,sizeof(buf));
		f=open( file1, O_RDONLY);
		if ( f <= 0 ){
			log( "Can't open %s..\n", file1);
			return 1;
		}
		lseek( f, -4, SEEK_END);
		read(f,buf,4);
		close(f);
		if ((buf[2]=='\\')&&(buf[3]=='n')&&
			(buf[0]=='\\')&&(buf[1]=='z')){
			return 1;
		}
	}
	if (mkdir("badchar", 0755) == 0)
		log("create %s\n", "badchar");
	sprintf( file2, "./badchar/%s", filename);
	fp = fopen( file2, "a+");
	if( fp == NULL ){
		
		log("create file:%s error !!\n", file1);
		return -1;
	}
	fprintf( fp, "%s %s\n", cdkey, file1);
	fclose( fp);
	
	log("backup badcharfile:%s..\n", userpath);
	return -1;
}
#endif

int loadCharOne( char *id , int num, char *output , int outlen )
{
	char fn[1024];
	FILE *fp;
	char c_temp,*c_ptr;
	c_ptr=output;
	
	makeCharFileName( id , fn , sizeof(fn),num );    
	fp  = fopen( fn , "r" );
	if( fp == NULL ){
		return -1;
	}
	
	do{
		c_temp = fgetc(fp);
		*c_ptr=c_temp;
		c_ptr++;
	}while(c_temp != EOF);
	*c_ptr='\0';
	
	if( output[0]=='|' && output[1]=='|' ){
		return -1;
	}
	fclose(fp);
#ifdef _CHECK_BADFILES
	if( backupBadcharfile( "badfilelist.txt", id, num) < 0 ){
		return -1;
	}
#endif
	return 0;
}

int saveCharOne( char *id , int num , char *input )
{
	char fn[1024];
	FILE *fp;
	char *strp;
	char *strp1;     // Won 修正 hp 为负的人
	
	makeCharFileName( id , fn , sizeof(fn),num );
	fp= fopen( fn , "w" );
	if( fp == NULL ) {
		log( "save error 01: %d",errno );//Change add errno
		return -1;
	}
	
	// Won 修正 hp 为负的人
	if ((strp=strstr(input,"\\nhp=-"))!=NULL) {
		strp1=strstr(input,"\\nmp="); 
		*(strp+5)='1';
		strcpy((strp+6), strp1);  
	}
	
	// Arminius
	if ((strp=strstr(input,"\\nhp=0\\n"))!=NULL) *(strp+5)='1';
	
	fprintf( fp , "%s" , input );
	fclose(fp);
	
#if 0 //_SLEEP_CHAR // 旧档也存
	{
		char fn1[1024], fn2[1024];
		makeSleepCharFileName( id , fn1 , sizeof(fn1),num );
		fp = fopen( fn1 , "w" );
		if( fp != NULL ) {
			fprintf( fp , "%s" , input );
			fclose(fp);
		}
		
		// 复制新仓库档到旧档
		makeCharPoolItemFileName( id, fn1, sizeof(fn));
		makeSleepCharPoolItemFileName( id, fn2, sizeof(fn));
		filecopy( fn1, fn2);
	}
#endif
#ifdef _SQL_BACKGROUND
	sasql_update(id,fn);
	sasql_online(id,NULL);
#endif
	log( "写入 %s 档案文件:%s\n",id , fn );
	return 0;
	
}

static int makeSaveCharString( char *out , int outlen ,
															char *nm , char *opt , char *info )
{
	char nmwork[CHARDATASIZE];
	char optwork[CHARDATASIZE];
	char infowork[CHARDATASIZE];
	char outwork[CHARDATASIZE];
	char *nmwork_p , *optwork_p , *infowork_p;
	int l;
	
	/* 戊疋□仄化 */
	
	strncpy( nmwork , nm , sizeof( nmwork ));
	nmwork[strlen( nm)]=0;
	
	strncpy( optwork , opt , sizeof( optwork ));
	optwork[strlen(opt)]=0;
	
	strncpy( infowork , info , sizeof( infowork ));
	infowork[strlen(info)]=0;
	
	/* 巨旦弗□皿仄 */
	/*    if ( esc ) {
	nmwork_p = makeEscapeString1( nm , nmwork ,sizeof( nmwork ));
	optwork_p = makeEscapeString1( opt , optwork , sizeof( optwork ));
	infowork_p = makeEscapeString1( info , infowork , sizeof( infowork ));
} else {*/

	nmwork_p = makeEscapeString( nm , nmwork ,sizeof( nmwork ));
	optwork_p = makeEscapeString( opt , optwork , sizeof( optwork ));
	infowork_p = makeEscapeString( info , infowork , sizeof( infowork ));

#ifdef _SASQL
	{
		FILE *f;
		f=fopen("nmkill.txt","w");
		if( f )
		{
			fprintf(f,"%s",nmwork_p);
			fclose( f );
		}
		f=fopen("optkill.txt","w");
		if( f )
		{
			fprintf(f,"%s",optwork_p);
			fclose( f );
		}
		f=fopen("infokill.txt","w");
		if( f )
		{
			fprintf(f,"%s",infowork_p);
			fclose( f );
		}
	}
#endif
	//    }
	/* 勾卅仆月［赞中互巨仿□腹请及凶户卞域荚伐□弁卞中木月［ */
	snprintf( outwork ,sizeof( outwork ) ,
		"%s" SPACESTRING "%s" SPACESTRING "%s" ,
		nmwork_p , optwork_p , infowork_p );
	
	l = strlen( outwork );
	if( l >= (outlen-1)){
		return -1;
	}
	
	/* NULL   侬互    卞丐月井日0毛中木氏午中井氏及分［ */
	memcpy( out , outwork , l + 1);
	
	return 0;
}

int getCharIndexByName( char *id , char *charname )
{
	int i;
	for(i=0;i<MAXCHAR_PER_USER;i++){
		char output[CHARDATASIZE];
		if( loadCharOne( id , i , output , sizeof( output )) < 0 ){
			/* 丐中化中月″ */
			continue;
		} else {
			/* 平乓仿互绣箕仄凶 */
			char cn[CHARDATASIZE];
			getCharNameFromString( output , cn );
			
			/* Escape毛荸仁 */
			if( strcmp( charname , makeStringFromEscaped(cn) ) == 0 ){
				return i;
			}else{
			}
		}
	}
	return -1;
}

/*

  平乓仿及丐中化中月旦夫永玄毛腹绸允月
	
		  卅日丐中化中卅中［0动晓卅日丐五旦夫永玄及index.
		
			
				签卞夫□玉匹五卅井匀凶日公木毛井尹允分仃［
				手匀午切扎氏午匹五月支欠仃升手［
				
*/
static int findBlankCharIndex( char *id  )
{
	int i;
	char output[CHARDATASIZE];
	for(i=0;i<MAXCHAR_PER_USER;i++){
		if( loadCharOne( id , i , output , sizeof( output ) ) < 0 ){
			return i;
		}
	}
	return -1;
}

static int unlinkCharFile( char *id , int num )
{
	char fname[1024];
#ifdef _SLEEP_CHAR //删除人物时旧档也删
	makeSleepCharFileName( id, fname, sizeof(fname),num );
	unlink( fname );
#endif
	makeCharFileName( id, fname, sizeof(fname),num );
	return unlink( fname );
}

#ifdef _LOCK_ADD_NAME
int lockUser(  char *gmsvname , char *id , char *name, char *passwd , int lock , 
						 char *result, int resultlen,
						 char *retdata , int retdatalen , char *process , char *deadline)
#else
int lockUser(  char *gmsvname , char *id , char *passwd , int lock , 
						 char *result, int resultlen,
						 char *retdata , int retdatalen , char *process , char *deadline)
#endif
{
	char fname[1024];
	
	int ret = -1;
	
	
	if(!id[0]){
		snprintf(result , resultlen , FAILED);
		snprintf(retdata , retdatalen , "bad id");
		return -1;
	}
	
	makeDirFilename( fname,sizeof(fname),lockdir , getHash(id), id );
	retdata[0] = 0;
	
	if( lock ){
		if (isLocked(id)) {
			snprintf( result , resultlen, FAILED );
			snprintf( retdata , retdatalen, "already locked" );
			log( "写入内存信息: 用户:%x/%s 已经同意锁定 !!\n", getHash(id), id);
			return -1;
		} else {
#ifdef _LOCK_ADD_NAME
			if( InsertMemLock( getHash(id) & 0xff, id, name, passwd, gmsvname, atoi(process), deadline ) )
#else
			if( InsertMemLock( getHash(id) & 0xff, id, passwd, gmsvname, atoi(process), deadline ) )
#endif
				return 0;
			else
				return -1;
		}
	} else {
		if( !isLocked( id) ) {
			log( "删除内存信息: 用户:%x/%s 没有锁定!!\n", getHash(id), id);
		}
		if( DeleteMemLock( getHash(id) & 0xff, id, &ret) ) {
			snprintf( result , resultlen, SUCCESSFUL );
			snprintf( retdata , retdatalen, "removed" );
			return ret;
		} else {
			snprintf( result , resultlen, FAILED );
			snprintf( retdata , retdatalen, "不能移除锁定" );
			
			log( "不能解锁 %x:%s !\n", getHash(id), id);
			return ret;
		}
	}
}

int isLocked( char *id )
{
	if (!id[0]) return 1;	// invalid id: lock it
	return isMemLocked(getHash(id) & 0xff, id);
}

int checkValidCharName( char *a)
{
	int i;
	for(i=0;;i++){
		if( a[i] == 0 )break;
		if( a[i] == SPACE )return -1;
	}
	return 0;
}

#ifdef _CHAR_POOLITEM
static void makeCharPoolItemFileName( char *id,char *output, int outlen)
{
	char poolitem[256];
	if ( strlen( id) < 1)return;
	snprintf( poolitem , sizeof( poolitem ) , "%s.item" , id);
	makeDirFilename(output , outlen, chardir , getHash(id) ,poolitem);
}

#ifdef _SLEEP_CHAR // 取得非活跃人物仓库档名
static void makeSleepCharPoolItemFileName( char *id,char *output, int outlen)
{
	char poolitem[256];
	if ( strlen( id) < 1)return;
	snprintf( poolitem , sizeof( poolitem ) , "%s.item" , id);
	makeDirFilename(output , outlen, sleepchardir , getHash(id) ,poolitem);
}
#endif

int InsertCharPoolItem( char *id , char *input, int sizes)
{
	char fn[256];
	FILE *fp;
	
	if( sizes <= 0 ) return sizes;
	makeCharPoolItemFileName( id , fn , sizeof(fn));
	
	fp= fopen( fn , "a+" );
	if( fp == NULL ) {
		fp= fopen( fn , "w" );
		if( fp == NULL ) {
			log( "save %s error.\n", fn);
			return -1;
		}
	}
	
	fprintf( fp , "%s" , input );
	fclose(fp);
	
	log( "add %s charpoolitem file:%s\n", id , fn );
	return sizes;
}

int saveCharPoolItem( char *id , char *input, int sizes)
{
	char fn[256];
	FILE *fp;
	
	if( sizes < 0 ) return sizes;
	if( input == NULL ) return -1;
	makeCharPoolItemFileName( id , fn , sizeof(fn));
	
	fp= fopen( fn , "w" );
	if( fp == NULL ) {
		log( "save %s error.\n", fn);
		return -1;
	}
	
	fprintf( fp , "%s" , input );
	fclose(fp);
	
	log( "写入 %s 道具档案文件:%s\n", id , fn );
	return sizes;
}

int loadCharPoolItemOne( char *id , char *output , int outlen)
{
	char fn[256];
	FILE *fp;
  char c_temp,*c_ptr;
	int sizes=0;
	c_ptr=output;
	
	makeCharPoolItemFileName( id , fn , sizeof(fn));
	
	fp  = fopen( fn , "r" );
	if( fp == NULL ){
		if( (fp = fopen( fn , "w" )) == NULL)
			return -1;
		return 0;
	}
	
	do{
		c_temp = fgetc(fp);
		*c_ptr=c_temp;
		c_ptr++;
	   sizes++;
		 if( sizes >= (outlen-1) ) break;
	}while(c_temp != EOF);
	*c_ptr='\0';
	fclose(fp);
	return sizes;
}

int checkCharPoolItem( char *id)
{
	char fn[256];
	FILE *fp;
	
	makeCharPoolItemFileName( id , fn , sizeof(fn));
	fp = fopen( fn , "r" );
	if( fp == NULL ) {
		if( saveCharPoolItem( id , "", 0) < 0 ){
			log("\n Can't Create char:%s poolitem File!!", id);
			return -1;
		}
	}else{
		fclose(fp);
	}
	
	return 1;
}
#endif


#ifdef _CHAR_POOLPET
static void makeCharPoolPetFileName( char *id,char *output, int outlen)
{
	char poolpet[256];
	if ( strlen( id) < 1)return;
	snprintf( poolpet , sizeof( poolpet ) , "%s.pet" , id);
	makeDirFilename(output , outlen, chardir , getHash(id) ,poolpet);
}

#ifdef _SLEEP_CHAR // 取得非活跃人物仓库档名
static void makeSleepCharPoolPetFileName( char *id,char *output, int outlen)
{
	char poolpet[256];
	if ( strlen( id) < 1)return;
	snprintf( poolpet , sizeof( poolpet ) , "%s.pet" , id);
	makeDirFilename(output , outlen, sleepchardir , getHash(id) ,poolpet);
}
#endif

int InsertCharPoolPet( char *id , char *input, int sizes)
{
	char fn[256];
	FILE *fp;
	
	if( sizes <= 0 ) return sizes;
	makeCharPoolPetFileName( id , fn , sizeof(fn));
	
	fp= fopen( fn , "a+" );
	if( fp == NULL ) {
		fp= fopen( fn , "w" );
		if( fp == NULL ) {
			log( "save %s error.\n", fn);
			return -1;
		}
	}
	
	fprintf( fp , "%s" , input );
	fclose(fp);
	
	log( "add %s charpoolpet file:%s\n", id , fn );
	return sizes;
}

int saveCharPoolPet( char *id , char *input, int sizes)
{
	char fn[256];
	FILE *fp;
	
	if( sizes < 0 ) return sizes;
	if( input == NULL ) return -1;
	makeCharPoolPetFileName( id , fn , sizeof(fn));
	
	fp= fopen( fn , "w" );
	if( fp == NULL ) {
		log( "save %s error.\n", fn);
		return -1;
	}
	
	fprintf( fp , "%s" , input );
	fclose(fp);
	
	log( "写入 %s 宠物档案文件:%s\n", id , fn );
	return sizes;
}

int loadCharPoolPetOne( char *id , char *output , int outlen)
{
	char fn[256];
	FILE *fp;
	
	char c_temp,*c_ptr;
	int sizes=0;
	c_ptr=output;
	
	makeCharPoolPetFileName( id , fn , sizeof(fn));
	
	fp  = fopen( fn , "r" );
	if( fp == NULL ){
		if( (fp = fopen( fn , "w" )) == NULL)
			return -1;
		return 0;
	}
	
	do{
		c_temp = fgetc(fp);
		*c_ptr=c_temp;
		c_ptr++;
	   sizes++;
		 if( sizes >= (outlen-1) ) break;
	}while(c_temp != EOF);
	*c_ptr='\0';
	fclose(fp);
	return sizes;
}

int checkCharPoolPet( char *id)
{
	char fn[256];
	FILE *fp;
	
	makeCharPoolPetFileName( id , fn , sizeof(fn));
	fp = fopen( fn , "r" );
	if( fp == NULL ) {
		if( saveCharPoolPet( id , "", 0) < 0 ){
			log("\n Can't Create char:%s poolpet File!!", id);
			return -1;
		}
	}else{
		fclose(fp);
	}
	
	return 1;
}
#endif




int filecopy( char *oldfilename, char *newfilename)
{
	FILE *oldfile;
	FILE *newfile;
	char c_temp;
	
	if( (oldfile = fopen( oldfilename, "r") ) == NULL ) {
		return -1;
	}
	if( (newfile = fopen( newfilename, "w") ) == NULL ) {
		return -1;
	}
	
	while(1) {
		c_temp = fgetc( oldfile);
		if( c_temp == EOF)
			break;
		fputc( c_temp, newfile);
	};
	
	fclose( oldfile);
	fclose( newfile);
	
	return 0;
}
