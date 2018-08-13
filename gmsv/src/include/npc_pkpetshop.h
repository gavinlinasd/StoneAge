//krynn 2001/12/6
//PKPetShop

#ifndef __NPC_PKPetShop_H__
#define __NPC_PKPetShop_H__

void NPC_PKPetShopTalked( int meindex , int talkerindex , char *msg , int color );
BOOL NPC_PKPetShopInit( int meindex );
void NPC_PKPetShopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_PKPetShopLooked( int meindex , int lookedindex);

#endif 


//krynn end

