#ifndef __MAPTRADE_H__
#define __MAPTRADE_H__
#include "char.h"

typedef struct tagMapTrade	{
	int masterindex;
	int x;
	int y;
	int Ttime;
	int Goodindex;
	char Goodname[256];
}MapTrade;

#define TRADEMAP 1090
#define TRADEXSIZE 4
#define TRADEYSIZE 4

BOOL CHECKMAP_TRADE( int charindex, int floor, int x, int y);
int CHECKMAP_TRADEXY( int charindex, int floor, int x, int y);
BOOL MAP_TRADEDROP( int charindex, int itemindex,int floor, int x, int y);
BOOL MAP_TRADEPICKUP( int charindex, int itemindex, int floor, int x, int y, int flg);

void InitMapTradeData( int index, int Stime);

int TRADE_getMasterInt( int index);
int TRADE_getTimeInt( int index);
BOOL TRADE_setMasterInt( int index, int Num);
BOOL TRADE_setTimeInt( int index, int Num);
int TRADE_AddMasrerTrade( int toindex);	//设定摊位主人
int TRADE_getMaxNumInt();
void MAPTRADE_CLEANGOLD( int floor, int num);
BOOL MAPTRADE_CHECKMASTERIN( int masterindex , int toindex, int num);
BOOL MAPTRADE_CHECKMAPFULL(int fl, int x, int y);
BOOL MAP_TRADEPETDROP( int charindex, int petindex,int floor, int x, int y);
int MAPTRADE_getItemSpace( int meindex, int itemindex);
int MAPTRADE_getPetSpace( int masterindex, int petindex);

int MAPTRADE_getSellIndex( int index);
BOOL MAPTRADE_setSellIndex( int index, int num);
void MAPTRADE_setCharSellName( int index, char *buf);
char* MAPTRADE_getCharSellName( int index );

#define TRADESTARTNUM 1
#define TRADETYPE_SELL (1<<3)
#define TRADEITEMID 0
#define TRADEPETID 0
#define TRADEPETUPLV ((1<<24)+(1<<16)+(1<<8)+1)
#define TRADEPETTYPE (1<<4)
#define TRADEITEMTYPE (1<<8)
#endif


