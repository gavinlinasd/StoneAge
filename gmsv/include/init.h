#ifndef __INIT_H__
#define __INIT_H__
#include "common.h"
BOOL init( int argc , char** argv, char** env );

#ifdef _ITEM_QUITPARTY

typedef struct tagDisappearItem
{
	char string[64];	
}DisappearItem;
extern DisappearItem *Disappear_Item;
extern int itemquitparty_num;

#endif

#endif 
