#include "version.h"
#include "common.h"
#include "readmap.h"
#include "char.h"
#include "map_util.h"
#include "lssproto_serv.h"
#include "net.h"
#include "anim_tbl.h"

/*#define MAPEDITORTROUBLE*/
#define SPR_kmydamY CG_HIT_MARK_10 // anim_tbl.h 勗頞  踏躂堎竘ぁ  摯操啞攝袲哱

/*------------------------------------------------------------
 * 倵崹堎凝汔竣凝  茯埰堎�袸褐珝罈�  庌ぁ笢堎凝汔竣凝礎倳誑堣堎��
 * 礎倳
 *  ff          int     floor
 *  fx          int     x淢  
 *  fy          int     y淢  
 *  isfly       int       庌ぁ笢堎凝汔竣凝
 * 蒍堇偯
 *  倵崹堎      TRUE(1)
 *  倵崹埵笢    FALSE(0)
 ------------------------------------------------------------*/
BOOL MAP_walkAbleFromPoint( int ff, int fx, int fy, BOOL isfly )
{
    int map[2];

    if( !MAP_getTileAndObjData( ff,fx,fy, &map[0], &map[1] ) ){
        return FALSE;
	}

    if( isfly ){
        int i;
        for( i = 0 ; i < 2 ; i ++ )
            if( MAP_getImageInt( map[i], MAP_HAVEHEIGHT ) == TRUE  ){
                return FALSE;
			}
        return TRUE;
    }else{
        switch( MAP_getImageInt( map[1], MAP_WALKABLE ) ){
        case 0:
            return FALSE;
            break;
        case 1:
            if( MAP_getImageInt( map[0], MAP_WALKABLE ) == 1 ){
                return TRUE;
			}else{
                return FALSE;
			}
            break;
        case 2:
            return TRUE;
            break;
        default:
            return FALSE;
            break;
        }
    }

    return FALSE;
}



/*------------------------------------------------------------
 * 倵崹堎凝汔竣凝  茯埰堎��
 * 礎倳
 *  index       int     Char    ぁ摯 Index
 *  ff          int     floor id
 *  fx          int     x淢  
 *  fy          int     y淢  
 * 蒍堇偯
 *  倵笢趙源笢      TRUE(1)
 *  倵笢趙毀譫隍    FALSE(0)
 ------------------------------------------------------------*/
BOOL MAP_walkAble( int index,int ff, int fx, int fy)
{
    /*      す籤溘埵�徖K    */
    if( CHAR_getFlg( index, CHAR_ISTRANSPARENT ) )      return TRUE;

    // Arminius 7.9 Airplane ok
    if( CHAR_getInt( index, CHAR_WHICHTYPE ) == CHAR_TYPEBUS) return TRUE;
    if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT ) {
      int oyaindex = CHAR_getWorkInt( index, CHAR_WORKPARTYINDEX1);
      if( CHAR_CHECKINDEX( oyaindex)) {
        if (CHAR_getInt( oyaindex, CHAR_WHICHTYPE ) == CHAR_TYPEBUS) return TRUE;
      }
    }

    return MAP_walkAbleFromPoint( ff,fx,fy,CHAR_getFlg(index,CHAR_ISFLYING)|CHAR_getWorkInt(index,CHAR_WORKSKYWALKER) );
}

/*------------------------------------------------------------
 * 淢  禱炩擭媃趙��鼠喫摯酘々喇攝袲哱勗禜踏誑堣堎凝汔竣凝
 * 礎倳
 *  fl              int     啞痲囮
 *  x               int     x 淢  
 *  y               int     y 淢  
 * 蒍堇偯
 *  禜踏誑堣堎  TRUE
 *  禜踏誑埵笢  FALSE
 ------------------------------------------------------------*/
BOOL MAP_haveHeight( int fl, int x, int y )
{
    int     map[2];

    /*  悃蚗鏤溢↓淏禱  趙�彸�  */
    if( !MAP_getTileAndObjData( fl,x,y, &map[0], &map[1] ) )
        return FALSE;

    return MAP_getImageInt( map[1], MAP_HAVEHEIGHT );
}



/*----------------------------------------
 * 筒  ↓淏筒  撬捈摯  脾禱埰堎
 * 礎倳
 *  index   int     す籤溘摯騷璃溢蚗袲筒
 *  map     int     悃蚗鏤溢↓淏
 *  outof   BOOL    鵌資鄹��蛌�岏旓� TRUE 埵�桱� 鵌資鄹ぁ堣堎��
 * 蒍堇偯
 *  蚕溘隍↓淏  撬禱媃倜    TRUE
 *  蚕溘隍↓淏  撬禱媃埵凝埱倜  FALSE
  ----------------------------------------*/
static BOOL MAP_changeCharStatusFromMapDataAndTime( int index,
                                                    int map, BOOL outof)
{
#if 0
// 筒哱↓璃操騷喇ぁ毀窅渦埵笢
    int i;
    int offset;
    BOOL    change=FALSE;
    static struct tagStatusInteractionOfBitAndDefine{
        int     mapdataindex;
        int     charadataindex;
    }statusInteraction[]={
        {MAP_INTOPOISON,        CHAR_POISON},
        {MAP_INTOPARALYSIS,     CHAR_PARALYSIS},
        {MAP_INTOSILENCE,       CHAR_SLEEP},
        {MAP_INTOSTONE,         CHAR_STONE},
        {MAP_INTODARKNESS,      CHAR_DRUNK},
        {MAP_INTOCONFUSION,     CHAR_CONFUSION},

        {MAP_OUTOFPOISON,       CHAR_POISON},
        {MAP_OUTOFPARALYSIS,    CHAR_PARALYSIS},
        {MAP_OUTOFSILENCE,      CHAR_SLEEP},
        {MAP_OUTOFSTONE,        CHAR_STONE},
        {MAP_OUTOFDARKNESS,     CHAR_DRUNK},
        {MAP_OUTOFCONFUSION,    CHAR_CONFUSION},
    };

    if( outof == FALSE )offset = 6;
    else                offset= 0;
    for( i = 0 ; i < 6 ; i ++ ){
        int     newdata;
        newdata = MAP_getImageInt(map,statusInteraction[i+offset].
                                  mapdataindex);
        if( newdata > 0 ){
            change=TRUE;
            /*  筒  ↓淏筒摯优擭    */
            CHAR_setInt( index,
                         statusInteraction[i+offset].charadataindex,
                         CHAR_getInt(index,
                                     statusInteraction[i+offset].
                                     charadataindex ) + newdata );
        }
    }
    return change;
#else
	return FALSE;
#endif
}


/*------------------------------------------------------------
 * Map 騷穫璃哱摯 pre post禱窐  埰堎
 * 礎倳
 *  index       int     篥Й竣敁媃趙笢堎す籤溘摯騷璃溢蚗袲筒
 *  mode        BOOL    TRUE摯鄹毀��in  FALSE摯鄹毀 out 勗葡伀
 * 蒍堇偯埵媃
 ------------------------------------------------------------*/
static void MAP_dealprepostevent( int index, BOOL mode )
{
    int     map[2];
    int     i;
    int     damaged=FALSE,statuschange=FALSE;

    if( CHAR_getFlg(index,CHAR_ISFLYING) )
        /*    庌ぁ堎盓僑勗毀笰忒媃埵笢  */
        return;

    if( !MAP_getMapDataFromCharIndex( index  ,  map ) ) return;

    for( i = 0 ; i < 2 ; i ++ ){
        /*  damage */
        int damage;
        damage = MAP_getImageInt(map[i],
                                 mode ? MAP_INTODAMAGE : MAP_OUTOFDAMAGE);
        if( damage != 0 ){
            /*  譫隍↓喇倵禱諜ぁ  藯埰堎摯ぁ  蠕  躂袑窇    */
            int opt[2]={SPR_kmydamY,-damage};
            damaged=TRUE;
            CHAR_setInt(index, CHAR_HP,
                        CHAR_getInt(index,CHAR_HP) + damage );
            CHAR_complianceParameter(index);
            /*  譫隍↓喇偏崹倜操啞攝袲哱③埰    */
            CHAR_sendWatchEvent(CHAR_getWorkInt(index,CHAR_WORKOBJINDEX),
                                CHAR_ACTDAMAGE,opt,2,TRUE);
        }
        if( MAP_changeCharStatusFromMapDataAndTime(
            index,map[i], mode ? TRUE : FALSE ) )
            statuschange=TRUE;
    }
    if( damaged )       CHAR_sendStatusString(index,"M");
    if( statuschange ){
        CHAR_sendCToArroundCharacter(index);
        CHAR_sendStatusString(index,"P");
    }

}


/*----------------------------------------
 * 鼠摯淏騷鳴勗篥堎鞳勗蠙�厊戰瞿蛦薱蟜旦奕蟛旅欳糾庉蝤�xxx捈摯
 * 痀禱  藯埰堎��
 * 礎倳
 *  index       int     篥Й竣敁媃趙笢堎す籤溘摯騷璃溢蚗袲筒
 *  flooor      int     啞痲囮ID
 *  fx          int     x淢  
 *  fy          int     y淢  
 * 蒍堇偯
 *  埵媃
 ----------------------------------------*/
void MAP_preovered( int index )
{
    MAP_dealprepostevent( index, FALSE );
}

/*----------------------------------------
 * 鼠摯淏騷鳴勗篥埱倜  勗蠙�厊戰瞿�
 * 礎倳
 *  index       int     篥Й竣敁媃趙笢堎す籤溘摯騷璃溢蚗袲筒
 * 蒍堇偯
 *  埵媃
 ----------------------------------------*/
void MAP_postovered( int index )
{
    MAP_dealprepostevent( index, TRUE );
}


/*----------------------------------------
 * す籤溘摯薛堇摯悃蚗鏤禱埰迋趙邞堎
 * 礎倳
 *  fd          int
 *  charaindex  int     す籤溘摯騷璃溢蚗袲筒
 * 蒍堇偯
 *  埬      TRUE(1)
 *  謄      FALSE(0)
 ----------------------------------------*/
BOOL MAP_sendArroundCharNeedFD( int fd,int charaindex )
{
    char*   stringdata;
    int     x=CHAR_getInt(charaindex,CHAR_X);
    int     y=CHAR_getInt(charaindex,CHAR_Y);
    int     fl=CHAR_getInt(charaindex,CHAR_FLOOR);
    int     size=MAP_CHAR_DEFAULTSEESIZ;
    RECT    seekr,retr;
    seekr.x = x - (int)(size/2);
    seekr.y = y - (int)(size/2);
    seekr.width  = size;
    seekr.height = size;
#if 1
{
	int		tilesum, objsum, eventsum;
    stringdata = MAP_getChecksumFromRECT(fl,&seekr,&retr, &tilesum,&objsum,&eventsum);
    if( stringdata == NULL )
        return FALSE;

    lssproto_MC_send(fd,fl,
                    retr.x,              retr.y,
                    retr.x + retr.width, retr.y + retr.height,
                    tilesum,
                    objsum,
                    eventsum,
                    stringdata );
}
#else
    stringdata = MAP_getdataFromRECT(fl,&seekr,&retr);
    if( stringdata == NULL )
        return FALSE;

    lssproto_M_send(fd,fl,
                    retr.x,              retr.y,
                    retr.x + retr.width, retr.y + retr.height,
                    stringdata );
#endif
    return TRUE;
}


/*----------------------------------------
 * 悃蚗鏤溢↓淏禱  堎��
 * 礎倳
 *  charaindex      int     す籤溘摯騷璃溢蚗袲筒
 * 蒍堇偯
 *  埬      TRUE(1)
 *  謄      FALSE(0)
 ----------------------------------------*/
BOOL MAP_sendArroundChar(int charaindex)
{
    int fd;
    fd = getfdFromCharaIndex( charaindex );
    if( fd == -1 )return FALSE;

    return MAP_sendArroundCharNeedFD(fd, charaindex);
}
