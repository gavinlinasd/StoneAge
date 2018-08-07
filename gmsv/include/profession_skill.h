#include "version.h"

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
#include "common.h"
#include "util.h"
#include "skill.h"

#define PROFESSION_MAX_LEVEL 26			// 职业等级上限  26级
#define PROFESSION_SKILL_MAX_LEVEL 100 	// 技能等级上限 100级
#ifdef _75_TEST
#define PROFESSION_SKILL_ADD_POINT 100   // 升级检定通过，增加经验值
#else
#define PROFESSION_SKILL_ADD_POINT 1   // 升级检定通过，增加经验值
#endif
// 职业别
typedef enum
{
	PROFESSION_CLASS_NONE=0,					// 无职业
	PROFESSION_CLASS_FIGHTER,					// 勇士
	PROFESSION_CLASS_WIZARD,					// 巫师
	PROFESSION_CLASS_HUNTER,					// 猎人
	PROFESSION_CLASS_NUM,
}PROFESSION_CLASS_TYPE;

typedef enum
{
	PROFESSION_SKILL_NAME,						// 技能名称
	PROFESSION_SKILL_TXT,						// 说明
	PROFESSION_SKILL_FUNCNAME,					// 使用函数
	PROFESSION_SKILL_OPTION,					// 函数参数
	PROFESSION_SKILL_DATACHARNUM,
}PROFESSION_SKILL_DATACHAR;


typedef enum
{
	PROFESSION_SKILL_ID,						// 技能编号
	PROFESSION_SKILL_PROFESSION_CLASS,			// 职业
	PROFESSION_SKILL_TARGET,					// 目标种类
	PROFESSION_SKILL_COST_MP,					// 耗费MP
	PROFESSION_SKILL_USE_FLAG,					// 使用旗标
	PROFESSION_SKILL_KIND,						// 技能种类
	PROFESSION_SKILL_ICON,						// ICON图号
	PROFESSION_SKILL_IMG_1,						// 攻击前图号(集气状态)
	PROFESSION_SKILL_IMG_2,						// 攻击图号(攻击中)	
	PROFESSION_SKILL_COST,						// 购买金额
	PROFESSION_SKILL_FIX_VALUE,					// 升级修正数值
	
	//角色先学习以下所设定技能与熟练度均达到时才可以学习本技能
	PROFESSION_SKILL_LIMIT1,					// 必修技能 1 编号
	PROFESSION_SKILL_PERCENT1,					// 必修技能应有熟练度% 1
	PROFESSION_SKILL_LIMIT2,					// 必修技能 2 编号
	PROFESSION_SKILL_PERCENT2,					// 必修技能应有熟练度% 2
	PROFESSION_SKILL_LIMIT3,					// 必修技能 3 编号
	PROFESSION_SKILL_PERCENT3,					// 必修技能应有熟练度% 3
	PROFESSION_SKILL_LIMIT4,					// 必修技能 4 编号
	PROFESSION_SKILL_PERCENT4,					// 必修技能应有熟练度% 4
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
// 职业技能
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
