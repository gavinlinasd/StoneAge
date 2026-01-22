#ifndef __MAGIC_H__
#define __MAGIC_H__

/* ��絊 */
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
#ifdef _MAGIC_WEAKEN// vincent  儕鍾:剞��
int	MAGIC_Weaken( int charaindex, int toindex, int marray, int mp );
#endif
#ifdef _MAGIC_BARRIER// vincent  儕鍾:藹梤
int	MAGIC_Barrier( int charaindex, int toindex, int marray, int mp );
#endif
#ifdef _MAGIC_NOCAST// vincent  儕鍾:麥蘇
int	MAGIC_Nocast( int charaindex, int toindex, int marray, int mp );
#endif
#ifdef _MAGIC_TOCALL
int MAGIC_ToCallDragon( int charaindex, int toindex,int marray, int mp );
#endif

//----------------------------------------------------------------------
// 囮騷  堜摯痾  凝�桱�絊  蠕禱蒍埰
//
int MAGIC_GetArrayNo(
	int charaindex, 	// す籤溘袲淏騷璃溢蚗袲筒
	int haveitemindex   //    埱趙堎囮騷  堜摯痾  
);
//
//----------------------------------------------------------------------
//-------------------------------------------------------------------
//
//  ��絊禱  旽窅竣
//
int MAGIC_DirectUse(
	int charaindex, // 窅竣す籤溘摯騷璃溢蚗袲筒
	int marray, 	// 窅竣��絊摯騷璃溢蚗袲筒
	int toindex, 	// 移勗窅竣ㄑ
	int itemnum
);
//
//-------------------------------------------------------------------

#endif 

