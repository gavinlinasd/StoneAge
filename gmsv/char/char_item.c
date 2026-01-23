#include "version.h"
#include <stdio.h>

#include "readmap.h"
#include "map_deal.h"
#include "object.h"
#include "char.h"
#include "char_base.h"
#include "item.h"
#include "lssproto_serv.h"
#include "handletime.h"
#include "log.h"
#include "item_event.h"
#include "battle.h"
#include "petmail.h"
#ifdef _MARKET_TRADE
#include "item_trade.h"
#endif
#ifdef _DROPSTAKENEW
#define CASINOTOKENID				20121			// 蚔氈部粗測瘍
#define MAXCASINOMAPNUM				150			// 蚔氈部華芞奻癹
#define MAXACCUMULATION				1000000		// 儅煦奻癹
typedef struct tagCasinoMapTable
{
	int casinofl;				// 蚔氈部芞脯
	int casinox;				// 蚔氈部