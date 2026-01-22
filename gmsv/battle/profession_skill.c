#include "version.h"


#ifdef _PROFESSION_SKILL			// WON ADD 職業技能
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "buf.h"
#include "profession_skill.h"
#include "char_base.h"
#include "battle.h"
#include "magic_base.h"
#include "configfile.h"
#include "char.h"
#include "lssproto_serv.h"
#include "skill.h"

static Professionskill	*PROFESSION_skill;


typedef struct tagProfessionskill_skillFunctionTable
{
	char			*functionname;
	PROFESSION_SKILL_CALLFUNC	func;
	int				hash;
//	int				status;
}PROFESSION_SKILL_skillFunctionTable;

static PROFESSION_SKILL_skillFunctionTable PROFESSION_SKILL_functbl[] = {

// 拵呇撮夔
	{ "PROFESSION_VOLCANO_SPRINGS", PROFESSION_volcano_springs, 0 },	// 鳶刓��
	{ "PROFESSION_FIRE_BALL",		PROFESSION_fire_ball,		0 },	// 鳶陎⑩
	{ "PROFESSION_FIRE_SPEAR",		PROFESSION_fire_spear,		0 },	// 鳶韓Л
	{ "PROFESSION_SUMMON_THUNDER",  PROFESSION_summon_thunder,  0 },	// 欸濘扲
	{ "PROFESSION_CURRENT",			PROFESSION_current,			0 },	// 萇霜扲
	{ "PROFESSION_STORM",			PROFESSION_storm,			0 },	// 惟瑞迾
	{ "PROFESSION_ICE_ARROW",		PROFESSION_ice_arrow,		0 },	// 梨璋扲
	{ "PROFESSION_ICE_CRACK",		PROFESSION_ice_crack,		0 },	// 梨惇扲
	{ "PROFESSION_ICE_MIRROR",		PROFESSION_ice_mirror,		0 },	// 梨噩扲
	{ "PROFESSION_DOOM",			PROFESSION_doom,			0 },	// 岍賜藺��
	{ "PROFESSION_BLOOD",			PROFESSION_blood,			0 },	// 岓悛傖俶
	{ "PROFESSION_BLOOD_WORMS",		PROFESSION_blood_worms,		0 },	// 岓悛嘍
	{ "PROFESSION_SIGN",			PROFESSION_sign,			0 },	// 珨渀獗悛
	{ "PROFESSION_FIRE_ENCLOSE",	PROFESSION_fire_enclose,	0 },	// 鳶蜇极
	{ "PROFESSION_ICE_ENCLOSE",		PROFESSION_ice_eclose,		0 },	// 梨蜇极
	{ "PROFESSION_THUNDER_ENCLOSE",	PROFESSION_thunder_enclose, 0 },	// 濘蜇极
	{ "PROFESSION_FIRE_PRACTICE",	PROFESSION_fire_practice,	0 },	// 鳶藹楊抇褶僅
	{ "PROFESSION_ICE_PRACTICE",	PROFESSION_ice_practice,	0 },	// 梨藹楊抇褶僅
	{ "PROFESSION_THUNDER_PRACTICE",PROFESSION_thunder_practice,0 },	// 濘藹楊抇褶僅
	{ "PROFESSION_ENCLOSE",			PROFESSION_enclose,			0 },	// 蜇旯扲
	{ "PROFESSION_TRANSPOSE",		PROFESSION_transpose,		0 },	// 痄倛遙弇
//	{ "PROFESSION_ALCHEMY",			PROFESSION_alchemy,			0 },	// 褻踢扲

// 蚋尪撮夔
	{ "PROFESSION_CHAIN_ATK",		PROFESSION_chain_atk,		0 },	// 蟀遠馴僻
	{ "PROFESSION_AVOID",			PROFESSION_avoid,			0 },	// 隙旌
	{ "PROFESSION_WEAPON_FOCUS",	PROFESSION_weapon_focus,	0 },	// 挕ん蚳儕	
	{ "PROFESSION_REBACK",			PROFESSION_reback,			0 },	// 袨怓隙葩
	{ "PROFESSION_BRUST",			PROFESSION_brust,			0 },	// 惇僻
	{ "PROFESSION_CHAIN_ATK_2",		PROFESSION_chain_atk_2,		0 },	// 邧笭馴僻
	{ "PROFESSION_SCAPEGOAT",		PROFESSION_scapegoat,		0 },	// 忔眒峈衭
	{ "PROFESSION_ENRAGE",			PROFESSION_enrage,			0 },	// 慾趙馴僻
	{ "PROFESSION_ENERGY_COLLECT",	PROFESSION_energy_collect,	0 },	// 夔講擄摩
	{ "PROFESSION_FOCUS",			PROFESSION_focus,			0 },	// 蚳蛁桵須
	{ "PROFESSION_SHIELD_ATTACK",	PROFESSION_shield_attack,	0 },	// 嗎僻
	{ "PROFESSION_DUAL_WEAPON",		PROFESSION_dual_weapon,		0 },	// 媼絮霜	
	{ "PROFESSION_DEFLECT",			PROFESSION_deflect,			0 },	// 跡紫
	{ "PROFESSION_THROUGH_ATTACK",	PROFESSION_through_attack,	0 },	// 嫗援馴僻
	{ "PROFESSION_CAVALRY",			PROFESSION_cavalry,			0 },	// 釱る馴僻
	{ "PROFESSION_DEAD_ATTACK",		PROFESSION_dead_attack,		0 },	// 梭侚馴僻
	{ "PROFESSION_CONVOLUTE",		PROFESSION_convolute,		0 },	// 隙唅馴僻
	{ "PROFESSION_CHAOS",			PROFESSION_chaos,			0 },	// 髦觴馴僻

// 轂��
	{ "PROFESSION_TRACK",			PROFESSION_track,			0 },	// 袚扆菩趿
	{ "PROFESSION_ESCAPE",			PROFESSION_escape,			0 },	// 隙旌桵須
	{ "PROFESSION_DOCILE",			PROFESSION_docile,			0 },	// 拲睦唾昜
	{ "PROFESSION_TRAP",			PROFESSION_trap,			0 },	// 疪筘
	{ "PROFESSION_ENRAGE_PET",		PROFESSION_enrage_pet,		0 },	// 慾躑唾昜
	{ "PROFESSION_DRAGNET",			PROFESSION_dragnet,			0 },	// 毞蹕華厙
	{ "PROFESSION_ENTWINE",			PROFESSION_entwine,			0 },	// 攷跦莊��
	{ "PROFESSION_AUTARKY",			PROFESSION_autarky,			0 },	// 赻跤赻逋
	{ "PROFESSION_PLUNDER",			PROFESSION_plunder,			0 },	//   极謨嗤
	{ "PROFESSION_TOXIN_WEAPON",	PROFESSION_toxin_weapon,	0 },	// 馮匼挕ん
	{ "PROFESSION_RESIST_FIRE",		PROFESSION_resist_fire,		0 },	// 鳶蕨俶
	{ "PROFESSION_RESIST_ICE",		PROFESSION_resist_ice,		0 },	// 梨蕨俶
	{ "PROFESSION_RESIST_THUNDER",	PROFESSION_resist_thunder,	0 },	// 濘蕨俶
#ifdef _PROFESSION_ADDSKILL
    { "PROFESSION_RESIST_F_I_T",    PROFESSION_resist_f_i_t,    0 },    // 赻�銓�夔
	{ "PROFESSION_CALL_NATURE",     PROFESSION_call_nature,     0 },    // 瘍欸赻��
	{ "PROFESSION_BOUNDARY",        PROFESSION_boundary,     0 },       // 侐扽俶賦賜
#endif
	{ "PROFESSION_G_RESIST_FIRE",	PROFESSION_g_resist_fire,	0 },	// 芶极鳶蕨俶
	{ "PROFESSION_G_RESIST_ICE",	PROFESSION_g_resist_ice,	0 },	// 芶极梨蕨俶
	{ "PROFESSION_G_RESIST_THUNDER",PROFESSION_g_resist_thunder,0 },	// 芶极濘蕨俶
	{ "PROFESSION_ATTACK_WEAK",		PROFESSION_attack_weak,		0 },	// �黤蓍本�
	{ "PROFESSION_INSTIGATE",		PROFESSION_instigate,		0 },	// 泔畢
	{ "PROFESSION_OBLIVION",		PROFESSION_oblivion,		0 },	// 疻咭

// 僕肮撮夔
	{ "PROFESSION_FULL_MP",			PROFESSION_full_mp,			0 },	// ァ薯喃驛
	{ "PROFESSION_STRONG_BACK",		PROFESSION_strong_back,		0 },	// 蛹笭崝樓
	{ "PROFESSION_STRENGTHEN",		PROFESSION_strengthen,		0 },	// 赻扂Ч趙

};


static int profession_skill_num;				// 眥珛撮夔軞杅

// 蔚 profession.txt 婥��
BOOL PROFESSION_initSkill( char *filename)
{

	FILE*   f;
    char    line[256];
    int     linenum=-1;
	int     skill_readlen=0;
	int		i, j;

	int max_skillid = 0;

    f = fopen(filename,"r");
    if( f == NULL ){
        print( "file open error\n");
        return FALSE;
    }

    profession_skill_num=0;

    while( fgets( line, sizeof( line ), f ) ){
		char    token[256];

        linenum ++;
        if( line[0] == '#' )continue;
        if( line[0] == '\n' )continue;
        chomp( line );
        profession_skill_num++;
#ifdef _PROSKILL_OPTIMUM // Robin �○囆豱鯡側嘹D
		if( getStringFromIndexWithDelim( line, ",", 
				PROFESSION_SKILL_DATACHARNUM+PROFESSION_SKILL_ID+1,
				token, sizeof(token)) == FALSE )
			continue;
		max_skillid = max( atoi( token), max_skillid);
#endif
    }

    if( fseek( f, 0, SEEK_SET ) == -1 ){
        fprint( "Seek Error\n" );
        fclose(f);
        return FALSE;
    }

#ifdef _PROSKILL_OPTIMUM // 眥撮Table杅峈 郔湮撮夔ID +1
	profession_skill_num = max_skillid +1;
	print("郔湮眥撮ID = %d ...", max_skillid);
#endif

	PROFESSION_skill = allocateMemory( sizeof(struct tagProfessionkill) * profession_skill_num + 1 );


	if( PROFESSION_skill == NULL ){
        fprint( "拸楊煦饜囀湔 %d\n", sizeof(struct tagProfessionkill)*profession_skill_num);
        fclose( f );
        return FALSE;
    }

	// init skill
    for( i = 0; i < profession_skill_num; i ++ ) {
    	for( j = 0; j < PROFESSION_SKILL_DATAINTNUM; j ++ )
    		PROFESSION_SKILL_setInt( i,j,-1);
    
    	for( j = 0; j < PROFESSION_SKILL_DATACHARNUM; j ++ ) 
    		PROFESSION_SKILL_setChar( i,j,"");    
    }

    linenum = -1;
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;
        if( line[0] == '\n' )continue;             
                
        chomp( line );

        replaceString( line, '\t' , ' ' );

		{
		    char    buf[256];
			for( i = 0; i < strlen( line); i ++) {
				if( line[i] != ' ' ) {
					break;
				}
				strcpy( buf, &line[i]);
			}
			
			if( i != 0 ) {
				strcpy( line, buf);
			}
		}  
		{
			char    token[256];
			int     ret;

#ifdef _PROSKILL_OPTIMUM // 黍�§憶倗側撐鯧D, 眻諉眕眥撮ID絞Table index
			ret = getStringFromIndexWithDelim( line, ",", PROFESSION_SKILL_DATACHARNUM+PROFESSION_SKILL_ID+1, token, sizeof(token));
			if( ret==FALSE ){
				fprint("Syntax Error file:%s line:%d\n",filename,linenum);
				break;
			}
			skill_readlen = atoi( token);
#endif
			for( i = 0; i < PROFESSION_SKILL_DATACHARNUM; i ++ ) {

			    ret = getStringFromIndexWithDelim( line,",", i + 1, token,sizeof(token));
				if( ret==FALSE ){
					fprint("Syntax Error file:%s line:%d\n",filename,linenum);
					break;
				} 
				PROFESSION_SKILL_setChar( skill_readlen, i, token);
			} 

			for( i = 0; i < PROFESSION_SKILL_DATAINTNUM; i ++ ) {
				ret = getStringFromIndexWithDelim( line, ",", i+PROFESSION_SKILL_DATACHARNUM+1, token, sizeof(token));
				
				if( ret==FALSE ){
					fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
					break;
				} 
				if( strlen( token) != 0 ) {
					PROFESSION_SKILL_setInt( skill_readlen, i , atoi( token ));
				} 
			}         		
			skill_readlen ++;
		} 		
    } 
    fclose(f);

    profession_skill_num = skill_readlen;

	for( i = 0; i < arraysizeof( PROFESSION_SKILL_functbl); i ++ ){
		PROFESSION_SKILL_functbl[i].hash = hashpjw( PROFESSION_SKILL_functbl[i].functionname );
	}

    return TRUE;

}


// 笭陔婥�� profession.txt
void rePROFESSION_initSkill()
{
	freeMemory( PROFESSION_skill );
	PROFESSION_initSkill(getProfession());
}

INLINE BOOL PROFESSION_SKILL_CHECKINDEX( int index )
{
    if( profession_skill_num < index || index < 0 )return FALSE;
    return TRUE;
}

static INLINE BOOL PROFESSION_SKILL_CHECKINTDATAINDEX( int index)
{
	if( PROFESSION_SKILL_DATAINTNUM < index || index < 0 ) return FALSE;
	return TRUE;
}

static INLINE BOOL PROFESSION_SKILL_CHECKCHARDATAINDEX( int index)
{
	if( PROFESSION_SKILL_DATACHARNUM < index || index < 0 ) return FALSE;
	return TRUE;
}

INLINE int PROFESSION_SKILL_getInt( int index, PROFESSION_SKILL_DATAINT element)
{
	return PROFESSION_skill[index].data[element];
}

INLINE int PROFESSION_SKILL_setInt( int index, PROFESSION_SKILL_DATAINT element, int data)
{
	int buf;
	buf = PROFESSION_skill[index].data[element];
	PROFESSION_skill[index].data[element]=data;
	return buf;
}

INLINE char* PROFESSION_SKILL_getChar( int index, PROFESSION_SKILL_DATACHAR element)
{
	if( !PROFESSION_SKILL_CHECKINDEX( index)) return NULL;
	if( !PROFESSION_SKILL_CHECKCHARDATAINDEX( element)) return NULL;
	return PROFESSION_skill[index].string[element].string;
}

INLINE BOOL PROFESSION_SKILL_setChar( int index ,PROFESSION_SKILL_DATACHAR element, char* new )
{
    if(!PROFESSION_SKILL_CHECKINDEX(index))return FALSE;
    if(!PROFESSION_SKILL_CHECKCHARDATAINDEX(element))return FALSE;

    strcpysafe( PROFESSION_skill[index].string[element].string,
                sizeof(PROFESSION_skill[index].string[element].string),
                new );
    return TRUE;
}

int PROFESSION_SKILL_getskillNum( void)
{
	return profession_skill_num;
}


/*----------------------------------------------------------------------*/

int PROFESSION_SKILL_getskillArray( int skillid)
{
#ifdef _PROSKILL_OPTIMUM // 脤戙眥撮ID蜊峈淝蹈宒
	if( skillid >= 0 && skillid < profession_skill_num )
		return PROFESSION_skill[skillid].data[PROFESSION_SKILL_ID];
#else
	int		i;
	for( i = 0; i <= profession_skill_num; i ++ ) { // 渾蚥趙
		if( PROFESSION_skill[i].data[PROFESSION_SKILL_ID] == skillid ) 
			return i;
	}
#endif
	return -1;
}

PROFESSION_SKILL_CALLFUNC PROFESSION_SKILL_getskillFuncPointer(char* name )
{
    int i;
    int hash = hashpjw( name );
	
	for( i = 0; i< arraysizeof( PROFESSION_SKILL_functbl); i++ ){
		if( PROFESSION_SKILL_functbl[i].hash == hash ){
        	if( strcmp( PROFESSION_SKILL_functbl[i].functionname, name ) == 0 ){
	            return PROFESSION_SKILL_functbl[i].func;
			}
		}
	}

    return NULL;
}

int _PROFESSION_SKILL_GetArray( char *file, int line, int charaindex, int skill )
{
	int		skillid,array;

	skillid = CHAR_getCharSkill( charaindex, skill);

	if( skillid < 0 )	return -1;

	array = PROFESSION_SKILL_getskillArray( skillid );
		
	return array;
}


/*----------------------------------------------------------------------*/

// 妏蚚眥珛撮夔
int PROFESSION_SKILL_Use( int charaindex, int skill, int toNo, char *data )
{
	int		Pskillid, skillid;
	int		ret=-1, skill_level;
	PROFESSION_SKILL_CALLFUNC func;
	CHAR_HaveSkill* hskill;
	int char_pskill, profession_skill;

	skillid = CHAR_getCharSkill( charaindex, skill);
	Pskillid = PROFESSION_SKILL_getskillArray( skillid);
	//print("\n skillid:%d Pskillid:%d ", skillid, Pskillid); // Robin log
	if( Pskillid == -1 ) {
		printf("\n 蜊猾婦??梑祥善蜆眥撮! ");
		return ret;
	}

#ifdef _PROSKILL_OPTIMUM	// Robin fix 潰脤眥珛岆瘁眈睫
	char_pskill = CHAR_getInt( charaindex, PROFESSION_CLASS );
	profession_skill = PROFESSION_SKILL_getInt( Pskillid, PROFESSION_SKILL_PROFESSION_CLASS);
	if( (char_pskill <=0) || (char_pskill != profession_skill) ) {
		print("\n 蜊猾婦??眥撮腔眥珛祥淏��!:%d:%d ", char_pskill, profession_skill);
		return ret;
	}
#endif

	// 梑扆勤茼 function
	func = PROFESSION_SKILL_getskillFuncPointer( PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_FUNCNAME) );

	// 瓚剿岆瘁峈�冼�
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER )	return ret;


	if( func ){
		// �冼嚗敵傿�撰
		hskill = CHAR_getCharHaveSkill( charaindex, skill );
		skill_level = SKILL_getInt( &hskill->skill, SKILL_LEVEL);
	
		// 瘧囷 MP 
		if( PROFESSION_SKILL_DEC_COST_MP( charaindex, skill, Pskillid, skill_level ) != 1 )	{
			print("\n Error:眥撮MP:%d:%d:%d ", skill, Pskillid, skill_level);
			return ret;
		}

		// 妏蚚眥珛撮夔
		ret = func( charaindex, toNo, Pskillid, data, skill_level );
		if( ret == -1 ){
			int rands = rand()%10;
			//andy_log
			print("ret = %d, rands:%d \n", ret, rands);
			if( rands > 5 ) return ret;
		}

		if( Pskillid == 57 //慾躑唾昜 恁唾昜符衄冪桄硉
			&& CHAR_getInt( BATTLE_No2Index( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX ), toNo ), CHAR_WHICHTYPE) != CHAR_TYPEPET ) 
			return ret;

		// 珨啜撮夔汔撰
		PROFESSION_NORMAL_SKILL_LEVLE_UP( &hskill->skill, Pskillid, charaindex );
	}
	else {
		print("\n 渣昫!梑祥善眥撮func ");
	}

	return ret;
}

// 杻忷撮夔汔撰
void PROFESSION_SKILL_LVEVEL_UP( int charaindex, char *name )
{
	int skill=-1;
	int skillid=0, Pskillid=0, i;
	char *skill_name;
	CHAR_HaveSkill* hskill;

	for( i=0; i<CHAR_SKILLMAXHAVE; i++ ){
			
		// 撮夔ID
		skillid = CHAR_getCharSkill( charaindex, i);
		Pskillid = PROFESSION_SKILL_getskillArray( skillid);		
		if( Pskillid < 0 ) continue;

		// 撮夔靡備
		skill_name = PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_FUNCNAME);
		if( skill_name == NULL ) continue;
			
		if( (strcmp( skill_name , name )) == 0 ){
			skill = i;		break;
		}
	}	

	if( (skill < 0) || (skill > CHAR_SKILLMAXHAVE) ) return;

	skillid = CHAR_getCharSkill( charaindex, skill);
	Pskillid = PROFESSION_SKILL_getskillArray( skillid);
	if( Pskillid == -1 )	return;

	// �冼嚗敵傿�撰
	hskill = CHAR_getCharHaveSkill( charaindex, skill );
	
	// 珨啜撮夔汔撰
	PROFESSION_NORMAL_SKILL_LEVLE_UP( &hskill->skill, Pskillid, charaindex );

	return;
}

// 媼絮霜撮夔汔撰
void PROFESSION_SKILL_DUAL_WEAPON_LVEVEL_UP( int charaindex, char *name )
{
	int skill=-1, arm = 0, shield = 0;
	int skillid=0, Pskillid=0, i;
	char *skill_name;
	CHAR_HaveSkill* hskill;

	// 潰脤酘衵忒蚾掘
	for( i = 0 ; i < CHAR_EQUIPPLACENUM ; i ++ ){
		int id = CHAR_getItemIndex(charaindex,i);
		if( ITEM_CHECKINDEX(id) ){
			if( i == CHAR_ARM ) arm = 1;
			else if( i == CHAR_EQSHIELD ) shield = 1;			
		}
	}

	if( arm != 1 || shield != 1 )	return;	

	for( i=0; i<CHAR_SKILLMAXHAVE; i++ ){
		
		// 撮夔ID
		skillid = CHAR_getCharSkill( charaindex, i);
		Pskillid = PROFESSION_SKILL_getskillArray( skillid);		
		if( Pskillid < 0 ) continue;

		// 撮夔靡備
		skill_name = PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_FUNCNAME);
		if( skill_name == NULL ) continue;
			
		if( (strcmp( skill_name , name )) == 0 ){
			skill = i;		break;
		}
	}

	if( (skill < 0) || (skill > CHAR_SKILLMAXHAVE) ) return;

	skillid = CHAR_getCharSkill( charaindex, skill);
	Pskillid = PROFESSION_SKILL_getskillArray( skillid);
	if( Pskillid == -1 )	return;

	// �冼嚗敵傿�撰
	hskill = CHAR_getCharHaveSkill( charaindex, skill );
	
	// 珨啜撮夔汔撰
	PROFESSION_NORMAL_SKILL_LEVLE_UP( &hskill->skill, Pskillid, charaindex );

	return;
}


// 挕ん蚳儕撮夔汔撰
void PROFESSION_SKILL_WEAPON_FOCUS_LVEVEL_UP( int charaindex, char *name )
{
	int skill = -1;
	int skillid=0, Pskillid = 0, i;
	char *skill_name, *pszOption, *pszP, diff[20];
	int WeaponType = -1;
	CHAR_HaveSkill* hskill;

	memset( diff, -1, sizeof(diff) );
	// 蚾掘挕ん
	WeaponType = BATTLE_GetWepon( charaindex );

	switch( WeaponType ){
		case ITEM_AXE:		
			sprintf( diff, "葦" ); break;
		case ITEM_CLUB:		
			sprintf( diff, "幔" ); break;
		case ITEM_SPEAR:	
			sprintf( diff, "Л" ); break;
		case ITEM_BOW:		
			sprintf( diff, "僮" ); break;
		case ITEM_BOOMERANG:	// 隙唅梓	
			sprintf( diff, "曚" ); break;
		case ITEM_BOUNDTHROW:	// 芘祣葦芛
			sprintf( diff, "芘" ); break;
		case ITEM_BREAKTHROW:	// 芘祣坒
			sprintf( diff, "坒" ); break;
		default: 
			sprintf( diff, "拸" ); break;
	}

	for( i=0; i<CHAR_SKILLMAXHAVE; i++ ){
		// 撮夔ID
		skillid = CHAR_getCharSkill( charaindex, i);
		Pskillid = PROFESSION_SKILL_getskillArray( skillid);		
		if( Pskillid < 0 ) continue;

		// 撮夔靡備
		skill_name = PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_FUNCNAME);
		if( skill_name == NULL ) continue;
	
		// 撮夔統杅
		pszOption = PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_OPTION);
		if( pszOption == NULL ) continue;
		
		if( (strcmp( skill_name , name )) == 0 ){
			if( (pszP=strstr( pszOption, diff ))!=NULL ){
				skill = i;		break;
			}
		}
	}	

	if( (skill < 0) || (skill > CHAR_SKILLMAXHAVE) ) return;

	skillid = CHAR_getCharSkill( charaindex, skill);
	Pskillid = PROFESSION_SKILL_getskillArray( skillid);
	if( Pskillid == -1 )	return;

	// �冼嚗敵傿�撰
	hskill = CHAR_getCharHaveSkill( charaindex, skill );
	
	// 珨啜撮夔汔撰
	PROFESSION_NORMAL_SKILL_LEVLE_UP( &hskill->skill, Pskillid, charaindex );

	return;
}

// 瘧囷 MP 
int PROFESSION_SKILL_DEC_COST_MP( int charaindex, int skill, int Pskillid, int skill_level )
{	
	
	if(skill_level > 0){
		int	dec_mp;
		int old_mp = CHAR_getInt( charaindex, CHAR_MP );
		int mp=0;
			
		// �○鷅騰� MP 
		if( (dec_mp = PROFESSION_MAGIC_COST_MP( charaindex, skill )) == -1 )
			dec_mp = PROFESSION_SKILL_getInt( Pskillid, PROFESSION_SKILL_COST_MP );

		// 潰脤 MP講			
		if( old_mp < dec_mp ) return -1;
		
		// 岓悛傖俶 
		if( Pskillid != 11 
#ifdef _PROFESSION_ADDSKILL
		//	&& Pskillid != 2 //渀渀眈勤祥諶 MP
#endif
			)
			if( dec_mp <= 0 ) return -1;
		
		// 諶裁�冼� MP
		mp = old_mp - dec_mp;
		if( mp < 0 ) mp = 0;
		CHAR_setInt( charaindex , CHAR_MP , mp );
		CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_MP);
	
		return 1;
	}
	
	return -1;
}



// 珨啜撮夔汔撰
void PROFESSION_NORMAL_SKILL_LEVLE_UP(Skill *skill, int Pskillid, int charaindex )
{
	int skill_level = 0, up_fix_value = 0;
	int rand_num = RAND( 0, 10000 ), rand_num2 = 0;
	
	// 撮夔脹撰善湛奻癹祥汔撰
	skill_level = SKILL_getRealInt( skill, SKILL_LEVEL);
	if( skill_level >= PROFESSION_SKILL_MAX_LEVEL * 100 ) return;

	// 汔撰党淏杅硉
	up_fix_value = PROFESSION_SKILL_getInt( Pskillid, PROFESSION_SKILL_FIX_VALUE) * 100;
	rand_num2 = RAND( 0, up_fix_value);

	// 汔撰潰隅
#ifdef _75_TEST
#else
	if( rand_num > skill_level + rand_num2){
#endif	
		// 崝樓抇褶僅
		skill_level += PROFESSION_SKILL_ADD_POINT;	
#ifdef _75_TEST
		if( skill_level > 10000 )
			skill_level = 10000;
#endif
		SKILL_setInt( skill, SKILL_LEVEL, skill_level );	

		if( ( skill_level % 100 ) == 0 ){
			char msg[256];
			sprintf( msg, "%s撮夔抇褶僅奻汔峈%d", 	PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_NAME), skill_level/100 );
			CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW);
			CHAR_sendStatusString( charaindex , "S");	
			
		
			// 眥珛冪桄硉崝樓		
			PROFESSION_LEVEL_CHECK_UP( charaindex );
		}
#ifdef _75_TEST
#else
	}
#endif
	return;
}


void PROFESSION_LEVEL_CHECK_UP( int charaindex )
{
	int skill_level_add = 0, skill_level_sum = 0, i = 0;
	int old_level = CHAR_getInt( charaindex, PROFESSION_LEVEL );
	int next_level_need_point = 9999999;
	CHAR_HaveSkill* hskill;

	// 狟珨撰眥珛汔撰萸杅
	next_level_need_point = old_level * 70 * 100;
		
	for( i=0; i<CHAR_SKILLMAXHAVE; i++ ){
		int skillID = -1;

		// 撮夔脹撰
		hskill = CHAR_getCharHaveSkill( charaindex, i );
		skillID = SKILL_getInt( &hskill->skill, SKILL_IDENTITY);
		if( skillID <= 0 ) continue;

		// 僕籵撮夔抇褶僅 50
		if( (skillID == 63) || (skillID == 64) || (skillID == 65) ){
			skill_level_add = 50*100;	
		}else{
			skill_level_add = SKILL_getRealInt( &hskill->skill, SKILL_LEVEL);
		}
		
		skill_level_sum += skill_level_add;	
	}
	
	print("\n撮夔抇褶僅奻汔 ==> 絞ヶ抇褶僅萸杅(%d) 狟珨撰抇褶僅汔撰萸杅(%d)", skill_level_sum, next_level_need_point );

	// 撮夔抇褶僅湛善狟珨撰眥珛汔撰萸杅奀ㄛ眥珛脹撰崝樓
	if(  skill_level_sum >= next_level_need_point ){
		int old_skill_point = CHAR_getInt( charaindex, PROFESSION_SKILL_POINT );
		int next_skill_point = old_skill_point+1;
		char msg[64];

		print("\n眥珛脹撰奻汔 ==> 絞ヶ眥珛脹撰萸杅(%d) 狟珨撰眥珛脹撰汔撰萸杅(%d)", old_skill_point, next_skill_point );
		
		CHAR_setInt( charaindex, PROFESSION_LEVEL, old_level + 1 );
		CHAR_setInt( charaindex, PROFESSION_SKILL_POINT, next_skill_point );
		CHAR_sendStatusString( charaindex , "P");
		
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));

		sprintf( msg, "眥珛脹撰奻汔峈%d撰ㄛ撮夔萸杅奻汔ㄠ萸", old_level + 1 );
		CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW);
	}
}

// 崝樓撮夔
int PROFESSION_SKILL_ADDSK( int charaindex, int skill, int level )
{
	int i;

	CHAR_HaveSkill *pSkil;

	if( level > 100 )		level = 100;
	else if( level < 1 )	level = 1;

	for( i = 0; i < CHAR_SKILLMAXHAVE; i ++ ){
		int skill_id = -1;

		pSkil = CHAR_getCharHaveSkill( charaindex, i );
		skill_id = SKILL_getInt( &pSkil->skill, SKILL_IDENTITY);
		if( skill == skill_id && skill_id != -1 ){
			CHAR_talkToCli(charaindex,-1, "斕眒冪悝炾徹森撮夔", CHAR_COLORYELLOW);
			return -1;
		}

		if( pSkil == NULL )continue;
		if( pSkil->use != 0 )continue;
		break;
	}
	if( i >= CHAR_SKILLMAXHAVE ){
		CHAR_talkToCli(charaindex,-1, "撮夔杅講眒湛奻癹", CHAR_COLORYELLOW);
		return -1;
	}else{
		level *= 100;
		SKILL_makeSkillData( &pSkil->skill, skill, level);
		pSkil->use = TRUE;
	}

	CHAR_sendStatusString( charaindex , "S");
	

	return 1;
}

void profession_common_fun( int charaindex, int toNo, int skill_level, int array, int com1 )
{
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, com1 );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3, skill_level);
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);
#ifdef _PROFESSION_ADDSKILL
	if( com1 == BATTLE_COM_S_DOOM 
		/*&& CHAR_getWorkInt( charaindex, CHAR_DOOMTIME) == 0*/ ){
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
		CHAR_setWorkInt( charaindex, CHAR_WORK_com1, com1 );
		CHAR_setWorkInt( charaindex, CHAR_WORK_toNo, toNo );
		CHAR_setWorkInt( charaindex, CHAR_WORK_mode, BATTLE_CHARMODE_C_OK );
		CHAR_setWorkInt( charaindex, CHAR_WORK_skill_level, skill_level );
		CHAR_setWorkInt( charaindex, CHAR_WORK_array, array );
		CHAR_setWorkInt( charaindex, CHAR_DOOMTIME, 3);
	}
	else if( com1 == BATTLE_COM_S_FIRE_SPEAR 
			/*&& CHAR_getWorkInt( charaindex, CHAR_DOOMTIME) == 0*/ ){
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
		CHAR_setWorkInt( charaindex, CHAR_WORK_com1, com1 );
		CHAR_setWorkInt( charaindex, CHAR_WORK_toNo, toNo );
		CHAR_setWorkInt( charaindex, CHAR_WORK_mode, BATTLE_CHARMODE_C_OK );
		CHAR_setWorkInt( charaindex, CHAR_WORK_skill_level, skill_level );
		CHAR_setWorkInt( charaindex, CHAR_WORK_array, array );
		CHAR_setWorkInt( charaindex, CHAR_DOOMTIME, 2);
	}
#endif
}

/*----------------------------------------------------------------------*/
// 眕狟峈眥珛撮夔

// 拵呇撮夔-鳶刓��
int PROFESSION_volcano_springs( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_VOLCANO_SPRINGS );		
	return TRUE;
}

// 拵呇撮夔-鳶陎⑩
int PROFESSION_fire_ball( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_FIRE_BALL );		
	return TRUE;
}

// 拵呇撮夔-鳶韓Л
int PROFESSION_fire_spear( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_FIRE_SPEAR );		
	return TRUE;
}

// 拵呇撮夔-欸濘扲
int PROFESSION_summon_thunder( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_SUMMON_THUNDER );		
	return TRUE;
}

// 拵呇撮夔-萇霜扲
int PROFESSION_current( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_CURRENT );		
	return TRUE;
}

// 拵呇撮夔-惟瑞迾
int PROFESSION_storm( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_STORM );		
	return TRUE;
}

// 拵呇撮夔-梨璋扲
int PROFESSION_ice_arrow( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ICE_ARROW );		
	return TRUE;
}

// 拵呇撮夔-梨惇扲
int PROFESSION_ice_crack( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ICE_CRACK );		
	return TRUE;
}

// 拵呇撮夔-梨噩扲
int PROFESSION_ice_mirror( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ICE_MIRROR );		
	return TRUE;
}

// 拵呇撮夔-岍賜藺��
int PROFESSION_doom( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_DOOM );		
	return TRUE;
}

// 拵呇撮夔-岓悛傖俶
int PROFESSION_blood( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_BLOOD );		
	return TRUE;
}

// 拵呇撮夔-岓悛嘍
int PROFESSION_blood_worms( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_BLOOD_WORMS );		
	return TRUE;
}

// 拵呇撮夔-珨渀獗悛
int PROFESSION_sign( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_SIGN );		
	return TRUE;
}

// 拵呇撮夔-鳶蜇极
int PROFESSION_fire_enclose( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_FIRE_ENCLOSE );		
	return TRUE;
}

// 拵呇撮夔-梨蜇极
int PROFESSION_ice_eclose( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ICE_ENCLOSE );		
	return TRUE;
}

// 拵呇撮夔-濘蜇极
int PROFESSION_thunder_enclose( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_THUNDER_ENCLOSE );		
	return TRUE;
}

// 拵呇撮夔-鳶藹楊抇褶僅
int PROFESSION_fire_practice( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}

// 拵呇撮夔-梨藹楊抇褶僅
int PROFESSION_ice_practice( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}

// 拵呇撮夔-濘藹楊抇褶僅
int PROFESSION_thunder_practice( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}

// 拵呇撮夔-蜇旯扲
int PROFESSION_enclose( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ENCLOSE );		
	return TRUE;
}

// 拵呇撮夔-痄倛遙弇
int PROFESSION_transpose( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_TRANSPOSE );		
	return TRUE;
}


/*----------------------------------------------------------------------*/

// 蚋尪撮夔-惇僻
int PROFESSION_brust( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_BRUST );
	return TRUE;
}

// 蚋尪撮夔-蟀遠馴僻
int PROFESSION_chain_atk( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_CHAIN_ATK );
	return TRUE;
}

// 蚋尪撮夔-隙旌
int PROFESSION_avoid( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_AVOID );
	return TRUE;
}

// 蚋尪撮夔-硃悛
int PROFESSION_recovery( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_RECOVERY );
	return TRUE;
}

// 蚋尪撮夔-挕ん蚳儕
int PROFESSION_weapon_focus( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}

// 蚋尪撮夔-袨怓隙葩
int PROFESSION_reback( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_REBACK );		
	return TRUE;
}

// 蚋尪撮夔-邧笭馴僻
int PROFESSION_chain_atk_2( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_CHAIN_ATK_2 );		
	return TRUE;
}

// 蚋尪撮夔-忔眒峈衭
int PROFESSION_scapegoat( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_SCAPEGOAT );		
	return TRUE;
}

// 蚋尪撮夔-慾趙馴僻
int PROFESSION_enrage( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ENRAGE );		
	return TRUE;
}

// 蚋尪撮夔-夔講擄摩
int PROFESSION_energy_collect( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_COLLECT );		
	return TRUE;
}

// 蚋尪撮夔-蚳蛁桵須
int PROFESSION_focus( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_FOCUS );		
	return TRUE;
}

// 蚋尪撮夔-嗎僻
int PROFESSION_shield_attack( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_SHIELD_ATTACK );		
	return TRUE;
}

// 蚋尪撮夔-媼絮霜
int PROFESSION_dual_weapon( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}

// 蚋尪撮夔-跡紫
int PROFESSION_deflect( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_DEFLECT );		
	return TRUE;
}

// 蚋尪撮夔-嫗援馴僻
int PROFESSION_through_attack( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_THROUGH_ATTACK );		
	return TRUE;
}

// 蚋尪撮夔-釱る馴僻
int PROFESSION_cavalry( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_CAVALRY );		
	return TRUE;
}

// 蚋尪撮夔-梭侚馴僻
int PROFESSION_dead_attack( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_DEAD_ATTACK );		
	return TRUE;
}

// 蚋尪撮夔-隙唅馴僻
int PROFESSION_convolute( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_CONVOLUTE );		
	return TRUE;
}

// 蚋尪撮夔-髦觴馴僻
int PROFESSION_chaos( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_CHAOS );		
	return TRUE;
}

/*----------------------------------------------------------------------*/

// 轂�佷敵�-疪筘
int PROFESSION_trap( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_TRAP );		
	return TRUE;
}

// 轂�佷敵�-拲睦唾昜
int PROFESSION_docile( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_DOCILE );		
	return TRUE;
}

// 轂�佷敵�-慾躑唾昜
int PROFESSION_enrage_pet( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ENRAGE_PET );		
	return TRUE;
}

// 轂�佷敵�-毞蹕華厙
int PROFESSION_dragnet( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_DRAGNET );		
	return TRUE;
}

// 轂�佷敵�-攷跦莊��
int PROFESSION_entwine( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ENTWINE );		
	return TRUE;
}

// 轂�佷敵�-赻跤赻逋
int PROFESSION_autarky( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}

// 轂�佷敵�-  极謨嗤
int PROFESSION_plunder( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_PLUNDER );	
	return TRUE;
}

// 轂�佷敵�-馮匼挕ん
int PROFESSION_toxin_weapon( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_TOXIN_WEAPON );	
	return TRUE;
}

// 轂�佷敵�-鳶蕨俶枑汔
int PROFESSION_resist_fire( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_RESIST_FIRE );	
	return TRUE;
}

// 轂�佷敵�-梨蕨俶枑汔
int PROFESSION_resist_ice( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_RESIST_ICE );	
	return TRUE;
}

// 轂�佷敵�-濘蕨俶枑汔
int PROFESSION_resist_thunder( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_RESIST_THUNDER );	
	return TRUE;
}

#ifdef _PROFESSION_ADDSKILL
// 轂�佷敵�-赻�銓�夔
int PROFESSION_resist_f_i_t( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_RESIST_F_I_T );	
	return TRUE;
}
// 轂�佷敵�-瘍欸赻��
int PROFESSION_call_nature( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_CALL_NATURE );	
	return TRUE;
}
// 轂�佷敵�-侐扽俶賦賜
int PROFESSION_boundary( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_BOUNDARY );	
	return TRUE;
}
#endif

// 轂�佷敵�-芶极鳶蕨俶枑汔
int PROFESSION_g_resist_fire( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_G_RESIST_FIRE );	
	return TRUE;
}

// 轂�佷敵�-芶极梨蕨俶枑汔
int PROFESSION_g_resist_ice( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_G_RESIST_ICE );	
	return TRUE;
}

// 轂�佷敵�-芶极濘蕨俶枑汔
int PROFESSION_g_resist_thunder( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_G_RESIST_THUNDER );	
	return TRUE;
}

// 轂�佷敵�-�黤蓍本�
int PROFESSION_attack_weak( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ATTACK_WEAK );	
	return TRUE;
}

// 轂�佷敵�-泔畢
int PROFESSION_instigate( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_INSTIGATE );	
	return TRUE;
}

// 轂�佷敵�-疻咭
int PROFESSION_oblivion( int charaindex, int toNo, int array, char *data, int skill_level )
{
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_OBLIVION );	
	return TRUE;
}


/*----------------------------------------------------------------------*/
// 準桵須撮夔

// 轂�佷敵�-袚扆菩趿
int PROFESSION_track( int charaindex, int toNo, int array, char *data, int skill_level )
{
	char *pszP = NULL, *pszOption = NULL, msg[20];
	int per=0, rate=0, img1, ret = 1;
	skill_level /= 10;

	if( CHAR_getWorkInt( charaindex, CHAR_ENCOUNT_NUM) >= (int)time(NULL) ){
		ret = -1;
	}

	// 撮夔腔統杅
	pszOption = PROFESSION_SKILL_getChar( array, PROFESSION_SKILL_OPTION);

	// 撮夔脹撰 rate
	if( (pszP = strstr( pszOption, "捷%" ) ) != NULL )	sscanf( pszP+3, "%d", &rate );
	per = skill_level * rate;

	// 袧掘ぶ腔雄賒
	img1 = PROFESSION_SKILL_getInt( array, PROFESSION_SKILL_IMG_1 );

	CHAR_setWorkInt( charaindex, CHAR_ENCOUNT_FIX, per);		
	CHAR_talkToCli( charaindex, -1, "郣菩薹奻汔",  CHAR_COLORRED);
	CHAR_setWorkInt( charaindex, CHAR_ENCOUNT_NUM, (int)time(NULL) + (60 * 3) );

	sprintf( msg, "%d", img1);
	lssproto_BATTLESKILL_send ( getfdFromCharaIndex(charaindex), msg ) ; 

	return ret;
}

// 轂�佷敵�-隙旌桵須
int PROFESSION_escape( int charaindex, int toNo, int array, char *data, int skill_level )
{
	char *pszP = NULL, *pszOption = NULL, msg[20];
	int per=0, rate=0, img1, ret =1;
	skill_level /= 10;

	if( CHAR_getWorkInt( charaindex, CHAR_ENCOUNT_NUM) >= (int)time(NULL) ){
		ret = -1;
	}

	// 撮夔腔統杅
	pszOption = PROFESSION_SKILL_getChar( array, PROFESSION_SKILL_OPTION);

	// 撮夔脹撰 rate
	if( (pszP = strstr( pszOption, "捷%" ) ) != NULL )	sscanf( pszP+3, "%d", &rate );
	per = skill_level * rate;

	// 袧掘ぶ腔雄賒
	img1 = PROFESSION_SKILL_getInt( array, PROFESSION_SKILL_IMG_1 );

	CHAR_setWorkInt( charaindex, CHAR_ENCOUNT_FIX, (-1) * per);
	CHAR_talkToCli( charaindex, -1, "郣菩薹狟蔥",  CHAR_COLORGREEN);

	CHAR_setWorkInt( charaindex, CHAR_ENCOUNT_NUM, (int)time(NULL) + (60 * 3) );

	sprintf( msg, "%d", img1);
	lssproto_BATTLESKILL_send ( getfdFromCharaIndex(charaindex), msg ) ; 

	return ret;
}



// 僕肮撮夔-ァ薯喃驛
int PROFESSION_full_mp( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}
// 僕肮撮夔-蛹笭崝樓
int PROFESSION_strong_back( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}
// 僕肮撮夔-赻扂Ч趙
int PROFESSION_strengthen( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}

#endif

