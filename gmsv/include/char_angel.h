#ifndef _CHAR_ANGEL_H_
#define _CHAR_ANGEL_H_

#include "common.h"

/* angel/hero mission system (char/char_angel.c) */
int checkIfAngel( int charaindex);
char* getMissionNameInfo( int charaindex, char* nameinfo);
int AngelCreate( int angelindex);
void sendAngelCleanToCli( int fd);
void selectAngel( int charaindex, int heroindex, int mission, int gm_cmd);
void CHAR_sendAngelMark( int objindex, int flag);
void Use_AngelToken( int charaindex, int toindex, int haveitemindex );
void Use_HeroToken( int charaindex, int toindex, int haveitemindex );

/* defined in callfromcli.c */
BOOL CheckDropatLogout(int charaindex );

#endif
