#include "version.h"
#include <string.h>
#include "common.h"
#include "char.h"
#include "char_base.h"
#include "configfile.h"
#include "object.h"
#include "item.h"
#include "map_deal.h"
#include "npcutil.h"
#include "npc_door.h"
#include "readmap.h"
#include "npccreate.h"
#include "enemy.h"

// CoolFish: Family 2001/7/29
#include "family.h"
#include "saacproto_cli.h"

#define IS_2BYTEWORD( _a_ ) ( (char)(0x80) <= (_a_) && (_a_) <= (char)(0xFF) )

extern  struct  FM_POINTLIST fmpointlist;

#ifdef _PERSONAL_FAME	// Arminius 8.30: 模逜跺�侂驫�

// Arminius: 扂參汒咡硉熬圉
int FMAdvTbl[] = {	// 模逜簸玸 Table
	0,	// 0
	15,	// 1
	5,	// 2
	0,	// 3
	10,	// 4
	5,	// 5
	0,	// 6
	0,	// 7
	30,	// 8
	0,	// 9
	0,	// 10
	0,	// 11
	68,	// 12
	68,	// 13
	0,	// 14
	15,	// 15
	120,	// 16
	90,	// 17
	0,	// 18
	120,	// 19
	0,	// 20
	0,	// 21
	60,	// 22
	0,	// 23
	0,	// 24
	0,	// 25
	60,	// 26
	0,	// 27
	0,	// 28
	0,	// 29
	0,	// 30
	60,	// 31
	0,	// 32
	0,	// 33
	30,	// 34
	225,	// 35
	0,	// 36
	0,	// 37
	60,	// 38
	135,	// 39
	600,	// 40
	0,	// 41
	360,	// 42
	0,	// 43
	0,	// 44
	480,	// 45
	840,	// 46
	0,	// 47
	600,	// 48
	600,	// 49
	600,	// 50
	600,	// 51
	600,	// 52
	600,	// 53
	720,	// 54
	0,	// 55
	0,	// 56
	0,	// 57
	0,	// 58
	0,	// 59
	0,	// 60
	0,	// 61
	0,	// 62
	120,	// 63
	900,	// 64
	0,	// 65
	0,	// 66
	0,	// 67
	0,	// 68
	0,	// 69
	0,	// 70
	15000,	// 71
	20000,	// 72
	0,	// 73
	0,	// 74
	0,	// 75
	0,	// 76
	0,	// 77
	0,	// 78
	0,	// 79
	0,	// 80 
	1200,	// 81
	3000,	// 82
	0,	// 83
	1500,	// 84
	0, // 85
	0, // 86
	100, // 87
	0, // 88
	400, // 89
	0, // 90
	0, //91
	0, // 92
	0, // 93
	0, // 94
	0, // 95
	0, // 96
	100, // 97
	0, // 98
	0, // 99
	0, // 100
	0, //101
	0, //102
	0, //103
	0, //104
	600, // 105
	0, // 106
	100, // 107
	0, // 108
	0, // 109
	400, // 110 
	500, // 111
	1000, // 112
	100, // 113
	300, // 114
	0, // 115
	500, // 116
	15000, // 117 
	0, // 118
	0, // 119
	0, // 120
	0, // 121 
	0, // 122 
	0, // 123
	0, // 124 
	400, // 125 
	300, // 126
	0, // 127
	0, // 128
	0, // 129
	0, // 130
	0, // 131
	0, // 132
	500, // 133
};

#endif

BOOL NPC_Util_AddOneTitle( int charindex, int titleindex )
{
    int i;
    Char *c ;

    if( !CHAR_CHECKINDEX( charindex ) )return FALSE;

    c = CHAR_getCharPointer( charindex );
	// CoolFish: +1 2001/11/05
	if (!c)	return FALSE;

    for(i=0;i<CHAR_TITLEMAXHAVE;i++){
        if( c->indexOfHaveTitle[i] == -1 ){
            c->indexOfHaveTitle[i] = titleindex;
            return TRUE;
        }
    }
    return FALSE;
}


BOOL NPC_Util_HaveTitle( int charindex , int titleindex )
{
    int i;
    Char *c;
    if( !CHAR_CHECKINDEX( charindex ) )return -1;

    c = CHAR_getCharPointer( charindex );
	// CoolFish: +1 2001/11/05
	if (!c)	return FALSE;

    for(i=0;i<CHAR_TITLEMAXHAVE;i++){
        if( c->indexOfHaveTitle[i] == titleindex ){
            return TRUE;
        }
    }
    return FALSE;
}


BOOL NPC_Util_Nearby( int x1 , int y1, int x2 , int y2 )
{
    if( ABS( x1 - x2 ) <= 1 &&
        ABS( y1 - y2 ) <= 1 ){
        return TRUE;
    } else {
        return FALSE;
    }
}

BOOL NPC_Util_CharNearby(int ind1,int ind2)
{
    if( !CHAR_CHECKINDEX(ind1)) return FALSE;
    if( !CHAR_CHECKINDEX(ind2)) return FALSE;

    if( CHAR_getInt( ind1 , CHAR_FLOOR ) !=
        CHAR_getInt( ind2 , CHAR_FLOOR ) ){
        return FALSE;
    }

    return NPC_Util_Nearby( CHAR_getInt( ind1,CHAR_X),
                            CHAR_getInt( ind1,CHAR_Y),
                            CHAR_getInt( ind2,CHAR_X),
                            CHAR_getInt( ind2,CHAR_Y));

}


static struct ynset
{
    int value;
    char string[32];
}yntable[] ={
    { 0 , "no" },
    { 0 , "No" },
    { 0 , "NO" },
    { 0 , "NO" },
    { 0 , "No" },
    { 0 , "no" },
    { 0 , "瘁" },
    { 0 , "瘁" },

    { 1 , "yes" },
    { 1 , "Yes" },
    { 1 , "YES" },
    { 1 , "YES" },
    { 1 , "Yes" },
    { 1 , "yes" },
    { 1 , "岆" },
    { 1 , "岆" },

};

int NPC_Util_YN(char *input )
{
    int i;
    for(i=0;;i++){
        int j;

        if( input[i] == '\0' ){
            return -1;
        }
        for(j=0;j<arraysizeof(yntable);j++){
            if( input[i] == yntable[j].string[0] &&
                strncmp( input+i, yntable[j].string,
                         strlen(yntable[j].string))==0){
                return yntable[j].value;
            }
        }
    }
    return -1;
}


int NPC_Util_getDirFromTwoPoint( POINT* pstart, POINT* pend )
{
    /*  y*x */
    static int     dirtable[3][3]={
        {7, 0, 1},
        {6,-1, 2},
        {5, 4, 3},
    };

    int difx, dify;
    difx = pend->x - pstart->x;
    dify = pend->y - pstart->y;

    if( difx < 0 )difx=-1;
    else if( difx > 0 )difx=1;
    if( dify < 0 )dify=-1;
    else if( dify > 0 )dify=1;

    return dirtable[dify+1][difx+1];
}

int NPC_Util_countHaveItem( int meindex , int itemid )
{
    int i, count=0, itemindex;
    if( !CHAR_CHECKINDEX(meindex))return -1;
     for( i=0 ; i<CHAR_MAXITEMHAVE ; i++ ){
        itemindex = CHAR_getItemIndex( meindex , i );
        if( !ITEM_CHECKINDEX( itemindex)) continue;
		if( ITEM_getInt( meindex, ITEM_ID) == itemid ) count++;
    }
    return count;
}

BOOL NPC_Util_isBackContact( int frontindex , int backindex )
{
    int dir;
    int x,y;

    dir = (CHAR_getInt( frontindex , CHAR_DIR ) + 4 ) % 8;
    VALIDATEDIR(dir);
    CHAR_getCoordinationDir( dir ,
                             CHAR_getInt( frontindex , CHAR_X ),
                             CHAR_getInt( frontindex , CHAR_Y ),
                             1 ,
                             &x , &y );
    if( x == CHAR_getInt( backindex , CHAR_X )
        && y == CHAR_getInt( backindex , CHAR_Y )
        && CHAR_getInt( backindex , CHAR_DIR )
        == CHAR_getInt( frontindex , CHAR_DIR ) )
        return TRUE;
    else
        return FALSE;
}


BOOL NPC_Util_isFaceToFace( int index1, int index2, int distance )
{
    int i;
    if(CHAR_getInt(index1,CHAR_FLOOR) != CHAR_getInt(index2,CHAR_FLOOR)){
        return FALSE;
    }
    if(CHAR_getInt(index1,CHAR_X) == CHAR_getInt(index2,CHAR_X )
       && CHAR_getInt(index1,CHAR_Y) == CHAR_getInt(index2,CHAR_Y) ){
        return FALSE;
    }
    for( i=1;i<=distance;i++ ){
        int walked_x, walked_y;
        CHAR_getCoordinationDir( CHAR_getInt(index1,CHAR_DIR)
                                 , CHAR_getInt(index1,CHAR_X)
                                 , CHAR_getInt(index1,CHAR_Y)
                                 ,i, &walked_x, &walked_y );
        if( CHAR_getInt(index2,CHAR_X) == walked_x
            && CHAR_getInt(index2,CHAR_Y) == walked_y ){
            CHAR_getCoordinationDir( CHAR_getInt(index2,CHAR_DIR)
                                     , CHAR_getInt(index2,CHAR_X)
                                     , CHAR_getInt(index2,CHAR_Y)
                                     ,i, &walked_x, &walked_y );
            if( CHAR_getInt(index1,CHAR_X) == walked_x
                && CHAR_getInt(index1,CHAR_Y) == walked_y ){
                return TRUE;
            }else{
                return FALSE;
            }
        }
    }
    return FALSE;
}
#ifdef _NPCSERVER_NEW
BOOL NPC_UtilObj_isFaceToFace( int npcobjindex, int objindex, int distance )
{
    int i;
	int npc_floor, npc_x, npc_y, npc_dir;
	npc_floor = OBJECT_getFloor( npcobjindex);
	npc_x = OBJECT_getX( npcobjindex);
	npc_y = OBJECT_getY( npcobjindex);
	npc_dir = OBJECT_getNpcdir( npcobjindex);

    if( OBJECT_getFloor( objindex) != npc_floor )
        return FALSE;
    if( OBJECT_getX( objindex) == npc_x && OBJECT_getY( objindex) == npc_y )
        return FALSE;

    for( i=1; i<=distance; i++ ){
        int walked_x, walked_y;
            CHAR_getCoordinationDir( OBJECT_getNpcdir( objindex)
                                     , OBJECT_getX( objindex), OBJECT_getY( objindex)
                                     , i, &walked_x, &walked_y );

            if( npc_x == walked_x && npc_y == walked_y ){
                return TRUE;
            }else{
                return FALSE;
            }
    }
    return FALSE;
}
#endif

BOOL NPC_Util_isFaceToChara( int index1, int index2, int distance )
{
    int i;
    if(CHAR_getInt(index1,CHAR_FLOOR) != CHAR_getInt(index2,CHAR_FLOOR)){
        return FALSE;
    }
    if(CHAR_getInt(index1,CHAR_X) == CHAR_getInt(index2,CHAR_X )
       && CHAR_getInt(index1,CHAR_Y) == CHAR_getInt(index2,CHAR_Y) ){
        /* 凝踏埵埱趙笢倜�帴ALSE */
        return FALSE;
    }

    for( i=1;i<=distance;i++ ){
        /* 竘囀��笢埱喫誧摯す籤溘誑倵笢倜  摯淢  禱滑誧堎 */
        int walked_x, walked_y;
        CHAR_getCoordinationDir( CHAR_getInt(index1,CHAR_DIR)
                                 , CHAR_getInt(index1,CHAR_X)
                                 , CHAR_getInt(index1,CHAR_Y)
                                 ,i, &walked_x, &walked_y );
        if( CHAR_getInt(index2,CHAR_X) == walked_x
            && CHAR_getInt(index2,CHAR_Y) == walked_y )
		{
            return TRUE;
        }else{
            return FALSE;
        }
    }
    /* 殽庌殽庌福埵�欷汙朗�倜�帴ALSE */
    return FALSE;
}

BOOL NPC_Util_charIsInFrontOfChar( int index1, int index2, int distance )
{
    int i;
    if(CHAR_getInt(index1,CHAR_FLOOR) != CHAR_getInt(index2,CHAR_FLOOR)){
        return FALSE;
    }
    if(CHAR_getInt(index1,CHAR_X) == CHAR_getInt(index2,CHAR_X )
       && CHAR_getInt(index1,CHAR_Y) == CHAR_getInt(index2,CHAR_Y) ){
        return FALSE;
    }

    for( i=1;i<=distance;i++ ){
        int walked_x, walked_y;
        CHAR_getCoordinationDir( CHAR_getInt(index1,CHAR_DIR)
                                 , CHAR_getInt(index1,CHAR_X)
                                 , CHAR_getInt(index1,CHAR_Y)
                                 ,i, &walked_x, &walked_y );
        if( CHAR_getInt(index2,CHAR_X) == walked_x
            && CHAR_getInt(index2,CHAR_Y) == walked_y ){
            return TRUE;
        }
    }
    return FALSE;
}

void NPC_Util_AnnounceFloor( int floorid , char *msg )
{
    int i;
    int playernum = CHAR_getPlayerMaxNum();

    for(i=0;i<playernum;i++){
        if( CHAR_getCharUse(i) &&
            CHAR_getInt( i , CHAR_WHICHTYPE)  == CHAR_TYPEPLAYER  &&
            CHAR_getInt( i , CHAR_FLOOR ) == floorid  ){
            CHAR_talkToCli( i  , -1 , msg , CHAR_COLORWHITE );
        }
    }
}

BOOL NPC_Util_moveItemToChar( int charindex, int itemindex, BOOL net )
{
    int emptyindex, oind ,cind;
    emptyindex = CHAR_findEmptyItemBox( charindex );
    if(  emptyindex < 0 )
        return FALSE;

    if( !ITEM_CHECKINDEX( itemindex ) )return FALSE;
    oind = ITEM_getWorkInt( itemindex , ITEM_WORKOBJINDEX );
    cind = ITEM_getWorkInt( itemindex , ITEM_WORKCHARAINDEX );

    if( oind >= 0 ){
        CHAR_ObjectDelete( oind );
        CHAR_setItemIndex( charindex, emptyindex, itemindex );
        ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1 );
        ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, charindex );
        if( net ) CHAR_sendItemDataOne( charindex, emptyindex);
        return TRUE;
    }else if( cind >= 0 ){
        int	itemgrp[2];
        int itemindexinchara;
        if( cind == charindex )return FALSE;
        itemindexinchara = NPC_Util_SearchItemInChar( cind , itemindex);
        if( itemindexinchara == -1 )return FALSE;
        CHAR_setItemIndex( cind , itemindexinchara , -1 );
        CHAR_complianceParameter(cind);
		itemgrp[0] = itemindexinchara;
        CHAR_setItemIndex( charindex , emptyindex , itemindex );
		itemgrp[1] = emptyindex;
        if( net )  CHAR_sendItemData( charindex, itemgrp, arraysizeof(itemgrp));
        ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1 );
        ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, charindex );
        return TRUE;
    }
    return FALSE;
}

BOOL NPC_Util_moveItemToMap( int itemindex , int fl , int x , int y,
                             BOOL net )
{
    int oind,cind;

    if( !ITEM_CHECKINDEX( itemindex ) ) return FALSE;

    oind = ITEM_getWorkInt( itemindex , ITEM_WORKOBJINDEX );
    cind = ITEM_getWorkInt( itemindex , ITEM_WORKCHARAINDEX );

    if( oind >= 0 ){
        return MAP_objmove( oind , OBJECT_getFloor(oind),
                     OBJECT_getX(oind),
                     OBJECT_getY(oind),
                     fl,x,y);
    } else if( cind >= 0 ){
        int itemindexinchara = NPC_Util_SearchItemInChar(cind,itemindex);
        if( itemindexinchara == -1 ) return FALSE;

        CHAR_setItemIndex( cind , itemindexinchara , -1 );
        CHAR_complianceParameter(cind);
        if( net ) CHAR_sendItemDataOne( cind, itemindexinchara);
        if( CHAR_DropItemAbsolute( itemindex, fl,x,y,TRUE) < 0 ){
            return FALSE;
        } else {
            return TRUE;
        }
    }
    return FALSE;
}

int NPC_Util_GiveAllItemToChar( int give , int take )
{
    int i ;
    int count=0;

    for(i=0;i<CHAR_MAXITEMHAVE;i++){
        int itemindex = CHAR_getItemIndex( give , i );
        BOOL aho = NPC_Util_moveItemToChar( take , itemindex,FALSE );
        if( aho ){
            count++;
        }
    }
    if( count ){
        CHAR_sendStatusString( give,"I");
        CHAR_sendStatusString( take,"I");
    }
    return count;
}

BOOL NPC_Util_createItemToChar( int charindex, int itemid , BOOL net)
{
    int emptyitemindexinchara, itemindex;
    emptyitemindexinchara = CHAR_findEmptyItemBox( charindex );

    if( emptyitemindexinchara < 0 )return FALSE;
	itemindex = ITEM_makeItemAndRegist( itemid );
    if( itemindex != -1 ){
        CHAR_setItemIndex( charindex, emptyitemindexinchara, itemindex );
        ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX,-1);
        ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX,charindex);

        if(net) {
        	CHAR_sendItemDataOne( charindex, emptyitemindexinchara);
        }
        return TRUE;
    }
    return FALSE;
}

int NPC_Util_CharDistance( int index1, int index2 )
{
    int dx, dy;

    if( CHAR_getInt(index1,CHAR_FLOOR)!= CHAR_getInt(index2,CHAR_FLOOR) ){
        return 0x7fffffff;
    }

    dx  =ABS(  CHAR_getInt(index1,CHAR_X)- CHAR_getInt(index2,CHAR_X) );
    dy  =ABS(  CHAR_getInt(index1,CHAR_Y)- CHAR_getInt(index2,CHAR_Y) );

    return max( dx, dy );

}

static int  SearchNearLine(
	int xStart,
	int yStart,
	int floor,
	int xPlus,
	int yPlus,
	int Count,
	int type
){
	OBJECT object;
	int i, x = xStart, y = yStart;

	for( i = 0; i < Count; i ++ ){
		for( object=MAP_getTopObj( floor, x,y ); object ;
			object = NEXT_OBJECT(object) )
		{
			int objindex = GET_OBJINDEX(object), iWork;
    		if( OBJECT_getType(objindex) != OBJTYPE_CHARA )continue;
    		iWork = OBJECT_getIndex(objindex);
    		if( CHAR_getInt(iWork,CHAR_HP) < 0 )continue;
			if( CHAR_getInt( iWork,CHAR_WHICHTYPE ) == type ){
				return iWork;
			}
		}
		x += xPlus;	y += yPlus;
	}
	return -1;
}


static int SearchNearAround(
	int x, 	   /* 號喙  刳摯  淢   */
	int y,     /* 號喙  刳摯  淢   */
	int floor, /* 號喙  刳摯啞痲囮 */
	int Part,  /* 號喙楣鴽埰堎  疐摯溘騷璃 */
	int Level, /*   刳源堇  躂趙笢堎嬾   */
	int type   /* 號喙埰堎 CHAR 淏騷鏤 */
){
	int i, iTarget = -1;
	for( i = 0; i < 4; i ++ ){
		if( Part == 0 ){ /* 窀摯溘騷璃禱號喙 */
			iTarget = SearchNearLine( x-Level+1, y-Level,
				floor, 1, 0, Level*2, type );
			if( iTarget >= 0 )break;
		}else
		if( Part == 1 ){ /* 蒟摯溘騷璃禱號喙 */
			iTarget = SearchNearLine( x+Level, y-Level+1,
				floor, 0, 1, Level*2, type );
			if( iTarget >= 0 )break;
		}else
		if( Part == 2 ){ /* き摯溘騷璃禱號喙 */
			iTarget = SearchNearLine( x+Level-1, y+Level,
				floor, -1, 0, Level*2, type );
			if( iTarget >= 0 ) break;
		}else
		if( Part == 3 ){ /* 嫌摯溘騷璃禱號喙 */
			iTarget = SearchNearLine( x-Level, y+Level-1,
				floor, 0, -1, Level*2, type );
			if( iTarget >= 0 ) break;
		}
		Part ++; Part &= 3; /* べ摯蚕↓哱禱號喙 */
	}
	return iTarget;
}

int NPC_Util_SearchNear( int meindex, int maxlen, int type )
{
#if 1
	/* MAP 凝�梩啡虃�袲ぁ號喙埰堎源竣勗  脾 */
    int	floor, x, y, i,iTarget = -1;
	/* 藯凜摯猷覕摯攷  禱礎拻③埰 */
    if( !CHAR_CHECKINDEX( meindex ) )return -1;
    floor = CHAR_getInt( meindex, CHAR_FLOOR );
    x  = CHAR_getInt( meindex, CHAR_X );
    y  = CHAR_getInt( meindex, CHAR_Y );

	/*       ⑹  ぁぅ迋堎 */
	for( i = 0; i < maxlen; i ++ ){
		iTarget = SearchNearAround( x, y, floor, RAND( 0,3 ), i+1, type );
		if( iTarget >= 0 )break;
	}
	return iTarget;


#else
	/* 喫з�梫棋譆倗忍�   */
    int	floor, x, y, i,iTarget = -1, iLen, iMin = 655360, tX, tY;
    /*   倳斛蚗哱*/
    if( !CHAR_CHECKINDEX( meindex ) )return -1;
    floor = CHAR_getInt( meindex, CHAR_FLOOR );
    x  = CHAR_getInt( meindex, CHAR_X );
    y  = CHAR_getInt( meindex, CHAR_Y );

    /*  號喙 */
    for( i = 0 ; i<objnum ; i++ ){
        /*す籤溘袲淏↓摯鄹*/
        if( obj[i].type != OBJTYPE_CHARA ) continue;
		/* 攣  禱峟堎*/
        if( CHAR_getInt( obj[i].index,CHAR_WHICHTYPE)
            != type ) continue;
		/*   啋啞痲囮摯鄹*/
	    if( floor != obj[i].floor ) continue;

	    /* CHAR_TYPEENEMY 摯鄹毀index誑猷覕摯鳹譴誑堣堎摯ぁ鏍攝蚗袲埰堎 */
        if( obj[i].index == meindex ) continue;

		/*     摯嬾  禱銓猀*/
		tX = ABS( x-CHAR_getInt(obj[i].index,CHAR_X) );
		tY = ABS( y-CHAR_getInt(obj[i].index,CHAR_Y) );
		if( tX > maxlen )continue;	/*   躂埰蕩*/
		if( tY > maxlen )continue;	/* 埵�梪摀�*/

        /* 備凝笢嬾  禱銓猀*/
		iLen = tX*tX+tY*tY;
		/* 郖  閟凝埱倜鳹譴毀楔  脾犍*/
		if( iMin > iLen ){
			iMin = iLen;
			iTarget = obj[i].index;	/* 騷璃溢蚗袲筒禱頞  */
		}
	}

	return iTarget;
#endif
}
/*------------------------------------------------------------
* 郖  閟笢鏤畛騷媕↓摯號喙
------------------------------------------------------------*/
int NPC_Util_SearchNearPlayer( int meindex, int maxlen )
{
	return( NPC_Util_SearchNear( meindex, maxlen, CHAR_TYPEPLAYER));
}
/*------------------------------------------------------------
* 郖  閟笢傍摯號喙-
-----------------------------------------------------------*/
int NPC_Util_SearchNearEnemy( int meindex, int maxlen )
{
	return( NPC_Util_SearchNear( meindex, maxlen, CHAR_TYPEENEMY));
}



/*******************************************************

  梓誧遉  埰堎鞳勗笰凝勗屾僑凝埱趙笢堎凝汔竣凝ぅ迋堎
  int NPC_Util_SuberiWalk(
	int	index,	CHAR 摯騷璃溢蚗袲筒
	int	dir		倵�抎鱺�
  );

    堇偯  0  7※倵崹堎  澺
          -1  ※悃蚗鏤勗礎埱凝凝埱趙操溘↓
          -2  ※す籤溘袲淏勗礎埱凝凝埱趙操溘↓
          -3  ※    勗礎埱凝凝埱趙操溘↓

  梓誧遉  埰堎鳹譴竘囀喳��莉勗遉  ぁ拻堎凝ぅ迋堎��
      遉  ぁ拻躂�伀篕岔姻鯆慪濛邴彸瞿�    媃凝遉  ぁ拻埵崹躂��
  鼠з�梩�  澺禱蒍埰��

 鼠喫勗笰凝堣埱倜��  悃蚗鏤雄翻  兒禱蒍埰
*******************************************************/
int NPC_Util_OtherCharCheck( int ff, int fx, int fy)
{
#if 1
	/* 號喙  祪禱      凝�梩啡虃�袲勗  脾 */
	OBJECT object;
	for( object=MAP_getTopObj(ff,fx,fy); object ;
		object = NEXT_OBJECT(object) )
	{
		int objindex = GET_OBJINDEX(object);
		/*   啋淢  勗す籤溘袲淏誑堣堎凝 */
    	if( OBJECT_getType(objindex) == OBJTYPE_CHARA ){
    		return 1;
		}
	}
	return 0;
#else
	/* 喫з�梫棹嗒蕉啄忍�   */
    int i;
    for( i = 0 ; i < objnum ; i ++){
		/* 淢  誑  啋凝ㄑ*/
        if( obj[i].x == fx && obj[i].y == fy && obj[i].floor == ff ){
        	/* す籤溘袲淏凝ㄑ*/
	        if( obj[i].type != OBJTYPE_NOUSE ){
	            return 1;
            }
        }
    }
	return 0;
#endif
}


/* 倵崹埵笢鳹譴毀遢埱趙陑堎*/
int NPC_Util_SuberiWalk(
	int	index,
	int	dir
)
{
	int	i, OkFlg = 0, x = 0, y = 0, fl = 0;
	int aDirList[3],dir2;
	int tX, tY;

    x  = CHAR_getInt(index,CHAR_X);
    y  = CHAR_getInt(index,CHAR_Y);
    fl = CHAR_getInt(index,CHAR_FLOOR);

	/* 梓誧  澺凝ㄑ*/
	switch( dir ){
	case 1: aDirList[0] = 0; aDirList[1] = 2; aDirList[2] = 1;
		break;
	case 3: aDirList[0] = 2; aDirList[1] = 4; aDirList[2] = 3;
		break;
	case 5: aDirList[0] = 4; aDirList[1] = 6; aDirList[2] = 5;
		break;
	case 7:	aDirList[0] = 6; aDirList[1] = 0; aDirList[2] = 7;
		break;
	default:
		tX = x+CHAR_getDX(dir);
		tY = y+CHAR_getDY(dir);
		/* 鵌誧倜*/
		if( MAP_walkAble( index,fl,tX,tY ) == 0 ){
			dir = -1;/* 悃蚗鏤勗礎埱凝凝埱倜*/
		}
//		}else
//		if( NPC_Util_OtherCharCheck( fl,tX, tY ) ){
//			dir = -2;/* す籤溘勗礎埱凝凝埱倜*/
//		}
		/* 竘埱埰妦煦!! 鼠摯竘竘倵��*/
/*		CHAR_walk( index, dir, 0 );*/
		return dir;
	}

	/* 竘埱埰妦埵��  擭埵媃*/
	for( i = 0; i < 3; i ++ ){
		dir2 = aDirList[i];
		tX = x+CHAR_getDX(dir2);
		tY = y+CHAR_getDY(dir2);
		if( MAP_walkAble( index,fl,tX,tY ) == 0 ){
			aDirList[i] = -1; /* 菜崹埵笢鳹譴毀鼠摯  澺詌睍*/
//		}else if( NPC_Util_OtherCharCheck( fl,tX, tY ) ){
//			aDirList[i] = -2; /* 菜崹埵笢鳹譴毀鼠摯  澺詌睍*/
		}else{
			/* 菜崹倜!!*/
			OkFlg ++;
		}
	}

	/* 兩僑蠈  菴  埵鳹譴毀��  疐摯玊堇  ��*/
	if( aDirList[0] >= 0 && aDirList[1] >= 0 && aDirList[2] >= 0 ){
	}else
        /* 喫з�梇�  崹堎凝ㄑ*/
	if( aDirList[0] >= 0 ){
		dir = aDirList[0];
	}else
        /* 鼠躂敁忒喫з�桴恐�*/
	if( aDirList[1] >= 0 ){
		dir = aDirList[1];
	}else{
		dir = aDirList[0]+aDirList[1];
	}

	/* 遉  */
/*	CHAR_walk( index, dir, 0 );*/

	return dir;
}
/*-----------------------------------------------------------------------
* NPC摯create鄹摯礎倳摯  凝�梤黃羸鯆憶�  棬  禱號喙媃��
* 鼠摯  摯倳偯禱噁  埰堎�詍鬊鬷刳鉏�ぁ倳偯禱炩擭踏鞠趙盓堇倜笢敁拻勗檎媋
*
* 曛徽※溢惉蕾淏毀"|"勗藦擭媃趙竘埰��
*       礎倳勗  啋  棬  誑兕僑嶈庌煦鳹譴��
*       袸摯  誑騅袸踏躂竘埰��
* ----------------------------------
* IN : int meindex : す籤溘index
*    : char* in    : 號喙埰堎  棬  
* OUT:
* RETURN:
* 噁  ③  埵凝埱倜鄹 :-1
* 噁  ③  倜鄹       :倳偯
* ----------------------------------
*
*   ※
* [File:enemykill.create]
* enemy=enemykill|range:4
*
* call:
* range = NPC_Util_GetNumFromArg( index, "range");
*----------------------------------------------------------------------*/
int NPC_Util_GetNumFromArg( int meindex, char* in)
{
	BOOL    rc;
	int     i;
	char    outstr[sizeof( STRING32)];
	int		out = -1;
	char 	*a = CHAR_getChar( meindex, CHAR_NPCARGUMENT);

	for( i = 1; ; i++ ) {
		/* 溢惉蕾淏兒僑囀僑  資 */
		rc = getStringFromIndexWithDelim( a, "|", i, outstr, sizeof(outstr));
		if( rc == FALSE) break;
		if( strstr( outstr, in) != NULL ) {
			out = textToInt( outstr);
			break;
		}
	}

	return out;
}

/*
 * 囮騷  堜摯 index 敁す籤溘 index 摯謂倜僑摯攷  凝�掁�
 * す籤溘誑鼠摯囮騷  堜禱  埱趙堎凝汔竣凝媃�梠芼�  埱趙倜�桾�摯
 * 痾  勗  埱趙堎凝禱蒍埰��  囮騷  堜匴  摯NPC暵�羡HAR_勗遉菜
 * ぁ拻堎凝忒媃躂庌匴倳圖惉
 * by ringo
 *
 * int charindex : 葡濯す籤溘摯 index
 * int itemindex : 囮騷  堜摯index
 *
 * 蒍堇偯
 *
 *  >=0 : 汔摯痾  勗  埱趙笢堎摯凝��  嗚埵偯��
 *  -1 : 忒媃�妢�  埱趙埵笢��
 *  -2 : 操溘↓��
 */
int NPC_Util_SearchItemInChar( int charindex , int itemindex)
{
    int i;

    if( !CHAR_CHECKINDEX(charindex) ) return -2;
    if( !ITEM_CHECKINDEX(itemindex) ) return -2;

    for(i=0;i<=CHAR_MAXITEMHAVE;i++){
        int ii = CHAR_getItemIndex( charindex , i );
        if( ii == itemindex ){
            return i;
        }
    }
    return -1;
}


/*------------------------------------------------------------------------
* す籤溘凝�桸鹹趼側副褐珝翹�  澺禱滑誧堎
*
* int NPC_Util_GetDirCharToChar( int fromindex, int toindex, int mode)
*
* int	fromindex 	喫摯す籤溘index凝��
* int	toindex		喫摯す籤溘index勗葡媃趙摯  澺誑滑誧�桫戰�
* int	mode		噁  埰堎  澺
*                   0:toindex勗澺凝埱趙
*                   1:toindex摯  葡  澺
*                   2:toindex勗葡媃趙蒟刻兔梣  澺
*                   3:toindex勗葡媃趙嫌刻兔梣  澺
*
* 蒍堇偯 埬   ※dir    澺禱蒍埰  
*        謄   ※-1
------------------------------------------------------------------------*/
int NPC_Util_GetDirCharToChar( int fromindex, int toindex, int mode)
{
	int	ret = -1;

	if( CHAR_getInt( fromindex, CHAR_FLOOR)
		== CHAR_getInt( toindex, CHAR_FLOOR))
	{
		POINT	start,end;
		int		dir;
		start.x = CHAR_getInt( fromindex, CHAR_X);
		start.y = CHAR_getInt( fromindex, CHAR_Y);
		end.x = CHAR_getInt( toindex, CHAR_X);
		end.y = CHAR_getInt( toindex, CHAR_Y);
		dir = NPC_Util_getDirFromTwoPoint( &start,&end );
		if( mode == 1 )		dir = (dir > 3)? dir -4 : dir +4;
		else if( mode == 2) dir = (dir + 2) % 8;
		else if( mode == 3) dir = (dir < 1)? dir +6 : dir -2;
		ret = dir;
	}
	return( ret);
}

/*------------------------------------------------------------------------
*     す籤溘勗葡媃趙�楉夭�  澺勗遉  埰堎��
*
* int NPC_Util_WalkCharToChar( int fromindex, int toindex, int mode)
*
* int	fromindex 	倵�岊褐珝檯ndex
* int	toindex		倵��    摯す籤溘index
* int	mode		倵��  澺
*                   0:toindex勗澺凝埱趙
*                   1:toindex摯  葡  澺
*                   2:toindex勗葡媃趙蒟刻兔梣  澺
*                   3:toindex勗葡媃趙嫌刻兔梣  澺
* BOOL	suberi		NPC_Util_SuberiWalk禱萳  埰堎凝ㄑTRUE:埰堎 FALSE:媃埵笢
*
* 蒍堇偯 埬    倵笢倜  ※dir  倵笢倜  澺禱蒍埰  
*        謄            ※-1
------------------------------------------------------------------------*/
int NPC_Util_WalkCharToChar( int fromindex, int toindex, int mode, int suberi)
{
	int	ret = -1;
	int	dir;

	if( CHAR_getInt( fromindex, CHAR_FLOOR)
		!= CHAR_getInt( toindex, CHAR_FLOOR))
	{
		return( -1);
	}
	dir = NPC_Util_GetDirCharToChar( fromindex, toindex, mode );
	if( dir != -1) {
		if( suberi) {
			dir = NPC_Util_SuberiWalk( fromindex, dir );
		}
	}
	if( dir >= 0 ) {
		ret = CHAR_walk(fromindex,dir,0);
		if( ret == CHAR_WALKSUCCESSED ) ret = dir;
		else							ret = -1;
	}
	return( ret);
}


/*------------------------------------------------------------------------
* NPC禱觛  踏鞠堎��
*
* void NPC_Util_NPCDelete( int srcindex)
*
* int	srcindex 	觛埰index
*
------------------------------------------------------------------------*/
void NPC_Util_NPCDelete( int srcindex)
{
    //int objindex;
    //objindex = CHAR_getWorkInt(srcindex,CHAR_WORKOBJINDEX);

	/* createnum 誑    摯鄹�椿煉�躂倜鄹蹬禱掛蚗哱埰堎
	 * 埵殽喫喫ぁ喫庌埵痀禱媃趙笢堎凝敁笢竣敁
	 * create啞栝騷鳴摯time偯誑牁竘躂趙凝�桯孩噸笭縍郱勛�趙笢堎敁��
	 * 憪葡勗埰妦勗牁竘躂趙�彸蟲誸迮凶�  禱埵庌敁凝媃源竣敁埰堎諱
	 */
	if( CHAR_getInt( srcindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) return;
	if( NPC_createCheckMaxEnemynum( CHAR_getInt(srcindex,CHAR_NPCCREATEINDEX )))
		NPC_createInitTime( CHAR_getInt(srcindex,CHAR_NPCCREATEINDEX ));
    NPC_createDecreaseEnemynum( CHAR_getInt(srcindex,
                                            CHAR_NPCCREATEINDEX ) );

	CHAR_CharaDelete( srcindex );

}

char *NPC_Util_CheckAssignArgFile( int index, char *filename)
{
	BOOL    rc;
	int     i;
	char	*cret = NULL;
	char    outstr[64];
	char    outstr2[64];
	char 	*a = CHAR_getChar( index , CHAR_NPCARGUMENT );

	if( a == NULL ) {
		print( "%s : %d err\n", __FILE__, __LINE__);
		return cret;
	}

	for( i = 1; ; i++ ) {
		/* 溢惉蕾淏兒僑囀僑  資 */
		rc = getStringFromIndexWithDelim( a, "|", i, outstr, sizeof(outstr));
		if( rc == FALSE) break;
		if( strstr( outstr, "file") != NULL ) {
			rc = getStringFromIndexWithDelim( outstr, ":", 2,
											outstr2, sizeof(outstr2));
			if( rc != FALSE ) {
				strcpy( filename, outstr2);
				cret = filename;
				break;
			}
		}
	}
	return( cret);
}

static char *NPC_Util_MargeStrFromArgFile( char *filename, char *buf, int len)
{
	FILE	*fp;
	char	*cret = NULL;
	char	line[NPC_UTIL_GETARGSTR_LINEMAX];
	char	opfile[32];

	sprintf( opfile, "%s/", getNpcdir( ) );
	strcat( opfile, filename);
	*buf = '\0';
	fp = fopen( opfile, "r");
	if( fp != NULL ) {
		while( fgets( line, sizeof( line), fp)) {
			if( strlen(buf) != 0 ) {
				if( buf[strlen(buf) -1] != '|' ) {
					strcatsafe( buf, len, "|");
				}
			}
			chompex( line);
			strcatsafe( buf,len,  line);
		}
		fclose( fp);
		cret=buf;
	}
	return( cret);
}

char *NPC_Util_GetArgStr( int index, char *argstr, int len)
{
	char	filename[128];
	char	*cret = NULL;
	if( NPC_Util_CheckAssignArgFile( index, filename) != NULL ) {
		if( NPC_Util_MargeStrFromArgFile( filename, argstr, len) != NULL ) {
			cret = argstr;
		}
	}else {
		if( CHAR_getChar( index, CHAR_NPCARGUMENT) != NULL ){
			strcpy( argstr, CHAR_getChar( index, CHAR_NPCARGUMENT));
			cret = argstr;
		}else{
			cret = NULL;
		}
	}
	if( cret == NULL ) print("File:%s\n", NPC_Util_CheckAssignArgFile( index, filename));

	return( cret);
}

int NPC_Util_GetNumFromStrWithDelim( char *srcstr, char* in)
{
	BOOL    rc;
	int     i;
	char    outstr[32];
	int		out = -1;

	for( i = 1; ; i++ ) {
		/* 溢惉蕾淏兒僑囀僑  資 */
		rc = getStringFromIndexWithDelim( srcstr, "|", i, outstr, sizeof(outstr));
		if( rc == FALSE) break;
		if( strstr( outstr, in) != NULL ) {
			out = textToInt( outstr);
			break;
		}
	}

	return out;
}

char *NPC_Util_GetStrFromStrWithDelim( char *srcstr, char *srhstr,
                                       char *buf, int buflen)
{
	BOOL    rc;
	char	*cret = NULL;
	int     i;
	char    outstr[1024];
	char    outstr2[1024];

	memset( outstr, 0, sizeof( outstr));
	memset( outstr2, 0, sizeof( outstr2));
	buf[0] = '\0';
	for( i = 1; ; i++ ) {
		rc = getStringFromIndexWithDelim( srcstr, "|", i, outstr, sizeof(outstr));
		if( rc == FALSE) break;
		if( strstr( outstr, srhstr) != NULL) {
			rc = getStringFromIndexWithDelim( outstr, ":", 2, outstr2, sizeof(outstr2));
			if( rc != FALSE ) {
				strcpysafe( buf, buflen, outstr2 );
				cret = buf;
				break;
			}
		}
	}
	return cret;
}

inline double NPC_Util_sellRate( int seller )
{
   	return (CHAR_getInt( seller, CHAR_MERCHANTLEVEL)&0x0000ffff)*0.01;
}

inline double NPC_Util_buyRate( int buyer )
{
    return (CHAR_getInt( buyer, CHAR_MERCHANTLEVEL )>>16)*0.01;
}

BOOL NPC_Util_IsVisiblePlayer( int meindex)
{
	int		chr_fl,chr_x,chr_y, i, j;
	BOOL	found = FALSE;

	chr_fl = CHAR_getInt( meindex, CHAR_FLOOR);
	chr_x  = CHAR_getInt( meindex, CHAR_X);
	chr_y  = CHAR_getInt( meindex, CHAR_Y);

    for( i = chr_x-CHAR_DEFAULTSEESIZ/2 ;
    	i <= chr_x+CHAR_DEFAULTSEESIZ/2 && found == FALSE; i++ ){
        for( j = chr_y-CHAR_DEFAULTSEESIZ/2 ;
        	j <= chr_y+CHAR_DEFAULTSEESIZ/2 && found == FALSE;
             j ++ ){
            OBJECT  object;
			for( object = MAP_getTopObj( chr_fl, i,j);
				object;
				object = NEXT_OBJECT(object) )
			{
                int objindex = GET_OBJINDEX( object);
                if( OBJECT_getType(objindex) == OBJTYPE_CHARA ){
                	if( CHAR_getInt( OBJECT_getIndex(objindex),
                                 CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER )
					{
						found = TRUE;
						break;
					}
				}
			}
		}
	}
	return( found);
}

BOOL NPC_Util_WordInclude( char *text , char *word )
{
    if( strstr( text , word ) == NULL ){
        return FALSE;
    } else {
        return TRUE;
    }
}

void NPC_Util_RandomToken(char *in, char *out, int outbufsize )
{
    int tokennum;
    int i;
    tokennum = 1;
    for( i=0;in[i]!='\0';i++ ){
        if( in[i] == ',' ) tokennum++;
    }
    getStringFromIndexWithDelim(in,",", rand()%tokennum+1,out,outbufsize );

}

void cutDotsTail( char *s )
{
    int l = strlen( s );
    int i;
    for(i=l-1;i>=0;i--){
        if( s[i] == '.' ){
            s[i] = 0;
        } else {
            break;
        }
    }
}

int NPC_Util_FrontItem( int meindex )
{
    int front_x, front_y,objindex,itemindex;
    OBJECT obj;

    CHAR_getCoordinationDir( CHAR_getInt(meindex,CHAR_DIR)
                             , CHAR_getInt(meindex,CHAR_X)
                             , CHAR_getInt(meindex,CHAR_Y)
                             ,1, &front_x, &front_y );
    for( obj=MAP_getTopObj(CHAR_getInt(meindex,CHAR_FLOOR),
                           front_x,front_y );obj;
        obj=NEXT_OBJECT(obj) ){
        objindex = GET_OBJINDEX(obj);
        if( OBJECT_getType(objindex) == OBJTYPE_ITEM ){
            itemindex = OBJECT_getIndex(objindex);
            if(ITEM_CHECKINDEX(itemindex) ){
                return itemindex;
            }else{
                return -1;
            }
        }
    }

    return -1;

}

int NPC_Util_FrontChar( int meindex )
{
    int front_x, front_y,objindex,enemyindex;
    OBJECT obj;
    CHAR_getCoordinationDir( CHAR_getInt(meindex,CHAR_DIR)
                             , CHAR_getInt(meindex,CHAR_X)
                             , CHAR_getInt(meindex,CHAR_Y)
                             ,1, &front_x, &front_y );
    for( obj=MAP_getTopObj(CHAR_getInt(meindex,CHAR_FLOOR),
                           front_x,front_y );obj;
        obj=NEXT_OBJECT(obj) ){
        objindex = GET_OBJINDEX(obj);
        if( OBJECT_getType(objindex) == OBJTYPE_CHARA ){
            enemyindex = OBJECT_getIndex(objindex);
            if(CHAR_CHECKINDEX(enemyindex) ){
                return enemyindex;
            }
        }
    }
    return -1;
}
static int	NPCUtil_enemytbl[NPC_ENEMY_ENEMYNUMBER+1];
int *NPC_Util_getEnemy( int meindex, int charaindex)
{
	int		i;
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf[64];
	int		ret;
	int		bigcnt;
	int		insert;

	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	for( i = 0; i < NPC_ENEMY_ENEMYNUMBER+1; i ++ ) {
		NPCUtil_enemytbl[i] = -1;
	}

	if( NPC_Util_GetStrFromStrWithDelim( argstr, "enemyno", buf, sizeof( buf))
		== NULL )
	{
		return NULL;
	}
	bigcnt = 0;
	insert = 0;

	for( i = 0; i < NPC_ENEMY_ENEMYNUMBER; i ++ ) {
		int		curEnemy;
		int		et_array;
		char	data[128];

		ret = getStringFromIndexWithDelim( buf,",", i + 1,data, sizeof(data));
		if( ret == FALSE) {
			break;
		}

		curEnemy = ENEMY_getEnemyArrayFromId( atoi( data));
		if( !ENEMY_CHECKINDEX( curEnemy) ) continue;
		et_array = ENEMYTEMP_getEnemyTempArray( curEnemy );
		/* �蚅昃齠�  拻笢摯誑笢倜鄹摯諱摯嶈怮袑窇 */
		if( ENEMYTEMP_getInt( et_array, E_T_SIZE ) == E_T_SIZE_BIG) {
			if( bigcnt >= 5 ) {
				continue;
			}
			/* 鳹鐎誑    勗埵埱趙笢倜鳹譴毀  躂儒窇堎 */
			if( insert > 4 ) {
				int	target_et_array;
				int	flg = FALSE;
				int	j;
				/*   躂堎鳹鐎禱潺埰 */
				for( j = 0; j < 5; j ++ ) {
					/* 昶笢趙堎摯毀堁凝媃笢 */
					if( !ENEMY_CHECKINDEX( NPCUtil_enemytbl[j])) break;

					target_et_array = ENEMYTEMP_getEnemyTempArray(
													NPCUtil_enemytbl[j]);
					/* 堁凝媃笢 */
					if( !ENEMYTEMP_CHECKINDEX( target_et_array)) break;

					/*   躂瓟渦躂堎 */
					if( ENEMYTEMP_getInt( target_et_array, E_T_SIZE)
						== E_T_SIZE_NORMAL )
					{
						flg = TRUE;
						break;
					}
				}
				if( !flg) continue;

				NPCUtil_enemytbl[insert] = NPCUtil_enemytbl[j];
				NPCUtil_enemytbl[j] = curEnemy;
			}
			else {
				NPCUtil_enemytbl[insert] = curEnemy;
			}
			bigcnt ++;
		}
		else {
			NPCUtil_enemytbl[insert] = curEnemy;
		}
		insert ++;
	}
	return NPCUtil_enemytbl;
}

/*==================雄き��啞溘精扠蚗哱匴祛=============================*/

/* 騷穫璃哱敖  
 * 扠蚗哱啞溘精禱  趙堎煦崹摯窐  
 */
void  NPC_EventSetFlg(int talker,int shiftbit)

{
	int point=0;
	int array;
	int shift;
#ifdef _TEACHER_SYSTEM
	int iGetFame = 0;
#endif

	/*--  -1毀啞溘精勗匴祛誑埵笢騷穫璃哱   -*/
	if(shiftbit==-1) return;

	array = shiftbit / 32;
	shift = shiftbit % 32;

	point = CHAR_getInt( talker, CHAR_ENDEVENT+array);
	
	// Arminius 8.24 prevent wash dp
	if (!(point & (1 << shift))) {
	  // CoolFish: Family Adv 2001/8/4
		AddFMAdv(talker, shiftbit);
#ifdef _TEACHER_SYSTEM
		if(shiftbit < arraysizeof(FMAdvTbl)) iGetFame = FMAdvTbl[shiftbit];
#endif
	}
#ifdef _TEACHER_SYSTEM
	iGetFame /= 20; // 絳呇鳳腕悝汜垀腕汒咡腔 5% (1/20)
	// 潰脤垀腕腔汒咡衄羶衄湮黺0
	if(iGetFame > 0){
		// 潰脤衄羶衄絳呇
		if(strlen(CHAR_getChar(talker,CHAR_TEACHER_ID)) > 0 && strlen(CHAR_getChar(talker,CHAR_TEACHER_NAME)) > 0){
			int iPlayernum = CHAR_getPlayerMaxNum(),i;
			char szMsg[128];
			
			// 潰脤絳呇婓祥婓盄奻
			for(i=0;i<iPlayernum;i++){
				if(CHAR_getCharUse(i) == FALSE) continue;
				if(strcmp(CHAR_getChar(talker,CHAR_TEACHER_ID),CHAR_getChar(i,CHAR_CDKEY)) == 0 &&
					strcmp(CHAR_getChar(talker,CHAR_TEACHER_NAME),CHAR_getChar(i,CHAR_NAME)) == 0){
					float fGetFame = (float)iGetFame/100;
					// 絳呇婓盄奻
					CHAR_setWorkInt(i,CHAR_WORK_GET_TEACHER_FAME,CHAR_getWorkInt(i,CHAR_WORK_GET_TEACHER_FAME) + iGetFame);
					sprintf(szMsg,"鳳腕悝汜 %s %.2f 萸汒咡",CHAR_getChar(talker,CHAR_NAME),fGetFame);
					CHAR_talkToCli(i,-1,szMsg,CHAR_COLORYELLOW);
					break;
				}
			}
		}
	}
#endif
	
	point = point | (1 << shift);
	CHAR_setInt( talker, CHAR_ENDEVENT+array, point);
}

/* 騷穫璃哱敖  
 * 扠蚗哱啞溘精誑  埱趙堎凝禱ぅ迋堎
 *
 *	  埱趙笢躂��	TRUE
 *	埵崹躂��		FALSE
 */
BOOL NPC_EventCheckFlg(int talker,int shiftbit)
{
	int point=0;
	int array;
	int shift;
	/*--  -1毀啞溘精勗匴祛誑埵笢騷穫璃哱   -*/
	if(shiftbit==-1) return FALSE;

	array = shiftbit / 32;
	shift = shiftbit % 32;

	point = CHAR_getInt( talker, CHAR_ENDEVENT+array);
	if( (point & (1<<shift))  == (1<<shift)){
		return TRUE;
	}
	return FALSE;
}

/* 騷穫璃哱  
 * 扠蚗哱啞溘精禱  趙堎煦崹摯窐  
 */
void  NPC_NowEventSetFlg(int talker,int shiftbit)
{
	int point=0;
	int array;
	int shift;
	/*--  -1毀啞溘精勗匴祛誑埵笢騷穫璃哱   -*/
	if(shiftbit==-1) return;

	array = shiftbit / 32;
	shift = shiftbit % 32;

	point = CHAR_getInt( talker, CHAR_NOWEVENT+array);
	point = point | (1 << shift);
	point = CHAR_setInt( talker, CHAR_NOWEVENT+array,point);
}

void  NPC_NowEventSetFlgCls(int talker,int shiftbit)
{

	int point=0;
	int array;
	int shift;
	if(shiftbit==-1) return;
	array = shiftbit / 32;
	shift = shiftbit % 32;
	point = CHAR_getInt( talker, CHAR_NOWEVENT+array);
	point = point ^ (1<<shift);
	CHAR_setInt(talker,CHAR_NOWEVENT+array,point);

}

BOOL NPC_NowEventCheckFlg(int talker,int shiftbit)
{
	int point=0;
	int array;
	int shift;
	if(shiftbit==-1) return FALSE;
	array = shiftbit / 32;
	shift = shiftbit % 32;
	point = CHAR_getInt( talker, CHAR_NOWEVENT+array);
	if( (point & (1<<shift))  == (1<<shift)){
		return TRUE;
	}
	return FALSE;
}

void  NPC_NowEndEventSetFlgCls(int talker,int shiftbit)
{
	int point=0;
	int array;
	int shift;
	if(shiftbit==-1) return;
	array = shiftbit / 32;
	shift = shiftbit % 32;
	if(NPC_NowEventCheckFlg(talker,shiftbit)==TRUE){
		point = CHAR_getInt( talker, CHAR_NOWEVENT+array);
		point = point ^ (1<<shift);
		CHAR_setInt(talker,CHAR_NOWEVENT+array,point);
	}
	if(NPC_EventCheckFlg(talker,shiftbit)==TRUE){
		point = CHAR_getInt( talker, CHAR_ENDEVENT+array);
		point = point ^ (1<<shift);
		CHAR_setInt(talker,CHAR_ENDEVENT+array,point);
	}
}

#ifdef _NEW_MANOR_LAW
extern  struct  FM_POINTLIST fmpointlist;
#endif

// CoolFish: Family Adv 2001/7/29
void AddFMAdv(int talker, int shiftbit)
{
	int fmadv = 0;
	char	buf[256];
#ifdef _NEW_MANOR_LAW
	char buf1[16];
#endif
#ifdef _FMVER21	
#else
	if (CHAR_getInt(talker, CHAR_FMINDEX) < 0)	return;
	if (CHAR_getInt(talker, CHAR_FMLEADERFLAG) <= 0 )	return;
	if (strcmp(CHAR_getChar(talker, CHAR_FMNAME), "") == 0)	return;
#endif
	if (shiftbit >= arraysizeof(FMAdvTbl))
		return;
	else
		fmadv = FMAdvTbl[shiftbit];
	if (fmadv <= 0)	return;
	sprintf(buf, "%d", fmadv);
	#ifdef _PERSONAL_FAME	// Arminius 8.30: 模逜跺�侂驫�
	  CHAR_earnFame(talker, fmadv);
	#endif
	#ifdef _NEW_MANOR_LAW
		sprintf(buf1, "%d", CHAR_getInt(talker,CHAR_FAME));
	#endif
#ifdef _FMVER21	
	if (CHAR_getInt(talker, CHAR_FMLEADERFLAG) > 0
	   && CHAR_getInt(talker, CHAR_FMLEADERFLAG) != FMMEMBER_APPLY)
	{
	   // CoolFish: 2001/10/03
	   int fd = getfdFromCharaIndex(talker);
		 saacproto_ACFixFMData_send(acfd,
			 CHAR_getChar(talker, CHAR_FMNAME),
			 CHAR_getInt(talker, CHAR_FMINDEX),
			 CHAR_getWorkInt(talker, CHAR_WORKFMINDEXI),
			 FM_FIX_FMADV, buf,
#ifndef _NEW_MANOR_LAW
			 "",
#else
			 buf1,
#endif
			 // CoolFish: 2001/10/03
			 CHAR_getWorkInt(talker, CHAR_WORKFMCHARINDEX),
			 CONNECT_getFdid(fd));
	   	// CHAR_getWorkInt(talker, CHAR_WORKFMCHARINDEX), 0);
#ifdef _NEW_MANOR_LAW
		 {
			 int i;
			 for(i=0;i<FAMILY_MAXHOME;i++){
				 // �庥扂遘囆耽商俶确譭褐鑒�,ァ岊硉珨蜊曹憩砃AC猁郔陔ァ岊訧蹋
				 if(fmpointlist.fm_inwar[i]){
					 saacproto_ACShowTopFMList_send(acfd, FM_TOP_MOMENTUM);
					 break;
				 }
			 }
		 }
#endif
	}
#ifdef _NEW_MANOR_LAW
	// 帤机瞄籵徹奀硐載陔傖埜ァ岊硉
	else if(CHAR_getInt(talker, CHAR_FMLEADERFLAG) == FMMEMBER_APPLY){
		int fd = getfdFromCharaIndex(talker);
		sprintf(buf,"%d",CHAR_getInt(talker,CHAR_FAME));
		saacproto_ACFixFMData_send(acfd,
			CHAR_getChar(talker,CHAR_FMNAME),
			CHAR_getInt(talker,CHAR_FMINDEX),
			CHAR_getWorkInt(talker,CHAR_WORKFMINDEXI),
			FM_FIX_FAME,buf,"",
			CHAR_getWorkInt(talker,CHAR_WORKFMCHARINDEX),
			CONNECT_getFdid(fd));
	}
#endif
#else
	saacproto_ACFixFMData_send(acfd,
	   	CHAR_getChar(talker, CHAR_FMNAME),
	   	CHAR_getInt(talker, CHAR_FMINDEX),
	   	CHAR_getWorkInt(talker, CHAR_WORKFMINDEXI),
	   	FM_FIX_FMADV, buf, "",
	   	CHAR_getWorkInt(talker, CHAR_WORKFMCHARINDEX), 0);
#endif	   	
}

// Robin 0817 family shop
#if 0
int addNpcFamilyTax( int meindex, int talkerindex, int income )
{
			 int fmindex, fmindexi, village, i;
			 char fmname[256], token[256], buf2[64];
			 int clifd = CHAR_getWorkInt( talkerindex, CHAR_WORKFD );
			
			 sprintf( buf2, "%d", income );
			 for( i=0 ; i<FMPOINTNUM ; i++ ) {
				getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 9, token, sizeof(token));
				village = atoi(token);
				if( village == CHAR_getInt( meindex, CHAR_FMINDEX ) ) {
					getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, token, sizeof(token));
					fmindex = atoi(token);
					getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 6, token, sizeof(token));
					strcpy(fmname, token);
					getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 8, token, sizeof(token));
					fmindexi = atoi(token);
					saacproto_ACFixFMData_send( acfd, fmname, fmindex, fmindexi,
						FM_FIX_FMGOLD, buf2 , "",
						CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
						CONNECT_getFdid(clifd) );
					
//					print(" addNpcFamilyTax_fm:%s ", fmname);
					return 1;
				}
			 }
			 return 0;
}
#else
int addNpcFamilyTax( int meindex, int talkerindex, int income )
{
	int fmindex, fmindexi, village, npc_village, i;
	char fmname[256], token[256], buf2[64];
	int clifd = CHAR_getWorkInt( talkerindex, CHAR_WORKFD );

	switch( (int)( CHAR_getInt( meindex, CHAR_FLOOR)/100 ) )
	{
		case 10:
			npc_village = 1;
			break;
		case 20:
			npc_village = 2;
			break;			
		case 30:
			npc_village = 3;
			break;		
		case 40:
			npc_village = 4;
			break;
#ifdef _FAMILY_MANORNUM_CHANGE
		case 50:
			npc_village = 5;
			break;	
		case 60:
			npc_village = 6;
			break;
// Syu ADD 党淏侐湮傑拸阭彶恀枙
		case 71:
			npc_village = 6;
			break;
		case 72:
			npc_village = 7;
			break;
		case 73:
			npc_village = 8;
			break;
		case 74:
			npc_village = 9;
			break;

		case 70:
			npc_village = 7;
			break;			
		case 80:
			npc_village = 8;
			break;		
		case 90:
			npc_village = 9;
			break;			
		case 100:
			npc_village = 10;
			break;			
#endif
		default:
			return 0;
	}
			 sprintf( buf2, "%d", income );
			 for( i=0 ; i<FMPOINTNUM ; i++ ) {
				if( getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 9, token, sizeof(token)) == FALSE )
					continue;
				village = atoi(token);
				if( village == npc_village ) {
					if( getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, token, sizeof(token)) == FALSE)
						return 0;
					fmindex = atoi(token);
// Terry add for �蝜�蚽埶羶�劼撙嬦繰遞で�
					if(fmindex < 0) return 0;
// end
					memset( token, 0, sizeof( token));
					if( getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 6, token, sizeof(token)) == FALSE)
						return 0;
					memset( fmname, 0, sizeof( fmname));
					sprintf( fmname, "%s", token);
					if( getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 8, token, sizeof(token)) == FALSE )
						return 0;
					fmindexi = atoi(token);
					saacproto_ACFixFMData_send( acfd, fmname, fmindex, fmindexi,
						FM_FIX_FMGOLD, buf2 , "",
						CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
						CONNECT_getFdid(clifd) );
					
//					print(" addNpcFamilyTax_fm:%s ", fmname);
					return 1;
				}
			 }
			 return 0;
}
#endif

