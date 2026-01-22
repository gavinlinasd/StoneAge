#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "configfile.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "npc_bankman.h"
#include "family.h"

extern	int	familyNumTotal;
extern	char	familyListBuf[MAXFAMILYLIST]; 

char sendbuf[1024];
char buf[1024];
char subbuf[128];
int i, j;

 
struct	{
	int		windowno;
	int		windowtype;
	int		buttontype;
	int		takeitem;
	int		giveitem;
	char	message[4096];
}w;

struct	{
	BOOL	use;
	int		checkhaveitem;
	int		checkhaveitemgotowin;
	int		checkdonthaveitem;
	int		checkdonthaveitemgotowin;
	int		warp;
	int		battle;
	int		gotowin;
}buttonproc[13];		/* ok,cancel, yes,no,prev,next 摯鄹摯窐   */





enum {
	CHAR_WORK_MSGCOLOR	= CHAR_NPCWORKINT1,
};
static void NPC_Bankman_selectWindow( int meindex, int toindex, int num);
static BOOL NPC_Bankman_readData( int meindex, int windowno, BOOL chkflg);
static int NPC_Bankman_restoreButtontype( char *data );

BOOL NPC_BankmanInit( int meindex )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf[1024];
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	if( NPC_Util_GetStrFromStrWithDelim( argstr, "conff", buf, sizeof( buf))== NULL ){
		print( "familyman:羶衄硌隅扢隅腔紫偶 ﹝\n");
		return FALSE;
	}
	/* 疐趵鄹勗优擭溢↓淏禱鏍攝蚗袲媃趙盓堎 */
	if( !NPC_Bankman_readData( meindex, -1, TRUE) ) {
		return FALSE;
	}
	
	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEWINDOWMAN );
	
	return TRUE;
}

void NPC_BankmanTalked( int meindex , int talkerindex , char *szMes ,int color )
{

	NPC_Bankman_selectWindow( meindex, talkerindex, 1 );
	
}

void NPC_BankmanLooked( int meindex , int lookedindex)
{

	print(" Look:me:%d,looked:%d ",meindex,lookedindex);
	NPC_Bankman_selectWindow( meindex, lookedindex,1 );
	
}

static void NPC_Bankman_selectWindow( int meindex, int toindex, int num)
{

	int		fd;
	char	buf[256];
	
	/* 設定相關參數處理 */
	if( CHAR_getInt( toindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
		return;
	}
	/* 角色動畫重置 */
	if( !NPC_Util_charIsInFrontOfChar( toindex, meindex, 1 )) return; 

	if( !NPC_Bankman_readData( meindex, num, FALSE) ) {
		print( "familyman:readdata error\n");
		return;
	}
	
	fd = getfdFromCharaIndex( toindex);
	if( fd != -1 ) {
		lssproto_WN_send( fd, w.windowtype, 
						w.buttontype,
						w.windowno+100,
						CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
						makeEscapeString( w.message, buf, sizeof(buf)));
	}
}

void NPC_BankmanWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)

{
	
	int	button = -1;
	char	buf[2048], buff[256];

	/* 角色動畫重置 */
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;
	
	/* 喫摯坳贖璃迶坳  蠕摯溢↓淏禱  陑  資 */	
	if( !NPC_Bankman_readData( meindex, seqno - 100, FALSE) ) {
		print( "familyman:readdata error\n");
		return;
	}
	/* 棟媃倜尨淏璃禱ぅ迋堎 */
	if( w.windowtype == WINDOW_MESSAGETYPE_SELECT ) {
	//if( w.windowtype == WINDOW_MESSAGETYPE_MESSAGE ) {
	// shan end
		button = atoi( data)+5;
		if( button > 12 ) {
			print( "familyman:invalid button[%d]\n", button);
			return;
		}
	}
	else if( select & WINDOW_BUTTONTYPE_OK) button = 0;
	else if( select & WINDOW_BUTTONTYPE_CANCEL) button = 1;
	else if( select & WINDOW_BUTTONTYPE_YES) button = 2;
	else if( select & WINDOW_BUTTONTYPE_NO) button = 3;
	else if( select & WINDOW_BUTTONTYPE_PREV) button = 4;
	else if( select & WINDOW_BUTTONTYPE_NEXT) button = 5;
	else {
		print( "familyman:invalid button[%d]\n", select);
		return;
	}
	// shan begin
	//if( buttonproc[button].use == TRUE ) {
	//if( select > 0) {
	if( button > 0 ) {
	// shan end
		int	fd;
		int	newwin = -1;
	
		fd = getfdFromCharaIndex( talkerindex);
		
		/* 尨淏璃勗源埱趙窐  禱謂堇渦崹堎 */
		if( newwin == -1 ) {
			newwin = buttonproc[button].gotowin;
		}
		
		// Robin
		// 窅俴
		// shan begin
		if( select == WINDOW_BUTTONTYPE_YES) {
		        newwin = 5;
		}
		// shan end
		if( newwin == 5 )	{
			if( CHAR_getInt( talkerindex, CHAR_FMINDEX ) < 1 && 
			    CHAR_getInt( talkerindex, CHAR_BANKGOLD) < 1 )
			    	lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
			    		WINDOW_BUTTONTYPE_OK, -1, -1,
			    		makeEscapeString("\n蠟腔訧跡祥睫磁ㄐ"
			    				 "\n③珂樓�趧眢憯�", buff, sizeof(buff)));
			else {
				sprintf(buf, "B|G|%d", CHAR_getInt( talkerindex, CHAR_BANKGOLD) );
				lssproto_FM_send( fd, buf );
			}
			return;
		}
		// 模逜梛誧
		if( newwin == 6 )
		{
			if( CHAR_getInt( talkerindex, CHAR_FMINDEX ) > 0 )
				saacproto_ACGetFMData_send( acfd, CHAR_getChar( talkerindex, CHAR_FMNAME),
								CHAR_getInt( talkerindex, CHAR_FMINDEX ),
								CHAR_getWorkInt( talkerindex, CHAR_WORKFMINDEXI ),
								1,
								CONNECT_getFdid(fd)
								);
			else
			    	lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
			    		WINDOW_BUTTONTYPE_OK, -1, -1,
			    		makeEscapeString("\n蠟腔訧跡祥睫磁ㄐ"
			    				 "\n③珂樓�趧眢憯�", buff, sizeof(buff)));
			return;
		}
		// 豖堤
		if( newwin == 7 )
		{
			if( CHAR_getInt( talkerindex, CHAR_FMINDEX ) == -1 ) {
			    	lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
			    		WINDOW_BUTTONTYPE_OK, -1, -1,
			    		makeEscapeString("\n蠟腔訧跡祥睫磁ㄐ"
			    				 "\n③珂樓�趧眢憯�", buff, sizeof(buff)));
				return;
			}
			
			print(" WN2FM ");
			FAMILY_Leave( fd, talkerindex, "E|1");
									
			return;
		}
		
		if( !NPC_Bankman_readData( meindex, newwin, FALSE) ) {
			print( "familyman:readdata error\n");
			return;
		}
		if( fd != -1 ) {
			lssproto_WN_send( fd, w.windowtype, 
							w.buttontype,
							w.windowno+100,
							CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							makeEscapeString( w.message, buf, sizeof(buf)));
		}
		
		
		
	}
}

static BOOL NPC_Bankman_readData( int meindex, int windowno, BOOL chkflg)
{
	
	int		i;
	int		linenum = 0;
	int		endflg = FALSE;
	int		buttonendflg;
	int		winno = -1;
	int		buttonconfmode;
	int		b_mode;
	int		selectnum ;
	int		messagepos;
	BOOL	errflg = FALSE;
	BOOL	readflg = TRUE;
	FILE	*fp;
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	filename[64];
	char	opfile[128];
	char	line[1024];
	char	firstToken[1024];
	char	secondToken[1024];

	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	NPC_Util_GetStrFromStrWithDelim( argstr, "conff", filename, sizeof( filename));
	sprintf( opfile, "%s/", getNpcdir( ) );
	strcat( opfile, filename);
	fp = fopen( opfile, "r");
	if( fp == NULL ) {
		print( "familyman:file open error [%s]\n", opfile);
		return FALSE;
	}
	
	while( readflg == TRUE ) {
		endflg = FALSE;
		buttonendflg = TRUE;
		buttonconfmode = FALSE;
		selectnum = 0;
		messagepos = 0;
		winno = -1;
		b_mode = -1;
		errflg = FALSE;

		/* 參數結構 */
		w.windowno = -1;
		w.windowtype = -1;
		w.buttontype = -1;
		w.takeitem = -1;
		w.giveitem = -1;
		w.message[0] = '\0';
	
		for( i = 0; i < arraysizeof( buttonproc); i ++ ) {
			buttonproc[i].use = FALSE;
			buttonproc[i].checkhaveitem = -1;
			buttonproc[i].checkhaveitemgotowin = -1;
			buttonproc[i].checkdonthaveitem = -1;
			buttonproc[i].checkdonthaveitemgotowin = -1;
			buttonproc[i].warp = -1;
			buttonproc[i].battle = -1;
			buttonproc[i].gotowin = -1;
		}

		while( 1) {
			char    buf[256];
			int		ret;
			if( !fgets( line, sizeof( line), fp)){
				readflg = FALSE;
				break;
			}
			
			linenum ++;
			
			/* 昡隍璃哱毀  鷋 */
			if( line[0] == '#' || line[0] == '\n') continue;
			/* 搊菜噁堎 */
			chomp( line );
			
			/* 實際調用的函數 */
			/* 將tab轉換為空格 */
			replaceString( line, '\t' , ' ' );
			/* 袸  摯筒妐↓筒禱噁堎��*/
			for( i = 0; i < strlen( line); i ++) {
				if( line[i] != ' ' ) {
					break;
				}
				strcpy( buf, &line[i]);
			}
			if( i != 0 ) strcpy( line, buf);

			/* delim "=" ぁ  疐(1)摯哱↓袲璃禱  堎*/
			ret = getStringFromIndexWithDelim( line, "=",  1, firstToken,
											   sizeof( firstToken ) );
			if( ret == FALSE ){
				print( "Find error at %s in line %d. Ignore\n",
					   filename , linenum);
				continue;
			}
			/* delim "=" ぁ2    摯哱↓袲璃禱  堎*/
			ret = getStringFromIndexWithDelim( line, "=", 2, secondToken,
											   sizeof( secondToken ) );
			if( ret == FALSE ){
				print( "Find error at %s in line %d. Ignore\n",
					   filename , linenum);
				continue;
			}
			
			if( strcasecmp( firstToken, "winno") == 0 ) {
				if( winno != -1 ) {
					print( "familyman:眒衄winno�棉寪繞例徲inno\n");
					print( "filename:[%s] line[%d]\n", filename, linenum);
					errflg = TRUE;
					readflg = FALSE;
					break;
				}
				/* 坳贖璃迶坳No禱瞀凎 */
				winno = atoi( secondToken);
				continue;
			}
			/* 坳贖璃迶坳No 誑鋆竘埱趙笢埵笢鄹摯菜毀  鷋埰堎 */
			if( winno == -1 ) {
				print( "familyman:winno 奾帤隅砱ㄛ訧蹋�棍挸頞芋αn");
				print( "filename:[%s] line[%d]\n", filename, linenum);
				readflg = FALSE;
				errflg = FALSE;
				break;
			}
			/* 坳贖璃迶坳No 誑郖窙媃倜鄹毀邳贀禱  資��
			 * 鼠躂雄翻毀  鷋埰堎 */
			if( (chkflg == FALSE && winno == windowno )||
				chkflg == TRUE) 
			{
				if( buttonconfmode == TRUE ) {
					if( strcasecmp( firstToken, "gotowin") == 0 ) {
						buttonproc[b_mode].gotowin = atoi( secondToken);
					}
					else if( strcasecmp( firstToken, "checkhaveitem") == 0 ) {
						buttonproc[b_mode].checkhaveitem = atoi( secondToken);
					}
					else if( strcasecmp( firstToken, "haveitemgotowin") == 0 ) {
						buttonproc[b_mode].checkhaveitemgotowin = atoi( secondToken);
					}
					else if( strcasecmp( firstToken, "checkdonthaveitem") == 0 ) {
						buttonproc[b_mode].checkdonthaveitem = atoi( secondToken);
					}
					else if( strcasecmp( firstToken, "donthaveitemgotowin") == 0 ) {
						buttonproc[b_mode].checkdonthaveitemgotowin = atoi( secondToken);
					}
					else if( strcasecmp( firstToken, "endbutton") == 0 ) {
						if( buttonproc[b_mode].gotowin == - 1 ) {
							if( buttonproc[b_mode].checkhaveitem == -1 && 
								buttonproc[b_mode].checkdonthaveitem == -1)
							{
								errflg = TRUE;
							}
							else {
								/* 汔埱з凝凝倜埱裂煦崹ぁ忒优擭踏躂趙笢躂��     */
								if( !((buttonproc[b_mode].checkhaveitem != -1 && 
									   buttonproc[b_mode].checkhaveitemgotowin != -1)
									 || (buttonproc[b_mode].checkdonthaveitem != -1 && 
									     buttonproc[b_mode].checkdonthaveitemgotowin != -1)))
								{
									errflg = TRUE;
								}
							}
						}
						
						if( errflg == TRUE) {
							print( "familyman: 梑祥善gotowin\n");
							print( "filename:[%s] line[%d]\n", filename, linenum);
							readflg = FALSE;
							errflg = TRUE;
							break;
						}
						buttonproc[b_mode].use = TRUE;
						buttonconfmode = FALSE;
						buttonendflg = TRUE;
					}
				}
				else {
					
					w.windowno = winno;
					/* 坳贖璃迶坳淏騷鏤摯优擭 */
					if( strcasecmp( firstToken, "wintype") == 0 ) {
						w.windowtype = atoi( secondToken);
					}
					/* 尨淏璃淏騷鏤摯优擭 */
					else if( strcasecmp( firstToken, "buttontype") == 0 ) {
						w.buttontype = NPC_Bankman_restoreButtontype( secondToken);
					}
					/* getitem摯优擭 */
					else if( strcasecmp( firstToken, "takeitem") == 0 ) {
						w.takeitem = atoi( secondToken);
					}
					/* giveitem摯优擭 */
					else if( strcasecmp( firstToken, "giveitem") == 0 ) {
						w.giveitem = atoi( secondToken);
					}
					/* message摯优擭 */
					else if( strcasecmp( firstToken, "message") == 0 ) {
						if( messagepos == 0 ) {
							strcpy(  w.message, secondToken);
							messagepos = strlen( w.message);
						}
						else {
							w.message[messagepos]='\n';
							messagepos++;
							strcpy( &w.message[messagepos], secondToken);
							messagepos+=strlen(secondToken);
						}
					}
					/* 尨淏璃禱棟媃倜鄹摯优擭 */
					else if( strcasecmp( firstToken, "okpressed") == 0 ) {
						buttonconfmode = TRUE;
						b_mode = 0;
						buttonendflg = FALSE;
					}
					else if( strcasecmp( firstToken, "cancelpressed") == 0 ) {
						buttonconfmode = TRUE;
						b_mode = 1;
						buttonendflg = FALSE;
					}
					else if( strcasecmp( firstToken, "yespressed") == 0 ) {
						buttonconfmode = TRUE;
						b_mode = 2;
						buttonendflg = FALSE;
					}
					else if( strcasecmp( firstToken, "nopressed") == 0 ) {
						buttonconfmode = TRUE;
						b_mode = 3;
						buttonendflg = FALSE;
					}
					else if( strcasecmp( firstToken, "prevpressed") == 0 ) {
						buttonconfmode = TRUE;
						b_mode = 4;
						buttonendflg = FALSE;
					}
					else if( strcasecmp( firstToken, "nextpressed") == 0 ) {
						buttonconfmode = TRUE;
						b_mode = 5;
						buttonendflg = FALSE;
					}
					else if( strcasecmp( firstToken, "selected") == 0 ) {
						buttonconfmode = TRUE;
						b_mode = 6 + selectnum;
						buttonendflg = FALSE;
						selectnum ++;
					}
					/* 优擭敖渦堇 */
					else if( strcasecmp( firstToken, "endwin") == 0 ) {
						endflg = TRUE;
						if( chkflg == FALSE) {
							readflg = FALSE;
						}
						break;
					}
					else {
						print( "familyman:扢隅岆祥褫夔腔統杅\n");
						print( "filename:[%s] line[%d]\n", filename, linenum);
					}
				}
			}
			else {
				if( strcasecmp( firstToken, "endwin") == 0 ) {
					winno = -1;
				}
			}
		}
		if( buttonendflg == FALSE) {
			print( "familyman: 梑祥善endbutton\n");
			print( "filename:[%s] line[%d]\n", filename, linenum);
			errflg = TRUE;
			break;
		}
		if( winno != -1 ) {
			if( w.windowtype == -1 ) {
				print( "familyman: 梑祥善wintype\n");
				print( "filename:[%s] line[%d]\n", filename, linenum);
				errflg = TRUE;
				break;
			}
			if( w.buttontype == -1 ) {
				print( "familyman: 梑祥善button\n");
				print( "filename:[%s] line[%d]\n", filename, linenum);
				errflg = TRUE;
				break;
			}
			if( strlen( w.message) == 0 ) {
				print( "familyman: 梑祥善message\n");
				print( "filename:[%s] line[%d]\n", filename, linenum);
				errflg = TRUE;
				break;
			}
		}
	}
	fclose( fp);
	
	if( chkflg == FALSE && w.windowno == -1 ) {
		print( "familyman: 梑祥善垀硌隅腔windowno\n");
		print( "filename:[%s] line[%d]\n", filename, linenum);
		return FALSE;
	}
	if( winno != -1 && endflg == FALSE) {
		print( "familyman: 梑祥善endwin\n");
		print( "filename:[%s] line[%d]\n", filename, linenum);
		return FALSE;
	}
	if( errflg == TRUE) return FALSE;
	
	return TRUE;
}
/*
 * buttontype=ぁ炩擭媃倜  棬  禱倳偯勗  儒埰堎��
 *
 */
static int NPC_Bankman_restoreButtontype( char *data )
{
	int		ret = 0;
	int		rc;
	int		i;
	char	buff[1024];
	
	for( i = 1; ; i ++ ) {
		rc = getStringFromIndexWithDelim( data, "|",  i, buff,
											   sizeof( buff ) );
		if( rc == FALSE) break;
		if( strcasecmp( buff, "ok") == 0 ) {
			ret |= WINDOW_BUTTONTYPE_OK;
		}
		else if( strcasecmp( buff, "cancel") == 0 ) {
			ret |= WINDOW_BUTTONTYPE_CANCEL;
		}
		else if( strcasecmp( buff, "yes") == 0 ) {
			ret |= WINDOW_BUTTONTYPE_YES;
		}
		else if( strcasecmp( buff, "no") == 0 ) {
			ret |= WINDOW_BUTTONTYPE_NO;
		}
		else if( strcasecmp( buff, "prev") == 0 ) {
			ret |= WINDOW_BUTTONTYPE_PREV;
		}
		else if( strcasecmp( buff, "next") == 0 ) {
			ret |= WINDOW_BUTTONTYPE_NEXT;
		}
	}
	if( ret == 0 ) {
		ret = atoi( data);
	}
	return ret;
}
