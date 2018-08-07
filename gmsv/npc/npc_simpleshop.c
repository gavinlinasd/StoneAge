#include "version.h"
#include <ctype.h>

#include "object.h"
#include "char_base.h"
#include "char.h"
#include "item.h"
#include "util.h"
#include "handletime.h"
#include "anim_tbl.h"
#include "npc_door.h"
#include "lssproto_serv.h"
#include "npcutil.h"
#include "npccreate.h"
#include "log.h"


/*
 *   赏读卅饕及伙□民件［by nakamura
 *
 * NPCARGUMENT卞俜  仄凶失奶  丞毛  蜃卞赭  仄化中月云饕［
 *     SIMPLESHOP_MAXINFINITITEM潘    蜃卞戏岳允月仇午互匹五月［
 *   蜃戏岳失奶  丞午  元潘  及失奶  丞毛  匀凶日  匀凶支勾互
 * 壑钗井日壅  允月［
 * 引凶｝皿伊奶乩□井日手及毛  中午月仇午互匹五月互｝丐月袄蟾
 * (NPC_SIMPLESHOPMAXBUYPRICE)方曰嫖中嗯毛障  歹卅仁化反中仃
 * 卅中  潸反匹五卅中［  中午匀凶失奶  丞反公及桦匹卅仁卅月［
 * 引凶｝失奶  丞及涩烂匹cost互涩烂今木化中卅中手及手  中午木卅中［
 * 2谛动晓及皿伊奶乩□互饕卞五凶午五反｝  及菸毛穸燮允月［公丹允月
 * 仇午卞方曰饕互捩菁卞簿井毛锹澎仄化中月仇午毛  什［
 *
 * 正奶皿  “SimpleShop
 * 健丹奶矛件玄: init, talked, specialtalked
 *
 *
 */
static void NPC_SimpleShopOpenShopWindow( int meindex, int cliindex,
                                          char *npcarg );
static void NPC_SimpleShopGetEscapedItemString( int shopindex,
                                                int cliindex, char *str );
static BOOL NPC_SimpleShopProcessBuyMsg( char *msg, int shop, int cli );
static BOOL NPC_SimpleShopProcessSellMsg( char *msg, int shop, int cli );

static BOOL NPC_LimitBuyInShopProcessSellMsg(char *msg,int shop,int cli);


static BOOL NPC_SimpleShopFillItem( int meindex, char *npcarg );
static int NPC_SimpleShopGetItemNum( int meindex );
static void NPC_SimpleShopNormalWindow( int meindex,
                                        int playerindex, char *str );
static int NPC_ShopWindowIndexToItemTableIndex( int charindex,
                                                int shopwindowindex );
static BOOL NPC_SimpleShopOpenShopOrNot( char *msg, char *openshopstr );
//static void NPC_SimpleShopSetLeakLevel( int meindex );

#define NPC_SIMPLESHOPMAXBUYPRICE 9999
#define SIMPLESHOPTALKBUFSIZE 256

#define SIMPLESHOP_FATALERRSTR "好痛！对不起,最近状况不太佳。。。"

#define SIMPLESHOP_MAXINFINITITEM 40

enum{
    OPENSHOPTOKEN=1,
        MSGTOKEN,
        MAINTOKEN,
        BUYTOKEN,
        OKBUYTOKEN,
        SELLTOKEN,
        OKSELLTOKEN,
        POORTOKEN,
        ITEMFULLTOKEN,
        ANOTHERPLAYERTOKEN,
        RAREITEMTOKEN,
        THANKYOUTOKEN,
        ITEMLISTTOKEN,

        BUYINITEMLISTTOKEN,
        };
void NPC_SimpleShopTalked( int meindex, int talker, char *msg, int color )
{
#define SHOPRANGE 3
    char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE], token[NPC_UTIL_GETARGSTR_LINEMAX];
    if( CHAR_getInt( talker,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ){
        return;
    }
    if( NPC_Util_charIsInFrontOfChar( talker, meindex, SHOPRANGE )
        && ! CHAR_getFlg( talker,CHAR_ISDIE )  ){
        NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg));
        getStringFromIndexWithDelim( npcarg,"|", OPENSHOPTOKEN,token, sizeof(token));
        if( NPC_SimpleShopOpenShopOrNot( msg, token )
           && NPC_Util_isFaceToFace( meindex, talker, SHOPRANGE ) ){
            CHAR_setWorkInt( meindex, CHAR_WORKSHOPCLIENTINDEX, talker );
            NPC_SimpleShopOpenShopWindow( meindex, talker, npcarg );
        }else{
            int tokennum;
            int i;
            char tmp[NPC_UTIL_GETARGSTR_LINEMAX];
            getStringFromIndexWithDelim( npcarg,"|", MSGTOKEN,token, sizeof(token));
            tokennum = 1;
            for( i=0;token[i]!='\0';i++ ){
                if( token[i] == ',' ) tokennum++;
            }
            getStringFromIndexWithDelim( token,",", rand()%tokennum+1,tmp, sizeof(tmp));
            CHAR_talkToCli( talker, meindex, tmp, CHAR_COLORWHITE );
        }
    }
}

void NPC_SimpleShopSpecialTalked( int meindex, int talker, char *msg, int color )
{
    if( CHAR_getInt(talker,CHAR_WHICHTYPE) == CHAR_TYPEPLAYER
        && NPC_Util_isFaceToFace( meindex, talker, SHOPRANGE ) ){
        char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE], token[NPC_UTIL_GETARGSTR_LINEMAX];
        NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg));
        switch( tolower(msg[0]) ){
        case 's':
        {
            int     ret;
            if( CHAR_getWorkInt(meindex,CHAR_WORKSHOPCLIENTINDEX)!= talker ){
                getStringFromIndexWithDelim( npcarg,"|",ANOTHERPLAYERTOKEN,
                                             token, sizeof(token));
                NPC_SimpleShopNormalWindow( meindex, talker, token );
                return;
            }
            ret = getStringFromIndexWithDelim( msg, "|", 3, token, sizeof(token));
            if( ret == FALSE ){
                token[0] = 'e';
                token[1] = '\0';
            }
            switch( tolower(token[0])){
            case 'b':
                if( (ret=NPC_SimpleShopProcessBuyMsg(msg,meindex,talker))
                    <0){
                    switch( ret ){
                    case -2:
                        getStringFromIndexWithDelim( npcarg,"|",POORTOKEN,
                                                     token,sizeof(token));
                        NPC_SimpleShopNormalWindow( meindex,talker,token);
                        break;
                    case -3:
                        getStringFromIndexWithDelim( npcarg,"|",
                                                     ITEMFULLTOKEN,
                                                     token,sizeof(token));
                        NPC_SimpleShopNormalWindow(meindex,talker,token);
                        break;
                    case -1:
                    default:
                        NPC_SimpleShopNormalWindow(meindex, talker,
                                                   SIMPLESHOP_FATALERRSTR
                            );
                        break;
                    }
                }else{
                    NPC_SimpleShopFillItem( meindex, npcarg );
                    getStringFromIndexWithDelim(npcarg,"|",THANKYOUTOKEN,
                                                token, sizeof(token));
                    NPC_SimpleShopNormalWindow(meindex,talker,token);
                }
                break;
            case 's':
                if( CHAR_getWorkInt(meindex,CHAR_WORKSHOPCLIENTINDEX) != talker ){
                    getStringFromIndexWithDelim( npcarg,"|",
                                                 ANOTHERPLAYERTOKEN,
                                                 token, sizeof(token));
                    NPC_SimpleShopNormalWindow( meindex, talker, token );
                    return;
                }
                if((ret=NPC_SimpleShopProcessSellMsg(msg,meindex,talker)) <0 ){
                    switch(ret){
                    case -2:
                        getStringFromIndexWithDelim( npcarg,"|",
                                                     RAREITEMTOKEN,
                                                     token, sizeof(token) );
                        NPC_SimpleShopNormalWindow( meindex,talker,token);
                        break;
                    case -1:
                    default:
                        NPC_SimpleShopNormalWindow(meindex, talker,
                                                   SIMPLESHOP_FATALERRSTR );
                        break;
                    }
                }else{
                    getStringFromIndexWithDelim(npcarg,"|",THANKYOUTOKEN,
                                                token, sizeof(token));
                    NPC_SimpleShopNormalWindow(meindex,talker,token);
                }
                break;
            case 'e':
            default:
                CHAR_setWorkInt( meindex, CHAR_WORKSHOPCLIENTINDEX, -1 );
                break;
            }
            break;
        }
        case 'i':
            if( CHAR_getWorkInt(meindex,CHAR_WORKSHOPCLIENTINDEX)
                == talker ){
                NPC_SimpleShopOpenShopWindow( meindex, talker, npcarg );
            }
            break;
        default:
            break;
        }
    }
}

void NPC_LimitBuyInShopSpecialTalked( int meindex, int talker, char *msg, int color )
{
    if( CHAR_getInt(talker,CHAR_WHICHTYPE) == CHAR_TYPEPLAYER
        && NPC_Util_isFaceToFace( meindex, talker, SHOPRANGE ) ){
        char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE], token[NPC_UTIL_GETARGSTR_LINEMAX];
        NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg));
        switch( tolower(msg[0]) ){
        case 's':
        {
            int     ret;
            if( CHAR_getWorkInt(meindex,CHAR_WORKSHOPCLIENTINDEX) != talker ){
                getStringFromIndexWithDelim( npcarg,"|",
                                             ANOTHERPLAYERTOKEN,
                                             token, sizeof(token));
                NPC_SimpleShopNormalWindow( meindex, talker, token );
                return;
            }
            ret = getStringFromIndexWithDelim( msg, "|", 3, token, sizeof( token));
            if( ret == FALSE ){
                token[0] = 'e';
                token[1] = '\0';
            }
            switch( tolower(token[0])){
            case 'b':
                if( ( ret = NPC_SimpleShopProcessBuyMsg( msg, meindex, talker)) < 0){
                    switch( ret ){
                    case -2:
                        getStringFromIndexWithDelim( npcarg,"|",POORTOKEN,
                                                     token,sizeof(token));
                        NPC_SimpleShopNormalWindow( meindex,talker,token);
                        break;
                    case -3:
                        getStringFromIndexWithDelim( npcarg,"|",
                                                     ITEMFULLTOKEN,
                                                     token,sizeof(token));
                        NPC_SimpleShopNormalWindow(meindex,talker,token);
                        break;
                    case -1:
                    default:
                        NPC_SimpleShopNormalWindow(meindex, talker,
                                                   SIMPLESHOP_FATALERRSTR
                            );
                        break;
                    }
                }else{
                    NPC_SimpleShopFillItem( meindex, npcarg );
                    getStringFromIndexWithDelim(npcarg,"|",THANKYOUTOKEN,
                                                token, sizeof(token));
                    NPC_SimpleShopNormalWindow(meindex,talker,token);

                }
                break;
            case 's':
                if( CHAR_getWorkInt(meindex,CHAR_WORKSHOPCLIENTINDEX) != talker ){
                    getStringFromIndexWithDelim( npcarg,"|",
                                                 ANOTHERPLAYERTOKEN,
                                                 token, sizeof(token));
                    NPC_SimpleShopNormalWindow( meindex, talker, token );
                    return;
                }
                if((ret=NPC_LimitBuyInShopProcessSellMsg(
                    msg,meindex,talker))<0){
                    switch(ret){
                    case -2:
                        getStringFromIndexWithDelim( npcarg,"|",
                                                     RAREITEMTOKEN,
                                                     token,sizeof(token));
                        NPC_SimpleShopNormalWindow( meindex,talker,token);
                        break;
                    case -1:
                    default:
                        NPC_SimpleShopNormalWindow(meindex, talker, SIMPLESHOP_FATALERRSTR
                            );
                        break;
                    }
                }else{
                    getStringFromIndexWithDelim(npcarg,"|",THANKYOUTOKEN, token, sizeof(token));
                    NPC_SimpleShopNormalWindow(meindex,talker,token);

                }
                break;
            case 'e':
            default:
                CHAR_setWorkInt( meindex, CHAR_WORKSHOPCLIENTINDEX, -1 );
                break;
            }
            break;
        }
        case 'i':
            if( CHAR_getWorkInt(meindex,CHAR_WORKSHOPCLIENTINDEX)
                == talker ){
                NPC_SimpleShopOpenShopWindow( meindex, talker, npcarg );
            }
            break;
        default:
            break;
        }
    }
}





/*
 * 饕互用□穴伙它奶件玉它毛请允［它奶件玉它及廿凶氏毛瓷允午｝
 *   赓卞饕卞  仄井仃凶午五午  元橇谪卞手升月［
 * int meindex : 饕及index
 * int playerindex : 它奶件玉它毛请允皿伊奶乩□及index
 * char *str :
 */
static void NPC_SimpleShopNormalWindow( int meindex, int playerindex,
                                        char *str )
{
    int fd;
    int objindex;
    char tmp[512], *name;

    objindex = CHAR_getWorkInt(meindex,CHAR_WORKOBJINDEX);
    name = CHAR_getChar( meindex, CHAR_NAME );
    snprintf(tmp,sizeof(tmp),"W|%d|N|%s|123|%s", objindex, name, str );
    fd = getfdFromCharaIndex(playerindex);
    if( fd == -1 ) return;
    lssproto_TK_send(fd,-1,tmp,CHAR_COLORWHITE );
}

/*
 * 饕它奶件玉它及奶件犯弁旦井日｝失奶  丞  □皮伙及奶件犯弁旦卞  晶
 * 娄醒
 * int charindex:平乓仿及奶件犯弁旦
 * int shopwindowindex:饕它奶件玉它及奶件犯弁旦［0井日反元引月［
 * 忒曰袄
 * 失奶  丞  □皮伙及奶件犯弁旦［巨仿□反-1［
 */
static int NPC_ShopWindowIndexToItemTableIndex( int charindex,
                                                int shopwindowindex )
{
    int i, counter;

    counter = 0;
    for( i=CHAR_STARTITEMARRAY;i<CHAR_MAXITEMHAVE;i++ ){
        if( ITEM_CHECKINDEX( CHAR_getItemIndex(charindex,i) ) ){
            if( shopwindowindex == counter ) return i;
            counter++;
        }
    }
    return -1;
}

static BOOL NPC_SimpleShopProcessBuyMsg(char *msg, int shopindex,
                                        int playerindex )
{
    int shopwindowindex, itemtableindex,itemind;
    int price, tmpgold;
    char buf[64];

    if(!CHAR_CHECKINDEX(shopindex)||!CHAR_CHECKINDEX(playerindex))
        return -1;

    if( !getStringFromIndexWithDelim( msg,"|",4,buf,sizeof(buf) )){
        return -1;
    }
    shopwindowindex = atoi(buf);
    itemtableindex=NPC_ShopWindowIndexToItemTableIndex( shopindex, shopwindowindex );
    itemind = CHAR_getItemIndex(shopindex,itemtableindex );
    if( ! ITEM_CHECKINDEX(itemind) ) return -1;
    price = ITEM_getInt( itemind, ITEM_COST )
        * NPC_Util_buyRate(playerindex);

    tmpgold = CHAR_getInt(playerindex,CHAR_GOLD);
    if( tmpgold < price ){
        return -2;
    }

    if( ! NPC_Util_moveItemToChar(playerindex,itemind,TRUE ) ){
        return -3;
    }
	CHAR_AddGold( playerindex, price );

    CHAR_sendStatusString( playerindex , "P");
    return 1;
}

static BOOL NPC_SimpleShopProcessSellMsg(char *msg, int shopindex,
                                         int playerindex )
{
    int sellwindowindex, itemtableindex,itemind, id;
    int price, tmpgold;
    char buf[64];
    if(!CHAR_CHECKINDEX(shopindex)||!CHAR_CHECKINDEX(playerindex))
        return -1;
    if( !getStringFromIndexWithDelim( msg,"|",4,buf,sizeof(buf) )){
        return -1;
    }
    sellwindowindex = atoi(buf);
    itemtableindex = sellwindowindex + CHAR_STARTITEMARRAY;
    itemind = CHAR_getItemIndex(playerindex,itemtableindex );
    if( ! ITEM_CHECKINDEX(itemind) ) return -1;

    price = ITEM_getInt( itemind, ITEM_COST );

    if( price <= 0 || price > NPC_SIMPLESHOPMAXBUYPRICE ){
        return -2;
    }
    price *= NPC_Util_sellRate(playerindex);

	CHAR_AddGold( playerindex, price );

	CHAR_sendStatusString( playerindex , "P");

    id = ITEM_getInt(itemind,ITEM_ID);
	{
		LogItem(
			CHAR_getChar( playerindex, CHAR_NAME ), /* 平乓仿   */
			CHAR_getChar( playerindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
			itemind,
#else
       		ITEM_getInt( itemind, ITEM_ID ),  /* 失奶  丞  寞 */
#endif
			"Sell",
			CHAR_getInt( playerindex,CHAR_FLOOR),
			CHAR_getInt( playerindex,CHAR_X ),
 	      	CHAR_getInt( playerindex,CHAR_Y ),
	        ITEM_getChar( itemind, ITEM_UNIQUECODE),
			ITEM_getChar( itemind, ITEM_NAME),
			ITEM_getInt( itemind, ITEM_ID)
		);
	}

    if( ! NPC_Util_moveItemToChar( shopindex,itemind,TRUE ) ){
        return -1;
    }
    NPC_Util_RemoveItemByID(shopindex,id,FALSE);
    return 1;
}

static BOOL NPC_LimitBuyInShopProcessSellMsg(char *msg,
                                 int shopindex, int playerindex )
{
    int sellwindowindex, itemtableindex,itemind, id,i,buyokflag, setid;
    int price, tmpgold;
    char buf[BUFSIZ];
    char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE], token[NPC_UTIL_GETARGSTR_LINEMAX];

    if(!CHAR_CHECKINDEX(shopindex)||!CHAR_CHECKINDEX(playerindex))
        return -1;
    if( !getStringFromIndexWithDelim( msg,"|",4,buf,sizeof(buf) )){
        return -1;
    }
    sellwindowindex = atoi(buf);
    itemtableindex = sellwindowindex + CHAR_STARTITEMARRAY;
    itemind = CHAR_getItemIndex(playerindex,itemtableindex );
    if( ! ITEM_CHECKINDEX(itemind) ) return -1;
    id = ITEM_getInt(itemind,ITEM_ID);
    NPC_Util_GetArgStr( shopindex, npcarg, sizeof(npcarg));
    getStringFromIndexWithDelim( npcarg, "|", BUYINITEMLISTTOKEN, token, sizeof( token) );
    buyokflag = FALSE;
    for( i=1;getStringFromIndexWithDelim(token,",",i,buf,sizeof(buf));  i++){

	  char *tok1 = NULL;
	  char *tok2 = NULL;
	  int no1=0, no2=0;
	  tok1 = strtok( buf, "-" );
	  tok2 = strtok( NULL, "-" );

	  setid = atoi(buf);

	  if( tok1 == NULL || tok2 == NULL ){
		if( setid == 0 ) return -2;
		if( setid == id ) buyokflag = TRUE;
	  } else {
		no1 = atoi( tok1 );
		no2 = atoi( tok2 );
		if( no1 <= id && id <= no2 ){
		  buyokflag = TRUE;
		}
	  }
    }
    if( buyokflag == FALSE ) return -2;
    price = ITEM_getInt( itemind, ITEM_COST );
    price *= NPC_Util_sellRate(playerindex);

	CHAR_AddGold( playerindex, price );

    CHAR_sendStatusString( playerindex , "P");
    if( ! NPC_Util_moveItemToChar( shopindex,itemind,TRUE ) ){
        return -1;
    }

	{
		LogItem(
			CHAR_getChar( playerindex, CHAR_NAME ), /* 平乓仿   */
			CHAR_getChar( playerindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
			itemind,
#else
       		ITEM_getInt( itemind, ITEM_ID ),  /* 失奶  丞  寞 */
#endif
			"Sell",
			CHAR_getInt( playerindex,CHAR_FLOOR),
			CHAR_getInt( playerindex,CHAR_X ),
 	      	CHAR_getInt( playerindex,CHAR_Y ),
	        ITEM_getChar( itemind, ITEM_UNIQUECODE),
			ITEM_getChar( itemind, ITEM_NAME),
			ITEM_getInt( itemind, ITEM_ID)
		);
	}

    /*   中潸匀凶失奶  丞毛壅蛔 */
    NPC_Util_RemoveItemByID(shopindex,id,FALSE);

    return 1;

}

/*
 * 饕卞丐月失奶  丞及醒毛  月
 */
static int NPC_SimpleShopGetItemNum( int meindex )
{
    int counter, i;

    counter = 0;
    for( i=CHAR_STARTITEMARRAY;i<CHAR_MAXITEMHAVE;i++ ){
        /* 失奶  丞  毛蝈  扔□民仄化手匀化月支勾毛市它件玄允月 */
        if( ITEM_CHECKINDEX( CHAR_getItemIndex(meindex,i) ) ){
            counter++;
        }
    }
    return counter;

}

/*
 * 丐月平乓仿奶件犯弁旦及平乓仿卞｝云心六window毛钒仁凶户及
 * 树  毛芨尹月［
 * int meindex:愤坌及(饕及)奶件犯弁旦
 * int cliindex:菸及奶件犯弁旦
 */
static void NPC_SimpleShopOpenShopWindow( int meindex, int cliindex,
                                          char *npcarg )
{
    int fd;
    int objindex;

    char tmp[1024*12], itemstr[1024 * 8];

    char maincaption[SIMPLESHOPTALKBUFSIZE];
    char buycaption[SIMPLESHOPTALKBUFSIZE];
    char okbuycaption[SIMPLESHOPTALKBUFSIZE];
    char sellcaption[SIMPLESHOPTALKBUFSIZE];
    char oksellcaption[SIMPLESHOPTALKBUFSIZE];
    char poorcaption[SIMPLESHOPTALKBUFSIZE];
    char itemfullcaption[SIMPLESHOPTALKBUFSIZE];

    getStringFromIndexWithDelim( npcarg,"|",MAINTOKEN,
                                 maincaption,sizeof(maincaption) );
    getStringFromIndexWithDelim( npcarg,"|",BUYTOKEN,
                                 buycaption,sizeof(buycaption) );
    getStringFromIndexWithDelim( npcarg,"|",OKBUYTOKEN,
                                 okbuycaption,sizeof(okbuycaption) );
    getStringFromIndexWithDelim( npcarg,"|",SELLTOKEN,
                                 sellcaption,sizeof(sellcaption) );
    getStringFromIndexWithDelim( npcarg,"|",OKSELLTOKEN,
                                 oksellcaption,sizeof(oksellcaption) );
    getStringFromIndexWithDelim( npcarg,"|",POORTOKEN,
                                 poorcaption,sizeof(poorcaption));
    getStringFromIndexWithDelim( npcarg,"|",ITEMFULLTOKEN,
                                 itemfullcaption,sizeof(itemfullcaption));

    tmp[0] = 0;
    NPC_SimpleShopGetEscapedItemString( meindex, cliindex, itemstr );
    objindex = CHAR_getWorkInt( meindex,CHAR_WORKOBJINDEX );
    snprintf( tmp,sizeof(tmp),"S|%d|%s|%s|%s|"
              "%s|%s|%s|%s|"
              "-1|%d%s", objindex,
              maincaption,
              buycaption,
              okbuycaption,
              sellcaption,
              oksellcaption,
              poorcaption,
              itemfullcaption,
              NPC_SimpleShopGetItemNum( meindex ),
              itemstr );
    fd = getfdFromCharaIndex( cliindex );

    if( fd == -1 )return;

    lssproto_TK_send(fd, -1 ,tmp, CHAR_COLORWHITE );


}


/*
 * TK匹霜月凶户及  侬  毛勾仁月［
 */
static void NPC_SimpleShopGetEscapedItemString( int shopindex,
                                                int cliindex, char *str )
{
    int i, cost, cl, imageno, itemindex;
    char *itemname;
    char tmp[1024];
    char escapedname[256];
//    char *oli; /* oli means onelineinfo */

    str[0] = '\0';

    /* 引内饕及  切  及伉旦玄 */
    for( i=CHAR_STARTITEMARRAY;i<CHAR_MAXITEMHAVE;i++ ){
        itemindex=CHAR_getItemIndex( shopindex , i );
        if( ITEM_CHECKINDEX(itemindex) ){
            itemname = ITEM_getChar( itemindex, ITEM_SECRETNAME );
            makeEscapeString( itemname, escapedname, sizeof(escapedname));
            cost = ITEM_getInt( itemindex, ITEM_COST );
            /* 嗯喊及譬帮［MERCHANTLEVEL卞方匀化  歹月［ */
            cost *= NPC_Util_buyRate(cliindex);
            cl = ITEM_getInt( itemindex, ITEM_LEVEL );
            imageno = ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER );
            //oli =  ITEM_getMemo( itemindex );
#if 0
            snprintf( tmp, sizeof(tmp), "|%s|%d|%d|%d|%s",
                      escapedname, cost, cl, imageno, oli );
#endif
            snprintf( tmp, sizeof(tmp), "|%s|%d|%d|%d|",
                      escapedname, cost, cl, imageno );
            strcat( str, tmp );
        }
    }

    /* 戚卞｝菸及  切  及伉旦玄 */
    for( i=CHAR_STARTITEMARRAY;i<CHAR_MAXITEMHAVE;i++ ){
        itemindex=CHAR_getItemIndex( cliindex , i );
        if( ITEM_CHECKINDEX(itemindex) ){
            cost = ITEM_getInt( itemindex, ITEM_COST );
            /* 嗯喊及譬帮［MERCHANTLEVEL卞方匀化  歹月［ */
            cost *=NPC_Util_sellRate(cliindex);
            snprintf( tmp, sizeof(tmp), "|%d", cost );
            strcat( str, tmp );
        }else{
            /* 失奶  丞毛  匀化卅中桦赭卞反0毛中木月     */
            strcat( str, "|0" );
        }
    }

}

/*
 * 饕互  蜃卞  匀化中月失奶  丞毛  凶六月［赓渝祭凛午潸曰娄五及  
 * 卞裟少［
 */
static BOOL NPC_SimpleShopFillItem( int meindex, char *npcarg )
{
    char token[256], buf[16];
    int i,itemid, num;

    getStringFromIndexWithDelim( npcarg,"|",ITEMLISTTOKEN,token,
                                 sizeof(token) );

    for( i=1; i<=SIMPLESHOP_MAXINFINITITEM;i++ ){
        if( getStringFromIndexWithDelim( token,",",i,buf,sizeof(buf) )){
            itemid = atoi( buf );
            if( itemid == 0 ){
                NPC_Util_ReleaseHaveItemAll(meindex);
                return FALSE;
            }
            num=NPC_Util_countHaveItem(meindex,itemid);
            if( num < 0 ){
                return FALSE;/* 尕恳卅id卅升及fatal err */
            }else if( num == 0 ){ /* 手匀化卅井匀凶日  虾 */
                NPC_Util_createItemToChar( meindex,itemid, FALSE);

            }else if( num>=2 ){ /* 手切允亢及桦宁绰轮 */
                NPC_Util_RemoveItemByID(meindex,itemid,FALSE);
            }
        }else{
            break;
        }
    }

    /* 域荚仄五屯勾今木凶橇谪卞允月 */
    //NPC_SimpleShopSetLeakLevel( meindex );

    return TRUE;
}


/*
 * 赓渝祭允月［
 */
BOOL NPC_SimpleShopInit( int meindex )
{
    unsigned int mlevel;
    char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];


    /* 域荚撩  仄凶丐午反2蘸午综日卅中方丹卞允月 */
    int createindex = CHAR_getInt( meindex, CHAR_NPCCREATEINDEX );

/*    print("shopinit start\n" );*/
    if( NPC_CHECKCREATEINDEX(createindex) ){
        NPC_create[createindex].intdata[NPC_CREATETIME] = -1;
    }

    CHAR_setInt( meindex , CHAR_HP , 0 );
    CHAR_setInt( meindex , CHAR_MP , 0 );
    CHAR_setInt( meindex , CHAR_MAXMP , 0 );
    CHAR_setInt( meindex , CHAR_STR , 0 );
    CHAR_setInt( meindex , CHAR_TOUGH, 0 );
    CHAR_setInt( meindex , CHAR_LV , 0 );
    mlevel = ((100<<16)|(100<<0)); /* 晓匏2田奶玄互  丹午五及    ｝
                                    票匏2田奶玄互  月午五及    ［ */
    CHAR_setInt( meindex, CHAR_MERCHANTLEVEL, mlevel );

    CHAR_setWorkInt( meindex, CHAR_WORKSHOPCLIENTINDEX, -1 );

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPESHOP );
    CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );

    NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg));

    /* 失奶  丞  凶六月［撩  仄凶日return FALSE */
    if( ! NPC_SimpleShopFillItem( meindex,npcarg ) ){
        print( "SHOP INIT ERROR: npcarg=%s\n", npcarg );
        return FALSE;
    }

/*    print("shopinit end\n" );*/


    return TRUE;
}

/*
 * 饕它奶件玉它毛钒仁井升丹井  烂允月［
 * npcarg及中匀仇户及玄□弁件卞
 * 仇氏卞切反,仍户氏仁分今中,仁分今中卅
 * 及方丹卞俜  今木凶  侬  毛手午卞  烂允月［仇及午五及犯伉立正反","［
 *
 * 娄醒
 * msg:皿伊奶乩□互仄扎屯匀凶  侬  
 * openshopstr:npcarg及中匀仇户及玄□弁件［
 * 忒曰袄
 * 钒仁卅日TRUE, 钒井卅中卅日FALSE
 */
static BOOL NPC_SimpleShopOpenShopOrNot( char *msg, char *openshopstr )
{
    int i;
    char buf[256];

    i=1;
    while(getStringFromIndexWithDelim(openshopstr,",",i,buf,sizeof(buf))){
        if( strstr( msg, buf ) ) return TRUE;
        i++;
    }
    return FALSE;
}

/*
 * 允屯化及手切手及毛域荚摹  今木凶橇谪卞允月［
 */
#if 0
static void NPC_SimpleShopSetLeakLevel( int meindex )
{
    int i, itemindex;

    for( i=CHAR_STARTITEMARRAY;i<CHAR_MAXITEMHAVE;i++ ){
        itemindex=CHAR_getItemIndex(meindex,i);
        if( ITEM_CHECKINDEX(itemindex)){
            ITEM_setInt(itemindex,ITEM_LEAKLEVEL, 1 );
        }
    }
}
#endif

