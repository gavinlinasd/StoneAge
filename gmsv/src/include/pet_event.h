#ifndef __PET_EVENT_H__
#define __PET_EVENT_H__

/*
 * 矢永玄楮  及涩烂匹允［
 * 矢永玄奶矛件玄楮  
 */

void PET_Talkfunc( int meindex, int talkerindex, char *msg, int color);

void PET_Watchfunc( int objmeindex, int objmoveindex,
                           CHAR_ACTION act, int x, int y, int dir,
                           int* opt, int optlen );
int PET_CleanPetdeletetime( int objmeindex);
void PET_CleanFreePetAll();


#endif
