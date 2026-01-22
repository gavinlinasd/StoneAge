/* 衬涩烂楮   */
#include "version.h"
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "util.h"
#include "buf.h"
#include "char_base.h"
#include "char.h"
#include "configfile.h"
#include "encount.h"
#include "enemy.h"
#include "pet.h"
#include "enemyexptbl.h"
#include "petmail.h"
#include "battle.h"
#include "pet_skillinfo.h"
#include "anim_tbl.h"
#include "log.h"

#ifdef _ADD_ENCOUNT           // WON ADD 增加敌遭遇触发修件
#include "encount.h"
#include "npcutil.h"
#endif

#define		ENEMY_INDEXTABLEMAXSIZE		16
static ENEMY_EnemyTable *ENEMY_enemy;
static int              ENEMY_enemynum;

static ENEMYTEMP_Table *ENEMYTEMP_enemy;
static int              ENEMYTEMP_enemynum;

static GROUP_Table 		*GROUP_group;
static int              GROUP_groupnum;

int                     ENEMY_indextable[ENEMY_INDEXTABLEMAXSIZE];

#ifdef _ENEMY_FALLGROUND
extern	tagRidePetTable ridePetTable[296];
#endif

INLINE BOOL ENEMY_CHECKINDEX( int index)
{
    if( index < 0 || index >= ENEMY_enemynum) return FALSE;
    return TRUE;
}

static INLINE BOOL ENEMY_CHECKINTDATAINDEX( int index)
{
    if( index< 0 || index >= ENEMY_DATAINTNUM) return FALSE;
    return TRUE;
}

static INLINE BOOL ENEMY_CHECKCHARDATAINDEX( int index)
{
    if( index< 0 || index >= ENEMY_DATACHARNUM) return FALSE;
    return TRUE;
}

INLINE int ENEMY_getInt( int index, ENEMY_DATAINT element)
{
    return ENEMY_enemy[index].intdata[element];
}

INLINE int *ENEMY_getIntdata( int index)
{
	return ENEMY_enemy[index].intdata;
}

INLINE int ENEMY_setInt( int index, ENEMY_DATAINT element, int data)
{
    int buf;
    buf = ENEMY_enemy[index].intdata[element];
    ENEMY_enemy[index].intdata[element] = data;
    return buf;
}

INLINE char *ENEMY_getChar( int index, ENEMY_DATACHAR element)
{
    if(!ENEMY_CHECKINDEX(index))return NULL;
    if(!ENEMY_CHECKCHARDATAINDEX(element))return NULL;

    return ENEMY_enemy[index].chardata[element].string;
}

INLINE BOOL ENEMY_setChar( int index ,ENEMY_DATACHAR element, char* new )
{
    if(!ENEMY_CHECKINDEX(index))return FALSE;
    if(!ENEMY_CHECKCHARDATAINDEX(element))return FALSE;
    strcpysafe( ENEMY_enemy[index].chardata[element].string,
                sizeof(ENEMY_enemy[index].chardata[element].string),
                new );
    return TRUE;
}

int ENEMY_getEnemyNum( void)
{
    return ENEMY_enemynum;
}

INLINE BOOL ENEMYTEMP_CHECKINDEX( int index)
{
    if( index < 0 || index >= ENEMYTEMP_enemynum) return FALSE;
    return TRUE;
}

static INLINE BOOL ENEMYTEMP_CHECKINTDATAINDEX( int index)
{
    if( index< 0 || index >= E_T_DATAINTNUM) return FALSE;
    return TRUE;
}

static INLINE BOOL ENEMYTEMP_CHECKCHARDATAINDEX( int index)
{
    if( index< 0 || index >= E_T_DATACHARNUM) return FALSE;
    return TRUE;
}

INLINE int ENEMYTEMP_getInt( int index, ENEMYTEMP_DATAINT element)
{
    return ENEMYTEMP_enemy[index].intdata[element];
}

INLINE int ENEMYTEMP_setInt( int index, ENEMYTEMP_DATAINT element, int data)
{
    int buf;
    buf = ENEMYTEMP_enemy[index].intdata[element];
    ENEMYTEMP_enemy[index].intdata[element] = data;
    return buf;
}

INLINE char *ENEMYTEMP_getChar( int index, ENEMYTEMP_DATACHAR element)
{
    if(!ENEMYTEMP_CHECKINDEX(index))return NULL;
    if(!ENEMYTEMP_CHECKCHARDATAINDEX(element))return NULL;

    return ENEMYTEMP_enemy[index].chardata[element].string;
}

INLINE BOOL ENEMYTEMP_setChar( int index ,ENEMYTEMP_DATACHAR element, char* new )
{
    if(!ENEMYTEMP_CHECKINDEX(index))return FALSE;
    if(!ENEMYTEMP_CHECKCHARDATAINDEX(element))return FALSE;
    strcpysafe( ENEMYTEMP_enemy[index].chardata[element].string,
                sizeof(ENEMYTEMP_enemy[index].chardata[element].string),
                new );
    return TRUE;
}

int ENEMYTEMP_getEnemyNum( void)
{
    return ENEMYTEMP_enemynum;
}

INLINE BOOL GROUP_CHECKINDEX( int index)
{
    if( index < 0 || index >= GROUP_groupnum) return FALSE;
    return TRUE;
}

static INLINE BOOL GROUP_CHECKINTDATAINDEX( int index)
{
    if( index< 0 || index >= GROUP_DATAINTNUM) return FALSE;
    return TRUE;
}

static INLINE BOOL GROUP_CHECKCHARDATAINDEX( int index)
{
    if( index< 0 || index >= GROUP_DATACHARNUM) return FALSE;
    return TRUE;
}

INLINE int GROUP_getInt( int index, GROUP_DATAINT element)
{
    return GROUP_group[index].intdata[element];
}

INLINE int GROUP_setInt( int index, GROUP_DATAINT element, int data)
{
    int buf;
    buf = GROUP_group[index].intdata[element];
    GROUP_group[index].intdata[element] = data;
    return buf;
}

INLINE char *GROUP_getChar( int index, GROUP_DATACHAR element)
{
    if(!GROUP_CHECKINDEX(index))return NULL;
    if(!GROUP_CHECKCHARDATAINDEX(element))return NULL;

    return GROUP_group[index].chardata[element].string;
}

INLINE BOOL GROUP_setChar( int index ,GROUP_DATACHAR element, char* new )
{
    if(!GROUP_CHECKINDEX(index))return FALSE;
    if(!GROUP_CHECKCHARDATAINDEX(element))return FALSE;
    strcpysafe( GROUP_group[index].chardata[element].string,
                sizeof(GROUP_group[index].chardata[element].string),
                new );
    return TRUE;
}

int GROUP_getEnemyNum( void)
{
    return GROUP_groupnum;
}

BOOL ENEMYTEMP_initEnemy( char* filename )
{
    FILE*   f;
    char    line[256];
    int     linenum=0;
    int     enemytemp_readlen=0;
	int		i,j;

#ifdef _ENEMYTEMP_OPTIMUM
	int		max_enemytempid =0;
	char	token[256];
#endif
    f = fopen(filename,"r");
    if( f == NULL ){
        print( "文件打开失败\n");
        return FALSE;
    }

    ENEMYTEMP_enemynum=0;

    /*  引内  躲卅垫互窒垫丐月井升丹井譬屯月    */
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );

#ifdef _ENEMYTEMP_OPTIMUM // Robin 取出最大ENEMYTEMP ID
		if( getStringFromIndexWithDelim( line, ",", E_T_DATACHARNUM+E_T_TEMPNO+1,
				token, sizeof(token)) == FALSE )
			continue;
		max_enemytempid = max( atoi( token), max_enemytempid);
#endif

        ENEMYTEMP_enemynum++;
    }

    if( fseek( f, 0, SEEK_SET ) == -1 ){
        fprint( "Seek Error\n" );
        fclose(f);
        return FALSE;
    }

#ifdef _ENEMYTEMP_OPTIMUM
	print("\n 有效EBT:%d 最大EBT:%d \n", ENEMYTEMP_enemynum, max_enemytempid);
	ENEMYTEMP_enemynum = max_enemytempid +1;
#endif

    ENEMYTEMP_enemy = allocateMemory( sizeof(struct tagENEMYTEMP_Table)
                                   * ENEMYTEMP_enemynum );
    if( ENEMYTEMP_enemy == NULL ){
        fprint( "无法分配内存 %d\n" ,
                sizeof(struct tagENEMYTEMP_Table)*ENEMYTEMP_enemynum);
        fclose( f );
        return FALSE;
    }

	/* 赓渝祭 */
    for( i = 0; i < ENEMYTEMP_enemynum; i ++ ) {
    	for( j = 0; j < E_T_DATAINTNUM; j ++ ) {
    		ENEMYTEMP_setInt( i,j,-1);
    	}
    }
    linenum = 0;
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );
        replaceString( line, '\t' , ' ' );
{
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

#ifdef _ENEMYTEMP_OPTIMUM
		if( getStringFromIndexWithDelim( line, ",", E_T_DATACHARNUM+E_T_TEMPNO+1,
				token, sizeof(token)) == FALSE )
			continue;
		enemytemp_readlen = atoi(token);
#endif

#if 0
        ret = getStringFromIndexWithDelim( line,",",1,token,
                                           sizeof(token));
        if( ret==FALSE ){
            fprint("文件语法错误:%s 第%行\n",filename,linenum);
            continue;
        }
        ENEMYTEMP_setChar( enemytemp_readlen, E_T_NAME, token);
#endif
		for( i = 0; i < E_T_DATACHARNUM; i ++ ) {
	        ret = getStringFromIndexWithDelim( line,",",i+1,token,
	                                           sizeof(token));
	        if( ret==FALSE ){
	            fprint("文件语法错误:%s 第%d行\n",filename,linenum);
	            continue;
	        }
	        ENEMYTEMP_setChar( enemytemp_readlen, E_T_NAME + i, token);

		}
#define	ENEMYTEMP_STARTINTNUM		(E_T_DATACHARNUM+1)
        for( i = ENEMYTEMP_STARTINTNUM; i < E_T_DATAINTNUM+ENEMYTEMP_STARTINTNUM; i ++ ) {
            ret = getStringFromIndexWithDelim( line,",",i,token,
                                               sizeof(token));
            if( ret==FALSE ){
                fprint("文件语法错误:%s 第%d行\n",filename,linenum);
                break;
            }
            if( strlen( token) != 0 ) {
	            ENEMYTEMP_setInt( enemytemp_readlen, i - ENEMYTEMP_STARTINTNUM, atoi( token));
			}
        }
        if( i < E_T_DATAINTNUM+ENEMYTEMP_STARTINTNUM ) continue;


        enemytemp_readlen ++;
}
    }
    fclose(f);

    ENEMYTEMP_enemynum = enemytemp_readlen;

    print( "有效宠物基本状态数是 %d...", ENEMYTEMP_enemynum );

#if 0

    for( i=0; i <ENEMYTEMP_enemynum ; i++ ){
        for( j = 0; j < E_T_DATACHARNUM; j ++ ) {
	        print( "%s ", ENEMYTEMP_getChar( i, j));
        }
        for( j = 0; j < E_T_DATAINTNUM; j ++ ) {
            print( "%d ", ENEMYTEMP_getInt( i, j));
        }
        print( "\n");
    }
#endif
    return TRUE;
}

BOOL ENEMYTEMP_reinitEnemy( void )
{
	freeMemory( ENEMYTEMP_enemy);
	return( ENEMYTEMP_initEnemy( getEnemyBasefile()));
}

int ENEMYTEMP_getEnemyTempArray( int enemyindex)
{
	if( !ENEMY_CHECKINDEX( enemyindex)) return -1;
	return ENEMY_enemy[enemyindex].enemytemparray;
}

int ENEMYTEMP_getEnemyTempArrayFromTempNo( int EnemyTempNo)
{
#ifdef _ENEMYTEMP_OPTIMUM
	if( EnemyTempNo >= 0 && EnemyTempNo < ENEMYTEMP_enemynum )
		return	EnemyTempNo;
#else
	int		i;
	for( i = 0; i < ENEMYTEMP_enemynum; i ++ ) {
		if( ENEMYTEMP_getInt( i, E_T_TEMPNO) == EnemyTempNo) {
			return i;
		}
	}
#endif
	return -1;
}

int ENEMYTEMP_getEnemyTempArrayFromInitnum( int EnemyTempNo)
{
	int		i;
	for( i = 0; i < ENEMYTEMP_enemynum; i ++ ) {
		if( ENEMYTEMP_getInt( i, E_T_INITNUM) == EnemyTempNo) {
			return i;
		}
	}
	return -1;
}

BOOL ENEMY_initEnemy( char* filename )
{
    FILE*   f;
    char    line[256];
    int     linenum=0;
    int     enemy_readlen=0;
	int		i,j;
#ifdef _ENEMY_OPTIMUM
	int		max_enemyid =0;
	char	token[256];
#endif

    f = fopen(filename,"r");
    if( f == NULL ){
        print( "文件打开失败\n");
        return FALSE;
    }

    ENEMY_enemynum=0;
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );

#ifdef _ENEMY_OPTIMUM // Robin 取出最大ENEMY ID
		if( getStringFromIndexWithDelim( line, ",", ENEMY_DATACHARNUM+ENEMY_ID+1,
				token, sizeof(token)) == FALSE )
			continue;
		max_enemyid = max( atoi( token), max_enemyid);
#endif

        ENEMY_enemynum++;
    }

    if( fseek( f, 0, SEEK_SET ) == -1 ){
        fprint( "寻找失败\n" );
        fclose(f);
        return FALSE;
    }

#ifdef _ENEMY_OPTIMUM
	print("\n 有效ET:%d 最大ET:%d \n", ENEMY_enemynum, max_enemyid);
	ENEMY_enemynum = max_enemyid +1;
#endif

    ENEMY_enemy = allocateMemory( sizeof(struct tagENEMY_EnemyTable)
                                   * ENEMY_enemynum );
    if( ENEMY_enemy == NULL ){
        fprint( "无法分配内存 %d\n" ,
                sizeof(struct tagENEMY_EnemyTable)*ENEMY_enemynum);
        fclose( f );
        return FALSE;
    }
    for( i = 0; i < ENEMY_enemynum; i ++ ) {
    	for( j = 0; j < ENEMY_DATAINTNUM; j ++ ) {
    		ENEMY_setInt( i,j,-1);
    	}
    }
    linenum = 0;
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );
        replaceString( line, '\t' , ' ' );
{
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

#ifdef _ENEMY_OPTIMUM
		if( getStringFromIndexWithDelim( line, ",", ENEMY_DATACHARNUM+ENEMY_ID+1,
				token, sizeof(token)) == FALSE )
			continue;
		enemy_readlen = atoi(token);
#endif

        ret = getStringFromIndexWithDelim( line,",",1,token,sizeof(token));
        if( ret==FALSE ){
            fprint("文件语法错误:%s 第%d行\n",filename,linenum);
            continue;
        }
        ENEMY_setChar( enemy_readlen, ENEMY_NAME, token);
        ret = getStringFromIndexWithDelim( line,",",2,token,sizeof(token));
        if( ret==FALSE ){
            fprint("文件语法错误:%s 第%d行\n",filename,linenum);
            continue;
        }
        ENEMY_setChar( enemy_readlen, ENEMY_TACTICSOPTION, token);
#ifdef _BATTLENPC_WARP_PLAYER
	    ret = getStringFromIndexWithDelim( line, ",", 3, token, sizeof(token));
        if(ret==FALSE){
            fprint("文件语法错误:%s 第%d行\n",filename,linenum);
            continue;
        }
        ENEMY_setChar( enemy_readlen, ENEMY_ACT_CONDITION, token);
#endif

#ifdef _BATTLENPC_WARP_PLAYER
#define	ENEMY_STARTINTNUM		4
#else
#define	ENEMY_STARTINTNUM		3
#endif

        for( i = ENEMY_STARTINTNUM; i < ENEMY_DATAINTNUM+ENEMY_STARTINTNUM; i ++ ) {
            ret = getStringFromIndexWithDelim( line,",",i,token,sizeof(token));
            if( ret==FALSE ){
                fprint("文件语法错误:%s 第%d行\n",filename,linenum);
                break;
            }
            ENEMY_setInt( enemy_readlen, i - ENEMY_STARTINTNUM, atoi( token));
        }
        if( i < ENEMY_DATAINTNUM+ENEMY_STARTINTNUM ) continue;
		for( i = 0; i < ENEMYTEMP_enemynum; i ++ ) {
			if( ENEMYTEMP_getInt( i, E_T_TEMPNO)== ENEMY_getInt( enemy_readlen, ENEMY_TEMPNO)){
				break;
			}
		}
		if( i == ENEMYTEMP_enemynum) {
            fprint("文件语法错误:%s 第%d行\n",filename,linenum);
			continue;
		}
		ENEMY_enemy[enemy_readlen].enemytemparray = i;
{
		int	lv_min, lv_max;
		lv_min = ENEMY_getInt( enemy_readlen, ENEMY_LV_MIN);
		lv_max = ENEMY_getInt( enemy_readlen, ENEMY_LV_MAX);
		if( lv_min == 0 ) lv_min = lv_max;
		ENEMY_setInt( enemy_readlen, ENEMY_LV_MIN, min( lv_min, lv_max));
		ENEMY_setInt( enemy_readlen, ENEMY_LV_MAX, max( lv_min, lv_max));

}

        enemy_readlen ++;
}
    }
    fclose(f);
    ENEMY_enemynum = enemy_readlen;
    print( "有效宠物数是 %d..", ENEMY_enemynum );

#if 0
    {
        for( i=0; i <ENEMY_enemynum ; i++ ){
            print( "%s ", ENEMY_getChar( i, ENEMY_NAME));
            print( "%s ", ENEMY_getChar( i, ENEMY_TACTICSOPTION));
            for( j = 0; j < ENEMY_DATAINTNUM; j ++ ) {
                print( "%d ", ENEMY_getInt( i, j));
            }
            print( "[%d]", ENEMY_enemy[i].enemytemparray);
            print( "\n");
        }
    }
#endif
    return TRUE;
}
/*------------------------------------------------------------------------
 * ENEMY_enemy及涩烂白央奶伙  心  仄
 *-----------------------------------------------------------------------*/
BOOL ENEMY_reinitEnemy( void )
{
	freeMemory( ENEMY_enemy);
	return( ENEMY_initEnemy( getEnemyfile()));
}

/*------------------------------------------------------------------------
 ENEMY_Enemy及骄侬毛襞月
 *-----------------------------------------------------------------------*/
int ENEMY_getEnemyArrayFromIndex( int groupindex, int index)
{
	if( !GROUP_CHECKINDEX( groupindex)) return -1;
	if( index < 0 || index >= CREATEPROB1 - ENEMY_ID1) return -1;
	return GROUP_group[groupindex].enemyarray[index];
}
/*------------------------------------------------------------------------
 * ENEMY_ID 井日ENEMY_Enemy及骄侬毛襞月
 *-----------------------------------------------------------------------*/
int ENEMY_getEnemyArrayFromId( int EnemyId)
{
#ifdef _ENEMY_OPTIMUM
	if( EnemyId >= 0 && EnemyId < ENEMY_enemynum )
		return	EnemyId;
#else
	int		i;
	for( i = 0; i < ENEMY_enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == EnemyId) {
			return i;
		}
	}
#endif
	return -1;
}

int ENEMY_getEnemyArrayFromTempNo( int EnemyTempNo)
{
	int		i;
	for( i = 0; i < ENEMY_enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_TEMPNO) == EnemyTempNo) {
			return i;
		}
	}
	return -1;
}

int ENEMY_getEnemyIdFromTempNo( int EnemyTempNo)
{
	int		i;
	for( i = 0; i < ENEMY_enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_TEMPNO) == EnemyTempNo) {
			return ENEMY_getInt( i, ENEMY_ID);
		}
	}
	return -1;
}

int ENEMY_getEnemyTempNoFromId( int EnemyId)
{
#ifdef _ENEMY_OPTIMUM
	if( EnemyId >= 0 && EnemyId < ENEMY_enemynum)
		return ENEMY_getInt( EnemyId, ENEMY_TEMPNO);
#else
	int		i;
	for( i = 0; i < ENEMY_enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == EnemyId) {
			return ENEMY_getInt( i, ENEMY_TEMPNO);
		}
	}
#endif
	return -1;
}

/*------------------------------------------------------------------------
 * 弘伙□皿涩烂白央奶伙毛  戈
 *-----------------------------------------------------------------------*/
BOOL GROUP_initGroup( char* filename )
{
    FILE*   f;
    char    line[256];
    int     linenum=0;
    int     group_readlen=0;
	int		i,j;

    f = fopen(filename,"r");
    if( f == NULL ){
        print( "文件打开失败\n");
        return FALSE;
    }

    GROUP_groupnum=0;

    /*  引内  躲卅垫互窒垫丐月井升丹井譬屯月    */
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );

        GROUP_groupnum++;
    }

    if( fseek( f, 0, SEEK_SET ) == -1 ){
        fprint( "寻找失败\n" );
        fclose(f);
        return FALSE;
    }

    GROUP_group = allocateMemory( sizeof(struct tagGROUP_Table)
                                   * GROUP_groupnum );
    if( GROUP_group == NULL ){
        fprint( "无法分配内存 %d\n" ,
                sizeof(struct tagGROUP_Table)*GROUP_groupnum);
        fclose( f );
        return FALSE;
    }
	/* 赓渝祭 */
    for( i = 0; i < GROUP_groupnum; i ++ ) {
    	for( j = 0; j < GROUP_DATAINTNUM; j ++ ) {
    		GROUP_setInt( i,j,-1);
    	}
		for( j = 0; j < CREATEPROB1 - ENEMY_ID1; j ++ ) {
			GROUP_group[i].enemyarray[j] = -1;
		}
    }

    /*  引凶  心  允    */
    linenum = 0;
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );

        /*  垫毛帮溥允月    */
        /*  引内 tab 毛 " " 卞  五晶尹月    */
        replaceString( line, '\t' , ' ' );
        /* 燮  及旦矢□旦毛潸月［*/
{
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
        int		loop;

		/* 手仄手ㄡ蘸户及伙□皿卞  匀凶凛及凶户卞赓渝祭仄化云仁 */
    	for( j = 0; j < GROUP_DATAINTNUM; j ++ ) {
    		GROUP_setInt( group_readlen,j,-1);
    	}
		for( j = 0; j < CREATEPROB1 - ENEMY_ID1; j ++ ) {
			GROUP_group[group_readlen].enemyarray[j] = -1;
		}

        /*  夫午勾户及玄□弁件毛苇月    */
        ret = getStringFromIndexWithDelim( line,",",1,token,
                                           sizeof(token));
        if( ret==FALSE ){
            fprint("文件语法错误:%s 第%d行\n",filename,linenum);
            continue;
        }
        GROUP_setChar( group_readlen, GROUP_NAME, token);

        /* 2勾  动嫦反醒袄犯□正 */
#define	GROUP_STARTINTNUM		2
        for( i = GROUP_STARTINTNUM; i < GROUP_DATAINTNUM+GROUP_STARTINTNUM; i ++ ) {
            ret = getStringFromIndexWithDelim( line,",",i,token,
                                               sizeof(token));
            if( ret==FALSE ){
                fprint("文件语法错误:%s 第%d行\n",filename,linenum);
                break;
            }
            if( strlen( token) != 0 ) {
	            GROUP_setInt( group_readlen, i - GROUP_STARTINTNUM, atoi( token));
			}
        }
        if( i < GROUP_DATAINTNUM+GROUP_STARTINTNUM ) continue;

		{
			int		enemycnt = 0;
			for( loop = ENEMY_ID1; loop < CREATEPROB1; loop ++ ) {
				if( GROUP_getInt( group_readlen, loop) != -1 ) {
					for( i = 0; i < ENEMY_enemynum; i ++ ) {
						if( ENEMY_getInt( i, ENEMY_ID)
							== GROUP_getInt( group_readlen, loop))
						{
							break;
						}
					}
					if( i == ENEMY_enemynum) {
						i = -1;
						GROUP_setInt( group_readlen, loop, -1);
					}
					else {
						enemycnt ++;
					}
					GROUP_group[group_readlen].enemyarray[loop - ENEMY_ID1] = i;
				}
			}
			if( enemycnt == 0 ) {
                fprint("团队设定中敌人尚未设定 文件:%s 第%d行\n",filename,linenum);
				continue;;
			}
            if( checkRedundancy( &GROUP_group[group_readlen].intdata[ENEMY_ID1],
            			CREATEPROB1 - ENEMY_ID1))
            {
                fprint("团队设定中敌人重复设定 文件:%s 第%d行\n",filename,linenum);
				continue;;
			}

		}


        group_readlen ++;
}
    }
    fclose(f);

    GROUP_groupnum = group_readlen;

    print( "有效遇敌组群数是 %d...", GROUP_groupnum );

#if 0

    for( i=0; i <GROUP_groupnum ; i++ ){
        int j;
        print( "%s ", GROUP_getChar( i, GROUP_NAME));
        for( j = 0; j < GROUP_DATAINTNUM; j ++ ) {
            print( "%d ", GROUP_getInt( i, j));
        }
        for( j = 0; j < 10; j ++ ) {
	        print( "[%d] ",GROUP_group[i].enemyarray[j]);
		}
        print( "\n");
    }
#endif
	return TRUE;
}
/*------------------------------------------------------------------------
 * 弘伙□皿及涩烂白央奶伙  心  仄
 *-----------------------------------------------------------------------*/
BOOL GROUP_reinitGroup( void )
{
	freeMemory( GROUP_group);
	return( GROUP_initGroup( getGroupfile()));
}
/*------------------------------------------------------------------------
 * GROUP_ID 井日GROUP_Group及骄侬毛襞月
 *-----------------------------------------------------------------------*/
int GROUP_getGroupArray( int groupid)
{
	int		i;
	for( i = 0; i < GROUP_groupnum; i ++ ) {
		if( GROUP_getInt( i, GROUP_ID) == groupid) {
			return i;
		}
	}
	return -1;
}
/*------------------------------------------------------------------------
 * 潸  烦董袄毛综岳允月［
 *-----------------------------------------------------------------------*/
static int ENEMY_getExp( int array,int tarray, int level, int rank )
{
	int		ret;
	int		*p;
    int		*tp;
    float		ranknum = 0.0;
	float		alpha;
    struct 	{
    	int		num;
    	float	rank;
    }ranktbl[] = {
    	{ 100, 2.5},
    	{ 95, 2.0},
    	{ 90, 1.5},
    	{ 85, 1.0},
    	{ 80, 0.5},
    	{ 0, 0.0},
    };
    level --;
    if( level < 0 || level >= arraysizeof( enemybaseexptbl)) return 0;

    p  = ENEMY_enemy[array].intdata;
    tp = ENEMYTEMP_enemy[tarray].intdata;

	// 仿件弁井日    毛潸  
	if( rank < 0 || rank > 5 ) rank = 0;
	ranknum = ranktbl[rank].rank;

	alpha = (   *( tp + E_T_CRITICAL) + *( tp + E_T_COUNTER)   + *(tp + E_T_GET)
			  + *( tp + E_T_POISON)   + *( tp + E_T_PARALYSIS) + *(tp + E_T_SLEEP)
			  + *( tp + E_T_STONE)    + *( tp + E_T_DRUNK)     + *(tp + E_T_CONFUSION)
			) / 100.0
			+ *( tp + E_T_RARE);
	/* EXP＞湘  EXP≈－  仿件弁≈汐  ←伊矛伙×*/
	//return enemybaseexptbl[*(p+ENEMY_LV)] + (ranknum + alpha)*(*(tp+ENEMY_LV));
	ret = enemybaseexptbl[level] + (ranknum + alpha)*(level+1);
	return ( ret < 1 ) ? 1 : ret;

}


int ENEMY_getRank( int array, int tarray ){
    int     *p;
    int		*tp;
    int		paramsum;
	int		i;
	int		ranknum;

    struct 	{
    	int		num;
    	float	rank;
    }ranktbl[] = {
    	{ 100, 2.5}, //总成长率 >= 100 petrank=0
    	{ 95, 2.0},
    	{ 90, 1.5},
    	{ 85, 1.0},
    	{ 80, 0.5},
    	{ 0, 0.0},
    };

    p  = ENEMY_enemy[array].intdata;
    tp = ENEMYTEMP_enemy[tarray].intdata;
	/* 仿件弁毛煌遥允月 */
	/* 仇及  及酷  反酷  踏毛辅哔及仪［ */
	paramsum  = *( tp + E_T_BASEVITAL) +
		   		*( tp + E_T_BASESTR) +
		   		*( tp + E_T_BASETGH) +
		   		*( tp + E_T_BASEDEX);

	ranknum = 0;	// 犯白巧伙玄反ㄟ
	for( i = 0; i < arraysizeof( ranktbl); i ++ ) {
		if(  paramsum >= ranktbl[i].num ) {
			ranknum = i;
			break;
		}
	}

	return ranknum;

}


//*********************************************************
// 仿件母丞卅  
//*********************************************************
static int EnemyGymSkill[] = {
	PETSKILL_GUARDBREAK, 			// ㄢ“布□玉旰仄(3)
	PETSKILL_CONTINUATIONATTACK1, 	// ㄠㄟ“  粮  猾(10)
	PETSKILL_CONTINUATIONATTACK2, 	// ㄠㄠ“  粮  猾(11)
	PETSKILL_CONTINUATIONATTACK3, 	// ㄠㄡ“  粮  猾(12)
	PETSKILL_CHARGE1,				// ㄢㄟ“民乓□斥ㄠ(30)
	PETSKILL_CHARGE2,				// ㄢㄠ“民乓□斥ㄡ(31)
	PETSKILL_MIGHTY1,				// ㄣㄟ“域猾  诮(40)
	PETSKILL_MIGHTY2,				// ㄣㄠ“域猾  诮(41)
	PETSKILL_POWERBALANCE1,			// ㄤㄟ“    及讽ㄠ(50)
	PETSKILL_POWERBALANCE2,			// ㄤㄠ“    及讽ㄡ(51)
	PETSKILL_POWERBALANCE3,			// ㄤㄡ“    及讽ㄢ(52)
	PETSKILL_POISON_ATTACK1,		// ㄥㄟ“    猾(60)
	PETSKILL_POISON_ATTACK2,		// ㄥㄠ“    猾(61)
	PETSKILL_STONE,					// ㄧㄟ“檗祭  猾(80)
	PETSKILL_CONFUSION_ATTACK,		// ㄨㄟ“渔刭  猾(90)
	PETSKILL_DRUNK_ATTACK,			// ㄠㄟㄟ“听办  猾(100)
	PETSKILL_SLEEP_ATTACK,			// ㄠㄠㄟ“戽曰  猾(110)
	PETSKILL_NOGUARD1,				// ㄠㄤㄟ“用□布□玉ㄠ(150)
	PETSKILL_NOGUARD2,				// ㄠㄤㄠ“用□布□玉ㄡ(151)
	PETSKILL_NOGUARD3,				// ㄠㄤㄡ“用□布□玉ㄢ(152)
#ifdef _PSKILL_FALLGROUND
	PETSKILL_FALLGROUND,			//落马术
#endif
#ifdef _SKILL_DAMAGETOHP
	PETSKILL_DAMAGETOHP,		//嗜血技
#endif
#ifdef _Skill_MPDAMAGE
	PETSKILL_MPDAMAGE,		//MP伤害
#endif

#ifdef _SKILL_WILDVIOLENT_ATT
    PETSKILL_WILDVIOLENTATTACK,   //(540)狂暴攻击  vincent add 2002/05/16
#endif
#ifdef _SKILL_SPEEDY_ATT
    PETSKILL_SPEEDYATTACK,        //(541)疾速攻击  vincent add 2002/05/16
#endif
#ifdef _SKILL_GUARDBREAK2
    PETSKILL_GUARDBREAK2,         //(542)破除防御2 vincent add 2002/05/20
#endif
#ifdef _SKILL_SACRIFICE
    PETSKILL_SACRIFICE,           //(543)救援      vincent add 2002/05/30
#endif
#ifdef _SKILL_WEAKEN  
    PETSKILL_WEAKEN,              //(544)救援      vincent add 2002/07/11
#endif
#ifdef _SKILL_DEEPPOISON  
    PETSKILL_DEEPPOISON,          //(545)救援      vincent add 2002/07/16
#endif
#ifdef _SKILL_BARRIER  
    PETSKILL_BARRIER,             //(546)魔障      vincent add 2002/07/16
#endif
#ifdef _SKILL_NOCAST  
    PETSKILL_NOCAST,              //(547)沉默      vincent add 2002/07/16
#endif
#ifdef _SKILL_ROAR 
    PETSKILL_ROAR,                //(548)大吼      vincent add 2002/07/11
#endif

#ifdef _SKILL_REFRESH
    PETSKILL_REFRESH,           //(550)救援      vincent add 2002/08/08
#endif
#ifdef _VARY_WOLF
	PETSKILL_VARY,
#endif
#ifdef _PETSKILL_SETDUCK
	PETSKILL_SETDUCK,
#endif

#ifdef _MAGICPET_SKILL
	PETSKILL_SETMAGICPET,
#endif
#ifdef _PETSKILL_BECOMEPIG
    PETSKILL_BECOMEPIG,
#endif
#ifdef _PETSKILL_LER
		PETSKILL_BATFLY,
		PETSKILL_DIVIDEATTACK,
#endif
#ifdef _PETSKILL_BATTLE_MODEL
		PETSKILL_BATTLE_MODEL,
#endif
};

static int gymbody[] = {
	SPR_001em,SPR_011em,SPR_021em,SPR_031em,SPR_041em,SPR_051em,
	SPR_061em,SPR_071em,SPR_081em,SPR_091em,SPR_101em,SPR_111em,

	SPR_002em,SPR_012em,SPR_022em,SPR_032em,SPR_042em,SPR_052em,
	SPR_062em,SPR_072em,SPR_082em,SPR_092em,SPR_102em,SPR_112em,

	SPR_003em,SPR_013em,SPR_023em,SPR_033em,SPR_043em,SPR_053em,
	SPR_063em,SPR_073em,SPR_083em,SPR_093em,SPR_103em,SPR_113em,

	SPR_004em,SPR_014em,SPR_024em,SPR_034em,SPR_044em,SPR_054em,
	SPR_064em,SPR_074em,SPR_084em,SPR_094em,SPR_104em,SPR_114em,
};


/*------------------------------------------------------------------------
 * ENEMY｛卞仿件母丞卅    毛芨尹月
 *-----------------------------------------------------------------------*/
int ENEMY_RandomChange( int enemyindex, int tempno )
{
	int work, work2, iRet = 0;

	// 仿件母丞平乓仿井升丹井民尼永弁
	if( ( 564 <= tempno && tempno <= 580 )
	||  ( 739 <= tempno && tempno <= 750 )
	||  ( 895 <= tempno && tempno <= 906 )
	){
		//********************************************
		// 皿伊奶乩□及涌
		//********************************************
		iRet = 1;
	}else
	// 仿件母丞平乓仿井升丹井民尼永弁
	if( ( 655 <= tempno && tempno <= 720 )
	||  ( 859 <= tempno && tempno <= 894 )
	||  ( 907 <= tempno && tempno <= 940 )
	){
		//********************************************
		// 矢永玄及涌
		//********************************************
		iRet = 2;
	}else{
		return 0;
	}


	if( iRet == 1 ){
		//********************************************
		//
		// 皿伊奶乩□及涌仄凶衬卅及匹｝箪岭手仿件母丞
		//
		//********************************************
		// 铣手仿件母丞
		CHAR_setInt( enemyindex, CHAR_BASEBASEIMAGENUMBER,
			gymbody[RAND( 0, arraysizeof( gymbody ) - 1)] );
		CHAR_setInt( enemyindex, CHAR_BASEIMAGENUMBER,
			CHAR_getInt( enemyindex, CHAR_BASEBASEIMAGENUMBER ) );
		// 箪岭反赝癫
		work = ( RAND( 0, 20 ) - 10 ) * 10;
		work2 = 100 - ABS( work );
		CHAR_setInt( enemyindex, CHAR_EARTHAT, work );
		CHAR_setInt( enemyindex, CHAR_FIREAT,  -work );
		if( RAND( 0, 1 ) ){
			work2 *= -1;
		}
		CHAR_setInt( enemyindex, CHAR_WATERAT, work2 );
		CHAR_setInt( enemyindex, CHAR_WINDAT,  -work2 );

		// 仿件母丞卅  湛毛  凶六月
		if( DoujyouRandomWeponSet( enemyindex ) ){
			//   溃  湛及桦宁反骚橘  猾毛勾仃月
			CHAR_setPetSkill( enemyindex, 0, PETSKILL_NORMALATTACK );
			CHAR_setPetSkill( enemyindex, 1, PETSKILL_NORMALATTACK );
		}else{
			//   溃  湛元扎卅中及匹  勾仃月
			//   毛尥仃月
			CHAR_setPetSkill( enemyindex, 0,
				EnemyGymSkill[RAND( 0, arraysizeof( EnemyGymSkill ) - 1 )] );
			//   毛尥仃月
			CHAR_setPetSkill( enemyindex, 1,
				EnemyGymSkill[RAND( 0, arraysizeof( EnemyGymSkill ) - 1 )] );
		}

	}else
	if( iRet == 2 ){
		//   溃  湛元扎卅中及匹  勾仃月
		//   毛尥仃月
		CHAR_setPetSkill( enemyindex, 0,
			EnemyGymSkill[RAND( 0, arraysizeof( EnemyGymSkill ) - 1 )] );
		//   毛尥仃月
		CHAR_setPetSkill( enemyindex, 1,
			EnemyGymSkill[RAND( 0, arraysizeof( EnemyGymSkill ) - 1 )] );
	}

	return 1;

}


/*------------------------------------------------------------------------
 * ENEMY_enemy井日平乓仿弁正□毛综岳允月［
 *-----------------------------------------------------------------------*/
int ENEMY_createEnemy( int array, int baselevel )
{
    Char    CharNew;
    int newindex;
    int     *p;
    int		tp[E_T_DATAINTNUM];
    int		tarray, i;
	int itemindex,iloop;
	int		level;
	int	enemyrank;

    if( !ENEMY_CHECKINDEX( array)) return -1;

    p = ENEMY_enemy[array].intdata;
    tarray = ENEMYTEMP_getEnemyTempArray( array);
	if( !ENEMYTEMP_CHECKINDEX( tarray)) return -1;
//    tp = ENEMYTEMP_enemy[tarray].intdata;
	// 犯□正戊疋□
	for( i = 0; i < E_T_DATAINTNUM; i ++ ){
	    tp[i] = ENEMYTEMP_enemy[tarray].intdata[i];
	}

    //     赓渝祭
    memset( &CharNew, 0, sizeof( Char ) );

    if( !CHAR_getDefaultChar( &CharNew,31010 ) )return -1;

    /*    飓  寞    */
    CharNew.data[CHAR_BASEBASEIMAGENUMBER]
        = CharNew.data[CHAR_BASEIMAGENUMBER] = *(tp+E_T_IMGNUMBER);
    /*  衬匹丐月午涩烂允月   */
    CharNew.data[CHAR_WHICHTYPE] = CHAR_TYPEENEMY;
    /*     毛壅允 */
    CharNew.data[CHAR_DUELPOINT] = 0;

	/* 伊矛伙毛瑁烂允月［ */
	if( baselevel > 0 ){
		level = baselevel;	// 伊矛伙裔烂
	}else{
		level = RAND( (*(p + ENEMY_LV_MIN)), (*(p+ ENEMY_LV_MAX)));
	}
#define		E_PAR( a)		(*(p + (a)))
#define		ET_PAR( a)		(*(tp + (a)))
/* 由仿丢□正隙醒井日  端及湘  由仿丢□正尺及煌遥挚 */
#if 1
#define		PARAM_CAL( l) 	( ( level -1)*ET_PAR( E_T_LVUPPOINT)+ ET_PAR(E_T_INITNUM)) * ET_PAR( (l))
#else
#define		PARAM_CAL( l) 	( (E_PAR(ENEMY_LV) -1)*ET_PAR( E_T_LVUPPOINT)+ ET_PAR(E_T_INITNUM)) * ET_PAR( (l))
#endif
	/* 燮内湘  喃曰蕊曰禾奶件玄卞＋ㄡ毛仿件母丞匹垫丹 */
	tp[E_T_BASEVITAL] += RAND( 0, 4 ) - 2;
	tp[E_T_BASESTR] += RAND( 0, 4 ) - 2;
	tp[E_T_BASETGH] += RAND( 0, 4 ) - 2;
	tp[E_T_BASEDEX] += RAND( 0, 4 ) - 2;
	/* 仇及凛鳔匹喃曰蕊曰禾奶件玄毛忡绣仄化云仁 */
    CharNew.data[CHAR_ALLOCPOINT]
    = ( tp[E_T_BASEVITAL] << 24 )
    + ( tp[E_T_BASESTR] << 16 )
    + ( tp[E_T_BASETGH] << 8 )
    + ( tp[E_T_BASEDEX] << 0 );
	/* 公及  ｝仿件母丞匹禾奶件玄毛ㄠ勿勾笛遥［仇木毛ㄠㄟ荚楞曰忒允［*/
	for( i = 0; i < 10; i ++ ){
		int work = RAND( 0, 3 );
		if( work == 0 )tp[E_T_BASEVITAL]++;
		if( work == 1 )tp[E_T_BASESTR]++;
		if( work == 2 )tp[E_T_BASETGH]++;
		if( work == 3 )tp[E_T_BASEDEX]++;
	}

    /* 由仿丢□正本永玄 */
    CharNew.data[CHAR_VITAL]    = PARAM_CAL(E_T_BASEVITAL);
    CharNew.data[CHAR_STR]      = PARAM_CAL(E_T_BASESTR);
    CharNew.data[CHAR_TOUGH]    = PARAM_CAL(E_T_BASETGH);
    CharNew.data[CHAR_DEX]      = PARAM_CAL(E_T_BASEDEX);
    CharNew.data[CHAR_FIREAT]   = *(tp+ E_T_FIREAT);
    CharNew.data[CHAR_WATERAT]  = *(tp+ E_T_WATERAT);
    CharNew.data[CHAR_EARTHAT]  = *(tp+ E_T_EARTHAT);
    CharNew.data[CHAR_WINDAT]   = *(tp+ E_T_WINDAT);
    CharNew.data[CHAR_MODAI]    = *(tp+ E_T_MODAI);
    CharNew.data[CHAR_VARIABLEAI]    = 0;
	CharNew.data[CHAR_LV]       = level;
	//CharNew.data[CHAR_LV]       = *(p+ ENEMY_LV);
    CharNew.data[CHAR_SLOT]     = *(tp+ E_T_SLOT);
    CharNew.data[CHAR_POISON]   = *(tp+ E_T_POISON);
    CharNew.data[CHAR_PARALYSIS]= *(tp+ E_T_PARALYSIS);
    CharNew.data[CHAR_SLEEP]	= *(tp+ E_T_SLEEP);
    CharNew.data[CHAR_STONE]	= *(tp+ E_T_STONE);
    CharNew.data[CHAR_DRUNK]	= *(tp+ E_T_DRUNK);
    CharNew.data[CHAR_CONFUSION]= *(tp+ E_T_CONFUSION);
	CharNew.data[CHAR_RARE]		= *(tp+ E_T_RARE);
	CharNew.data[CHAR_PETID]	= *(tp + E_T_TEMPNO);
	CharNew.data[CHAR_CRITIAL]	= *(tp + E_T_CRITICAL);
	CharNew.data[CHAR_COUNTER]	= *(tp + E_T_COUNTER);
	
	for( i = 0; i < CHAR_MAXPETSKILLHAVE; i ++ ) {
		CharNew.unionTable.indexOfPetskill[i] = *(tp + E_T_PETSKILL1 + i);
	}
	enemyrank = ENEMY_getRank( array, tarray );
	CharNew.data[CHAR_PETRANK]	= enemyrank;
#undef	E_PAR
#undef	ET_PAR
#undef	PARAM_CAL
	CharNew.data[CHAR_DUELPOINT] = *( p + ENEMY_DUELPOINT);
	if( *( p + ENEMY_DUELPOINT) <= 0 ) {
		if( *(p + ENEMY_EXP) != -1 ) {
		    CharNew.data[CHAR_EXP]      = *( p+ENEMY_EXP);
		}else {
		    CharNew.data[CHAR_EXP]      = ENEMY_getExp( array,tarray,level, enemyrank);
		}
	}
    strcpysafe( CharNew.string[CHAR_NAME].string,
                sizeof(CharNew.string[CHAR_NAME].string),
                (char *)ENEMYTEMP_enemy[tarray].chardata[E_T_NAME].string );

    newindex = CHAR_initCharOneArray( &CharNew );
    if( newindex < 0 ){
        return -1;
    }
	for( iloop=0; iloop< (ENEMY_ITEMPROB10 - ENEMY_ITEM1+1)/2; iloop++){
		if( *(p+ENEMY_ITEMPROB1+iloop) != 0) {
#ifdef _FIX_ITEMPROB
			if( RAND( 0,999) < *(p+ENEMY_ITEMPROB1+iloop)){
#else
			if( RAND( 0,99) < *(p+ENEMY_ITEMPROB1+iloop)){
#endif
				itemindex = ITEM_makeItemAndRegist(  *(p+ENEMY_ITEM1+iloop) );
				CHAR_setItemIndex( newindex, CHAR_STARTITEMARRAY +iloop, itemindex);
				ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, newindex );
		        ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX,-1);

			}
		}
	}

	{ int style, wepon = -1;
		style = (*(p + ENEMY_STYLE));
		switch( style ){
		case 1:		wepon = 0;break;	//   
		case 2:		wepon = 100;break;  // 轺徇
		case 3:		wepon = 200;break;	// 键
		case 4:		wepon = 400;break;	// 菰
		case 5:		wepon = 500;break;	// 皮□丢仿件
		case 6:		wepon = 700;break;  // 髑仆檗
		case 7:		wepon = 600;break;	// 髑仆  
		default:break;
		}
		if( wepon >= 0 ){
			itemindex = ITEM_makeItemAndRegist( wepon ) ;
			CHAR_setItemIndex( newindex, CHAR_ARM, itemindex );
			ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, newindex );
	        ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX,-1);
		}
	}

	ENEMY_RandomChange( newindex, *( p+ENEMY_ID) );
    CHAR_complianceParameter( newindex );

	CHAR_setInt( newindex, CHAR_HP, CHAR_getWorkInt( newindex, CHAR_WORKMAXHP));
	CHAR_setWorkInt( newindex, CHAR_WORKTACTICS, *(p+ENEMY_TACTICS));
	CHAR_setWorkChar( newindex, CHAR_WORKBATTLE_TACTICSOPTION,
                		ENEMY_enemy[array].chardata[ENEMY_TACTICSOPTION].string );
#ifdef _BATTLENPC_WARP_PLAYER	
	CHAR_setWorkChar( newindex, CHAR_WORKBATTLE_ACT_CONDITION,
                		ENEMY_enemy[array].chardata[ENEMY_ACT_CONDITION].string );
#endif
	CHAR_setWorkInt( newindex, CHAR_WORK_PETFLG, *(p+ENEMY_PETFLG));
	CHAR_setWorkInt( newindex, CHAR_WORKMODCAPTUREDEFAULT, *(tp + E_T_GET));
#ifdef _ENEMY_FALLGROUND
	{
		int i=0;
		for( i=0;i<arraysizeof( ridePetTable);i++)	{			
			if( CHAR_getInt( newindex, CHAR_BASEBASEIMAGENUMBER) == ridePetTable[i].rideNo )	{
				CHAR_setInt( newindex, CHAR_BASEBASEIMAGENUMBER, ridePetTable[i].charNo );
				CHAR_setInt( newindex, CHAR_BASEIMAGENUMBER, ridePetTable[i].rideNo );
				break;
			}
		}
	}
#endif
#ifdef _PETSKILL_BECOMEFOX //将媚惑术中招时间清除为-1
     CHAR_setWorkInt( newindex, CHAR_WORKFOXROUND, -1 ); 
#endif
    return newindex;

}


#define RANDOMENEMY_TOP		945
#define RANDOMENEMY_END		956

static int RandomHuman945[] = {739,742,745,748};
static int RandomPet946[] = {859,860,861,868,869,870,881,882,883,884};
static int RandomPet964[] = {859,860,861,868,869,870,881,882,883,884};

static int RandomHuman947[] = {740,743,746,749};
static int RandomPet948[] = {864,871,872,873,885,886,889,890,891,892,893,894};
static int RandomPet965[] = {864,871,872,873,885,886,889,890,891,892,893,894};

static int RandomHuman949[] = {741,744,747,750};
static int RandomPet950[] = {862,863,865,866,867,874,875,876,877,878,879,880};
static int RandomPet966[] = {862,863,865,866,867,874,875,876,877,878,879,880};

static int RandomHuman951[] = {895,898,901,904};
static int RandomPet952[] = {912,913,914,915,916,917,918,919,920,928};
static int RandomPet967[] = {912,913,914,915,916,917,918,919,920,928};

static int RandomHuman953[] = {896,899,902,905};
static int RandomPet954[] = {929,930,931,932,933,934,935,936,937,938,939,940};
static int RandomPet968[] = {929,930,931,932,933,934,935,936,937,938,939,940};

static int RandomHuman955[] = {897,900,903,906};
static int RandomPet956[] = {907,908,909,910,911,921,922,923,924,925,926,927};
static int RandomPet969[] = {907,908,909,910,911,921,922,923,924,925,926,927};

typedef struct {
	int num;
	int *pTbl;
	int arraysize;
}RANDOMENEMY;

static RANDOMENEMY RandomEnemyTbl[] = {
	{ 945, RandomHuman945, arraysizeof( RandomHuman945 ) },
	{ 946, RandomPet946,	arraysizeof( RandomPet946 ) },
	{ 947, RandomHuman947, arraysizeof( RandomHuman947 ) },
	{ 948, RandomPet948, arraysizeof( RandomPet948 ) },
	{ 949, RandomHuman949, arraysizeof( RandomHuman949 ) },
	{ 950, RandomPet950, arraysizeof( RandomPet950 ) },
	{ 951, RandomHuman951, arraysizeof( RandomHuman951 ) },
	{ 952, RandomPet952, arraysizeof( RandomPet952 ) },
	{ 953, RandomHuman953, arraysizeof( RandomHuman953 ) },
	{ 954, RandomPet954, arraysizeof( RandomPet954 ) },
	{ 955, RandomHuman955, arraysizeof( RandomHuman955 ) },
	{ 956, RandomPet956, arraysizeof( RandomPet956 ) },

	{ 964, RandomPet964, arraysizeof( RandomPet964 ) },
	{ 965, RandomPet965, arraysizeof( RandomPet965 ) },
	{ 966, RandomPet966, arraysizeof( RandomPet966 ) },
	{ 967, RandomPet967, arraysizeof( RandomPet967 ) },
	{ 968, RandomPet968, arraysizeof( RandomPet968 ) },
	{ 969, RandomPet969, arraysizeof( RandomPet969 ) },
};


/*------------------------------------------------------------
 * 衬    涩烂及摹    寞互  溃卅袄卅日仿件母丞卞涩烂允月
 ------------------------------------------------------------*/
int ENEMY_RandomEnemyArray( int e_array, int *pNew )
{
	int i = 0, randwork, work;
//	RANDOMENEMY *pRandomEnemy;
	*pNew = -1;
	// 仇及  区反仿件母丞
	if(
		( RANDOMENEMY_TOP <= e_array && e_array <= RANDOMENEMY_END )
	||	( 964 <= e_array && e_array <= 969 )
	){
		// 升及母立□井譬屯月
		for( i = 0; i < arraysizeof( RandomEnemyTbl ); i ++ ){
			if( RandomEnemyTbl[i].num == e_array ){
				break;
			}
		}
		//   区毛译尹化中凶日巨仿□匹  仃月
		if( i >= arraysizeof( RandomEnemyTbl ) ) return 0;

		// 仿件母丞涩烂
		randwork = RAND( 0, RandomEnemyTbl[i].arraysize - 1 );
		//     井日蕙仄中  寞毛潸  
		work = RandomEnemyTbl[i].pTbl[randwork];
		*pNew = ENEMY_getEnemyArrayFromId( work );
		return 1;
	}else{
		// 窒仪手卅中
		return 0;
	}
}

int *ENEMY_getEnemy( int charaindex, int x, int y)
{
    int     i;
    int		array;
    int		g_array;
    int		e_array;
    int     found = 0;
    int		work[ENEMY_INDEXTABLEMAXSIZE];
    int		wr[ENEMY_INDEXTABLEMAXSIZE];
    int		createenemynum;
    int		enemyentrymax;
    int		entrymax;
    int		r_max;
    int		groupid;
	int		bigcnt;
	int		loopcounter;
    array = ENCOUNT_getEncountAreaArray( CHAR_getInt( charaindex, CHAR_FLOOR),x,y);
	if( array == -1 ) return NULL;
    enemyentrymax = ENCOUNT_getCreateEnemyMaxNumFromArray( array);
    if( enemyentrymax == -1 ) {
    	return NULL;
    }
    for( i = 0 ; i < ENEMY_INDEXTABLEMAXSIZE; i ++ ) {
        work[i] = -1;
		wr[i] = -1;
        ENEMY_indextable[i] = -1;
    }

	r_max= 0;
	for( i = 0; i < ENCOUNT_GROUPMAXNUM; i ++ ){
		if( ENCOUNT_getGroupIdFromArray( array, i ) != - 1 ) {
			int		itemid;
			groupid = ENCOUNT_getGroupIdFromArray( array, i);
			g_array = GROUP_getGroupArray( groupid);
            itemid = GROUP_getInt( g_array, GROUP_APPEARBYITEMID);
            if( itemid != -1 ) {
            	int		j;
            	for( j = 0; j < CHAR_MAXITEMHAVE; j ++ ) {
            		int itemindex = CHAR_getItemIndex( charaindex, j);
            		if( ITEM_CHECKINDEX( itemindex)) {
            			if( ITEM_getInt( itemindex, ITEM_ID) == itemid){
            				break;
            			}
            		}
            	}
            	if( j == CHAR_MAXITEMHAVE ) continue;
            }
            itemid = GROUP_getInt( g_array, GROUP_NOTAPPEARBYITEMID);
            if( itemid != -1 ) {
            	int		j;
            	for( j = 0; j < CHAR_MAXITEMHAVE; j ++ ) {
            		int itemindex = CHAR_getItemIndex( charaindex, j);
            		if( ITEM_CHECKINDEX( itemindex)) {
            			if( ITEM_getInt( itemindex, ITEM_ID) == itemid){
            				break;
            			}
            		}
            	}
            	if( j != CHAR_MAXITEMHAVE ) continue;
            }

			work[found] = i;
            wr[found] = ENCOUNT_getGroupProbFromArray( array, i);
            r_max += wr[found];
			found ++;
		}
	}
	r_max --;

	if( found <= 0 ) return NULL;
	{
		int	aaa = 0;
		int	r;
		r = RAND( 0, r_max);
		for( i = 0; i < found-1; i ++ ) {
			aaa += wr[i];
			if( wr[i] != 0 && r < aaa ) break;
		}
		r = i;

		groupid = ENCOUNT_getGroupIdFromArray( array, work[r]);
		g_array = GROUP_getGroupArray( groupid);
    }
    for( i = 0 ; i < ENEMY_INDEXTABLEMAXSIZE; i ++ ) {
        work[i] = -1;
		wr[i] = -1;
    }
	createenemynum = 0;
	found = 0;
	r_max = 0;
    for( i = ENEMY_ID1; i < CREATEPROB1; i ++ ) {
		int newarray, enemywork;
		e_array = ENEMY_getEnemyArrayFromIndex( g_array, i - ENEMY_ID1 );

#ifdef _ADD_ENCOUNT           // WON ADD 增加敌遭遇触发修件
		{	
		int char_event_now = -1, char_event_end = -1;
		if(ENCOUNT_table[array].event_now != -1){
			char_event_now = ENCOUNT_table[array].event_now;
		}
		else if(ENCOUNT_table[array].event_end != -1){
			char_event_end = ENCOUNT_table[array].event_end;
		}
		if( (char_event_now > 0) || ( char_event_end > 0 ) ) {
			if( (NPC_NowEventCheckFlg( charaindex, char_event_now) == TRUE ) ||
				(NPC_EventCheckFlg( charaindex, char_event_end) == TRUE )    ){
					
				e_array = ENEMY_getEnemyArrayFromIndex( 
					   GROUP_getGroupArray( ENCOUNT_table[array].enemy_group ),i - ENEMY_ID1 );
			}
		}	
		}
#endif
		enemywork = ENEMY_getInt( e_array, ENEMY_ID );
		if( ENEMY_RandomEnemyArray( enemywork, &newarray ) ){
			e_array = newarray;
		}

		if( e_array != -1 ) {
            work[found] = e_array;
            wr[found] = GROUP_getInt( g_array, i + (CREATEPROB1 - ENEMY_ID1));
            r_max += wr[found];
            found ++;
            createenemynum += ENEMY_getInt( e_array, ENEMY_CREATEMAXNUM);
        }
    }
    r_max --;

	if( found <= 0 ) return NULL;
    enemyentrymax= min( enemyentrymax, createenemynum );
	entrymax = RAND( 1, enemyentrymax);
	bigcnt = 0;
    for( i = 0,loopcounter = 0; i < entrymax && loopcounter < 100; loopcounter ++) {
		int	cnt;
    	int	j,r;
		int	aaa = 0;
		int		et_array;
		int	k,samecount;
		r = RAND( 0, r_max);
		for( j = 0; j < found-1; j ++ ) {
			aaa += wr[j];
			if( wr[j] != 0 && r < aaa ) break;
		}
		r = j;
		cnt = 0;
		for( j = 0; j < ENEMY_INDEXTABLEMAXSIZE && ENEMY_indextable[j] != -1; j ++ ) {
			if( ENEMY_indextable[j] == work[r] ) cnt ++;
		}
		samecount = 0;
		for( k = 0; k < found; k ++ ) {
			if( work[r] == work[k] ) samecount ++;
		}
    	if( cnt >= ENEMY_getInt( work[r], ENEMY_CREATEMAXNUM) * samecount ) {
    		continue;
    	}

		et_array = ENEMYTEMP_getEnemyTempArray( work[r]);
		if( ENEMYTEMP_CHECKINDEX( et_array)) {
			if( ENEMYTEMP_getInt( et_array, E_T_SIZE) == E_T_SIZE_BIG ) {
				if( bigcnt >= 5 ) {
					entrymax --;
					continue;
				}
				if( i > 4 ) {
					int	target_et_array;
					int	flg = FALSE;
					for( j = 0; j < 5; j ++ ) {
						if( !ENEMY_CHECKINDEX( ENEMY_indextable[j])) break;

						target_et_array = ENEMYTEMP_getEnemyTempArray(
														ENEMY_indextable[j]);
						if( !ENEMYTEMP_CHECKINDEX( target_et_array)) break;
						if( ENEMYTEMP_getInt( target_et_array, E_T_SIZE) == E_T_SIZE_NORMAL ){
							flg = TRUE;
							break;
						}
					}
					if( !flg) continue;
					ENEMY_indextable[i] = ENEMY_indextable[j];
					ENEMY_indextable[j] = work[r];
				}
				else {
					ENEMY_indextable[i] = work[r];
				}
				bigcnt ++;
			}else {
				ENEMY_indextable[i] = work[r];
    		}

			i++;
    	}
    }
    return found > 0 ? ENEMY_indextable : NULL;
}
int ENEMY_createPetFromEnemyIndex( int charaindex, int array)
{
    Char    CharNew;
    int newindex;
    int     *p;
    int		tp[E_T_DATAINTNUM];
    int		tarray, i;
    int     havepetelement;
	int		level;
	int		enemyrank;
    if( !ENEMY_CHECKINDEX( array)) return -1;
    havepetelement = CHAR_getCharPetElement( charaindex);
    if( havepetelement < 0 ) return -1;

    p = ENEMY_enemy[array].intdata;
	tarray = ENEMYTEMP_getEnemyTempArray( array);
	if( !ENEMYTEMP_CHECKINDEX( tarray)) return -1;
//    tp = ENEMYTEMP_enemy[tarray].intdata;
	for( i = 0; i < E_T_DATAINTNUM; i ++ ){
	    tp[i] = ENEMYTEMP_enemy[tarray].intdata[i];
	}
    memset( &CharNew, 0, sizeof( Char ) );
    if( !CHAR_getDefaultChar( &CharNew,31010 ) )return -1;
    CharNew.data[CHAR_BASEBASEIMAGENUMBER]
        = CharNew.data[CHAR_BASEIMAGENUMBER] = *(tp+E_T_IMGNUMBER);
    CharNew.data[CHAR_WHICHTYPE] = CHAR_TYPEPET;
	level = RAND( (*(p + ENEMY_LV_MIN)), ( *(p+ ENEMY_LV_MAX)));

#define		E_PAR( a)		(*(p + (a)))
#define		ET_PAR( a)		(*(tp + (a)))
#if 1
#define		PARAM_CAL( l) 	(( level -1)*ET_PAR( E_T_LVUPPOINT)+ ET_PAR(E_T_INITNUM)) * ET_PAR( (l))
#else
#define		PARAM_CAL( l)	( (E_PAR(ENEMY_LV) -1)*ET_PAR( E_T_LVUPPOINT)+ ET_PAR(E_T_INITNUM))	* ET_PAR( (l))
#endif
	tp[E_T_BASEVITAL] += RAND( 0, 4 ) - 2;
	tp[E_T_BASESTR] += RAND( 0, 4 ) - 2;
	tp[E_T_BASETGH] += RAND( 0, 4 ) - 2;
	tp[E_T_BASEDEX] += RAND( 0, 4 ) - 2;
    CharNew.data[CHAR_ALLOCPOINT]
    = ( tp[E_T_BASEVITAL] << 24 )
    + ( tp[E_T_BASESTR] << 16 )
    + ( tp[E_T_BASETGH] << 8 )
    + ( tp[E_T_BASEDEX] << 0 );

	for( i = 0; i < 10; i ++ ){
		int work = RAND( 0, 3 );
		if( work == 0 )tp[E_T_BASEVITAL]++;
		if( work == 1 )tp[E_T_BASESTR]++;
		if( work == 2 )tp[E_T_BASETGH]++;
		if( work == 3 )tp[E_T_BASEDEX]++;
	}
    /* 由仿丢□正本永玄 */
    CharNew.data[CHAR_VITAL]    = PARAM_CAL(E_T_BASEVITAL);
    CharNew.data[CHAR_STR]      = PARAM_CAL(E_T_BASESTR);
    CharNew.data[CHAR_TOUGH]    = PARAM_CAL(E_T_BASETGH);
    CharNew.data[CHAR_DEX]      = PARAM_CAL(E_T_BASEDEX);

    CharNew.data[CHAR_FIREAT]   = *(tp+ E_T_FIREAT);
    CharNew.data[CHAR_WATERAT]  = *(tp+ E_T_WATERAT);
    CharNew.data[CHAR_EARTHAT]  = *(tp+ E_T_EARTHAT);
    CharNew.data[CHAR_WINDAT]   = *(tp+ E_T_WINDAT);

    CharNew.data[CHAR_SLOT]     = *(tp+ E_T_SLOT);
    CharNew.data[CHAR_MODAI]    = *(tp+ E_T_MODAI);
    CharNew.data[CHAR_VARIABLEAI]    = 0;
	CharNew.data[CHAR_LV]       = level;
	//CharNew.data[CHAR_LV]       = *(p+ ENEMY_LV);
    CharNew.data[CHAR_POISON]   = *(tp+ E_T_POISON);
    CharNew.data[CHAR_PARALYSIS]= *(tp+ E_T_PARALYSIS);
    CharNew.data[CHAR_SLEEP]	= *(tp+ E_T_SLEEP);
    CharNew.data[CHAR_STONE]	= *(tp+ E_T_STONE);
    CharNew.data[CHAR_DRUNK]	= *(tp+ E_T_DRUNK);
    CharNew.data[CHAR_CONFUSION]= *(tp+ E_T_CONFUSION);
    CharNew.data[CHAR_RARE]     = *(tp+ E_T_RARE);
	CharNew.data[CHAR_PETID]	= *(tp + E_T_TEMPNO);
	CharNew.data[CHAR_CRITIAL]	= *(tp + E_T_CRITICAL);
	CharNew.data[CHAR_COUNTER]	= *(tp + E_T_COUNTER);
    // Arminius 8.6 limit lv
    CharNew.data[CHAR_LIMITLEVEL]	= *(tp + E_T_LIMITLEVEL);
#ifdef _PET_FUSION
	CharNew.data[CHAR_FUSIONCODE]	= *(tp + E_T_FUSIONCODE);
#endif
	CharNew.data[CHAR_PETMAILEFFECT] = RAND(0, PETMAIL_EFFECTMAX);
	for( i = 0; i < CHAR_MAXPETSKILLHAVE; i ++ ) {
		CharNew.unionTable.indexOfPetskill[i] = *(tp + E_T_PETSKILL1 + i);
	}
	enemyrank = ENEMY_getRank( array, tarray );
	CharNew.data[CHAR_PETRANK]	= enemyrank;
#undef	E_PAR
#undef	ET_PAR
#undef	PARAM_CAL
    strcpysafe( CharNew.string[CHAR_NAME].string,
                sizeof(CharNew.string[CHAR_NAME].string),
                (char *)ENEMYTEMP_enemy[tarray].chardata[E_T_NAME].string );
    newindex = PET_initCharOneArray( &CharNew );
    if( newindex < 0 ){
        return -1;
    }
    CHAR_complianceParameter( newindex );
	CHAR_setInt( newindex, CHAR_HP, CHAR_getWorkInt( newindex, CHAR_WORKMAXHP));
    CHAR_setWorkInt( newindex, CHAR_WORKPLAYERINDEX, charaindex);
    CHAR_setCharPet( charaindex, havepetelement, newindex);
	CHAR_setChar( newindex, CHAR_OWNERCDKEY,
				CHAR_getChar( charaindex, CHAR_CDKEY));
	CHAR_setChar( newindex, CHAR_OWNERCHARANAME,
				CHAR_getChar( charaindex, CHAR_NAME));
    return newindex;
}

#ifdef _TEST_PETCREATE
int ENEMY_TEST_createPetIndex( int array)
{
    Char    CharNew;
    int newindex;
    int     *p;
    int		tp[E_T_DATAINTNUM];
    int		tarray, i;
    //int     havepetelement;
	int		level;
	int		enemyrank;
    if( !ENEMY_CHECKINDEX( array)) return -1;

    //havepetelement = CHAR_getCharPetElement( charaindex);
    //if( havepetelement < 0 ) return -1;
    p = ENEMY_enemy[array].intdata;
	tarray = ENEMYTEMP_getEnemyTempArray( array);
	if( !ENEMYTEMP_CHECKINDEX( tarray)) return -1;
	for( i = 0; i < E_T_DATAINTNUM; i ++ ){
	    tp[i] = ENEMYTEMP_enemy[tarray].intdata[i];
	}
    memset( &CharNew, 0, sizeof( Char ) );
    if( !CHAR_getDefaultChar( &CharNew,31010 ) )return -1;
    CharNew.data[CHAR_BASEBASEIMAGENUMBER]
        = CharNew.data[CHAR_BASEIMAGENUMBER] = *(tp+E_T_IMGNUMBER);
    CharNew.data[CHAR_WHICHTYPE] = CHAR_TYPEPET;
	level = RAND( (*(p + ENEMY_LV_MIN)), ( *(p+ ENEMY_LV_MAX)));

#define		E_PAR( a)		(*(p + (a)))
#define		ET_PAR( a)		(*(tp + (a)))

#if 1
#define		PARAM_CAL( l) 	(( level -1)*ET_PAR( E_T_LVUPPOINT)+ ET_PAR(E_T_INITNUM)) * ET_PAR( (l))
#else
#define		PARAM_CAL( l)	( (E_PAR(ENEMY_LV) -1)*ET_PAR( E_T_LVUPPOINT)+ ET_PAR(E_T_INITNUM))	* ET_PAR( (l))
#endif
	tp[E_T_BASEVITAL] += RAND( 0, 4 ) - 2;
	tp[E_T_BASESTR] += RAND( 0, 4 ) - 2;
	tp[E_T_BASETGH] += RAND( 0, 4 ) - 2;
	tp[E_T_BASEDEX] += RAND( 0, 4 ) - 2;
    CharNew.data[CHAR_ALLOCPOINT]
    = ( tp[E_T_BASEVITAL] << 24 )
    + ( tp[E_T_BASESTR] << 16 )
    + ( tp[E_T_BASETGH] << 8 )
    + ( tp[E_T_BASEDEX] << 0 );
	for( i = 0; i < 10; i ++ ){
		int work = RAND( 0, 3 );
		if( work == 0 )tp[E_T_BASEVITAL]++;
		if( work == 1 )tp[E_T_BASESTR]++;
		if( work == 2 )tp[E_T_BASETGH]++;
		if( work == 3 )tp[E_T_BASEDEX]++;
	}
    CharNew.data[CHAR_VITAL]    = PARAM_CAL(E_T_BASEVITAL);
    CharNew.data[CHAR_STR]      = PARAM_CAL(E_T_BASESTR);
    CharNew.data[CHAR_TOUGH]    = PARAM_CAL(E_T_BASETGH);
    CharNew.data[CHAR_DEX]      = PARAM_CAL(E_T_BASEDEX);
    CharNew.data[CHAR_FIREAT]   = *(tp+ E_T_FIREAT);
    CharNew.data[CHAR_WATERAT]  = *(tp+ E_T_WATERAT);
    CharNew.data[CHAR_EARTHAT]  = *(tp+ E_T_EARTHAT);
    CharNew.data[CHAR_WINDAT]   = *(tp+ E_T_WINDAT);

    CharNew.data[CHAR_SLOT]     = *(tp+ E_T_SLOT);
    CharNew.data[CHAR_MODAI]    = *(tp+ E_T_MODAI);
    CharNew.data[CHAR_VARIABLEAI]    = 0;
	CharNew.data[CHAR_LV]       = level;
    CharNew.data[CHAR_POISON]   = *(tp+ E_T_POISON);
    CharNew.data[CHAR_PARALYSIS]= *(tp+ E_T_PARALYSIS);
    CharNew.data[CHAR_SLEEP]	= *(tp+ E_T_SLEEP);
    CharNew.data[CHAR_STONE]	= *(tp+ E_T_STONE);
    CharNew.data[CHAR_DRUNK]	= *(tp+ E_T_DRUNK);
    CharNew.data[CHAR_CONFUSION]= *(tp+ E_T_CONFUSION);
    CharNew.data[CHAR_RARE]     = *(tp+ E_T_RARE);
	CharNew.data[CHAR_PETID]	= *(tp + E_T_TEMPNO);
	CharNew.data[CHAR_CRITIAL]	= *(tp + E_T_CRITICAL);
	CharNew.data[CHAR_COUNTER]	= *(tp + E_T_COUNTER);
    // Arminius 8.6 limit lv
    CharNew.data[CHAR_LIMITLEVEL]	= *(tp + E_T_LIMITLEVEL);
#ifdef _PET_FUSION
	CharNew.data[CHAR_FUSIONCODE]	= *(tp + E_T_FUSIONCODE);
#endif
	CharNew.data[CHAR_PETMAILEFFECT] = RAND(0, PETMAIL_EFFECTMAX);
	for( i = 0; i < CHAR_MAXPETSKILLHAVE; i ++ ) {
		CharNew.unionTable.indexOfPetskill[i] = *(tp + E_T_PETSKILL1 + i);
	}
	enemyrank = ENEMY_getRank( array, tarray );
	CharNew.data[CHAR_PETRANK]	= enemyrank;
#undef	E_PAR
#undef	ET_PAR
#undef	PARAM_CAL
    strcpysafe( CharNew.string[CHAR_NAME].string,
                sizeof(CharNew.string[CHAR_NAME].string),
                (char *)ENEMYTEMP_enemy[tarray].chardata[E_T_NAME].string );
    newindex = PET_initCharOneArray( &CharNew );
    if( newindex < 0 ){
        return -1;
    }
    CHAR_complianceParameter( newindex );
	CHAR_setInt( newindex, CHAR_HP, CHAR_getWorkInt( newindex, CHAR_WORKMAXHP));
//  CHAR_setWorkInt( newindex, CHAR_WORKPLAYERINDEX, charaindex);
//  CHAR_setCharPet( charaindex, havepetelement, newindex);
//	CHAR_setChar( newindex, CHAR_OWNERCDKEY,
//	CHAR_getChar( charaindex, CHAR_CDKEY));
//	CHAR_setChar( newindex, CHAR_OWNERCHARANAME,
//	CHAR_getChar( charaindex, CHAR_NAME));
    return newindex;
}
#endif

#ifdef _PET_EVOLUTION

BOOL PETFUSION_getIndexForChar( int toindex, int *MainIndex, int *Subindex1, int *Subindex2, char *data)
{
	char buf1[256];
	int pindex[3]={-1,-1,-1};
	int i;
	if( getStringFromIndexWithDelim( data, "|", 1, buf1, sizeof(buf1)) == FALSE )
		return FALSE;
	pindex[0] = atoi( buf1)-1;
	if( getStringFromIndexWithDelim( data, "|", 2, buf1, sizeof(buf1)) == FALSE )
		return FALSE;
	pindex[1] = atoi( buf1)-1;
	if( getStringFromIndexWithDelim( data, "|", 3, buf1, sizeof(buf1)) == FALSE )
		return FALSE;
	pindex[2] = atoi( buf1)-1;
	for( i=0; i<3; i++)	{
		int petindex;
		if( pindex[i] < 0 ) continue;
		petindex = CHAR_getCharPet( toindex, pindex[i]);
		if( !CHAR_CHECKINDEX( petindex) ) continue;
#ifdef _PET_2TRANS
		if( CHAR_getInt( petindex, CHAR_TRANSMIGRATION ) > 1 ) {
			CHAR_talkToCli( toindex, -1, "二转宠物不能融合。", CHAR_COLORYELLOW);
			return FALSE;
		}
#endif
		if( i == 0 ) {
			*MainIndex = petindex;
		}else if( i == 1)	{
			*Subindex1 = petindex;
		}else if( i == 2)	{
			*Subindex2 = petindex;
		}
	}
	return TRUE;
}
int NPC_getPetArrayForNo( int PetCode)
{
	int i;
	int enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {//PetCode
		if( ENEMY_getInt( i, ENEMY_TEMPNO ) == PetCode )
		break;
	}

	if( i == enemynum ){
		print("ANDY 2.err i == enemynum \n");
		return -1;
	}
	return i;
}
int NPC_getFusionTableForBase( int charaindex, int petindex1, int petindex2 )
{
	int base1, base2;// 属性,PETCODE
	if( !CHAR_CHECKINDEX( petindex1) ) return -1;
	if( !CHAR_CHECKINDEX( petindex2) ) return -1;
#ifdef _PET_EVOLUTION
	if( (base2 = EVOLUTION_getPetTable( charaindex, petindex1, petindex2)) < 0 ){
		print("ANDY err base2=%d\n", base2);
		return -1;
	}
	if( (base1 = EVOLUTION_getPropertyTable( charaindex, petindex1, petindex2)) < 0 ){
		print("ANDY err base1=%d\n", base1);
		return -1;
	}
	return EVOLUTION_getFusionTable( charaindex, base2, base1); // get new pet IDNO
#else
	return -1;
#endif
}

BOOL PET_getEvolutionAns( int petindex, int *base)
{
	int i, total1, total2, total;
	int defwork = 50, defbase = 150;
	int work[4]={0,0,0,0};

	work[0] = CHAR_getInt( petindex, CHAR_EVOLUTIONBASEVTL);
	work[1] = CHAR_getInt( petindex, CHAR_EVOLUTIONBASESTR);
	work[2] = CHAR_getInt( petindex, CHAR_EVOLUTIONBASETGH);
	work[3] = CHAR_getInt( petindex, CHAR_EVOLUTIONBASEDEX);
	for( i=0; i<4; i++)	{
		work[i] = (work[i]*0.7)/100;
		if( work[i]<0 ) work[i] = 0;
		if( work[i]>60 ) work[i] = 60;
	}
	total1 = work[0] + work[1] + work[2] + work[3];
	for( i=0; i<4; i++)	{
		if( base[i] < 0 ) base[i] = 5;
		if( base[i] > 60 ) base[i] = 60;
	}
	total2 = base[0] + base[1] + base[2] + base[3];
	if( total1 > defwork )	{
		for( i=0; i<4; i++)	{
			work[i] = ((work[i]*defwork)/total1);
			if( work[i]<0 ) work[i] = 0;
			if( work[i]>60 ) work[i] = 60;
		}
	}

	total1 = work[0] + work[1] + work[2] + work[3];
	total = (total1/2) + total2;

	if( total > 0 ){
		for( i=0; i<4; i++)	{
			float fixwork=0.0;
			fixwork = (base[i])+(float)(work[i]/2);
			base[i] += (int)((fixwork/total) * total1);
			if( base[i] < 1 ) base[i] = 1;
			if( base[i] > 60 ) base[i] = 60;
		}
	}
	total2 = base[0] + base[1] + base[2] + base[3];
	if( total2 > defbase ){
		for( i=0; i<4; i++)	{
			base[i] = (base[i]*defbase)/total2;
			if( base[i] < 1 ) base[i] = 1;
			if( base[i] > 60 ) base[i] = 60;
		}
	}
	total2 = base[0] + base[1] + base[2] + base[3];
	if( base[0]<0 || base[1]<0 || base[2]<0 || base[3]<0 ){
		print("ANDY err EVOLUTION base someone < 0 !!\n");
	}
	return TRUE;
}

int EVOLUTION_createPetFromEnemyIndex( int charaindex, int baseindex, int flg)
{
    Char    CharNew;
    int newindex;
    int     *p;
    int		tp[E_T_DATAINTNUM];
    int		tarray, i, havepetelement, enemynum, petID;
	int		level, enemyrank, array;
	int		petskill[7]={-1,-1,-1,-1,-1,-1,-1};
	int		base[4]={0,0,0,0};
	int		illegalpetskill[15] = {41,52,600,601,602,603,604,614,617,628,630,631,635,638,641};//不可遗传的宠技
#define		E_PAR( a)		(*(p + (a)))
#define		ET_PAR( a)		(*(tp + (a)))
#define		PARAM_CAL( l) 	(( level -1)*ET_PAR( E_T_LVUPPOINT)+ ET_PAR(E_T_INITNUM)) * ET_PAR( (l))
	havepetelement=-1;
	petID = CHAR_getInt( baseindex, CHAR_FUSIONINDEX);
	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_TEMPNO ) == petID ){
			break;
		}
	}
	if( i >= enemynum ) return -1;
	array = i;
    if( !ENEMY_CHECKINDEX( array)) return -1;
//--------------------------------------------------------------------------
    p = ENEMY_enemy[array].intdata;
	tarray = ENEMYTEMP_getEnemyTempArray( array);
	if( !ENEMYTEMP_CHECKINDEX( tarray)){
		print( "ANDY !tarray\n");
		return -1;
	}
	for( i = 0; i < E_T_DATAINTNUM; i ++ ){
	    tp[i] = ENEMYTEMP_enemy[tarray].intdata[i];
	}
    memset( &CharNew, 0, sizeof( Char ) );
    if( !CHAR_getDefaultChar( &CharNew,31010 ) ){
		print( "ANDY !CHAR_getDefaultChar( &CharNew,31010 )\n");
		return -1;
	}
    CharNew.data[CHAR_BASEBASEIMAGENUMBER]
        = CharNew.data[CHAR_BASEIMAGENUMBER] = *(tp+E_T_IMGNUMBER);
    CharNew.data[CHAR_WHICHTYPE] = CHAR_TYPEPET;
	level = RAND( (*(p + ENEMY_LV_MIN)), ( *(p+ ENEMY_LV_MAX)));
//------------------------------------------------------
	{
		if( PET_getBaseAndSkill( charaindex, baseindex, base, petskill, 1) == FALSE ){
			print( "ANDY err PET_getBaseAndSkill(%d) == FALSE \n", baseindex);
			return -1;
		}
		if( PET_getEvolutionAns( baseindex, base) == FALSE ) return -1;
		for( i=0; i < CHAR_MAXPETSKILLHAVE; i ++ ) {//宠技
			int j;
			for( j=0; j<15; j++)	{	//检查非法技能
				if( illegalpetskill[j] == petskill[i] ){
					petskill[i] = -1;
					break;
				}
			}
			CharNew.unionTable.indexOfPetskill[i] = petskill[i];
		}
		if( flg == 1 ){
			if( CHAR_DelPetForIndex( charaindex, baseindex) == FALSE ) return FALSE;
		}
	}
	if( flg == 1 ){
		havepetelement = CHAR_getCharPetElement( charaindex);//找出宠物空位
		if( havepetelement < 0 ) return -1;
	}
//------------------------------------------------------
	tp[E_T_BASEVITAL] = base[0];
	tp[E_T_BASESTR] = base[1];
	tp[E_T_BASETGH] = base[2];
	tp[E_T_BASEDEX] = base[3];
    CharNew.data[CHAR_ALLOCPOINT] = (base[0]<<24)+(base[1]<<16)+(base[2]<<8)+(base[3]<<0);

	for( i = 0; i < 10; i ++ ){
		int work = RAND( 0, 3 );
		if( work == 0 )tp[E_T_BASEVITAL]++;
		if( work == 1 )tp[E_T_BASESTR]++;
		if( work == 2 )tp[E_T_BASETGH]++;
		if( work == 3 )tp[E_T_BASEDEX]++;
	}
//------------------------------------------------------
    CharNew.data[CHAR_VITAL]    = PARAM_CAL(E_T_BASEVITAL);
    CharNew.data[CHAR_STR]      = PARAM_CAL(E_T_BASESTR);
    CharNew.data[CHAR_TOUGH]    = PARAM_CAL(E_T_BASETGH);
    CharNew.data[CHAR_DEX]      = PARAM_CAL(E_T_BASEDEX);
    CharNew.data[CHAR_FIREAT]   = *(tp+ E_T_FIREAT);
    CharNew.data[CHAR_WATERAT]  = *(tp+ E_T_WATERAT);
    CharNew.data[CHAR_EARTHAT]  = *(tp+ E_T_EARTHAT);
    CharNew.data[CHAR_WINDAT]   = *(tp+ E_T_WINDAT);
    CharNew.data[CHAR_SLOT]     = *(tp+ E_T_SLOT);
    CharNew.data[CHAR_MODAI]    = *(tp+ E_T_MODAI);
    CharNew.data[CHAR_VARIABLEAI]    = 0;
	CharNew.data[CHAR_LV]       = level;
    CharNew.data[CHAR_POISON]   = *(tp+ E_T_POISON);
    CharNew.data[CHAR_PARALYSIS]= *(tp+ E_T_PARALYSIS);
    CharNew.data[CHAR_SLEEP]	= *(tp+ E_T_SLEEP);
    CharNew.data[CHAR_STONE]	= *(tp+ E_T_STONE);
    CharNew.data[CHAR_DRUNK]	= *(tp+ E_T_DRUNK);
    CharNew.data[CHAR_CONFUSION]= *(tp+ E_T_CONFUSION);
    CharNew.data[CHAR_RARE]     = *(tp+ E_T_RARE);
	CharNew.data[CHAR_PETID]	= *(tp + E_T_TEMPNO);
	CharNew.data[CHAR_CRITIAL]	= *(tp + E_T_CRITICAL);
	CharNew.data[CHAR_COUNTER]	= *(tp + E_T_COUNTER);
    CharNew.data[CHAR_LIMITLEVEL]	= *(tp + E_T_LIMITLEVEL);
	CharNew.data[CHAR_FUSIONCODE]	= *(tp + E_T_FUSIONCODE);
	CharNew.data[CHAR_PETMAILEFFECT] = RAND(0, PETMAIL_EFFECTMAX);
//------------------------------------------------------

	enemyrank = ENEMY_getRank( array, tarray );
	CharNew.data[CHAR_PETRANK]	= enemyrank;
#undef	E_PAR
#undef	ET_PAR
#undef	PARAM_CAL
    strcpysafe( CharNew.string[CHAR_NAME].string,
                sizeof(CharNew.string[CHAR_NAME].string),
                (char *)ENEMYTEMP_enemy[tarray].chardata[E_T_NAME].string );
    newindex = PET_initCharOneArray( &CharNew );
    if( newindex < 0 ){
		print( "ANDY err newindex=%d\n", newindex);
        return -1;
    }
//------------------------------------------------------
    CHAR_complianceParameter( newindex );
	CHAR_setInt( newindex, CHAR_HP, CHAR_getWorkInt( newindex, CHAR_WORKMAXHP));
	CHAR_setInt ( newindex, CHAR_WHICHTYPE , CHAR_TYPEPET);
#ifdef _PET_2TRANS
	CHAR_setInt ( newindex, CHAR_TRANSMIGRATION, 2);
#else
	CHAR_setInt ( newindex, CHAR_TRANSMIGRATION, 1);
#endif
	CHAR_setInt ( newindex, CHAR_FUSIONBEIT, 1);
	CHAR_setInt ( newindex, CHAR_FUSIONRAISE, 0);
	if( flg == 1 ){
		CHAR_setWorkInt( newindex, CHAR_WORKPLAYERINDEX, charaindex);
		CHAR_setCharPet( charaindex, havepetelement, newindex);
		CHAR_setChar( newindex, CHAR_OWNERCDKEY, CHAR_getChar( charaindex, CHAR_CDKEY));
		CHAR_setChar( newindex, CHAR_OWNERCHARANAME, CHAR_getChar( charaindex, CHAR_NAME));
		{
		char msgbuf[256];
		snprintf( msgbuf, sizeof( msgbuf ), "K%d", havepetelement );
		CHAR_sendStatusString( charaindex, msgbuf );
		snprintf( msgbuf, sizeof( msgbuf ), "W%d", havepetelement );
		CHAR_sendStatusString( charaindex, msgbuf );
		}
	}
    return newindex;
}

int PET_CheckIncubate( int charaindex)
{
#ifndef _USER_CHARLOOPS
	int i;
	static time_t checkeage;
	int anhour = PETFEEDTIME;
	checkeage = (int)time( NULL);
	if( !CHAR_CHECKINDEX( charaindex) )return 0;

	for(i=0 ; i < CHAR_MAXPETHAVE ; i++)	{
		int time_l=0;
		int petindex = CHAR_getCharPet( charaindex, i);
		if( !CHAR_CHECKINDEX( petindex) ) continue;
		if( CHAR_getInt( petindex, CHAR_FUSIONBEIT) != 1 ||
			CHAR_getInt( petindex, CHAR_FUSIONRAISE) <= 0 )return;//检查是否为融合宠

		time_l = CHAR_getInt( petindex, CHAR_FUSIONTIMELIMIT);
		if( time_l < 0 || time_l > checkeage ){
			CHAR_setInt( petindex, CHAR_FUSIONTIMELIMIT, checkeage);
			continue;
		}

		if( (int)checkeage > (time_l+(anhour*2)) ){
			char buf[256];
			int levelup, vital, str, tgh, dex;
			int raise = CHAR_getInt( petindex, CHAR_FUSIONRAISE);
			//  养次数
			raise++;
			CHAR_setInt( petindex, CHAR_FUSIONTIMELIMIT, (int)time(NULL)+anhour-1);
			if( raise < 0 ) raise = 0;
			if( raise >= 40 ) raise = 30;
			CHAR_setInt( petindex, CHAR_FUSIONRAISE, raise);
			levelup = CHAR_getInt( petindex, CHAR_ALLOCPOINT);
			vital = ((levelup>>24) & 0xFF) - 5;
			str   = ((levelup>>16) & 0xFF) - 5;
			tgh   = ((levelup>> 8) & 0xFF) - 5;
			dex   = ((levelup>> 0) & 0xFF) - 5;
			if( vital < 0 ) vital = 0;
			if( str < 0 ) str = 0;
			if( tgh < 0 ) tgh = 0;
			if( dex < 0 ) dex = 0;
			//扣属性
			levelup = (vital<<24) + (str<<16) + (tgh<<8) + (dex<<0);
			CHAR_setInt( petindex, CHAR_ALLOCPOINT, levelup);
			sprintf( buf, "蛋〈%s〉的品质变差了。", CHAR_getUseName( petindex ));
			CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORYELLOW);

			LogPetFeed(
				CHAR_getChar( charaindex, CHAR_NAME),
				CHAR_getChar( charaindex, CHAR_CDKEY),
				CHAR_getChar( petindex, CHAR_NAME),
				petindex,
				CHAR_getInt( petindex, CHAR_LV),
				"品质变差", // Key
				CHAR_getInt( charaindex, CHAR_FLOOR),
				CHAR_getInt( charaindex, CHAR_X),
				CHAR_getInt( charaindex, CHAR_Y),
				CHAR_getChar( petindex, CHAR_UNIQUECODE) );

		}else if( (int)checkeage > (time_l+anhour)  ){

			char buf[256];
			sprintf( buf, "蛋〈%s〉呈现可  食状态。", CHAR_getUseName( petindex ));
			CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORYELLOW);
		}
	}
	return 1;
#else
		int masterindex, time_l=0;
		static time_t checkeage;
		int anhour = PETFEEDTIME, i;
		
		if( !CHAR_CHECKINDEX( charaindex) )return 0;
		masterindex = CHAR_getWorkInt( charaindex, CHAR_WORKPLAYERINDEX);
		if( !CHAR_CHECKINDEX( masterindex) )return 0;

		if( CHAR_getInt( charaindex, CHAR_FUSIONBEIT) != 1 ||
			CHAR_getInt( charaindex, CHAR_FUSIONRAISE) <= 0 ) return 0;//检查是否为融合宠

		for(i=0 ; i < CHAR_MAXPETHAVE ; i++)	{
			if( charaindex != CHAR_getCharPet( masterindex, i) ) continue;
			break;
		}
		if( i >= CHAR_MAXPETHAVE ) return 0;

#ifdef _PET_FUSIONSHOWTIME
		{

			char caname[256], msgbuf[64];
			int deftime;
			int oldtime = CHAR_getInt( charaindex, CHAR_FUSIONTIMELIMIT);

			for(i=0 ; i < CHAR_MAXPETHAVE ; i++)	{
				if( charaindex != CHAR_getCharPet( masterindex, i) ) continue;
				deftime = anhour - ( checkeage - oldtime);
				if( deftime >= 0 ){
					sprintf( caname, "宠物蛋(%d)", (int)(deftime/60));
				}else{
					deftime =  ( checkeage - oldtime) - anhour;
					sprintf( caname, "宠物蛋(饿%d)", (int)(deftime/60));
				}
				CHAR_setChar( charaindex, CHAR_USERPETNAME, caname);
				snprintf( msgbuf, sizeof( msgbuf ), "K%d", i );
				CHAR_sendStatusString( masterindex, msgbuf );
				snprintf( msgbuf, sizeof( msgbuf ), "W%d", i );
				CHAR_sendStatusString( masterindex, msgbuf );
				break;
			}
		}
#endif
		checkeage = (int)time( NULL);

		time_l = CHAR_getInt( charaindex, CHAR_FUSIONTIMELIMIT);
		if( time_l < 0 || time_l > checkeage ){
			CHAR_setInt( charaindex, CHAR_FUSIONTIMELIMIT, checkeage);
			return 0;
		}
		if( (int)checkeage > (time_l+(anhour*2.5)) ){
			char buf[256];
			int levelup, vital, str, tgh, dex;
			int raise = CHAR_getInt( charaindex, CHAR_FUSIONRAISE);
			raise++;
			CHAR_setInt( charaindex, CHAR_FUSIONTIMELIMIT, (int)time(NULL)+anhour-1);
			if( raise < 0 ) raise = 0;
			if( raise >= 40 ) raise = 30;
			CHAR_setInt( charaindex, CHAR_FUSIONRAISE, raise);
			levelup = CHAR_getInt( charaindex, CHAR_ALLOCPOINT);
			vital = ((levelup>>24) & 0xFF) - 6;
			str   = ((levelup>>16) & 0xFF) - 6;
			tgh   = ((levelup>> 8) & 0xFF) - 6;
			dex   = ((levelup>> 0) & 0xFF) - 6;
			if( vital < 0 ) vital = 0;
			if( str < 0 ) str = 0;
			if( tgh < 0 ) tgh = 0;
			if( dex < 0 ) dex = 0;
			//扣属性
			levelup = (vital<<24) + (str<<16) + (tgh<<8) + (dex<<0);
			CHAR_setInt( charaindex, CHAR_ALLOCPOINT, levelup);
			sprintf( buf, "蛋〈%s〉的品质变差了。", CHAR_getUseName( charaindex ));
			CHAR_talkToCli( masterindex, -1, buf, CHAR_COLORYELLOW);

			LogPetFeed(
				CHAR_getChar( masterindex, CHAR_NAME),
				CHAR_getChar( masterindex, CHAR_CDKEY),
				CHAR_getChar( charaindex, CHAR_NAME),
				charaindex,
				CHAR_getInt( charaindex, CHAR_LV),
				"品质变差", // Key
				CHAR_getInt( masterindex, CHAR_FLOOR),
				CHAR_getInt( masterindex, CHAR_X),
				CHAR_getInt( masterindex, CHAR_Y),
				CHAR_getChar( charaindex, CHAR_UNIQUECODE) );

		}else if( (int)checkeage > (time_l+anhour) ){
			char buf[256];
			sprintf( buf, "蛋〈%s〉呈现可  食状态。", CHAR_getUseName( charaindex ));
			CHAR_talkToCli( masterindex, -1, buf, CHAR_COLORYELLOW);
		}
		return 1;
#endif
}
#endif

#ifdef _NPC_FUSION
int PETFUSION_SetNewEgg( int toindex , int petindex, int array, int *work, int *skill1, int *skill2)
{
    int     *p;
    int		tp[E_T_DATAINTNUM];
    int		tarray, i;
	int		level, workrank, petrank=0;
	int		LevelUpPoint;
	struct 	{
    	int		num;
    	float	rank;
    }ranktbl[] = {
    	{ 130, 2.5},
    	{ 100, 2.0},
    	{ 95, 1.5},
    	{ 85, 1.0},
    	{ 80, 0.5},
    	{ 0, 0.0},
    };
    if( !ENEMY_CHECKINDEX( array))
		return -1;
	p = ENEMY_getIntdata( array);
	if( p == NULL )	{
		print("\n p = NULL" );
		return -1;
	}
	tarray = ENEMYTEMP_getEnemyTempArray( array);
	if( !ENEMYTEMP_CHECKINDEX( tarray))
		return -1;
	//print( "ANDY tarray/array=%d/%d-->%s \n", tarray, array,
	//	ENEMYTEMP_getChar( tarray, E_T_NAME));
	for( i = 0; i < E_T_DATAINTNUM; i ++ ){
		tp[i] = ENEMYTEMP_getInt( tarray, i);
	}
	level = 1;
#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )
#define	E_PAR( a)		(*(p + (a)))
#define	ET_PAR( a)		(*(tp + (a)))
#define	PARAM_CAL( l) 	( ( level -1) * ET_PAR( E_T_LVUPPOINT) + ET_PAR( E_T_INITNUM) )
	work[0] += ( RAND(0,4) - 2 );
	work[1] += ( RAND(0,4) - 2 );
	work[2] += ( RAND(0,4) - 2 );
	work[3] += ( RAND(0,4) - 2 );
	LevelUpPoint = ( work[0] << 24 )
		+ (work[1] << 16)
		+ (work[2] << 8 )
		+ (work[3] << 0 );

	CHAR_setInt( petindex, CHAR_ALLOCPOINT, LevelUpPoint);
	workrank = work[0]+work[1]+work[2]+work[3];
	for( i = 0; i < arraysizeof( ranktbl); i ++ ) {
		if(  workrank >= ranktbl[i].num ) {
			petrank = i;
			break;
		}
	}
	if( i>= arraysizeof( ranktbl)) i = arraysizeof( ranktbl);
	CHAR_setInt( petindex, CHAR_PETRANK, petrank);
	for( i = 0; i < 10; i ++ ){
		int rnt = RAND( 0, 3 );
		if( rnt == 0 ) work[0]++;
		if( rnt == 1 ) work[1]++;
		if( rnt == 2 ) work[2]++;
		if( rnt == 3 ) work[3]++;
	}

    CHAR_setInt( petindex, CHAR_VITAL, ( PARAM_CAL(E_T_BASEVITAL) * work[0] ));
    CHAR_setInt( petindex, CHAR_STR , ( PARAM_CAL(E_T_BASESTR) * work[1] ));
    CHAR_setInt( petindex, CHAR_TOUGH , ( PARAM_CAL(E_T_BASETGH) * work[2] ));
    CHAR_setInt( petindex, CHAR_DEX , ( PARAM_CAL(E_T_BASEDEX) * work[3] ));
	CHAR_setMaxExp( petindex, 0);
	CHAR_setInt( petindex, CHAR_LV, level);

	//宠物技能设为七技
	CHAR_setInt( petindex, CHAR_SLOT, 7);
	{
		int j;
		int	illegalpetskill[15] = {41,52,600,601,602,603,604,614,617,628,630,631,635,638,641};
		for( i=0; i<CHAR_MAXPETSKILLHAVE; i++)	{
			if( RAND( 0, 1 ) == 0 )	{
				skill1[i] = skill2[i];
			}
			for( j=0; j<15; j++)	{	//检查非法技能
				if( illegalpetskill[j] == skill1[i] ){
					skill1[i] = -1;
					break;
				}
			}
		}
	}
	for( i=0; i<CHAR_MAXPETSKILLHAVE; i++)	{
		CHAR_setPetSkill( petindex, i, skill1[i]);
	}
#undef	E_PAR
#undef	ET_PAR
#undef	PARAM_CAL
    CHAR_complianceParameter( petindex );
	CHAR_setInt( petindex, CHAR_HP, CHAR_getWorkInt( petindex, CHAR_WORKMAXHP));
	if( CHAR_CHECKINDEX( toindex) ){
		CHAR_setWorkInt( petindex, CHAR_WORKPLAYERINDEX, toindex);
		CHAR_setChar( petindex, CHAR_OWNERCDKEY, CHAR_getChar( toindex, CHAR_CDKEY));
		CHAR_setChar( petindex, CHAR_OWNERCHARANAME, CHAR_getChar( toindex, CHAR_NAME));
	}
	CHAR_setInt ( petindex, CHAR_WHICHTYPE , CHAR_TYPEPET);
#ifdef _PET_2TRANS
	CHAR_setInt ( petindex, CHAR_TRANSMIGRATION, 2);
#else
	CHAR_setInt ( petindex, CHAR_TRANSMIGRATION, 1);
#endif
	CHAR_setInt( petindex, CHAR_FUSIONCODE, -1);
	CHAR_setInt( petindex, CHAR_FUSIONRAISE, 40);
	CHAR_setInt( petindex, CHAR_FUSIONBEIT, 1);

	CHAR_setInt( petindex, CHAR_FUSIONTIMELIMIT, (int)time(NULL));
	CHAR_setInt( petindex, CHAR_EVOLUTIONBASEVTL, 0);
	CHAR_setInt( petindex, CHAR_EVOLUTIONBASESTR, 0);
	CHAR_setInt( petindex, CHAR_EVOLUTIONBASETGH, 0);
	CHAR_setInt( petindex, CHAR_EVOLUTIONBASEDEX, 0);
#ifdef _USER_CHARLOOPS
	{
		Char 	*ch;
		ch  = CHAR_getCharPointer( petindex);
		if( ch == NULL ) return 0;
		//andy_log
//		print("*CHAR_LOOPFUNCTEMP1:%s \n", "PET_CheckIncubateLoop");
		strcpysafe( ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
				sizeof( ch->charfunctable[CHAR_LOOPFUNCTEMP1]), "PET_CheckIncubateLoop");
		ch->data[CHAR_LOOPINTERVAL] = 60000;
		CHAR_constructFunctable( petindex);
	}
#endif
	if( CHAR_CHECKINDEX( toindex) ){
		for(i = 0; i < CHAR_MAXPETHAVE; i++){
			char msgbuf[256];
			int pindex = CHAR_getCharPet(toindex, i);
			if( !CHAR_CHECKINDEX( pindex) )
				continue;
			memset( msgbuf, 0, sizeof( msgbuf));
			snprintf( msgbuf, sizeof( msgbuf ), "K%d", i );
			CHAR_sendStatusString( toindex, msgbuf );

			CHAR_send_K_StatusString(toindex, i,CHAR_K_STRING_HP|CHAR_K_STRING_AI);
		}
	}
    return petindex;
}

BOOL PETFUSION_AddEgg( int toindex, int petID, int PetCode)
{
	int	ret;
	char msgbuf[64];
	int	enemynum;
	int	i,j;
	int petindex, petindex2;
	
	//检查宠物栏是否有空位
	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( toindex, i);
		if( petindex == -1  )
			break;
	}
	if( i == CHAR_MAXPETHAVE )      {
		snprintf( msgbuf,sizeof( msgbuf), "宠物已满！！");
		CHAR_talkToCli( toindex, -1, msgbuf,  CHAR_COLORYELLOW);
		return -1;
	}
	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {//PetCode
		if( ENEMY_getInt( i, ENEMY_ID ) == petID )
		//if( ENEMY_getInt( i, ENEMY_TEMPNO ) == PetCode )
			break;
	}
	if( i == enemynum ){
		print("ANDY err i == enemynum \n");
		return -1;
	}
	ret = ENEMY_createPetFromEnemyIndex( toindex, i);
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ )	{
		if( CHAR_getCharPet( toindex, i ) == ret )
			break;
	}
	if( i == CHAR_MAXPETHAVE )
		i = 0;	
	if( CHAR_CHECKINDEX( ret ) == TRUE ){
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
	}
	petindex2 = CHAR_getCharPet( toindex, i);
	if( !CHAR_CHECKINDEX( petindex2) ){
		print("ANDY petindex2=%d\n", petindex2);
		return -1;
	}
	CHAR_setInt( petindex2, CHAR_FUSIONINDEX, PetCode);
	snprintf( msgbuf,sizeof( msgbuf), "拿到%s。", CHAR_getChar(petindex2,CHAR_NAME));
	CHAR_talkToCli( toindex, -1, msgbuf,  CHAR_COLORYELLOW);
	for(j = 0; j < CHAR_MAXPETHAVE; j++){
		petindex = CHAR_getCharPet(toindex, j);
		if( !CHAR_CHECKINDEX( petindex) )
			continue;
		CHAR_complianceParameter( petindex );
		snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
		CHAR_sendStatusString( toindex, msgbuf );
		snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
		CHAR_sendStatusString( toindex, msgbuf );
	}
	LogPet(
		CHAR_getChar( toindex, CHAR_NAME ),
		CHAR_getChar( toindex, CHAR_CDKEY ),
		CHAR_getChar( petindex2, CHAR_NAME),
		CHAR_getInt( petindex2, CHAR_LV),
		"TenseiGet",
		CHAR_getInt( toindex,CHAR_FLOOR),
		CHAR_getInt( toindex,CHAR_X ),
		CHAR_getInt( toindex,CHAR_Y ),
		CHAR_getChar( petindex2, CHAR_UNIQUECODE)   // shan 2001/12/14
	);

	return petindex2;
}
#endif

#ifdef _PET_TRANS
int GetNewPet( int toindex , int petindex, int array, int *work)
{
    int     *p;
    int		tp[E_T_DATAINTNUM];
    int		tarray, i;
	int		level, workrank, petrank=0;
	int		LevelUpPoint;

	struct 	{
    	int		num;
    	float	rank;
    }ranktbl[] = {
    	{ 130, 2.5},
    	{ 100, 2.0},
    	{ 95, 1.5},
    	{ 85, 1.0},
    	{ 80, 0.5},
    	{ 0, 0.0},
    };

    if( !ENEMY_CHECKINDEX( array))
		return -1;
	p = ENEMY_getIntdata( array);
	if( p == NULL )	{
		print("\n p = NULL" );
		return -1;
	}
	tarray = ENEMYTEMP_getEnemyTempArray( array);
	if( !ENEMYTEMP_CHECKINDEX( tarray))
		return -1;
	for( i = 0; i < E_T_DATAINTNUM; i ++ ){
		tp[i] = ENEMYTEMP_getInt( tarray, i);
	}
	level = 1;
#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )
#define		E_PAR( a)		(*(p + (a)))
#define		ET_PAR( a)		(*(tp + (a)))
#define		PARAM_CAL( l) 	( ( level -1) * ET_PAR( E_T_LVUPPOINT) + ET_PAR( E_T_INITNUM) )
	work[0] += ( RAND(0,4) - 2 );
	work[1] += ( RAND(0,4) - 2 );
	work[2] += ( RAND(0,4) - 2 );
	work[3] += ( RAND(0,4) - 2 );

	LevelUpPoint = ( work[0] << 24 )
		+ (work[1] << 16)
		+ (work[2] << 8 )
		+ (work[3] << 0 );
	CHAR_setInt( petindex, CHAR_ALLOCPOINT, LevelUpPoint);
	workrank = work[0]+work[1]+work[2]+work[3];
	for( i = 0; i < arraysizeof( ranktbl); i ++ ) {
		if(  workrank >= ranktbl[i].num ) {
			petrank = i;
			break;
		}
	}

	if( i>= arraysizeof( ranktbl)) i = arraysizeof( ranktbl);
	CHAR_setInt( petindex, CHAR_PETRANK, petrank);

	for( i = 0; i < 10; i ++ ){
		int rnt = RAND( 0, 3 );
		if( rnt == 0 ) work[0]++;
		if( rnt == 1 ) work[1]++;
		if( rnt == 2 ) work[2]++;
		if( rnt == 3 ) work[3]++;
	}

    CHAR_setInt( petindex, CHAR_VITAL, ( PARAM_CAL(E_T_BASEVITAL) * work[0] ));
    CHAR_setInt( petindex, CHAR_STR , ( PARAM_CAL(E_T_BASESTR) * work[1] ));
    CHAR_setInt( petindex, CHAR_TOUGH , ( PARAM_CAL(E_T_BASETGH) * work[2] ));
    CHAR_setInt( petindex, CHAR_DEX , ( PARAM_CAL(E_T_BASEDEX) * work[3] ));
	CHAR_setMaxExp( petindex, 0);
	CHAR_setInt( petindex, CHAR_LV, level);
	//宠物技能设为七技
	CHAR_setInt( petindex, CHAR_SLOT, 7);
/*	for( i=0; i<CHAR_MAXPETSKILLHAVE; i++)	{
		petskill = CHAR_getPetSkill( petindex, i);
		print("\n petskill = %d ", petskill);
		if( petskill == -1)	{
			
		}
	}
*/
#undef	E_PAR
#undef	ET_PAR
#undef	PARAM_CAL
    CHAR_complianceParameter( petindex );
	CHAR_setInt( petindex, CHAR_HP, CHAR_getWorkInt( petindex, CHAR_WORKMAXHP));
	if( CHAR_CHECKINDEX( toindex) ){
		CHAR_setWorkInt( petindex, CHAR_WORKPLAYERINDEX, toindex);
		CHAR_setChar( petindex, CHAR_OWNERCDKEY, CHAR_getChar( toindex, CHAR_CDKEY));
		CHAR_setChar( petindex, CHAR_OWNERCHARANAME, CHAR_getChar( toindex, CHAR_NAME));
	}
	CHAR_setInt ( petindex, CHAR_WHICHTYPE , CHAR_TYPEPET);
#ifdef _PET_2TRANS
	CHAR_setInt ( petindex, CHAR_TRANSMIGRATION, CHAR_getInt ( petindex, CHAR_TRANSMIGRATION )+1);
#else
	CHAR_setInt ( petindex, CHAR_TRANSMIGRATION, 1);
#endif
	return petindex;
}
#endif




