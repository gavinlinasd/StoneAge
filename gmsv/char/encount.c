#include "version.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef _REDHAT_V9
#include <errno.h>
#endif

#include "common.h"
#include "util.h"
#include "buf.h"
#include "char_base.h"
#include "char.h"
#include "configfile.h"
#include "encount.h"
#include "enemy.h"

#ifdef _ADD_ENCOUNT           // WON ADD 崝樓菩婈郣揖楷党璃
#include "encount.h"
#endif

/* 操璃庈坳璃哱匴  摯藺↓筒 */

#ifndef _ADD_ENCOUNT           // WON ADD 崝樓菩婈郣揖楷党璃
typedef struct tagENCOUNT_Table
{
    int                 index;
    int                 floor;
    int                 encountprob_min;                /* 操璃庈坳璃哱賃   */
    int                 encountprob_max;                /* 操璃庈坳璃哱賃   */
    int                 enemymaxnum;        /* 汔躂煦崹傍禱軘堎凝 */
    int                 zorder;
    int                 groupid[ENCOUNT_GROUPMAXNUM];       /* 精鳴↓鏤No */
    int                 createprob[ENCOUNT_GROUPMAXNUM];    /* 鼠摯精鳴↓鏤摯③藯   */
    RECT                rect;
}ENCOUNT_Table;
ENCOUNT_Table           *ENCOUNT_table;
#endif

int                     ENCOUNT_encountnum;
#define     ENCOUNT_ENEMYMAXCREATENUM   10

static INLINE BOOL ENCOUNT_CHECKENCOUNTTABLEARRAY( int array)
{
    if( array < 0 || array > ENCOUNT_encountnum-1) return FALSE;
    return TRUE;
}

/*------------------------------------------------------------
 * 操璃庈坳璃哱优擭摯疐趵撬禱埰堎��
 * 礎倳
 *  filename        char*       优擭啞栝騷鳴  
 * 蒍堇偯
 *  埬      TRUE(1)
 *  謄      FALSE(0)
 *------------------------------------------------------------*/
BOOL ENCOUNT_initEncount( char* filename )
{
    FILE*   f;
    char    line[256];
    int     linenum=0;
    int     encount_readlen=0;

    f = fopen(filename,"r");
    if( f == NULL ){
        errorprint;
        return FALSE;
    }

    ENCOUNT_encountnum=0;

    /*  竘囀  嗚埵菜誑笰菜堣堎凝汔竣凝ぅ迋堎    */
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );

        ENCOUNT_encountnum++;
    }

    if( fseek( f, 0, SEEK_SET ) == -1 ){
        fprint( "扆梑渣昫\n" );
        fclose(f);
        return FALSE;
    }

    ENCOUNT_table = allocateMemory( sizeof(struct tagENCOUNT_Table)
                                   * ENCOUNT_encountnum );
    if( ENCOUNT_table == NULL ){
        fprint( "拸楊煦饜囀湔 %d\n" ,
                sizeof(ENCOUNT_table)*ENCOUNT_encountnum);
        fclose( f );
        return FALSE;
    }

    /* 疐趵撬 */
{
    int     i,j;
    for( i = 0; i < ENCOUNT_encountnum; i ++ ) {
        ENCOUNT_table[i].index = -1;
        ENCOUNT_table[i].floor = 0;
        ENCOUNT_table[i].encountprob_min = 1;
        ENCOUNT_table[i].encountprob_min = 50;
        ENCOUNT_table[i].enemymaxnum = 4;
        ENCOUNT_table[i].rect.x = 0;
        ENCOUNT_table[i].rect.y = 0;
        ENCOUNT_table[i].rect.height = 0;
        ENCOUNT_table[i].rect.width = 0;
        ENCOUNT_table[i].zorder = 0;
        for( j = 0; j < ENCOUNT_GROUPMAXNUM; j ++ ) {
            ENCOUNT_table[i].groupid[j] = -1;
            ENCOUNT_table[i].createprob[j] = -1;
        }
#ifdef _ADD_ENCOUNT           // WON ADD 崝樓菩婈郣揖楷党璃
		ENCOUNT_table[i].event_now = -1;
		ENCOUNT_table[i].event_end = -1;
		ENCOUNT_table[i].enemy_group = -1;
#endif
    }
}

    /*  竘倜  陑  埰    */
    linenum = 0;
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );

        /*  菜禱堆魠埰堎    */
        /*  竘囀 tab 禱 " " 勗  拻儒窇堎    */
        replaceString( line, '\t' , ' ' );
        /* 袸  摯筒妐↓筒禱噁堎��*/
{
        int     i;
        char    buf[256];
        for( i = 0; i < strlen( line); i ++) {
            if( line[i] != ' ' ) {
                break;
            }
            strcpy( buf, &line[i]);
        }
        if( i != 0 ) {
            strcpy( line, buf);
        }
}
{
        char    token[256];
        int     ret;
        int     x1,x2,y1,y2;
		int		j;
        
        /*   梣誧摯鳴↓鏤勗  埱倜鄹摯諱摯疐趵撬 */
        ENCOUNT_table[encount_readlen].index = -1;
        ENCOUNT_table[encount_readlen].floor = 0;
        ENCOUNT_table[encount_readlen].encountprob_min = 1;
        ENCOUNT_table[encount_readlen].encountprob_min = 50;
        ENCOUNT_table[encount_readlen].enemymaxnum = 4;
        ENCOUNT_table[encount_readlen].rect.x = 0;
        ENCOUNT_table[encount_readlen].rect.y = 0;
        ENCOUNT_table[encount_readlen].rect.height = 0;
        ENCOUNT_table[encount_readlen].rect.width = 0;
        ENCOUNT_table[encount_readlen].zorder = 0;
        for( j = 0; j < ENCOUNT_GROUPMAXNUM; j ++ ) {
            ENCOUNT_table[encount_readlen].groupid[j] = -1;
            ENCOUNT_table[encount_readlen].createprob[j] = -1;
        }
#ifdef _ADD_ENCOUNT           // WON ADD 崝樓菩婈郣揖楷党璃
		ENCOUNT_table[encount_readlen].event_now = -1;
		ENCOUNT_table[encount_readlen].event_end = -1;
		ENCOUNT_table[encount_readlen].enemy_group = -1;
#endif


        /*  痲敁僑誧摯哱↓袲璃禱峟堎    */
        ret = getStringFromIndexWithDelim( line,",",1,token,
                                           sizeof(token));
        if( ret==FALSE ){
            fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
            continue;
        }
        ENCOUNT_table[encount_readlen].index = atoi(token);
        
        /*  2僑誧摯哱↓袲璃禱峟堎    */
        ret = getStringFromIndexWithDelim( line,",",2,token,
                                           sizeof(token));
        if( ret==FALSE ){
            fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
            continue;
        }
        ENCOUNT_table[encount_readlen].floor = atoi(token);

        /*  3僑誧摯哱↓袲璃禱峟堎    */
        ret = getStringFromIndexWithDelim( line,",",3,token,
                                           sizeof(token));
        if( ret==FALSE ){
            fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
            continue;
        }
        x1 = atoi(token);

        /*  4僑誧摯哱↓袲璃禱峟堎    */
        ret = getStringFromIndexWithDelim( line,",",4,token,
                                           sizeof(token));
        if( ret==FALSE ){
            fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
            continue;
        }
        y1= atoi(token);
        
        /*  5僑誧摯哱↓袲璃禱峟堎    */
        ret = getStringFromIndexWithDelim( line,",",5,token,
                                           sizeof(token));
        if( ret==FALSE ){
            fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
            continue;
        }
        
        x2 = atoi(token);
        
        /*  6僑誧摯哱↓袲璃禱峟堎    */
        ret = getStringFromIndexWithDelim( line,",",6,token,
                                           sizeof(token));
        if( ret==FALSE ){
            fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
            continue;
        }
        y2= atoi(token);

        ENCOUNT_table[encount_readlen].rect.x      = min(x1,x2);
        ENCOUNT_table[encount_readlen].rect.width  = max(x1,x2) - min(x1,x2);
        ENCOUNT_table[encount_readlen].rect.y      = min(y1,y2);
        ENCOUNT_table[encount_readlen].rect.height = max(y1,y2) - min(y1,y2);

        /*  7誧摯哱↓袲璃禱峟堎    */
        ret = getStringFromIndexWithDelim( line,",",7,token,
                                           sizeof(token));
        if( ret==FALSE ){
            fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
            continue;
        }
        ENCOUNT_table[encount_readlen].encountprob_min = atoi(token);

        /*  8誧摯哱↓袲璃禱峟堎    */
        ret = getStringFromIndexWithDelim( line,",",8,token,
                                           sizeof(token));
        if( ret==FALSE ){
            fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
            continue;
        }
        ENCOUNT_table[encount_readlen].encountprob_max = atoi(token);

{
		int		a,b;
		a = ENCOUNT_table[encount_readlen].encountprob_min;
		b = ENCOUNT_table[encount_readlen].encountprob_max;
		/*   覽摯ぅ堆 */
        ENCOUNT_table[encount_readlen].encountprob_min 
        	= min( a,b);
        ENCOUNT_table[encount_readlen].encountprob_max 
        	= max( a,b);
}
        /*  9僑誧摯哱↓袲璃禱峟堎    */
        ret = getStringFromIndexWithDelim( line,",",9,token,
                                           sizeof(token));
        if( ret==FALSE ){
            fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
            continue;
        }
        {
            int maxnum = atoi( token);
            /* 倳摯諜騍鍛摯鏍攝蚗袲 */
            if( maxnum < 1 || maxnum > ENCOUNT_ENEMYMAXCREATENUM ) {
                fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
                continue;
            }
            ENCOUNT_table[encount_readlen].enemymaxnum = maxnum;
        }
        /*  10誧摯哱↓袲璃禱峟堎    */
        ret = getStringFromIndexWithDelim( line,",",10,token,
                                           sizeof(token));
        if( ret==FALSE ){
            fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
            continue;
        }
        ENCOUNT_table[encount_readlen].zorder = atoi(token);
        #define		CREATEPROB_TOKEN	11
        
        /*  11  31誧摯哱↓袲璃禱峟堎    */
        {
            int     i;
            
            for( i = CREATEPROB_TOKEN; i < CREATEPROB_TOKEN +ENCOUNT_GROUPMAXNUM*2; i ++) {
                ret = getStringFromIndexWithDelim( line,",",i,token,
                                                   sizeof(token));
                if( ret==FALSE ){
                    fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
                    continue;
                }
                if( strlen( token) != 0 ) {
                    if( i < CREATEPROB_TOKEN + ENCOUNT_GROUPMAXNUM ) {
                        ENCOUNT_table[encount_readlen].groupid[i-CREATEPROB_TOKEN] 
                            = atoi(token);
                    }
                    else {
                        ENCOUNT_table[encount_readlen].createprob[i-(CREATEPROB_TOKEN + ENCOUNT_GROUPMAXNUM)] 
                            = atoi(token);
                    }
                }
            }

            /* 福  鏍攝蚗袲 */
            if( checkRedundancy( ENCOUNT_table[encount_readlen].groupid, 
            			arraysizeof( ENCOUNT_table[encount_readlen].groupid)))
            {
            	fprint( "恅璃逄楊渣昫:%s 菴%d俴\n", 
            				filename,linenum);
            	continue;
            }
        }


#ifdef _ADD_ENCOUNT           // WON ADD 崝樓菩婈郣揖楷党璃
        ret = getStringFromIndexWithDelim( line,",",31,token,
                                           sizeof(token));
        if( ret==FALSE ){
            fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
            continue;
        }
        ENCOUNT_table[encount_readlen].event_now = atoi(token);

        ret = getStringFromIndexWithDelim( line,",",32,token,
                                           sizeof(token));
        if( ret==FALSE ){
            fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
            continue;
        }
        ENCOUNT_table[encount_readlen].event_end = atoi(token);

        ret = getStringFromIndexWithDelim( line,",",33,token,
                                           sizeof(token));
        if( ret==FALSE ){
            fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
            continue;
        }
        ENCOUNT_table[encount_readlen].enemy_group = atoi(token);
#endif

        encount_readlen ++;
}
    }
    fclose(f);

    ENCOUNT_encountnum = encount_readlen;

    print( "衄虴腔郣菩釴梓杅岆 %d..", ENCOUNT_encountnum );

#if 0

    {
        int i;
        for( i=0; i <ENCOUNT_encountnum ; i++ )
            print( "encount idx[%d] fl[%d] prob_min[%d] prob_max[%d] e_max[%d] x[%d] wth[%d] y[%d] hgt[%d] \n",
                   ENCOUNT_table[i].index,
                   ENCOUNT_table[i].floor,
                   ENCOUNT_table[i].encountprob_min,
                   ENCOUNT_table[i].encountprob_max,
                   ENCOUNT_table[i].enemymaxnum,
                   ENCOUNT_table[i].rect.x,
                   ENCOUNT_table[i].rect.width,
                   ENCOUNT_table[i].rect.y,
                   ENCOUNT_table[i].rect.height);
    }
#endif
    return TRUE;
}
/*------------------------------------------------------------------------
 * 操璃庈坳璃哱优擭啞栝騷鳴  陑  媃
 *-----------------------------------------------------------------------*/
BOOL ENCOUNT_reinitEncount( void )
{
	freeMemory( ENCOUNT_table);
	return( ENCOUNT_initEncount( getEncountfile()));
}

/*------------------------------------------------------------
 * 炩擭踏躂倜淢  摯ENCOUNT_table摯蝨棬禱ぅ迋堎��
 * zorder摯倳棬禱峟趙騅袸棹痾摯禜笢  禱噁  埰堎��
 * 礎倳
 *  floor       int     啞痲囮ID
 *  x           int     x淢  
 *  y           int     y淢  
 * 蒍堇偯
 *  諜橖      蝨棬
 *  噁  謄    -1
 ------------------------------------------------------------*/
int ENCOUNT_getEncountAreaArray( int floor, int x, int y)
{
    int     i;
    int     index = -1;
    for( i=0 ; i<ENCOUNT_encountnum ; i++ ) {
        if( ENCOUNT_table[i].floor == floor ) {
            if( CoordinateInRect( &ENCOUNT_table[i].rect, x, y) ) {
                int curZorder = ENCOUNT_getZorderFromArray(i);
                if( curZorder >0) {
                    if( index != -1 ) {
                        /* 騅袸棹痾禱ぅ迋堎 */
                        /*   拻笢  騅袸 */
                        if(  curZorder > ENCOUNT_getZorderFromArray(index)) {
                            index = i;
                        }
                    }
                    else {
                        index = i;
                    }
                }
            }
        }
    }
    return index;
}

/*------------------------------------------------------------
 * 炩擭踏躂倜淢  摯操璃庈坳璃哱賃  禱ぅ迋堎��
 * 礎倳
 *  floor       int     啞痲囮ID
 *  x           int     x淢  
 *  y           int     y淢  
 * 蒍堇偯
 *  諜橖      兔雄窀摯賃  
 *  噁  謄    -1
 ------------------------------------------------------------*/
int ENCOUNT_getEncountPercentMin( int charaindex, int floor , int x, int y )
{
    int ret;
    
    ret = ENCOUNT_getEncountAreaArray( floor, x, y);
    if( ret != -1 ) {
        ret = ENCOUNT_table[ret].encountprob_min;
		/* 哱醴痲筒嗚堅禱僑崹堎 */
		if( CHAR_getWorkInt( charaindex, CHAR_WORK_TOHELOS_COUNT) > 0 ) {
			ret = ceil( ret * 
				((100 + CHAR_getWorkInt( charaindex, CHAR_WORK_TOHELOS_CUTRATE)) 
					/ 100.0));
		}
		if( ret < 0 ) ret = 0;
		if( ret > 100 ) ret = 100;
    }
    return ret;
}
/*------------------------------------------------------------
 * 炩擭踏躂倜淢  摯操璃庈坳璃哱賃  禱ぅ迋堎��
 * 礎倳
 *  floor       int     啞痲囮ID
 *  x           int     x淢  
 *  y           int     y淢  
 * 蒍堇偯
 *  諜橖      兔雄窀摯賃  
 *  噁  謄    -1
 ------------------------------------------------------------*/
int ENCOUNT_getEncountPercentMax( int charaindex, int floor , int x, int y )
{
    int ret;
    
    ret = ENCOUNT_getEncountAreaArray( floor, x, y);
    if( ret != -1 ) {
        ret = ENCOUNT_table[ret].encountprob_max;
		/* 哱醴痲筒嗚堅禱僑崹堎 */
		if( CHAR_getWorkInt( charaindex, CHAR_WORK_TOHELOS_COUNT) > 0 ) {
			ret = ceil( ret * 
				((100 + CHAR_getWorkInt( charaindex, CHAR_WORK_TOHELOS_CUTRATE)) 
					/ 100.0));
		}
		if( ret < 0 ) ret = 0;
		if( ret > 100 ) ret = 100;
    }
    return ret;
}
/*------------------------------------------------------------
 * 炩擭踏躂倜淢  摯傍牁埬MAX倳禱ぅ迋堎��
 * 礎倳
 *  floor       int     啞痲囮ID
 *  x           int     x淢  
 *  y           int     y淢  
 * 蒍堇偯
 *  諜橖      兔雄窀摯賃  
 *  噁  謄    -1
 ------------------------------------------------------------*/
int ENCOUNT_getCreateEnemyMaxNum( int floor , int x, int y )
{
    int ret;
    
    ret = ENCOUNT_getEncountAreaArray( floor, x, y);
    if( ret != -1 ) {
        ret = ENCOUNT_table[ret].enemymaxnum;
    }
    return ret;
}
/*------------------------------------------------------------
 * 炩擭踏躂倜淢  摯操璃庈坳璃哱啞贖↓鳴迶摯index禱ぅ迋堎��
 * 礎倳
 *  floor       int     啞痲囮ID
 *  x           int     x淢  
 *  y           int     y淢  
 * 蒍堇偯
 *  諜橖      兔雄窀
 *  噁  謄    -1
 ------------------------------------------------------------*/
int ENCOUNT_getEncountIndex( int floor , int x, int y )
{
    int ret;
    
    ret = ENCOUNT_getEncountAreaArray( floor, x, y);
    if( ret != -1 ) {
        ret = ENCOUNT_table[ret].index;
    }
    return ret;
}
/*------------------------------------------------------------
 * 炩擭踏躂倜淢  摯操璃庈坳璃哱啞贖↓鳴迶摯index禱ぅ迋堎��
 * 礎倳
 *  array           int     ENCOUNTTABLE摯蝨棬
 * 蒍堇偯
 *  諜橖      兔雄窀
 *  噁  謄    -1
 ------------------------------------------------------------*/
int ENCOUNT_getEncountIndexFromArray( int array )
{
    if( !ENCOUNT_CHECKENCOUNTTABLEARRAY( array)) return -1;
    return ENCOUNT_table[array].index;
}
/*------------------------------------------------------------
 * 炩擭踏躂倜淢  摯操璃庈坳璃哱賃  禱ぅ迋堎��
 * 礎倳
 *  array           int     ENCOUNTTABLE摯蝨棬
 * 蒍堇偯
 *  諜橖      兔雄窀
 *  噁  謄    -1
 ------------------------------------------------------------*/
int ENCOUNT_getEncountPercentFromArray( int array )
{
    if( !ENCOUNT_CHECKENCOUNTTABLEARRAY( array)) return -1;
    return ENCOUNT_table[array].encountprob_min;
}
/*------------------------------------------------------------
 * 炩擭踏躂倜淢  摯傍牁埬MAX倳禱ぅ迋堎��
 * 礎倳
 *  array           int     ENCOUNTTABLE摯蝨棬
 * 蒍堇偯
 *  諜橖      兔雄窀
 *  噁  謄    -1
 ------------------------------------------------------------*/
int ENCOUNT_getCreateEnemyMaxNumFromArray( int array )
{
    if( !ENCOUNT_CHECKENCOUNTTABLEARRAY( array)) return -1;
    return ENCOUNT_table[array].enemymaxnum;
}
/*------------------------------------------------------------
 * 炩擭踏躂倜蝨棬摯精鳴↓鏤  蠕禱ぅ迋堎��
 * 礎倳
 *  array           int     ENCOUNTTABLE摯蝨棬
 * 蒍堇偯
 *  諜橖      兔雄窀
 *  噁  謄    -1
 ------------------------------------------------------------*/
int ENCOUNT_getGroupIdFromArray( int array, int grouparray )
{
    if( !ENCOUNT_CHECKENCOUNTTABLEARRAY( array)) return -1;
    return ENCOUNT_table[array].groupid[grouparray];
}
/*------------------------------------------------------------
 * 炩擭踏躂倜蝨棬摯精鳴↓鏤摯③藯  禱ぅ迋堎��
 * 礎倳
 *  array           int     ENCOUNTTABLE摯蝨棬
 * 蒍堇偯
 *  諜橖      兔雄窀
 *  噁  謄    -1
 ------------------------------------------------------------*/
int ENCOUNT_getGroupProbFromArray( int array, int grouparray )
{
    if( !ENCOUNT_CHECKENCOUNTTABLEARRAY( array)) return -1;
    return ENCOUNT_table[array].createprob[grouparray];
}
/*------------------------------------------------------------
 * 炩擭踏躂倜蝨棬摯騅袸棹痾禱ぅ迋堎��
 * 礎倳
 *  array           int     ENCOUNTTABLE摯蝨棬
 * 蒍堇偯
 *  諜橖      兔雄窀
 *  噁  謄    -1
 ------------------------------------------------------------*/
int ENCOUNT_getZorderFromArray( int array )
{
    if( !ENCOUNT_CHECKENCOUNTTABLEARRAY( array)) return -1;
    return ENCOUNT_table[array].zorder;
}
