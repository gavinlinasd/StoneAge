#define _MAIL_C_

#include "main.h"
#include "mail.h"
#include "util.h"
#include "saacproto_serv.h"

#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

typedef enum
{
    MS_NOUSE = 0,
    MS_NEWMESSAGE,
    MS_WAIT_ACK,
} MAILSTATE;

#define TEXT_MAX 1024
struct mail
{
    int use;
    unsigned int id_charname_hash;
    char id_to[USERID_MAX];
    char charname_to[CHARNAME_MAX];
    char id_from[USERID_MAX];
    char charname_from[CHARNAME_MAX];
    char text[TEXT_MAX];
    int option;
    unsigned int message_id;
    MAILSTATE state;
    time_t recv_time;
};

struct mail *mailbuf;
int mailbufsize = 0;


static unsigned int
getNextMessageID(void)
{
    FILE *fp;
    unsigned int i;
    char filename[1024];
    char line[1000];
    snprintf( filename, sizeof( filename ),
              "%s/mail_id" , maildir );
    fp = fopen( filename, "r" );
    if( fp == NULL ){
        fp = fopen( filename ,"w" );
        if( fp == NULL ){
            log( "祥夔斐膘 %s ... 妏蚚肮欴腔蚘璃ID,"
                 " saac 楷冞曹腕遣鞣!(id:9999)\n", filename );
            return 9999;
        }
        fprintf( fp, "10000\n" );
        fclose(fp);
        return 1000;
    }
    fgets( line, sizeof(line), fp);
    i = strtoul( line, NULL, 10 );
    fclose(fp);

    fp = fopen( filename, "w" );
    if( fp == NULL ){
        log( "祥夔迡�遶繕鯧D善 %s ... 妏蚚肮欴腔杅趼!\n", filename );
        return i;
    }
    fprintf( fp, "%u", i+1 );
    fclose(fp);

    log( "陔蚘璃ID:%u\n", i);
    return i;
}

static int reallocMailBuf( void )
{
    struct mail *previous = mailbuf;
    struct mail *newbuf;
    int new_mailbufsize;
    if( mailbufsize == 0 ){
        new_mailbufsize = 1;
    } else {
        new_mailbufsize = mailbufsize * 2;
    }

    newbuf = ( struct mail * )calloc( 1, new_mailbufsize *
                                      sizeof( struct mail ));
    if( newbuf == NULL ){
        log( "隙葩蚘璃遣喳: 囀璃祥逋!! 陔蚘璃湮苤:%d\n",
             new_mailbufsize );
        return -1;
    }
    memset( newbuf, 0 , new_mailbufsize * sizeof( struct mail ));
    if( previous ) memcpy( (char*)newbuf, (char*)previous,
                           mailbufsize * sizeof( struct mail ));
    free( previous );
    mailbufsize = new_mailbufsize;
    mailbuf = newbuf;

    log( "笭陔煦饜蚘璃遣喳: "
         "陔蚘璃遣喳:%d 導華硊:%x 陔華硊:%x\n",
         new_mailbufsize, (unsigned int)previous,(unsigned int)newbuf );
    return 0;
}

static int mailbuf_finder = 0;
static int allocMail( int use_msgid, unsigned int msgid  )
{
    int i;
    for(i=0;i<mailbufsize;i++){
        mailbuf_finder ++;
        if( mailbuf_finder == mailbufsize ) mailbuf_finder = 0;
        if( mailbuf[mailbuf_finder].use == 0 ){
            mailbuf[mailbuf_finder].use = 1;
            mailbuf[mailbuf_finder].text[0] = 0;
            mailbuf[mailbuf_finder].id_to[0] = 0;
            mailbuf[mailbuf_finder].charname_to[0] = 0;
            mailbuf[mailbuf_finder].id_from[0] = 0;
            mailbuf[mailbuf_finder].charname_from[0] = 0;
            if( use_msgid ){
                mailbuf[mailbuf_finder].message_id = msgid;
            } else {
                mailbuf[mailbuf_finder].message_id = getNextMessageID();
            }
            mailbuf[mailbuf_finder].state = MS_NEWMESSAGE;
            mailbuf[mailbuf_finder].recv_time = time(NULL);
            return mailbuf_finder;
        }
    }
    log( "煦饜蚘璃: 蚘璃遣喳囮啖.淏婓輛俴煦饜...\n" );
    if( reallocMailBuf() < 0 ){
        log( "煦饜蚘璃: 煦饜囮啖\n" );
    } else {
        return allocMail(use_msgid, msgid );
    }

    return -1;
}

void receiveMail( char *id_from,
             char *charname_from,
             char *id_to,
             char *charname_to,
             char *message,
             int option,
             int use_msgid,
             unsigned int msgid 
             )
{
    char id_charname[1024];
    int h, mbindex;

    snprintf( id_charname, sizeof( id_charname), "%s_%s", id_to, charname_to );
    h = hashpjw( id_charname ) & 0xff ;
    if( (mbindex = allocMail(use_msgid, msgid )) < 0 ){
        log( "隙葩蚘璃: 鳳�－繕鼯宎�遣喳囮啖.\n" );
        return;
    }
    mailbuf[mbindex].id_charname_hash = h;
    snprintf( mailbuf[mbindex].text, sizeof( mailbuf[mbindex].text ), "%s", message );
    snprintf( mailbuf[mbindex].id_to, sizeof( mailbuf[mbindex].id_to ), "%s", id_to );
    snprintf( mailbuf[mbindex].charname_to, sizeof( mailbuf[mbindex].charname_to ), "%s", charname_to );
    snprintf( mailbuf[mbindex].id_from, sizeof( mailbuf[mbindex].id_from ),"%s", id_from );
    snprintf( mailbuf[mbindex].charname_from, sizeof( mailbuf[mbindex].charname_from ),"%s", charname_from );
    mailbuf[mbindex].option = option;

    {
        char childname[1000];
        char savefile[1000];
        char charname2[CHARNAME_MAX*2+1];
        char text2[TEXT_MAX*2+1];
        FILE *fp;
        
        snprintf( childname, sizeof( childname ),"%u" ,
                  mailbuf[mbindex].message_id );
        makeDirFilename( savefile , sizeof(savefile), maildir, h, childname );
        fp = fopen( savefile, "w" );
        if( fp == NULL ){
            log( "隙葩蚘璃 : 祥夔悵湔蚘璃恅璃: %s %s\n",
                 savefile, strerror( errno ));
            return;
        }
#define TO_ID_HEAD "ToID: "
#define TO_CHAR_HEAD "ToChar: "
#define FROM_ID_HEAD "FromID: "
#define FROM_CHAR_HEAD "FromChar: "
#define OPTION_HEAD "Option: "
#define TEXT_HEAD "Text: "
        
        fprintf( fp, TO_ID_HEAD "%s\n", id_to );
        snprintf( charname2, sizeof( charname2), "%s", charname_to );
        fprintf( fp, TO_CHAR_HEAD "%s\n", makeEscapeString( charname_to,
                                               charname2,sizeof(charname2)) );
        fprintf( fp, FROM_ID_HEAD "%s\n", id_from );
        snprintf( charname2, sizeof( charname2), "%s", charname_from );
        fprintf( fp, FROM_CHAR_HEAD "%s\n", makeEscapeString( charname_from,
                                               charname2,sizeof(charname2)) );
        fprintf( fp, OPTION_HEAD "%d\n", option );
        snprintf( text2, sizeof( text2 ) , "%s", message );
        fprintf( fp, TEXT_HEAD "%s\n", makeEscapeString( message,
                                               text2, sizeof(text2)) );
        fclose(fp);
		{
			static int mailnum=0;
			if( mailnum%10 == 0)
				log(".");
			mailnum = (++mailnum%1000);
			if( mailnum == 0 )
				log("\n");
		}
    }
    {
        extern gmsv gs[MAXCONNECTION];
        int i;
        for(i=0;i<MAXCONNECTION;i++){
            if( gs[i].use && gs[i].name[0] ){
                saacproto_Message_send( i,
                                        id_from, charname_from,
                                        id_to, charname_to,
                                        message, option,
                                        mailbuf[mbindex].message_id );
                mailbuf[mbindex].state = MS_WAIT_ACK;
            }
        }
    }
}

void receiveMailAck( char *id, char *charname, int a , int mesgid )
{
    int i;
    unsigned int h;
    char id_charname[1024];
    snprintf( id_charname, sizeof( id_charname), "%s_%s", id, charname );    
    h = hashpjw( id_charname ) & 0xff ;
    for(i=0;i<mailbufsize;i++){
        if( mailbuf[i].message_id == mesgid ){
            if( mailbuf[i].use
                && mailbuf[i].id_charname_hash == h
                && strcmp( mailbuf[i].id_to, id ) == 0
                && strcmp( mailbuf[i].charname_to, charname ) == 0
                && mailbuf[i].state == MS_WAIT_ACK ){
                char savefile[1024];
                char childname[1000];
                snprintf( childname,sizeof(childname),"%u",
                          mailbuf[i].message_id );
                makeDirFilename( savefile, sizeof(savefile),maildir,
                                 h, childname );
                if( unlink( savefile ) != 0 ){
                    log( "failed to unlink %s: %s\n",
                         savefile, strerror(errno ));
                } else {
                    log( "隙葩蚘璃: 刉壺蚘璃 "
                         "%u 植 %s(%s) to %s(%s)\n",
                         mailbuf[i].message_id,
                         mailbuf[i].id_from,
                         mailbuf[i].charname_from,
                         mailbuf[i].id_to,
                         mailbuf[i].charname_to );
                }
                memset( &mailbuf[i], 0 , sizeof( mailbuf[0] ));
                return;
            } else {
                log( "隙葩蚘璃: 拸蚚陓洘麼ID麼靡備麼汒隴旆笭渣昫" );
                log( "use[%d] h[%d][%d] id[%s][%s] nm[%s][%s] st[%d]\n",
                     mailbuf[i].use, mailbuf[i].id_charname_hash, h,
                     mailbuf[i].id_to, id,
                     mailbuf[i].charname_to, charname,
                     mailbuf[i].state);
            }
        }
    }
    log( "隙葩蚘璃: 蚘璃 %u 祥夔植 %s(%s) 梑善\n",
         mesgid, id,charname );
}

void flushMail( int fd,
           char *id,
           char *charname )
{
    int i,c=0;
    unsigned int h ;
    char id_charname[1000];
#define MAX_FLUSH_MAIL 1024
    int flush_index[MAX_FLUSH_MAIL];
    int flush_i=0;
    snprintf( id_charname, sizeof( id_charname ) , "%s_%s", id, charname );
    h = hashpjw( id_charname );
    // Nuke +1
    log("蚘璃遣喳湮苤:%d (%s)\n",mailbufsize,chartime());
    // Nuke *1
    for(i=0;(i<mailbufsize)&&(i<MAX_FLUSH_MAIL);i++){
        if( mailbuf[i].id_charname_hash == h &&
            mailbuf[i].use &&
            strcmp( mailbuf[i].id_to , id ) == 0 &&
            strcmp( mailbuf[i].charname_to, charname ) == 0 ){
            log( "秏洘ID:%u\n", mailbuf[i].message_id );
            flush_index[flush_i++] = i;
            c++;
        }
    }

    for(i=0; i< flush_i ; i++ ){
        int j;
        for(j=0; j < flush_i-1 ; j++ ){
            if( mailbuf[flush_index[j]].message_id >
                mailbuf[flush_index[j+1]].message_id ){
                int sw = flush_index[j];
                flush_index[j] = flush_index[j+1];
                flush_index[j+1] = sw;
                log( "inverted %d and %d in %d \n", flush_index[j],
                     flush_index[j+1], j );
            }
        }
    }

    /*   傷勗邞嚭埰堎 */
    for(i=0;i< flush_i; i++ ){
        /* flush 摯鳹譴毀��flush禱邞嚭媃趙拻倜斛↓堜�荂懽鵅麙�
           葡媃趙煦崹邞嚭埰躂�屼倗� */
        saacproto_Message_send( fd,
                                mailbuf[flush_index[i]].id_from,
                                mailbuf[flush_index[i]].charname_from,
                                mailbuf[flush_index[i]].id_to,
                                mailbuf[flush_index[i]].charname_to,
                                mailbuf[flush_index[i]].text,
                                mailbuf[flush_index[i]].option,
                                mailbuf[flush_index[i]].message_id );
        mailbuf[flush_index[i]].state = MS_WAIT_ACK;
        log( "煦濬蚘璃ID:%u\n",
                mailbuf[flush_index[i]].message_id );
    }
    // Nuke *1
    log( "蚘璃: 楷冞 %d 猾蚘璃善 %s(%s)(%s)\n", c, id, charname ,chartime());
}

// Nuke start: To expire undelivered mail
#define MAIL_EXPIRE_TIME 3600
void
expireMail()
{
    int i,c=0;
    unsigned int h ;
    char id_charname[1000];
    time_t now=time(NULL);
    
    /* 1樊摯flush ぁ邞嚭埰堎    倳 */
#define MAX_FLUSH_MAIL 1024
    int flush_index[MAX_FLUSH_MAIL];
    int flush_i=0;
    // Nuke +1
    log("蚘璃遣喳湮苤:%d (%s)\n",mailbufsize,chartime());
    // Nuke *1
    for(i=0;(i<mailbufsize)&&(i<MAX_FLUSH_MAIL);i++){
        if(mailbuf[i].use && (now - mailbuf[i].recv_time >= MAIL_EXPIRE_TIME)) {
            log( "秏洘ID:%u 眒徹ぶ\n", mailbuf[i].message_id );
            flush_index[flush_i++] = i;
            c++;
        }
    }

    /*   傷勗邞嚭埰堎 */
    for(i=0;i< flush_i; i++ ){
        
        snprintf( id_charname, sizeof( id_charname), "%s_%s", 
        	mailbuf[flush_index[i]].id_to, 
        	mailbuf[flush_index[i]].charname_to );
        h = hashpjw( id_charname ) & 0xff ;
	{
            char savefile[1024];
            char childname[1000];
            snprintf( childname,sizeof(childname),"%u",
            	mailbuf[flush_index[i]].message_id );
            makeDirFilename( savefile, sizeof(savefile),maildir,
            	h, childname );
            if( unlink( savefile ) != 0 ){
                log( "failed to unlink %s: %s\n",
                savefile, strerror(errno ));
            } else {
                log( "徹ぶ蚘璃: 刉壺蚔牁 "
                	"%u 植 %s(%s) 善 %s(%s)\n",
                        mailbuf[flush_index[i]].message_id,
                        mailbuf[flush_index[i]].id_from,
                        mailbuf[flush_index[i]].charname_from,
                        mailbuf[flush_index[i]].id_to,
                        mailbuf[flush_index[i]].charname_to );
            }
            memset( &mailbuf[flush_index[i]], 0 , sizeof( mailbuf[0] ));
	}
    }
    // Nuke *1
    log( "徹ぶ蚘璃: 徹ぶ %d 秏洘 (%s)\n", c ,chartime());
}

int readMail( char *dir )
{
    int i, read_count=0;
    for( i=0; i<256; i++){
        char dirname[1000];
        DIR *d;

        snprintf(dirname, sizeof( dirname ), "%s/0x%x", dir, i );
        d = opendir(dirname);
        if(d == NULL ){
			mkdir( dirname, 0755);
			log("斐膘 %s\n", dirname);
            continue;
        }
        while(1){
            struct dirent *de;        
            de = readdir( d );
            if( de == NULL )break;
            if( de->d_name[0] != '.' ){
                char filename[1000];
                FILE *fp;
                struct stat s;
                snprintf( filename, sizeof( filename),
                          "%s/%s", dirname, de->d_name );
                if( stat( filename, &s ) < 0 ){
                    continue;
                }
                if( !(s.st_mode & S_IFREG)) continue;
                fp = fopen( filename, "r" );
                if( fp == NULL ){
                    log( "祥夔湖羲恅璃 %s %s\n",filename,strerror(errno));
                    continue;
                }
                {
                    char line[16384];
                    char toid[1000] , fromid[1000];
                    char tochar[CHARNAME_MAX*2+1];
                    char fromchar[CHARNAME_MAX*2+1];
                    char text[TEXT_MAX*2+1];
                    int opt=0;
                    toid[0] = fromid[0] = tochar[0] =
                        fromchar[0] = text[0] = 0;
                    fgets( line, sizeof(line), fp );                    
                    chop(line);
                    if( strncmp( TO_ID_HEAD , line, strlen(TO_ID_HEAD) )==0){
                        snprintf( toid , sizeof( toid ),"%s",
                                  line+strlen(TO_ID_HEAD ));
                    }
                    fgets( line, sizeof( line ), fp );
                    chop(line);
                    if( strncmp( TO_CHAR_HEAD, line,strlen(TO_CHAR_HEAD))==0){
                        snprintf( tochar, sizeof( tochar ), "%s",
                                  line+strlen( TO_CHAR_HEAD ));
                        makeStringFromEscaped( tochar );
                    }
                    fgets( line, sizeof( line ),fp );
                    chop(line);                    
                    if( strncmp( FROM_ID_HEAD,line,strlen(FROM_ID_HEAD))==0){
                        snprintf( fromid, sizeof( fromid ),"%s",
                                  line + strlen( FROM_ID_HEAD ));
                    }
                    fgets( line,sizeof(line),fp);
                    chop(line);                    
                    if(strncmp(FROM_CHAR_HEAD,line,strlen(FROM_CHAR_HEAD))==0){
                        snprintf( fromchar, sizeof( fromchar ),
                                  line + strlen(FROM_CHAR_HEAD ));
                        makeStringFromEscaped( fromchar );
                    }
                    fgets( line,sizeof(line),fp);
                    chop(line);                    
                    if( strncmp( OPTION_HEAD,line, strlen(OPTION_HEAD))==0){
                        opt = atoi( line + strlen( OPTION_HEAD ) );
                    }
					fgets( line, sizeof( line),fp);
                    chop(line);                    
                    if( strncmp(TEXT_HEAD,line,strlen(TEXT_HEAD))==0){
                        snprintf( text, sizeof( text), "%s",
                                  line + strlen( TEXT_HEAD ));
                        makeStringFromEscaped( text );
                    }
                    if( toid[0] == 0 || fromid[0] == 0 ||
                        tochar[0] == 0 || fromchar[0] == 0 ||
                        text[0] == 0 ){
                        log( "衄恀枙蚘璃! %s 諉彶ID[%c] 諉彶靡趼[%c]"
                             " 楷冞ID[%c] 楷冞靡趼[%c] 恅掛[%c]\n",
                             filename,
                             toid[0], tochar[0], fromid[0], fromchar[0],
                             text[0] );
                        // Nuke +1 1027: Close for safe
                        fclose(fp);
                        continue;
                    }
                    receiveMail( fromid, fromchar,
                                 toid, tochar,
                                 text, opt , 1,
                                 strtoul(de->d_name,NULL,10));
                    read_count++;                    
                }
                fclose(fp);

            }
        }
        closedir(d);
    }
    log( "黍�±宎�: 婓'%s'醴翹爵黍�△� %d 猾蚘璃 \n", dir, read_count );
    return 0;
}
