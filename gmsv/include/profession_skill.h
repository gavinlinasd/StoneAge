#include "version.h"

#ifdef _PROFESSION_SKILL			// WON ADD 職業技能
#include "common.h"
#include "util.h"
#include "skill.h"

#define PROFESSION_MAX_LEVEL 26			// 眥珛脹撰奻癹  26撰
#define PROFESSION_SKILL_MAX_LEVEL 100 	// 撮夔脹撰奻癹 100撰
#ifdef _75_TEST
#define PROFESSION_SKILL_ADD_POINT 100   // 汔撰潰隅籵徹ㄛ崝樓冪桄硉
#else
#define PROFESSION_SKILL_ADD_POINT 1   // 汔撰潰隅籵徹ㄛ崝樓冪桄硉
#endif
// 眥珛梗
typedef enum
{
	PROFESSION_CLASS_NONE=0,					// 拸眥珛
	PROFESSION_CLASS_FIGHTER,					// 蚋尪
	PROFESSION_CLASS_WIZARD,					// 拵呇
	PROFESSION_CLASS_HUNTER,					// 轂��
	PROFESSION_CLASS_NUM,
}PROFESSION_CLASS_TYPE;

typedef enum
{
	PROFESSION_SKILL_NAME,						// 撮夔靡備
	PROFESSION_SKILL_TXT,						// 佽隴
	PROFESSION_SKILL_FUNCNAME,					// 妏蚚滲杅
	PROFESSION_SKILL_OPTION,					// 滲杅統杅
	PROFESSION_SKILL_DATACHARNUM,
}PROFESSION_SKILL_DATACHAR;


typedef enum
{
	PROFESSION_SKILL_ID,						// 撮夔晤瘍
	PROFESSION_SKILL_PROFESSION_CLASS,			// 眥珛
	PROFESSION_SKILL_TARGET,					// 醴梓笱濬
	PROFESSION_SKILL_COST_MP,					// 瘧煤MP
	PROFESSION_SKILL_USE_FLAG,					// 妏蚚よ梓
	PROFESSION_SKILL_KIND,						// 撮夔笱濬
	PROFESSION_SKILL_ICON,						// ICON芞瘍
	PROFESSION_SKILL_IMG_1,						// 馴僻ヶ芞瘍(摩ァ袨怓)
	PROFESSION_SKILL_IMG_2,						// 馴僻芞瘍(馴僻笢)	
	PROFESSION_SKILL_COST,						// 劃鎗踢塗
	PROFESSION_SKILL_FIX_VALUE,					// 汔撰党淏杅硉
	
	//褒伎珂悝炾眕狟垀扢隅撮夔迵抇褶僅歙湛善奀符褫眕悝炾掛撮夔
	PROFESSION_SKILL_LIMIT1,					// 斛党撮夔 1 晤瘍
	PROFESSION_SKILL_PERCENT1,					// 斛党撮夔茼衄抇褶僅% 1
	PROFESSION_SKILL_LIMIT2,					// 斛党撮夔 2 晤瘍
	PROFESSION_SKILL_PERCENT2,					// 斛党撮夔茼衄抇褶僅% 2
	PROFESSION_SKILL_LIMIT3,					// 斛党撮夔 3 晤瘍
	PROFESSION_SKILL_PERCENT3,					// 斛党撮夔茼衄抇褶僅% 3
	PROFESSION_SKILL_LIMIT4,					// 斛党撮夔 4 晤瘍
	PROFESSION_SKILL_PERCENT4,					// 斛党撮夔茼衄抇褶僅% 4
	PROFESSION_SKILL_DATAINTNUM,
}PROFESSION_SKILL_DATAINT;


typedef struct tagProfessionkill
{
	STRING64	string[PROFESSION_SKILL_DATACHARNUM];
	int			data[PROFESSION_SKILL_DATAINTNUM];		
}Professionskill;


//----------------------------------------------------------------------------
typedef int (*PROFESSION_SKILL_CALLFUNC)( int, int, int, char *, int );
BOOL PROFESSION_initSkill( char *filename);
void rePROFESSION_initSkill();

//----------------------------------------------------------------------------
INLINE BOOL PROFESSION_SKILL_CHECKINDEX( int index );
INLINE int PROFESSION_SKILL_getInt( int index, PROFESSION_SKILL_DATAINT element);
INLINE int PROFESSION_SKILL_setInt( int index, PROFESSION_SKILL_DATAINT element, int data);
INLINE char* PROFESSION_SKILL_getChar( int index, PROFESSION_SKILL_DATACHAR element);
INLINE BOOL PROFESSION_SKILL_setChar( int index ,PROFESSION_SKILL_DATACHAR element, char* new );
int PROFESSION_SKILL_getskillNum( void );

//----------------------------------------------------------------------------
int PROFESSION_SKILL_getskillArray( int skillid);
PROFESSION_SKILL_CALLFUNC PROFESSION_SKILL_getskillFuncPointer(char* name );
#define PROFESSION_SKILL_GetArray( charaindex, skill) _PROFESSION_SKILL_GetArray( __FILE__, __LINE__, charaindex, skill)
int _PROFESSION_SKILL_GetArray( char *file, int line, int charaindex, int skill );
int PROFESSION_SKILL_Use( int charaindex, int skill, int toNo, char *data );
void PROFESSION_SKILL_LVEVEL_UP( int charaindex, char *name );
void PROFESSION_LEVEL_CHECK_UP( int charaindex );
void PROFESSION_NORMAL_SKILL_LEVLE_UP(Skill* skill, int Pskillid, int charaindex );
void PROFESSION_SKILL_WEAPON_FOCUS_LVEVEL_UP( int charaindex, char *name );
void PROFESSION_SKILL_DUAL_WEAPON_LVEVEL_UP( int charaindex, char *name );
int PROFESSION_SKILL_DEC_COST_MP( int charaindex, int skill, int Pskillid, int skill_level );
int PROFESSION_SKILL_ADDSK( int charaindex, int skill, int level );

//----------------------------------------------------------------------------
// 眥珛撮夔
void profession_common_fun( int charaindex, int toNo, int skill_level, int array, int com1 );
int PROFESSION_brust( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_chain_atk( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_avoid( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_recovery( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_weapon_focus( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_volcano_springs( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_fire_ball( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_fire_spear( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_summon_thunder( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_current( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_storm( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_ice_arrow( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_ice_crack( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_ice_mirror( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_doom( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_blood( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_blood_worms( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_sign( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_fire_enclose( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_ice_eclose( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_thunder_enclose( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_fire_practice( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_ice_practice( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_thunder_practice( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_enclose( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_transpose( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_reback( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_chain_atk_2( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_scapegoat( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_enrage( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_energy_collect( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_focus( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_shield_attack( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_dual_weapon( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_deflect( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_through_attack( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_cavalry( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_dead_attack( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_convolute( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_chaos( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_docile( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_trap( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_enrage_pet( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_dragnet( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_entwine( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_autarky( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_plunder( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_toxin_weapon( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_resist_fire( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_resist_ice( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_resist_thunder( int charaindex, int toindex, int array, char *data, int skill_level );
#ifdef _PROFESSION_ADDSKILL
int PROFESSION_resist_f_i_t( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_call_nature( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_boundary( int charaindex, int toindex, int array, char *data, int skill_level );
#endif
int PROFESSION_attack_weak( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_instigate( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_track( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_oblivion( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_full_mp( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_strong_back( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_strengthen( int charaindex, int toindex, int array, char *data, int skill_level );
int PROFESSION_escape( int charaindex, int toNo, int array, char *data, int skill_level );
int PROFESSION_g_resist_fire( int charaindex, int toNo, int array, char *data, int skill_level );
int PROFESSION_g_resist_ice( int charaindex, int toNo, int array, char *data, int skill_level );
int PROFESSION_g_resist_thunder( int charaindex, int toNo, int array, char *data, int skill_level );

//----------------------------------------------------------------------------

#endif
