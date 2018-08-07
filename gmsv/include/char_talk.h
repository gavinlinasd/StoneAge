#ifndef __CHAR_TALK_H__
#define __CHAR_TALK_H__

#include "common.h"
#include "util.h"
#include "net.h"

  
void CHAR_getMessageBody(char* message, char* kind, int kindlen,
            char** body);

#ifdef _GM_SIGUSR2
    typedef void (*CHATMAGICFUNC2)(int,char*);//功能相同typedef void (*CHATMAGICFUNC)(int,char*);
    CHATMAGICFUNC2 gm_CHAR_getChatMagicFuncPointer(char* name,BOOL isDebug);
#endif

int CHAR_getChatMagicFuncLevel(char* name,BOOL isDebug);
int CHAR_getChatMagicFuncNameAndString( int ti, char* name, char *usestring, int level, BOOL isDebug);
int CHAR_getChatMagicFuncMaxNum( void);

void OneByOneTkChannel ( int fd , char *tmp1 , char *tmp2 , int color) ; 


#endif 
