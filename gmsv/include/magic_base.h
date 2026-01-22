#ifndef __MAGIC_BASE_H__
#define __MAGIC_BASE_H__

#include "util.h"

typedef enum
{
	MAGIC_FIELD_ALL,			/* 埰迋趙摯鳹鐎ぁ窅窇堎 */
	MAGIC_FIELD_BATTLE,				/* 橋    摯陑 */
	MAGIC_FIELD_MAP,				/* 玊橖悃蚗鏤窀摯陑 */

}MAGIC_FIELDTYPE;

typedef enum
{
	MAGIC_TARGET_MYSELF,		/* 猷覕摯陑 */
	MAGIC_TARGET_OTHER,			/* 眥摯硞  猷覕硈資) */
	MAGIC_TARGET_ALLMYSIDE,		/*     蠈   */
	MAGIC_TARGET_ALLOTHERSIDE,	/* Ъ鱗插蠈   */
	MAGIC_TARGET_ALL,			/* 蠈趙 */
	MAGIC_TARGET_NONE,			/* 移忒撒  ③  埵笢��  遛盓倜誧摯鄹 */
	MAGIC_TARGET_OTHERWITHOUTMYSELF,/* 眥摯硞  猷覕硈竘埵笢) */
	MAGIC_TARGET_WITHOUTMYSELFANDPET,  /* 猷覕敁妐蚗哱雄翻 */
	MAGIC_TARGET_WHOLEOTHERSIDE,/*     摯�蚅枅嚦�   */

#ifdef __ATTACK_MAGIC

        MAGIC_TARGET_SINGLE,            // 渀勤菩源腔議珨��
        MAGIC_TARGET_ONE_ROW,           // 渀勤菩源腔議珨蹈
        MAGIC_TARGET_ALL_ROWS,          // 渀勤菩源腔垀衄��

#endif
}MAGIC_TARGETTYPE;

typedef enum
{
	MAGIC_ID,					/* 集    蠕 */
	MAGIC_FIELD,				/* 窅窇堎鳹鐎 */
	MAGIC_TARGET,				/* 葡濯 */
	MAGIC_TARGET_DEADFLG,		/* 屻庌煦榆忒葡濯勗硈誧堎凝 */
#ifdef __ATTACK_MAGIC
  MAGIC_IDX ,
#endif
	MAGIC_DATAINTNUM,
}MAGIC_DATAINT;

typedef enum
{
	MAGIC_NAME,					/* ��絊   */
	MAGIC_COMMENT,				/* 昡隍璃哱*/
	MAGIC_FUNCNAME,				/* 匴倳   */
	MAGIC_OPTION,				/* 酘鏤で砫璃 */
	MAGIC_DATACHARNUM,
}MAGIC_DATACHAR;

typedef struct tagMagic
{
	int			data[MAGIC_DATAINTNUM];
	STRING64	string[MAGIC_DATACHARNUM];

}Magic;

#ifdef __ATTACK_MAGIC

typedef struct tagAttMagic
{
  unsigned int  uiSpriteNum;// 森紸扲婓Spr_x.bin腔晤瘍
  unsigned int  uiAttackType;// 馴僻腔源宒ㄩ等�芄盈�齬( 謫霜 ) ,  淕齬( 謫霜 ) , 淕齬( 肮奀 ) , �屏�( 謫霜 ) , �屏�( 肮奀 )
  unsigned int  uiSliceTime;// 謫霜馴僻奀腔奀潔船
  unsigned int  uiShowType;             // 珆尨腔弇离源宒ㄩ笢栝﹜硌隅
  int           siSx;                   // 珆尨腔弇离 - X粣
  int           siSy;                   // 珆尨腔弇离 - Y粣
  unsigned int  uiShowBehindChar;       // 珆尨婓�冼齔饒健蝠藲繚�
  unsigned int  uiShakeScreen;          // 岆瘁涾雄賒醱
  unsigned int  uiShakeFrom;            // 涾雄賒醱腔れ宎奀潔( 瑭鏃 )
  unsigned int  uiShakeTo;              // 涾雄賒醱腔賦旰奀潔( 瑭鏃 _
  unsigned int  uiPrevMagicNum;         // ヶ离紸扲腔坰竘瘍( 0XFFFFFFFFFF 桶尨拸ヶ离紸扲 )
  int           siPrevMagicSx;          // ヶ离紸扲腔珆尨弇离 - X粣
  int           siPrevMagicSy;          // ヶ离紸扲腔珆尨弇离 - Y粣
  int           siPrevMagicOnChar;      // ヶ离紸扲珆尨婓�冼齔饒健蝠藲繚�
  unsigned int  uiPostMagicNum;         // 摽离紸扲腔坰竘瘍( 0XFFFFFFFF 桶尨拸摽离紸扲 )
  int           siPostMagicSx;          // 摽离紸扲腔珆尨弇离 - X粣
  int           siPostMagicSy;          // 摽离紸扲腔珆尨弇离 - Y粣
  int           siPostMagicOnChar;      // 摽离紸扲珆尨婓�冼齔饒健蝠藲繚�
  int           siField[3][5];          // 馴僻坰竘
}AttMagic;

#endif

#ifdef _MAGIC_TOCALL

typedef struct tagToCallMagic
{
  unsigned int  uiSpriteNum;// 森紸扲婓Spr_x.bin腔晤瘍
  unsigned int  uiAttackType;// 馴僻腔源宒ㄩ等�芄盈�齬( 謫霜 ) ,  淕齬( 謫霜 ) , 淕齬( 肮奀 ) , �屏�( 謫霜 ) , �屏�( 肮奀 )
  unsigned int  uiSliceTime;// 謫霜馴僻奀腔奀潔船
  unsigned int  uiShowType;             // 珆尨腔弇离源宒ㄩ笢栝﹜硌隅
  int           siSx;                   // 珆尨腔弇离 - X粣
  int           siSy;                   // 珆尨腔弇离 - Y粣
  unsigned int  uiShowBehindChar;       // 珆尨婓�冼齔饒健蝠藲繚�
  unsigned int  uiShakeScreen;          // 岆瘁涾雄賒醱
  unsigned int  uiShakeFrom;            // 涾雄賒醱腔れ宎奀潔( 瑭鏃 )
  unsigned int  uiShakeTo;              // 涾雄賒醱腔賦旰奀潔( 瑭鏃 _
  unsigned int  uiPrevMagicNum;         // ヶ离紸扲腔坰竘瘍( 0XFFFFFFFFFF 桶尨拸ヶ离紸扲 )
  int           siPrevMagicSx;          // ヶ离紸扲腔珆尨弇离 - X粣
  int           siPrevMagicSy;          // ヶ离紸扲腔珆尨弇离 - Y粣
  int           siPrevMagicOnChar;      // ヶ离紸扲珆尨婓�冼齔饒健蝠藲繚�
  unsigned int  uiPostMagicNum;         // 摽离紸扲腔坰竘瘍( 0XFFFFFFFF 桶尨拸摽离紸扲 )
  int           siPostMagicSx;          // 摽离紸扲腔珆尨弇离 - X粣
  int           siPostMagicSy;          // 摽离紸扲腔珆尨弇离 - Y粣
  int           siPostMagicOnChar;      // 摽离紸扲珆尨婓�冼齔饒健蝠藲繚�
  int			isPostDisappear;		// 紸扲珨啜馴僻俇奀岆瘁鎮奻秏囮
  int			ToCallMagicNo;			// 欸遢扲腔晤瘍
}ToCallMagic;

#endif

typedef int (*MAGIC_CALLFUNC)( int, int, int, int );

INLINE BOOL MAGIC_CHECKINDEX( int index );
INLINE int MAGIC_getInt( int index, MAGIC_DATAINT element);
INLINE int MAGIC_setInt( int index, MAGIC_DATAINT element, int data);
INLINE char* MAGIC_getChar( int index, MAGIC_DATACHAR element);
INLINE BOOL MAGIC_setChar( int index ,MAGIC_DATACHAR element, char* new );
int MAGIC_getMagicNum( void);
BOOL MAGIC_initMagic( char *filename);
BOOL MAGIC_reinitMagic( void );

#ifdef __ATTACK_MAGIC

BOOL ATTMAGIC_initMagic( char *filename );
BOOL ATTMAGIC_reinitMagic( void );

#endif

int MAGIC_getMagicArray( int magicid);
MAGIC_CALLFUNC MAGIC_getMagicFuncPointer(char* name);
// Nuke +1 08/23 : For checking the validity of magic target
int MAGIC_isTargetValid( int magicid, int toindex);

#endif

