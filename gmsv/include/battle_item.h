#ifndef _BATTLE_ITEM_H_
#define _BATTLE_ITEM_H_
//--------------------------------------------------------------
//  樊蜾囮騷  堜禱窅埱倜鳹譴摯窐  
//--------------------------------------------------------------
// 橋    摯鳹譴
void ITEM_useRecovery_Battle( int charaindex, int toindex, int itemindex );
void ITEM_useStatusChange_Battle( int charaindex, int toindex, int itemindex );
void ITEM_useMagicDef_Battle( int charaindex, int toindex, int itemindex );
void ITEM_useParamChange_Battle( int charaindex, int toindex, int itemindex );
void ITEM_useFieldChange_Battle( int charaindex, int toindex, int itemindex );
void ITEM_useAttReverse_Battle( int charaindex, int toindex, int itemindex );
void ITEM_useStatusRecovery_Battle( int charaindex, int toindex, int itemindex );
void ITEM_useCaptureUp_Battle( int charaindex, int toindex, int itemindex );
#ifdef _ITEM_CRACKER
void ITEM_useCracker_Effect( int charaindex, int toindex, int haveitemindex);
#endif
#ifdef _ITEM_ADDEXP	//vincent 冪桄枑汔
void ITEM_useAddexp_Effect( int charaindex, int toindex, int haveitemindex);
#endif
#ifdef _ITEM_REFRESH //vincent 賤壺祑都袨怓耋撿
void ITEM_useRefresh_Effect( int charaindex, int toindex, int haveitemindex);
#endif
//Terry add 2001/12/24
#ifdef _ITEM_FIRECRACKER
void ITEM_useFirecracker_Battle(int charaindex,int toindex,int itemindex);
#endif
//Terry end

#ifdef _ITEM_MAGICRECOVERY
void ITEM_useMRecovery_Battle( int charaindex, int toNo, int haveitemindex );
#endif
#ifdef _ITEM_USEMAGIC
void ITEM_useMagic_Battle( int charaindex, int toNo, int haveitemindex );
#endif
#endif
