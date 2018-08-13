#ifndef __MAGIC_H__
#define __MAGIC_H__

/* 热诸 */
int MAGIC_Use( int charaindex, int haveitemindex, int toindex);
int MAGIC_Recovery( int charaindex, int toindex,int marray, int mp );
int MAGIC_OtherRecovery( int charaindex, int toindex, int marray, int mp );
int	MAGIC_FieldAttChange( int charaindex, int toindex, int marray, int mp );
int	MAGIC_StatusChange( int charaindex, int toindex, int marray, int mp );
#ifdef _MAGIC_DEEPPOISON
int	MAGIC_StatusChange2( int charaindex, int toindex, int marray, int mp );
#endif
int	MAGIC_StatusRecovery( int charaindex, int toindex, int marray, int mp );
int	MAGIC_MagicDef( int charaindex, int toindex, int marray, int mp );
int	MAGIC_Ressurect( int charaindex, int toindex, int marray, int mp );
int	MAGIC_AttReverse( int charaindex, int toindex, int marray, int mp );
int	MAGIC_ResAndDef( int charaindex, int toindex, int marray, int mp );
#ifdef _OTHER_MAGICSTAUTS
int	MAGIC_MagicStatusChange( int charaindex, int toindex, int marray, int mp );
#endif
#ifdef __ATTACK_MAGIC
int     MAGIC_AttMagic( int charaindex , int toindex , int marray , int mp );
#endif
#ifdef _ITEM_METAMO
int MAGIC_Metamo( int charaindex, int toindex,int marray, int mp );
#endif

#ifdef _ITEM_ATTSKILLMAGIC
int MAGIC_AttSkill( int charaindex, int toindex,int marray, int mp );
#endif
#ifdef _MAGIC_WEAKEN// vincent  精灵:虚弱
int	MAGIC_Weaken( int charaindex, int toindex, int marray, int mp );
#endif
#ifdef _MAGIC_BARRIER// vincent  精灵:魔障
int	MAGIC_Barrier( int charaindex, int toindex, int marray, int mp );
#endif
#ifdef _MAGIC_NOCAST// vincent  精灵:沉默
int	MAGIC_Nocast( int charaindex, int toindex, int marray, int mp );
#endif
#ifdef _MAGIC_TOCALL
int MAGIC_ToCallDragon( int charaindex, int toindex,int marray, int mp );
#endif

//----------------------------------------------------------------------
// 失奶  丞及匏  井日热诸  寞毛忒允
//
int MAGIC_GetArrayNo(
	int charaindex, 	// 平乓仿弁正奶件犯永弁旦
	int haveitemindex   //    匀化月失奶  丞及匏  
);
//
//----------------------------------------------------------------------
//-------------------------------------------------------------------
//
//  热诸毛  涛银丹
//
int MAGIC_DirectUse(
	int charaindex, // 银丹平乓仿及奶件犯永弁旦
	int marray, 	// 银丹热诸及奶件犯永弁旦
	int toindex, 	// 簿卞银丹＂
	int itemnum
);
//
//-------------------------------------------------------------------

#endif 

