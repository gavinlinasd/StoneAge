#include "version.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "common.h"
#include "util.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "net.h"
#include "char.h"
#include "object.h"
#include "readmap.h"
#include "addressbook.h"
#include "handletime.h"
#include "configfile.h"
#include "event.h"
#include "pet.h"
#include "battle.h"
#include "battle_command.h"
#include "magic.h"
#include "petmail.h"
#include "item_gen.h"
#include "pet_skill.h"
#include "log.h"  //add this because the second had it
#include "map_deal.h" // CoolFish: 2001/4/18
#include "trade.h" // CoolFish: Trade 2001/4/18
#include "family.h" // CoolFish: Family 2001/5/24
#include "item_event.h" // shan: blackmarket

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
#include "profession_skill.h"
#endif
#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道
#include "chatroom.h"
#endif
BOOL checkStringErr( char * );

// shan add
extern struct FM_PKFLOOR fmpkflnum[FAMILY_FMPKFLOOR];


/* -----------------------------------------------------------------------
 * 蓟  仄凶  读及  寞井日平乓仿奶件犯永弁旦毛  月
 * ----------------------------------------------------------------------*/
static int Callfromcli_Util_getTargetCharaindex( int fd, int toindex)
{
	int	to_charaindex = -1;
    int fd_charaindex = CONNECT_getCharaindex( fd );

	/* 愤坌愤褥 */
	if( toindex == 0 ) {
		to_charaindex = fd_charaindex;
	}
	/* 矢永玄 1   5 */
	else if( toindex > 0 && toindex < 6 ) {
		to_charaindex = CHAR_getCharPet( fd_charaindex, toindex-1);
		if( !CHAR_CHECKINDEX( to_charaindex)) {
			to_charaindex = -1;
		}
	}
	/* 醮棉 6   10 */
	else if( toindex > 5 && toindex < 11 ) {
		to_charaindex = CHAR_getPartyIndex( fd_charaindex, toindex - 6);
	}
	return to_charaindex;
}
/*----------------------------------------
 * 弁仿奶失件玄互夫弘奶件允月 匹手丢乒伉卞卺户月分仃卅及匹民尼永弁反卅中
 * 仇木毛裟少午 CLI 卞卅月［
 ----------------------------------------*/
void lssproto_ClientLogin_recv( int fd,char* cdkey, char* passwd )
{
    /*  2褐卞仇木互裟壬木月及反中中    */
    /*  由旦伐□玉  凳毛仄化岳  分匀凶日疯太仇木毛裟少仪［*/
    {//ttom avoid the restore 2001/01/09
     int fd_charaindex;
     Char *chwk;

    // CoolFish: +2 2001/4/18
    fd_charaindex = CONNECT_getCharaindex(fd);
    chwk = CHAR_getCharPointer(fd_charaindex);
		if(CONNECT_isNOTLOGIN(fd)==FALSE){
			print("\n the Client had  Logined fd=%d",fd);
			return;
		}
    }
    //print( "CliLogin cdkey=%s\n" , cdkey );
    /* connect卞戊疋□允月 */
    CONNECT_setCdkey( fd, cdkey );
    CONNECT_setPasswd( fd, passwd );
    CONNECT_setCtype( fd, CLI );
    {//ttom
       unsigned long ip;
       int a,b,c,d;
       int i;
       ip=CONNECT_get_userip(fd);
       a=(ip % 0x100); ip=ip / 0x100;
       b=(ip % 0x100); ip=ip / 0x100;
       c=(ip % 0x100); ip=ip / 0x100;
       d=(ip % 0x100);
       print( "\n登陆账号=%s 来自 %d.%d.%d.%d \n",cdkey,a,b,c,d);

			 for(i=0;i<getMaxLockip();i++)
			 	 if(cmpLockip(i,a,b,c,d,1))return;
    }
    /* 忒蚕 */
    lssproto_ClientLogin_send( fd , "ok" );
}

void lssproto_CreateNewChar_recv( int fd,int dataplacenum,char* charname,
								  int imgno,int faceimgno,
								  int vital,int str,int tgh,int dex,
								  int earth,int water,int fire,int wind,
								  int hometown )
{
    char cdkey[CDKEYLEN];

    if( CONNECT_isCLI( fd ) == FALSE )return;

    if( CONNECT_isNOTLOGIN(fd) == FALSE ){
        lssproto_CreateNewChar_send( fd, FAILED, "Not NOTLOGIN State\n" );
        return;
    }

#ifdef _DEATH_FAMILY_LOGIN_CHECK 	// pk战无法创新人物
		lssproto_CreateNewChar_send( fd, FAILED, "" );
		return;
#endif

#ifdef _DEATH_CONTEND	// pk战无法创新人物
		lssproto_CreateNewChar_send( fd, FAILED, "" );
		return;
#endif

    if( strlen( charname ) == 0 ){
        lssproto_CreateNewChar_send(fd,FAILED, "0 length name\n");
        return;
    }else if( strlen(charname) >= 32 ){
        lssproto_CreateNewChar_send(fd,FAILED, "Too long charname\n");
        return;
    // Nuke start 0711: Avoid naming as WAEI
    }else if (strstr(charname,"华义")  
// WON ADD
			 || strstr(charname,"gm")   || strstr(charname,"GM")  
			 || strstr(charname,"Gm")   || strstr(charname,"gM")  
			 || strstr(charname,"ｇｍ") || strstr(charname,"ＧＭ") 
			 || strstr(charname,"Ｇｍ") || strstr(charname,"ｇＭ") 
#ifdef _UNREG_NEMA
			 || (strstr(charname,getUnregname(0)) && strlen(getUnregname(0))>0)
			 || (strstr(charname,getUnregname(1)) && strlen(getUnregname(1))>0)
			 || (strstr(charname,getUnregname(2)) && strlen(getUnregname(2))>0)
			 || (strstr(charname,getUnregname(3)) && strlen(getUnregname(3))>0)
			 || (strstr(charname,getUnregname(4)) && strlen(getUnregname(4))>0)
#endif
			 || strstr(charname,"神秘人物")
// WON END
		) {
 
	unsigned ip=CONNECT_get_userip(fd);
	int a, b, c, d, ck;
                    
	a=(ip % 0x100); ip=ip / 0x100;
	b=(ip % 0x100); ip=ip / 0x100;
	c=(ip % 0x100); ip=ip / 0x100;
	d=(ip % 0x100);
                                                    
	ck= (
			( (a== 10) && (b==0)   && (c==0) ) ||
			( (a==211) && (b==76) && (c==176) && (d==21) ) ||	// 台北wayi
			( (a==210) && (b==64)  && (c==97)  && ((d>=21)&&(d<=25)) ) ||
			( (a==61)  && (b==222) && (c==142) && (d==66) )
		);
                                                                            
	print(" name_WAEI_IP:%d.%d.%d.%d ck:%d ",a,b,c,d,ck );
                                                                                        
	if( !ck ) {
		lssproto_CreateNewChar_send(fd,FAILED, "Invalid charname\n");
		return;
	}
    }
    {
	// Nuke start 0801,0916: Avoid strange name
        int i,ach;
        for (i=0,ach=0;i<strlen(charname);i++) {
        	if ((unsigned char)charname[i]==0xff) { ach=1; break; } // Force no 0xff
                if (((unsigned char)charname[i]>=0x7f)&&
                    ((unsigned char)charname[i]<=0xa0)) { ach=1; break; } // Force no 0x7f~0xa0
                if ((unsigned char)charname[i]<=0x20) { ach=1; break; } // Force greater than 0x20
                if (ach) {
                	if ((((unsigned char)charname[i]>=0x40)&&((unsigned char)charname[i]<=0x7e))||
                        (((unsigned char)charname[i]>=0xa1)&&((unsigned char)charname[i]<=0xfe))) ach=0;
                } else {
                	if (((unsigned char)charname[i]>=0xa1)&&((unsigned char)charname[i]<=0xfe)) ach=1;
                }
	}
	if (ach) { lssproto_CreateNewChar_send(fd,FAILED, "Error in Chinese\n"); return; }
        // Nuke end
    }
    // Nuke end
                                    

    CONNECT_getCdkey( fd, cdkey, sizeof( cdkey ));
    CHAR_createNewChar( fd, dataplacenum, charname ,imgno, faceimgno,
    					vital, str, tgh, dex,
    					earth, water, fire, wind,
    					hometown , cdkey );
}

void lssproto_CharLogin_recv( int fd,char* charname )
{
    char cdkey[CDKEYLEN], passwd[PASSWDLEN];

    if( CONNECT_isCLI( fd ) == FALSE )return;
    print( "\n尝试登陆: 人物名称=%s\n", charname);
    if( charname[0] == '\0' ){
        lssproto_CharLogin_send( fd, FAILED, "\n人物名称含有不能访问字符\n" );
        return;
    }
    if( CONNECT_isNOTLOGIN(fd) == FALSE ){
        lssproto_CharLogin_send( fd, FAILED, "Already Logged in\n" );
        return;
    }
    CONNECT_setCharname( fd, charname );
    CONNECT_getCdkey( fd, cdkey, sizeof( cdkey ));
    CONNECT_getPasswd( fd, passwd, sizeof(passwd));
    saacproto_ACCharLoad_send( acfd, cdkey,passwd, charname,1,"",
                               CONNECT_getFdid(fd) );
    CONNECT_setState( fd, WHILELOGIN );
}

#ifdef _ITEM_CHECKDROPATLOGOUT
BOOL CheckDropatLogout(int charaindex )
{
	int i;
	for( i=0 ; i<CHAR_MAXITEMHAVE ; i++ ){
		int     itemindex;
		itemindex = CHAR_getItemIndex(charaindex,i);
		if( ITEM_CHECKINDEX(itemindex) == FALSE )continue;
		if( ITEM_getInt(itemindex,ITEM_DROPATLOGOUT ) == TRUE ) {
			return TRUE;
		}
	}
	return FALSE;
}	
#endif

void lssproto_CharLogout_recv( int fd, int flg)
{
    char cdkey[CDKEYLEN] , charname[CHARNAMELEN];

    if( CONNECT_isCLI( fd ) == FALSE )return;
    if( CONNECT_isLOGIN(fd) == FALSE ){
        lssproto_CharLogout_send( fd, FAILED, "Not Logged in\n" );
        return;
    }

    {
        int charaindex=CONNECT_getCharaindex(fd);
        int fl,x,y;
        // CoolFish: 2001/10/18
        if (!CHAR_CHECKINDEX(charaindex))	return;
#ifdef _MUSEUM
		if( (CHAR_getInt( charaindex, CHAR_LASTTALKELDER) >= 0) || getMuseum() )
#else
		if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER) >= 0 )
#endif
		{
#ifdef _MUSEUM
			if( getMuseum() ) {
				fl = 9000; x = 40; y = 40;
			}
			else {
				CHAR_getElderPosition( CHAR_getInt( charaindex, CHAR_LASTTALKELDER),	&fl, &x, &y );
			}
#else
			CHAR_getElderPosition( CHAR_getInt( charaindex, CHAR_LASTTALKELDER),	&fl, &x, &y );
#endif

#ifdef _CHAR_NEWLOGOUT
			if( flg == 1 ){//回纪录点
				if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE){
					CHAR_talkToCli( charaindex, -1, "战斗中无法回纪录点！", CHAR_COLORYELLOW);
					return;
				}
#ifdef _ITEM_CHECKWARES
				if( CHAR_CheckInItemForWares( charaindex, 0) == FALSE ){
					CHAR_talkToCli( charaindex, -1, "携带货物无法使用。", CHAR_COLORYELLOW);
					return;
				}
#endif
#ifdef _BAD_PLAYER             // WON ADD 送坏玩家去关
				if( (CHAR_getInt(charaindex,CHAR_FLOOR)==117)||(CHAR_getInt(charaindex,CHAR_FLOOR)==887) ){
					CHAR_talkToCli( charaindex, -1, "此处无法使用。", CHAR_COLORYELLOW);
					return;
				}
#endif
				if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) != CHAR_PARTY_NONE ){
					CHAR_talkToCli( charaindex, -1, "团队中无法回纪录点！", CHAR_COLORYELLOW);
					return;
				}
#ifdef _ITEM_CHECKDROPATLOGOUT
				if( CheckDropatLogout( charaindex ) ){
					CHAR_talkToCli( charaindex, -1, "携带的物品使你无法回纪录点！", CHAR_COLORYELLOW);
					return;
				}
#endif
#ifdef _DEATH_CONTEND
/*
				if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
					if( CHAR_getInt( charaindex, CHAR_FLOOR) != 8250 &&
						CHAR_getInt( charaindex, CHAR_PKLISTLEADER) == 1 ){
						//andy_log
						print("PlayerLogout_Exit()\n");
						NPC_PKLIST_PlayerLogout_Exit( charaindex );
						CHAR_warpToSpecificPoint( charaindex, 8250, 17, 17 );
					}
				}
*/
#else
				if( CHAR_getInt( charaindex,CHAR_FLOOR ) != 117 &&
					CHAR_getInt( charaindex,CHAR_FLOOR ) != 887 
#ifdef _ADD_DUNGEON            //追加地牢
                    && CHAR_getInt( charaindex,CHAR_FLOOR ) != 8513
#endif
					){
					CHAR_warpToSpecificPoint( charaindex, fl, x, y );

				}
#endif
				return;
			}
#else
	        if( CHAR_getInt(charaindex,CHAR_FLOOR ) == 117){
	           CHAR_setInt(charaindex,CHAR_X,225);
	           CHAR_setInt(charaindex,CHAR_Y,13);
	        }else{
			   CHAR_setInt(charaindex,CHAR_FLOOR,fl);
			   CHAR_setInt(charaindex,CHAR_X,x);
			   CHAR_setInt(charaindex,CHAR_Y,y);
			}
#endif
		}

		// Robin add
		//CHAR_setInt( charaindex, CHAR_LASTLEAVETIME, (int)time(NULL));
    }

    CHAR_logout(fd,TRUE);
    CONNECT_setState( fd, WHILELOGOUTSAVE );
    CONNECT_setCharaindex( fd, -1 );
    CONNECT_getCdkey( fd, cdkey, sizeof(cdkey ));
    CONNECT_getCharname( fd, charname, sizeof(charname));
    print( "Logout cdkey:%s charname=%s\n" , cdkey, charname );
}

void lssproto_CharDelete_recv( int fd , char* charname)
{
    char cdkey[CDKEYLEN],passwd[PASSWDLEN];
    int fdid;

    if( CONNECT_isCLI( fd ) == FALSE )return;
    if( CONNECT_isNOTLOGIN( fd ) == FALSE ){
        lssproto_CharDelete_send( fd, FAILED, "Already Logged in\n" );
        return;
    }
    CONNECT_getCdkey( fd, cdkey, sizeof(cdkey));
    CONNECT_getPasswd( fd, passwd, sizeof(passwd));
    fdid = CONNECT_getFdid(fd);
    saacproto_ACCharDelete_send( acfd, cdkey,passwd, charname , "" ,fdid );

#ifndef _DEATH_CONTEND
	{
		char buff[512];
		char escapebuf[1024];
		snprintf( buff, sizeof(buff), "%s_%s", cdkey, charname);
		makeEscapeString( buff, escapebuf, sizeof(escapebuf));
		saacproto_DBDeleteEntryInt_send(acfd, DB_DUELPOINT, escapebuf, fdid, 0 );
		saacproto_DBDeleteEntryString_send(	acfd, DB_ADDRESSBOOK, escapebuf, fdid, 0 );
	}
	saacproto_Broadcast_send( acfd, cdkey, charname, "chardelete", 0);
#endif

    CONNECT_setState( fd, WHILECHARDELETE );
}

void lssproto_CharList_recv( int fd )
{
    char cdkey[CDKEYLEN], passwd[PASSWDLEN];
    int fdid=-1;

    if( CONNECT_isCLI( fd ) == FALSE )return;

    if( CONNECT_isNOTLOGIN( fd ) == FALSE ){
        lssproto_CharList_send( fd, FAILED, "Already Logged in\n" );
        return;
    }

    CONNECT_getCdkey( fd, cdkey, sizeof(cdkey));
    CONNECT_getPasswd( fd, passwd, sizeof(passwd));
    fdid = CONNECT_getFdid( fd );
	{
		int i;
		int playernum = CHAR_getPlayerMaxNum();
		for( i=0; i<playernum; i++){
			if( !CHAR_CHECKINDEX( i) )continue;
			if( !strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey) ){
				lssproto_CharList_send( fd, FAILED, "-1" );
				CONNECT_setState( fd, NOTLOGIN );
				return;
			}
		}
	}
//#ifdef _PKSEVER_VER
//	saacproto_ACCharList_send(acfd, cdkey, passwd, fdid, star);
//#else
    saacproto_ACCharList_send(acfd, cdkey, passwd, fdid );
//#endif
    CONNECT_setState( fd, WHILEDOWNLOADCHARLIST );
}

void lssproto_Echo_recv( int fd,char* arg0 )
{
	lssproto_Echo_send( fd , arg0 );
}

#define CHECKFD	if( CONNECT_isCLI( fd ) == FALSE )return;	if( CONNECT_isLOGIN(fd) == FALSE )return;
#define CHECKFDANDTIME	if( CONNECT_isCLI(fd) == FALSE )return;	if( CONNECT_isLOGIN(fd) == FALSE )return;

void lssproto_W_recv( int fd,int x,int y,char* direction )
{
        //ttom +3
        int fd_charaindex, ix, iy;
        fd_charaindex = CONNECT_getCharaindex( fd );

        ix=CHAR_getInt(fd_charaindex, CHAR_X);
        iy=CHAR_getInt(fd_charaindex, CHAR_Y);
                                      
        // CoolFish: Prevent Trade Cheat 2001/4/18
        if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
        	return;
	// nuke 0407
	if (checkNu(fd)<0) {
           // Robin 0521	
           print(" NU-Err ");
           CHAR_talkToCli(fd_charaindex, -1, "讯号错误。", CHAR_COLORYELLOW);
           CONNECT_setCloseRequest( fd , 1 );
	   return;		
	}
        //ttom debug
        if((x==0)&&(y==0)){
           //CHAR_talkToCli(fd_charaindex, -1, "因座标错误而断线。", CHAR_COLORYELLOW);
           // Roibn 03/14
           return;
        }
        //ttom avoid the warp at will 11/6
        {
          int i_diff_x,i_diff_y;
          i_diff_x=abs(ix-x);
          i_diff_y=abs(iy-y);
          // Robin 03/14
          if( (i_diff_x>1)||(i_diff_y>1) ){          
               // Robin 0518
               //CHAR_talkToCli(fd_charaindex, -1, "因走路座标错误而断线。", CHAR_COLORYELLOW);
           
               //return;
               x = ix;
               y = iy;
          }
        }
        if(!(MAP_walkAble(fd_charaindex,CHAR_getInt(fd_charaindex, CHAR_FLOOR),x,y))){
           // Robin 03/14
           x = ix;
           y = iy;
        }else{
        }
	CHAR_walk_init( fd, x, y, direction, TRUE);
}
/*------------------------------------------------------------
 * 汹仁
 ------------------------------------------------------------*/
void lssproto_W2_recv( int fd,int x,int y,char* direction )
{
     //ttom +3
     int fd_charaindex, ix, iy, i_fl;
     //Char *chwk;// CoolFish: Rem 2001/4/18
     fd_charaindex = CONNECT_getCharaindex( fd );

     ix=CHAR_getInt(fd_charaindex, CHAR_X);
     iy=CHAR_getInt(fd_charaindex, CHAR_Y);
     i_fl=CHAR_getInt(fd_charaindex, CHAR_FLOOR);
      
     // CoolFish: Prevent Trade Cheat 2001/4/18
     if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
     		return;
                
     //ttom avoid the warp at will 11/6
     {
         int i_diff_x,i_diff_y;
         //ix=CHAR_getInt(fd_charaindex, CHAR_X);
         //iy=CHAR_getInt(fd_charaindex, CHAR_Y);
         //i_fl=CHAR_getInt(fd_charaindex, CHAR_FLOOR);
         i_diff_x=abs(ix-x);
         i_diff_y=abs(iy-y);
         if( (i_diff_x>1)||(i_diff_y>1) ){//2
            //print("\n<www>Warp Error!!!!!!!!!");
            //print("\n<www>the origion->fd=%d,x=%d,y=%d",fd,ix,iy);
            //print("\n<www>the modify-->fd=%d,X=%d,Y=%d,dir=%s",fd,x,y,direction);
            x=ix;
            y=iy;
            // Robin 03/14
            //return;
          }
          //if((i_fl==117)&&(ix==225)&&(iy==13)) goto END_w;
     }//ttom
     if(!(MAP_walkAble(fd_charaindex,CHAR_getInt(fd_charaindex, CHAR_FLOOR),x,y))){
          print("\n<wwww> the map is invaild(f:%d,x:%d,y:%d)",CHAR_getInt(fd_charaindex, CHAR_FLOOR),x,y);
          x = ix;
          y = iy;
     }     
//END_w:            
         CHAR_walk_init( fd, x, y, direction, FALSE);
}

void lssproto_SKD_recv( int fd,int dir, int index)
{
    CHECKFDANDTIME;
}

void lssproto_ID_recv( int fd,int x,int y,int haveitemindex,int toindex )
{
    int		to_charaindex;
    int fd_charaindex;

    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );

    // CoolFish: Prevent Trade Cheat 2001/4/18
    if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    	return;
                
	/* 爵  凛反轮仁   仿弘匹仇木卞娄匀井井月第  岭丐曰  */
	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
	//ttom avoid the warp at will 12/5
	{
	    int ix,iy;
	    ix=CHAR_getInt(fd_charaindex, CHAR_X);
	    iy=CHAR_getInt(fd_charaindex, CHAR_Y);
	    x=ix;
	    y=iy;
	}
	CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
    to_charaindex = Callfromcli_Util_getTargetCharaindex( fd, toindex);
    CHAR_ItemUse( fd_charaindex, to_charaindex, haveitemindex );
}


/*------------------------------------------------------------
 * 惫寞毛蓟少
 ------------------------------------------------------------*/
void lssproto_ST_recv( int fd,int titleindex )
{
    CHECKFDANDTIME;
    CHAR_selectTitle( CONNECT_getCharaindex( fd) , titleindex );
}
/*------------------------------------------------------------
 * 惫寞毛绰轮允月
 ------------------------------------------------------------*/
void lssproto_DT_recv( int fd,int titleindex )
{
    CHECKFDANDTIME;
    CHAR_deleteTitle( CONNECT_getCharaindex(fd) , titleindex );
}


/*------------------------------------------------------------
 * 愤裘惫寞毛    允月
 ------------------------------------------------------------*/
void lssproto_FT_recv( int fd,char* data )
{
    CHECKFDANDTIME;
    
    // Robin 04/23 debug
    if( strlen(data) > 12 ) return;
    
    if( checkStringErr(data) )	return;
    
    CHAR_inputOwnTitle( CONNECT_getCharaindex(fd) , data);
}

/*------------------------------------------------------------
 * 失奶  丞毛胶丹
 ------------------------------------------------------------*/
void lssproto_PI_recv( int fd,int x, int y, int dir )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    {//ttom avoid warp at will
       int ix,iy;
       ix=CHAR_getInt(fd_charaindex, CHAR_X);
       iy=CHAR_getInt(fd_charaindex, CHAR_Y);
       if( (ix!=x)||(iy!=y)){
           //print("\n<PI>--Error!!!!");
           //print("\n<PI>origion x=%d,y=%d",ix,iy);
           //print("\n<PI>modify  X=%d,Y=%d",x,y);
           x=ix;
           y=iy;
       }
    }//ttom end
    
    CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
    CHAR_PickUpItem( fd_charaindex, dir);
}

void lssproto_DI_recv( int fd,int x, int y, int itemindex )
{
    int charaindex;
    CHECKFDANDTIME;
    charaindex = CONNECT_getCharaindex( fd );

    if( CHAR_getWorkInt(charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE) return;
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return;

    CHAR_setMyPosition( charaindex ,
		CHAR_getInt( charaindex, CHAR_X), CHAR_getInt( charaindex, CHAR_Y), TRUE);

    CHAR_DropItem( charaindex, itemindex );
}

void lssproto_DP_recv( int fd,int x, int y, int petindex )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
	    	return;
    {
      int ix,iy;
      ix=CHAR_getInt(fd_charaindex, CHAR_X);
      iy=CHAR_getInt(fd_charaindex, CHAR_Y);
      x=ix;
      y=iy;
    }
    CHAR_setMyPosition( fd_charaindex , x,y,TRUE);
	if( CHAR_getWorkInt( fd_charaindex , CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
	PET_dropPet( fd_charaindex, petindex);
}

/*------------------------------------------------------------
 * 嗯毛  仁
 ------------------------------------------------------------*/
void lssproto_DG_recv( int fd,int x, int y, int amount )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    //ttom avoid the warp at will 12/15
    {
      int ix,iy;
      ix=CHAR_getInt(fd_charaindex, CHAR_X);
      iy=CHAR_getInt(fd_charaindex, CHAR_Y);
      x=ix;
      y=iy;
   }
   CHAR_setMyPosition( fd_charaindex, x,y,TRUE);

	/* 爵    反轮仁   仿弘匹仇木卞娄匀井井月第  岭丐曰  */
	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
		
	// CoolFish: Prevent Trade Cheat 2001/4/18
	if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
		return;
 

    CHAR_DropMoney( fd_charaindex, amount );
}

/*------------------------------------------------------------
 * 失奶  丞毛啖  允月［隶  手仇木匹
 ------------------------------------------------------------*/
void lssproto_MI_recv( int fd,int fromindex,int toindex )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    
    // CoolFish: Prevent Trade Cheat 2001/4/18
    if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
	    	return;
    
	/* 爵    反轮仁   仿弘匹仇木卞娄匀井井月第  岭丐曰  */
	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
    CHAR_moveEquipItem( fd_charaindex, fromindex, toindex );

}

/*------------------------------------------------------------
 * 旦平伙失永皿
 ------------------------------------------------------------*/
void lssproto_SKUP_recv( int fd,int skillid )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex(fd);

	/* 爵    反轮仁   仿弘匹仇木卞娄匀井井月第  岭丐曰  */
	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
    CHAR_SkillUp(fd_charaindex,skillid);
}

/*------------------------------------------------------------
 * 戊生弁扑亦件锹澎卞丢永本□斥毛霜耨
 ------------------------------------------------------------*/
void lssproto_MSG_recv( int fd,int index,char* message, int color )
{
    int fd_charaindex;
    CHECKFD;
    fd_charaindex = CONNECT_getCharaindex( fd);
    ADDRESSBOOK_sendMessage( fd_charaindex, index,message, color );

}

/*------------------------------------------------------------
 * 失玉伊旦皮永弁及    毛母它件夫□玉允月邰菲互  凶
 ------------------------------------------------------------*/
void lssproto_AB_recv( int fd )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    ADDRESSBOOK_sendAddressbookTable( fd_charaindex );
}

/*------------------------------------------------------------
 * 失玉伊旦皮永弁及嫩  毛绰轮允月
 ------------------------------------------------------------*/
void lssproto_DAB_recv( int fd , int index)
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    ADDRESSBOOK_deleteEntry( fd_charaindex ,index);
}

void lssproto_AAB_recv( int fd , int x, int y)
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    {
       int ix,iy;
       ix=CHAR_getInt(fd_charaindex, CHAR_X);
       iy=CHAR_getInt(fd_charaindex, CHAR_Y);
       if( (ix!=x)||(iy!=y)){
           x=ix;
           y=iy;
       }
   }
   CHAR_setMyPosition( fd_charaindex , x,y,TRUE);
    ADDRESSBOOK_addEntry( fd_charaindex );
}

void lssproto_L_recv( int fd, int dir )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    CHAR_Look( fd_charaindex ,dir );
}


/*------------------------------------------------------------
 * 民乓永玄迕丢永本□斥及霜耨
 ------------------------------------------------------------*/
void lssproto_TK_recv( int fd,int x, int y,char* message,int color, int area )
{
    int fd_charaindex,ix,iy;//ttom+2
    int fmindex, channel;
    
    CHECKFD;
    fd_charaindex = CONNECT_getCharaindex( fd );
    fmindex = CHAR_getInt( fd_charaindex, CHAR_FMINDEX );
    channel = CHAR_getWorkInt( fd_charaindex, CHAR_WORKFMCHANNEL );
  {// Robin 0629 silent
    int silentSec, talkCount;
    silentSec = CHAR_getInt(fd_charaindex,CHAR_SILENT);
    if( silentSec > 0 ) {
		int loginTime;
		char buf[256];
		int leftSec;
		loginTime = CHAR_getWorkInt(fd_charaindex, CHAR_WORKLOGINTIME );
		// 防止时间修正回朔後　异常禁言  Robin 20040817
		if( (int)NowTime.tv_sec < loginTime) {
			CHAR_setInt(fd_charaindex, CHAR_SILENT, 0 );
			return;
		}
		if( ((int)NowTime.tv_sec -loginTime) > silentSec ) {
			CHAR_setInt(fd_charaindex, CHAR_SILENT, 0 );
			return;
		}
		silentSec += 10;  //多禁10秒

		leftSec = silentSec - ((int)NowTime.tv_sec - loginTime);
		sprintf(buf, "禁言中!!还有%d秒，再讲多禁10秒钟。", leftSec );
		CHAR_talkToCli(fd_charaindex, -1, buf, color);
		CHAR_setInt(fd_charaindex, CHAR_SILENT, silentSec );
		return;
    }

    talkCount = CHAR_getWorkInt(fd_charaindex, CHAR_WORKTALKCOUNT );
    talkCount ++;
    CHAR_setWorkInt( fd_charaindex, CHAR_WORKTALKCOUNT, talkCount);
    if( talkCount > 8 ) {
    	int lastTalkTime = CHAR_getWorkInt(fd_charaindex, CHAR_WORKTALKTIME );
		if( (int)NowTime.tv_sec - lastTalkTime < 10 ) {
			CHAR_setInt( fd_charaindex,CHAR_SILENT, 60 );
			CHAR_setWorkInt( fd_charaindex, CHAR_WORKLOGINTIME, (int)NowTime.tv_sec );
			CHAR_talkToCli( fd_charaindex, -1, "你太多话了唷，请你的嘴巴先休息个一分钟吧！", color);
			CHAR_setWorkInt(fd_charaindex, CHAR_WORKTALKCOUNT, 0 );
			return;
		}else {
			CHAR_setWorkInt( fd_charaindex, CHAR_WORKTALKTIME, (int)NowTime.tv_sec );
			CHAR_setWorkInt(fd_charaindex, CHAR_WORKTALKCOUNT, 0 );
		}
    }

  }
    ix=CHAR_getInt(fd_charaindex, CHAR_X);
    iy=CHAR_getInt(fd_charaindex, CHAR_Y);
    x=ix;
    y=iy;
    CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
    if(!CONNECT_get_shutup(fd)){ //ttom add the shut up function
        CHAR_Talk( fd,fd_charaindex, message, color, area );
    }
}

void lssproto_M_recv( int fd, int fl, int x1, int y1 , int x2, int y2 )
{
    char*   mapdata;
    RECT    seek={x1,y1,x2-x1,y2-y1},ret;
    CHECKFD;

    mapdata = MAP_getdataFromRECT(fl,&seek,&ret);
    if( mapdata != NULL ){
        lssproto_M_send( fd, fl, ret.x, ret.y,
                         ret.x+ret.width, ret.y+ret.height, mapdata );
	}
}

/*------------------------------------------------------------
 * 平乓仿犯□正 邰菲［
 ------------------------------------------------------------*/
void lssproto_C_recv( int fd, int index )
{
    /*  仇木分仃凛棉及涩烂毛苇卅中仪卞允月  */
    CHECKFD;
    CHAR_sendCSpecifiedObjindex( fd, index);
}

void lssproto_S_recv( int fd, char* category )
{
    char*   string;
    int fd_charaindex;

    fd_charaindex = CONNECT_getCharaindex( fd );
    string = CHAR_makeStatusString( fd_charaindex, category );
    if( string != NULL )
        lssproto_S_send( fd , string );

}

void lssproto_EV_recv( int fd,int event,int seqno,int x,int y, int dir )
{
	int		rc;
	int		fx,fy;
    int fd_charaindex;

	CHECKFD;
	fd_charaindex = CONNECT_getCharaindex( fd );
	{
		int ix,iy;
		ix=CHAR_getInt(fd_charaindex, CHAR_X);
		iy=CHAR_getInt(fd_charaindex, CHAR_Y);
		if( ( ix != x ) || ( iy != y ) ){
			goto CK1;
		}
		goto OK;
	}
CK1:
    {
		OBJECT  object;
		int ix,iy,ifloor,i,j;
		int warp_point_x[9];
		int warp_point_y[9];
		int warp_point=0;
		ix=CHAR_getInt(fd_charaindex, CHAR_X);
		iy=CHAR_getInt(fd_charaindex, CHAR_Y);
		ifloor=CHAR_getInt(fd_charaindex,CHAR_FLOOR);
		for(i=iy-1;i<=iy+1;i++){
			for(j=ix-1;j<=ix+1;j++){
				for( object = MAP_getTopObj(ifloor,j,i) ; object ;object = NEXT_OBJECT(object ) ){
					int o = GET_OBJINDEX(object);
					if( OBJECT_getType(o) == OBJTYPE_CHARA ){
						int     etype;
						int charaindex=OBJECT_getIndex(o);
						if( !CHAR_CHECKINDEX(charaindex) ) continue;
						etype = CHAR_getWorkInt( charaindex, CHAR_WORKEVENTTYPE);
						if( etype != CHAR_EVENT_NONE ) {
							if(etype==CHAR_EVENT_WARP){
								warp_point_x[warp_point]=j;
								warp_point_y[warp_point]=i;
								warp_point++;
							}
						}
					}
#ifdef _MAP_WARPPOINT
					else if( OBJECT_getType(o) == OBJTYPE_WARPPOINT ){
						int	etype = OBJECT_getchartype( o);
						if( etype != CHAR_EVENT_NONE ) {
							warp_point_x[warp_point]=j;
							warp_point_y[warp_point]=i;
							warp_point++;
							break;
						}
					}
#endif
				}
			}
		}

		for(i=0;i<warp_point;i++){
			if((x==warp_point_x[i])&& (y==warp_point_y[i]))
				goto OK;
		}
		x=CHAR_getInt(fd_charaindex, CHAR_X);
		y=CHAR_getInt(fd_charaindex, CHAR_Y);

	}
OK:
	CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
	CHAR_setWorkChar( fd_charaindex , CHAR_WORKWALKARRAY,"");

	if( dir < 0 || dir > 7) {
		fx =  CHAR_getInt(fd_charaindex, CHAR_X);
		fy =  CHAR_getInt(fd_charaindex, CHAR_Y);
	}else {
		CHAR_getCoordinationDir( dir, CHAR_getInt(fd_charaindex, CHAR_X),
			CHAR_getInt(fd_charaindex, CHAR_Y),1,&fx,&fy);
	}
	rc = EVENT_main(fd_charaindex, event,fx,fy);
	lssproto_EV_send( fd, seqno, rc);
}
/*------------------------------------------------------------
 * 巨件市它件玄  戏
 ------------------------------------------------------------*/
void lssproto_EN_recv( int fd , int x,int y )
{
	int		ret = FALSE, err = 0;
    int fd_charaindex;
	CHECKFD;
    fd_charaindex = CONNECT_getCharaindex( fd);

	//print(" EN_recv ");

	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKPARTYMODE)	!= CHAR_PARTY_CLIENT){
           CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
		CHAR_setWorkChar( fd_charaindex, CHAR_WORKWALKARRAY,"");
		err = BATTLE_CreateVsEnemy( fd_charaindex,0, -1);
		if( err != 0 ){
			ret = FALSE;
		}else{
			ret = TRUE;
		}
	}
}
/*------------------------------------------------------------
 * 皿伊奶乩□  衔匹巨件市它件玄  瑁      戏
 ------------------------------------------------------------*/
void lssproto_DU_recv( int fd , int x,int y )
{
    OBJECT  object;
    int fd_charaindex;
	int		ret = FALSE, charaindex = -1, enemyindex;
	int		frontx,fronty;
    int		cnt = 0;
    BOOL	found = FALSE;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    {//ttom avoid warp at will
			int ix,iy;
			ix=CHAR_getInt(fd_charaindex, CHAR_X);
			iy=CHAR_getInt(fd_charaindex, CHAR_Y);
			if( (ix!=x)||(iy!=y)){
				//print("\n<DU>--Error!!!!");
				//print("\n<DU>origion x=%d,y=%d",ix,iy);
				//print("\n<DU>modify  X=%d,Y=%d",x,y);
				x=ix;
				y=iy;
			}
		}
    
                                                                                   
	/* 阂及凛反  骰允月 */
	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKPARTYMODE)
		!= CHAR_PARTY_CLIENT)
	{
		int		i;
		// 愤坌及奶件犯永弁旦
	    charaindex = fd_charaindex;
	    CHAR_setMyPosition( charaindex, x,y,TRUE);
	    /* WALKARRAY毛弁伉失允月 */
		CHAR_setWorkChar( charaindex, CHAR_WORKWALKARRAY,"");


		/* 赓渝祭允月 */
		for( i = 0; i < CONNECT_WINDOWBUFSIZE ; i ++ ) {
            CONNECT_setDuelcharaindex( fd, i, -1 );
	    }

	    /*   及蟆及甄  毛  月 */
	    CHAR_getCoordinationDir( CHAR_getInt( charaindex, CHAR_DIR ) ,
	                             CHAR_getInt( charaindex , CHAR_X ),
	                             CHAR_getInt( charaindex , CHAR_Y ) ,
	                             1 , &frontx , &fronty );

	    /*愤坌及  及蟆及平乓仿毛潸  允月 */
	    for( object = MAP_getTopObj( CHAR_getInt( charaindex, CHAR_FLOOR),
	    							frontx,fronty) ;
	         object ;
	         object = NEXT_OBJECT(object ) )
	    {
	        int toindex;
	        int objindex = GET_OBJINDEX(object);
	        /* 平乓仿弁正□元扎卅中 */
	        if( OBJECT_getType( objindex) != OBJTYPE_CHARA) continue;
	        toindex = OBJECT_getIndex( objindex);
	        /* 皿伊奶乩□元扎卅中 */
	        if( CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ) continue;
			found = TRUE;
	        /* 爵    分匀凶日蛲   */
	        if( CHAR_getWorkInt( toindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE ){
				continue;
			}
			/* 辅爵蛐  卅日蛲   */
			if( !CHAR_getFlg( toindex, CHAR_ISDUEL)) continue;

                        // shan begin
			{
			        int i;
			        for( i=0; i<FAMILY_FMPKFLOOR; i++){
			            if( fmpkflnum[i].fl == CHAR_getInt( charaindex, CHAR_FLOOR) ){
			                if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEFLAG) == -1 ){
			                    lssproto_EN_send( fd, FALSE, 0 );
			                    return;
			                }
			                if(CHAR_getInt( charaindex, CHAR_FMINDEX) == CHAR_getInt( toindex, CHAR_FMINDEX)){
			                    lssproto_EN_send( fd, FALSE, 0 );
			                    return;
			                }
			            }
			        }
			    }
		        // shan end

			// 阂间卅日褪毛裟氏匹仁月
			if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE )
				== CHAR_PARTY_CLIENT )
			{
				int tmpindex = CHAR_getWorkInt( toindex, CHAR_WORKPARTYINDEX1 );
				/* 锹澎互皿伊奶乩□匹卅中仪手丐月 */
				if( CHAR_CHECKINDEX( tmpindex)) {
					if( CHAR_getWorkInt( tmpindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER){
						continue;
					}
				}
			}


            CONNECT_setDuelcharaindex( fd, cnt,toindex );
			cnt++;
			if( cnt == CONNECT_WINDOWBUFSIZE ) break;
		}
		/* 中卅井匀凶 */
		if( cnt == 0 ) {
			goto lssproto_DU_recv_Err;
		}
		/* ㄠ谛分仃分匀凶日穑巨件玄伉□ */
		else if( cnt == 1 ) {
			//   及蟆及平乓仿及奶件犯永弁旦
			enemyindex = CONNECT_getDuelcharaindex(fd,0);
			// 锹澎互褪卅日公及引引巨件市它件玄今六月互
			// 阂间卅日褪毛裟氏匹仁月
			if( CHAR_getWorkInt( enemyindex, CHAR_WORKPARTYMODE )
				== CHAR_PARTY_CLIENT )
			{
				enemyindex = CHAR_getWorkInt( enemyindex, CHAR_WORKPARTYINDEX1 );
				// 卅兮井褪互中卅中
				if( enemyindex < 0 )goto lssproto_DU_recv_Err;
			}
			ret = BATTLE_CreateVsPlayer( charaindex, enemyindex );
			if( ret != 0 ){
				 ret = FALSE;
			}else{
				ret = TRUE;
			}
		}
		/* ㄠ谛动晓中月桦宁反它奴件玉它毛请仄化  中宁歹六月 */
		else if( cnt > 1 ) {
			int		strlength;
			char	msgbuf[1024];
			char	escapebuf[2048];
			strcpy( msgbuf, "1\n要和谁战斗？\n");
			strlength = strlen( msgbuf);
			/* 它奴件玉它及丢永本□斥综岳［
			 * 爵    及平乓仿及域  
			 */
			for( i = 0;
				CONNECT_getDuelcharaindex( fd,i) != -1
				&& i< CONNECT_WINDOWBUFSIZE;
				i ++ )
			{
				char	*a = CHAR_getChar(
                    CONNECT_getDuelcharaindex( fd,i) , CHAR_NAME);
				char	buf[256];
				snprintf( buf, sizeof( buf),"%s [%s]\n", a,
							CHAR_getWorkInt(
                                CONNECT_getDuelcharaindex(fd,i),
                                CHAR_WORKPARTYMODE )
							!= CHAR_PARTY_NONE ? "团体": "单独");
				if( strlength + strlen( buf) > arraysizeof( msgbuf)){
					print( "%s:%d视窗讯息buffer不足。\n",
							__FILE__,__LINE__);
					break;
				}
				strcpy( &msgbuf[strlength], buf);
				strlength += strlen(buf);
			}
			
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT,
							WINDOW_BUTTONTYPE_CANCEL,
							CHAR_WINDOWTYPE_SELECTDUEL,
							-1,
						makeEscapeString( msgbuf, escapebuf, sizeof(escapebuf)));
			ret = TRUE;
		}
	}


// 巨仿□质  
lssproto_DU_recv_Err:;
	if( ret == FALSE ) {
		/* 瑛绊霜耨 */
		lssproto_EN_send( fd, FALSE, 0 );
		if( cnt > 0 ) CHAR_talkToCli( charaindex, -1, "遭遇失败！", CHAR_COLORYELLOW);
		else if( found ) CHAR_talkToCli( charaindex, -1, "无人可以对战。", CHAR_COLORYELLOW);
		else CHAR_talkToCli( charaindex, -1, "那里没有任何人。", CHAR_COLORYELLOW);
	}
}
/*------------------------------------------------------------
 * 巨件市它件玄蔽  
 ------------------------------------------------------------*/
void lssproto_EO_recv( int fd, int dummy )
{
    int fd_charaindex;
   int battle_index;//ttom++    
	CHECKFD;
    fd_charaindex = CONNECT_getCharaindex( fd );
	BattleEncountOut( fd_charaindex );
        // Nuke start 0827 : Battle acceleration
         battle_index=CHAR_getWorkInt(fd_charaindex,CHAR_WORKBATTLEINDEX);
		if( BATTLE_CHECKINDEX( battle_index ) == FALSE ){
			return;
		}
         if(BattleArray[battle_index].type != BATTLE_TYPE_P_vs_P){
            if(CONNECT_get_watchmode(fd)) {
               //print("fd= %d Watching the battle __ lssprot_EO_recv \n",fd);//for debug
               CONNECT_set_watchmode(fd,FALSE);
               return;
            }
            else if (checkBEOTime( fd ) < 0) {
                //CHAR_talkToCli(fd_charaindex, -1, "你加速喔。", CHAR_COLORYELLOW);
            }
         }
         // Nuke end
                                                                                                                                                                        	
}

/*------------------------------------------------------------
 * 巨件市它件玄  蝇邰菲
 ------------------------------------------------------------*/
void lssproto_BU_recv( int fd, int dummy)
{
    int fd_charaindex;
	CHECKFD;
    fd_charaindex = CONNECT_getCharaindex( fd );
	// shan 2001/12/25
	//BATTLE_WatchStop( fd_charaindex );
}

void lssproto_B_recv( int fd, char *command )
{
	int fd_charaindex;
	int battle_index;//ttom++
	CHECKFD;
	fd_charaindex = CONNECT_getCharaindex( fd );
	BattleCommandDispach( fd, command );
	// Nuke +1 0827: Battle acceleration
	battle_index=CHAR_getWorkInt(fd_charaindex,CHAR_WORKBATTLEINDEX);
	if( BATTLE_CHECKINDEX( battle_index ) == FALSE ){
		return;
	}
	if(BattleArray[battle_index].type != BATTLE_TYPE_P_vs_P){
	   if(BattleArray[battle_index].type == BATTLE_TYPE_WATCH) {
	      CONNECT_set_watchmode(fd,TRUE);
	      return;
	   }
	}
	//Nuke end
}

void lssproto_FS_recv( int fd,int flg )
{
    int fd_charaindex;
    CHECKFDANDTIME;

    fd_charaindex = CONNECT_getCharaindex( fd );
	/* 椭瘀反竣卞六内卞公及引引白仿弘凳蕙 */
	CHAR_setFlg( fd_charaindex, CHAR_ISPARTY,
				(flg & CHAR_FS_PARTY )? TRUE:FALSE);
	//CHAR_setFlg( fd_charaindex, CHAR_ISBATTLE,
	//			(flg & CHAR_FS_BATTLE )? TRUE:FALSE);
	CHAR_setFlg( fd_charaindex, CHAR_ISDUEL,
				(flg & CHAR_FS_DUEL )? TRUE:FALSE);
	CHAR_setFlg( fd_charaindex, CHAR_ISPARTYCHAT,
				(flg & CHAR_FS_PARTYCHAT )? TRUE:FALSE);
	CHAR_setFlg( fd_charaindex, CHAR_ISTRADECARD,
				(flg & CHAR_FS_TRADECARD )? TRUE:FALSE);
#ifdef _CHANNEL_MODIFY
	//密语频道开关
	CHAR_setFlg(fd_charaindex,CHAR_ISTELL,(flg & CHAR_FS_TELL )? TRUE:FALSE);
	//家族频道开关
	CHAR_setFlg(fd_charaindex,CHAR_ISFM,(flg & CHAR_FS_FM )? TRUE:FALSE);
	//职业频道开关
	CHAR_setFlg(fd_charaindex,CHAR_ISOCC,(flg & CHAR_FS_OCC )? TRUE:FALSE);
	//聊天室
	CHAR_setFlg(fd_charaindex,CHAR_ISCHAT,(flg & CHAR_FS_CHAT )? TRUE:FALSE);
	//储存对话开关
	CHAR_setFlg(fd_charaindex,CHAR_ISSAVE,(flg & CHAR_FS_SAVE )? TRUE:FALSE);
#endif
#ifdef _AUCPROTOCOL				// (不可开) Syu ADD 拍卖频道开关Protocol
	CHAR_setFlg( fd_charaindex, CHAR_ISAUC,
				(flg & CHAR_FS_AUC )? TRUE:FALSE);
#endif
        // CoolFish: Trade 2001/4/18
        CHAR_setFlg( fd_charaindex, CHAR_ISTRADE,
        	(flg & CHAR_FS_TRADE )? TRUE:FALSE);
        /*
        if (CHAR_getFlg(fd_charaindex, CHAR_ISTRADECARD) == TRUE)
        	CHAR_setFlg(fd_charaindex, CHAR_ISTRADE, FALSE);
        if (CHAR_getFlg(fd_charaindex, CHAR_ISTRADE) == TRUE)
        	CHAR_setFlg(fd_charaindex, CHAR_ISTRADECARD, FALSE);
        */
                                                                                                
	lssproto_FS_send( fd, flg);
}
/*------------------------------------------------------------
 * 醮棉邰菲  戏［
 ------------------------------------------------------------*/
void lssproto_PR_recv( int fd,int x, int y, int request )
{
	int result = FALSE;
    int fd_charaindex;
    CHECKFDANDTIME;

    fd_charaindex = CONNECT_getCharaindex( fd );

#if 1 // 禁止组队区域
	if( request == 1 )
	{
		int nowFloor;
		nowFloor = CHAR_getInt( fd_charaindex, CHAR_FLOOR);
		if(	nowFloor == 31706
			|| nowFloor == 10204
			|| (10601 <= nowFloor && nowFloor <= 10605 )
			|| nowFloor == 10919 || nowFloor == 10920
			|| nowFloor == 20711 || nowFloor == 20712
			|| nowFloor == 1008 || nowFloor == 1021
			|| nowFloor == 3008 || nowFloor == 3021 
			|| ( nowFloor <= 8213 && nowFloor >= 8200 )
			|| ( nowFloor >= 30017 && nowFloor <= 30021 )
#ifdef _TIME_TICKET
			|| check_TimeTicketMap(nowFloor)
#endif
			){
			print("\n 改封包!禁止组队区域!!:%s ", CHAR_getChar( fd_charaindex, CHAR_CDKEY) );
			return;
		}
	}
#endif

    {//ttom avoid warp at will
       int ix,iy;
       ix=CHAR_getInt(fd_charaindex, CHAR_X);
       iy=CHAR_getInt(fd_charaindex, CHAR_Y);
       if( (ix!=x)||(iy!=y)){
           //print("\n<PR>--Error!!!!");
           //print("\n<PR>origion x=%d,y=%d",ix,iy);
           //print("\n<PR>modify  X=%d,Y=%d",x,y);
           x=ix;
           y=iy;
       }
   }
   CHAR_setMyPosition( fd_charaindex, x,y,TRUE);

	if( request == 0 ) {
		/* 轮迩允月 */
		result = CHAR_DischargeParty(fd_charaindex, 0);
	}
	else if( request == 1 ) {
		/*   迩允月 */
		result = CHAR_JoinParty(fd_charaindex);
	}
}
/*------------------------------------------------------------
 * 爵  卞瓒  允月矢永玄毛蓟  仄凶［
 ------------------------------------------------------------*/
void lssproto_KS_recv( int fd,int petarray )
{
	int ret , fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    	
    	if( CHAR_getInt( fd_charaindex, CHAR_RIDEPET) == petarray )
    		lssproto_KS_send( fd, petarray, FALSE);
    	
	ret = PET_SelectBattleEntryPet( fd_charaindex, petarray);
	lssproto_KS_send( fd, petarray, ret);
}

#ifdef _STANDBYPET
void lssproto_SPET_recv( int fd, int standbypet )
{
	int fd_charaindex;
	int i, s_pet =0, cnt =0;

    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );

	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE 
		&& standbypet >= CHAR_getWorkInt( fd_charaindex, CHAR_WORKSTANDBYPET) ) {
		print("\n 改封包!??战斗中用SPET增加待机宠!!:%s ", CHAR_getChar( fd_charaindex, CHAR_CDKEY) );
		return;
	}

    //if( CHAR_getInt( fd_charaindex, CHAR_RIDEPET) == petarray ) {
    //	lssproto_SPET_send( fd, petarray, FALSE);
	//}

	for( i =0; i < CHAR_MAXPETHAVE; i++) {
		if( standbypet & ( 1 << i ) ) {

			//if( CHAR_getInt( fd_charaindex, CHAR_RIDEPET) == i )
			//	continue;

			cnt++;
			if( cnt > 3 ) {
				print("\n 改封包!待机宠超过数量!!:%s ", CHAR_getChar( fd_charaindex, CHAR_CDKEY) );
				//lssproto_SPET_send( fd, s_pet, FALSE);
				break;
			}
			
			s_pet |= ( 1 << i );
		}
	}
	CHAR_setWorkInt( fd_charaindex, CHAR_WORKSTANDBYPET, s_pet);
    	
	lssproto_SPET_send( fd, s_pet, TRUE);
}
#endif

/*------------------------------------------------------------
 * 减变升啼卅升及  蜇六方午熬仃午匀凶
 ------------------------------------------------------------*/
void lssproto_AC_recv( int fd,int x, int y,int actionno )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    {//ttom avoid the warp at will
       Char *ch;
       ch = CHAR_getCharPointer( fd_charaindex);
	   // CoolFish: +1 2001/11/05
	   if (!ch)	return;

       if((ch->data[CHAR_X]!=x)||(ch->data[CHAR_Y]!=y)){
           return;
       }
    }
        CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
	CHAR_sendAction( fd_charaindex, actionno, FALSE);
	return;
}
/*------------------------------------------------------------
 *   芊毛银匀凶［
 ------------------------------------------------------------*/
void lssproto_MU_recv( int fd,int x,int y,int array,int toindex )
{
    int		to_charaindex = -1, fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    {//ttom avoid warp at will
       int ix,iy;
       ix=CHAR_getInt(fd_charaindex, CHAR_X);
       iy=CHAR_getInt(fd_charaindex, CHAR_Y);
       if( (ix!=x)||(iy!=y)){
           //print("\n<MU>--Error!!!!");
           //print("\n<MU>origion x=%d,y=%d",ix,iy);
           //print("\n<MU>modify  X=%d,Y=%d",x,y);
           x=ix;
           y=iy;
       }
   }
                                                                                   
	CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
	/* toindex 毛平乓仿弁正□犯□正及index卞  晶允月 */
	to_charaindex = Callfromcli_Util_getTargetCharaindex( fd, toindex);
	MAGIC_Use( fd_charaindex, array, to_charaindex);
}

void lssproto_JB_recv( int fd,int x,int y )
{
	int charaindex, floor;

    CHECKFDANDTIME;
    charaindex = CONNECT_getCharaindex( fd );
    {
       int ix,iy;
       ix=CHAR_getInt(charaindex, CHAR_X);
       iy=CHAR_getInt(charaindex, CHAR_Y);
       if( (ix!=x)||(iy!=y)){
            x=ix;
            y=iy;
       }
   }
                                                                                   
	CHAR_setMyPosition( charaindex, x,y,TRUE);
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return;
	floor = CHAR_getInt( charaindex, CHAR_FLOOR );
	if( floor == 1007
	|| floor == 2007
	|| floor == 3007
	|| floor == 4007
	|| floor == 130	){
		BATTLE_WatchTry( charaindex );
	}else{
		BATTLE_RescueTry( charaindex );
	}
}

void lssproto_KN_recv( int fd,int havepetindex,char* data )
{
    int fd_charaindex;
    CHECKFD;
    fd_charaindex = CONNECT_getCharaindex( fd );
    
    // Robin 04/26 debug
    if( strlen(data) > 16 )	return;
    
    // CoolFish: Prevent Trade Cheat 2001/4/18
    if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
	    	return;

	if( checkStringErr(data) )	return;
	
	CHAR_inputUserPetName( fd_charaindex, havepetindex, data);
	
}
/*------------------------------------------------------------
 * 它奴件玉它毛蓟  仄凶［
 ------------------------------------------------------------*/
void lssproto_WN_recv( int fd,int x,int y,int seqno,
                       int objindex,int select, char* data )
{
    int fd_charaindex;

    CHECKFDANDTIME;
    
    if( checkStringErr(data) )	return;
#ifdef _NO_WARP	
	{  
		if(seqno!=CONNECT_get_seqno(fd)){
			return;
		}
		if( !( (select)&(CONNECT_get_selectbutton(fd)) ) && select ){
			if( CONNECT_get_seqno(fd)==CHAR_WINDOWTYPE_QUIZ_MAIN ){				
			}else if( (  (CONNECT_get_seqno(fd)==CHAR_WINDOWTYPE_SCHEDULEMAN_START)
				     || (CONNECT_get_seqno(fd)==CHAR_WINDOWTYPE_SCHEDULEMAN_SELECT) )
					 && (select==1) ){
			}else{
				return ;
			}
		}    
    } // shan End    
#endif

    fd_charaindex = CONNECT_getCharaindex( fd );
    // CoolFish: Prevent Trade Cheat 2001/4/18
    if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)	{
		return;
	}
    // Robin
    if( checkStringErr(data) )	return;

#ifdef _ANGEL_SUMMON
	if( seqno == CHAR_WINDOWTYPE_ANGEL_ASK )
	{
		print("\n CHAR_WINDOWTYPE_ANGEL_ASK objindex:%d select:%d data:%s ",
			objindex, select, data );
		
		if(select==WINDOW_BUTTONTYPE_YES ) {
			if( AngelCreate( fd_charaindex) == FALSE ) {
				sendAngelCleanToCli( fd );
			}
		}
		else if(select==WINDOW_BUTTONTYPE_NO ) {
			int mindex;
			char nameinfo[64];
			mindex = checkIfAngel( fd_charaindex);
			print(" ====不接受召唤任务==== ");
			getMissionNameInfo( fd_charaindex, nameinfo);
			saacproto_ACMissionTable_send( acfd, mindex, 3, nameinfo, "");

			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK, -1, -1,
			"真是遗憾。\n少了你的帮助，看来魔族会继续危害大陆的人民了。" );

			//CHAR_talkToCli( fd_charaindex, -1, "天之声：真是可惜，这可是难得的机会呀。", CHAR_COLORYELLOW);
			
			sendAngelCleanToCli( fd );
		}
	}
#endif

#ifdef _CONTRACT
	if( seqno == CHAR_WINDOWTYPE_CONTRACT_ANSWER ) {
		ITEM_contractSign( fd, objindex, select);
	}
#endif

    {//ttom avoid the warp at will
       Char *ch;
       ch = CHAR_getCharPointer( fd_charaindex);
	   // CoolFish: +1 2001/11/05
	   if (!ch)	return;
       if((ch->data[CHAR_X]!=x)||(ch->data[CHAR_Y]!=y)){
           // Robin 04/20 test
           return;
           x=ch->data[CHAR_X];
           y=ch->data[CHAR_Y];
       }
     //ttom avoid WN at will
     if(seqno==CHAR_WINDOWTYPE_NPCENEMY_START){
//      Char *ch;
        OBJECT  object;
        int ix,iy,ifloor,i,j;
        int     whichtype= -1;
        int enemy=0;
        int enemy_index;
        if(!CHECKOBJECT(objindex)){
            goto  FIRST;
        }
        enemy_index=OBJECT_getIndex(objindex);//ttom 11/15/2000
        ix    =ch->data[CHAR_X];
        iy    =ch->data[CHAR_Y];
        ifloor=ch->data[CHAR_FLOOR];
        for(i=iy-1;i<=iy+1;i++){
            for(j=ix-1;j<=ix+1;j++){
             for( object = MAP_getTopObj(ifloor,j,i) ; object ;
                  object = NEXT_OBJECT(object ) ){
                  int objindex = GET_OBJINDEX(object);
                  switch( OBJECT_getType(objindex)  ){
                          case OBJTYPE_CHARA:
                               whichtype =  CHAR_getInt( OBJECT_getIndex( objindex), CHAR_WHICHTYPE);
                               if( whichtype == CHAR_TYPENPCENEMY){
                                   int i_ene_temp;
                                   i_ene_temp=OBJECT_getIndex( objindex);
                                   //print("\n<WN>--enetemp=%d",i_ene_temp);
                                   if(i_ene_temp== enemy_index){
                                       goto START_WN;
                                   }else{
                                       enemy=0;
                                   }
                               }
                               break;
                          case OBJTYPE_ITEM:
                               break;
                          case OBJTYPE_GOLD:
                               break;
                          default:
                           break;
                  }
             }
            }
        }
        if(enemy==0){
FIRST:
		
           lssproto_EN_send( fd, FALSE, 0 );
           CHAR_talkToCli(fd_charaindex, -1, "事件错误。", CHAR_COLORYELLOW);
           goto END_WN;
        }
     }
     }
     //ttom end
START_WN:
        CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
	         if(seqno==CHAR_WINDOWTYPE_WINDOWWARPMAN_MAIN){
		         if(!CONNECT_get_first_warp(fd)){
		             select=1;
	        	 }
		     }
	         CHAR_processWindow( fd_charaindex, seqno,
							select, objindex, makeStringFromEscaped(data));

#ifdef _NO_WARP
			 if (CONNECT_get_seqno(fd)==CHAR_WINDOWTYPE_WINDOWWARPMAN_MAIN)
             {
				 CONNECT_set_seqno(fd,-1);
				 CONNECT_set_selectbutton(fd,1);
			 }
#endif
	}
//ttom+1
END_WN:
	return;
          //CONNECT_set_pass(fd,TRUE);//ttom
}

/*------------------------------------------------------------
 * 云踞仃乒□玉  凳毛熬仃凶
 ------------------------------------------------------------*/
void lssproto_HL_recv( int fd,int flg )
{
	char	msgbuf[128];
	int		i, fd_charaindex;
    CHECKFD;

    fd_charaindex = CONNECT_getCharaindex( fd );
	/* 爵    元扎卅井匀凶日  骰允月 */
	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE)
		== BATTLE_CHARMODE_NONE)
	{
		return;
	}
#ifdef _LOCKHELP_OK				// (不可开) Syu ADD 锁定不可加入战斗
	if((CHAR_getInt(fd_charaindex,CHAR_FLOOR) >= 8200 && CHAR_getInt(fd_charaindex,CHAR_FLOOR) <= 8213) ||
		 (CHAR_getInt(fd_charaindex,CHAR_FLOOR) >= 30017 && CHAR_getInt(fd_charaindex,CHAR_FLOOR) <= 30021)
		){
		return ; 
	}
#endif
	if( flg == TRUE ) {

#ifdef _ESCAPE_RESET // 使用恶宝逃跑後x分钟内不可求救
		if( getStayEncount( fd ) ) {
			//print(" 恶宝中组队 ");
			if( time(NULL) - CHAR_getWorkInt( fd_charaindex, CHAR_WORKLASTESCAPE) < 5*60 ) {
				lssproto_HL_send( fd, FALSE);
				//print(" 恶宝逃跑後组队 ");
				CHAR_talkToCli( fd_charaindex, -1, "暂时不可以求救。", CHAR_COLORYELLOW);
				return;
			}
		}
#endif

		/* 云踞仃乒□玉及白仿弘  化月 */
		BattleArray[CHAR_getWorkInt( fd_charaindex,
			CHAR_WORKBATTLEINDEX)].Side[
			CHAR_getWorkInt( fd_charaindex,
			CHAR_WORKBATTLESIDE)].flg |= BSIDE_FLG_HELP_OK;

		snprintf( msgbuf, sizeof( msgbuf),
				  "%s 在求救！",
				  CHAR_getChar( fd_charaindex, CHAR_NAME));
	}
	else {
		/* 云踞仃乒□玉及白仿弘  午允 */
		BattleArray[CHAR_getWorkInt( fd_charaindex,
			CHAR_WORKBATTLEINDEX)].Side[
			CHAR_getWorkInt( fd_charaindex,
			CHAR_WORKBATTLESIDE)].flg &= ~BSIDE_FLG_HELP_OK;

		snprintf( msgbuf, sizeof( msgbuf),
				  "%s 决定拒绝帮助。",
				  CHAR_getChar( fd_charaindex, CHAR_NAME));
	}

	/* 云踞仃橇谪互  歹匀凶仪毛爵  醮棉卞(愤坌殖戈)霜耨 */
	for( i = 0; i < 5; i ++ ) {
		int toindex = BattleArray[CHAR_getWorkInt(
					    fd_charaindex, CHAR_WORKBATTLEINDEX)].Side[
						CHAR_getWorkInt( fd_charaindex,
						CHAR_WORKBATTLESIDE)].Entry[i].charaindex;
		if( CHAR_CHECKINDEX( toindex)) {
			int tofd = getfdFromCharaIndex( toindex );
			if( tofd != -1 ) {
				lssproto_HL_send( tofd, flg);
			}
			/* 丢永本□斥霜耨 */
			CHAR_talkToCli( toindex, -1, msgbuf, CHAR_COLORYELLOW);
			/* 云踞仃CA  憎］引凶反壅允 */
			CHAR_sendBattleEffect( toindex, ON);
		}
	}
}
/*------------------------------------------------------------
 * proc 毛仁木午蜕歹木凶［
 ------------------------------------------------------------*/
void lssproto_ProcGet_recv( int fd )
{
	outputNetProcLog( fd, 1);
}
/*------------------------------------------------------------
 * 皿伊奶乩□醒毛仁木午蜕歹木凶［
 ------------------------------------------------------------*/
void lssproto_PlayerNumGet_recv( int fd )
{
	int		i;
	int		clicnt  =0;
	int		playercnt = 0;
    for( i = 0; i < ConnectLen; i ++ ) {
        if( CONNECT_getUse_debug(i,1017) ){
            if( CONNECT_getCtype(i) == CLI) {
            	clicnt ++;
            	if( CONNECT_getCharaindex(i) >= 0 ) playercnt++;
            }
		}
	}
	lssproto_PlayerNumGet_send( fd, clicnt, playercnt);
}


/*------------------------------------------------------------
 * 棋爵邰菲  戏［
 ------------------------------------------------------------*/
void lssproto_LB_recv( int fd,int x,int y )
{
    int fd_charaindex;

    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    {//ttom avoid warp at will
       int ix,iy;
       ix=CHAR_getInt(fd_charaindex, CHAR_X);
       iy=CHAR_getInt(fd_charaindex, CHAR_Y);
       if( (ix!=x)||(iy!=y)){
         //print("\n<LB>--Error!!!!");
         //print("\n<LB>origion x=%d,y=%d",ix,iy);
         //print("\n<LB>modify  X=%d,Y=%d",x,y);
         x=ix;
         y=iy;
       }
    }   
    CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
	/* 杀梁匹五月井民尼永弁仄化杀梁允月 */
	BATTLE_WatchTry( fd_charaindex );
}
/*------------------------------------------------------------
 * 扑乓永玄母它件质  钒铵
 ------------------------------------------------------------*/
void lssproto_Shutdown_recv( int fd,char* passwd,int min )
{
	char	buff[256];
	if( strcmp( passwd, "hogehoge") == 0 ) {
	    int     i;
	    int     playernum = CHAR_getPlayerMaxNum();

		snprintf( buff, sizeof( buff),"极度的系统公告。");
	    for( i = 0 ; i < playernum ; i++) {
	        if( CHAR_getCharUse(i) != FALSE ) {
				CHAR_talkToCli( i, -1, buff, CHAR_COLORYELLOW);
			}
		}
		SERVSTATE_setLimittime(min);
		SERVSTATE_setShutdown( NowTime.tv_sec );
		SERVSTATE_setDsptime( 0 );
	}
}
void lssproto_PMSG_recv( int fd,int index,int petindex,int itemindex,
						char* message,int color )
{

	// CoolFish: Prevent Trade Cheat 2001/4/18
	int fd_charaindex;
	fd_charaindex = CONNECT_getCharaindex(fd);
	if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
	    	return;
	PETMAIL_sendPetMail( CONNECT_getCharaindex( fd ),
							index, petindex, itemindex, message, color);

}
/*------------------------------------------------------------
 * 矢永玄  银迕熬耨
 ------------------------------------------------------------*/
void lssproto_PS_recv( int fd, int havepetindex, int havepetskill, int toindex, char* data )
{
    int to_charaindex = Callfromcli_Util_getTargetCharaindex( fd, toindex);
	int charaindex = CONNECT_getCharaindex( fd );
	int	petindex;
	BOOL	ret;
	petindex = CHAR_getCharPet( charaindex, havepetindex);
	if( !CHAR_CHECKINDEX( petindex)) return;
	
	ret = PETSKILL_Use( petindex, havepetskill, to_charaindex, data );
	lssproto_PS_send( fd, ret, havepetindex, havepetskill, toindex);
}
/*------------------------------------------------------------
 * 甄  毛本永玄允月
 ------------------------------------------------------------*/
void lssproto_SP_recv( int fd,int x,int y, int dir  )
{
    int fd_charaindex;

    fd_charaindex = CONNECT_getCharaindex( fd );
    {//ttom avoid the warp at will
       int i_x,i_y;
       i_x=CHAR_getInt(fd_charaindex, CHAR_X);
       i_y=CHAR_getInt(fd_charaindex, CHAR_Y);
                         
       if((i_x!=x)||(i_y!=y)){
           x=i_x;
           y=i_y;
       }
    }//ttom
                                                                                       
    CHAR_setMyPosition_main( fd_charaindex, x,y,dir,TRUE);

}

/*------------------------------------------------------------
 * CoolFish: Trade Command 2001/4/18
 ------------------------------------------------------------*/
void lssproto_TD_recv( int fd, char* message )
{
      int fd_charaindex;
      CHECKFDANDTIME;
          
      fd_charaindex = CONNECT_getCharaindex( fd );
      print(" MAP_TRADEPICKUP_check0 ");
      CHAR_Trade(fd, fd_charaindex, message);
}

/*------------------------------------------------------------
 * CoolFish: Family Command 2001/5/24
 ------------------------------------------------------------*/
void lssproto_FM_recv( int fd, char* message )
{
      int fd_charaindex;
      struct timeval recvtime;
      CHECKFDANDTIME;
      
      // add code by shan
      CONNECT_getLastrecvtime( fd, &recvtime);
      if( time_diff( NowTime, recvtime) < 0.5 ){
          return;
      }
      CONNECT_setLastrecvtime(fd, &NowTime);

      fd_charaindex = CONNECT_getCharaindex( fd );
      
      if( checkStringErr( message ) )	return;
      
      CHAR_Family(fd, fd_charaindex, message);
      
}

// shan 2002/01/10
void lssproto_PETST_recv( int fd,  int nPet, int sPet )
{
      int charaindex;
	  int i, nums=0;
      CHECKFDANDTIME;
      
      charaindex = CONNECT_getCharaindex( fd );            
	  if (!CHAR_CHECKINDEX( charaindex ) )	return;

	  if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
	  
	  for( i=0; i<5; i++)	{
		if( CHAR_getWorkInt( charaindex, CHAR_WORK_PET0_STAT+i) == TRUE )
			nums++;
	  }
	  if( nums <= 3 )
		CHAR_setWorkInt( charaindex, CHAR_WORK_PET0_STAT+nPet, sPet);
	     
}

void lssproto_BM_recv(int fd, int iindex)
{
#ifdef _BLACK_MARKET
	int charaindex;      
    CHECKFDANDTIME;
      
    charaindex = CONNECT_getCharaindex( fd);            
	if(!CHAR_CHECKINDEX( charaindex)) return;
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
	
	ITEM_BM_Exchange( charaindex, iindex);
#endif
}

#ifdef _MIND_ICON
void lssproto_MA_recv(int fd, int x, int y, int nMind)
{
	int charaindex;      
    CHECKFDANDTIME;    

	charaindex = CONNECT_getCharaindex( fd);            
	if(!CHAR_CHECKINDEX( charaindex)) return;	
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return ;		
	
	{
       int i_x, i_y;
       i_x = CHAR_getInt( charaindex, CHAR_X);
       i_y = CHAR_getInt( charaindex, CHAR_Y);
                         
       if((i_x!=x)||(i_y!=y)){     
           x = i_x;
           y = i_y;
       }
    }
	
	//print("\nshan------------------>mind action->%d x->%d y->%d", nMind, x, y);
	CHAR_setMyPosition( charaindex, x, y, TRUE);
	CHAR_setWorkInt( charaindex, CHAR_MIND_NUM, nMind);
	CHAR_sendMindEffect( charaindex, CHAR_getWorkInt( charaindex, CHAR_MIND_NUM));	
	if(CHAR_getWorkInt( charaindex, CHAR_MIND_NUM) != 101290 &&
	   CHAR_getWorkInt( charaindex, CHAR_MIND_NUM) != 101294   &&
	   CHAR_getWorkInt( charaindex, CHAR_MIND_NUM) != 101288 )
	CHAR_setWorkInt( charaindex, CHAR_MIND_NUM, 0);
	//print("\nshan------------------>end");
	
	return;
}
#endif
BOOL checkStringErr( char *checkstring )
{
        int i,ach;
        for (i=0,ach=0;i<strlen(checkstring);i++) {
        	if ((unsigned char)checkstring[i]==0xff) { ach=1; break; } // Force no 0xff
                if ((unsigned char)checkstring[i]==0x80) { ach=1; break; } // Force no 0x80
                if ((unsigned char)checkstring[i]==0x7f) { ach=1; break; } // Force no 0x7f
                if ((unsigned char)checkstring[i]<=0x20) { ach=1; break; } // Force greater than 0x20
                if (ach) {
                	if ((((unsigned char)checkstring[i]>=0x40)&&((unsigned char)checkstring[i]<=0x7e))||
                        (((unsigned char)checkstring[i]>=0xa1)&&((unsigned char)checkstring[i]<=0xfe))) ach=0;
                } else {
                	if (((unsigned char)checkstring[i]>=0xa1)&&((unsigned char)checkstring[i]<=0xfe)) ach=1;
                }
	}
	if (ach)
	{
		print(" StringDog! ");
		return	TRUE;
	}
	
	return FALSE;
	
}

#ifdef _FIX_DEL_MAP           // WON ADD 玩家抽地图送监狱
void lssproto_DM_recv( int fd )
{
	 int index;
	 index = CONNECT_getCharaindex( fd );
 	 if( !CHAR_CHECKINDEX( index)) return;
	 CHAR_warpToSpecificPoint(index,117,225,13);
}
#endif

#ifdef _CHECK_GAMESPEED
void lssproto_CS_recv( int fd )
{
	 int index, Ttime, NowTimes;
	 index = CONNECT_getCharaindex( fd );
 	 if( !CHAR_CHECKINDEX( index)) return;

	 NowTimes = (int)time(NULL);
	 Ttime = getGameSpeedTime( fd);
	 setGameSpeedTime( fd, NowTimes);

	 if( CHAR_getWorkInt( index, CHAR_WORKFLG) & WORKFLG_DEBUGMODE )	{	
	 }else if( (NowTimes-Ttime) < 20 ){
		lssproto_CS_send( fd, 20 - (NowTimes-Ttime));
	 }
}
#endif

#ifdef _TEAM_KICKPARTY
void lssproto_KTEAM_recv( int fd, int si)
{
	int charaindex=-1, pindex;
	if( si < 0 || si > 5 ) return;
	charaindex = CONNECT_getCharaindex( fd );
	if( !CHAR_CHECKINDEX( charaindex) ) return;

	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_LEADER ) return;

	pindex = CHAR_getWorkInt( charaindex, si + CHAR_WORKPARTYINDEX1);
	if( !CHAR_CHECKINDEX( pindex) ) return;
	if( CHAR_getWorkInt( pindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER ) return;

	if( CHAR_DischargeParty( pindex, 0) == FALSE ){
		CHAR_talkToCli( charaindex, -1, "踢除失败！", CHAR_COLORYELLOW);
	}else{
		char buf1[256];
		sprintf( buf1, "队长[%s]将你踢除！", CHAR_getUseName( charaindex ));
		CHAR_talkToCli( pindex, -1, buf1, CHAR_COLORYELLOW);
		sprintf( buf1, "将[%s]踢除出团队！", CHAR_getUseName( pindex ));
		CHAR_talkToCli( charaindex, -1, buf1, CHAR_COLORYELLOW);
	}
}
#endif

#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道
void lssproto_CHATROOM_recv (int fd , char *data)
{
	ChatRoom_recvall ( fd , data ) ; 
}
#endif

#ifdef _NEWREQUESTPROTOCOL			// (不可开) Syu ADD 新增Protocol要求细项
void lssproto_RESIST_recv (int fd )
{
	int charindex = -1 ;

	char token[256];
	charindex = CONNECT_getCharaindex( fd );
	if( !CHAR_CHECKINDEX( charindex) ) return;
	sprintf ( token , "%d|%d|%d|%d|%d|%d|%d|%d" , 
		CHAR_getInt( charindex, CHAR_EARTH_RESIST ) ,
		CHAR_getInt( charindex, CHAR_WATER_RESIST ) ,
		CHAR_getInt( charindex, CHAR_FIRE_RESIST ) ,
		CHAR_getInt( charindex, CHAR_WIND_RESIST ) ,		
		CHAR_getInt( charindex, CHAR_EARTH_EXP ) ,
		CHAR_getInt( charindex, CHAR_WATER_EXP ) ,
		CHAR_getInt( charindex, CHAR_FIRE_EXP ) ,
		CHAR_getInt( charindex, CHAR_WIND_EXP ) 
		);
	lssproto_RESIST_send ( fd , token ) ; 
}
#endif

#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
void lssproto_BATTLESKILL_recv (int fd, int iNum)
{
	int charaindex = CONNECT_getCharaindex( fd );
#ifndef _PROSKILL_OPTIMUM
	int skillindex=-1,char_pskill=-1,profession_skill=-1;
#endif

	if( !CHAR_CHECKINDEX( charaindex) ) return;
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE ) return;

#ifndef _PROSKILL_OPTIMUM	// Robin fix cancel 此处略过职业检查, 改在 PROFESSION_SKILL_Use 中检查
	// 人物的职业
	char_pskill = CHAR_getInt( charaindex, PROFESSION_CLASS );
	
	skillindex = PROFESSION_SKILL_GetArray( charaindex, iNum);
	Pskillid = skillindex;
	// 技能的职业
	profession_skill = PROFESSION_SKILL_getInt( Pskillid, PROFESSION_SKILL_PROFESSION_CLASS);

	if( (char_pskill > 0) && (char_pskill == profession_skill) ){
#else
	if( 1 ){
#endif
		if( PROFESSION_SKILL_Use( charaindex, iNum, 0, NULL ) != 1 ){
			print("\n 玩家(%s)使用技能错误!", CHAR_getUseName( charaindex ) );
			saacproto_ACKick_send( acfd, CHAR_getChar(charaindex,CHAR_CDKEY), getFdidFromCharaIndex( charaindex), 1);
		}
	}

}

#endif

#ifdef _STREET_VENDOR
void lssproto_STREET_VENDOR_recv(int fd,char *message)
{
	int charaindex = CONNECT_getCharaindex(fd);
	
	if(!CHAR_CHECKINDEX(charaindex)) return;
	if(CHAR_getWorkInt(charaindex,CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return;
	if(CHAR_getWorkInt(charaindex,CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE){
		CHAR_talkToCli(charaindex,-1,"组队状态下不能交易",CHAR_COLORYELLOW);
		return;
	}
	CHAR_sendStreetVendor(charaindex,message);
}
#endif

#ifdef _RIGHTCLICK
void lssproto_RCLICK_recv(int fd, int type, char* data)
{
	print("\n RCLICK_recv( type=%d data=%s) ", type, data);
}
#endif

#ifdef _JOBDAILY
void lssproto_JOBDAILY_recv(int fd,char *data)
{
	int charaindex = CONNECT_getCharaindex(fd);
	if(!CHAR_CHECKINDEX(charaindex)) return;

	CHAR_JobDaily(charaindex,data);
}
#endif

#ifdef _TEACHER_SYSTEM
void lssproto_TEACHER_SYSTEM_recv(int fd,char *data)
{
	int charaindex = CONNECT_getCharaindex(fd);

	if(!CHAR_CHECKINDEX(charaindex)) return;
	CHAR_Teacher_system(charaindex,data);
}
#endif
