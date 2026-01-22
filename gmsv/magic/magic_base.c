#include "version.h"
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "buf.h"
#include "configfile.h"
#include "magic_base.h"
#include "magic.h"

static Magic	*MAGIC_magic;
static int		MAGIC_magicnum;



#ifdef __ATTACK_MAGIC

AttMagic	*ATTMAGIC_magic;
int		 ATTMAGIC_magicnum;

#endif



typedef struct tagMagic_MagicFunctionTable
{
	char			*functionname;		/*	��絊优擭啞栝騷鳴勗怳�抌�倳摯  鞳 */
	MAGIC_CALLFUNC	func;				/*    傷勗蠙怮③踏躂堎匴倳 */
	int				hash;				/*  hash */
}MAGIC_MagicFunctionTable;

/* ��絊禱  盓媃倜�梜薱藈樘�  埰堎痀 */
static MAGIC_MagicFunctionTable MAGIC_functbl[] = {
	{ "MAGIC_Recovery", 		MAGIC_Recovery,			0},
	{ "MAGIC_OtherRecovery",	MAGIC_OtherRecovery,	0},
	{ "MAGIC_FieldAttChange",	MAGIC_FieldAttChange,	0},
	{ "MAGIC_StatusChange",		MAGIC_StatusChange,		0},
	{ "MAGIC_MagicDef",			MAGIC_MagicDef,			0},
	{ "MAGIC_StatusRecovery",	MAGIC_StatusRecovery,	0},
	{ "MAGIC_Ressurect",		MAGIC_Ressurect,	0},
	{ "MAGIC_AttReverse",		MAGIC_AttReverse,	0},
	{ "MAGIC_ResAndDef",		MAGIC_ResAndDef,	0},
	
#ifdef __ATTACK_MAGIC
	{ "MAGIC_AttMagic" , 		MAGIC_AttMagic , 0 },
#endif
#ifdef _OTHER_MAGICSTAUTS
	{ "MAGIC_MagicStatusChange",	MAGIC_MagicStatusChange,	0},
#endif
#ifdef _ITEM_METAMO
	{ "MAGIC_Metamo", 		MAGIC_Metamo,	0},
#endif
#ifdef _ITEM_ATTSKILLMAGIC
	//{ "MAGIC_AttSkill", 		MAGIC_AttSkill,	0},
#endif
#ifdef _MAGIC_WEAKEN       // vincent  儕鍾:剞��
	{ "MAGIC_Weaken", 		  MAGIC_Weaken,	      0},
#endif
#ifdef _MAGIC_DEEPPOISON   // vincent  儕鍾:曄馮
	{ "MAGIC_StatusChange2",  MAGIC_StatusChange2,0},
#endif
#ifdef _MAGIC_BARRIER      // vincent  儕鍾:藹梤
	{ "MAGIC_Barrier", 		  MAGIC_Barrier,	  0},
#endif
#ifdef _MAGIC_NOCAST       // vincent  儕鍾:麥蘇
	{ "MAGIC_Nocast", 		  MAGIC_Nocast,	      0},
#endif
#ifdef _MAGIC_TOCALL	// 掉韓淝
	{ "MAGIC_ToCallDragon",	MAGIC_ToCallDragon,		0},
#endif
};

/*----------------------------------------------------------------------*/


/* 盻  鏍攝蚗袲�楟抄舠噩抵�祛 */
/*----------------------------------------------------------------------*/
INLINE BOOL MAGIC_CHECKINDEX( int index )
{
    if( MAGIC_magicnum<=index || index<0 )return FALSE;
    return TRUE;
}
/*----------------------------------------------------------------------*/
static INLINE BOOL MAGIC_CHECKINTDATAINDEX( int index)
{
	if( MAGIC_DATAINTNUM <= index || index < 0 ) return FALSE;
	return TRUE;
}
/*----------------------------------------------------------------------*/
static INLINE BOOL MAGIC_CHECKCHARDATAINDEX( int index)
{
	if( MAGIC_DATACHARNUM <= index || index < 0 ) return FALSE;
	return TRUE;
}
/*----------------------------------------------------------------------*/
INLINE int MAGIC_getInt( int index, MAGIC_DATAINT element)
{
	return MAGIC_magic[index].data[element];
}
/*----------------------------------------------------------------------*/
INLINE int MAGIC_setInt( int index, MAGIC_DATAINT element, int data)
{
	int buf;
	buf = MAGIC_magic[index].data[element];
	MAGIC_magic[index].data[element]=data;
	return buf;
}
/*----------------------------------------------------------------------*/
INLINE char* MAGIC_getChar( int index, MAGIC_DATACHAR element)
{
	if( !MAGIC_CHECKINDEX( index)) return NULL;
	if( !MAGIC_CHECKCHARDATAINDEX( element)) return NULL;
	return MAGIC_magic[index].string[element].string;
}

/*----------------------------------------------------------------------*/
INLINE BOOL MAGIC_setChar( int index ,MAGIC_DATACHAR element, char* new )
{
    if(!MAGIC_CHECKINDEX(index))return FALSE;
    if(!MAGIC_CHECKCHARDATAINDEX(element))return FALSE;
    strcpysafe( MAGIC_magic[index].string[element].string,
                sizeof(MAGIC_magic[index].string[element].string),
                new );
    return TRUE;
}
/*----------------------------------------------------------------------
 *   傮摯倳禱蠐堎��
 *---------------------------------------------------------------------*/
int MAGIC_getMagicNum( void)
{
	return MAGIC_magicnum;
}

/*----------------------------------------------------------------------
 *   傮摯优擭啞栝騷鳴禱  資
 *---------------------------------------------------------------------*/
BOOL MAGIC_initMagic( char *filename)
{
    FILE*   f;
    char    line[256];
    int     linenum=0;
    int     magic_readlen=0;
	int		i,j;

	int		max_magicid =0;
	char	token[256];

    f = fopen(filename,"r");
    if( f == NULL ){
        print( "恅璃湖羲囮啖\n");
        return FALSE;
    }

    MAGIC_magicnum=0;

    /*  竘囀  嗚埵菜誑笰菜堣堎凝汔竣凝ぅ迋堎    */
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );

#ifdef _MAGIC_OPTIMUM // Robin �○囆豱趭AGIC ID
		if( getStringFromIndexWithDelim( line, ",", MAGIC_DATACHARNUM+MAGIC_ID+1,
				token, sizeof(token)) == FALSE )
			continue;
		max_magicid = max( atoi( token), max_magicid);
#endif

        MAGIC_magicnum++;
    }

#ifdef _MAGIC_OPTIMUM
	print("衄虴藹楊:%d 郔湮藹楊:%d ...", MAGIC_magicnum, max_magicid);
	MAGIC_magicnum = max_magicid +1;
#endif

    if( fseek( f, 0, SEEK_SET ) == -1 ){
        fprint( "刲坰渣昫\n" );
        fclose(f);
        return FALSE;
    }

    MAGIC_magic = allocateMemory( sizeof(struct tagMagic)
                                   * MAGIC_magicnum );
    if( MAGIC_magic == NULL ){
        fprint( "拸楊煦饜囀湔 %d\n" ,
                sizeof(struct tagMagic)*MAGIC_magicnum);
        fclose( f );
        return FALSE;
    }

	/* 參數結構 */
    for( i = 0; i < MAGIC_magicnum; i ++ ) {
    	for( j = 0; j < MAGIC_DATAINTNUM; j ++ ) {
    		MAGIC_setInt( i,j,-1);
    	}
    	for( j = 0; j < MAGIC_DATACHARNUM; j ++ ) {
    		MAGIC_setChar( i,j,"");
    	}
    }

    /*  竘倜  陑  埰    */
    linenum = 0;
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );

        /* 實際調用的函數 */
        /* 將tab轉換為空格 */
        replaceString( line, '\t' , ' ' );
        /* 袸  摯筒妐↓筒禱噁堎��*/
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

#ifdef _MAGIC_OPTIMUM
		if( getStringFromIndexWithDelim( line, ",", MAGIC_DATACHARNUM+MAGIC_ID+1,
				token, sizeof(token)) == FALSE )
			continue;
		magic_readlen = atoi( token);
#endif

		for( i = 0; i < MAGIC_DATACHARNUM; i ++ ) {

	        /*    棬  暵哱↓袲璃禱峟堎    */
	        ret = getStringFromIndexWithDelim( line,",",
	        									i + 1,
	        									token,sizeof(token));
	        if( ret==FALSE ){
	            fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
	            break;
	        }
	        MAGIC_setChar( magic_readlen, i, token);
		}
        /* 4僑  雄禢毀倳偯溢↓淏 */
#define	MAGIC_STARTINTNUM		5
        for( i = MAGIC_STARTINTNUM; i < MAGIC_DATAINTNUM+MAGIC_STARTINTNUM; i ++ ) {
            ret = getStringFromIndexWithDelim( line,",",i,token,
                                               sizeof(token));

#ifdef __ATTACK_MAGIC
            
            if( FALSE == ret )

            	break;
            	
            if( 0 != strlen( token ) )
            {	
            	MAGIC_setInt( magic_readlen , i - MAGIC_STARTINTNUM , atoi( token ) );
            }
            	                                               
#else
                                               
            if( ret==FALSE ){
                fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
                break;
            }
            if( strlen( token) != 0 ) {
	            MAGIC_setInt( magic_readlen, i - MAGIC_STARTINTNUM, atoi( token));
			}
			
	    #endif
        }

#ifdef __ATTACK_MAGIC
        
        if( i != MAGIC_STARTINTNUM + MAGIC_IDX && i != MAGIC_DATAINTNUM + MAGIC_STARTINTNUM )
        	continue;
        	
#else
        
        if( i < MAGIC_DATAINTNUM+MAGIC_STARTINTNUM )
        	 continue;
        	 
#endif
		/* з踝埱敁箾備隋煦崹汔喫竣埰堎�� */
		if( MAGIC_getInt( magic_readlen, MAGIC_TARGET_DEADFLG) == 1 ) {
			MAGIC_setInt( magic_readlen, MAGIC_TARGET,
						MAGIC_getInt( magic_readlen, MAGIC_TARGET)+100);
		}

        magic_readlen ++;
}
    }
    fclose(f);

    MAGIC_magicnum = magic_readlen;


    print( "衄虴藹楊杅岆 %d...", MAGIC_magicnum );

	/* hash 摯頞   */
	for( i = 0; i < arraysizeof( MAGIC_functbl); i ++ ) {
		MAGIC_functbl[i].hash = hashpjw( MAGIC_functbl[i].functionname);
	}
#if 0
    for( i=0; i <MAGIC_magicnum ; i++ ){
        for( j = 0; j < MAGIC_DATACHARNUM; j ++ ) {
	        print( "%s ", MAGIC_getChar( i, j));
		}
		print( "\n");
        for( j = 0; j < MAGIC_DATAINTNUM; j ++ ) {
            print( "%d ", MAGIC_getInt( i, j));
        }
        print( "\n-------------------------------------------------\n");
    }
#endif
    return TRUE;
}
/*------------------------------------------------------------------------
 * Magic摯优擭啞栝騷鳴  陑  媃
 *-----------------------------------------------------------------------*/
BOOL MAGIC_reinitMagic( void )
{
	freeMemory( MAGIC_magic);
	return( MAGIC_initMagic( getMagicfile()));
}


#ifdef __ATTACK_MAGIC

/*------------------------------------------------------------------------
 * AttMagic腔場宎趙
 *-----------------------------------------------------------------------*/
BOOL ATTMAGIC_initMagic( char *filename )
{
    FILE *file;

	// Open file
    if( NULL == ( file = fopen( filename , "r" ) ) )
	{ 
	    ATTMAGIC_magicnum	= 0;
		ATTMAGIC_magic		= NULL;

        return TRUE;
    }

	fseek( file , 0 , SEEK_END );

	// Calculate the number of attack magics
	ATTMAGIC_magicnum = ftell( file ) / sizeof( struct tagAttMagic );
	if( ATTMAGIC_magicnum % 2 )
	{
		fprint( "湖羲恅璃囮啖\n" );
		fclose( file );

		return FALSE;
	}

        fseek( file , 0 , SEEK_SET );
        
	// Allocate memory to attack magics
    ATTMAGIC_magic = allocateMemory( sizeof( struct tagAttMagic ) * ATTMAGIC_magicnum );
	if( NULL == ATTMAGIC_magic )
	{
		fprint( "拸楊煦饜囀湔 %d\n" , sizeof( struct tagAttMagic ) * ATTMAGIC_magicnum );
		fclose( file );

		return FALSE;
    }

	// Read attack magics information
	memset( ATTMAGIC_magic , 0 , sizeof( struct tagAttMagic ) * ATTMAGIC_magicnum );
	fread( ATTMAGIC_magic , 1 , sizeof( struct tagAttMagic ) * ATTMAGIC_magicnum , file );
	
	fclose( file );

	ATTMAGIC_magicnum = ATTMAGIC_magicnum / 2;

    print( "衄虴腔馴僻藹楊杅 %d\n" , ATTMAGIC_magicnum );

	return TRUE;
}



/*------------------------------------------------------------------------
 * AttMagic腔婬僅場宎趙
 *-----------------------------------------------------------------------*/
BOOL ATTMAGIC_reinitMagic( void )
{
   freeMemory( ATTMAGIC_magic );
   ATTMAGIC_magicnum = 0;
   
   return ATTMAGIC_initMagic( getAttMagicfileName() );
//	    return ATTMAGIC_initMagic( getMagicfile() );
}

#endif

/*------------------------------------------------------------------------
 * MAGIC_ID凝�桵撼紐拜籥鎔�倳
 * 蒍堇偯
 * 埬  : 蝨棬
 * 謄  : -1
 *-----------------------------------------------------------------------*/
int MAGIC_getMagicArray( int magicid)
{
#ifdef _MAGIC_OPTIMUM
	if( magicid >= 0 && magicid < MAGIC_magicnum)
		return	magicid;
#else
	int		i;
	for( i = 0; i < MAGIC_magicnum; i ++ ) {
		if( MAGIC_magic[i].data[MAGIC_ID] == magicid ) {
			return i;
		}
	}
#endif
	return -1;
}
/*------------------------------------------------------------
 * ��絊摯匴倳  凝�梣昉抯�淏↓禱蒍埰
 * 礎倳
 *  name        char*       ��絊摯  鞳
 * 蒍堇偯
 *  匴倳喜摯睽騷璃淏�訹聿剒踽�勗毀NULL
 ------------------------------------------------------------*/
MAGIC_CALLFUNC MAGIC_getMagicFuncPointer(char* name)
{
    int i;
    int hash; //ttom
    //ttom 12/18/2000
    if(name==NULL) return NULL;
    //ttom
    //int hash = hashpjw( name );
    hash=hashpjw(name);
    for( i = 0 ; i< arraysizeof( MAGIC_functbl) ; i++ ) {
        if( MAGIC_functbl[i].hash == hash ) {
        	if( strcmp( MAGIC_functbl[i].functionname, name ) == 0 )  {
	            return MAGIC_functbl[i].func;
			}
		}
	}
    return NULL;
}


// Nuke start (08/23)
/*
  蓬   Nuke 踏庌摯鏍攝蚗袲��
    傮摯嗚堅  ⑹禱鏍攝蚗袲埰堎��

  Check the validity of the target of a magic.
  Return value:
	0: Valid
	-1: Invalid
*/
int MAGIC_isTargetValid( int magicid, int toindex)
{
	int marray;
	marray= MAGIC_getMagicArray( magicid);

	#ifdef __ATTACK_MAGIC

	if( toindex >= 0 && toindex <= 19 )
		return 0;

	// One side of players
	if( 20 == toindex || 21 == toindex )
	{
		if( MAGIC_TARGET_WHOLEOTHERSIDE == MAGIC_magic[marray].data[MAGIC_TARGET] || MAGIC_TARGET_ALL_ROWS == MAGIC_magic[marray].data[MAGIC_TARGET] ) 
			return 0;
		else
			return -1;
	}

	// All players
	if( 22 == toindex )
	{
		if( MAGIC_TARGET_ALL == MAGIC_magic[marray].data[MAGIC_TARGET] ) 
			return 0;
		else
			return -1;
	}

	// One row
	if( 23 == toindex || 24 == toindex || 25 == toindex || 26 == toindex )
	{
		if( MAGIC_TARGET_ONE_ROW == MAGIC_magic[marray].data[MAGIC_TARGET] )
			return 0;
		else
			return -1;
	}

	#else

	// Single player
	if ((toindex >= 0x0) && (toindex <= 0x13)) return 0;
	// All players
	if (toindex == 0x16) {
		if (MAGIC_magic[marray].data[MAGIC_TARGET] == MAGIC_TARGET_ALL) 
			return 0; else return -1;
	}
	// One side of players
	if ((toindex == 0x14) || (toindex == 0x15)) {
		if (MAGIC_magic[marray].data[MAGIC_TARGET] == MAGIC_TARGET_WHOLEOTHERSIDE) 
			return 0; else return -1;
	}
	
	#endif
	
	// Others: Error
	return -1;
}
// Nuke end
