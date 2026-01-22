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
  仃勾及荼垫毛仇欠允
  娄醒“ 手午及  侬  ［  涛  凳今木月冗

  windows , macintosh , unix及升及垫手覆杀匹五月［

  windows : \r\n
  macintosh : \n\r
  unix : \n

  分匀凶井卅
  macintosh : \r
  匹允［
  公木午仇及皿夫弘仿丞分午仃勾及  侬元扎卅仁化手
  荼垫壅允仃升中中氏匹允井
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
 * 仃勾及1  侬毛潸曰轮仁
 * 娄醒
 *  src           凳允月  侬  
 ----------------------------------------*/
void chop( char* src )
{
    /*
     * 凶分凶氏卞    及  侬毛潸曰轮仁
     */
    int length = strlen( src );
    if( length == 0 ) return;
    src[length-1] = '\0';
}


/*----------------------------------------
 * 仃勾及  侬毛潸月［公及  侬毛  醒隙烂匹五月［
 * 娄醒
 *  src     葭及  侬  
 *  delim   绰轮允月  侬及    ［    反NULL  侬匹蔽匀化中月仪
 ----------------------------------------*/
void dchop( char* src , char* del)
{
    int dellen, srclen;
    int i;
    BOOL delete=FALSE;

    /*
     * src及  欠井日譬屯化中仁［1  侬卞勾五 del 允屯化毛伙□皿仄化
     * 域谯允木壬壅允［公仄化戚及  侬卞中仁［
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

    /*  濠曰潸曰    */
    if( delete )src[srclen - 1] = '\0';
}


/*----------------------------------------
 * dchop及菅［  赓及  侬毛潸月［公及  侬毛  醒隙烂匹五月［
 * 娄醒
 *  src     葭及  侬  
 *  delim   绰轮允月  侬及    ［    反NULL  侬匹蔽匀化中月仪
 ----------------------------------------*/
void pohcd( char* src , char* del)
{
    int dellen, srclen;
    int i;
    BOOL delete=FALSE;

    /*
     * src及蟆井日譬屯化中仁［1  侬卞勾五 del 允屯化毛伙□皿仄化
     * 域谯允木壬壅允［公仄化戚及  侬卞中仁［
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

    /*  绰轮    */
    if( delete )
        /*  仇及戊疋□匹 NULL  侬手戊疋□今木月 */
        for( i = 0 ; i < srclen ; i ++ )
            src[i] = src[i+1];
}


/*----------------------------------------
 *   侬  卞  侬互殖引木化中月井升丹井譬屯月［
 * 娄醒
 *  src     char*   譬屯月  侬  
 *  include char*   殖引木化中月井升丹井及  侬  ［
 * 忒曰袄
 *  岳        赓卞殖引木化中凶src及桦赭
 *  撩      -1
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
 * EUC及2田奶玄  分仃毛戊疋□仄卅中strncpy
 * freeBSD 及仿奶皮仿伉井日 strncpy 毛潸匀化五化｝
 * 公木毛手午卞荼瞻［*dest++ 午井银中凶中仃升｝瑛宁赐辆卞愤褥互
 * 卅中及匹｝银匀化中卅中［井歹曰卞    分仃升｝禾奶件正手银匀化化
 * 井卅曰  卅皿夫弘仿丞
 * 娄醒
 *  dest        char*   戊疋□燮
 *  src         char*   末□旦
 *  n           size_t  赢今
 * 忒曰袄
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
                /*  戊疋□仄云歹匀凶日 NULL   侬毛  木月   */
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
 * src 井日 dest 卞戊疋□允月［ 戊疋□燮及扔奶术毛譬屯月［
 * 娄醒
 *      dest        戊疋□燮
 *      n           扔奶术
 *      src         葭及  侬  
 ----------------------------------------*/
void strcpysafe( char* dest ,size_t n ,const char* src )
{
    /*
     * src 井日 dest 尺戊疋□允月.
     * strcpy, strncpy 匹反 dest 方曰 戊疋□允月汹互
     *   五中凛卞裟少午,丢乒伉陆失弁本旦互粟月.
     * 仇木毛  什啃卞, strncpy 互丐月互 strlen( src ) 互 n 方曰
     *   五中凛卞反, dest 及    互 NULL   侬午反卅日卅中.
     *
     * 仄凶互匀化 dest 及  五今方曰 src 及幻丹互赢中凛卞反
     * n-1 匹 strncpy 毛允月. 凝今中凛反公及引引戊疋□允月
     *
     * n 互  及凛反云井仄仁卅月及匹  及凛反 窒手仄卅中［
     *
     */
    // Nuke +1 (08/25): Danger if src=0
    if (!src) {
        *dest = '\0';
        return;
    }
    if( n <= 0 )        /* 窒手仄卅中   */
        return;

    /*  仇及凛鳔匹｝ n >= 1 动晓互瑁烂  */
    /*  NULL  侬毛哔  仄化  胜允月  */
    else if( n < strlen( src ) + 1 ){
        /*
         * 田永白央互箫曰卅中及匹 n - 1(NULL  侬)
         * 匹 strncpy 毛裟少
         */
        strncpy2( dest , src , n-1 );
        dest[n-1]='\0';
    }else
        strcpy( dest , src );

}

/*----------------------------------------
 * src 井日 length 掏允［   戊疋□燮及  五今毛譬屯月［
 * 娄醒
 *          dest        戊疋□燮
 *          n           戊疋□燮及扔奶术
 *          src         戊疋□葭
 *          length      窒  侬戊疋□允月井
 ----------------------------------------*/
void strncpysafe( char* dest , const size_t n ,
                  const char* src ,const int length )
{
    /*
     * src 井日 dest 卞 length 戊疋□允月
     * strcpy, strncpy 匹反 dest 方曰 戊疋□允月汹互
     *   五中凛卞裟少午,丢乒伉陆失弁本旦互粟月.
     * 仇及楮醒匹反｝strlen( src ) 午 length 及凝今中幻丹
     * (  端卞戊疋□允月汹) 午 dest 及扔奶术毛  屯化｝
     * strcpysafe 午  元仪毛允月［
     */

    int Short;
    Short = min( strlen( src ) , length );

    /* NULL  侬 毛哔  仄凶  胜 */
    if( n < Short + 1 ){
        /*
         * 田永白央互箫曰卅中及匹 n - 1(NULL  侬)
         * 匹 strncpy 毛裟少
         */
        strncpy2( dest , src , n-1 );
        dest[n-1]='\0';

    }else if( n <= 0 ){
        return;
    }else{
        /*
         * 田永白央反蜗坌卞丐月及匹 Short 匹strncpy毛裟少
         * 卅云 src 卞反 Short 及赢今  卞 NULL 互卅中及匹｝
         * dest 卞反 馨笛仄化云仁［
         */

        strncpy2( dest , src , Short );
        dest[Short]= '\0';

    }
}

/*------------------------------------------------------------
 * strcat毛兜蝈卞  蜇允月［赞中及匹｝    卅桦宁毛轮中化银歹卅中仪［
 * 娄醒
 *  src     char*       葭卞卅月  侬  
 *  size    int         src 及扔奶术(   欠卞升木分仃酸匀化中月井匹反
 *                          卅仁化田永白央蝈   )
 *  ap      char*       尥仃笛尹月  
 * 忒曰袄
 *  src     (  木阂迕)
 ------------------------------------------------------------*/
char* strcatsafe( char* src, int size, char* ap )
{
    strcpysafe( src + strlen( src ) , size - strlen(src), ap );
    return src;
}

/*----------------------------------------
 *   侬  及    毛  胜允月［  元井升丹井仄井  烂仄卅中 by ringo
 * char *s1 , *s2 :   胜允月  侬  
 * 忒曰袄
 * 0 “ 云卅元
 * 1 : 仇午卅月
 * 仇木反NPC及  平旦玄荸  卞支仁分勾［
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
 *   侬  及    卞踏井木化中月帮醒毛必永玄允月［
 * NPC及  平旦玄荸  卞支仁分勾［
 * 凶分仄    卞中日氏  侬互丐月桦宁  骰允月［
 * 耨痊互  切卅中失伙打伉术丞卞  凳［
 * 娄醒
 *  hoge    char*   荸  允月  侬  
 * 忒曰袄
 *  荸  仄凶帮醒［
 *
 *   “
 * "ringo ( 吧噗吧噗星人 ) ： 100"
 *      及桦宁反100互井尹月［
 * "ringo ( 吧噗吧噗星人 ) ： 是100"
 *      手100卞卅月［幻氏引卞NPC迕卅曰友［
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
 *   侬  井日｝醒袄毛  允午  歹木月  坌毛  五请允［燮  及幻丹井日
 * 茧仄化  五请允［euc迕［
 *
 *    : in "这本书七万册300美金吗？"
 *      out "7万"
 * 娄醒
 * char *in:      侬  
 * char *out:请    侬  
 * int outsiz:请    侬  及田永白央扔奶术
 * 忒曰袄
 * 醒袄毛  允  坌互丐匀凶日公及  侬  及赢今｝卅井匀凶日ㄟ
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
            /* EUC及桦宁 */
            tmp[0] = in[i];
            tmp[1] = in[i+1];
            tmp[2] = '\0';
        }else{
            /* ASCII及桦宁 */
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
 *   侬  毛int卞  晶［
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
 * 醒袄毛  允  侬  互｝    贿挚卅及井壑钗挚卅及井毛    
 * 娄醒     允月  侬  ［醒侬毛  允  侬  动陆反殖氏匹反中仃卅中［
 * 忒曰袄     贿挚卅日ㄠ｝壑钗挚卅日ㄟ
 *    str 互"一亿四千万。"及午五｛忒曰袄1
 *    str 互"123"及午五｛忒曰袄ㄟ
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
 * 蜗褡芊匹醒袄毛  仄凶  侬  毛int卞  晶
 * char *str   侬  
 * 忒曰袄   侬  互  允醒袄
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
 *     挚匹  仄凶醒袄毛帮醒卞  晶
 * char *str 醒袄毛  允  侬  
 * 忒曰袄 醒袄
 *   
 * str互"一亿4千万？"及午五忒曰袄 140000000
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
 *   侬  及    卞踏井木化中月帮醒毛必永玄允月［
 * NPC及  平旦玄荸  卞支仁分勾［
 * 凶分仄    卞中日氏  侬互丐月桦宁  骰允月［
 * 耨痊互  切卅中失伙打伉术丞卞  凳［
 * 娄醒
 *  hoge    char*   荸  允月  侬  
 * 忒曰袄
 *  荸  仄凶帮醒［
 *
 *   “
 * "ringo ( 吧噗吧噗星人 ) ： 100"
 *      及桦宁反100互井尹月［
 * "ringo ( 吧噗吧噗星人 ) ： 是100"
 *      手100卞卅月［幻氏引卞NPC迕卅曰友［
 ----------------------------------------*/
int texttoiTail( char *hoge)
{
    return 0; /* 壬什匀化凶及匹仇丹仄凶［公及丹切楮醒仍午壅允［ */


#if 0
    int len = strlen(hoge);

    if( len == 0 )return atoi(hoge);
    /*    井日茧仄化  赓卞醒侬毛苇勾仃月    */
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
 *   侬  毛醒侬井升丹井民尼永弁仄化｝醒侬分匀凶日
 *  娄醒卞  木化忒允
 * 娄醒
 *      arg               晶葭
 *      number            晶  毛医  允月赭
 *      base            湘醒
 *      type            娄醒及滇( CTYPE 反 common.h 卞烛蜕今木化中月 )
 * 忒曰袄
 *      TRUE(1)    岳  
 *      FALSE(0)   strtol互敦僬卞反岳  仄卅井匀凶
 *                  仇及凛及number反strtol及忒曰袄匹丐月
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
         * 酸曰  侬互丐月午中丹仪反  晶仄五木化卅中午中丹仪卅及分［
         */
        return FALSE;

    return TRUE;

}




/*----------------------------------------
 * src 井日 dels 匹隙烂仄凶  侬毛潸曰轮仁
 * 田永弁旦仿永扑亘反巨旦弗□皿匹丐月仪卞镗啦［
 * 勾引曰田永弁旦仿永扑亘反绰轮匹五卅中［
 * 娄醒
 *  src      凳今木月  侬  
 *  char*  绰轮允月  侬(  侬  匹隙烂第)
 * 忒曰袄
 *  卅仄
 ----------------------------------------*/
void deleteCharFromString( char* src , char* dels )
{
    int index=0;    /* 匹五丐互曰及  侬  匹及 index */
    int delength;   /* dels 及赢今毛涩烂允月(剂仄反簧蘸失永皿及啃 */
    int i=0,j;/* i 反 src 毛伙□皿允月  醒 j 反 dels 毛伙□皿允月  醒 */

    delength= strlen( dels );

    while( src[i] != '\0' ){
        if( src[i] == BACKSLASH  )
            /*
             * 田永弁旦仿永扑亘卅及匹巨旦弗□皿允月
             * 勾引曰戚及  侬反  椭瘀卞啖  允月
             */
            src[index++] = src[++i];         /* substitute next char */

        else{
            for( j = 0 ; j < delength ; j ++ ){
                if( src[i] == dels[j] )
                    /*
                     * 绰轮允月  侬分［方匀化 i 卞 1 毛凶允
                     */
                    goto incrementi;
            }
            /*
             * 壅允  卞隙烂今木化中卅中及匹戊疋□允月［
             */
            src[index++] =  src[i];
        }
    incrementi:
        i++;
    }

    src[index] = '\0';
}



/*----------------------------------------
 * src 井日 dels 匹隙烂仄凶  侬毛潸曰轮仁
 * 巨旦弗□皿反卅中
 * 娄醒
 *  src      凳今木月  侬  
 *  char*  绰轮允月  侬(  侬  匹隙烂第)
 * 忒曰袄
 *  卅仄
 ----------------------------------------*/
void deleteCharFromStringNoEscape( char* src , char* dels )
{
    int index=0;    /* 匹五丐互曰及  侬  匹及 index */
    int delength;   /* dels 及赢今毛涩烂允月(剂仄反簧蘸失永皿及啃 */
    int i=0,j;/* i 反 src 毛伙□皿允月  醒 j 反 dels 毛伙□皿允月  醒 */

    delength= strlen( dels );

    while( src[i] != '\0' ){
        for( j = 0 ; j < delength ; j ++ ){
            if( src[i] == dels[j] )
                /*
                 * 绰轮允月  侬分［方匀化 i 卞 1 毛凶允
                 */
                goto incrementi;
        }
        /*
         * 壅允  卞隙烂今木化中卅中及匹戊疋□允月［
         */
        src[index++] =  src[i];
    incrementi:
        i++;
    }

    src[index] = '\0';
}

/*------------------------------------------------------------
 * 隙烂今木凶  侬毛隙烂今木凶  侬匹  五井尹月
 * 娄醒
 *  src     char*         凳今木月  侬  
 *  oldc    char          凳今木月  侬
 *  newc    char          凳允月  侬
 * 忒曰袄
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
 * makeEscapeString匹综岳今木凶  侬  毛葭卞  允
 * 娄醒
 *  src             char*       葭卞卅月  侬  ［仇木互踏五赘月［
 * 忒曰袄
 *  src    毛忒允［(楮醒毛  木仇卞仄支允中方丹卞)
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
                // 戚及  侬卞垫仁
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

        //   侬  互卅仁卅月引匹腹绸
        for( ;src[0] != '\0'; src ++ ){
             // 蝈剩及ㄠ田奶玄  井升丹井毛民尼永弁
          if( IS_2BYTEWORD( src[0] ) ){
              // 蝈剩分［公及桦宁反ㄠ田奶玄芴坌卞褡引六月［
              // 凶分仄ㄠ田奶玄仄井卅中桦宁反公丹仄卅中
              if( src[1] != 0 ){
                  src ++;
              }
              continue;
          }
          //   剩分匀凶［仇仇匹覆擂及  侬午  胜
          if( src[0] == delim ){
              return src;
          }
        }
        // 伙□皿  仃凶日苇勾井日卅井匀凶［
        return NULL;
}
                                                                                                                                                                                                                                                                                                                


/*----------------------------------------
 * delim 匹隙烂今木凶  侬  毛嗉濠曰  侬午仄化
 * index     及  毛  月［index反1匹反元引月［
 *   侬  酷  ［
 * 娄醒
 *  src     葭及  侬  
 *  delim   犯伉立正午卅月  侬  ［    反NULL  侬匹蔽匀化中月仪
 *  index   窒    及  毛潸月井
 *  buf     瑛绊  侬  毛伞  允月桦赭尺及禾奶件正□
 *  buflen  瑛绊  侬  毛伞  允月桦赭及扔奶术
 * 忒曰袄
 *  丐匀凶日 TRUE(1);
 *  卅井匀凶日 FALSE(0);
 *  ex
 *      getStringFromIndexWithDelim( "aho=hoge","=",1,buf,sizeof(buf) );
 *      buf ... aho
 *
 *      getStringFromIndexWithDelim( "aho=hoge","=",2,buf,sizeof(buf) );
 *      buf ... hoge
 *      仇木手  户日木月［
 *
 *      getStringFromIndexWithDelim( "aho=hoge","=",3,buf,sizeof(buf) );
 *      忒曰袄 FALSE
 ----------------------------------------*/
BOOL getStringFromIndexWithDelim_body( char* src ,char* delim ,int index,
                                 char* buf , int buflen ,
                                       char *file, int line )
{//ttom this function all change,copy from the second
    int i;          /* 伙□皿  醒 */
    int length =0;  /* 潸曰请仄凶  侬  及赢今 */
    int addlen=0;   /* 箫今木月赢今 */
    int oneByteMode = 0; /* ㄠ田奶玄乒□玉井＂ */

    if( strlen( delim ) == 1 ){ // 腹绸互ㄠ田奶玄卅日ㄠ田奶玄乒□玉卞允月
        oneByteMode = 1;// 公及端ㄡ田奶玄  侬反民尼永弁仄卅中
    }
    for( i =  0 ; i < index ; i ++ ){
         char* last;
         src += addlen;/* 心勾井匀凶赢今毛箫允 */
      
         if( oneByteMode ){
             // ㄠ田奶玄乒□玉分匀凶日仇切日匹腹绸
             last = ScanOneByte( src, delim[0] );
         }else{
                 last  = strstr( src , delim );  /* 苇尥仃月 */
         }
         if( last == NULL ){
            /*
             * 心勾井日卅井匀凶及匹允屯化戊疋□仄化 return［
            */
            strcpysafe( buf , buflen, src );

            if( i == index - 1 )
                /*切斤丹升心勾井匀凶*/
                return TRUE;
                                                                                                           
                /*心勾井日卅井匀凶*/
             return FALSE;
          }
          
          /*
           * 心勾井匀凶赭午  赓及匏  及犒毛菲户月
           * 勾引曰嗉濠日木化中月  侬  及赢今
          */
          length = last - src;
                                           
          /*
           * 戚及伙□皿及啃卞心勾井匀凶赢今午 delim 及赢今毛箫仄化云仁
          */
          addlen= length + strlen( delim );
       }
       strncpysafe( buf, buflen , src,length );

       return TRUE;
}


/*------------------------------------------------------------
 *   侬   "a,b,c,d" 毛娄醒卞医  允月［犯白巧伙玄反 0 卞允月［
 * 井卅曰及  溃楮醒［蜇箕反切斤匀午赞中［
 * 娄醒
 *  src         char*   葭及  侬  
 *  int1        int*    int及禾奶件正［(a毛医  允月)
 *  int2        int*    int及禾奶件正［(b毛医  允月)
 *  int3        int*    int及禾奶件正［(c毛医  允月)
 *  int4        int*    int及禾奶件正［(d毛医  允月)
 * 忒曰袄
 *  卅仄
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
 * src 及  匹dels匹隙烂仄凶  侬互  粮仄化中凶日
 * 1勾卞引午户月［
 * 田永弁旦仿永扑亘反巨旦弗□皿扑□弗件旦午卅月［
 * 仄凶互匀化  粮仄凶田永弁旦仿永扑亘毛壅允仪反匹五卅中［
 * 娄醒
 *  src      祭允月  侬  
 *  dels   壅允  侬(  侬  匹隙烂第)
 * 忒曰袄
 *  卅仄
 ---------------------------------------------*/
void deleteSequentChar( char* src , char* dels )
{
    int length;         /* src 及赢今 */
    int delength;       /* dels 及赢今 */
    int i,j;            /* 伙□皿  醒 */
    int index=0;        /* 割烂  侬  及 index */
    char backchar='\0';   /* 绰轮允月  侬 */

    length = strlen( src );
    delength = strlen( dels );

    /*
     * 伙□皿允月仍午卞戊疋□允月
     */
    for( i = 0 ; i < length ; i ++ ){
        if( src[i] == BACKSLASH ){
            /*
             * 田永弁旦仿永扑亘分匀凶日
             * 雄孺读卞  勾( BACKSLASH愤褥午｝戚及  侬 )
             * 褡户月
             */
            src[index++]=src[i++];
            if( i >= length ){
                print( "*\n");
                break;
            }
            src[index++]=src[i];
            /*   粮互鞅匀凶仪卞允月 */
            backchar = '\0';
            continue;
        }
        if( src[i] == backchar )
            /*
             * 绰轮允月手元分匀凶日窒手仄卅中［勾引曰绰轮卞卅月［
             */
            continue;

        /*
         *   粮及绰轮允月  侬互卅仁卅匀凶及匹 backchar 反 NULL   侬卞允月
         * 仇木互井氏元氏
         */
        backchar = '\0';
        for( j = 0 ; j < delength ; j ++ ){
            if( src[i] == dels[j] ){
                /*
                 * 绰轮允月  侬  分［ 仇仇匹 continue 仄卅中仪卞方曰
                 * 域  侬反绰轮仄卅中仪互忡据匹五月［
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
 * hash袄毛忒允［  侬  卞及心银丹仇午
 * 仇木反  公及引引卅及匹失伙打伉术丞反方丹歹井日氏互
 * 中中失伙打伉术丞日仄中［
 * 娄醒
 *  s     侬  
 * 忒曰袄
 *  int 甩永扑亘袄
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
 * 夫□市伙卞田奶件玉仄凶末弗永玄毛忒允( TCP )
 * 娄醒
 *          port        夫□市伙禾□玄
 * 忒曰袄
 *          -1      巨仿□
 *                      1. socket扑旦  丞戊□伙及巨仿□
 *                      2. bind扑旦  丞戊□伙及巨仿□
 *                      3. listen扑旦  丞戊□伙及巨仿□
 ----------------------------------------*/
int bindlocalhost( int port )
{
    struct sockaddr_in sin;         /*夫□市伙失玉伊旦迕*/
    int sfd;                        /*末弗永玄犯奴旦弁伉皿正*/
    int rc;                         /*夫□市伙伉正□件戊□玉*/

    /*AF_INET皿夫玄戊伙 及 SOCK_STREAM */
    sfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sfd == -1 ){
        print( "%s\n" , strerror(errno ) );
        return -1;
    }
    
    if( getReuseaddr() ) {
		int sendbuff;
	    /* 夫□市伙失玉伊旦及疯厍迕 */
		setsockopt( sfd, SOL_SOCKET, SO_REUSEADDR, 
						(char *)&sendbuff, sizeof( sendbuff));
	}
	
    /*0弁伉失仄化袄毛医  */
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
 * 戊生弁玄允月［( TCP )
 * 娄醒
 *      hostname    涛粮允月石旦玄  
 *                      (xx.xxx.xxx.xxx匹手中中仄xxx.co.jp午井匹手中中)
 *      port        涛粮允月石旦玄及禾□玄  
 * 忒曰袄
 *      犯奴旦弁伉皿正( -1 及凛反巨仿□ )
 ----------------------------------------*/
int connectHost( char* hostname , unsigned short port )
{
    struct sockaddr_in sock;        /*connect及凛卞银丹*/
    struct hostent* hoste;          /*hostname及hostent*/
    int     fd;                     /*戊生弁玄仄凶末弗永玄犯奴旦弁伉皿正*/
    int     lr;                     /*夫□市伙伉正□件戊□玉*/

    memset( &sock ,  0 , sizeof( struct sockaddr_in ) );
    sock.sin_family = AF_INET;
    sock.sin_port = htons( port );

    /* dot notation 井升丹井毛譬屯月 */
    sock.sin_addr.s_addr = inet_addr( hostname );
    if( sock.sin_addr.s_addr == -1 ){
        /*
         *巨仿□ dot notation 匹反卅中［仄凶互匀化  骚及  蟆荸瑁互  邰分［
         * dns毛夫井卅仃木壬卅日卅中凛反夫五卞中仁及手仇仇匹支匀化仁木月［
         */
        hoste = gethostbyname( hostname );
        if( hoste == NULL ){
            print( "获取主机名: %s\n", hostname);
            return -1;
        }

        memcpy((void*)&sock.sin_addr.s_addr ,
               hoste->h_addr , sizeof(struct in_addr) );
    }

    /*末弗永玄毛综月*/
    fd = socket( AF_INET, SOCK_STREAM , 0 );
    if( fd == -1 ){
        print("Cannot Create Socket(%s errno:%d)\n"
              ,strerror( errno ) ,errno );
        return -1;
    }
    /*戊生弁玄允月*/
    lr = connect(fd,(struct sockaddr*)&sock,sizeof(struct sockaddr_in));
    if( lr != 0 ){
        print("Cannot connect. (%s errno:%d)\n"
              ,strerror( errno ) ,errno );
        return -1;
    }

    return fd;
}


/*----------------------------------------
 * 田永白央及  卞荼垫筏寞互丐月井升丹井［
 * 剂仄镀中失伙打伉术丞卞  凳［
 * 娄醒
 *  char*   譬屯月  侬  
 * 忒曰袄
 *    赓卞苇勾井凶荼垫筏寞及匏       失弁本旦仄凶凛卞中中袄［
 *  升丹中丹仪井午中丹午｝
 *      existsNewLinwCharacter( "aho\nhoge\n" )
 *  分午｝忒曰袄反 3 卞卅月［仇木卞镗啦允月仪［
 *  -1      卅井匀凶
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
    int i;                          /*伙□皿  醒*/
    int length = strlen( line );    /*譬屯月  侬  及赢今*/
    for( i = 0 ; i < length ; i ++ ){
        if( line[i] == NEWLINE )
            return i;
    }
#endif

    return -1;
}



/*----------------------------------------
 *    侬    卞  侬互  歹木月桦赭毛井尹允［窒    井毛瑁户木月［
 *  nindex( string , c , 1 ) 午 index( string , c ) 反 equivalent 匹丐月［
 *  number互  引凶反0及凛反忒曰袄反｝string午  元匹丐月［
 *
 *  娄醒
 *      string  char*   譬屯月  侬  
 *      c       int     譬屯月  侬
 *      number  int       歹月  寞
 *  忒曰袄
 *      域谯仄凶  侬及禾奶件正□
 *      NULL    心勾井日卅井匀凶［
 ----------------------------------------*/
char* nindex( char* string, int c , int number )
{
    int i;                          /*伙□皿  醒*/
    int num=0;
    int length = strlen( string );  /*譬屯月  侬  及赢今*/
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

        /*  . 匹铵引月白央奶伙反  引卅中    */
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
 * 瞬滇匹  坌鳔毛手午户月［ by ringo
 * 蔷及啖  迕
 *
 * double val1 , val2 : 仇及袄及丐中分毛午月
 * double d :   坌  
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
 *   侬  及卅井井日  啦卅  侬毛len  侬午曰分仄化仿件母丞卅
 *   侬  毛勾仁月［NPC及仿件母丞由旦伐□玉戏岳卞勾井匀化月
 *
 * char *cand : 仇仇井日蓟少［跳  及  侬［
 * char *out : 请  田永白央
 * int len : 请  及赢今
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
 * 娄醒卞隙烂今木凶白央奶伙互白央奶伙井升丹毛譬屯月
 * 娄醒
 *  filename        char*        白央奶伙  
 * 忒曰袄
 *  白央奶伙        TRUE(1)
 *  白央奶伙匹卅中  FALSE(0)
 ------------------------------------------------------------*/
BOOL isExistFile( char* filename )
{
    /*  fopen 匹譬屯月  */
    FILE*   fp;
    fp = fopen( filename, "w" );
    if( fp ){
        fclose( fp );
        return TRUE;
    }else
        return FALSE;
}
/*------------------------------------------------------------
 * 10褡  62褡  晶
 * 娄醒
 * a        int     葭及ㄠㄟ褡
 * out      char *  伞  田永白央□
 * outlen   int     伞  田永白央及  五今
 *   曰袄
 *          岳      out及失玉伊旦
 *          撩      NULL
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
/* 衙中   */
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
 * 芨尹日木凶    及  匹］醒袄互褐  仄化中月井譬屯月楮醒
 * -1 反覆擂陆午中丹  卅楮醒
 *
 * 娄醒
 * src		*int		譬屯月葭及    
 * srclen	int			src 及  五今
 *
 *   曰袄   褐  丐曰	TRUE
 *				卅仄	FALSE
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


static unsigned char BitTable[] =	/*	申永玄及岈太赐毛菅卞允月  □皮伙	*/
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
static unsigned short crctab16[] =	/*	crc｛及煌遥  □皮伙		*/
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
