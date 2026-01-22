#include "version.h"
#include <sys/stat.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "common.h"
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "item.h"
#include "buf.h"
#include "readmap.h"
#include "pet.h"

static Object *obj;
static int objnum;

#ifndef _SIMPLIFY_ITEMSTRING
static int Restored = FALSE;
#endif

BOOL initObjectArray( int num )
{
    int     i;
    objnum = num;
    obj = allocateMemory( sizeof( Object ) * objnum );
    if( obj == NULL ) return FALSE;
    for( i = 0 ; i < objnum ; i ++ ){
        memset( &obj[i], 0 , sizeof( Object ));
        obj[i].type = OBJTYPE_NOUSE;
    }
    return TRUE;
}


BOOL endObjectArray( void )
{
    freeMemory( obj );
    return TRUE;
}

INLINE int _initObjectOne( char *file, int line, Object* ob )
{
    static int allocobjnum = 0;
    int i;
    BOOL    first;

    i = allocobjnum;
    first = TRUE;
    while( 1 ) {
        if( !first && i >= allocobjnum ) {
            fprint( "Can't allocate an object\n" );
            return -1;
        }
        if( obj[i].type == OBJTYPE_NOUSE ) {
            if( MAP_addNewObj( ob->floor ,ob->x, ob->y, i ) == 1 ){
                memcpy( &obj[i] , ob , sizeof( Object ) );
                allocobjnum = ( i+1 >= objnum) ? 0:i+1;
                return i;
            }else{
                allocobjnum = ( i+1 >= objnum) ? 0:i+1;
                fprint( "MAP_addNewObj error\n" );
                return -1;
            }
        }else {
            if( ++i >= objnum ) {
                i = 0;
                first = FALSE;
            }
        }
    }

    fprint( "Can't allocate an object\n" );
    return -1;
}

void endObjectOne( int index )
{
    if(objnum <= index || index < 0 )return;

    if( MAP_removeObj(obj[index].floor,obj[index].x, obj[index].y,
                      index) == FALSE){
        //fprint( "REMOVE OBJ ERROR  floor:%d  x:%d  y:%d\n",obj[index].floor,obj[index].x, obj[index].y );
    }
    obj[index].type = OBJTYPE_NOUSE;
}

BOOL CHECKOBJECT( int index )
{
    if( 0<=index && index<objnum )return TRUE;
    return FALSE;
}

BOOL CHECKOBJECTUSE( int index )
{
    if( CHECKOBJECT(index) == FALSE )return FALSE;
    if( obj[index].type == OBJTYPE_NOUSE )return FALSE;
    return TRUE;
}

INLINE int OBJECT_getType( int index )
{
    return obj[index].type;
}

#ifdef _NPCSERVER_NEW
INLINE int OBJECT_getNpcIndex( int index )
{
    return obj[index].npcsindex;
}
INLINE char* OBJECT_getName( int index )
{
    return obj[index].objname;
}//imagenum
INLINE int OBJECT_getNpcImage( int index )
{
    return obj[index].imagenum;
}
INLINE int OBJECT_getNpcdir( int index )
{
    return obj[index].dir;
}

#endif

INLINE int OBJECT_getchartype( int index )
{
    return obj[index].chartype;
}
INLINE void OBJECT_setchartype( int index, int flg)
{
    obj[index].chartype = flg;
}

INLINE int OBJECT_setType( int index, int newvalue )
{
    int     old;
    old = obj[index].type;
    obj[index].type = newvalue;
    return old;
}

INLINE int OBJECT_getFloor( int index )
{
    return obj[index].floor;
}

INLINE int OBJECT_setFloor( int index, int newvalue )
{
    int     old;
    old = obj[index].floor;
    obj[index].floor = newvalue;
    return old;
}

INLINE int OBJECT_getX( int index )
{
    return obj[index].x;
}

INLINE int OBJECT_setX( int index, int newvalue )
{
    int     old;
    old = obj[index].x;
    obj[index].x = newvalue;
    return old;
}

INLINE int OBJECT_getY( int index )
{
    return obj[index].y;
}

INLINE int OBJECT_setY( int index, int newvalue )
{
    int     old;
    old = obj[index].y;
    obj[index].y = newvalue;
    return old;
}

#ifdef _DEL_DROP_GOLD
INLINE int OBJECT_getTime( int index )
{
    return obj[index].time;
}

INLINE int OBJECT_setTime( int index, int newvalue )
{
    int     old;
    old = obj[index].time;
    obj[index].time = newvalue;
    return old;
}
#endif


/*------------------------------------------------------------
 * index 毛  月
 * 娄醒
 *  index      int     奶件犯永弁旦
 * 忒曰袄
 *  int
 ------------------------------------------------------------*/
INLINE int OBJECT_getIndex( int index )
{
    return obj[index].index;
}
/*------------------------------------------------------------
 * y 毛涩烂允月
 * 娄醒
 *  index      int     奶件犯永弁旦
 *  newvalue    int     蕙仄中袄
 * 忒曰袄
 *  int 樯及袄
 ------------------------------------------------------------*/
INLINE int OBJECT_setIndex( int index, int newvalue )
{
    int     old;
    old = obj[index].index;
    obj[index].index = newvalue;
    return old;
}


/*------------------------------------------------------------
 * 左皮斥尼弁玄及    醒毛菲户月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  int
 ------------------------------------------------------------*/
INLINE int OBJECT_getNum( void )
{
    return objnum;
}

/*------------------------------------------------------------
 *  左皮斥尼弁玄厌瞻  及丢件田毛蝈  娄醒卞  匀化
 *  左皮斥尼弁玄厌瞻  毛综曰请仄化｝瓒  允月［
 *
 * 娄醒
 *  type    OBJTYPE     左皮斥尼弁玄及正奶皿
 *  index   int         公木冗木及    匹及index
 *  x       int         x甄  
 *  y       int         y甄  
 *  floor   int         floorid
 * 忒曰袄
 *   int  喃曰癫化木凶index, 撩  及凛反｝ -1
 ------------------------------------------------------------*/
int initObjectFromObjectMember(
    OBJTYPE type, int index, int x, int y , int floor )
{
    Object  obj;
    obj.type = type;
    obj.index = index;
    obj.x = x;
    obj.y = y;
    obj.floor = floor;

    return initObjectOne( &obj );
}

int searchObjectFromCharaIndex( int index )
{
    int i;
    for( i = 0 ; i < objnum ; i ++ )
        if( obj[i].type == OBJTYPE_CHARA && obj[i].index == index )
            return i;
    return -1;
}

#define ITEMGOLDSTOREFILENAME   "itemgold"

#define STOREITEMID         "ITEM"
#define STOREGOLDID         "GOLD"
#define	STORECHARID			"CHAR"

#ifndef _SIMPLIFY_ITEMSTRING
static BOOL checkObjectStoreFile( char* line, Object* one, char** stringstart)
{
    char    token[16];
    int     ret;
    int     i;

    ret = getStringFromIndexWithDelim(line,"|" ,1 , token, sizeof( token ) );
    if( ret == FALSE )return FALSE;
    if( strcmp( token , STOREITEMID ) == 0 )
        one->type = OBJTYPE_ITEM;
    else if( strcmp( token , STOREGOLDID ) == 0 )
        one->type = OBJTYPE_GOLD;
	else if( strcmp( token, STORECHARID) == 0 )
		one->type = OBJTYPE_CHARA;
    for(  i = 2 ; i < 5 ; i ++ ){
        char    first[64];
        char    second[64];
        ret = getStringFromIndexWithDelim(line,"|" ,i,
                                          token,sizeof( token ));
        if( ret == FALSE )return FALSE;

        ret = getStringFromIndexWithDelim(token,"=" ,1, first,sizeof( first ));
        if( ret == FALSE )return FALSE;
        ret = getStringFromIndexWithDelim(token,"=" ,2, second,sizeof( second ));
        if( ret == FALSE )return FALSE;

        if( strcmp( first , "x" ) == 0 )
            one->x = atoi( second );
        else if( strcmp( first , "y" ) == 0 )
            one->y = atoi( second );
        else if( strcmp( first , "floor" ) == 0 )
            one->floor = atoi( second );
    }
    {
        char*   findex = nindex( line , '|' , 4);
        *stringstart = findex + 1;
    }
    return TRUE;
}

BOOL storeObjects( char* dirname )
{
    int i;
    FILE*   igfile;
    char    igfilename[256];
	
	if( Restored == TRUE ){
    	snprintf( igfilename ,sizeof( igfilename ) ,"%s/%s" ,dirname ,
              ITEMGOLDSTOREFILENAME );
	}else{
    	snprintf( igfilename ,sizeof( igfilename ) ,"%s/%s_extra" ,dirname ,
              ITEMGOLDSTOREFILENAME );

                print( "\n---- Saving didn't do  with gmsv. ----- \n");
                print( "---- It is because gmsv stopped before reading . ----- \n");
                print( "---- It is kept with a name of (%s) just in case. ----- \n", igfilename );
	}
    igfile = fopen( igfilename , "w" );
    if( igfile == NULL ){
        print( "\n\n---- ERR:can not open (%s) for backup items. ----- \n\n", igfilename );
    	return FALSE;
    }

    print( "Store Objects ...");
    for( i = 0 ; i < objnum ; i ++ ){
        switch( obj[i].type ){
        case OBJTYPE_ITEM:
        {
            fprintf( igfile , STOREITEMID "|x=%d|y=%d|floor=%d|%s\n" ,
                     obj[i].x , obj[i].y ,obj[i].floor,
                     ITEM_makeStringFromItemIndex( obj[i].index,0 ) );
            break;
        }
        case OBJTYPE_GOLD:
        {
            fprintf( igfile , STOREGOLDID  "|x=%d|y=%d|floor=%d|%d\n" ,
                     obj[i].x , obj[i].y ,obj[i].floor, obj[i].index );
            break;
        }
        case OBJTYPE_CHARA:
        {
        	int	petindex = obj[i].index;
        	if( CHAR_CHECKINDEX( petindex) ) {
	        	if( CHAR_getInt( obj[i].index, CHAR_WHICHTYPE) == CHAR_TYPEPET){
		            fprintf( igfile , STORECHARID "|x=%d|y=%d|floor=%d|%s\n" ,
		                     obj[i].x , obj[i].y ,obj[i].floor,
		                     CHAR_makePetStringFromPetIndex( obj[i].index ) );
				}
        	}
        	break;
        }
        default:
            break;
        }
    }
    fclose( igfile );
    print( "done\n");
system( "./itemda.pl" );
    print( "item backup end\n");
    return TRUE;
}
#endif

int detectStoreFile( char*  filename )
{
    FILE*   f;
    char    buffer[10];
    f = fopen ( filename ,"r" );
    if( f == NULL )return 3;

    if( fgets( buffer, sizeof( buffer ),  f ) == NULL ){
        fclose(f);
        return 3;
    }

    if( strncmp(buffer,STOREITEMID,sizeof(STOREITEMID) ) == 0 ||
        strncmp(buffer,STOREGOLDID,sizeof(STOREGOLDID) ) == 0   ){
        fclose(f);
        return 2;
    }

    fclose(f);
    return 3;
}

#ifndef _SIMPLIFY_ITEMSTRING
BOOL restoreObjects( char* dirname )
{
    char    igfilename[512];
    FILE*   file;
    char    line[2048];

    snprintf( igfilename, sizeof(igfilename), "%s/%s", dirname, ITEMGOLDSTOREFILENAME );
    file = fopen( igfilename , "r" );
    if( file == NULL ){
		Restored = TRUE;
    	return FALSE;
    }
    while(  fgets( line, sizeof( line ), file ) != NULL ){
        Object  one;
        char    *string;
        chomp( line );
        if( checkObjectStoreFile( line, &one ,&string ) == FALSE )
            continue;
        switch( one.type ){
        case OBJTYPE_ITEM:
        {
            ITEM_Item   itmone;
            BOOL ret;
            int     objindex;
            ret = ITEM_makeExistItemsFromStringToArg( string, &itmone, 0 );
            if( ret == TRUE ){
                one.index = ITEM_initExistItemsOne( &itmone );
                if( one.index == -1 )   break;
                objindex = initObjectOne( &one );
                ITEM_setWorkInt(one.index,ITEM_WORKCHARAINDEX,-1);
                ITEM_setWorkInt(one.index,ITEM_WORKOBJINDEX,objindex);
            }
            break;
        }

        case OBJTYPE_GOLD:
            one.index = atoi( string );
            initObjectOne( &one );
            break;
		case OBJTYPE_CHARA:
		{
			Char	ch;
			int		ret;
			ret = CHAR_makePetFromStringToArg(string,&ch, -1);
			if( ret ) {
			    Object object;
			    int objindex;
		        int petindex = PET_initCharOneArray( &ch );
		        if( petindex < 0 ) {
		        	print( "宠物制作失败。\n");
		        }
			    object.type = OBJTYPE_CHARA;
			    object.index = petindex;
			    object.x = ch.data[CHAR_X];
			    object.y = ch.data[CHAR_Y];
			    object.floor = ch.data[CHAR_FLOOR];
			    objindex = initObjectOne( &object );
				if( objindex == -1 ) {
	                CHAR_endCharOneArray( petindex );
				}else {
				    CHAR_setWorkInt( petindex,CHAR_WORKOBJINDEX,objindex );
				}
			}
			break;
        }
        default:
            break;
        }
    }
    fclose( file );
	Restored = TRUE;

    return TRUE;
}
#endif
