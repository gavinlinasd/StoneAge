#ifndef __TRADE_H__
#define __TRADE_H__
#include "version.h"
#include "common.h"
#include "util.h"
#include "net.h"
 
BOOL TRADE_Search(int fd, int meindex, char* message);
    
void CHAR_Trade(int fd, int index, char* message);

#ifdef _ITEM_PILEFORTRADE
void TRADE_InitTradeList( void);
#endif

void TRADE_CheckTradeListUser( void);

#endif 
