#ifndef __NPC_SIMPLESHOP_H__
#define __NPC_SIMPLESHOP_H__

void NPC_SimpleShopTalked( int index, int talker, char *msg, int color );
void NPC_SimpleShopSpecialTalked( int index, int talker, char *msg, int color );
void NPC_LimitBuyInShopSpecialTalked( int index, int talker, char *msg, int color );
void NPC_SimpleShopInit( int meindex );

#endif 
 /*__NPC_SIMPLESHOP_H__*/
