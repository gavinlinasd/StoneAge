#include "version.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "util.h"
#include "configfile.h"

#define IS_2BYTEWORD( _a_ ) ( (char)(0x80) <= (_a_) && (_a_) <= (char)(0xFF) )

/*-----------------------------------------
  崹僑摯搊菜禱喫Й埰
  礎倳※ 忒敁摯  棬  ��  旽  脾踏躂堎��

  windows , macintosh , unix摯汔摯菜忒葡伀ぁ拻堎��

  windows : \r\n
  macintosh : \n\r
  unix : \n

  煦埱倜凝埵
  macintosh : \r
  ぁ埰��
  鼠躂敁喫摯鏤痲精溘堜煦敁崹僑摯  棬啋崨埵�妡缺�
  搊菜觛埰崹汔笢笢庌ぁ埰凝
  -----------------------------------------*/
void chompex( char *buf )
{
    while( *buf ){
        if( *buf == '\r' || *buf == '\n' ){
            *buf='\0';
        }
        buf++;
    }
}



/*----------------------------------------
 * 崹僑摯1  棬禱噁堇謫��
 * 礎倳
 *  src           脾埰堎  棬  
 ----------------------------------------*/
void chop( char* src )
{
    /*
     * 倜煦倜庌勗    摯  棬禱噁堇謫��
     */
    int length = strlen( src );
    if( length == 0 ) return;
    src[length-1] = '\0';
}


/*----------------------------------------
 * 崹僑摯  棬禱噁堎�蛫姨�  棬禱  倳炩擭ぁ拻堎��
 * 礎倳
 *  src     楁摯  棬  
 *  delim   朝謫埰堎  棬摯    ��    毀NULL  棬ぁ敖埱趙笢堎痀
 ----------------------------------------*/
void dchop( char* src , char* del)
{
    int dellen, srclen;
    int i;
    BOOL delete=FALSE;

    /*
     * src摯  Й凝�梪怛芼胱倞呁�1  棬勗僑拻 del 埰迋趙禱鳴↓鏤媃趙
     * 郖窙埰躂�幵梊呁蛫建し紜敯�  棬勗笢�呁�
     */

    srclen = strlen( src );
    dellen = strlen( del );
    if( srclen == 0 || dellen == 0 ) return;

    for( i = 0 ; i < dellen ; i ++ ){
        if( src[srclen-1] == del[i] ){
            delete = TRUE;
            break;
        }
    }

    /*  憍堇噁堇    */
    if( delete )src[srclen - 1] = '\0';
}


/*----------------------------------------
 * dchop摯楪��  疐摯  棬禱噁堎�蛫姨�  棬禱  倳炩擭ぁ拻堎��
 * 礎倳
 *  src     楁摯  棬  
 *  delim   朝謫埰堎  棬摯    ��    毀NULL  棬ぁ敖埱趙笢堎痀
 ----------------------------------------*/
void pohcd( char* src , char* del)
{
    int dellen, srclen;
    int i;
    BOOL delete=FALSE;

    /*
     * src摯鞳凝�梪怛芼胱倞呁�1  棬勗僑拻 del 埰迋趙禱鳴↓鏤媃趙
     * 郖窙埰躂�幵梊呁蛫建し紜敯�  棬勗笢�呁�
     */

    srclen = strlen( src );
    dellen = strlen( del );
    if( srclen == 0 || dellen == 0 ) return;

    for( i = 0 ; i < dellen ; i ++ ){
        if( src[0] == del[i] ){
            delete = TRUE;
            break;
        }
    }

    /*  朝謫    */
    if( delete )
        /*  喫摯昡鼀↓ぁ NULL  棬忒昡鼀↓踏躂堎 */
        for( i = 0 ; i < srclen ; i ++ )
            src[i] = src[i+1];
}


/*----------------------------------------
 *   棬  勗  棬誑硈竘躂趙笢堎凝汔竣凝ぅ迋堎��
 * 礎倳
 *  src     char*   ぅ迋堎  棬  
 *  include char*   硈竘躂趙笢堎凝汔竣凝摯  棬  ��
 * 蒍堇偯
 *  埬        疐勗硈竘躂趙笢倜src摯鳹鐎
 *  謄      -1
 ----------------------------------------*/
int charInclude( char* src, char* include )
{
    int     i,j;
    int     srclen,includelen;

    srclen = strlen( src );
    includelen = strlen( include );

    for( i = 0 ; i < srclen ; i ++ ){
        for( j = 0 ; j < includelen ; j ++ )
            if( src[i]  == include[j] )
                return i;
    }
    return -1;
}


/*------------------------------------------------------------
 * EUC摯2泬騷哱  煦崹禱昡鼀↓媃埵笢strncpy
 * freeBSD 摯溘騷々溘惉凝�� strncpy 禱噁埱趙拻趙��
 * 鼠躂禱忒敁勗搊桹��*dest++ 敁凝窅笢倜笢崹汔��踕譴棹謙勗猷�儢�
 * 埵笢摯ぁ��窅埱趙笢埵笢�蛨捎灄跼�    煦崹汔��睽騷璃淏忒窅埱趙趙
 * 凝埵堇  埵鏤痲精溘堜
 * 礎倳
 *  dest        char*   昡鼀↓袸
 *  src         char*   藺↓筒
 *  n           size_t  荇踏
 * 蒍堇偯
 *  dest
 ------------------------------------------------------------*/
char* strncpy2( char* dest, const char* src, size_t n )
{
    if( n > 0 ){
        char*   d = dest;
        const char*   s = src;
        int i;
        for( i=0; i<n ; i++ ){
            if( *(s+i) == 0 ){
                /*  昡鼀↓媃堁渦埱倜�� NULL   棬禱  躂堎   */
                *(d+i) = '\0';
                return dest;
            }
            if( *(s+i) & 0x80 ){
                *(d+i)  = *(s+i);
                i++;
                if( i>=n ){
                    *(d+i-1)='\0';
                    break;
                }
                *(d+i)  = *(s+i);
            }else
                *(d+i) = *(s+i);
        }
    }
    return dest;
}

/*----------------------------------------
 * src 凝�� dest 勗昡鼀↓埰堎�� 昡鼀↓袸摯�蚅昃劗宦怛邴瞿�
 * 礎倳
 *      dest        昡鼀↓袸
 *      n           �蚅昃�
 *      src         楁摯  棬  
 ----------------------------------------*/
void strcpysafe( char* dest ,size_t n ,const char* src )
{
    /*
     * src 凝�� dest 喜昡鼀↓埰堎.
     * strcpy, strncpy ぁ毀 dest 源堇 昡鼀↓埰堎倵誑
     *   拻笢鄹勗蠙屾敁,隍さ惉翻囮袲掛筒誑厔堎.
     * 喫躂禱  妦諱勗, strncpy 誑堣堎誑 strlen( src ) 誑 n 源堇
     *   拻笢鄹勗毀, dest 摯    誑 NULL   棬敁毀埵�欷聿�.
     *
     * 媃倜誑埱趙 dest 摯  拻踏源堇 src 摯酵竣誑荇笢鄹勗毀
     * n-1 ぁ strncpy 禱埰堎. 覽踏笢鄹毀鼠摯竘竘昡鼀↓埰堎
     *
     * n 誑  摯鄹毀堁凝媃�忐耒翹副�  摯鄹毀 笰忒媃埵笢��
     *
     */
    // Nuke +1 (08/25): Danger if src=0
    if (!src) {
        *dest = '\0';
        return;
    }
    if( n <= 0 )        /* 笰忒媃埵笢   */
        return;

    /*  喫摯鄹鬻ぁ�� n >= 1 雄窀誑鋆擭  */
    /*  NULL  棬禱萳  媃趙  吨埰堎  */
    else if( n < strlen( src ) + 1 ){
        /*
         * 泬蚗啞栝誑鵿堇埵笢摯ぁ n - 1(NULL  棬)
         * ぁ strncpy 禱蠙屾
         */
        strncpy2( dest , src , n-1 );
        dest[n-1]='\0';
    }else
        strcpy( dest , src );

}

/*----------------------------------------
 * src 凝�� length 昅埰��   昡鼀↓袸摯  拻踏禱ぅ迋堎��
 * 礎倳
 *          dest        昡鼀↓袸
 *          n           昡鼀↓袸摯�蚅昃�
 *          src         昡鼀↓楁
 *          length      笰  棬昡鼀↓埰堎凝
 ----------------------------------------*/
void strncpysafe( char* dest , const size_t n ,
                  const char* src ,const int length )
{
    /*
     * src 凝�� dest 勗 length 昡鼀↓埰堎
     * strcpy, strncpy ぁ毀 dest 源堇 昡鼀↓埰堎倵誑
     *   拻笢鄹勗蠙屾敁,隍さ惉翻囮袲掛筒誑厔堎.
     * 喫摯匴倳ぁ毀��strlen( src ) 敁 length 摯覽踏笢酵竣
     * (  傷勗昡鼀↓埰堎倵) 敁 dest 摯�蚅昃劗�  迋趙��
     * strcpysafe 敁  啋痀禱埰堎��
     */

    int Short;
    Short = min( strlen( src ) , length );

    /* NULL  棬 禱萳  媃倜  吨 */
    if( n < Short + 1 ){
        /*
         * 泬蚗啞栝誑鵿堇埵笢摯ぁ n - 1(NULL  棬)
         * ぁ strncpy 禱蠙屾
         */
        strncpy2( dest , src , n-1 );
        dest[n-1]='\0';

    }else if( n <= 0 ){
        return;
    }else{
        /*
         * 泬蚗啞栝毀恘覕勗堣堎摯ぁ Short ぁstrncpy禱蠙屾
         * 埵堁 src 勗毀 Short 摯荇踏  勗 NULL 誑埵笢摯ぁ��
         * dest 勗毀 黹萃媃趙堁�呁�
         */

        strncpy2( dest , src , Short );
        dest[Short]= '\0';

    }
}

/*------------------------------------------------------------
 * strcat禱項蠈勗  藯埰堎�觚皵邾副奴�    埵鳹譴禱謫笢趙窅渦埵笢痀��
 * 礎倳
 *  src     char*       楁勗埵堎  棬  
 *  size    int         src 摯�蚅昃�(   Й勗汔躂煦崹呫埱趙笢堎凝ぁ毀
 *                          埵�妡羔擽幫豽錓�   )
 *  ap      char*       痹崹萃窇堎  
 * 蒍堇偯
 *  src     (  躂碳暵)
 ------------------------------------------------------------*/
char* strcatsafe( char* src, int size, char* ap )
{
    strcpysafe( src + strlen( src ) , size - strlen(src), ap );
    return src;
}

/*----------------------------------------
 *   棬  摯    禱  吨埰堎��  啋凝汔竣凝媃凝  擭媃埵笢 by ringo
 * char *s1 , *s2 :   吨埰堎  棬  
 * 蒍堇偯
 * 0 ※ 堁埵啋
 * 1 : 喫敁埵堎
 * 喫躂毀NPC摯  す筒哱搣  勗盓�妢砦提�
 ----------------------------------------*/
int strcmptail( char *s1 , char *s2 )
{
    int i;
    int len1 = strlen( s1 );
    int len2 = strlen( s2 );

    for(i=0;;i++){
        int ind1 = len1 - 1 - i;
        int ind2 = len2 - 1 - i;
        if( ind1 < 0 || ind2 < 0 ) return 0;
        if( s1[ind1] != s2[ind2] ) return 1;
    }
}



/*----------------------------------------
 *   棬  摯    勗怳凝躂趙笢堎堆倳禱斛蚗哱埰堎��
 * NPC摯  す筒哱搣  勗盓�妢砦提�
 * 倜煦媃    勗笢�梛�  棬誑堣堎鳹譴  鷋埰堎��
 * 嚭�牴�  з埵笢囮鳴湖惉扲堜勗  脾��
 * 礎倳
 *  hoge    char*   搣  埰堎  棬  
 * 蒍堇偯
 *  搣  媃倜堆倳��
 *
 *   ※
 * "ringo ( 勘靷勘靷陎�� ) ㄩ 100"
 *      摯鳹譴毀100誑凝窇堎��
 * "ringo ( 勘靷勘靷陎�� ) ㄩ 岆100"
 *      忒100勗埵堎�蛬蟾玾�勗NPC暵埵堇衭��
 ----------------------------------------*/
#define DECIMALCHARNUM 10
#define FIGURECHARNUM 3
#define SPECIALCHARNUM 2
char *UTIL_decimalchar[DECIMALCHARNUM] = {"0","1","2",
                                     "3",  "4","5",
                                     "6","7","8","9"};
char *UTIL_figurechar[FIGURECHARNUM] = {"","",""};
char *UTIL_specialchar[SPECIALCHARNUM] = {"",""};

/*
 *   棬  凝�掁�倳偯禱  埰敁  渦躂堎  覕禱  拻③埰�詗�  摯酵竣凝��
 * 潺媃趙  拻③埰�菼uc暵��
 *
 *    : in "涴掛抎ほ勀聊300藝踢鎘ˋ"
 *      out "7勀"
 * 礎倳
 * char *in:      棬  
 * char *out:③    棬  
 * int outsiz:③    棬  摯泬蚗啞栝�蚅昃�
 * 蒍堇偯
 * 倳偯禱  埰  覕誑堣埱倜�桯姨�  棬  摯荇踏��埵凝埱倜�旍�
 */
static int findNumberString( char *in, char *out, int outsiz )
{
    int len,i,j,k, findflag, numstrflag;
    char tmp[3];

    len = strlen(in);
    if( len == 0 ) return 0;

    numstrflag=0;
    findflag = 0;
    for( i=0,j=0;in[i]!='\0'&& j<outsiz-2;i++ ){
        findflag = 0;
        if( in[i] & 0x80 ){
            /* EUC摯鳹譴 */
            tmp[0] = in[i];
            tmp[1] = in[i+1];
            tmp[2] = '\0';
        }else{
            /* ASCII摯鳹譴 */
            tmp[0] = in[i];
            tmp[1] = '\0';
        }
        for( k=0;k<DECIMALCHARNUM;k++ ){
            if( strstr( UTIL_decimalchar[k], tmp ) ){
                numstrflag = 1;
                findflag = 1;
            }
        }
        for( k=0;k<FIGURECHARNUM;k++ ){
            if( strstr( UTIL_figurechar[k], tmp ) ){
                numstrflag = 1;
                findflag = 1;
            }
        }
        for( k=0;k<SPECIALCHARNUM;k++ ){
            if( strstr( UTIL_specialchar[k], tmp ) ){
                numstrflag = 1;
                findflag = 1;
            }
        }
        if( findflag ){
            if( in[i] & 0x80 ){
                out[j] = tmp[0];
                out[j+1] = tmp[1];
                j+=2;
                i++;
            }else{
                out[j] = tmp[0];
                j+=1;
            }
            continue;
        }

        if( numstrflag ){
            out[j] = '\0';
            return j;
        }

    }

    if( findflag ){
        out[j] = '\0';
        return j;
    }else{
        return 0;
    }

}


/*
 *   棬  禱int勗  儒��
 *
 */
int numstrIsKanji( char *str );
int decimalstrToInt( char *str );
int kanjistrToInt( char *str );

int textToInt( char *str )
{
    char numstr[256];
    if( !findNumberString(str,numstr,sizeof(numstr)) ) return 0;

    if( numstrIsKanji(numstr) ) return kanjistrToInt(numstr);
    else return decimalstrToInt(numstr);

}

/*
 * 倳偯禱  埰  棬  誑��    鞅祪埵摯凝詎鎃祪埵摯凝禱    
 * 礎倳     埰堎  棬  �覕栟紐�  埰  棬  雄翻毀硈庌ぁ毀笢崹埵笢��
 * 蒍堇偯     鞅祪埵�旍逽�詎鎃祪埵�旍�
 *    str 誑"珨砬侐ロ勀﹝"摯敁拻��蒍堇偯1
 *    str 誑"123"摯敁拻��蒍堇偯兔
 *
 */
int numstrIsKanji( char *str )
{
    int i,j;
    char tmp[3];

    for( i=0;str[i]!='\0';i++ ){
        if( str[i] & 0x80 ){
            tmp[0] = str[i];
            tmp[1] = str[i+1];
            tmp[2] = '\0';
            i++;
        }else{
            tmp[0] = str[i];
            tmp[1] = '\0';
        }
        for( j=0;j<FIGURECHARNUM;j++ ){
            if( strstr(UTIL_figurechar[j],tmp) ){
                return 1;
            }
        }
        for( j=0;j<SPECIALCHARNUM;j++ ){
            if( strstr(UTIL_specialchar[j],tmp) ){
                return 1;
            }
        }
    }
    return 0;

}

/*
 * 恘鵌傮ぁ倳偯禱  媃倜  棬  禱int勗  儒
 * char *str   棬  
 * 蒍堇偯   棬  誑  埰倳偯
 */
int decimalstrToInt( char *str )
{

    double val;
    char tmp[3];
    int i,j;

    val = 0;
    for( i=0;str[i]!='\0';i++ ){
        if( str[i] & 0x80 ){
            tmp[0] = str[i];
            tmp[1] = str[i+1];
            tmp[2] = '\0';
            i++;
        }else{
            tmp[0] = str[i];
            tmp[1] = '\0';
        }
        for( j=0;j<DECIMALCHARNUM;j++ ){
            if( strstr(UTIL_decimalchar[j],tmp) ){
                val = val*10+j;
            }
        }
    }

    if( val > 0x7fffffff ) return 0x7fffffff;
    else     return (int)val;

}


/*
 *     祪ぁ  媃倜倳偯禱堆倳勗  儒
 * char *str 倳偯禱  埰  棬  
 * 蒍堇偯 倳偯
 *   
 * str誑"珨砬4ロ勀ˋ"摯敁拻蒍堇偯 140000000
 */
int kanjistrToInt( char *str )
{
    double val,tmpval;
    char tmp[3];
    int num,i,j,numflag;

    numflag = 0;
    num = 1;
    tmpval = 0;
    val = 0;
    for( i=0;str[i]!='\0';i++ ){
        if( str[i] & 0x80 ){
            tmp[0] = str[i];
            tmp[1] = str[i+1];
            tmp[2] = '\0';
            i++;
        }else{
            tmp[0] = str[i];
            tmp[1] = '\0';
        }
        for( j=0;j<SPECIALCHARNUM;j++ ){
            if( strstr(UTIL_specialchar[j],tmp ) ){
                if( numflag == 1 ) tmpval += num;
                if( j == 0 ) val += tmpval*10000;
                else if( j == 1 ) val += tmpval*100000000;
                num = 1;
                numflag = 0;
                tmpval = 0;
                goto nextchar;
            }
        }
        for( j=0;j<FIGURECHARNUM;j++ ){
            if( strstr(UTIL_figurechar[j],tmp) ){
                if( j == 0 ) tmpval += num*10;
                else if( j == 1 ) tmpval += num*100;
                else if( j == 2 ) tmpval += num*1000;
                num = 1;
                numflag = 0;
                goto nextchar;
            }
        }
        for( j=0;j<DECIMALCHARNUM;j++ ){
            if( strstr(UTIL_decimalchar[j],tmp) ){
                num = j;
                numflag = 1;
                goto nextchar;
            }
        }
nextchar:
		continue;
    }
    if( numflag == 1 ) tmpval += num;
    val += tmpval;

    if( val > 0x7fffffff ) return 0x7fffffff;
    else     return (int)val;

}



/*----------------------------------------
 *   棬  摯    勗怳凝躂趙笢堎堆倳禱斛蚗哱埰堎��
 * NPC摯  す筒哱搣  勗盓�妢砦提�
 * 倜煦媃    勗笢�梛�  棬誑堣堎鳹譴  鷋埰堎��
 * 嚭�牴�  з埵笢囮鳴湖惉扲堜勗  脾��
 * 礎倳
 *  hoge    char*   搣  埰堎  棬  
 * 蒍堇偯
 *  搣  媃倜堆倳��
 *
 *   ※
 * "ringo ( 勘靷勘靷陎�� ) ㄩ 100"
 *      摯鳹譴毀100誑凝窇堎��
 * "ringo ( 勘靷勘靷陎�� ) ㄩ 岆100"
 *      忒100勗埵堎�蛬蟾玾�勗NPC暵埵堇衭��
 ----------------------------------------*/
int texttoiTail( char *hoge)
{
    return 0; /* �圴紳�趙倜摯ぁ喫竣媃倜�蛫姨做匹剒�倳�婠誻梊呁� */


#if 0
    int len = strlen(hoge);

    if( len == 0 )return atoi(hoge);
    /*    凝�梩醓し�  疐勗倳棬禱峟僑崹堎    */
    while(--len && len >= 0){
        if( isdigit(hoge[len]) )
            break;
    }
    while( --len && len >= 0 ){
        if( !isdigit(hoge[len]) )
            break;
        if( hoge[len]=='+' || hoge[len]=='-')
            break;
    }
    return atoi(hoge+len+1);
#endif
#if 0
    int f = 0;
    for(i=len-1;i>=0;i--){
        if( isdigit( hoge[i] ) ) f = 1;
        if( (f == 1)
            && !( isdigit(hoge[i]) || hoge[i]=='+' || hoge[i]=='-') ){
            return atoi( hoge + i + 1 );
        }
    }
    return atoi(hoge);
#endif
}

/*----------------------------------------
 *   棬  禱倳棬凝汔竣凝鏍攝蚗袲媃趙��倳棬煦埱倜��
 *  礎倳勗  躂趙蒍埰
 * 礎倳
 *      arg               儒楁
 *      number            儒  禱瓟  埰堎鐎
 *      base            盻倳
 *      type            礎倳摯菲( CTYPE 毀 common.h 勗羕虭踏躂趙笢堎 )
 * 蒍堇偯
 *      TRUE(1)    埬  
 *      FALSE(0)   strtol誑嗟棔勗毀埬  媃埵凝埱倜
 *                  喫摯鄹摯number毀strtol摯蒍堇偯ぁ堣堎
 ----------------------------------------*/
BOOL strtolchecknum( char* arg , void* number,int base ,CTYPE type)
{
    char* buf;
    int     num;

    num = strtol( arg, &buf, base);

    switch( type ){
    case CHAR:
        *(char*)number = (char)num;
        break;
    case SHORT:
        *(short*)number = (short)num;
        break;
    case INT:
        *(int*)number = num;
        break;
    case DOUBLE:
        *(double*)number = (double)num;
        break;
    default:
        break;
    }


    if( strlen( buf ) >= 1 )
        /*
         * 呫堇  棬誑堣堎敁笢竣痀毀  儒媃拻躂趙埵笢敁笢竣痀埵摯煦��
         */
        return FALSE;

    return TRUE;

}




/*----------------------------------------
 * src 凝�� dels ぁ炩擭媃倜  棬禱噁堇謫��
 * 泬蚗袲筒溘蚗で堥毀操筒艇↓鏤ぁ堣堎痀勗曛徽��
 * 僑竘堇泬蚗袲筒溘蚗で堥毀朝謫ぁ拻埵笢��
 * 礎倳
 *  src      脾踏躂堎  棬  
 *  char*  朝謫埰堎  棬(  棬  ぁ炩擭菴)
 * 蒍堇偯
 *  埵媃
 ----------------------------------------*/
void deleteCharFromString( char* src , char* dels )
{
    int index=0;    /* ぁ拻堣誑堇摯  棬  ぁ摯 index */
    int delength;   /* dels 摯荇踏禱优擭埰堎(撙媃毀銅梣囮蚗鏤摯諱 */
    int i=0,j;/* i 毀 src 禱鳴↓鏤埰堎  倳 j 毀 dels 禱鳴↓鏤埰堎  倳 */

    delength= strlen( dels );

    while( src[i] != '\0' ){
        if( src[i] == BACKSLASH  )
            /*
             * 泬蚗袲筒溘蚗で堥埵摯ぁ操筒艇↓鏤埰堎
             * 僑竘堇べ摯  棬毀  邳贀勗遉  埰堎
             */
            src[index++] = src[++i];         /* substitute next char */

        else{
            for( j = 0 ; j < delength ; j ++ ){
                if( src[i] == dels[j] )
                    /*
                     * 朝謫埰堎  棬煦�蛪諂�趙 i 勗 1 禱倜埰
                     */
                    goto incrementi;
            }
            /*
             * 觛埰  勗炩擭踏躂趙笢埵笢摯ぁ昡鼀↓埰堎��
             */
            src[index++] =  src[i];
        }
    incrementi:
        i++;
    }

    src[index] = '\0';
}



/*----------------------------------------
 * src 凝�� dels ぁ炩擭媃倜  棬禱噁堇謫��
 * 操筒艇↓鏤毀埵笢
 * 礎倳
 *  src      脾踏躂堎  棬  
 *  char*  朝謫埰堎  棬(  棬  ぁ炩擭菴)
 * 蒍堇偯
 *  埵媃
 ----------------------------------------*/
void deleteCharFromStringNoEscape( char* src , char* dels )
{
    int index=0;    /* ぁ拻堣誑堇摯  棬  ぁ摯 index */
    int delength;   /* dels 摯荇踏禱优擭埰堎(撙媃毀銅梣囮蚗鏤摯諱 */
    int i=0,j;/* i 毀 src 禱鳴↓鏤埰堎  倳 j 毀 dels 禱鳴↓鏤埰堎  倳 */

    delength= strlen( dels );

    while( src[i] != '\0' ){
        for( j = 0 ; j < delength ; j ++ ){
            if( src[i] == dels[j] )
                /*
                 * 朝謫埰堎  棬煦�蛪諂�趙 i 勗 1 禱倜埰
                 */
                goto incrementi;
        }
        /*
         * 觛埰  勗炩擭踏躂趙笢埵笢摯ぁ昡鼀↓埰堎��
         */
        src[index++] =  src[i];
    incrementi:
        i++;
    }

    src[index] = '\0';
}

/*------------------------------------------------------------
 * 炩擭踏躂倜  棬禱炩擭踏躂倜  棬ぁ  拻凝窇堎
 * 礎倳
 *  src     char*         脾踏躂堎  棬  
 *  oldc    char          脾踏躂堎  棬
 *  newc    char          脾埰堎  棬
 * 蒍堇偯
 *  src
 ------------------------------------------------------------*/
char*   replaceString( char* src, char oldc ,char newc )
{
    char*   cp=src;

    do{
        if( *cp == oldc ) *cp=newc;
    }while( *cp++ );
    return src;
}

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


char makeCharFromEscaped( char c )//add this function,because the second had it
{
        int i;
        
        for( i = 0; i < sizeof( escapeChar )/sizeof( escapeChar[0] ); i++ )
        {
            if( escapeChar[i].escapedchar == c )
            {
               c = escapeChar[i].escapechar;
               break;
            }
        }
        return c;
}
/*----------------------------------------
 * makeEscapeStringぁ軘埬踏躂倜  棬  禱楁勗  埰
 * 礎倳
 *  src             char*       楁勗埵堎  棬  �蛦蟤噱甘及樝詳瞿�
 * 蒍堇偯
 *  src    禱蒍埰��(匴倳禱  躂喫勗媃盓埰笢源竣勗)
 ----------------------------------------*/
char   *makeStringFromEscaped( char* src )
{//ttom this function all change,beside copy from the second

    int     i;
    // CoolFish: Fix bug 2001/10/13
    // int     srclen = strlen( src );
    int	    srclen = 0;
    int     searchindex=0;
    
    // CoolFish: Fix bug 2001/10/13
    if (!src)	return	NULL;
    srclen = strlen(src);
    
    for( i = 0; i < srclen; i ++ )
    {
    // for 2Byte Word
    if( IS_2BYTEWORD( src[i] ) ){
        src[searchindex++] = src[i++];
        src[searchindex++] = src[i];
    }else
            if( src[i] == '\\' )
            {	
                // べ摯  棬勗菜��
                i++;
                src[searchindex++] = makeCharFromEscaped( src[i] );
            }
            else
            {
                src[searchindex++] = src[i];
            }
    }        
            src[searchindex] = '\0';
        
            return src;
}

char*  makeEscapeString( char* src , char* dest, int sizeofdest)
{ //ttom this function all change, copy from the second
    int i;
    int	    srclen = 0;
    int     destindex=0;
    
    // CoolFish: Fix bug 2001/10/13
    if (!src)	return	NULL;
    srclen = strlen(src);
  
    for( i = 0 ; i < srclen ; i ++ ){
         BOOL dirty=FALSE;
         int  j;
         char escapechar='\0';
		if( destindex + 1 >= sizeofdest )break;
         if( IS_2BYTEWORD( src[i] ) ){
               if( destindex + 2 >= sizeofdest )break;
               
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
                  continue;
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

//this function copy all from the second
char * ScanOneByte( char *src, char delim ){
	// Nuke
	if (!src) return NULL;

        //   棬  誑埵�忐耒觴�ぁ號喙
        for( ;src[0] != '\0'; src ++ ){
             // 蠈呁摯兒泬騷哱  凝汔竣凝禱鏍攝蚗袲
          if( IS_2BYTEWORD( src[0] ) ){
              // 蠈呁煦�蛫姨啗踽�毀兒泬騷哱嗌覕勗鵌竘鞠堎��
              // 倜煦媃兒泬騷哱媃凝埵笢鳹譴毀鼠竣媃埵笢
              if( src[1] != 0 ){
                  src ++;
              }
              continue;
          }
          //   呁煦埱倜�蛦薱蟜斥窕瑆�  棬敁  吨
          if( src[0] == delim ){
              return src;
          }
        }
        // 鳴↓鏤  崹倜�梉食晷旅欷汙朗�倜��
        return NULL;
}
                                                                                                                                                                                                                                                                                                                


/*----------------------------------------
 * delim ぁ炩擭踏躂倜  棬  禱鉏憍堇  棬敁媃趙
 * index     摯  禱  堎�菈ndex毀1ぁ毀啋竘堎��
 *   棬  蹄  ��
 * 礎倳
 *  src     楁摯  棬  
 *  delim   溢惉蕾淏敁埵堎  棬  ��    毀NULL  棬ぁ敖埱趙笢堎痀
 *  index   笰    摯  禱噁堎凝
 *  buf     踕堅  棬  禱氶  埰堎鳹鐎喜摯睽騷璃淏↓
 *  buflen  踕堅  棬  禱氶  埰堎鳹鐎摯�蚅昃�
 * 蒍堇偯
 *  堣埱倜�� TRUE(1);
 *  埵凝埱倜�� FALSE(0);
 *  ex
 *      getStringFromIndexWithDelim( "aho=hoge","=",1,buf,sizeof(buf) );
 *      buf ... aho
 *
 *      getStringFromIndexWithDelim( "aho=hoge","=",2,buf,sizeof(buf) );
 *      buf ... hoge
 *      喫躂忒  誧�桫戰瞿�
 *
 *      getStringFromIndexWithDelim( "aho=hoge","=",3,buf,sizeof(buf) );
 *      蒍堇偯 FALSE
 ----------------------------------------*/
BOOL getStringFromIndexWithDelim_body( char* src ,char* delim ,int index,
                                 char* buf , int buflen ,
                                       char *file, int line )
{//ttom this function all change,copy from the second
    int i;          /* 鳴↓鏤  倳 */
    int length =0;  /* 噁堇③媃倜  棬  摯荇踏 */
    int addlen=0;   /* 鵿踏躂堎荇踏 */
    int oneByteMode = 0; /* 兒泬騷哱さ↓迶凝ㄑ */

    if( strlen( delim ) == 1 ){ // 號喙誑兒泬騷哱埵�旍鈲巀昐�さ↓迶勗埰堎
        oneByteMode = 1;// 鼠摯傷兕泬騷哱  棬毀鏍攝蚗袲媃埵笢
    }
    for( i =  0 ; i < index ; i ++ ){
         char* last;
         src += addlen;/* 陑僑凝埱倜荇踏禱鵿埰 */
      
         if( oneByteMode ){
             // 兒泬騷哱さ↓迶煦埱倜�梜蟓倞梪斥像�
             last = ScanOneByte( src, delim[0] );
         }else{
                 last  = strstr( src , delim );  /* 峟痹崹堎 */
         }
         if( last == NULL ){
            /*
             * 陑僑凝�欷汙朗�倜摯ぁ埰迋趙昡鼀↓媃趙 return��
            */
            strcpysafe( buf , buflen, src );

            if( i == index - 1 )
                /*з踝竣汔陑僑凝埱倜*/
                return TRUE;
                                                                                                           
                /*陑僑凝�欷汙朗�倜*/
             return FALSE;
          }
          
          /*
           * 陑僑凝埱倜鐎敁  疐摯痾  摯蕍禱滑誧堎
           * 僑竘堇鉏憍�桫噱胱俶�  棬  摯荇踏
          */
          length = last - src;
                                           
          /*
           * べ摯鳴↓鏤摯諱勗陑僑凝埱倜荇踏敁 delim 摯荇踏禱鵿媃趙堁��
          */
          addlen= length + strlen( delim );
       }
       strncpysafe( buf, buflen , src,length );

       return TRUE;
}


/*------------------------------------------------------------
 *   棬   "a,b,c,d" 禱礎倳勗瓟  埰堎�蛪萼觙伒懩�毀 0 勗埰堎��
 * 凝埵堇摯  壓匴倳�跍婸�毀з踝埱敁婝笢��
 * 礎倳
 *  src         char*   楁摯  棬  
 *  int1        int*    int摯睽騷璃淏��(a禱瓟  埰堎)
 *  int2        int*    int摯睽騷璃淏��(b禱瓟  埰堎)
 *  int3        int*    int摯睽騷璃淏��(c禱瓟  埰堎)
 *  int4        int*    int摯睽騷璃淏��(d禱瓟  埰堎)
 * 蒍堇偯
 *  埵媃
 ------------------------------------------------------------*/
void getFourIntsFromString(char* src,int* int1,int* int2,int* int3,
                           int* int4)
{
    int     ret;
    char    string[128];

    ret = getStringFromIndexWithDelim( src,"," ,1,string,sizeof(string));
    if( ret == FALSE )*int1=0;
    else    *int1 = atoi(string);

    ret = getStringFromIndexWithDelim( src, ",",2,string,sizeof(string) );
    if( ret == FALSE )*int2=0;
    else    *int2 = atoi(string);

    ret = getStringFromIndexWithDelim( src, ",",3,string,sizeof(string) );
    if( ret == FALSE )*int3=0;
    else    *int3 = atoi(string);

    ret = getStringFromIndexWithDelim( src, ",",4,string,sizeof(string) );
    if( ret == FALSE )*int4=0;
    else    *int4 = atoi(string);

}


/*----------------------------------------------
 * src 摯  ぁdelsぁ炩擭媃倜  棬誑  襄媃趙笢倜��
 * 1僑勗竘敁誧堎��
 * 泬蚗袲筒溘蚗で堥毀操筒艇↓鏤で↓艇璃筒敁埵堎��
 * 媃倜誑埱趙  襄媃倜泬蚗袲筒溘蚗で堥禱觛埰痀毀ぁ拻埵笢��
 * 礎倳
 *  src      撬埰堎  棬  
 *  dels   觛埰  棬(  棬  ぁ炩擭菴)
 * 蒍堇偯
 *  埵媃
 ---------------------------------------------*/
void deleteSequentChar( char* src , char* dels )
{
    int length;         /* src 摯荇踏 */
    int delength;       /* dels 摯荇踏 */
    int i,j;            /* 鳴↓鏤  倳 */
    int index=0;        /* 賃擭  棬  摯 index */
    char backchar='\0';   /* 朝謫埰堎  棬 */

    length = strlen( src );
    delength = strlen( dels );

    /*
     * 鳴↓鏤埰堎�婠蝐槸旚漶灄彸�
     */
    for( i = 0 ; i < length ; i ++ ){
        if( src[i] == BACKSLASH ){
            /*
             * 泬蚗袲筒溘蚗で堥煦埱倜��
             * 倯�瘨螫�  僑( BACKSLASH猷�嬭蝤�べ摯  棬 )
             * 鵌誧堎
             */
            src[index++]=src[i++];
            if( i >= length ){
                print( "*\n");
                break;
            }
            src[index++]=src[i];
            /*   襄誑鰼埱倜痀勗埰堎 */
            backchar = '\0';
            continue;
        }
        if( src[i] == backchar )
            /*
             * 朝謫埰堎忒啋煦埱倜�欶珅笭ぴ聿苺蛫棍�堇朝謫勗埵堎��
             */
            continue;

        /*
         *   襄摯朝謫埰堎  棬誑埵�忐耒�倜摯ぁ backchar 毀 NULL   棬勗埰堎
         * 喫躂誑凝庌啋庌
         */
        backchar = '\0';
        for( j = 0 ; j < delength ; j ++ ){
            if( src[i] == dels[j] ){
                /*
                 * 朝謫埰堎  棬  煦�� 喫喫ぁ continue 媃埵笢痀勗源堇
                 * 郖  棬毀朝謫媃埵笢痀誑瞀擂ぁ拻堎��
                 */
                backchar=src[i];
                break;
            }
        }
        src[index++]=src[i];
    }
    src[index++]='\0';
}

/*----------------------------------------
 * hash偯禱蒍埰��  棬  勗摯陑窅竣喫敁
 * 喫躂毀  鼠摯竘竘埵摯ぁ囮鳴湖惉扲堜毀源竣渦凝�梛牰�
 * 笢笢囮鳴湖惉扲堜�欷ひ苺�
 * 礎倳
 *  s     棬  
 * 蒍堇偯
 *  int 辿蚗で堥偯
 *----------------------------------------*/
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

/*----------------------------------------
 * 痲↓庈鳴勗泬騷璃迶媃倜藺艇蚗哱禱蒍埰( TCP )
 * 礎倳
 *          port        痲↓庈鳴睽↓哱
 * 蒍堇偯
 *          -1      操溘↓
 *                      1. socketで筒  堜昡↓鳴摯操溘↓
 *                      2. bindで筒  堜昡↓鳴摯操溘↓
 *                      3. listenで筒  堜昡↓鳴摯操溘↓
 ----------------------------------------*/
int bindlocalhost( int port )
{
    struct sockaddr_in sin;         /*痲↓庈鳴囮迶畛筒暵*/
    int sfd;                        /*藺艇蚗哱溢贖筒袲惉鏤淏*/
    int rc;                         /*痲↓庈鳴惉淏↓璃昡↓迶*/

    /*AF_INET鏤痲哱昡鳴 摯 SOCK_STREAM */
    sfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sfd == -1 ){
        print( "%s\n" , strerror(errno ) );
        return -1;
    }
    
    if( getReuseaddr() ) {
		int sendbuff;
	    /* 痲↓庈鳴囮迶畛筒摯瑁媋暵 */
		setsockopt( sfd, SOL_SOCKET, SO_REUSEADDR, 
						(char *)&sendbuff, sizeof( sendbuff));
	}
	
    /*0袲惉囮媃趙偯禱瓟  */
    memset( &sin ,0, sizeof( struct sockaddr_in ) );
    sin.sin_family=AF_INET;
    sin.sin_port = htons( port );
    sin.sin_addr.s_addr = INADDR_ANY;

    rc = bind( sfd , (struct sockaddr*)&sin, sizeof(struct sockaddr_in));
    if( rc == -1 ){
        print( "%s\n" , strerror(errno ) );
        return -1;
    }
	
	
    rc = listen( sfd , 5 );
    if( rc == -1 ){
        print( "%s\n" , strerror(errno ) );
        return -1;
    }
    return sfd;
}



/*----------------------------------------
 * 昡汜袲哱埰堎��( TCP )
 * 礎倳
 *      hostname    旽襄埰堎坒筒哱  
 *                      (xx.xxx.xxx.xxxぁ忒笢笢媃xxx.co.jp敁凝ぁ忒笢笢)
 *      port        旽襄埰堎坒筒哱摯睽↓哱  
 * 蒍堇偯
 *      溢贖筒袲惉鏤淏( -1 摯鄹毀操溘↓ )
 ----------------------------------------*/
int connectHost( char* hostname , unsigned short port )
{
    struct sockaddr_in sock;        /*connect摯鄹勗窅竣*/
    struct hostent* hoste;          /*hostname摯hostent*/
    int     fd;                     /*昡汜袲哱媃倜藺艇蚗哱溢贖筒袲惉鏤淏*/
    int     lr;                     /*痲↓庈鳴惉淏↓璃昡↓迶*/

    memset( &sock ,  0 , sizeof( struct sockaddr_in ) );
    sock.sin_family = AF_INET;
    sock.sin_port = htons( port );

    /* dot notation 凝汔竣凝禱ぅ迋堎 */
    sock.sin_addr.s_addr = inet_addr( hostname );
    if( sock.sin_addr.s_addr == -1 ){
        /*
         *操溘↓ dot notation ぁ毀埵笢�訹ぬ袘疋�趙  玊摯  鞳搣鋆誑  菺煦��
         * dns禱痲凝埵崹躂�屻牟欷聿釓揧晰藬撅樛倞宎動硈薱蟜皮忍�趙�岉戰瞿�
         */
        hoste = gethostbyname( hostname );
        if( hoste == NULL ){
            print( "鳳�＞鷋�靡: %s\n", hostname);
            return -1;
        }

        memcpy((void*)&sock.sin_addr.s_addr ,
               hoste->h_addr , sizeof(struct in_addr) );
    }

    /*藺艇蚗哱禱軘堎*/
    fd = socket( AF_INET, SOCK_STREAM , 0 );
    if( fd == -1 ){
        print("Cannot Create Socket(%s errno:%d)\n"
              ,strerror( errno ) ,errno );
        return -1;
    }
    /*昡汜袲哱埰堎*/
    lr = connect(fd,(struct sockaddr*)&sock,sizeof(struct sockaddr_in));
    if( lr != 0 ){
        print("Cannot connect. (%s errno:%d)\n"
              ,strerror( errno ) ,errno );
        return -1;
    }

    return fd;
}


/*----------------------------------------
 * 泬蚗啞栝摯  勗搊菜楔蠕誑堣堎凝汔竣凝��
 * 撙媃傲笢囮鳴湖惉扲堜勗  脾��
 * 礎倳
 *  char*   ぅ迋堎  棬  
 * 蒍堇偯
 *    疐勗峟僑凝倜搊菜楔蠕摯痾       囮袲掛筒媃倜鄹勗笢笢偯��
 *  汔竣笢竣痀凝敁笢竣敁��
 *      existsNewLinwCharacter( "aho\nhoge\n" )
 *  煦敁��蒍堇偯毀 3 勗埵堎�蛦蟤勳樖衈紳彸觴ラ�
 *  -1      埵凝埱倜
 *
 ----------------------------------------*/
int existsNewLineCharacter( char* line )
{
#if 1
    char*   old = line;
    do{
        if( *line == NEWLINE)return line-old;
    }while( *(line++) );
#else
    int i;                          /*鳴↓鏤  倳*/
    int length = strlen( line );    /*ぅ迋堎  棬  摯荇踏*/
    for( i = 0 ; i < length ; i ++ ){
        if( line[i] == NEWLINE )
            return i;
    }
#endif

    return -1;
}



/*----------------------------------------
 *    棬    勗  棬誑  渦躂堎鳹鐎禱凝窇埰�詎�    凝禱鋆誧躂堎��
 *  nindex( string , c , 1 ) 敁 index( string , c ) 毀 equivalent ぁ堣堎��
 *  number誑  竘倜毀0摯鄹毀蒍堇偯毀��string敁  啋ぁ堣堎��
 *
 *  礎倳
 *      string  char*   ぅ迋堎  棬  
 *      c       int     ぅ迋堎  棬
 *      number  int       渦堎  蠕
 *  蒍堇偯
 *      郖窙媃倜  棬摯睽騷璃淏↓
 *      NULL    陑僑凝�欷汙朗�倜��
 ----------------------------------------*/
char* nindex( char* string, int c , int number )
{
    int i;                          /*鳴↓鏤  倳*/
    int num=0;
    int length = strlen( string );  /*ぅ迋堎  棬  摯荇踏*/
    if( number <= 0 )return string;
    for( i = 0 ; i < length ; i ++ ){
        if( string[i] == c )num++;
        if( number == num ) return &string[i];
    }
    return NULL;
}

BOOL rrd( char* dirname , STRING64* buf ,int bufsize, int* index)
{
    DIR*    d;
    char    dirn[1024];

    d = opendir( dirname );
    if( d == NULL )return FALSE;

    while( 1 ){
        struct dirent* dent;
        struct  stat    st;
        dent = readdir( d );
        if( dent == NULL ){
            if( errno == EBADF ){
                errorprint;
                closedir(d);
                return FALSE;
            }else
                /*  successful */
                break;
        }

        /*  . ぁ鴽竘堎啞栝騷鳴毀  竘埵笢    */
        if(  dent->d_name[0] == '.' )continue;

        snprintf(dirn, sizeof(dirn), "%s/%s" , dirname,dent->d_name );
        if( stat( dirn , &st ) == -1 )continue;
        if( S_ISDIR( st.st_mode ) ){
            if( rrd( dirn , buf ,bufsize, index) == FALSE ){
                closedir(d);
                return FALSE;
            }
        }else{
            if( *index >= bufsize )
                break;

            strcpysafe( buf[*index].string , sizeof( buf[*index].string ), dirn );
            (*index)++;
        }
    }
    closedir(d);
    return TRUE;
}

int rgetFileName( char* dirname , STRING64* string, int size)
{
    int     index=0;
    if( rrd( dirname , string ,size ,&index ) == FALSE )return -1;
    else return index;
}

BOOL checkStringsUnique( char** strings, int num ,int verbose)
{
    int     i,j;
    for( i = 0 ; i < num - 1; i ++ ){
        for( j = i+1 ; j < num ; j ++){
            if( !strcmp( strings[i] ,strings[j] )){
                if( verbose )
                    print( "Overlapp string is %s\n", strings[i] );
                return FALSE;
            }
		}
	}
    return TRUE;
}

BOOL PointInRect( RECT* rect, POINT* p )
{
    if( rect->x         <= p->x && p->x <= rect->x + rect->width &&
        rect->y         <= p->y && p->y <= rect->y + rect->height )
        return TRUE;
    return FALSE;
}

BOOL CoordinateInRect( RECT* rect, int x, int y)
{
    POINT   p={x,y};
    return PointInRect(rect,&p);
}

int clipRect( RECT *rect1, RECT *rect2, RECT *ret )
{
    if(   rect1->x > rect2->x + rect2->width -1
       || rect2->x > rect1->x + rect1->width -1
       || rect1->y > rect2->y + rect2->height -1
       || rect2->y > rect1->y + rect1->height -1 )
	   return 0;
    ret->x = max( rect1->x, rect2->x );
    ret->y = max( rect1->y, rect2->y );
    ret->width = min( rect1->x+rect1->width, rect2->x+rect2->width ) - ret->x;
    ret->height = min( rect1->y+rect1->height, rect2->y+rect2->height ) - ret->y;
    return 1;
}

BOOL isstring1or0( char*  string )
{
    if( strcasecmp(string,"TRUE" ) == 0 )return TRUE;
    if( strcasecmp(string,"FALSE") == 0 )return FALSE;
    if( strcasecmp(string,"1"    ) == 0 )return TRUE;
    if( strcasecmp(string,"0"    ) == 0 )return FALSE;
    if( strcasecmp(string,"ON"   ) == 0 )return TRUE;
    if( strcasecmp(string,"OFF"  ) == 0 )return FALSE;
    return FALSE;
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
                /* copy it */
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

/*------------------------------------------------------------
 * 侘菲ぁ  覕鬻禱忒敁誧堎�� by ringo
 * Ц摯遉  暵
 *
 * double val1 , val2 : 喫摯偯摯堣笢煦禱敁堎
 * double d :   覕  
 *
 *
 *  -d<0----- val1 ---0<d<1------- val2 ------d>1----
 *
 *
 ------------------------------------------------------------*/
float  linearDiv( float val1 , float val2 , float d )
{
    return val1 + ( val2 - val1 ) * ( d );
}


/*------------------------------------------------------------
 *   棬  摯埵凝凝��  徽埵  棬禱len  棬敁堇煦媃趙溘璃譫堜埵
 *   棬  禱僑�彸瞿腇PC摯溘璃譫堜蚕筒極↓迶牁埬勗僑凝埱趙堎
 *
 * char *cand : 喫喫凝�梩銅晻袽�  摯  棬��
 * char *out : ③  泬蚗啞栝
 * int len : ③  摯荇踏
 ------------------------------------------------------------*/
void makeRandomString( char *cand , char *out , int len )
{
    int i;
    int l = strlen( cand );

    for(i=0;i<len;i++){
        out[i] = cand[ rand() % l];
    }
    out[i] = '\0';
}


/*------------------------------------------------------------
 * 礎倳勗炩擭踏躂倜啞栝騷鳴誑啞栝騷鳴凝汔竣禱ぅ迋堎
 * 礎倳
 *  filename        char*        啞栝騷鳴  
 * 蒍堇偯
 *  啞栝騷鳴        TRUE(1)
 *  啞栝騷鳴ぁ埵笢  FALSE(0)
 ------------------------------------------------------------*/
BOOL isExistFile( char* filename )
{
    /*  fopen ぁぅ迋堎  */
    FILE*   fp;
    fp = fopen( filename, "w" );
    if( fp ){
        fclose( fp );
        return TRUE;
    }else
        return FALSE;
}
/*------------------------------------------------------------
 * 10鵌  62鵌  儒
 * 礎倳
 * a        int     楁摯兒兔鵌
 * out      char *  氶  泬蚗啞栝↓
 * outlen   int     氶  泬蚗啞栝摯  拻踏
 *   堇偯
 *          埬      out摯囮迶畛筒
 *          謄      NULL
 ------------------------------------------------------------*/
char *cnv10to62( int a, char *out, int outlen )
{
#if 1
	int		i, j;
    char    base[] = { "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    int     tmp[64];
    int     src;
    int minus;
	int baselen = sizeof( base)-1;
    if( a < 0 ){
		minus = 1;
		a *= -1;
	} else {
		minus = 0;
	}
    /* special case */
    if( a < baselen) {
		if( minus ){
			*(out) = '-';
			*(out+1) = base[a];
			*(out+2) = '\0';
			return (out);
		} else {
			*out = base[a];
			*(out+1) = '\0';
			return( out);
		}
    }
    src = a;
    for( i = 0; src >= baselen; i ++ ) {
        tmp[i] = src % baselen;
        src /= baselen;
    }
    i--;
    if( minus ){
		*out = '-';
     	*(out+1) = base[src];
		for( j = 2; i >= 0; i --, j ++ ) {
			if( j > outlen - 2 ) return NULL;
			*(out+j) = base[tmp[i]];
		}
	} else {
     	*out = base[src];
		for( j = 1; i >= 0; i --, j ++ ) {
			if( j > outlen - 2 ) return NULL;
			*(out+j) = base[tmp[i]];
		}
	}
	*(out+j) = '\0';
    return( out);
#else
/* 捙笢   */
#define     CNV_NUMBER  62
	int		i, j;
    char    base[] = { "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    int     tmp[64];
    int     src;
    if( a < 0 ) return( NULL);
    if( a < CNV_NUMBER) {
	    *out = base[a];
	    *(out+1) = '\0';
        return( out);
    }
    src = a;
    for( i = 0; src >= CNV_NUMBER; i ++ ) {
        tmp[i] = src % CNV_NUMBER;
        src /= CNV_NUMBER;
    }
    i--;
	*out = base[src];
    for( j = 1; i >= 0; i --, j ++ ) {
		if( j > outlen - 2 ) return NULL;
		*(out+j) = base[tmp[i]];
    }
	*(out+j) = '\0';
    return( out);
#undef      CNV_NUMBER
#endif
}
/* 
 * 僄窇�桫憶�    摯  ぁ�楅扆擊朮�  媃趙笢堎凝ぅ迋堎匴倳
 * -1 毀葡濯翻敁笢竣  埵匴倳
 *
 * 礎倳
 * src		*int		ぅ迋堎楁摯    
 * srclen	int			src 摯  拻踏
 *
 *   堇偯   福  堣堇	TRUE
 *				埵媃	FALSE
 */
BOOL checkRedundancy( int *src, int srclen)
{
	int		i,j;
	int		ret = FALSE;
	
	for( i = 0; i < srclen; i ++ ) {
		if( *(src + i) != -1 ) {
			for( j = i+1; j < srclen; j ++ ) {
				if( *(src +i) == *( src+j) ) {
					ret = TRUE;
					break;
				}
			}
		}
	}
	return ret;
}
void
shuffle_ints( int num, int *a, int t )
{
    int i;
    for(i=0;i<t; i++ )
    {
        int x = random() % num;
        int y = random() % num;
        int s;
        s = a[x];
        a[x] = a[y];
        a[y] = s;
    }
}


static unsigned char BitTable[] =	/*	扠蚗哱摯嶈怮棹禱楪勗埰堎  ↓々鳴	*/
{
		0x00 , 0x80 , 0x40 , 0xC0 , 0x20 , 0xA0 , 0x60 , 0xE0 , 
		0x10 , 0x90 , 0x50 , 0xD0 , 0x30 , 0xB0 , 0x70 , 0xF0 , 
		0x08 , 0x88 , 0x48 , 0xC8 , 0x28 , 0xA8 , 0x68 , 0xE8 , 
		0x18 , 0x98 , 0x58 , 0xD8 , 0x38 , 0xB8 , 0x78 , 0xF8 , 
		0x04 , 0x84 , 0x44 , 0xC4 , 0x24 , 0xA4 , 0x64 , 0xE4 , 
		0x14 , 0x94 , 0x54 , 0xD4 , 0x34 , 0xB4 , 0x74 , 0xF4 , 
		0x0C , 0x8C , 0x4C , 0xCC , 0x2C , 0xAC , 0x6C , 0xEC , 
		0x1C , 0x9C , 0x5C , 0xDC , 0x3C , 0xBC , 0x7C , 0xFC , 
		0x02 , 0x82 , 0x42 , 0xC2 , 0x22 , 0xA2 , 0x62 , 0xE2 , 
		0x12 , 0x92 , 0x52 , 0xD2 , 0x32 , 0xB2 , 0x72 , 0xF2 , 
		0x0A , 0x8A , 0x4A , 0xCA , 0x2A , 0xAA , 0x6A , 0xEA , 
		0x1A , 0x9A , 0x5A , 0xDA , 0x3A , 0xBA , 0x7A , 0xFA , 
		0x06 , 0x86 , 0x46 , 0xC6 , 0x26 , 0xA6 , 0x66 , 0xE6 , 
		0x16 , 0x96 , 0x56 , 0xD6 , 0x36 , 0xB6 , 0x76 , 0xF6 , 
		0x0E , 0x8E , 0x4E , 0xCE , 0x2E , 0xAE , 0x6E , 0xEE , 
		0x1E , 0x9E , 0x5E , 0xDE , 0x3E , 0xBE , 0x7E , 0xFE , 
		0x01 , 0x81 , 0x41 , 0xC1 , 0x21 , 0xA1 , 0x61 , 0xE1 , 
		0x11 , 0x91 , 0x51 , 0xD1 , 0x31 , 0xB1 , 0x71 , 0xF1 , 
		0x09 , 0x89 , 0x49 , 0xC9 , 0x29 , 0xA9 , 0x69 , 0xE9 , 
		0x19 , 0x99 , 0x59 , 0xD9 , 0x39 , 0xB9 , 0x79 , 0xF9 , 
		0x05 , 0x85 , 0x45 , 0xC5 , 0x25 , 0xA5 , 0x65 , 0xE5 , 
		0x15 , 0x95 , 0x55 , 0xD5 , 0x35 , 0xB5 , 0x75 , 0xF5 , 
		0x0D , 0x8D , 0x4D , 0xCD , 0x2D , 0xAD , 0x6D , 0xED , 
		0x1D , 0x9D , 0x5D , 0xDD , 0x3D , 0xBD , 0x7D , 0xFD , 
		0x03 , 0x83 , 0x43 , 0xC3 , 0x23 , 0xA3 , 0x63 , 0xE3 , 
		0x13 , 0x93 , 0x53 , 0xD3 , 0x33 , 0xB3 , 0x73 , 0xF3 , 
		0x0B , 0x8B , 0x4B , 0xCB , 0x2B , 0xAB , 0x6B , 0xEB , 
		0x1B , 0x9B , 0x5B , 0xDB , 0x3B , 0xBB , 0x7B , 0xFB , 
		0x07 , 0x87 , 0x47 , 0xC7 , 0x27 , 0xA7 , 0x67 , 0xE7 , 
		0x17 , 0x97 , 0x57 , 0xD7 , 0x37 , 0xB7 , 0x77 , 0xF7 , 
		0x0F , 0x8F , 0x4F , 0xCF , 0x2F , 0xAF , 0x6F , 0xEF , 
		0x1F , 0x9F , 0x5F , 0xDF , 0x3F , 0xBF , 0x7F , 0xFF
};
static unsigned short crctab16[] =	/*	crc��摯銓猀  ↓々鳴		*/
{
		0x0000,  0x1021,  0x2042,  0x3063,  0x4084,  0x50a5,  0x60c6,  0x70e7,
		0x8108,  0x9129,  0xa14a,  0xb16b,  0xc18c,  0xd1ad,  0xe1ce,  0xf1ef,
		0x1231,  0x0210,  0x3273,  0x2252,  0x52b5,  0x4294,  0x72f7,  0x62d6,
		0x9339,  0x8318,  0xb37b,  0xa35a,  0xd3bd,  0xc39c,  0xf3ff,  0xe3de,
		0x2462,  0x3443,  0x0420,  0x1401,  0x64e6,  0x74c7,  0x44a4,  0x5485,
		0xa56a,  0xb54b,  0x8528,  0x9509,  0xe5ee,  0xf5cf,  0xc5ac,  0xd58d,
		0x3653,  0x2672,  0x1611,  0x0630,  0x76d7,  0x66f6,  0x5695,  0x46b4,
		0xb75b,  0xa77a,  0x9719,  0x8738,  0xf7df,  0xe7fe,  0xd79d,  0xc7bc,
		0x48c4,  0x58e5,  0x6886,  0x78a7,  0x0840,  0x1861,  0x2802,  0x3823,
		0xc9cc,  0xd9ed,  0xe98e,  0xf9af,  0x8948,  0x9969,  0xa90a,  0xb92b,
		0x5af5,  0x4ad4,  0x7ab7,  0x6a96,  0x1a71,  0x0a50,  0x3a33,  0x2a12,
		0xdbfd,  0xcbdc,  0xfbbf,  0xeb9e,  0x9b79,  0x8b58,  0xbb3b,  0xab1a,
		0x6ca6,  0x7c87,  0x4ce4,  0x5cc5,  0x2c22,  0x3c03,  0x0c60,  0x1c41,
		0xedae,  0xfd8f,  0xcdec,  0xddcd,  0xad2a,  0xbd0b,  0x8d68,  0x9d49,
		0x7e97,  0x6eb6,  0x5ed5,  0x4ef4,  0x3e13,  0x2e32,  0x1e51,  0x0e70,
		0xff9f,  0xefbe,  0xdfdd,  0xcffc,  0xbf1b,  0xaf3a,  0x9f59,  0x8f78,
		0x9188,  0x81a9,  0xb1ca,  0xa1eb,  0xd10c,  0xc12d,  0xf14e,  0xe16f,
		0x1080,  0x00a1,  0x30c2,  0x20e3,  0x5004,  0x4025,  0x7046,  0x6067,
		0x83b9,  0x9398,  0xa3fb,  0xb3da,  0xc33d,  0xd31c,  0xe37f,  0xf35e,
		0x02b1,  0x1290,  0x22f3,  0x32d2,  0x4235,  0x5214,  0x6277,  0x7256,
		0xb5ea,  0xa5cb,  0x95a8,  0x8589,  0xf56e,  0xe54f,  0xd52c,  0xc50d,
		0x34e2,  0x24c3,  0x14a0,  0x0481,  0x7466,  0x6447,  0x5424,  0x4405,
		0xa7db,  0xb7fa,  0x8799,  0x97b8,  0xe75f,  0xf77e,  0xc71d,  0xd73c,
		0x26d3,  0x36f2,  0x0691,  0x16b0,  0x6657,  0x7676,  0x4615,  0x5634,
		0xd94c,  0xc96d,  0xf90e,  0xe92f,  0x99c8,  0x89e9,  0xb98a,  0xa9ab,
		0x5844,  0x4865,  0x7806,  0x6827,  0x18c0,  0x08e1,  0x3882,  0x28a3,
		0xcb7d,  0xdb5c,  0xeb3f,  0xfb1e,  0x8bf9,  0x9bd8,  0xabbb,  0xbb9a,
		0x4a75,  0x5a54,  0x6a37,  0x7a16,  0x0af1,  0x1ad0,  0x2ab3,  0x3a92,
		0xfd2e,  0xed0f,  0xdd6c,  0xcd4d,  0xbdaa,  0xad8b,  0x9de8,  0x8dc9,
		0x7c26,  0x6c07,  0x5c64,  0x4c45,  0x3ca2,  0x2c83,  0x1ce0,  0x0cc1,
		0xef1f,  0xff3e,  0xcf5d,  0xdf7c,  0xaf9b,  0xbfba,  0x8fd9,  0x9ff8,
		0x6e17,  0x7e36,  0x4e55,  0x5e74,  0x2e93,  0x3eb2,  0x0ed1,  0x1ef0,
};
unsigned short CheckCRC( unsigned char *p , int size )
{
	unsigned short	crc = 0;
	int		i;
	
	for( i = 0 ; i < size ; i ++ ){
		crc = ( crctab16[ ( crc >> 8 ) & 0xFF ] 
					^ ( crc << 8 ) ^ BitTable[ p[ i ] ] );
	}
	return crc;
}

// Add Code By Shan  2001.06.16
INLINE double time_diff(struct timeval subtrahend,
                      struct timeval subtractor)
{
    return( (subtrahend.tv_sec - subtractor.tv_sec)
             + (subtrahend.tv_usec
             - subtractor.tv_usec  ) / (double)1E6 );
}
