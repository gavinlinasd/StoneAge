/*
  溘璃す璃精敦  僑拻溢↓淏穫↓筒摯  薔��

  1999 Aug 14 Created by ringo

  溢↓淏穫↓筒毀1  摯惉璃袲摯魠ぁ瞀凎踏躂堎��
  
  
 */
#define _DB_C_
#include "db.h"
#include "main.h"
#include "util.h"
#include "version.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

/*   棬  操璃哱惉摯    length */
//#define CHARVALUE_MAX 1024
#define MAXTABLE 16
// Spock 2000/10/12
#define CHARVALUE_MAX 256	// DB 趼揹訧蹋腔buffer湮苤
#define KEY_MAX 64		// DB Key趼揹腔buffer湮苤
#define HASH_SIZE 65536		// Hash table 珨棒崝樓腔Entry杅講
#define HASH_PRIME 65521	// Hash function 妏蚚腔窐杅
#define DBINIT_SIZE 16384	// DB 藩棒饜离Entry腔杅講
// Spock end

/* 溢↓淏穫↓筒摯惉璃袲摯菺搫1蠀禱堣�桭灄呁� */
struct dbentry
{
    int use;
//    unsigned int keyhash;       /* 號喙す↓摯辿蚗で堥昡↓迶 */
    int ivalue;                  /* 筒昡囮�覕�蚗鏤 NODE 毀  -1 ぁ��
                                 埰迋趙摯筒昡囮毀 0 雄窀ぁ埵笢敁笢崹埵笢*/
//    int nextind;                /* -1 煦埱倜��    禱徽  埰堎 */
    // Spock 2000/10/12
    int prev;	// ヶ珨跺dbentry, -1桶尨森砐峈head
    int next;	// 狟珨跺dbentry, -1桶尨森砐峈tail
    char key[KEY_MAX];
    char charvalue[CHARVALUE_MAX];
//    char key[64];               /* 號喙す↓敁埵堎  棬   */
//    int charvalue_index;        /*   棬  泬蚗啞栝禱踏埰index */
    // Spock end
};

// Spock 2000/10/12
// Database hashtable
struct hashentry
{
    char key[KEY_MAX];	// 坰竘key硉
    int use;		// 岆瘁眒掩妏蚚
    int dbind;		// 硌砃 dbentry 腔 index
    int prev;		// 肮珨key硉腔奻珨跺 hashentry, -1峈head
    int next;		// 肮珨key硉腔狟珨跺 hashentry, -1峈tail
};
// Spock end

typedef enum
{
    DB_INT_SORTED,
    DB_STRING,
}DBTYPE;

/* 1蠀摯溢↓淏穫↓筒禱堣�桭灄� */
struct table
{
    int use;		// 0:帤妏蚚 1:眒妏蚚
    DBTYPE type;                    /* DB摯攣   */
    char name[32];                  /* 溢↓淏穫↓筒摯  鞳 */
    int num;                        /* 操璃哱惉摯倳 */
    int toplinkindex;
    // Spock 2000/10/12
    struct hashentry *hashtable;
    int hashsize;
    int updated;	// 0:dbflush摽帤載陔 1:眒載陔
    int ent_finder;	// 硌砃郔摽珨棒饜离腔 hashentry
    // Spock end
};

struct dbentry *master_buf;     /* 操璃哱惉楔呏暵 */
int dbsize = 0;                 /*   疐0ぁ��1,2,4,8,16...*/
static int dbent_finder = 0;

struct table dbt[MAXTABLE];

static void dbShowAllTable(void);

// Spock 2000/10/12
int dbHash(char* s)
{
    char *p;
    unsigned int h= 0 ,g;
    for( p = s ; *p ; p ++ ){
        h = ( h<< 4 ) + (*p);
        if( (g = h & 0xf0000000) != 0){
            h = h ^ (g>>24);
            h = h ^ g;
        }
    }
    return h % HASH_PRIME;
}
// Spock end

/* Spock deleted 2000/10/12
struct charvalue
{
    int use;
    char buf[CHARVALUE_MAX];
};
struct charvalue *charvalue_buf;
int charvaluesize=0;
*/

/*
    棬  泬蚗啞栝↓禱錚耦埰堎
 */
/* Spock deleted 2000/10/12
int
reallocCharValue(void)
{
    struct charvalue *previous = charvalue_buf;
    struct charvalue *newbuf;
    int new_charvaluesize;
    if( charvaluesize == 0 ){
        new_charvaluesize = 1;
    } else {
        new_charvaluesize = charvaluesize * 2;
    }

    newbuf = ( struct charvalue *) calloc( 1, new_charvaluesize *
                                          sizeof( struct charvalue ));
    if( newbuf == NULL ){
        log( "reallocCharValue: memory shortage!! new_charvaluesize:%d\n",
             new_charvaluesize );
        return -1;
    }
    memset( newbuf, 0 , new_charvaluesize * sizeof( struct charvalue ));
    if( previous) memcpy( (char*)newbuf, (char*)previous,
            charvaluesize * sizeof( struct charvalue ));
    free( previous );
    charvaluesize = new_charvaluesize;
    charvalue_buf = newbuf;

    log( "reallocCharValue: "
         "new_charvaluesize:%d Old address:%x New address:%x\n",
         new_charvaluesize , (unsigned int )previous,
         (unsigned int)newbuf );
    return 0;
}
*/

/*
    棬  泬蚗啞栝↓禱1蠀渦堇堣趙堎��
  倜堇埵�忐耒�倜�捯ealloc埰堎��
  
*/
/* Spock deleted 2000/10/12
static int charvalue_finder=0;
static int
dbAllocCharValue( void )
{
    int i;
    for(i=0;i<charvaluesize;i++){
        charvalue_finder++;
        if( charvalue_finder == charvaluesize ) charvalue_finder =0;
        if( charvalue_buf[charvalue_finder].use == 0 ){
            charvalue_buf[charvalue_finder].use =1;
            charvalue_buf[charvalue_finder].buf[0] = 0;
            return charvalue_finder;
        }

    }
    log( "dbAllocCharValue: charvalue array full. reallocating....\n" );
    if( reallocCharValue() < 0 ){
        log( "dbAllocCharValue: reallocation fail\n");
    } else {
        return dbAllocCharValue();
    }
    return -1;
}
*/

/*
  charvalue 凝��/勗偯禱斛蚗哱/掛蚗哱埰堎
  int index : charvalue index
  
 */
/* Spock deleted 2000/10/12
static char *
dbGetString( int index )
{
    return charvalue_buf[index].buf;
}
static int
dbSetString( int index , char *data )
{
    int l = strlen(data);
    if( l >= (sizeof( charvalue_buf[0].buf )-1)) return -1;
    memcpy( charvalue_buf[index].buf , data, l+1 );
    return 0;
}
*/

/*
  DB摯堁堁拻踏誑倜�梛珂忐耒�倜�桭灄隙六胰耒ば呁詎倷�摯�蚅昃齞�2  勗埰堎
  0煦埱倜��1勗埰堎
 */
static int
reallocDB( void )
{
    struct dbentry *previous = master_buf;
    struct dbentry *newbuf;
    int new_dbsize;
/* Spock deleted 2000/10/12    
    if( dbsize == 0 ){
        new_dbsize = 1;
    } else {
        new_dbsize = dbsize * 2;
    }
*/
    // Spock+1 2000/10/12
    new_dbsize = dbsize + DBINIT_SIZE;
    
    newbuf = (struct dbentry* ) calloc( 1, new_dbsize *
                                         sizeof( struct dbentry) );
    /* 隍さ惉倜堇埵笢   */
    if( newbuf == NULL ){
        log( "笭陔煦饜杅擂: 囀湔祥逋!!! 陔杅擂湮苤: %d\n", new_dbsize );
        return -1;
    }

    /* 捙笢酵竣凝�殌目ひ�  勗昡鼀↓媃趙 */

    memset( newbuf , 0 , new_dbsize * sizeof( struct dbentry ) );
    /* Spock deleted 2000/10/19
    if( previous )memcpy( (char*)newbuf, (char*)previous,
            dbsize * sizeof( struct dbentry ));

    // 捙笢酵竣禱搣  媃
    free( previous );
    */
    // Spock 2000/10/19
    if ( dbsize > 0 )
    {
    	memcpy( newbuf , previous , dbsize * sizeof(struct dbentry));
    	free( previous );
    }
    // Spock end

    dbent_finder = dbsize;	// 蔚 dbent_finder 硌砃帤妏蚚腔 entry
    dbsize = new_dbsize;
    master_buf = newbuf;
    
    log( "笭陔煦饜杅擂: 陔杅擂湮苤:%d 導華硊: %x 陔華硊:%x\n",
         new_dbsize , (unsigned int)previous, (unsigned int)newbuf );

    return 0;
}



/*
  allocate a node
 */
static int
//dbAllocNode( DBTYPE type  )
// Spock +1 2000/10/13
dbAllocNode()
{
    int i;
    for(i=0;i<dbsize;i++){
        dbent_finder ++;
        if( dbent_finder == dbsize ) {
            dbent_finder = 0;
        }
        if( master_buf[dbent_finder].use == 0 ){
            master_buf[dbent_finder].use = 1;
            /* Spock deleted 2000/10/12
            // int ぁ忒痹萃攷  摯諱勗stringbuffer禱  僑痀勗埰堎 kawata
            if( type == DB_STRING || type == DB_INT_SORTED){
                if( ( master_buf[dbent_finder].charvalue_index =
                      dbAllocCharValue() ) < 0 ){
                    //   棬  泬蚗啞栝↓誑倜堇埵笢��
                    return -1;
                }
            }
            */
            return dbent_finder;
        }
    }
    log( "杅擂輛�賱蚆倇妍�. 笭陔煦饜笢....\n" );
    if( reallocDB() < 0 ){
        log( "笭陔煦饜囮啖\n" );
    } else {
        //return dbAllocNode( type );
        // Spock 2000/10/13
        master_buf[dbent_finder].use = 1;
        log( "杅擂抻聆=%d\n" , dbent_finder );
        return dbent_finder;
        // Spock end
    }
    return -1;
}
static void
dbReleaseNode( int index )
{
    // Spock 2000/10/12
    int prev = master_buf[index].prev;
    int next = master_buf[index].next;
    master_buf[index].use = 0;
    if ( prev >= 0 ) master_buf[prev].next = next;
    if ( next >= 0 ) master_buf[next].prev = prev;
    // Spock end
    /* Spock deleted 2000/10/12
	if( master_buf[index].charvalue_index >= 0 ) {
		charvalue_buf[ master_buf[index].charvalue_index].use = 0;
	}
    */
}
void
dbShowLink( int topind )
{
    int cur = topind;

    log( "羲宎植 %d 蟈諉杅擂\n", cur );
    
    /* Spock deleted 2000/10/19
    for(;;){
        if( cur == -1 )break;
    */
    // Spock +1 2000/10/19
    while ( cur >= 0 )
    {
        if( master_buf[cur].use == 0 ){
            log( "dbShowLink: use is 0! key:%s\n", master_buf[cur].key );
            return;
        }
        // Spock +1  2000/10/12
        log( "%s %i\n", master_buf[cur].key, master_buf[cur].ivalue );
        /* Spock deleted 2000/10/12
        log( "%s %u %i\n", master_buf[cur].key ,
             master_buf[cur].keyhash, master_buf[cur].ivalue );
        */
        cur = master_buf[cur].next;
    }
}
// Spock 2000/10/13
static int
reallocHash( int dbi )
{
    struct hashentry *previous = dbt[dbi].hashtable;
    struct hashentry *newbuf;
    int new_hashsize;
 
    new_hashsize = dbt[dbi].hashsize + HASH_SIZE;
    newbuf = (struct hashentry* ) calloc( 1, new_hashsize *
                                         sizeof( struct hashentry) );
    if( newbuf == NULL ){
        log( "笭陔煦饜拸蚚陓洘: 囀湔祥逋!!! 陔拸蚚陓洘湮苤: %d\n", new_hashsize );
        return -1;
    }

    memset( newbuf , 0 , new_hashsize * sizeof( struct hashentry ) );
    if( previous )
    {
    	memcpy( newbuf, previous,
            dbt[dbi].hashsize * sizeof( struct hashentry ));
        free( previous );
    }

    if ( dbt[dbi].hashsize > HASH_PRIME )
        dbt[dbi].ent_finder = dbt[dbi].hashsize;
    else
        dbt[dbi].ent_finder = HASH_PRIME;
    dbt[dbi].hashsize = new_hashsize;
    dbt[dbi].hashtable = newbuf;
    
    log( "笭陔煦饜拸蚚陓洘: 陔拸蚚陓洘湮苤:%d 導華硊: %x 陔華硊:%x\n",
         new_hashsize , (unsigned int)previous, (unsigned int)newbuf );

    return 0;
}

static int tableGetEntry( int dbi , char *k )
{
    int hashkey = dbHash( k );
    struct hashentry *hash = dbt[dbi].hashtable;
    if ( hash[hashkey].use == 0 ) return -1;
    while ( 1 ) {
//		if ( hash[hashkey].use == 1 && strcmp( hash[hashkey].key , k ) == 0 ){
		if ( hash[hashkey].use == 1 ){
			if( strcmp( hash[hashkey].key , k ) == 0 )return hashkey;
		}
   		hashkey = hash[hashkey].next;
		if ( hashkey <= 0 ){
//			log("err not found hash[%x] -%s!\n", hashkey, k)
			return -1;
		} 
    }
}

static int tableInsertNode( int dbi , char *k , int dbind )
{
    int hashkey = dbHash( k );
    int hashnext = -1;
    int i;
    struct hashentry *hash = dbt[dbi].hashtable;
  
    if ( hash[hashkey].use == 0 )  {
    	strcpy( hash[hashkey].key , k );
   	hash[hashkey].use = 1;
    	hash[hashkey].dbind = dbind;
    	hash[hashkey].prev = -1;
    	hash[hashkey].next = -1;
    	dbt[dbi].num++;
        return hashkey;
    }else {
    	for ( i=0; i<dbt[dbi].hashsize-HASH_PRIME; i++ ){
    	    dbt[dbi].ent_finder++;
    	    if ( dbt[dbi].ent_finder >= dbt[dbi].hashsize )
    	        dbt[dbi].ent_finder = HASH_PRIME;
    	    if ( hash[dbt[dbi].ent_finder].use == 0 )
    	    {
    	        hashnext = dbt[dbi].ent_finder;
    	        break;
    	    }
    	}
    	if ( hashnext < HASH_PRIME )
    	{
            log( "tableInsertNode: hashentry array full. reallocating....\n" );
            if( reallocHash( dbi ) < 0 ){
                log( "tableInsertNode: reallocation fail\n" );
                return -1;
            }
            else
            {
            	hash = dbt[dbi].hashtable;
            	hashnext = dbt[dbi].ent_finder;
            }
        }
        strcpy( hash[hashnext].key , k );
        hash[hashnext].use = 1;
        hash[hashnext].dbind = dbind;
        hash[hashnext].prev = hashkey;
        hash[hashnext].next = hash[hashkey].next;
        if ( hash[hashkey].next >= 0 )
            hash[hash[hashkey].next].prev = hashnext;
        hash[hashkey].next = hashnext;
        dbt[dbi].num++;
        return hashnext;
    }	
}

static void
tableReleaseNode( int dbi , int ind )
{
    dbt[dbi].hashtable[ind].use = 0;
    if ( dbt[dbi].hashtable[ind].prev >= 0 ){
        dbt[dbi].hashtable[dbt[dbi].hashtable[ind].prev].next =
        dbt[dbi].hashtable[ind].next;
    }
    if ( dbt[dbi].hashtable[ind].next >= 0 ){
        dbt[dbi].hashtable[dbt[dbi].hashtable[ind].next].prev =
        dbt[dbi].hashtable[ind].prev;
    }
    dbt[dbi].num--;
}
// Spock end
/*
  惉璃袲摯哱蚗鏤禱僄窇�桫憶觕掁�す↓禱  堇勗蚚↓迶禱號喙埰堎��
  陑僑凝�欷聿剒踽�毀操溘↓ぁ毀埵笢摯ぁ0

 */
/* Spock deleted 2000/10/13
static int
dbExtractNodeByKey( int topind , char *k  )
{
    int cur = topind;
    int prev = -1;
    unsigned int h = hashpjw( k );

    // 惉璃袲誑昶ぁ忒陑僑凝�欷聿郱笭縋汝副�0禱凝窇埰
    if( topind == -1 ) return 0;

    for(;;){
        if( cur == -1 )break;
        if( master_buf[cur].keyhash == h
            && strcmp( master_buf[cur].key , k ) == 0 ){
            // prev 摯 べ誑 cur 摯べ勗埵堎源竣勗埰堎
            if( prev == -1 ){
                // 袸  煦埱倜摯ぁ惉璃袲毀笢啋�欷聿�
            } else {                
                master_buf[prev].nextind = master_buf[cur].nextind;
            }
            // 鼠躂ぁ猷覕誑惉筒哱凝�梒衝戰翹副禾�  埰堎
            dbReleaseNode( cur );
            log( "find key %s deleted\n", k );
            return 0;
        }
        prev = cur;
        cur = master_buf[cur].nextind;
    }
    // not found
    log( "dbExtractNodeBykey: %s not found\n" , k );
    return 0;
}
*/
// Spock 2000/10/13
static int
dbExtractNodeByKey( int dbi , char *k  )
{
    int hashind = tableGetEntry( dbi , k );

    if ( hashind < 0 ){
    	log( "dbExtractNodeByKey: tableGetEntry fail, key:%s\n" , k );
    	return -1;
    }
    if ( dbt[dbi].hashtable[hashind].dbind < 0 ){
    	log( "dbExtractNodeByKey: invalid dbind in hash, key:%s\n" , k );
    	return -1;
    }
    dbReleaseNode( dbt[dbi].hashtable[hashind].dbind );
    tableReleaseNode( dbi , hashind );
    return 0;
}
// Spock end

/* Spock deleted 2000/10/12
static int 
dbGetEntryByKey( int topind , char *k )
{
    int cur = topind;
    unsigned int h = hashpjw( k );

    if( topind == -1 ) return 0;

    for(;;){
        if( cur == -1 )break;
        if( master_buf[cur].keyhash == h
            && strcmp( master_buf[cur].key, k ) == 0 ){
            return cur;
        }
        cur = master_buf[cur].nextind;
    }
    return -1;
}
*/

/*
  惉璃袲摯哱蚗鏤禱僄窇�桫憶觕掁�偯禱  堇勗蚚↓迶禱號喙媃趙
  媏憍埵敁喫Й勗 Insert 埰堎�裀倗郇鵖�  凝�梊ばな樛�  勗埵�梛玅皮俶蟲�
  痄擭

 */
/* Spock deleted 2000/10/13
static int
dbInsertNodeByIValue( int topind , int ins )
{
    int cur = topind;
    int prev = -1;

    if( topind == -1 ) return -1;
    
    for(;;){
        if( cur == -1 ){
            //     竘ぁ笢埱倜摯ぁ黹萃埰堎
            master_buf[prev].nextind = ins;
            master_buf[ins].nextind = -1;
            return 0;
        }
        if( master_buf[cur].ivalue < master_buf[ins].ivalue ){
            if( prev == -1 ){
                log( "top_node is badly configured\n" );
                return -1;
            }
            master_buf[prev].nextind = ins;
            master_buf[ins].nextind = cur;
            return 0;
        }
        prev = cur;
        cur = master_buf[cur].nextind;
    }
    
    return -1;
}
*/
// Spock 2000/10/13
static int
dbInsertNodeByIValue( int topind , int ins )
{
    int cur = topind;
    if ( (topind < 0)||(topind >= dbsize)||(ins < 0)||(ins >= dbsize ) )
        return -1;
    while ( master_buf[cur].next >= 0 ){
    	if ( master_buf[master_buf[cur].next].ivalue < master_buf[ins].ivalue )
    	    break;
    	cur = master_buf[cur].next;
    }
    master_buf[ins].prev = cur;
    master_buf[ins].next = master_buf[cur].next;
    if ( master_buf[cur].next >= 0 )
        master_buf[master_buf[cur].next].prev = ins;
    master_buf[cur].next = ins;
    return 0;
}
// Spock end
/* Spock deleted 2000/10/13
static int
dbAppendNode( int topind , int ins )
{
    int cur =topind;
    int prev = -1;
    if( topind == -1 ) return -1;
    for(;;){
        if( cur == -1 ){
            master_buf[prev].nextind = ins;
            master_buf[ins].nextind = -1;
            return 0;
        }
        prev = cur;
        cur = master_buf[cur].nextind;
    }
    return -1;
}
*/
// Spock 2000/10/13
static int
dbAppendNode( int topind , int ins )
{

    if ( (topind < 0)||(topind >= dbsize)||(ins < 0)||(ins >= dbsize ) )
        return -1;
    master_buf[ins].prev = topind;
    master_buf[ins].next = master_buf[topind].next;
    if ( master_buf[topind].next >= 0 )
        master_buf[master_buf[topind].next].prev = ins;
    master_buf[topind].next = ins;
    return 0;
}
// Spock end

/*
  溢↓淏穫↓筒摯  鞳禱  堎�菤b毀倳誑撙埵笢摯ぁ  玊勗strcmp媃趙源笢
  DBTYPE :攣  ��

  堁埵啋埵竘窇摯  毀��堆倳敁  棬  摯    勗凎凜埰堎喫敁誑ぁ拻堎��
  
 */
static int
dbGetTableIndex( char *tname , DBTYPE type )
{
    int i;

    for(i=0;i<MAXTABLE;i++){
        if( dbt[i].use && strcmp( dbt[i].name , tname ) == 0 &&
            dbt[i].type == type ){
            return i ;
        }
    }

    /* 陑僑凝�欷汙朗�倜摯ぁ犍踐煦 */
    for(i=0;i<MAXTABLE;i++){
        if( dbt[i].use == 0 ){
            int topind;
            dbt[i].use = 1;
            dbt[i].type = type;
            snprintf( dbt[i].name , sizeof( dbt[i].name ) , "%s", tname );
            // Spock 2000/10/16
            if ( reallocHash( i ) < 0 )
            {
            	log( "笭陔煦饜拸蚚陓洘囮啖\n");
            	return -2;
            }
            dbt[i].ent_finder = HASH_PRIME;
            // Spock end

            //topind = dbAllocNode( type );
            // Spock +1 2000/10/16
            topind = dbAllocNode();
            if( topind < 0 ){
                log( "杅擂煦饜誹萸囮啖\n" );
                return -2;
            }
            /* Spock deleted 2000/10/16
            snprintf( master_buf[topind].key ,
                      sizeof(master_buf[topind].key), "top_node" );
            master_buf[topind].keyhash = hashpjw( master_buf[topind].key );
            master_buf[topind].nextind = -1;
            */
            
            /* 哱蚗鏤摯蚚↓迶禱疐趵撬埰堎埵堇��
               棬  摯鳹譴忒堆倳摯鳹譴忒堁埵啋ぁ源笢��
             0x7fffffff敁笢竣偯毀��  棬  摯鳹譴毀踏酵汔徽  禱忒倜埵笢摯煦��*/
            master_buf[topind].ivalue = 0x7fffffff;    
            /* Spock deleted 2000/10/16
            if( type == DB_INT_SORTED ){
                master_buf[topind].charvalue_index = -1;
                //dbSetString( master_buf[topind].charvalue_index, "" );
            } else {
                dbSetString( master_buf[topind].charvalue_index, "" );
            }
	    */
            // Spock 2000/10/16
            master_buf[topind].prev = -1;
            master_buf[topind].next = -1;
            strcpy( master_buf[topind].key , "top_node" );
            master_buf[topind].charvalue[0] = 0;
            // Spock end
            dbt[i].toplinkindex = topind;
            return i;
        }
    }

    /*   誑笢埱毞笢煦�� */
    log( "dbGetTableIndex: table full. now tables are:\n" );
    dbShowAllTable();

    return -1;
}

/*

  敁堇堣窇囀 strtol ぁ拻堎偯媃凝�蚨怴欃�媃埵笢源

  號喙媃趙峟僑崹倜蚚↓迶勗葡媃趙��
  惉璃袲凝��  拻敁埱趙凝�掁�ぽ梣毀藺↓哱埰堎摯勗窅竣偯禱倜源堇勗
  號喙媃趙��笢笢鳹鐎禱陑僑崹倜�掁�鼠喫勗蕍媃喫資��
  
 */
/* Spock deleted 2000/10/16
int dbUpdateEntryInt( char *table , char *key , int value, char *info )
{
    int r, dbi = dbGetTableIndex( table , DB_INT_SORTED );
    int entind;

    if( strlen(key) >= sizeof( master_buf[0].key) )return -1;
    
    if( dbi < 0 ) return -1;

    r = dbExtractNodeByKey( dbt[dbi].toplinkindex , key );
    if( r < 0 ){
        log( "dbUpdateEntryInt: dbExtractNodeByKey fail! bug!!!!\n" );
        return -1;
    }

    entind = dbAllocNode(DB_INT_SORTED);
    if( entind < 0 ) return -1;
    master_buf[entind].ivalue = value;
    snprintf( master_buf[entind].key ,
              sizeof(master_buf[entind].key), "%s", key );
    master_buf[entind].keyhash = hashpjw( master_buf[entind].key );
    master_buf[entind].nextind = -1; 
	
	// 痹萃攷  禱掛蚗哱埰堎
    dbSetString( master_buf[entind].charvalue_index, info );
	
	
    r = dbInsertNodeByIValue( dbt[dbi].toplinkindex , entind );
    if( r < 0 ){
        log( "dbUpdateEntryInt: dbInsertNodeByIValue failed\n" );
        return -1;
    }

    log( "dbUpdateEntryInt: successfully updated entry %s:%s:%d\n",
             table, key, value );
    return 0;    
    
}
*/
// Spock 2000/10/16
int dbUpdateEntryInt( char *table , char *key , int value, char *info )
{
    int dbi = dbGetTableIndex( table , DB_INT_SORTED );
    int dbind, hashind, newpos;
    // Spock 2000/10/23
    if ( strlen( key ) >= KEY_MAX ) {
    	log( "dbUpdateEntryInt: key is too long, key:%s\n", key );
    	return -1;
    }
    if ( strlen( info ) >= CHARVALUE_MAX ) {
    	log( "dbUpdateEntryInt: charvalue is too long, charvalue:%s\n", info );
    	return -1;
    }
    // Spock end
    if ( dbi < 0 ){
    	log( "dbUpdateEntryInt: dbGetTableIndex fail\n");
    	return -1;
    }
    hashind = tableGetEntry( dbi , key );
    if ( hashind < 0 )
    {
    	dbind = dbAllocNode();
    	if ( dbind < 0 )
    	{
    	    log( "dbUpdateEntryInt: dbAllocNode fail\n" );
    	    return -1;
    	}
    	master_buf[dbind].ivalue = value;
    	strcpy( master_buf[dbind].key , key );
    	strcpy( master_buf[dbind].charvalue , info );
    	if ( dbInsertNodeByIValue( dbt[dbi].toplinkindex , dbind ) < 0 )
    	{
    	    master_buf[dbind].use = 0;
    	    log( "dbUpdateEntryInt: dbInsertNodeByIValue fail\n" );
    	    return -1;
    	}
    	if ( tableInsertNode( dbi , key , dbind ) < 0 )
    	{
    	    dbReleaseNode( dbind );
    	    log( "dbUpdateEntryInt: tableInsertNode fail\n" );
    	    return -1;
    	}
    }
    else
    {
    	dbind = dbt[dbi].hashtable[hashind].dbind;
    	master_buf[dbind].ivalue = value;
    	strcpy( master_buf[dbind].charvalue , info );
    	newpos = dbind;
    	while ( master_buf[newpos].prev >= 0 )
    	{
      	    if ( value <= master_buf[master_buf[newpos].prev].ivalue )
    	    {
    	    	break;
    	    }
    	    newpos = master_buf[newpos].prev;
    	}
    	if ( newpos != dbind )
    	{
    	    master_buf[master_buf[dbind].prev].next = master_buf[dbind].next;
            if ( master_buf[dbind].next >= 0 )
    	        master_buf[master_buf[dbind].next].prev = master_buf[dbind].prev;
    	    master_buf[dbind].prev = master_buf[newpos].prev;
    	    master_buf[dbind].next = newpos;
    	    if ( master_buf[newpos].prev >= 0 )
    	        master_buf[master_buf[newpos].prev].next = dbind;
    	    master_buf[newpos].prev = dbind;
    	    dbt[dbi].updated = 1;
		/*
	    log( "dbUpdateEntryInt: successfully updated entry %s:%s:%d\n",
	        table, key, value );
		*/
    	    return 0;
    	}
    	while ( master_buf[newpos].next >= 0 )
    	{
    	    if ( value >= master_buf[master_buf[newpos].next].ivalue )
    	    {
    	    	break;
    	    }
	    newpos = master_buf[newpos].next;
    	}
    	if ( newpos != dbind )
    	{
    	    master_buf[master_buf[dbind].prev].next = master_buf[dbind].next;
    	    master_buf[master_buf[dbind].next].prev = master_buf[dbind].prev;
    	    master_buf[dbind].prev = newpos;
    	    master_buf[dbind].next = master_buf[newpos].next;
    	    if ( master_buf[newpos].next >= 0 )
    	        master_buf[master_buf[newpos].next].prev = dbind;
    	    master_buf[newpos].next = dbind;
    	}
    }
    dbt[dbi].updated = 1;
	/*
    log( "dbUpdateEntryInt: successfully updated entry %s:%s:%d\n",
             table, key, value );
			 */
    return 0;
}
// Spock end

int
dbDeleteEntryInt( char *table, char *key )
{
    int dbi = dbGetTableIndex( table , DB_INT_SORTED );
    int r;

    if ( strlen( key ) >= KEY_MAX ) {
    	log( "dbDeleteEntryInt: key is too long, key:%s\n", key );
    	return -1;
    }
    if( dbi < 0 ) {
    	log( "dbDeleteEntryInt: dbGetTableIndex failed for %s\n", table );
    	return -1;
    }
    //r = dbExtractNodeByKey( dbt[dbi].toplinkindex , key );
    // Spock fixed 2000/10/19
    r = dbExtractNodeByKey( dbi , key );
    if( r < 0 ){
        log( "dbDeleteEntryInt: dbExtractNodeByKey failed for %s in %s\n",
             key,table );
        return -1;
    }
    // Spock +1 2000/10/19
    dbt[dbi].updated = 1;
    log( "deleted key %s from table %s\n", key, table );
    return 0;
}

static void
dbShowAllTable(void)
{
    int i;

    for(i=0;i<MAXTABLE;i++){
        if( dbt[i].use ){
            log( "%d Name:%s Use:%d Type:%d\n",i,
                     dbt[i].name , dbt[i].num , dbt[i].type );
        }
    }
    
}

/* 溢↓淏禱1蠀噁堇煦埰��
 */
int
dbGetEntryInt( char *table, char *key, int *output )
{
    int dbi = dbGetTableIndex( table , DB_INT_SORTED );
    int entind;
    // Spock +1 2000/10/19
    int hashind;
 
    // Spock deleted 2000/10/19
    //if( strlen(key) >= sizeof( master_buf[entind].key) ) return -1;
    if( dbi <0 ) {
    	log( "dbGetEntryInt: dbGetTableIndex fail\n" );
    	return -1;
    }
    // Spock 2000/10/19
    if( strlen(key) >= KEY_MAX ) {
    	log( "dbGetEntryInt: key is too long, key:%s\n" , key );
    	return -1;
    }
    hashind = tableGetEntry( dbi , key );
    if( hashind < 0 ) return -1;
    entind = dbt[dbi].hashtable[hashind].dbind;
    //entind = dbGetEntryByKey( dbt[dbi].toplinkindex , key );
    // Spock end
    if( entind < 0 ) {
    	log( "dbGetEntryInt: Invalid dbind in hashtable of %s\n" , table );
    	return -1;
    }
    /* 陑僑凝埱倜摯ぁ偯禱③  勗  躂趙蒍埰 */
    *output = master_buf[entind].ivalue;

    return 0;
}

/*
  操溘↓摯鳹譴毀  ��0煦埱倜�梊�  ��

  int *rank_out : 溘璃袲摯③  
  int *count_out : 窀凝�欶砃�  凝摯③  

  int 溢↓淏穫↓筒障暵衭
  
 */

int
dbGetEntryRank( char *table, char *key , int *rank_out, int *count_out)
{
    int dbi = dbGetTableIndex( table , DB_INT_SORTED );
    // Spock deleted 2000/10/19
    //unsigned int hash = hashpjw(key);
    int cur;
    int now_score = 0x7fffffff;     /*int ぁ笢з�圴玅札核訄� */
    int r = -1 , i=0;

    // Spock 2000/10/23
    //if( strlen(key) >= sizeof( master_buf[cur].key) ) return -1;
    if( strlen(key) >= KEY_MAX ) {
    	log( "dbGetEntryRank: key is too long, key:%s\n" , key );
    	return -1;
    }
    if( dbi <0 ) {
    	log( "dbGetEntryRank: dbGetTableIndex fail\n" );
    	return -1;
    }
    // Spock end

    // Spock 2000/10/23
    //cur = master_buf[dbt[dbi].toplinkindex].nextind;
    cur = master_buf[dbt[dbi].toplinkindex].next;
    //i=0;
    //for(;;){
    //    if( cur == -1 )break;
    while ( cur >= 0 )
    {
    // Spock end
        if( master_buf[cur].ivalue != now_score ){
            r=i;
            now_score = master_buf[cur].ivalue;
        }
        // Spock 2000/10/19
        //if( hash == master_buf[cur].keyhash &&
        //    strcmp( master_buf[cur].key, key )== 0 ){
        if( strcmp( master_buf[cur].key , key ) == 0 )
        {
        // Spock end
            *rank_out = r;
            *count_out = i;
            return 0;
        }
        //cur = master_buf[cur].nextind;
        // Spock fixed 2000/10/19
        cur = master_buf[cur].next;
        i++;
    }
    *count_out = i;
    *rank_out = r;
    return 0;
}

/*
  int 障暵衭
 */
int
dbGetEntryRankRange( char *table,
                     int start, int end, char *output, int outlen )
{
#define MAXHITS 1024        /* 媏騍盓埵堣�袸瓜硈蟤嬴用畈倞欷ひ倞� ringo */
    struct hitent{          /* 喫摯栖桹  勗樅蚗哱媃倜盓僑禱倜誧趙笢�� */
        int entind;
        int rank;
    };

    int r=0;
    struct hitent hits[MAXHITS];
    int dbi = dbGetTableIndex( table , DB_INT_SORTED );
    int cur;
    int hitsuse = 0,i;
    int now_score = 0x7fffffff;

    if( dbi <0 ) return -1;
    if( outlen <= 0 )return -1;
    
    cur = dbt[dbi].toplinkindex;
    // Spock 2000/10/23
    //for(;;){
    //    if( cur == -1 )break;
    while ( cur >= 0 )
    {
    // Spock end
        if( master_buf[cur].ivalue != now_score ){
            r++;
            now_score = master_buf[cur].ivalue;
        }
        if( r >= start && r <= end ){
            hits[hitsuse].entind = cur;
            hits[hitsuse].rank = r;
            hitsuse++;
            //if( hitsuse == MAXHITS )break;
            // Spock fixed 2000/10/23
            if( hitsuse >= MAXHITS ) break;
        }
        //cur = master_buf[cur].nextind;
        // Spock fixed 2000/10/19
        cur = master_buf[cur].next;
    }
    output[0] = 0;
    
    for(i=0;i<hitsuse;i++){
        char tmp[1024];
        snprintf( tmp, sizeof(tmp),
                  "%d,%s,%d,%s", hits[i].rank, master_buf[hits[i].entind].key,
                  master_buf[hits[i].entind].ivalue,
	//		            dbGetString( master_buf[i].charvalue_index ));
	// Spock fixed 2000/10/19
		  master_buf[hits[i].entind].charvalue );
        strcatsafe( output, outlen, tmp );
        if( i != ( hitsuse -1 ) ){
            strcatsafe( output, outlen, "|" );
        }
    }
    return 0;
}

int
dbFlush( char *dir )
{
    int i;

    for(i=0;i<MAXTABLE;i++){
        FILE *fp;
        char filename[1024];
        int entind;
        //int j;
        if( !dbt[i].use ) continue;
        // Spock 2000/10/23
        if( dbt[i].updated == 0 )
        {
            log( "dbFlush: table %s not updated\n" , dbt[i].name );
            continue;
        }
        // Spock end

        if( dbt[i].type == DB_INT_SORTED ){
            snprintf( filename, sizeof(filename),
                      "%s/int/%s", dir, dbt[i].name );
        } else {
            snprintf( filename, sizeof( filename),
                      "%s/string/%s", dir, dbt[i].name );
        }
        
        fp = fopen( filename, "w" );
        if( fp == NULL ){
            log( "cannot open file: %s %s\n",
                     filename, strerror( errno ));
            continue;
        }

        // Spock 2000/10/19
        //entind = master_buf[dbt[i].toplinkindex].nextind;
        entind = master_buf[dbt[i].toplinkindex].next;
        //for(j=0;;j++){
        //    if( entind == -1 )break;
        while ( entind >= 0 )
        {
        // Spock end
            if( dbt[i].type == DB_INT_SORTED ){
                fprintf( fp , "%s %d %s\n", master_buf[entind].key,
                         master_buf[entind].ivalue,
                         //makeStringFromEscaped(
                         //    dbGetString(master_buf[entind].charvalue_index)));
                         // Spock fixed 2000/10/19
                         makeStringFromEscaped(master_buf[entind].charvalue));
            } else {
                fprintf( fp , "%s %s\n", master_buf[entind].key,
                         //makeStringFromEscaped(
                         //    dbGetString(master_buf[entind].charvalue_index)));
                         // Spock fixed 2000/10/19
                         makeStringFromEscaped(master_buf[entind].charvalue));
            }
            //entind = master_buf[entind].nextind;
            // Spock fixed 2000/10/19
            entind = master_buf[entind].next;
        }
        fclose(fp);
        dbt[i].updated = 0;
    }

    return 0;
}


int dbRead( char *dir )
{
    char dirname[1024];
    DIR *d;
    struct dirent *de;
    // Spock +1 2000/10/19
    memset( dbt , 0 , MAXTABLE * sizeof(struct table) );
    {
        char tmp[1024];
        snprintf( tmp, sizeof( tmp ), "%s/int" , dir );
        if( mkdir( tmp, 0755 )==0){
            log( "斐膘 %s\n", tmp );
        }
        snprintf( tmp, sizeof( tmp ), "%s/string" , dir );
        if( mkdir( tmp, 0755 )==0){
            log( "斐膘 %s\n", tmp );
        }        
    }
        
    snprintf( dirname, sizeof( dirname ),
              "%s/int" , dir );
    d = opendir(dirname);
    if( d == NULL ){
        log( "祥夔湖羲恅璃 %s\n", dirname );
        return -1;
    }

    while(1){
        de = readdir( d );
        if( de == NULL )break;
        if( de->d_name[0] != '.' ){
            char filename[1024];
            FILE *fp;
            struct stat s;
            snprintf( filename, sizeof(filename),"%s/%s",dirname, de->d_name );
			log( "黍�﹋�擂:%s\n..", filename);
            if( stat( filename, &s ) < 0 ){
                continue;
            }
            if( !( s.st_mode & S_IFREG ) ){
                continue;
            }
            
            fp = fopen( filename, "r" );            
            if( fp == NULL ){
                log( "祥夔湖羲恅璃 %s %s\n",
                         filename, strerror( errno ));
                continue;
            }
            while(1){
                char line[1024];
                char k[1024] , v[1024], info[1024];
                if( fgets( line , sizeof( line) , fp ) == NULL )break;
                chop( line);
				k[0] = '\0';
                easyGetTokenFromString( line, 1, k, sizeof(k));
				v[0] = '\0';
                easyGetTokenFromString( line, 2, v, sizeof(v));
                info[0] = '\0';
                easyGetTokenFromString( line, 3, info, sizeof(info));
                dbUpdateEntryInt( de->d_name, k, atoi(v), info);
            }
            fclose(fp);
        }
    }
    closedir(d);
    snprintf( dirname, sizeof( dirname), "%s/string" , dir );
    d = opendir( dirname );
    if( d == NULL ){
        log( "祥夔湖羲恅璃 %s\n", dirname );
        return -1;
    }
    while(1){
        de = readdir( d );
        if( de == NULL )break;
        if( de->d_name[0] != '.' ){
            char filename[1024];
            FILE *fp;
            struct stat s;
            snprintf( filename, sizeof( filename),"%s/%s",dirname,de->d_name );
			log( "黍�﹋�擂:%s\n..", filename);

            if( stat( filename, &s ) < 0 ){
                continue;
            }
            if( !(s.st_mode & S_IFREG )){
                continue;
            }
            fp = fopen( filename, "r" );
            if( fp == NULL ){
                log( "祥夔湖羲恅璃 %s %s\n",
                     filename, strerror(errno ));
                continue;
            }
            while(1){
                char line[CHARVALUE_MAX+1024];     
                char k[1024];
                if( fgets( line, sizeof( line), fp ) == NULL )break;
                /* chop */
                chop(line);
				k[0] = '\0';
                easyGetTokenFromString( line, 1, k,sizeof(k));
                dbUpdateEntryString( de->d_name, k, line+strlen(k)+1);
            }
            // Nuke +1 1027: Close for safe
            fclose(fp); 
        }
    }
    closedir(d);
    return 0;
}

/* 炩擭媃倜痾  凝�梤黃贈ぬ酖覕栵�堇煦埰��
 謄  媃倜��  ��埬  媃倜��0�觚�  媃趙忒昶摯③  摯敁拻誑堣堎�舝�
   ※num誑0摯敁拻敁凝��器騍埰堎操璃哱惉誑埵笢敁拻��

 int 溢↓淏穫↓筒障暵煦��

 */
int dbGetEntryCountRange( char *table, int count_start, int  num,
                      char *output, int outlen )
{
    int dbi = dbGetTableIndex( table , DB_INT_SORTED );
    int cur;
    int i;
    int now_score = 0x7fffffff , r;

    if( dbi < 0) return -1;
    if( outlen < 1 ) return -1;
    output[0]=0;

    //cur = master_buf[dbt[dbi].toplinkindex].nextind;
    // Spock fixed 2000/10/19
    cur = master_buf[dbt[dbi].toplinkindex].next;
    i=0;
    r=0;
    for(;;){
        if( cur == -1 ) break;

        if( master_buf[cur].ivalue != now_score ){
            r=i;
            now_score = master_buf[cur].ivalue;
        }

        if( ( i >= count_start ) &&
            ( i < (count_start + num ) ) ){
            char tmp[1024];            
            if( (i !=count_start)){
                strcatsafe( output, outlen, "|" );
            } 
              
            snprintf( tmp, sizeof( tmp),
                      "%d,%d,%s,%d,%s", i, r, master_buf[cur].key,
                      master_buf[cur].ivalue,
			            //dbGetString( master_buf[cur].charvalue_index ));
		      // Spock fixed 2000/10/19
		      master_buf[cur].charvalue);
            strcatsafe( output, outlen,tmp );
        }
        i++;
        //cur = master_buf[cur].nextind;
        // Spock fixed 2000/10/19
        cur = master_buf[cur].next;
    }
    return 0;
}


/*
    棬  溢↓淏穫↓筒摯窐  
 */
/* Spock deleted 2000/10/19
int
dbUpdateEntryString( char *table, char *key, char *value )
{
    int dbi = dbGetTableIndex(table, DB_STRING);
    int r, entind;
    
    log( "dbUpdateEntryString: [%s] [%s] [%s]\n", table, key, value );
    
    if( strlen(key) >= sizeof(master_buf[0].key) )return -1;
    if( dbi < 0 )return -1;

    r = dbExtractNodeByKey( dbt[dbi].toplinkindex, key );
    if( r< 0 ){
        log( "dbUpdateEntryString dbExtractNodeByKey fail! bug!!\n" );
        return -1;
    }

    entind = dbAllocNode( DB_STRING );
    if( entind < 0 ) return -1;

    master_buf[entind].ivalue = 0;
    dbSetString( master_buf[entind].charvalue_index, value );
    snprintf( master_buf[entind].key,
              sizeof(master_buf[0].key), "%s",key );
    master_buf[entind].keyhash = hashpjw( master_buf[entind].key );
    master_buf[entind].nextind = -1;

    if(  dbAppendNode( dbt[dbi].toplinkindex, entind ) < 0 ){
        log( "dbUpdateEntryString: dbAppendNode failed\n" );
        return -1;
    }
    log( "dbUpdateEntryString: successfully updated entry %s:%s:%s\n",
         table,key,value );

    return 0;
}
*/
// Spock 2000/10/19
int dbUpdateEntryString( char *table, char *key, char *value )
{
    int dbi = dbGetTableIndex( table , DB_STRING );
    int dbind, hashind;

    if ( strlen( key ) >= KEY_MAX ) {
    	log( "dbUpdateEntryString: key is too long, key:%s\n", key );
    	return -1;
    }
    if ( strlen( value ) >= CHARVALUE_MAX ) {
    	log( "dbUpdateEntryString: charvalue is too long, charvalue:%s\n", value );
    	return -1;
    }
    if ( dbi < 0 ) {
    	log( "dbUpdateEntryString: dbGetTableIndex fail, table:%s\n", table );
    	return -1;
    }
    hashind = tableGetEntry( dbi , key );
    if ( hashind < 0 )
    {
    	dbind = dbAllocNode();
    	if ( dbind < 0 )
    	{
    	    log( "dbUpdateEntryString: dbAllocNode fail\n" );
    	    return -1;
    	}
    	strcpy( master_buf[dbind].key , key );
    	strcpy( master_buf[dbind].charvalue , value );
    	if ( dbAppendNode( dbt[dbi].toplinkindex , dbind ) < 0 )
    	{
    	    master_buf[dbind].use = 0;
    	    log( "dbUpdateEntryString: dbAppendNode fail\n" );
    	    return -1;
    	}
    	if ( tableInsertNode( dbi , key , dbind ) < 0 )
    	{
    	    dbReleaseNode( dbind );
	    log( "dbUpdateEntryString: tableInsertNode fail\n" );
	    return -1;
	}
    }
    else
    {
    	dbind = dbt[dbi].hashtable[hashind].dbind;
    	strcpy( master_buf[dbind].charvalue , value );
    }
    dbt[dbi].updated = 1;
	/*
    log( "dbUpdateEntryString: successfully updated entry %s:%s:%s\n",
         table,key,value );
		 */
    return 0;
}
// Spock end

int
dbGetEntryString( char *table, char *key, char *output, int outlen )
{
    int dbi = dbGetTableIndex( table, DB_STRING );
    int entind;
    // Spock +1 2000/10/19
    int hashind;

    // Spock 2000/10/23
    //if( strlen(key) >= sizeof( master_buf[entind].key) ) return -1;
    if ( strlen(key) >= KEY_MAX ) {
    	log( "dbGetEntryString: key is too long, key:%s\n", key );
    	return -1;
    }
    if( dbi <0 ) {
    	log( "dbGetEntryString: dbGetTableIndex fail\n" );
    	return -1;
    }
    // Spock 2000/10/19
    hashind = tableGetEntry( dbi , key );
    if ( hashind < 0 ){
		log("err hashind <0\n")
		return -1;
	}
    entind = dbt[dbi].hashtable[hashind].dbind;

    if ( entind < 0 ){
		log( "entind < 0 ");
		return -1;
	}
    snprintf( output , outlen , "%s" , master_buf[entind].charvalue );

    return 0;
}

int
dbDeleteEntryString( char *table, char *key )
{
    int dbi = dbGetTableIndex( table, DB_STRING );
    int r;

    // Spock 2000/10/23
    //if( strlen(key) >= sizeof( master_buf[entind].key) ) return -1;
    if ( strlen(key) >= KEY_MAX ) {
    	log( "dbDeleteEntryString: key is too long, key:%s\n", key );
    	return -1;
    }
    if( dbi <0 ) {
    	log( "dbDeleteEntryString: dbGetTableIndex fail\n" );
    	return -1;
    }
    // Spock end
    //r = dbExtractNodeByKey( dbt[dbi].toplinkindex, key );
    // Spock fixed 2000/10/19
    r = dbExtractNodeByKey( dbi , key );
    if( r < 0 ){
        log( "dbDeleteEntryString: dbExtractNodeByKey failed for %s in %s\n",
             key,table );
        return -1;
    }
    dbt[dbi].updated = 1;
    log( "deleted key %s from table %s\n", key, table );
    return 0;
}
