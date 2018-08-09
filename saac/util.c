#define _UTIL_C_

#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "main.h"
#include "util.h"

#define IS_2BYTEWORD( _a_ ) ( (char)(0x80) <= (_a_) && (_a_) <= (char)(0xFF) )
#define PRIME 211
int hashpjw ( char* s )
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
    return h % PRIME;
}

int getHash ( char* s )
{
    int i;
    int h=0;

    for(i=0;; i++){
        if( s[i] == 0 ){
            break;
        }
        h += s[i];
    }
    return h;
}

void easyGetTokenFromString( char *src,int count,char*output,int len )
{
    int i;
    int counter = 0;

    if( len <= 0 )return;

#define ISSPACETAB( c )   ( (c) == ' ' || (c) == '\t' )

    for(i=0;;i++){
        if( src[i]=='\0'){
            output[0] = '\0';
            return;
        }
        if( i > 0 && ! ISSPACETAB( src[i-1] ) &&
            ! ISSPACETAB(  src[i] ) ){
            continue;
        }

        if( ! ISSPACETAB( src[i]) ){
            counter++;
            if( counter == count){
                int j;
                for(j=0;j<len-1;j++){
                    if( src[i+j] == '\0' ||
                        ISSPACETAB( src[i+j] ) ){
                        break;
                    }
                    output[j]=src[i+j];
                }
                output[j]='\0';
                return;

            }
        }

    }
}

void prepareDirectories(char *base)
{
    int i;
    char dname[1024];

    for(i=0;i<256;i++){
        int ret;
        snprintf( dname , sizeof( dname ) , "%s/0x%x", base , i );
        ret = mkdir( dname , 0755 );
        if( ret <0 && errno != EEXIST ){
            log( "mkdir error:%d %s: %s\n", ret ,strerror(errno),
                 dname );
        }
        if( ret == 0 ) log(".");
    }
}

void remove_r( char *s )
{
    int i;
    for(i=0;;i++){
        if(s[i]=='\r')s[i] =0;
        if( s[i]==0)break;
    }
}

void makeDirFilename( char *out , int outlen ,
                  char *base , int dirchar , char *child )
{
    snprintf( out , outlen ,
              "%s/0x%x/%s" , base ,
              dirchar & 0xff , child );
}

int isFile( char *fn )
{
    FILE*fp=fopen(fn,"r");
    if( fp){
        fclose(fp);
        return 1;
    }
    return 0;
}

int createFile( char *fn , char *line )
{
    FILE *fp = fopen( fn , "w" );
    if( fp== NULL ){
        return -1;
    } else {
        fprintf( fp , "%s" , line );
        fclose(fp);
        return 0;
    }
}

#define BOOL int
#define FALSE 0
#define TRUE  1

typedef struct tagEscapeChar
{
    char     escapechar;
    char     escapedchar;
}EscapeChar;
static EscapeChar   escapeChar[]=
{
    { '\n',   'n' },
    { ',',    'c' },
    { '|',    'z' },
    { '\\',   'y' },
};

char*   makeStringFromEscaped( char* src )
{
    int     i;
    int     srclen = strlen( src );
    int     searchindex=0;

    for( i = 0 ; i < srclen ; i ++ ){
    	// for 2Byte Word
    	if( IS_2BYTEWORD( src[i] ) ){
            src[searchindex++] = src[i++];
            src[searchindex++] = src[i];
    	}else
        if( src[i] == '\\' ){
            int j;

            /*  ÆÝ¼°  Ù¯±åµæÈÊ  */
            i++;
            for( j = 0; j<sizeof(escapeChar)/sizeof(escapeChar[0]); j++){
                if( escapeChar[j].escapedchar == src[i] ){
                    /*  ÆÝ»¥¾Þµ©¸¥¡õÃóÆ½ÅÒ·Â·Ö  */
                    src[searchindex++] = escapeChar[j].escapechar;
                    goto NEXT;
                }
            }
            /*  ¾Þ·Â¡õÖ§ØêÉý¹«¼°ÒýÒýÎìñâ¡õØÆ»¯ÔÆÈÊ  */
            src[searchindex++] = src[i];
        }else{
            src[searchindex++] = src[i];
        }
    NEXT:;
    }
    src[searchindex] = '\0';
    return src;
}

char *   makeEscapeString( char* src , char* dest, int sizeofdest)
{
    int         i;
    int     srclen = strlen( src );
    int     destindex=0;

    for( i = 0 ; i < srclen ; i ++ ){
        BOOL dirty=FALSE;
        int     j;
        char    escapechar='\0';
        if( destindex + 1 >= sizeofdest ) break;
		if( IS_2BYTEWORD( src[i] ) ){
	        if( destindex + 2 >= sizeofdest ) break;
            dest[destindex] = src[i];
            dest[destindex+1] = src[i+1];
   	        destindex += 2;
   	        i ++;
   	        continue;
		}

        for( j = 0; j<sizeof(escapeChar)/sizeof(escapeChar[0]); j++){
            if( src[i] == escapeChar[j].escapechar ){
                dirty=TRUE;
                escapechar= escapeChar[j].escapedchar;
                break;
            }
		}
		if( dirty == TRUE ){
			if( destindex + 2 < sizeofdest ){
				dest[destindex] = '\\';
				dest[destindex+1] = escapechar;
				destindex+=2;
				dirty=TRUE;
				continue;       /*  ÆÝ¼°  Ù¯±åñ×¸ê  */
			}else{
				dest[destindex] = '\0';
				return dest;
			}
		}else{
			dest[destindex] = src[i];
			destindex++;
		}
	}

    dest[destindex] = '\0';
    return dest;
}

char *   makeEscapeString1( char* src , char* dest, int sizeofdest)
{
    int         i;
    int     srclen = strlen( src );
    int     destindex=0;

    for( i = 0 ; i < srclen ; i ++ ){
        BOOL dirty=FALSE;
        int     j;
        char    escapechar='\0';
        if( destindex + 1 >= sizeofdest )
            /*  '\0'ÛÐ»¥óïÔ»Ø¦ÖÐ¼°Æ¥³ð³ðÆ¥±ÎÔ»   */
            break;

	/*	// òåÊ£¼°¨àÌïÄÌÐþ  ¾®Éýµ¤¾®Ã«ÃñÄáÓÀÛÍ
		if( IS_2BYTEWORD( src[i] ) ){
			// òåÊ£·Ö£Û¹«¼°èëÄþ·´¨àÌïÄÌÐþÜÌÛÐ±åñ×ÒýÁùÔÂ£Û
			// Ð×·ÖØÆ¨àÌïÄÌÐþØÆ¾®Ø¦ÖÐèëÄþ·´¹«µ¤ØÆØ¦ÖÐ
			// ¹«¼°ÒýÒý¨áÌïÄÌÐþ±îËª
	        if( destindex + 2 >= sizeofdest )break;

            dest[destindex] = src[i];
            dest[destindex+1] = src[i+1];
   	        destindex += 2;
   	        i ++;
   	        continue;
		}
	*/
        for( j = 0; j<sizeof(escapeChar)/sizeof(escapeChar[0]); j++)
            if( src[i] == escapeChar[j].escapechar ){
                dirty=TRUE;
                escapechar= escapeChar[j].escapedchar;
                break;
            }

        if( dirty == TRUE ){
            /*  ¾Þµ©¸¥¡õÃóÔÊÔÂ  Ù¯·Ö    */
            if( destindex + 2 < sizeofdest ){
                /*  +2 ÎçÖÐµ¤¼°·´£ý '\\' Îç 'n'¼°ÒÇ·Ö   */
                /*  ËáÔ»  Ù¯·´ÎÏÛÐ·Ö    */
                dest[destindex] = '\\';
                dest[destindex+1] = escapechar;
                destindex+=2;
                dirty=TRUE;
                continue;       /*  ÆÝ¼°  Ù¯±åñ×¸ê  */
            }else{
                /*  ËáÔ»ÌïÓÀ°×Ñë»¥æØóïØÆ»¯ÖÐÔÂ  */
                dest[destindex] = '\0';
                return dest;
            }
        }else{
            dest[destindex] = src[i];
            destindex++;
        }

    }
    dest[destindex] = '\0';
    return dest;
}
#undef BOOL
#undef FALSE
#undef TRUE



/* ÔÞÖÐstrcatsafe£ÛÌïÓÀ°×Ñë»¥Ø¤¿×Ä¾ÔÂèëÄþ·´ÖÏÊÖØÆØ¦ÖÐ£Û */
int
strcatsafe( char *dest, int destlen , char *append )
{
    int dl = strlen( dest);
    int al = strlen(append);

    if( (dl + al) >= destlen ){
        return -1;
    } else {
        strcat( dest, append );
    }
	return 0;
}

char *chop( char *s )
{
    int l = strlen(s);
    if( l >= 1 ){
        s[l-1]=0;
    }
    return s;
}

// CoolFish: Family 2001/5/30
void easyGetTokenFromBuf(char *src, char delim, int count, char *output, int len)
{
    int  i;
    int  counter = 0;
    int  wordflag = 0;
    
    if( len <= 0 ) return;
    
    #define ISSEPARATE( c )   ((c) == delim)
    
    for( i=0; ; i++){
    	if( src[i] == '\0' ){
    		output[0] = '\0';
    		return;
    	}
    	if(IS_2BYTEWORD(src[i])){
    		if( i>0 && wordflag == 1 ){
    			i++;
    			wordflag = 1;
    			continue;
    		}
    		if( i>0 && !ISSEPARATE(src[i-1]) && !ISSEPARATE(src[i])){
    			i++;
    			wordflag = 1;
    			continue;
    		}						
    		wordflag = 1;
    	}else{
    		if( i>0 && wordflag ){
    			wordflag = 0;
    			continue;
    		}
    		if( i>0 && !ISSEPARATE(src[i-1]) && !ISSEPARATE(src[i])){
    			wordflag = 0; 
    			continue;
    		}
    		wordflag = 0;
    	}
        if( !ISSEPARATE(src[i]) ){
        	counter++;
        	if( counter == count ){
        		int j;
        		for ( j=0; j<len-1; j++){
        			if( IS_2BYTEWORD( src[i+j] ) ){
		        		output[j] = src[i+(j++)];
		        		output[j] = src[i+j];
		        	}else{
		        		if( src[i+j]=='\0' || ISSEPARATE(src[i+j])){
		        			break;
		        		}
		        		output[j] = src[i+j];
		        	}
		        }
		        output[j] = '\0';
		        return;
		}
		if( wordflag )
			i++;
		if ( i==0)
		{
			if( IS_2BYTEWORD(src[i]) ){
				wordflag = 1;
				i++;
			}
			else
			{
				wordflag = 0;
			}
		}
	}
    }
}
