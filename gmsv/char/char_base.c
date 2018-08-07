#include "version.h"
#include <string.h>

// CoolFish: 2001/10/12  _UNIQUE_P_I
#include <time.h>


#include "char_base.h"
#include "char_data.h"
#include "item.h"
#include "buf.h"
#include "magic.h"
#include "function.h"
#include "npccreate.h"
#include "configfile.h"
#include "title.h"
#include "pet.h"
#include "pet_skill.h"
#include "anim_tbl.h"
#include "enemy.h"
#include "saacproto_cli.h"

// CoolFish: 2001/10/29
#include "log.h"

Char* CHAR_chara;

static int      CHAR_charanum;
static int      CHAR_playernum;
static int      CHAR_petnum;
static int      CHAR_othersnum;

// CoolFish: 2001/10/11
#ifdef _UNIQUE_P_I
#define	MAX_UNIQUE_P_I	1000
static int unique_p = 0;
static int unique_i = 0;
#endif

typedef struct tagINITCHARCOUNTER{
	int startcnt;
	int cnt;
	int endcnt;
}INITCHARCOUNTER;
static INITCHARCOUNTER  initCharCounter[3] = {
	{-1,-1,-1},{-1,-1,-1},{-1,-1,-1}
};


tagRidePetTable ridePetTable[296] =
{
	{ 101000, 100000, 100352, 331 },
	{ 101001, 100005, 100352, 331 },
	{ 101002, 100010, 100352, 331 },
	{ 101003, 100015, 100352, 331 },
	{ 101004, 100000, 100329, 309 },
	{ 101005, 100005, 100327, 307 },
	{ 101006, 100010, 100330, 310 },
	{ 101007, 100015, 100328, 308 },
//	{ 101008, 100700, 100372, 351 },
//	{ 101009, 100705, 100373, 352 },
#ifdef _GM_METAMO_RIDE
	{ 101000, 100700, 100352, 331 },
	{ 101001, 100700, 100352, 331 },
	{ 101002, 100700, 100352, 331 },
	{ 101003, 100700, 100352, 331 },
	{ 101004, 100700, 100329, 309 },
	{ 101005, 100700, 100327, 307 },
	{ 101006, 100700, 100330, 310 },
	{ 101007, 100700, 100328, 308 },
	{ 101000, 100705, 100352, 331 },
	{ 101001, 100705, 100352, 331 },
	{ 101002, 100705, 100352, 331 },
	{ 101003, 100705, 100352, 331 },
	{ 101004, 100705, 100329, 309 },
	{ 101005, 100705, 100327, 307 },
	{ 101006, 100705, 100330, 310 },
	{ 101007, 100705, 100328, 308 },
#endif

	{ 101010, 100020, 100396, 483 },
	{ 101011, 100025, 100396, 483 },
	{ 101012, 100030, 100396, 483 },
	{ 101013, 100035, 100396, 483 },
	{ 101014, 100020, 100328, 308 },
	{ 101015, 100025, 100329, 309 },
	{ 101016, 100030, 100327, 307 },
	{ 101017, 100035, 100330, 310 },
//	{ 101018, 100710, 100372, 351 },
//	{ 101019, 100715, 100373, 352 },
#ifdef _GM_METAMO_RIDE
	{ 101010, 100710, 100396, 483 },
	{ 101011, 100710, 100396, 483 },
	{ 101012, 100710, 100396, 483 },
	{ 101013, 100710, 100396, 483 },
	{ 101014, 100710, 100328, 308 },
	{ 101015, 100710, 100329, 309 },
	{ 101016, 100710, 100327, 307 },
	{ 101017, 100710, 100330, 310 },
	{ 101010, 100715, 100396, 483 },
	{ 101011, 100715, 100396, 483 },
	{ 101012, 100715, 100396, 483 },
	{ 101013, 100715, 100396, 483 },
	{ 101014, 100715, 100328, 308 },
	{ 101015, 100715, 100329, 309 },
	{ 101016, 100715, 100327, 307 },
	{ 101017, 100715, 100330, 310 },
	
	{ 100420, 100710, 100369, 348 },
	{ 100425, 100710, 100370, 349 },
	{ 100445, 100710, 100904, 1642 },
	
	{ 100420, 100715, 100369, 348 },
	{ 100425, 100715, 100370, 349 },
	{ 100445, 100715, 100904, 1642 },
#endif

	{ 101020, 100040, 100351, 330 },
	{ 101021, 100045, 100351, 330 },
	{ 101022, 100050, 100351, 330 },
	{ 101023, 100055, 100351, 330 },
	{ 101024, 100040, 100328, 308 },
	{ 101025, 100045, 100330, 310 },
	{ 101026, 100050, 100327, 307 },
	{ 101027, 100055, 100329, 309 },
//	{ 101028, 100720, 100372, 351 },
//	{ 101029, 100725, 100373, 352 },
#ifdef _GM_METAMO_RIDE
	{ 101020, 100720, 100351, 330 },
	{ 101021, 100720, 100351, 330 },
	{ 101022, 100720, 100351, 330 },
	{ 101023, 100720, 100351, 330 },
	{ 101024, 100720, 100328, 308 },
	{ 101025, 100720, 100330, 310 },
	{ 101026, 100720, 100327, 307 },
	{ 101027, 100720, 100329, 309 },
	{ 101020, 100725, 100351, 330 },
	{ 101021, 100725, 100351, 330 },
	{ 101022, 100725, 100351, 330 },
	{ 101023, 100725, 100351, 330 },
	{ 101024, 100725, 100328, 308 },
	{ 101025, 100725, 100330, 310 },
	{ 101026, 100725, 100327, 307 },
	{ 101027, 100725, 100329, 309 },
#endif

	{ 101030, 100060, 100353, 332 },
	{ 101031, 100065, 100353, 332 },
	{ 101032, 100070, 100353, 332 },
	{ 101033, 100075, 100353, 332 },
	{ 101034, 100060, 100329, 309 },
	{ 101035, 100065, 100327, 307 },
	{ 101036, 100070, 100330, 310 },
	{ 101037, 100075, 100328, 308 },
//	{ 101038, 100730, 100372, 351 },
//	{ 101039, 100735, 100373, 352 },
#ifdef _GM_METAMO_RIDE
	{ 101030, 100730, 100353, 332 },
	{ 101031, 100730, 100353, 332 },
	{ 101032, 100730, 100353, 332 },
	{ 101033, 100730, 100353, 332 },
	{ 101034, 100730, 100329, 309 },
	{ 101035, 100730, 100327, 307 },
	{ 101036, 100730, 100330, 310 },
	{ 101037, 100730, 100328, 308 },
	{ 101030, 100735, 100353, 332 },
	{ 101031, 100735, 100353, 332 },
	{ 101032, 100735, 100353, 332 },
	{ 101033, 100735, 100353, 332 },
	{ 101034, 100735, 100329, 309 },
	{ 101035, 100735, 100327, 307 },
	{ 101036, 100735, 100330, 310 },
	{ 101037, 100735, 100328, 308 },
#endif

	{ 101040, 100080, 100396, 483 },
	{ 101041, 100085, 100396, 483 },
	{ 101042, 100090, 100396, 483 },
	{ 101043, 100095, 100396, 483 },
	{ 101044, 100080, 100328, 308 },
	{ 101045, 100085, 100327, 307 },
	{ 101046, 100090, 100330, 310 },
	{ 101047, 100095, 100329, 309 },
//	{ 101048, 100740, 100372, 351 },
//	{ 101049, 100745, 100373, 352 },
#ifdef _GM_METAMO_RIDE
	{ 101040, 100740, 100396, 483 },
	{ 101041, 100740, 100396, 483 },
	{ 101042, 100740, 100396, 483 },
	{ 101043, 100740, 100396, 483 },
	{ 101044, 100740, 100328, 308 },
	{ 101045, 100740, 100327, 307 },
	{ 101046, 100740, 100330, 310 },
	{ 101047, 100740, 100329, 309 },
	{ 101040, 100740, 100396, 483 },
	{ 101041, 100740, 100396, 483 },
	{ 101042, 100740, 100396, 483 },
	{ 101043, 100740, 100396, 483 },
	{ 101044, 100740, 100328, 308 },
	{ 101045, 100740, 100327, 307 },
	{ 101046, 100740, 100330, 310 },
	{ 101047, 100740, 100329, 309 },
#endif

	{ 101050, 100100, 100353, 332 },
	{ 101051, 100105, 100353, 332 },
	{ 101052, 100110, 100353, 332 },
	{ 101053, 100115, 100353, 332 },
	{ 101054, 100100, 100329, 309 },
	{ 101055, 100105, 100328, 308 },
	{ 101056, 100110, 100330, 310 },
	{ 101057, 100115, 100327, 307 },
//	{ 101058, 100750, 100372, 351 },
//	{ 101059, 100755, 100373, 352 },
#ifdef _GM_METAMO_RIDE
	{ 101050, 100750, 100353, 332 },
	{ 101051, 100750, 100353, 332 },
	{ 101052, 100750, 100353, 332 },
	{ 101053, 100750, 100353, 332 },
	{ 101054, 100750, 100329, 309 },
	{ 101055, 100750, 100328, 308 },
	{ 101056, 100750, 100330, 310 },
	{ 101057, 100750, 100327, 307 },
	{ 101050, 100755, 100353, 332 },
	{ 101051, 100755, 100353, 332 },
	{ 101052, 100755, 100353, 332 },
	{ 101053, 100755, 100353, 332 },
	{ 101054, 100755, 100329, 309 },
	{ 101055, 100755, 100328, 308 },
	{ 101056, 100755, 100330, 310 },
	{ 101057, 100755, 100327, 307 },
#endif

	{ 101060, 100120, 100354, 333 },
	{ 101061, 100125, 100354, 333 },
	{ 101062, 100130, 100354, 333 },
	{ 101063, 100135, 100354, 333 },
	{ 101064, 100120, 100327, 307 },
	{ 101065, 100125, 100330, 310 },
	{ 101066, 100130, 100328, 308 },
	{ 101067, 100135, 100329, 309 },
//	{ 101068, 100760, 100372, 351 },
//	{ 101069, 100765, 100373, 352 },
#ifdef _GM_METAMO_RIDE
	{ 101060, 100760, 100354, 333 },
	{ 101061, 100760, 100354, 333 },
	{ 101062, 100760, 100354, 333 },
	{ 101063, 100760, 100354, 333 },
	{ 101064, 100760, 100327, 307 },
	{ 101065, 100760, 100330, 310 },
	{ 101066, 100760, 100328, 308 },
	{ 101067, 100760, 100329, 309 },
	{ 101060, 100765, 100354, 333 },
	{ 101061, 100765, 100354, 333 },
	{ 101062, 100765, 100354, 333 },
	{ 101063, 100765, 100354, 333 },
	{ 101064, 100765, 100327, 307 },
	{ 101065, 100765, 100330, 310 },
	{ 101066, 100765, 100328, 308 },
	{ 101067, 100765, 100329, 309 },
#endif

	{ 101070, 100140, 100354, 333 },
	{ 101071, 100145, 100354, 333 },
	{ 101072, 100150, 100354, 333 },
	{ 101073, 100155, 100354, 333 },
	{ 101074, 100140, 100327, 307 },
	{ 101075, 100145, 100329, 309 },
	{ 101076, 100150, 100330, 310 },
	{ 101077, 100155, 100328, 308 },
//	{ 101078, 100770, 100372, 351 },
//	{ 101079, 100775, 100373, 352 },
#ifdef _GM_METAMO_RIDE
	{ 101070, 100770, 100354, 333 },
	{ 101071, 100770, 100354, 333 },
	{ 101072, 100770, 100354, 333 },
	{ 101073, 100770, 100354, 333 },
	{ 101074, 100770, 100327, 307 },
	{ 101075, 100770, 100329, 309 },
	{ 101076, 100770, 100330, 310 },
	{ 101077, 100770, 100328, 308 },
	{ 101070, 100775, 100354, 333 },
	{ 101071, 100775, 100354, 333 },
	{ 101072, 100775, 100354, 333 },
	{ 101073, 100775, 100354, 333 },
	{ 101074, 100775, 100327, 307 },
	{ 101075, 100775, 100329, 309 },
	{ 101076, 100775, 100330, 310 },
	{ 101077, 100775, 100328, 308 },
#endif

	{ 101080, 100160, 100352, 331 },
	{ 101081, 100165, 100352, 331 },
	{ 101082, 100170, 100352, 331 },
	{ 101083, 100175, 100352, 331 },
	{ 101084, 100160, 100330, 310 },
	{ 101085, 100165, 100329, 309 },
	{ 101086, 100170, 100327, 307 },
	{ 101087, 100175, 100328, 308 },
//	{ 101088, 100780, 100372, 351 },
//	{ 101089, 100785, 100373, 352 },
#ifdef _GM_METAMO_RIDE
	{ 101040, 100780, 100396, 483 },
	{ 101041, 100780, 100396, 483 },
	{ 101042, 100780, 100396, 483 },
	{ 101043, 100780, 100396, 483 },
	{ 101044, 100780, 100328, 308 },
	{ 101045, 100780, 100327, 307 },
	{ 101046, 100780, 100330, 310 },
	{ 101047, 100780, 100329, 309 },
	{ 101080, 100785, 100352, 331 },
	{ 101081, 100785, 100352, 331 },
	{ 101082, 100785, 100352, 331 },
	{ 101083, 100785, 100352, 331 },
	{ 101084, 100785, 100330, 310 },
	{ 101085, 100785, 100329, 309 },
	{ 101086, 100785, 100327, 307 },
	{ 101087, 100785, 100328, 308 },
#endif

	{ 101090, 100180, 100351, 330 },
	{ 101091, 100185, 100351, 330 },
	{ 101092, 100190, 100351, 330 },
	{ 101093, 100195, 100351, 330 },
	{ 101094, 100180, 100328, 308 },
	{ 101095, 100185, 100330, 310 },
	{ 101096, 100190, 100329, 309 },
	{ 101097, 100195, 100327, 307 },
//	{ 101098, 100790, 100372, 351 },
//	{ 101099, 100795, 100373, 352 },
#ifdef _GM_METAMO_RIDE
	{ 101090, 100790, 100351, 330 },
	{ 101091, 100790, 100351, 330 },
	{ 101092, 100790, 100351, 330 },
	{ 101093, 100790, 100351, 330 },
	{ 101094, 100790, 100328, 308 },
	{ 101095, 100790, 100330, 310 },
	{ 101096, 100790, 100329, 309 },
	{ 101097, 100790, 100327, 307 },
	{ 101090, 100795, 100351, 330 },
	{ 101091, 100795, 100351, 330 },
	{ 101092, 100795, 100351, 330 },
	{ 101093, 100795, 100351, 330 },
	{ 101094, 100795, 100328, 308 },
	{ 101095, 100795, 100330, 310 },
	{ 101096, 100795, 100329, 309 },
	{ 101097, 100795, 100327, 307 },
#endif

	{ 101100, 100200, 100353, 332 },
	{ 101101, 100205, 100353, 332 },
	{ 101102, 100210, 100353, 332 },
	{ 101103, 100215, 100353, 332 },
	{ 101104, 100200, 100329, 309 },
	{ 101105, 100205, 100328, 308 },
	{ 101106, 100210, 100327, 307 },
	{ 101107, 100215, 100330, 310 },
//	{ 101108, 100800, 100372, 351 },
//	{ 101109, 100805, 100373, 352 },
#ifdef _GM_METAMO_RIDE
	{ 101100, 100800, 100353, 332 },
	{ 101101, 100800, 100353, 332 },
	{ 101102, 100800, 100353, 332 },
	{ 101103, 100800, 100353, 332 },
	{ 101104, 100800, 100329, 309 },
	{ 101105, 100800, 100328, 308 },
	{ 101106, 100800, 100327, 307 },
	{ 101107, 100800, 100330, 310 },
	{ 101100, 100805, 100353, 332 },
	{ 101101, 100805, 100353, 332 },
	{ 101102, 100805, 100353, 332 },
	{ 101103, 100805, 100353, 332 },
	{ 101104, 100805, 100329, 309 },
	{ 101105, 100805, 100328, 308 },
	{ 101106, 100805, 100327, 307 },
	{ 101107, 100805, 100330, 310 },
#endif

	{ 101110, 100220, 100396, 483 },
	{ 101111, 100225, 100396, 483 },
	{ 101112, 100230, 100396, 483 },
	{ 101113, 100235, 100396, 483 },
	{ 101114, 100220, 100330, 310 },
	{ 101115, 100225, 100327, 307 },
	{ 101116, 100230, 100329, 309 },
	{ 101117, 100235, 100328, 308 },
//	{ 101118, 100810, 100372, 351 },
//	{ 101119, 100815, 100373, 352 },
#ifdef _GM_METAMO_RIDE
	{ 101110, 100810, 100396, 483 },
	{ 101111, 100810, 100396, 483 },
	{ 101112, 100810, 100396, 483 },
	{ 101113, 100810, 100396, 483 },
	{ 101114, 100810, 100330, 310 },
	{ 101115, 100810, 100327, 307 },
	{ 101116, 100810, 100329, 309 },
	{ 101117, 100810, 100328, 308 },
	{ 101110, 100815, 100396, 483 },
	{ 101111, 100815, 100396, 483 },
	{ 101112, 100815, 100396, 483 },
	{ 101113, 100815, 100396, 483 },
	{ 101114, 100815, 100330, 310 },
	{ 101115, 100815, 100327, 307 },
	{ 101116, 100815, 100329, 309 },
	{ 101117, 100815, 100328, 308 },
#endif

	{ 101179, 101177, 101178, 1804},
	{ 101179, 101178, 101177, 1926},
};

#ifdef _NEW_RIDEPETS

tagRideCodeMode RideCodeMode[] = {
	{ 100374, RIDE_PET0 },  /*骑宠帖拉所伊朵*/
	{ 100358, RIDE_PET1 },  /*骑宠玛恩摩洛斯*/
	{ 100362, RIDE_PET2 },  /*骑宠朵拉比斯*/
	{ 100279, RIDE_PET3 },  /*骑宠拉奇鲁哥*/
	{ 100288, RIDE_PET4 },  /*骑宠扬奇洛斯*/
	{ 100283, RIDE_PET5 },  /*骑宠卡达鲁卡斯*/
	{ 100346, RIDE_PET6 },  /*骑宠卡卡金宝*/
	{ 100310, RIDE_PET7 },  /*骑宠格尔格*/
	{ 100372, RIDE_PET8 },  /*骑宠左迪洛斯*/
	{ 100373, RIDE_PET9 },  /*骑宠巴朵兰恩*/
	{ 101532, RIDE_PET10 }, /*镲宠史卡鲁*/
	{ 101576, RIDE_PET11 }, /*骑宠罗多克雷*/
};

tagRideNoList RideNoList[] = {
	{{ 101305, 101306, 101307, 101308, 101309, 101310, 101311, 101312, 101008, 101009, 101978, 101989},0},	//小矮子
	{{ 101313, 101314, 101315, 101316, 101317, 101318, 101319, 101320, 101018, 101019, 101986, 101988},0},	//赛亚人
	{{ 101321, 101322, 101323, 101324, 101325, 101326, 101327, 101328, 101028, 101029, 101975, 101965},0},	//辫子男孩
	{{ 101329, 101330, 101331, 101332, 101333, 101334, 101335, 101336, 101038, 101039, 101976, 101966},0},	//酷哥
	{{ 101337, 101338, 101339, 101340, 101341, 101342, 101343, 101344, 101048, 101049, 101984, 101973},0},	//熊皮男
	{{ 101345, 101346, 101347, 101348, 101349, 101350, 101351, 101352, 101058, 101059, 101985, 101974},0},	//大个
	{{ 101353, 101354, 101355, 101356, 101357, 101358, 101359, 101360, 101068, 101069, 101977, 101967},0},	//小矮妹
	{{ 101361, 101362, 101363, 101364, 101365, 101366, 101367, 101368, 101078, 101079, 101979, 101968},0},	//熊皮妹
	{{ 101369, 101370, 101371, 101372, 101373, 101374, 101375, 101376, 101088, 101089, 101981, 101970},0},	//帽子妹
	{{ 101377, 101378, 101379, 101380, 101381, 101382, 101383, 101384, 101098, 101099, 101980, 101969},0},	//短法发夹妹
	{{ 101385, 101386, 101387, 101388, 101389, 101390, 101391, 101392, 101108, 101109, 101983, 101972},0},	//手套女
	{{ 101393, 101394, 101395, 101396, 101397, 101398, 101399, 101400, 101118, 101119, 101982, 101970},0},	//辣妹
};

tagRidePetList RPlistMode[] = {
	{ 100000, 0, 1 }, { 100005, 0, 1 }, { 100010, 0, 1 }, { 100015, 0, 1 }, { 100700, 0, 1 }, { 100705, 0, 1 },	//小矮子
	{ 100020, 1, 1 }, { 100025, 1, 1 }, { 100030, 1, 1 }, { 100035, 1, 1 },	{ 100710, 1, 1 }, { 100715, 1, 1 },//赛亚人
	{ 100040, 2, 1 }, { 100045, 2, 1 }, { 100050, 2, 1 }, { 100055, 2, 1 }, { 100720, 2, 1 }, { 100725, 2, 1 },	//辫子男孩
	{ 100060, 3, 1 }, { 100065, 3, 1 }, { 100070, 3, 1 }, { 100075, 3, 1 },	{ 100730, 3, 1 }, { 100735, 3, 1 },	//酷哥
	{ 100080, 4, 1 }, { 100085, 4, 1 }, { 100090, 4, 1 }, { 100095, 4, 1 },	{ 100740, 4, 1 }, { 100745, 4, 1 },	//熊皮男
	{ 100100, 5, 1 }, { 100105, 5, 1 }, { 100110, 5, 1 }, { 100115, 5, 1 },	{ 100750, 5, 1 }, { 100755, 5, 1 },	//大个
	{ 100120, 6, 2 }, { 100125, 6, 2 }, { 100130, 6, 2 }, { 100135, 6, 2 },	{ 100760, 6, 2 }, { 100765, 6, 2 },	//小矮妹
	{ 100140, 7, 2 }, { 100145, 7, 2 }, { 100150, 7, 2 }, { 100155, 7, 2 },	{ 100770, 7, 2 }, { 100775, 7, 2 },	//熊皮妹
	{ 100160, 8, 2 }, { 100165, 8, 2 }, { 100170, 8, 2 }, { 100175, 8, 2 },	{ 100780, 8, 2 }, { 100785, 8, 2 },	//帽子妹
	{ 100180, 9, 2 }, { 100185, 9, 2 }, { 100190, 9, 2 }, { 100195, 9, 2 },	{ 100790, 9, 2 }, { 100795, 9, 2 },	//短发夹妹
	{ 100200, 10, 2 }, { 100205, 10, 2 }, { 100210, 10, 2 }, { 100215, 10, 2 }, { 100800, 10, 2 }, { 100805, 10, 2 },	//手套女
	{ 100220, 11, 2 }, { 100225, 11, 2 }, { 100230, 11, 2 }, { 100235, 11, 2 },	{ 100810, 11, 2 }, { 100815, 11, 2 },	//辣妹
};

int RIDEPET_getNOindex( int baseNo)
{
	int i;
	for( i=0; i< sizeof(RPlistMode)/sizeof(tagRidePetList); i++ )	{
		if( RPlistMode[i].charNo == baseNo )	{
			return RPlistMode[i].Noindex;
		}
	}
	return -1;
}

int RIDEPET_getPETindex( int PetNo, int learnCode)
{
	int i;
	for( i=0; i< sizeof(RideCodeMode)/sizeof(tagRideCodeMode); i++ )	{
		if( RideCodeMode[i].petNo == PetNo && 
			(RideCodeMode[i].learnCode & learnCode) )	{
			return i;
		}
	}
	return -1;
}

int RIDEPET_getRIDEno( int index, int ti)
{
	if( index < 0 || index >= sizeof( RideNoList)/sizeof( tagRideNoList) )
		return -1;
	if( ti < 0 || ti >= MAXNOINDEX )
		return -1;

	return RideNoList[index].RideNo[ti];
}
#endif

#ifdef _EMENY_CHANCEMAN
int CHAR_getSexInt( int baseNo )
{
	int i;
	for( i=0; i< sizeof(RPlistMode)/sizeof(tagRidePetList); i++ )	{
		if( RPlistMode[i].charNo == baseNo )	{
			return RPlistMode[i].sex;
		}
	}
	return -1;
}
#endif

#ifdef _PET_EVOLUTION
#define MAXTABLE_X 29
#define MAXTABLE_Y 29
int PetTable[MAXTABLE_X][MAXTABLE_Y]={
	//0           4              9             14             19             24
	{ 1, 2, 5, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 5, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5},
	{ 2, 5, 1, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1},
	{ 5, 1, 2, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2},
	{ 1, 2, 5, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5},
	{ 2, 5, 1, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1},

	{ 2, 1, 2, 1, 2, 2, 2, 5, 5, 1, 2, 5, 2, 1, 2, 1, 2, 5, 2, 1, 2, 1, 2, 5, 2, 5, 2, 1, 2},
	{ 1, 2, 5, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 1},
	{ 2, 5, 1, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 2},
	{10, 3,10, 3,10, 8,10, 8,10, 3,10, 3,10, 8,10, 8,10, 3,10, 3,10, 8,10, 8,10, 8,10, 3,10},
	{ 3, 8, 3, 8, 3,10, 3,10, 8, 3, 3, 8, 3, 8, 3,10, 3,10, 3, 8, 3, 8, 3,10, 3,10, 3, 8, 3},

	{ 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8},
	{ 5, 1, 5, 1, 5, 2, 5, 2, 5, 1, 5, 5, 5, 1, 5, 2, 5, 2, 5, 1, 5, 1, 5, 2, 5, 2, 5, 1, 5},
	{ 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10},
	{10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3},
	{ 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8},

	{ 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1},
	{ 9, 0, 9, 0, 9, 4, 9, 4, 9, 0, 9, 0, 9, 4, 9, 4, 9, 0, 9, 0, 9, 4, 9, 4, 9, 0, 9, 4, 9},
	{ 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2},
	{ 6, 0, 6, 0, 6, 9, 6, 9, 6, 0, 6, 0, 6, 9, 6, 9, 6, 0, 6, 0, 6, 9, 6, 9, 6, 0, 6, 9, 6},
	{ 4, 6, 4, 6, 4, 9, 4, 9, 4, 6, 4, 6, 4, 9, 4, 9, 4, 6, 4, 6, 4, 9, 4, 9, 4, 6, 4, 9, 4},

	{ 8, 3, 8, 3, 8,10, 8,10, 8, 3, 8, 3, 8,10, 8,10, 8, 3, 8, 3, 8,10, 8,10, 8, 3, 8,10, 8},
	{ 8,10, 3, 8,10, 3, 8,10,10, 3, 8,10, 3, 8,10, 3,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10},
	{ 1, 2, 5, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5},
	{ 0, 4, 0, 4, 0, 6, 0, 6, 0, 4, 0, 4, 0, 6, 0, 6, 0, 4, 0, 4, 0, 6, 0, 4, 0, 6, 0, 4, 0},
	{ 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2},

	{ 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8},
	{10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3},
	{ 2, 1, 2, 1, 5, 1, 5, 1, 2, 1, 2, 1, 5, 1, 5, 1, 2, 1, 2, 1, 5, 1, 5, 1, 2, 1, 2, 5, 1},
	{ 6, 9, 6, 9, 6, 0, 6, 0, 6, 9, 6, 9, 6, 0, 6, 0, 6, 9, 6, 9, 6, 0, 6, 0, 6, 9, 6, 0, 6}
};

int PropertyTable[4][4]={
	{ 0, 4, 5, 6},
	{ 7, 1, 8, 9},
	{10,11, 2,12},
	{13,14,15, 3},
};
#define MAXFTABLE_X 11
#define MAXFTABLE_Y	16
int FusionTable[MAXFTABLE_X][MAXFTABLE_Y]={
	{ 989, 990, 991, 992, 989, 992, 989, 990, 990, 990, 991, 991, 991, 992, 989, 992},
	{1001,1002,1003,1004,1001,1001,1004,1001,1002,1002,1003,1003,1003,1004,1004,1003},
	{1005,1006,1007,1008,1005,1005,1005,1006,1006,1006,1007,1006,1007,1008,1008,1008},
	{1021,1025,1023,1024,1025,1021,1021,1022,1022,1022,1023,1023,1023,1021,1024,1024},
	{1030,1031,1032,1033,1030,1030,1030,1031,1031,1031,1032,1031,1032,1030,1033,1033},
	{1017,1018,1019,1020,1018,1017,1017,1018,1019,1018,1019,1019,1020,1017,1020,1020},
	{1009,1010,1011,1012,1010,1009,1009,1010,1010,1010,1011,1011,1011,1012,1012,1011},
	{ 993, 994, 995, 996, 994, 993, 993, 994, 995, 994, 995, 993, 996, 993, 996, 996},
	{1026,1027,1028,1029,1026,1026,1026,1026,1028,1027,1028,1028,1029,1029,1029,1029},
	{ 997, 998, 999, 999,1000, 997, 997,1000, 998, 998,1000, 998, 999, 999, 999, 999},
	{1013,1014,1015,1016,1013,1013,1016,1013,1015,1014,1015,1015,1015,1016,1016,1016}
};

int EVOLUTION_getPetFusionCode( int petid)
{
	int petarray = ENEMYTEMP_getEnemyTempArrayFromTempNo( petid);
	return ENEMYTEMP_getInt( petarray, E_T_FUSIONCODE);
}

int EVOLUTION_getPetTable( int charaindex, int petindex1, int petindex2)
{
	int table1,table2;

	if( !CHAR_CHECKINDEX( petindex1) ) return -1;
	if( !CHAR_CHECKINDEX( petindex2) ) return -1;

	table2 = EVOLUTION_getPetFusionCode( CHAR_getInt( petindex1, CHAR_PETID));
	table1 = EVOLUTION_getPetFusionCode( CHAR_getInt( petindex2, CHAR_PETID));

	if( table1 < 0 || table1 >= MAXTABLE_X ) return -1;
	if( table2 < 0 || table2 >= MAXTABLE_Y ) return -1;
	return PetTable[table1][table2];
}

int EVOLUTION_getPropertyTable( int charaindex, int petindex1, int petindex2)
{
	int i, k1=0, k2=0;
	int table1,table2;
	int Property1[4]={-1,-1,-1,-1};
	int Property2[4]={-1,-1,-1,-1};

	if( !CHAR_CHECKINDEX( petindex1) ) return -1;
	if( !CHAR_CHECKINDEX( petindex2) ) return -1;

	for( i=0; i<4; i++)	{
		if( CHAR_getInt( petindex1, CHAR_EARTHAT+i) > 0 ){
			Property1[k1] = i;
			k1++;
		}
		if( CHAR_getInt( petindex2, CHAR_EARTHAT+i) > 0 ){
			Property2[k2] = i;
			k2++;
		}
	}
	if( k1 == 0 || k2 == 0 ) return -1;

	if( k1 <= 1 )	{
		table1 = Property1[ 0];
	}else{
		table1 = Property1[ (rand()%(k1-1)) ];
	}
	if( k2 <= 1 )	{
		table2 = Property2[ 0];
	}else	{
		table2 = Property2[ (rand()%(k2-1)) ];
	}

	if( table1 < 0 || table1 >= 4 ) return -1;
	if( table2 < 0 || table2 >= 4 ) return -1;

	return PropertyTable[table1][table2];
}

int EVOLUTION_getFusionTable( int charaindex, int px, int py)
{
	if( px >= MAXFTABLE_X || px < 0 ) return -1;
	if( py >= MAXFTABLE_Y || py < 0 ) return -1;
	return FusionTable[px][py];
}
#endif

#ifdef _CHAR_FIXDATADEF
/*
typedef struct _tagPetSetIntData{
	char com[256];
	int type;
}PetSetIntData;

PetSetIntData PET_SetIntData[ ]={
};
*/
#endif

static char* CHAR_setintdata[CHAR_DATAINTNUM]={
    /* dci  , PLAYER,  */
	"pn",				/*  CHAR_DATAPLACENUMBER */

    "bi",               /*  CHAR_BASEIMAGENUMBER */
    "bbi",              /*  CHAR_BASEBASEIMAGENUMBER */
	"fb",				/*  CHAR_FACEIMAGENUMBER */
    "fl",               /*  CHAR_FLOOR  */
    "x",                /*  CHAR_X */
    "y",                /*  CHAR_Y  */
    "dir",              /*  CHAR_DIR 12凛毛0卞凛煌璃曰卞 */
	"lv",               /*  CHAR_LV  */
    "gld",              /*  CHAR_GOLD    */

    "hp",               /*  CHAR_HP  */
    "mp",               /*  CHAR_MP  */
    "mmp",              /*  CHAR_MAXMP   */

	"vi",				/*  CHAR_VITAL	 */
    "str",              /*  CHAR_STR */
    "tou",              /*  CHAR_TOUGH */
	"dx",				/*  CHAR_DEX	 */

	"chr",				/*  CHAR_CHARM	*/
	"luc",				/*  CHAR_LUCK	*/
	"aea",				/*  哗箪岭 */
	"awa",				/*    箪岭 */
	"afi",				/*  绍箪岭 */
	"awi",				/*  氘箪岭 */

	"slt",				/*  CHAR_SLOT */
	"cr",				/*  CHAR_CRITIAL */
	"cou",				/*  CHAR_COUNTER */
	"rar",				/*  CHAR_RARE */
    "rst",              /*  CHAR_RADARSTRLENGTH */
    "cvo",              /*  CHAR_CHATVOLUME */
    "ml",               /*  CHAR_MERCHANTLEVEL */
    "hl",               /*  CHAR_HEALERLEVEL */
    "di",               /*  CHAR_DETERMINEITEM  */
    "ieqt",             /*  CHAR_INDEXOFEQTITLE  */
    "poi",              /*  CHAR_POISON  */
    "par",              /*  CHAR_PARALYSIS  */
    "sil",              /*  CHAR_SILENCE    */
    "sto",              /*  CHAR_STONE  */
    "dar",              /*  CHAR_DARKNESS   */
    "con",              /*  CHAR_CONFUSION  */
    "loc",              /*  CHAR_LOGINCOUNT */
    "dc",               /*  CHAR_DEADCOUNT  */
    "wc",               /*  CHAR_WALKCOUNT  */
    "tc",               /*  CHAR_TALKCOUNT  */
    "dmc",				/*  CHAR_DAMAGECOUNT */
    "gpc",				/*  CHAR_GETPETCOUNT */
    "kpc",				/*  CHAR_KILLPETCOUNT */
    "dpc",				/*  CHAR_DEADPETCOUNT */
	"smc",				/*  CHAR_SENDMAILCOUNT */
	"mic",				/*  CHAR_MERGEITEMCOUNT */
	"dbc",				/*  CHAR_DUELBATTLECOUNT */
	"dwc",				/*  CHAR_DUELWINCOUNT */
	"dlc",				/*  CHAR_DUELLOSECOUNT */
    "dswc",				/*  CHAR_DUELSTWINCOUNT */
    "dmswc",			/*  CHAR_DUELMAXSTWINCOUNT */
    "wht",              /*  CHAR_WHICHTYPE  */
    "wint",             /*  CHAR_WALKNTERVAL */
    "lint",             /*  CHAR_LOOPINTERVAL */
#ifdef _NEWOPEN_MAXEXP
	"exp",              /*  CHAR_OLDEXP  */
	"nexp",				/*  CHAR_EXP  */
#else
    "exp",              /*  CHAR_EXP  */
#endif
    "lte",              /*  CHAR_LASTTALKELDER  */
    "skup",             /*  CHAR_SKILLUPPOINT   */
	"lvup",				/*  CHAR_LEVELUPPOINT	*/
    "ity",              /*  CHAR_IMAGETYPE   */
	"nc",				/*  CHAR_NAMECOLOR */
	"pnc",				/*  CHAR_POPUPNAMECOLOR */
	"llt",				/*  CHAR_LASTTIMESETLUCK	*/
	"duel",				/*  CHAR_DUELPOINT	*/
	"evt",				/*  CHAR_EVENT	*/
	"evt2",				/*  CHAR_EVENT2	*/
	"evt3",				/*  CHAR_EVENT3	*/
#ifdef _NEWEVENT
	"evt4",				/*  CHAR_EVENT4	*/
	"evt5",				/*  CHAR_EVENT5	*/
	"evt6",				/*  CHAR_EVENT6	*/
#endif
#ifdef _ADD_NEWEVENT              // WON 多增任务旗标
	"evt7",				/*  CHAR_EVENT7	*/
	"evt8",				//	CHAR_ENDEVENT8, 224~255 精灵召唤专用
#endif

	"nev",				/*  CHAR_NOWEVENT	*/
	"nev2",				/*  CHAR_NOWEVENT2	*/
	"nev3",				/*  CHAR_NOWEVENT3	*/
#ifdef _NEWEVENT
	"nev4",				/*  CHAR_NOWEVENT4	*/
	"nev5",				/*  CHAR_NOWEVENT5	*/
	"nev6",				/*  CHAR_NOWEVENT6	*/
#endif
#ifdef _ADD_NEWEVENT              // WON 多增任务旗标
	"nev7",				/*  CHAR_NOWEVENT7	*/
	"nev8",				//	CHAR_NOWEVENT8, 224~255 精灵召唤专用
#endif
    
	"trn",				/*  CHAR_TRANSMIGRATION */
    "teq",				/*  CHAR_TRANSEQUATION */
    
    "ini",              /*  CHAR_INITDATA   */
    "silent",              /*  CHAR_SILENT   */    
    "fmi",		// CoolFish: Family 2001/5/24 家族 index
    "fmlf",		// CoolFish: Family 2001/5/24 是否为家族族长
    "fmspr",		// CoolFish: Family 2001/7/13 家族守护精灵

    "bankgld",		/* CHAR_BANKGOLD */
    "ridepet",		/* CHAR_RIDEPET */
    "learnride",	/* CHAR_LEARNRIDE */
#ifdef _NEW_RIDEPETS
	"lowridepet",	//CHAR_LOWRIDEPETS
#endif
    "limitlv",		// Arminius 7.30 pet limit level
#ifdef _PET_FUSION
	"fusioncode",	//CHAR_FUSIONCODE
	"fusionindex",	//CHAR_FUSIONINDEX
	"fusionraise",	//CHAR_FUSIONRAISE
	"fusionbe",		//CHAR_FUSIONBEIT
	"futimelimit",	//CHAR_FUSIONTIMELIMIT
#endif
#ifdef _DEATH_CONTEND
	"pkltnum", //CHAR_PKLISTTEAMNUM,
	"pkleader", //CHAR_PKLISTLEADER,
#endif

#ifdef _PERSONAL_FAME	// Arminius 8.30: 家族个人声望
    "fame",
#endif

#ifdef _NEWSAVE
    "sindex",		/*  CHAR_SAVEINDEXNUMBER */
#endif

#ifdef __ATTACK_MAGIC
		"earth_exp",						// 玩家的地魔法熟练度
		"water_exp",						// 玩家的水魔法熟练度
		"fire_exp",	  					// 玩家的火魔法熟练度
		"wind_exp", 						// 玩家的风魔法熟练度
		"earth_resist", 				// 玩家的地魔法抗性
		"water_resist", 				// 玩家的水魔法抗性
		"fire_resist",					// 玩家的火魔法抗性
		"wind_resist",					// 玩家的风魔法抗性
		"earth_attmagic_exp",		// 玩家的地魔法熟练度经验值
		"water_attmagic_exp",		// 玩家的水魔法熟练度经验值
		"fire_attmagic_exp",		// 玩家的火魔法熟练度经验值
		"wind_attmagic_exp",		// 玩家的风魔法熟练度经验值
		"earth_defmagic_exp",   // 玩家的地魔法抗性经验值
		"water_defmagic_exp",   // 玩家的水魔法抗性经验值
		"fire_defmagic_exp",    // 玩家的火魔法抗性经验值
		"wind_defmagic_exp",    // 玩家的风魔法抗性经验值
#endif

#ifdef _GAMBLE_BANK //个人银行
		"personaglod",	//CHAR_PERSONAGOLD,
#endif
#ifdef _DROPSTAKENEW
		"gamblenums",	//CHAR_GAMBLENUM
#endif
#ifdef _ADD_ACTION          //npc动作
		"actsty",		//CHAR_ACTIONSTYLE
#endif
#ifdef _AUCTIONEER
    "aucgold",    // CHAR_AUCGOLD 拍卖所得
#endif
#ifdef _PET_EVOLUTION
	"evbasevtl",//	CHAR_EVOLUTIONBASE,	
	"evbasestr",//	CHAR_EVOLUTIONBASE,	
	"evbasetgh",//	CHAR_EVOLUTIONBASE,	
	"evbasedex",//	CHAR_EVOLUTIONBASE,	
#endif

#ifdef _ACTION_BULLSCR
	"abullstart",	//CHAR_ABULLSTART
	"abullscore",	//CHAR_ABULLSCORE,
	"abulltime",	//CHAR_ABULLTIME,
	"abullstarttime",	//CHAR_ABULLSTARTTIME
#endif

#ifdef _ACTION_GMQUE
	"gmqueflg",	//CHAR_GMQUEFLG,
	"gmquenum",	//CHAR_GMQUENUMS,
#endif


#ifdef _FAMILYBANKSTONELOG
	"fmbankgold", //家族银行存款
#endif

#ifdef _FM_JOINLIMIT
	"fmtimelimit", //	CHAR_FMTIMELIMIT,
#endif

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
	"profession_class",
	"profession_level",
//	"profession_exp",
	"profession_skill_point",
	"attachpile",
	"profession_fire_p",
	"profession_ice_p",
	"profession_thunder_p",
	"profession_fire_r",
	"profession_ice_r",
	"profession_thunder_r",
#endif
#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
	"herofloor",
#endif
#ifdef _PETSKILL_BECOMEPIG
    "becomepig",
	"becomepig_bbi",
#endif
	"leavetime", // CHAR_LASTLEAVETIME
#ifdef _NEW_MANOR_LAW
	"momentum",	// CHAR_MOMENTUM
#endif

#ifdef _ITEM_ADDEXP2
	"aexp",		// CHAR_ADDEXPPOWER
	"aexpt",		// CHAR_ADDEXPTIME
#endif

#ifdef _ANGEL_SUMMON
	"herocnt",	// CHAR_HEROCNT
#endif
#ifdef _TEACHER_SYSTEM
	"tsf",	// CHAR_TEACHER_FAME,	// 导师领导声望
#endif

#ifdef _RACEMAN
	"checkin",	// CHAR_CHECKIN
	"catchcnt1", // CHAR_CATCHCNT1
	"catchcnt2", // CHAR_CATCHCNT2
	"catchcnt3", // CHAR_CATCHCNT3
	"catchcnt4", // CHAR_CATCHCNT4
	"catchcnt5", // CHAR_CATCHCNT5
	"kindcnt1",	//CHAR_KINDCNT1,	/*   (0~31)		*/
	"kindcnt2",	//CHAR_KINDCNT2,    /*   (32~63)		*/
	"kindcnt3",	//CHAR_KINDCNT3,	/*   (64~95)		*/
	"kindcnt4",	//CHAR_KINDCNT4,	/*   (96~127)		*/
	"kindcnt5",	//CHAR_KINDCNT5,	/*   (128~159)		*/
	"kindcnt6",	//CHAR_KINDCNT6,	/*   (160~191)		*/
	"kindcnt7",	//CHAR_KINDCNT7,	/*   (192~223)		*/
	"kindcnt8",	//CHAR_KINDCNT8,	/*   (224~255)		*/
	"kindcnt9",	//CHAR_KINDCNT9,	/*   (256~287)		*/
	"kindcnt10", //CHAR_KINDCNT10,	/*   (288~319)		*/
#endif
#ifdef _GM_ITEM
	"gmtime", // 玩家GM命令次数
#endif
};

static char* CHAR_setchardata[CHAR_DATACHARNUM]={
    /* dcc ,    PLAYER  */
    "name" ,            /*  CHAR_NAME    */
    "ownt" ,            /*  CHAR_OWNTITLE*/
    "arg",                 /*  CHAR_NPCARGUMENT */
    "ocd",				/* CHAR_OWNERCDKEY */
    "ocn",				/* CHAR_OWNERCHARANAME */
    "fmname",	// CoolFish: Family 2001/5/24 家族名称 CHAR_FMNAME
#ifdef _UNIQUE_P_I    
    "ucode",	// CoolFish: 2001/10/11 UNIQUE_P_I CHAR_UNIQUECODE
#endif
#ifdef _ACTION_GMQUE
	//人物 npc 使用 
	"gmque",	//CHAR_GMQUESTR1,
	//"temp2",	//CHAR_GMQUESTR2,
	//"temp3",	//CHAR_GMQUESTR3,
	//"temp4",	//CHAR_GMQUESTR4,
#endif

#ifdef _GM_IDENTIFY
  "gmidentify", //gm名称
#endif
#ifdef _TEACHER_SYSTEM
	"tsi",	// 导师帐号
	"tsn",	// 导师名字
#endif

#ifdef _ITEM_SETLOVER
	"love",       // 结婚判断
	"lovrrid",    // 爱人帐号
	"lovrrneme",	// 爱人名字
#endif

#ifdef _GM_ITEM
	"gmfunction", // 玩家GM命令
#endif
};

INLINE BOOL _CHAR_CHECKINDEX( char *file, int line, int index )
{
    if( CHAR_charanum<=index || index<0 ) {
    	return FALSE;
    }
    if( CHAR_chara[(index)].use == FALSE ) {
    	return FALSE;
    }
    return TRUE;
}

static INLINE BOOL CHAR_CHECKINTDATAINDEX(int index)
{
    if( CHAR_DATAINTNUM<=index || index<0 )return FALSE;
    return TRUE;
}

static INLINE BOOL CHAR_CHECKCHARDATAINDEX(int index)
{
    if( CHAR_DATACHARNUM<=index || index<0 )return FALSE;
    return TRUE;
}

static INLINE BOOL CHAR_CHECKFLGDATAINDEX( int index )
{
    if( CHAR_FLGNUM<=index || index<0 )return FALSE;
    return TRUE;
}

static INLINE BOOL CHAR_CHECKCHARWORKDATAINDEX(int index)
{
    if( CHAR_WORKDATACHARNUM<=index || index<0 )return FALSE;
    return TRUE;
}

static INLINE BOOL CHAR_CHECKCHARFUNCTABLEINDEX(int index)
{
    if( CHAR_FUNCTABLENUM <= index || index < 0 )return FALSE;
    return TRUE;
}

INLINE BOOL _CHAR_CHECKITEMINDEX( char *file, int line, int charaindex, int ti )
{
    if( 0 <= ti && ti < CHAR_MAXITEMHAVE ) return TRUE;
    return FALSE;
}

static INLINE BOOL CHAR_CHECKSKILLINDEX( int skillindex )
{
    if( skillindex<0 || CHAR_SKILLMAXHAVE <= skillindex )return FALSE;
    return TRUE;
}
static INLINE BOOL CHAR_CHECKADDRESSBOOKINDEX( int aindex )
{
    if( aindex < 0 || ADDRESSBOOK_MAX <= aindex ) return FALSE;
    return TRUE;
}

static INLINE BOOL CHAR_CHECKTITLEINDEX( int titleindex )
{
    if( titleindex<0 || CHAR_TITLEMAXHAVE <= titleindex )return FALSE;
    return TRUE;
}

INLINE BOOL _CHAR_CHECKPETINDEX( char *file, int line, int petindex )
{
    if( petindex < 0 || CHAR_MAXPETHAVE <= petindex ) {
		if( petindex != -1 ) {
			print( "err %s:%d \n", file, line);
		}
    	return FALSE;
    }
    return TRUE;
}

INLINE BOOL _CHAR_CHECKPOOLPETINDEX( char *file, int line, int petindex )
{
    if( petindex < 0 || CHAR_MAXPOOLPETHAVE <= petindex ) {
		
		if( petindex != -1 ) {
			print( "err %s:%d :%d \n", file, line, petindex);
		}
    	return FALSE;
    }
    return TRUE;
}

INLINE BOOL _CHAR_CHECKPETSKILLINDEX( char *file, int line, int havepetskillindex )
{
    if( havepetskillindex < 0 || CHAR_MAXPETSKILLHAVE <= havepetskillindex ) {
		if( havepetskillindex != -1 ) {
			print( "err %s:%d \n", file, line);
		}
    	return FALSE;
    }
    return TRUE;
}

INLINE int CHAR_getIntStrict( int index ,CHAR_DATAINT element,int* error)
{
    if(!CHAR_CHECKINDEX(index))return *error=FALSE;
    if(!CHAR_CHECKINTDATAINDEX(element))return *error=FALSE;
    return CHAR_chara[index].data[element];
}

INLINE int CHAR_getInt( int index , CHAR_DATAINT element)
{
    return CHAR_chara[index].data[element];
}

INLINE int CHAR_setIntStrict( int index ,CHAR_DATAINT element,int data,
                              int* error)
{
    int buf;
    if(!CHAR_CHECKINDEX(index))return *error=FALSE;
    if(!CHAR_CHECKINTDATAINDEX(element))return *error=FALSE;
    buf = CHAR_chara[index].data[element];
    CHAR_chara[index].data[element] = data;
    return buf;
}

INLINE int _CHAR_setInt( char *file, int line, int index ,CHAR_DATAINT element, int data)
{
    int buf;
	if( !CHAR_CHECKINDEX( index) )	{
		//print(" err CHAR_setInt index:%d. !:%s-%d\n", index, file, line);
		return -1;
	}
	if( CHAR_DATAPLACENUMBER > element || element >= CHAR_DATAINTNUM )	{
		print(" err CHAR_setInt element:%d.%s :%s-%d\n", element, CHAR_getChar( index, CHAR_CDKEY),file, line);
		return -1;
	}
    buf = CHAR_chara[index].data[element];
    CHAR_chara[index].data[element] = data;
    return buf;
}

#ifdef _FIX_SETWORKINT
INLINE int _CHAR_getWorkInt( char *file, int line, int index ,CHAR_WORKDATAINT element)
#else
INLINE int CHAR_getWorkInt( int index ,CHAR_WORKDATAINT element)
#endif
{
	if( CHAR_WORKBATTLEMODE > element || element >= CHAR_WORKDATAINTNUM )	{
		print("err Get WorkInt element=%d :: file:%s %d!\n", element, file, line);
		return -1;
	}
    return CHAR_chara[index].workint[element];
}

#ifdef _FIX_SETWORKINT
INLINE int _CHAR_setWorkInt( char *file, int line, int index ,CHAR_WORKDATAINT element, int data)
#else
INLINE int CHAR_setWorkInt( int index ,CHAR_WORKDATAINT element, int data)
#endif
{
    int buf;
	if( !CHAR_CHECKINDEX( index) )	{
		//print( "err Set WorkInt index:%d:%s[%d].\n", index, file, line);
		return -1;
	}
	if( CHAR_WORKBATTLEMODE > element || element >= CHAR_WORKDATAINTNUM )	{
#ifdef _FIX_SETWORKINT
		print("err Set WorkInt element=%d.%s:%s[%d]!\n", element, CHAR_getChar( index, CHAR_CDKEY), file, line);
#else
		print("err Set WorkInt element=%d.%s!\n", element, CHAR_getChar( index, CHAR_CDKEY) );
#endif
		return -1;
	}

    buf = CHAR_chara[index].workint[element];
    CHAR_chara[index].workint[element] = data;
    return buf;
}

INLINE char* _CHAR_getChar( char *file, int line, int index ,CHAR_DATACHAR element )
{
    if(!CHAR_CHECKINDEX(index)) {
		print( "err _CHAR_getChar %s:%d index[%d] !!\n", file, line, index);
    	return NULL;
    }
    if(!CHAR_CHECKCHARDATAINDEX(element)) {
		print( "err _CHAR_getChar %s:%d element[%d] !!\n", file, line, element);
    	return NULL;
	}
    return CHAR_chara[index].string[element].string;
}

INLINE BOOL _CHAR_setChar( char *file, int line, int index ,CHAR_DATACHAR element, char* new )
{
    if(!CHAR_CHECKINDEX(index)) {
		print( "err _CHAR_setChar %s:%d !\n", file, line);
    	return FALSE;
    }
    if(!CHAR_CHECKCHARDATAINDEX(element)) {
		print( "err _CHAR_setChar %s:%d !\n", file, line);
    	return FALSE;
    }
    strcpysafe( CHAR_chara[index].string[element].string,
                sizeof(CHAR_chara[index].string[element].string),
                new );
    return TRUE;
}

static INLINE char* CHAR_getCharfunctable( int index ,CHAR_FUNCTABLE element )
{
    if(!CHAR_CHECKINDEX(index))return NULL;
    if(!CHAR_CHECKCHARFUNCTABLEINDEX(element))return NULL;


    return CHAR_chara[index].charfunctable[element].string;
}

INLINE char* _CHAR_getWorkChar( char *file, int line, int index ,CHAR_WORKDATACHAR element )
{
    if(!CHAR_CHECKINDEX(index)) {
		print( "err %s:%d \n", file, line);
    	return NULL;
    }
    if(!CHAR_CHECKCHARWORKDATAINDEX(element)) {
		print( "err %s:%d \n", file, line);
    	return NULL;
    }
    return CHAR_chara[index].workchar[element].string;
}

INLINE BOOL _CHAR_setWorkChar( char *file, int line, int index ,CHAR_WORKDATACHAR element,
                              char* new )
{
    if(!CHAR_CHECKINDEX(index)) {
		print( "err %s:%d \n", file, line);
    	return FALSE;
    }
    if(!CHAR_CHECKCHARWORKDATAINDEX(element)) {
		print( "err %s:%d \n", file, line);
    	return FALSE;
    }
    strcpysafe( CHAR_chara[index].workchar[element].string,
                sizeof(CHAR_chara[index].workchar[element].string),
                new );
    return TRUE;
}

static char CHAR_flgbitmaskpattern[]=
{
    0x01,
    0x02,
    0x04,
    0x08,
    0x10,
    0x20,
    0x40,
    0x80,
};

INLINE BOOL _CHAR_getFlg( char *file, int line, int index ,CHAR_DATAFLG element )
{
    int     elementnum;
    int     bitnum;
    if(!CHAR_CHECKINDEX(index)) {
		//print( "err %s:%d from %s:%d \n", __FILE__, __LINE__,file, line);
    	return FALSE;
    }
    if(!CHAR_CHECKFLGDATAINDEX(element)) {
		//print( "err %s:%d from %s:%d \n", __FILE__, __LINE__,file, line);
	    return FALSE;
	}
    elementnum = element / (sizeof( char )*8);
    bitnum = element % (sizeof( char )*8);


    if( CHAR_chara[index].flg[elementnum]
        & CHAR_flgbitmaskpattern[bitnum])
        return TRUE;
    else
        return FALSE;

}

INLINE char _CHAR_setFlg( char *file, int line, int index , CHAR_DATACHAR element, int newdata )
{
    int buf;
    int elementnum;
    int bitnum;
    char    bitpattern;

    if(!CHAR_CHECKINDEX(index)) {
		print( "err %s:%d \n", file, line);
    	return FALSE;
    }
    if(!CHAR_CHECKFLGDATAINDEX(element)) {
		print( "err %s:%d \n", file, line);
    	return FALSE;
	}
    elementnum = element / (sizeof( char )*8);
    bitnum = element % (sizeof( char )*8);

    /*  樯及袄毛转      */
    buf = CHAR_chara[index].flg[elementnum]
        & CHAR_flgbitmaskpattern[bitnum];

    if( newdata != 0 ){
        bitpattern = CHAR_flgbitmaskpattern[bitnum];
        CHAR_chara[index].flg[elementnum] |= bitpattern;

    }else{
        bitpattern = ~CHAR_flgbitmaskpattern[bitnum];
        CHAR_chara[index].flg[elementnum] &= bitpattern;
    }

    if( buf )return 1;
    else     return 0;
}

INLINE int _CHAR_getItemIndex( char *file, int line, int charaindex ,int ti)
{
    if( !CHAR_CHECKINDEX( charaindex)) {
		print( "err %s:%d from %s:%d\n", __FILE__, __LINE__,file, line);
    	return -1;
    }
    if( !CHAR_CHECKITEMINDEX( charaindex, ti) ){
		print( "err %s:%d from %s:%d \n", __FILE__, __LINE__,file, line);
    	return -1;
    }
    return CHAR_chara[charaindex].indexOfExistItems[ti];
}

INLINE int _CHAR_setItemIndex( char *file, int line, int index ,int iindex,int id )
{
    int buf;
    if( !CHAR_CHECKINDEX(index)) {
		print( "err %s:%d \n", file, line);
    	return -1;
    }
    if( CHAR_MAXITEMHAVE <= iindex || iindex < 0 ) {
		print( "err %s:%d \n", file, line);
    	return -1;
    }

// CoolFish: 2001/10/12
#ifdef _UNIQUE_P_I    
	if ( (id != -1)  && (ITEM_getChar(id, ITEM_UNIQUECODE) != NULL) )  // shan add hjj
    	ITEM_setItemUniCode(id);
#endif

#ifdef _ITEM_TIME_LIMIT
	if( (id != -1) && ITEM_getWorkInt( id, ITEM_WORKTIMELIMIT) == -1 ){
		char    *arg, *p=NULL, tmp[16]="";	
		long    iTmp;
		time_t	iTime;
		
    	time(&iTime);
    	iTmp = iTime;
		arg = ITEM_getChar( id, ITEM_ARGUMENT);
		if(arg && (p=strstr( arg, "Time")) != NULL ){
			//andy_log
			print("%s setTime arg:%s \n", ITEM_getChar( id, ITEM_NAME), arg);
			strcpy( tmp, (p+4));				
			iTmp += atoi(tmp)*60;
			ITEM_setWorkInt( id, ITEM_WORKTIMELIMIT, iTmp);
		}
	}
#endif
    buf = CHAR_chara[index].indexOfExistItems[iindex];
    CHAR_chara[index].indexOfExistItems[iindex] = id;
    return buf;
}
INLINE int _CHAR_getPoolItemIndex( char *file, int line, int index ,int iindex)
{
    if( !CHAR_CHECKINDEX(index)) {
		print( "err %s:%d from %s:%d\n", __FILE__, __LINE__,file, line);
    	return -1;
    }
    if( CHAR_MAXPOOLITEMHAVE <= iindex || iindex < 0 ) {
		print( "err %s:%d from %s:%d \n", __FILE__, __LINE__,file, line);
    	return -1;
    }
    return CHAR_chara[index].indexOfExistPoolItems[iindex];
}
INLINE int _CHAR_setPoolItemIndex( char *file, int line, int index ,int iindex,int id )
{
    int buf;
    if( !CHAR_CHECKINDEX(index)) {
		print( "err %s:%d \n", file, line);
    	return -1;
    }
    if( CHAR_MAXPOOLITEMHAVE <= iindex || iindex < 0 ) {
		print( "err %s:%d \n", file, line);
    	return -1;
    }
    buf = CHAR_chara[index].indexOfExistPoolItems[iindex];
    CHAR_chara[index].indexOfExistPoolItems[iindex] = id;
    return buf;
}

INLINE BOOL CHAR_setAddressbookEntry( int index , int aindex ,
                                 ADDRESSBOOK_entry *a )
{
    if( !CHAR_CHECKINDEX(index))return FALSE;
    if( !CHAR_CHECKADDRESSBOOKINDEX( aindex ))return FALSE;

    memcpy( & CHAR_chara[index].addressBook[aindex] , a ,
            sizeof(ADDRESSBOOK_entry ));

    return TRUE;
}

INLINE ADDRESSBOOK_entry* CHAR_getAddressbookEntry( int index ,
                                                    int aindex)
{
    if( !CHAR_CHECKINDEX(index))return (ADDRESSBOOK_entry*)NULL;
    if( !CHAR_CHECKADDRESSBOOKINDEX( aindex ))
        return (ADDRESSBOOK_entry*) NULL;

    return &CHAR_chara[index].addressBook[aindex];
}

INLINE Char* CHAR_getCharPointer( int index )
{
    if( !CHAR_CHECKINDEX(index) )return NULL;
    return &CHAR_chara[index];
}

INLINE int CHAR_getCharNum( void )
{
    return CHAR_charanum;
}

INLINE int CHAR_getPlayerMaxNum( void )
{
    return CHAR_playernum;
}

INLINE int CHAR_getPetMaxNum( void )
{
    return CHAR_petnum;
}
INLINE int CHAR_getOthersMaxNum( void )
{
    return CHAR_othersnum;
}

INLINE BOOL CHAR_getCharUse( int index )
{
    if( !CHAR_CHECKINDEX(index))
    	return FALSE;
    return CHAR_chara[index].use;
}

INLINE CHAR_HaveSkill* CHAR_getCharHaveSkill( int index,int sindex )
{
    if( !CHAR_CHECKINDEX(index))return NULL;
    if( !CHAR_CHECKSKILLINDEX(sindex))return NULL;
	return &CHAR_chara[index].haveSkill[sindex];
}
//------------------------------------------------------------------------
//人物index  技能位置  索引
INLINE int _CHAR_getIntPSkill( char *file, int line, int index, int skillti, SKILL_DATAINT ti)
{
    if( !CHAR_CHECKINDEX( index) ) return -1;
    if( !CHAR_CHECKSKILLINDEX( skillti) )  return -1;
	if( ti >= SKILL_DATAINTNUM || ti < 0 ){
		print(" getIntPSkill()  SKILL_DATAINT (%d) err -%s:%d!!\n", ti , file, line);
		return -1;
	}
	if( CHAR_chara[index].haveSkill[skillti].skill.data[ti] <= 0 ) return -1;
	return CHAR_chara[index].haveSkill[skillti].skill.data[ti];
}
//人物index  技能位置  索引 欲设定值
INLINE void _CHAR_setIntPSkill( char *file, int line, int index, int skillti, SKILL_DATAINT ti, int data)
{
    if( !CHAR_CHECKINDEX( index) ) return;
    if( !CHAR_CHECKSKILLINDEX( skillti) )  return;
	if( ti >= SKILL_DATAINTNUM || ti < 0 ){
		print(" getIntPSkill()  SKILL_DATAINT (%d) err -%s:%d!!\n", ti , file, line);
		return;
	}
//	if( CHAR_chara[index].haveSkill[sindex].use > 0 )
	CHAR_chara[index].haveSkill[skillti].skill.data[ti] = data;
}
//------------------------------------------------------------------------

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
int CHAR_getCharSkill( int index,int sindex )
{
	CHAR_HaveSkill* hskill;
	
	hskill = CHAR_getCharHaveSkill( index, sindex );

	if(hskill != NULL)
		return SKILL_getInt( &hskill->skill, SKILL_IDENTITY);
	else
		return -1;

}

int CHAR_setCharSkill( int index,int sindex, int new )
{
	CHAR_HaveSkill* hskill;

    if( !CHAR_CHECKINDEX(index))	return -1;
    if( !CHAR_CHECKSKILLINDEX(sindex))	return -1;
	
	hskill = CHAR_getCharHaveSkill( index, sindex );
	SKILL_setInt( &hskill->skill, SKILL_IDENTITY, new);
	return TRUE;
}
#endif

INLINE int CHAR_getCharHaveTitle( int charaindex,int tindex )
{
    if( !CHAR_CHECKINDEX(charaindex))return 0;
    if( !CHAR_CHECKTITLEINDEX(tindex))return 0;
    return CHAR_chara[charaindex].indexOfHaveTitle[tindex];
}

INLINE int CHAR_setCharHaveTitle( int charaindex,int tindex, int new )
{
    int ret;
    if( !CHAR_CHECKINDEX(charaindex))return 0;
    if( !CHAR_CHECKTITLEINDEX(tindex))return 0;
	if( new != -1 )
		if( TITLE_getTitleIndex( new) == -1 ) return 0;
    ret = CHAR_getCharHaveTitle( charaindex,tindex);
    CHAR_chara[charaindex].indexOfHaveTitle[tindex] = new;
    return ret;
}

INLINE int CHAR_getCharPet( int charaindex,int petindex )
{
    if( !CHAR_CHECKINDEX(charaindex))return -1;
    if( !CHAR_CHECKPETINDEX(petindex))return -1;
    return CHAR_chara[charaindex].unionTable.indexOfPet[petindex];
}

INLINE int CHAR_setCharPet( int charaindex,int petindex, int new )
{
    int ret;
    if( !CHAR_CHECKINDEX(charaindex))return -1;
    if( !CHAR_CHECKPETINDEX(petindex))return -1;
    ret = CHAR_getCharPet( charaindex,petindex);

// CoolFish: 2001/10/12
#ifdef _UNIQUE_P_I
    if (new != -1)
	CHAR_setPetUniCode(new);
#endif

    CHAR_chara[charaindex].unionTable.indexOfPet[petindex] = new;
    return ret;
}

INLINE int CHAR_getCharPoolPet( int charaindex,int petindex )
{
    if( !CHAR_CHECKINDEX(charaindex))return -1;
    if( !CHAR_CHECKPOOLPETINDEX(petindex))return -1;
    return CHAR_chara[charaindex].indexOfPoolPet[petindex];
}

INLINE int CHAR_setCharPoolPet( int charaindex,int petindex, int new )
{
    int ret;
    if( !CHAR_CHECKINDEX(charaindex))return -1;
    if( !CHAR_CHECKPOOLPETINDEX(petindex))return -1;
    ret = CHAR_getCharPoolPet( charaindex,petindex);

    CHAR_chara[charaindex].indexOfPoolPet[petindex] = new;
    return ret;
}

int CHAR_getCharPetElement( int charaindex )
{
    int i;
    if( !CHAR_CHECKINDEX(charaindex))return -1;
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
	    if( CHAR_getCharPet( charaindex,i) == -1 ) break;
    }
    return ( i == CHAR_MAXPETHAVE ? -1:i );

}

int CHAR_getCharPoolPetElement( int charaindex )
{
    int i;
	int limit;
    if( !CHAR_CHECKINDEX(charaindex))return -1;

	//andy_reEdit 2003/09/18
	limit = (CHAR_getInt(charaindex,CHAR_TRANSMIGRATION)*2)+5;
	limit = min( limit, CHAR_MAXPOOLPETHAVE);
	for( i = 0; i < limit; i ++ ) {
		if( CHAR_getCharPoolPet( charaindex,i) == -1 ) break;
    }
    return ( i == limit ? -1:i );

}

int CHAR_getCharPoolItemIndexElement( int charaindex )
{
    int i;
    int limit;
    if( !CHAR_CHECKINDEX(charaindex))return -1;
	//andy_reEdit 2003/09/18
	limit = (CHAR_getInt(charaindex,CHAR_TRANSMIGRATION)*4)+10;
	limit = min( limit, CHAR_MAXPOOLITEMHAVE);
	for( i = 0; i < limit; i ++ ) {
	    if( CHAR_getPoolItemIndex( charaindex,i) == -1 ) break;
    }
    // Robin fix
	//return ( i == CHAR_MAXPOOLITEMHAVE ? -1:i );
	return ( i >= limit ? -1:i );

}

int CHAR_getEmptyCharPoolItemIndexNum( int charaindex)
{
	int i, cnt = 0;
    int limit;
    if( !CHAR_CHECKINDEX(charaindex))return 0;
	//andy_reEdit 2003/09/18
	limit = (CHAR_getInt(charaindex,CHAR_TRANSMIGRATION)*4)+10;
	limit = min( limit, CHAR_MAXPOOLITEMHAVE);

	for( i = 0; i < limit; i ++ ) {
		if( CHAR_getPoolItemIndex( charaindex, i) == -1 ) cnt++;
	}
	return cnt;
}

INLINE int _CHAR_getPetSkill( char *file, int line, int petindex, int havepetskillindex )
{
    if( !CHAR_CHECKINDEX(petindex)) {
    	print( "err %s:%d from %s:%d\n", __FILE__, __LINE__, file, line);
    	return -1;
    }
    if( !CHAR_CHECKPETSKILLINDEX(havepetskillindex)) {
    	print( "err %s:%d from %s:%d\n", __FILE__, __LINE__, file, line);
    	return -1;
    }
    return CHAR_chara[petindex].unionTable.indexOfPetskill[havepetskillindex];
}

INLINE int _CHAR_setPetSkill( char *file, int line, int petindex,int havepetskillindex, int new )
{
    int ret;
    if( !CHAR_CHECKINDEX(petindex)) {
    	print( "err %s:%d from %s:%d\n", __FILE__, __LINE__, file, line);
    	return -1;
    }
    if( !CHAR_CHECKPETSKILLINDEX(havepetskillindex)) {
    	print( "err %s:%d from %s:%d\n", __FILE__, __LINE__, file, line);
    	return -1;
    }
    ret = CHAR_getPetSkill( petindex,havepetskillindex);
    CHAR_chara[petindex].unionTable.indexOfPetskill[havepetskillindex] = new;
    return ret;
}

int _CHAR_getPetSkillElement( char *file, int line, int petindex )
{
    int i;
    if( !CHAR_CHECKINDEX(petindex)) {
    	print( "err %s:%d from %s:%d\n", __FILE__, __LINE__, file, line);
    	return -1;
	}
	for( i = 0; i < CHAR_MAXPETSKILLHAVE; i ++ ) {
	    if( CHAR_getPetSkill( petindex,i) == -1 ) break;
    }
    return ( i == CHAR_MAXPETSKILLHAVE ? -1:i );

}

INLINE int CHAR_getCharMakeSequenceNumber( int charaindex )
{
    if( !CHAR_CHECKINDEX(charaindex))return -1;
    return CHAR_chara[charaindex].CharMakeSequenceNumber;
}

void CHAR_constructFunctable( int charaindex )
{
    int     i;
    if( !CHAR_CHECKINDEX(charaindex))return;
    for( i=CHAR_FIRSTFUNCTION; i < CHAR_LASTFUNCTION ; i++ )
        CHAR_chara[charaindex].functable[i-CHAR_FIRSTFUNCTION]
            = getFunctionPointerFromName( CHAR_getCharfunctable( charaindex,i ) );
}

void* CHAR_getFunctionPointer( int charaindex, int functype )
{
    if( !CHAR_CHECKINDEX(charaindex) )return NULL;
    if( functype < CHAR_FIRSTFUNCTION  || functype >= CHAR_LASTFUNCTION )
        return NULL;

    return CHAR_chara[charaindex].functable[functype];
}

BOOL CHAR_initCharArray( int pnum, int petnum,int onum )
{
    int     i;

    BOOL CHAR_checksetdata( void );
    if( CHAR_checksetdata() == FALSE ){
		print(" err CHAR_checksetdata() FALSE !!\n");
		return FALSE;
	}

	//andy_log
	print("#");

    CHAR_initSeekGraphicNumberFromString();
    CHAR_initChatMagic();
	CHAR_charanum = pnum + petnum +onum;

	//andy_log
	print("#");

    CHAR_chara = allocateMemory( sizeof( Char ) * CHAR_charanum );
    if( CHAR_chara == NULL ){
		print("无法分配人物内存 !!\n");
		return FALSE;
	}


	//andy_log
	print("人物内存: %4.2f...", ((float)sizeof( Char )*CHAR_charanum)/1024);

    for( i = 0 ; i < CHAR_charanum ; i ++ ){
		memset( &CHAR_chara[i], 0 , sizeof( Char ));

	}
    for( i = 0 ; i < pnum ; i ++ ){
#ifdef _NPC_SEPARATEDATA
		CHAR_InitCharTempNum( &CHAR_chara[i] );
#endif
	}
    CHAR_playernum = pnum;
    CHAR_petnum = petnum;
    CHAR_othersnum = onum;

    return TRUE;
}

BOOL CHAR_endCharArray( void )
{
#ifdef _NPC_SEPARATEDATA
	CHAR_EndCharTempNum();
#endif
    freeMemory( CHAR_chara );
    CHAR_charanum = 0;
    return TRUE;
}

BOOL CHAR_getCharOnArrayPercentage( int mode, int *max, int *min, int *cnt)
{
	int po=0, Dnums=0;
	po = initCharCounter[0].endcnt;
	*cnt = -1;
	while( 1){
		if( !CHAR_chara[po].use ){
		}else{
			Dnums++;
		}
		po++;
		if( po >= initCharCounter[mode].endcnt ){
			break;
		}
	}
	*cnt = Dnums;
	*max = initCharCounter[mode].endcnt - initCharCounter[mode].startcnt;
	*min = 0;

	return TRUE;
}

int CHAR_initCharOneArray( Char* ch )
{
    int i;
    int mode;
    int ret = TRUE;
    int first;
	static unsigned int CharMakeSequenceNumber = 0;
    if( initCharCounter[0].startcnt == -1 ) {
        initCharCounter[0].startcnt = 0;
        initCharCounter[0].cnt      = initCharCounter[0].startcnt;
        initCharCounter[0].endcnt   = CHAR_playernum;

        initCharCounter[1].startcnt = initCharCounter[0].endcnt;
        initCharCounter[1].cnt      = initCharCounter[0].endcnt;
        initCharCounter[1].endcnt   = CHAR_playernum + CHAR_petnum;

        initCharCounter[2].startcnt = initCharCounter[1].endcnt;
        initCharCounter[2].cnt      = initCharCounter[1].endcnt;
        initCharCounter[2].endcnt   = CHAR_charanum;
    }
    if( ch->data[CHAR_WHICHTYPE] == CHAR_TYPEPLAYER ){
        mode = 0;
    }else if( CHAR_TYPEPET ==  ch->data[CHAR_WHICHTYPE] ) {
        mode = 1;
    }else{
        mode = 2;
    }
    i = initCharCounter[mode].cnt;
    first = TRUE;
    while( 1 ){
        if( !first && i >= initCharCounter[mode].cnt ) {
            ret = FALSE;
            break;
        }
        if( CHAR_chara[i].use == FALSE ){
            ret = TRUE;
            break;
        }else{
            i++;
            if( i >= initCharCounter[mode].endcnt ){
                i = initCharCounter[mode].startcnt;
                first = FALSE;
            }
        }
    }
    if( ret ){
        typedef BOOL (*INITFUNC)(int index);
        INITFUNC initfunc;

#ifdef _NPC_SEPARATEDATA
		ch->tempint = CHAR_chara[i].tempint;
		CHAR_getDefaultCharTempNum( ch);
#endif
        memset( &CHAR_chara[i] , 0, sizeof( Char ) );
        memcpy( &CHAR_chara[i] , ch , sizeof( Char ) );

        initfunc = (INITFUNC)getFunctionPointerFromName(
            ch->charfunctable[CHAR_INITFUNC].string );
        if( initfunc ) {
            if( initfunc( i ) == FALSE ){
				CHAR_chara[i].use = FALSE;
                ret = FALSE;
            }
        }
        if( ret ){
            CHAR_chara[i].use = TRUE;
            CHAR_constructFunctable( i );
            if( i + 1 >= initCharCounter[mode].endcnt ) {
                initCharCounter[mode].cnt
                    = initCharCounter[mode].startcnt;
            }else {
                initCharCounter[mode].cnt = i+1;
			}
        	CHAR_chara[i].CharMakeSequenceNumber = CharMakeSequenceNumber++;
			if( CharMakeSequenceNumber == 0xffffffff) CharMakeSequenceNumber = 0;
        }
    }else{
        fprint( "Error: Char full\n" );
	}
    return ret ? i: -1;
}

void CHAR_removeHaveItem( Char* ch )
{
    int     i;
    if( ch == NULL )return;
    for( i=0 ; i<CHAR_MAXITEMHAVE ; i++ ) {
		int itemindex = ch->indexOfExistItems[i];
		ch->indexOfExistItems[i] = -1;
        ITEM_endExistItemsOne( itemindex );
	}
}

void CHAR_removeHavePoolItem( Char* ch )
{
    int     i;
    if( ch == NULL )return;
    for( i=0 ; i<CHAR_MAXPOOLITEMHAVE ; i++ ) {
		int itemindex = ch->indexOfExistPoolItems[i];
		ch->indexOfExistPoolItems[i] = -1;
        ITEM_endExistItemsOne( itemindex );
	}
}

void CHAR_endCharData( Char* ch )
{
    if( ch == NULL ){
		return;
	}
    CHAR_removeHaveItem( ch );
	CHAR_removeHavePoolItem( ch);

#ifdef _CHAR_POOLITEM
//	CHAR_removeHaveDepotItem( ch );
#endif
#ifdef _CHAR_POOLPET
//	CHAR_removeHaveDepotPet( ch );
#endif

    ch->use = FALSE;
}

void CHAR_endCharOneArray( int index )
{
    Char*   ch = NULL;
	if( !CHAR_CHECKINDEX( index) ) return;
	ch = CHAR_getCharPointer(index);

    if( ch == NULL ){
		print( "CHAR_endCharOneArray err ch == NULL :%d\n", index);
		return;
	}
    CHAR_endCharData( ch );
}
static char CHAR_dataString[STRINGBUFSIZ*16*2];
BOOL CHAR_checksetdata( void )
{
    int     i;
    char*   strings[CHAR_DATAINTNUM + CHAR_DATACHARNUM];
    int     stringnum=0;
    for(i = 0; i < CHAR_DATAINTNUM ; i ++ ){
        if( CHAR_setintdata[i][0] != '#' )
            strings[stringnum++] = CHAR_setintdata[i];
    }
    for( i = 0 ; i < CHAR_DATACHARNUM ; i ++ ){
        if( CHAR_setchardata[i][0] != '#' )
            strings[stringnum++] = CHAR_setchardata[i];
    }
    if( ! checkStringsUnique( strings, stringnum , 1 ) ){
        fprint( "set????data is overlapped.\nIt is not allowed\n");
        return FALSE;
    }
    return TRUE;
}

#define FLGRESERVESTRING   "flg"
#define ITEMRESERVESTRING   "item"
#define POOLITEMRESERVESTRING   "poolitem"
#define SKILLRESERVESTRING  "skill"
#define TITLERESERVESTRING  "title"
#define ADDRESSBOOKRESERVESTRING  "adr"
#define	PETSERVERSTRING		"pet"
#define PETITEMRESERVESTRING   "pitem"
#define	PETSKILLSERVERSTRING	"psk"
#define	POOLPETSERVERSTRING		"poolpet"
#define ITEMPOOLRESERVESTRING   "itempool"

#define	DATAENDCHECKPOINT		"DATAEND"




#ifdef _CHAR_POOLITEM
#define DEPOTITEMRESERVESTRING   "Depotitem"
#endif
#ifdef _CHAR_POOLPET
#define DEPOTPETRESERVESTRING   "Depotpet"
#endif

char* CHAR_makeStringFromCharData( Char* one )
{
    int     i;
	int		petnum;
	int     strlength=0;

    if (!one) return NULL;

	memset( CHAR_dataString, 0, sizeof(CHAR_dataString));
    for( i = 0 ; i < CHAR_DATAINTNUM ; i ++ ){
        char    linedata[256];
        snprintf( linedata , sizeof(linedata),
                  "%s=%d" CHAR_DELIMITER,
                  CHAR_setintdata[i],
                  one->data[i] );
        strcpysafe( &CHAR_dataString[strlength],
                    sizeof( CHAR_dataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_dataString ) ) {
			fprint( "err chardata buffer over\n");
        	goto RETURN;
		}
    }
    for( i = 0 ; i < CHAR_DATACHARNUM ; i ++ ){
        char    linedata[256];
        char    escapebuffer[256];
        snprintf( linedata , sizeof(linedata),
                  "%s=%s" CHAR_DELIMITER,
                  CHAR_setchardata[i],
                  makeEscapeString( one->string[i].string, escapebuffer,
                                    sizeof( escapebuffer) ));

        strcpysafe( &CHAR_dataString[strlength],
                    sizeof( CHAR_dataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_dataString ) ) {
			fprint( "err chardata buffer over\n");
        	goto RETURN;
    	}
    }
    for( i = 0 ; i < arraysizeof( one->flg ) ; i ++ ){
        char    linedata[128];
        snprintf( linedata , sizeof(linedata),
                  FLGRESERVESTRING "%d=%d" CHAR_DELIMITER,
                  i, one->flg[i]);

        strcpysafe( &CHAR_dataString[strlength],
                    sizeof( CHAR_dataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_dataString ) ) {
			fprint( "err chardata buffer over\n");
        	goto RETURN;
    	}
    }

    for( i = 0 ; i < CHAR_SKILLMAXHAVE ; i ++ ){
        char    linedata[128];
        char    *skillstring=NULL;
        if( one->haveSkill[i].use == TRUE)
            skillstring = SKILL_makeStringFromSkillData(
                &one->haveSkill[i].skill );
        if( skillstring == NULL ) continue;
        snprintf( linedata , sizeof( linedata ),
                  SKILLRESERVESTRING "%d=%s" CHAR_DELIMITER,
                  i,skillstring );
        strcpysafe( &CHAR_dataString[strlength],
                    sizeof( CHAR_dataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_dataString ) ) {
			fprint( "err chardata buffer over\n");
        	goto RETURN;
    	}
    }

    for( i = 0 ; i < CHAR_MAXITEMHAVE ; i ++ ){
        char    linedata[2048];
        char    *itemstring=NULL;
        if( one->indexOfExistItems[i] != -1 )
            itemstring = ITEM_makeStringFromItemIndex(
                							one->indexOfExistItems[i], 0);
        if( itemstring == NULL )continue;
        snprintf( linedata , sizeof( linedata ),
                  ITEMRESERVESTRING "%d=%s" CHAR_DELIMITER,
                  i, itemstring);
        strcpysafe( &CHAR_dataString[strlength],
                    sizeof( CHAR_dataString ) - strlength,
                    linedata );

        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_dataString ) ) {
			fprint( "err chardata buffer over\n");
        	goto RETURN;
    	}
    }

    for( i = 0 ; i < CHAR_MAXPOOLITEMHAVE ; i ++ ){
        char    linedata[2048];
        char    *itemstring=NULL;
        if( one->indexOfExistPoolItems[i] != -1 )
            itemstring = ITEM_makeStringFromItemIndex(
                							one->indexOfExistPoolItems[i], 0);
        if( itemstring == NULL )continue;
        snprintf( linedata , sizeof( linedata ),
                  POOLITEMRESERVESTRING "%d=%s" CHAR_DELIMITER,
                  i, itemstring);
        strcpysafe( &CHAR_dataString[strlength],
                    sizeof( CHAR_dataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_dataString ) ) {
			fprint( "err chardata buffer over\n");
        	goto RETURN;
    	}
    }

    for( i = 0 ; i < CHAR_TITLEMAXHAVE ; i ++ ){
        char    linedata[128];
		if( one->indexOfHaveTitle[i]< 0 || one->indexOfHaveTitle[i] >= CHAR_TITLEMAXHAVE ) {
			continue;
		}
        snprintf( linedata , sizeof( linedata ),
                  TITLERESERVESTRING "%d=%d" CHAR_DELIMITER,
                  i, one->indexOfHaveTitle[i] );
        strcpysafe( &CHAR_dataString[strlength],
                    sizeof( CHAR_dataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_dataString ) ) {
			fprint( "err chardata buffer over\n");
        	goto RETURN;
    	}
    }
    for( i=0 ; i < ADDRESSBOOK_MAX ; i++ ){
        char    linedata[512];
		char	*p;
		p = ADDRESSBOOK_makeAddressbookString(&one->addressBook[i]);
		if( strlen( p) == 0 ) {
			continue;
		}
        snprintf( linedata ,sizeof(linedata),
                  ADDRESSBOOKRESERVESTRING "%d=%s" CHAR_DELIMITER,
                  i,p
            );
        strcpysafe( &CHAR_dataString[strlength],
                    sizeof( CHAR_dataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_dataString ) ) {
			fprint( "err chardata buffer over\n");
        	goto RETURN;
    	}
    }
    for( petnum = 0; petnum < CHAR_MAXPETHAVE; petnum ++ ) {
		int	petindex;
		char	linedata[4096*2];
		char	*petstring;
		petindex = one->unionTable.indexOfPet[petnum];
	    if( !CHAR_CHECKINDEX(petindex))continue;
		petstring = CHAR_makePetStringFromPetIndex( petindex);
		if( petstring == NULL ) continue;
        snprintf( linedata , sizeof(linedata),
                  PETSERVERSTRING "%d=%s" CHAR_DELIMITER,petnum,petstring );
        strcpysafe( &CHAR_dataString[strlength],
                    sizeof( CHAR_dataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_dataString ) ) {
			fprint( "err chardata buffer over\n");
        	goto RETURN;
		}
	}

    for( petnum = 0; petnum < CHAR_MAXPOOLPETHAVE; petnum ++ ) {
		int	petindex;
		char	linedata[4096*2];
		char	*petstring;
		petindex = one->indexOfPoolPet[petnum];
	    if( !CHAR_CHECKINDEX(petindex))continue;
		petstring = CHAR_makePetStringFromPetIndex( petindex);
		if( petstring == NULL ) continue;

		//andy_log
        if( strstr( petstring, "name:") == NULL ||
			strstr( petstring, "ownt:") == NULL ) {
			LodBadPetString( "petstring", "poolpet string buffer err:", petnum);
			fprint( "ANDY err poolpet string buffer err:\n%s\n", petstring);
        	goto RETURN;
		}

        snprintf( linedata , sizeof(linedata),
                  POOLPETSERVERSTRING "%d=%s" CHAR_DELIMITER,petnum,petstring );

        strcpysafe( &CHAR_dataString[strlength],
                    sizeof( CHAR_dataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_dataString ) ) {
			fprint( "err chardata buffer over\n");
        	goto RETURN;
		}
	}
#ifdef _CHAR_FIXDATADEF
	{
		char linedata[1024];
		memset( linedata, 0, sizeof(linedata));
        snprintf( linedata , sizeof(linedata),
                  DATAENDCHECKPOINT "=%d" CHAR_DELIMITER, (int)time(NULL)/1000);

        strcpysafe( &CHAR_dataString[strlength],
                    sizeof( CHAR_dataString ) - strlength, linedata );

		if( strlength > sizeof( CHAR_dataString ) ) {
			LodBadPetString( "DATAENDCHECKPOINT", "err chardata buffer over", -1);
			fprint( "err chardata buffer over:DATAENDCHECKPOINT !\n");
        	goto MAKESTRINGERR;
		}
	}
#endif

RETURN:
	if( strstr( CHAR_dataString, DATAENDCHECKPOINT) == NULL ){
		LodBadPetString( CHAR_dataString, "err mk dataString no end", -1);
	}

    return CHAR_dataString;
MAKESTRINGERR:
	return NULL;
}

char* CHAR_makeStringFromCharIndex( int index )
{
    if( 0 <= index && index <= CHAR_charanum        /*    index OK*/
        && CHAR_chara[index].use == TRUE )
        /* OK */;
    else
        return NULL;
    return CHAR_makeStringFromCharData(&CHAR_chara[index]);

}

BOOL CHAR_makeCharFromStringToArg( char* data, Char* one)
{
    int i;
    int readindex=1, rightData=0;

    if( data[0] == '\0' )
        return FALSE;
DebugPoint=51;
    {    
		char name[4096];
		char *sp = strstr( data, "name=");
		char *sp2 = strstr( data, "ownt=");
		if( sp && sp2 ){
			snprintf( name, sp2-sp-5, "%s", sp+5 );		
			print("\n开始登陆人物名称:%s ", name );
		}else
			print("\n开始登陆人物名称错误! ");
			
		if( data[strlen(data)-2] != '\n' ){
			print("\n 登陆错误!:%s ",data+strlen(data)-20 );
		}
		
		if( strlen(name) > 16 ){
			print("人物名称过长!! 人物名称:%s 长度:%d ", name ,strlen(name) );
		}
    }

    memset( one , 0 , sizeof( Char ));
DebugPoint=52;
    CHAR_getDefaultChar(one, 0);
    // Robin 0726 default RidePet
    one->data[CHAR_RIDEPET] = -1;
	strcpysafe( one->string[CHAR_FMNAME].string,
		sizeof( one->string[CHAR_FMNAME].string), "\0" );
	one->data[CHAR_FMINDEX] = -1;
#ifdef _PETSKILL_BECOMEPIG
    one->data[CHAR_BECOMEPIG] = -1;
	one->data[CHAR_BECOMEPIG_BBI] = 100250;
#endif

DebugPoint=53;
    while( 1 ){
        BOOL    ret;
        char    linebuf[4096];
        char firstToken[256];
        char secondToken[4096];
		memset( linebuf, 0, sizeof( linebuf));
		memset( firstToken, 0, sizeof( firstToken));
		memset( secondToken, 0, sizeof( secondToken));
DebugPoint=54;
        ret = getStringFromIndexWithDelim( data ,CHAR_DELIMITER,
                                           readindex,
                                           linebuf,  sizeof( linebuf ) );
DebugPoint=55;
        if( ret == FALSE )break;
        if( linebuf[0] == '#'   ||
            linebuf[0] == '\n'  ||
            linebuf[0] == '\0' ){           
            readindex ++;
            continue;
        }
DebugPoint=56;
        ret = getStringFromIndexWithDelim( linebuf , "=" ,  1 ,
                                           firstToken ,
                                           sizeof(firstToken) );
        if( ret == FALSE )  return FALSE;
DebugPoint=57;
        strcpysafe( secondToken , sizeof( secondToken ),
                    linebuf + strlen(firstToken) + strlen("=") );
DebugPoint=58;
	    for( i = 0 ; i < CHAR_DATAINTNUM ; i ++ ){
            if( strcmp(firstToken ,CHAR_setintdata[i] ) == 0 ){
                one->data[i] = atoi( secondToken );
                goto NEXT;
            }
        }
DebugPoint=59;
        for( i = 0 ; i < CHAR_DATACHARNUM ; i ++ ){
            if( strcmp( firstToken ,CHAR_setchardata[i] ) == 0 ){
                strcpysafe( one->string[i].string,
                            sizeof(one->string[i].string),
                            makeStringFromEscaped(secondToken) );
				if( strlen( one->string[i].string ) > 128 ){
                	print(" CHARDATA_to_long!!:%s:%d ", one->string[i].string, strlen( one->string[i].string ) );
                	one->string[i].string[0]=0;
                }
                goto NEXT;
            }
        }
DebugPoint=60;
        if( strncmp( firstToken , FLGRESERVESTRING,
                     strlen( FLGRESERVESTRING ) ) == 0 ){
            int     flgindex;
            flgindex = atoi( firstToken+strlen(FLGRESERVESTRING) );
            if( flgindex < 0 || arraysizeof( one->flg ) <=  flgindex )
                ;
            else
                one->flg[flgindex] = atoi( secondToken );

            goto NEXT;
        }
DebugPoint=61;
        if( strncmp( firstToken , ITEMRESERVESTRING,
                     strlen( ITEMRESERVESTRING ) ) == 0 ){
            int itemindex;
            if( strcmp( secondToken, NULLITEM) == 0 )
                goto NEXT;
             itemindex = atoi( firstToken+strlen(ITEMRESERVESTRING) );
            if( itemindex < 0 || CHAR_MAXITEMHAVE <=  itemindex
                ||  one->indexOfExistItems[itemindex] != -1 ){
                 ;
            }else{
                ITEM_Item   itmone;
                BOOL ret;

                ret = ITEM_makeExistItemsFromStringToArg( secondToken, &itmone, 0);

                if( ret == TRUE ){
                    int existitemindex;
                    existitemindex = ITEM_initExistItemsOne( &itmone );
                    one->indexOfExistItems[itemindex] = existitemindex;
                }
            }
            goto NEXT;
        }
DebugPoint=62;
        if( strncmp( firstToken , POOLITEMRESERVESTRING,
                     strlen( POOLITEMRESERVESTRING ) ) == 0 ){
            int itemindex;
            if( strcmp( secondToken, NULLITEM) == 0 )
                goto NEXT;
            itemindex = atoi( firstToken+strlen(POOLITEMRESERVESTRING) );
            if( itemindex < 0 || CHAR_MAXPOOLITEMHAVE <=  itemindex
                ||  one->indexOfExistPoolItems[itemindex] != -1 ){
                ;
            }else{
                ITEM_Item   itmone;
                BOOL ret;
                ret = ITEM_makeExistItemsFromStringToArg( secondToken , &itmone, 0 );

                if( ret == TRUE ){
                    int existitemindex;
                    existitemindex = ITEM_initExistItemsOne( &itmone );
                    one->indexOfExistPoolItems[itemindex] = existitemindex;
                }
            }
            goto NEXT;
        }
DebugPoint=63;
        if( strncmp( firstToken , SKILLRESERVESTRING,
                     strlen( SKILLRESERVESTRING ) ) == 0 ){
            int skillindex;
            if( strcmp( secondToken, NULLSKILL) == 0 )
                goto NEXT;
            skillindex =atoi( firstToken+strlen(SKILLRESERVESTRING) );
            if( skillindex < 0 || CHAR_SKILLMAXHAVE <=  skillindex
                ||  one->haveSkill[skillindex].use == TRUE ){
            }else{
                BOOL    ret;
                Skill   skillone;
                ret=SKILL_makeSkillFromStringToArg(secondToken,&skillone);

                if( ret == TRUE ){
                    memcpy( &one->haveSkill[skillindex].skill , &skillone,
                            sizeof( Skill ));
                    one->haveSkill[skillindex].use = TRUE;
                }
            }
            goto NEXT;
        }
DebugPoint=64;
        if( strncmp( firstToken , TITLERESERVESTRING,
                     strlen( TITLERESERVESTRING ) ) == 0 ){
			int titlenumber=atoi(firstToken+strlen(TITLERESERVESTRING));
            if( titlenumber < 0 || CHAR_TITLEMAXHAVE <= titlenumber ){
            }else{
                int titleindex = atoi( secondToken );
                if( TITLE_getTitleIndex( titleindex) != -1)
                    one->indexOfHaveTitle[titlenumber] = titleindex;
            }
            goto NEXT;
        }
DebugPoint=65;
        if( strncmp( firstToken , ADDRESSBOOKRESERVESTRING,
                     strlen( ADDRESSBOOKRESERVESTRING ) ) == 0 ){
            int addressnumber = atoi( firstToken
                                      +strlen(ADDRESSBOOKRESERVESTRING));
            if( addressnumber < 0 || ADDRESSBOOK_MAX <= addressnumber ){
            }else{
                ADDRESSBOOK_makeAddressbookEntry( secondToken,
                                                  &one
                                                  ->addressBook[
                                                      addressnumber]);
            }
            goto NEXT;
        }
DebugPoint=66;
        if( strncmp( firstToken , PETSERVERSTRING,
                     strlen( PETSERVERSTRING ) ) == 0 ){
			Char	ch;
			int		ret;
			int	petnumber = atoi( firstToken+strlen(PETSERVERSTRING));
			if( !CHAR_CHECKPETINDEX( petnumber)) goto NEXT;
			ret = CHAR_makePetFromStringToArg(secondToken,&ch, petnumber);
			if( ret == TRUE ){
		        int petindex = PET_initCharOneArray( &ch );
		        if( petindex < 0 ){
		        	print( "宠物制作失败。\n");
		        }
		        one->unionTable.indexOfPet[petnumber]  = petindex;
				goto NEXT;
			}else{
				LodBadPetString( data, "错误总计", petnumber);

				fprint( "错误 宠物字符串 无法作成\n");
				return FALSE;
			}
		}
DebugPoint=67;
        if( strncmp( firstToken , POOLPETSERVERSTRING, strlen( POOLPETSERVERSTRING ) ) == 0 ){

			Char	ch;
			int		ret;
			int	petnumber = atoi( firstToken+strlen(POOLPETSERVERSTRING));
			if( !CHAR_CHECKPOOLPETINDEX( petnumber)) goto NEXT;
			ret = CHAR_makePetFromStringToArg( secondToken, &ch, petnumber);
			if( ret == TRUE ) {
		        int petindex = PET_initCharOneArray( &ch );
		        if( petindex < 0 ) {
		        	fprint( "错误 宠物 无法作成\n");
		        }
		        one->indexOfPoolPet[petnumber]  = petindex;
				goto NEXT;
			}else{
				LodBadPetString( data, "err *total", petnumber);
				fprint( "错误 合成宠物字符串 无法作成\n");
				return FALSE;
			}
		}

		if( strncmp( firstToken , DATAENDCHECKPOINT, strlen( DATAENDCHECKPOINT ) ) == 0 ){
			rightData=1;
			goto NEXT;
		}


        print( "??? : %s[%s]\n" , linebuf, firstToken );
    NEXT:
        readindex++;
    }
DebugPoint=68;
    // Robin 0913  bad_chardata
    {    
		char name[4096];
		char* sp = strstr( data, "name=");
		// shan add
		char *sp2 = strstr( data, "ownt=");
		if( sp && sp2 ){		
			snprintf( name, sp2-sp-5, "%s", sp+5 );	
			print("\n登陆成功 人物名称:%s ", name );
		}
		else
			print("\n登陆成功 人物名称错误! ");
			
		if( data[strlen(data)-2] != '\n' ){
			print("\n玩家登陆出错! ");
			//return FALSE;
		}
    }
//	if( rightData != 1 ){
//		return FALSE;
//	}
    return TRUE;
}
static char CHAR_petdataString[4096];

char *CHAR_makePetStringFromPetIndex( int petindex)
{
	int		i;
	int		strlength = 0;

	memset( CHAR_petdataString, 0, sizeof(CHAR_petdataString));

    for( i = 0 ; i < CHAR_DATAINTNUM ; i ++ ){
        char    linedata[512];

#ifdef _SIMPLIFY_PETSTRING
		if( CHAR_getInt( petindex, i) == 0 ) continue;
#endif
        snprintf( linedata , sizeof(linedata),
                   "%s:%d" NONCHAR_DELIMITER, CHAR_setintdata[i], CHAR_getInt( petindex,i) );
        strcpysafe( &CHAR_petdataString[strlength],
                    sizeof( CHAR_petdataString ) - strlength, linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_petdataString ) ) {
			fprint( "err petdata buffer over\n");
        	return NULL;
    	}
    }

    for( i = 0 ; i < CHAR_DATACHARNUM ; i ++ ){
        char    linedata[512];
        char    escapebuffer[512];
#ifdef _SIMPLIFY_PETSTRING
		if( *CHAR_getChar( petindex, i) == NULL 
			&& strcmp( CHAR_setchardata[i], "name")
			&& strcmp( CHAR_setchardata[i], "ownt") )
			continue;
#endif
        snprintf( linedata , sizeof(linedata),
                  "%s:%s" NONCHAR_DELIMITER,
                  CHAR_setchardata[i],
                  makeEscapeString( CHAR_getChar(petindex, i), escapebuffer,
                                    sizeof( escapebuffer) ));

        strcpysafe( &CHAR_petdataString[strlength],
                    sizeof( CHAR_petdataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_petdataString ) ) {
			fprint( "错误 宠物数据缓冲不足\n");
        	return NULL;
    	}
    }

    for( i = 0; i < CHAR_MAXPETSKILLHAVE; i ++ ) {
		int		petskillid;
		char	linedata[512];
		petskillid = CHAR_getPetSkill( petindex, i);
		if( PETSKILL_getPetskillArray( petskillid) == -1 ) continue;
        snprintf( linedata , sizeof(linedata),
                  PETSKILLSERVERSTRING "%d:%d" NONCHAR_DELIMITER,i ,petskillid );
        strcpysafe( &CHAR_petdataString[strlength],
                    sizeof( CHAR_petdataString ) - strlength, linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_petdataString ) ) {
			fprint( "错误 宠物数据缓冲不足\n");
        	return NULL;
		}
	}

#ifndef _SIMPLIFY_ITEMSTRING
    for( i = 0 ; i < CHAR_MAXITEMHAVE ; i ++ ){
        char    linedata[2048];
        char    *itemstring=NULL;
        if( CHAR_getItemIndex( petindex, i) != -1 )
            itemstring = ITEM_makeStringFromItemIndex(
                						CHAR_getItemIndex( petindex, i), 1);
        if( itemstring == NULL )continue;
        snprintf( linedata , sizeof( linedata ),
                  PETITEMRESERVESTRING "%d:%s" NONCHAR_DELIMITER,
                  i, itemstring);
        strcpysafe( &CHAR_petdataString[strlength],
                    sizeof( CHAR_petdataString ) - strlength,
                    linedata );

        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_petdataString ) ) {
			fprint( "错误 宠物数据缓冲不足\n");
        	return NULL;
    	}
    }
#endif

	if( strstr( CHAR_petdataString, "name:") == NULL ||
		strstr( CHAR_petdataString, "ownt:") == NULL ){
		LodBadPetString( CHAR_petdataString, "名字 错误", -1);
	}
	return CHAR_petdataString;
}
void LodBadPetString( char *data, char *err, int ti)
{
	FILE *fp=NULL;
	if( data == NULL || strlen( data) <= 0 ) return;
	if( (fp=fopen( "badpetstring.txt", "a+")) == NULL ) return;
	fprintf( fp, "ti:%d %s:%s\n", ti, err, data);

	fclose( fp);
}
int CHAR_makePetFromStringToArg( char *src, Char *ch, int ti)
{
	int		readnum = 1;
	int		rc;
	BOOL	found;
	char	buff[4096];
	char	petfirstToken[256];
	char	petsecondToken[4096];
	int		i, findE=0;

	memset( ch, 0, sizeof( Char));
    CHAR_getDefaultChar(ch, 31010);

	if( strstr( src, "name:") == NULL ||
		strstr( src, "ownt:") == NULL ){//findE
		LodBadPetString( src, "make name err", ti);
		fprint( "err PetString make name err\n");
		return -1;
	}

	while( 1 ) {
        rc = getStringFromIndexWithDelim( src, NONCHAR_DELIMITER, readnum, buff,  sizeof( buff ));
		if( rc == TRUE ) {
	        if( buff[0] == '#' || buff[0] == '\n' || buff[0] == '\0' ){
	            readnum ++;
	            continue;
	        }
		}
        if( rc == TRUE ) {
			readnum ++;
	        rc = getStringFromIndexWithDelim( buff , ":" ,  1 ,
	                                           petfirstToken ,
	                                           sizeof(petfirstToken) );
	        if( rc == FALSE ) break;
	        strcpysafe( petsecondToken , sizeof( petsecondToken ),
	                    buff + strlen(petfirstToken) + strlen(":") );
//	        rc = getStringFromIndexWithDelim( buff , ":" ,  2 ,
//	                                           petsecondToken ,
//	                                           sizeof(petsecondToken) );

			found = FALSE;
	        for( i = 0 ; i < CHAR_DATAINTNUM ; i ++ ){
	            if( strcmp(petfirstToken ,CHAR_setintdata[i] ) == 0 ){
	                ch->data[i] = atoi( petsecondToken );
					found = TRUE;
	                break;
	            }
	        }
			if( !found ) {
		        for( i = 0 ; i < CHAR_DATACHARNUM ; i ++ ){
		            if( strcmp( petfirstToken ,CHAR_setchardata[i] ) == 0 ){
		                strcpysafe( ch->string[i].string,
		                            sizeof(ch->string[i].string),
		                            makeStringFromEscaped(petsecondToken) );
		            	found = TRUE;
						findE = 1;
		            	break;
		            }
		        }
			}
			if( !found) {
#ifndef _SIMPLIFY_ITEMSTRING
		        if( strncmp( petfirstToken , PETITEMRESERVESTRING,
		                     strlen( PETITEMRESERVESTRING ) ) == 0 )
				{
		            int itemindex;
		            itemindex = atoi( petfirstToken+strlen(PETITEMRESERVESTRING) );
		            if( itemindex < 0 || CHAR_MAXITEMHAVE <=  itemindex
		                ||  ch->indexOfExistItems[itemindex] != -1 ){
		                ;
		            }else{
		                ITEM_Item   itmone;
		                BOOL ret;

		                ret = ITEM_makeExistItemsFromStringToArg( petsecondToken ,
		                                                          &itmone,1);

		                if( ret == TRUE ){
		                    int existitemindex;
		                    existitemindex = ITEM_initExistItemsOne( &itmone );
		                    ch->indexOfExistItems[itemindex] = existitemindex;
			            	found = TRUE;
			            	break;
		                }
		            }
		        }
#endif

			}
        	if( !found ) {
        		if( strncmp( petfirstToken , PETSKILLSERVERSTRING,
        					 strlen( PETSKILLSERVERSTRING ) ) == 0 ){
					int		petskillnum = atoi( petfirstToken + strlen( PETSKILLSERVERSTRING));
		            if( CHAR_CHECKPETSKILLINDEX( petskillnum) ) {
						int	petskillid  = atoi( petsecondToken );
		                if( PETSKILL_getPetskillArray(petskillid) != -1) {
		                    ch->unionTable.indexOfPetskill[petskillnum] = petskillid;
		            		found = TRUE;
		            	}
		            }
                }
            }
			if( !found) {
		        fprint( "makePetFromString err?: %s : %s [%s] \n" , src, buff, petfirstToken );
			}
		}else {
			break;
		}
	}

	if( findE == 0 ){//findE
		LodBadPetString( src, "make charstr err", ti);
		fprint( "err PetString make charstr err\n");
		return -1;
	}

	return TRUE;
}

#ifdef _PERSONAL_FAME	// Arminius 8.30: 家族个人声望

void CHAR_earnFame(int index, int fame)
{
  int oldfame=CHAR_getInt(index, CHAR_FAME)+fame;
#ifdef _NEW_MANOR_LAW
	int momentum = CHAR_getInt(index,CHAR_MOMENTUM) + fame;
#endif

  if (oldfame>MAX_PERSONALFAME) oldfame=MAX_PERSONALFAME;
  if (oldfame<0) oldfame=0;
  CHAR_setInt(index, CHAR_FAME, oldfame);
#ifdef _NEW_MANOR_LAW
	if(momentum > MAX_PERSONAL_MOMENTUM) momentum = MAX_PERSONAL_MOMENTUM;
	else if(momentum < 0) momentum = 0;
	CHAR_setInt(index,CHAR_MOMENTUM,momentum);
#endif
}
#endif

// CoolFish: 2001/10/11 Unique Pet Code
#ifdef _UNIQUE_P_I
void CHAR_setPetUniCode(int petindex)
{
    	if( strcmp( CHAR_getChar(petindex, CHAR_UNIQUECODE), "" ) == 0 ||
			strstr( CHAR_getChar(petindex, CHAR_UNIQUECODE), "_" ) != NULL ||
			strstr( CHAR_getChar(petindex, CHAR_UNIQUECODE), UNIQUE_KEYCODEPET ) == NULL	){
    		char	petbuf[256];
    		time_t	t1;
    		time(&t1);
			sprintf(petbuf, "%ld%s%d%d", t1, UNIQUE_KEYCODEPET, getServernumber(), unique_p);
			unique_p++;
    		if (unique_p >= MAX_UNIQUE_P_I)
    			unique_p = 0;
    		CHAR_setChar(petindex, CHAR_UNIQUECODE, petbuf);
    	}
}

void ITEM_setItemUniCode(int itemindex)
{
	if( strcmp( ITEM_getChar(itemindex, ITEM_UNIQUECODE), "") == 0 ||
		strstr( ITEM_getChar(itemindex, ITEM_UNIQUECODE), "_") != NULL ){
    		char	itembuf[256];
    		time_t	t1;
    		time(&t1);

			sprintf(itembuf, "%ld%s%d%d%d", t1, "i", unique_i, getServernumber(), UNIQUE_KEYCODEITEM);

    		unique_i++;
    		if (unique_i >= MAX_UNIQUE_P_I)
    			unique_i = 0;
    		ITEM_setChar(itemindex, ITEM_UNIQUECODE, itembuf);
	}
}

void CHAR_DetainSameItem( int charaindex, int itemindex)
{
	ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, 100000);
	ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1);

		LogItem(
			CHAR_getChar( charaindex, CHAR_NAME),
			CHAR_getChar( charaindex, CHAR_CDKEY),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
			itemindex,
#else
			ITEM_getInt( itemindex, ITEM_ID),
#endif
			"SysDeleteSame(系统删除重覆道具)",
			CHAR_getInt( charaindex, CHAR_FLOOR),
			CHAR_getInt( charaindex, CHAR_X),
			CHAR_getInt( charaindex, CHAR_Y),
			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
			ITEM_getChar( itemindex, ITEM_NAME),
			ITEM_getInt( itemindex, ITEM_ID)
		);
//	ITEM_endExistItemsOne( itemindex);
}

void CHAR_DetainSamePet( int charaindex, int petindex)
{
	CHAR_setWorkInt( petindex, CHAR_WORKPLAYERINDEX, 100000);
	CHAR_setChar( petindex, CHAR_OWNERCDKEY, "SYS");
	CHAR_setChar( petindex, CHAR_OWNERCHARANAME, "SYS");
	CHAR_complianceParameter( petindex );
	LogPet(
				CHAR_getChar( charaindex, CHAR_NAME),
	 	   		CHAR_getChar( charaindex, CHAR_CDKEY),
	   	   		CHAR_getChar( petindex, CHAR_NAME),
	   	   		CHAR_getInt( petindex, CHAR_LV),
	   	   		"SysDeleteSame(删除重覆宠物)",
	   	   		CHAR_getInt( charaindex, CHAR_FLOOR),
	   	   		CHAR_getInt( charaindex, CHAR_X),
	   	   		CHAR_getInt( charaindex, CHAR_Y),
				CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);
//	CHAR_endCharOneArray( petindex );
}

void CHAR_DetainSameUCodePet( int charaindex, int petindex, int Ti)
{
	if( Ti == -1 ){
		int i;
		for( i = 0; i < CHAR_MAXPETHAVE; i++ ) {
		   int pindex = CHAR_getCharPet( charaindex, i);
		   if( !CHAR_CHECKINDEX( pindex) ) continue;
		   if( pindex == petindex ) break;
		}
		if( i >= CHAR_MAXPETHAVE ) return;
		CHAR_setCharPet( charaindex, i, -1);
	}else{
		CHAR_setCharPet( charaindex, Ti, -1);
	}
	CHAR_DetainSamePet( charaindex, petindex);
}

void CHAR_DetainSameUCodePoolPet( int charaindex, int petindex, int Ti)
{
	if( Ti == -1 ){
		int i;
		for( i = 0; i < CHAR_MAXPETHAVE; i++ ) {
		   int pindex = CHAR_getCharPet( charaindex, i);
		   if( !CHAR_CHECKINDEX( pindex) ) continue;
		   if( pindex == petindex ) break;
		}
		if( i >= CHAR_MAXPETHAVE ) return;
		CHAR_setCharPoolPet( charaindex, i, -1);
	}else{
		CHAR_setCharPoolPet( charaindex, Ti, -1);
	}
	CHAR_DetainSamePet( charaindex, petindex);
}

void CHAR_DetainSameUCodeItem( int charaindex, int itemindex, int Ti)
{
	if( Ti == -1 ){
		int i;
		for( i = 0; i < CHAR_MAXITEMHAVE; i++ ) {
		   int iindex = CHAR_getItemIndex( charaindex, i);
		   if( !ITEM_CHECKINDEX( iindex) ) continue;
		   if( iindex == itemindex ) break;
		}
		if( i >= CHAR_MAXITEMHAVE ) return;
		CHAR_setItemIndex( charaindex, i, -1);
	}else{
		CHAR_setItemIndex( charaindex, Ti, -1);
	}
	CHAR_DetainSameItem( charaindex, itemindex);
}

void CHAR_DetainSameUCodePoolItem( int charaindex, int itemindex, int Ti)
{
	if( Ti == -1 ){
		int i;
		for( i = 0; i < CHAR_MAXPOOLITEMHAVE; i++ ) {
		   int iindex = CHAR_getPoolItemIndex( charaindex, i);
		   if( !ITEM_CHECKINDEX( iindex) ) continue;
		   if( iindex == itemindex ) break;
		}
		if( i >= CHAR_MAXPOOLITEMHAVE ) return;
		CHAR_setPoolItemIndex( charaindex, i, -1);
	}else{
		CHAR_setPoolItemIndex( charaindex, Ti, -1);
	}
	CHAR_DetainSameItem( charaindex, itemindex);
}

void Check_P_I_UniCode( int charaindex)
{
#ifdef _CHECK_UCODE
	int	i = 0, j = 0;
	int	petindex, itemindex;

	int MAXPLAYER = CHAR_getPlayerMaxNum();
	int MAXPET = CHAR_getPetMaxNum();
	int MAXITEM = ITEM_getITEM_itemnum();

	for( i=0; i < CHAR_MAXPETHAVE; i++ ) {
		petindex = CHAR_getCharPet( charaindex, i);
		if( !CHAR_CHECKINDEX( petindex) ) continue;
		for( j=MAXPLAYER; j<MAXPLAYER+MAXPET; j++ ){
			if( !CHAR_CHECKINDEX( j) ) continue;
			if( j == petindex ) continue;
			if( !strcmp( CHAR_getChar( petindex, CHAR_UNIQUECODE), CHAR_getChar( j, CHAR_UNIQUECODE) ) ){
				CHAR_DetainSameUCodePet( charaindex, petindex, i);
				if( CHAR_getWorkInt( j, CHAR_WORKPLAYERINDEX) != 100000 ) {
					int userindex = CHAR_getWorkInt( j, CHAR_WORKPLAYERINDEX);
					if( !CHAR_CHECKINDEX( userindex) ) break;
					CHAR_DetainSameUCodePet( userindex, j, i);
				}
				break;
			}
		}
	}
/*
	for( i=0; i < CHAR_MAXPOOLPETHAVE; i++ ) {
		petindex = CHAR_getCharPoolPet( charaindex, i);
		if( !CHAR_CHECKINDEX( petindex) ) continue;
		for( j=MAXPLAYER; j<MAXPLAYER+MAXPET; j++ ){
			if( !CHAR_CHECKINDEX( j) ) continue;
			if( j == petindex ) continue;
			if( !strcmp( CHAR_getChar( petindex, CHAR_UNIQUECODE), CHAR_getChar( j, CHAR_UNIQUECODE) ) ){
				CHAR_DetainSameUCodePoolPet( charaindex, petindex, i);
				if( CHAR_getWorkInt( j, CHAR_WORKPLAYERINDEX) != 100000 ) {
					int userindex = CHAR_getWorkInt( j, CHAR_WORKPLAYERINDEX);
					if( !CHAR_CHECKINDEX( userindex) ) break;
					CHAR_DetainSameUCodePoolPet( userindex, j, i);
				}
				break;
			}
		}
	}
*/
	for( i=0; i<CHAR_MAXITEMHAVE; i++){
		itemindex = CHAR_getItemIndex( charaindex, i);
		if( !ITEM_CHECKINDEX( itemindex) ) continue;
		for( j=0; j<MAXITEM; j++ ){
			if( !ITEM_CHECKINDEX( j) ) continue;
			if( j == itemindex ) continue;
			if( !strcmp( ITEM_getChar(j, ITEM_UNIQUECODE), ITEM_getChar( itemindex, ITEM_UNIQUECODE))){
				CHAR_DetainSameUCodeItem( charaindex, itemindex, i);
				if( CHAR_getWorkInt( j, ITEM_WORKCHARAINDEX) != 100000 ) {
					int userindex = ITEM_getWorkInt( j, ITEM_WORKCHARAINDEX);
					if( !CHAR_CHECKINDEX( userindex) ) break;
					CHAR_DetainSameUCodeItem( userindex, j, i);
				}
				break;
			}
		}
	}
/*
	for( i=0; i<CHAR_MAXPOOLITEMHAVE; i++){
		itemindex = CHAR_getPoolItemIndex( charaindex, i);
		if( !ITEM_CHECKINDEX( itemindex) ) continue;
		for( j=0; j<MAXITEM; j++ ){
			if( !ITEM_CHECKINDEX( j) ) continue;
			if( j == itemindex ) continue;
			if( !strcmp( ITEM_getChar(j, ITEM_UNIQUECODE), ITEM_getChar( itemindex, ITEM_UNIQUECODE))){
				CHAR_DetainSameUCodePoolItem( charaindex, itemindex, i);
				if( CHAR_getWorkInt( j, ITEM_WORKCHARAINDEX) != 100000 ) {
					int userindex = ITEM_getWorkInt( j, ITEM_WORKCHARAINDEX);
					if( !CHAR_CHECKINDEX( userindex) ) break;
					CHAR_DetainSameUCodePoolItem( userindex, j, i);
				}
				break;
			}
		}
	}
*/
#endif
}
#endif

// Arminius 12.15 判断是否是男士
int IsMale(int charindex) {
  int bbi=CHAR_getInt(charindex, CHAR_BASEBASEIMAGENUMBER);

  if ((bbi==SPR_001em)||(bbi==SPR_002em)||(bbi==SPR_003em)||(bbi==SPR_004em)||
      (bbi==SPR_011em)||(bbi==SPR_012em)||(bbi==SPR_013em)||(bbi==SPR_014em)||
      (bbi==SPR_021em)||(bbi==SPR_022em)||(bbi==SPR_023em)||(bbi==SPR_024em)||
      (bbi==SPR_031em)||(bbi==SPR_032em)||(bbi==SPR_033em)||(bbi==SPR_034em)||
      (bbi==SPR_041em)||(bbi==SPR_042em)||(bbi==SPR_043em)||(bbi==SPR_044em)||
      (bbi==SPR_051em)||(bbi==SPR_052em)||(bbi==SPR_053em)||(bbi==SPR_054em))
  {
    return 1;
  }

  return 0;
}

// Arminius 12.15 判断是否是女士
int IsFemale(int charindex) {
  int bbi=CHAR_getInt(charindex, CHAR_BASEBASEIMAGENUMBER);

  if ((bbi==SPR_061em)||(bbi==SPR_062em)||(bbi==SPR_063em)||(bbi==SPR_064em)||
      (bbi==SPR_071em)||(bbi==SPR_072em)||(bbi==SPR_073em)||(bbi==SPR_074em)||
      (bbi==SPR_081em)||(bbi==SPR_082em)||(bbi==SPR_083em)||(bbi==SPR_084em)||
      (bbi==SPR_091em)||(bbi==SPR_092em)||(bbi==SPR_093em)||(bbi==SPR_094em)||
      (bbi==SPR_101em)||(bbi==SPR_102em)||(bbi==SPR_103em)||(bbi==SPR_104em)||
      (bbi==SPR_111em)||(bbi==SPR_112em)||(bbi==SPR_113em)||(bbi==SPR_114em))
  {
    return 1;
  }

  return 0;
}

#ifdef _TYPE_TOXICATION
BOOL CHAR_CanCureFlg( int charaindex, char *arg)
{
	int fd = getfdFromCharaIndex( charaindex );
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER	) return TRUE;
	if( getToxication( fd) == 1 ){
		if( strstr( arg, "FALSE") == 0 )	{
			char buf[256];
			sprintf( buf,"因中毒无法补%s。", arg);
			CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORYELLOW);
		}
		return FALSE;
	}
	return TRUE;
}
#endif

INLINE int CHAR_AddMaxExp( int charaindex, int addexp)
{
	int Myexp = CHAR_getInt( charaindex, CHAR_EXP);
	Myexp = min( Myexp + addexp, 1224160000 );
	CHAR_setInt( charaindex, CHAR_EXP, Myexp );
	return addexp;
}

INLINE int CHAR_setMaxExpFromLevel( int charaindex, int level)
{
	int LVexp;
	if( (LVexp = CHAR_GetLevelExp( charaindex, level+1)) < 0 )return -1;
	CHAR_setInt( charaindex, CHAR_EXP, LVexp);
	return LVexp;
}

INLINE int CHAR_setMaxExp( int charaindex, unsigned long int Setexp)
{
	if( !CHAR_CHECKINDEX( charaindex) ) return -1;
	CHAR_setInt( charaindex, CHAR_EXP, Setexp);
	return Setexp;
}
#ifdef _NEWOPEN_MAXEXP
INLINE int CHAR_ChangeExp( int charaindex )
{
	int level, defexp, Myexp;
	if( !CHAR_CHECKINDEX( charaindex) ) return -1;

	level = CHAR_getInt( charaindex, CHAR_LV);
	if( (defexp = CHAR_GetOldLevelExp( level)) <  0 ) return -1;
	Myexp = CHAR_getInt( charaindex, CHAR_OLDEXP);

	if( Myexp > CHAR_GetOldLevelExp( level+1) ){
		Myexp = 0;
	}else if( Myexp >= defexp ){
		Myexp = Myexp - defexp;
	}
	if( Myexp < 0 ) Myexp = 0;

	CHAR_setInt( charaindex, CHAR_EXP, Myexp);
	CHAR_setInt( charaindex, CHAR_OLDEXP, 0);

	CHAR_send_P_StatusString( charaindex ,CHAR_P_STRING_EXP|CHAR_P_STRING_LV);
	return Myexp;
}

INLINE int CHAR_HandleExp( int charaindex )
{
	int needexp, level, Myexp;
	if( !CHAR_CHECKINDEX( charaindex) ) return -1;
	level = CHAR_getInt( charaindex, CHAR_LV);
	Myexp = CHAR_getInt( charaindex, CHAR_EXP);
	if( (needexp = CHAR_GetLevelExp( charaindex, level+1)) < 0 ) return -1;
	Myexp = Myexp - needexp;
	if( Myexp < 0 ) Myexp = 0;
	CHAR_setInt( charaindex, CHAR_EXP, Myexp);
	return Myexp;
}
#endif

#ifdef _FIX_TSKILLCAN
BOOL CHAR_PETSKILLCAN( int toindex)
{
	int i, j, itemindex, petindex;
	int FixItem[20]={
		20270, 20271, 20272, 20273, 20274,
		20275, 20276, 20277, 20278, 20279,
		20280, 20281, 20282, 20283, 20284,
		20287, 20288, 20289, 20290, 20291
	};
	for (i = CHAR_STARTITEMARRAY; i < CHAR_MAXITEMHAVE; i ++) {
		itemindex = CHAR_getItemIndex( toindex, i);
		if( !ITEM_CHECKINDEX( itemindex)) continue;
		for( j=0; j<20; j++){
			if( ITEM_getInt( itemindex, ITEM_ID) == FixItem[j] ){
				ITEM_setInt( itemindex, ITEM_CANMERGEFROM, 0);
			}
		}
		if( ITEM_getInt( itemindex, ITEM_ID) != 1623 )continue;
		CHAR_setItemIndex( toindex, i, -1 );
		ITEM_endExistItemsOne( itemindex);
		CHAR_sendItemDataOne( toindex, i);
	}	

	for (i = 0; i < CHAR_MAXPOOLITEMHAVE; i++){
		int itemindex = CHAR_getPoolItemIndex(toindex, i);
		if( !ITEM_CHECKINDEX( itemindex)) continue;
		for( j=0; j<20; j++){
			if( ITEM_getInt( itemindex, ITEM_ID) == FixItem[j] ){
				ITEM_setInt( itemindex, ITEM_CANMERGEFROM, 0);
			}
		}
		if( ITEM_getInt( itemindex, ITEM_ID) != 1623 )continue;
		CHAR_setPoolItemIndex( toindex, i, -1);
		ITEM_endExistItemsOne( itemindex);
	}

	for( i=0; i < CHAR_MAXPETHAVE; i++)	{
		char szPet[128];
		BOOL Finds = FALSE;
		petindex = CHAR_getCharPet( toindex, i);
		if( !CHAR_CHECKINDEX( petindex) ) continue;

		{//清除狂暴攻击
			int j;
			for( j=0; j<CHAR_MAXPETSKILLHAVE; j++)	{
				if( CHAR_getPetSkill( petindex, j) != 541 )continue;
				if( CHAR_getInt ( petindex, CHAR_FUSIONBEIT) == 1 ) continue;
				if( CHAR_getInt( petindex, CHAR_PETID) != 979 &&
					CHAR_getInt( petindex, CHAR_PETID) != 965 ){
					Finds = TRUE;
					CHAR_setPetSkill( petindex, j, -1);
				}
			}
		}

		if( CHAR_getInt( petindex, CHAR_PETID) == 304 ) continue;
		{
			int j;
			for( j=0; j<CHAR_MAXPETSKILLHAVE; j++)	{
				if( CHAR_getPetSkill( petindex, j) != 41 )continue;
				CHAR_setPetSkill( petindex, j, -1);
				Finds = TRUE;
			}
		}
		if( CHAR_getInt( petindex, CHAR_PETID) != 968 &&
			CHAR_getInt ( petindex, CHAR_FUSIONBEIT) != 1 ){ //不是融合宠
			int j;
			for( j=0; j<CHAR_MAXPETSKILLHAVE; j++)	{
				if( CHAR_getPetSkill( petindex, j) != 542 )continue;
				CHAR_setPetSkill( petindex, j, -1);
				Finds = TRUE;
			}
		}

		if( Finds == TRUE ){
			CHAR_complianceParameter( petindex );
			snprintf( szPet, sizeof( szPet ), "K%d", i);
			CHAR_sendStatusString( toindex, szPet );
			snprintf( szPet, sizeof( szPet ), "W%d", i);
			CHAR_sendStatusString( toindex, szPet );
		}
	}

	for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
		BOOL Finds = FALSE;
		int petindex = CHAR_getCharPoolPet(toindex,i);
		if( !CHAR_CHECKINDEX( petindex) ) continue;
		if( CHAR_getInt( petindex, CHAR_PETID) != 304 ){
			int j;
			for( j=0; j<CHAR_MAXPETSKILLHAVE; j++)	{
				if( CHAR_getPetSkill( petindex, j) != 41 )continue;
				CHAR_setPetSkill( petindex, j, -1);
				Finds = TRUE;
			}
		}
		{//清除狂暴攻击
			int j;
			for( j=0; j<CHAR_MAXPETSKILLHAVE; j++)	{
				if( CHAR_getPetSkill( petindex, j) != 541 )continue;
				if( CHAR_getInt ( petindex, CHAR_FUSIONBEIT) == 1 ) continue;
				if( CHAR_getInt( petindex, CHAR_PETID) != 979 &&
					CHAR_getInt( petindex, CHAR_PETID) != 965 ){
					Finds = TRUE;
					CHAR_setPetSkill( petindex, j, -1);
				}
			}
		}
//CHAR_setInt ( newindex, CHAR_FUSIONBEIT, 1);
		if( CHAR_getInt( petindex, CHAR_PETID) != 968 &&
			CHAR_getInt ( petindex, CHAR_FUSIONBEIT) != 1 ){ //不是融合宠
			int j;
			for( j=0; j<CHAR_MAXPETSKILLHAVE; j++)	{
				if( CHAR_getPetSkill( petindex, j) != 542 )continue;
				CHAR_setPetSkill( petindex, j, -1);
				Finds = TRUE;
			}
		}

	}

	return TRUE;
}

#endif

INLINE int _CHAR_AddPileItem( char *file, int line, int charaindex, int itemindex)
{
#ifdef _ITEM_PILENUMS
	int itempile , mypile, surplus, ret=-1;

	surplus = CHAR_findSurplusItemBox( charaindex);
	itempile = ITEM_getInt( itemindex, ITEM_USEPILENUMS);
	mypile = CHAR_getMyMaxPilenum( charaindex);

	if( itempile > (surplus*mypile) || itempile <= 0 ){
		return -1;
	}

	if( mypile >= itempile ){
		ret = CHAR_addItemSpecificItemIndex( charaindex, itemindex);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
				ITEM_endExistItemsOne( itemindex);
			return -1;
		}
		CHAR_sendItemDataOne( charaindex, ret);
	}else{
		int newindex[10]={	-1,-1,-1,-1,-1,
							-1,-1,-1,-1,-1};
		int i,  ti=0, ItemID, MaxPile;
		MaxPile = itempile;
		ItemID = ITEM_getInt( itemindex, ITEM_ID);
		while( MaxPile > 0 ){
			if( ti >= 10 ) return -1;
			newindex[ti] = ITEM_makeItemAndRegist( ItemID);
			if( !ITEM_CHECKINDEX( newindex[ti]) ) return -1;
			if( mypile > MaxPile ) mypile = MaxPile;
			ITEM_setInt( newindex[ti], ITEM_USEPILENUMS, mypile);
			ti++;
			MaxPile -= mypile;
			if( MaxPile < 0 ) return -1;
		}
		for( i=0; i<10; i++ ){
			if( newindex[i] == -1 ) break;
			ret = CHAR_addItemSpecificItemIndex( charaindex, newindex[i]);
			if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
				ITEM_endExistItemsOne( newindex[i]);
				return -1;
			}
			CHAR_sendItemDataOne( charaindex, ret);
		}
		ITEM_endExistItemsOne( itemindex);
	}
	return ret;
#endif
}


INLINE int _CHAR_DelItem( char *file, int line, int charaindex, int ti, int num, int flg)
{
//	char token[256];
	int pilenum;
	int itemindex = CHAR_getItemIndex( charaindex , ti );
	if( !ITEM_CHECKINDEX(itemindex) ) return 0;
#ifdef _ITEM_PILENUMS
	pilenum = ITEM_getInt( itemindex, ITEM_USEPILENUMS);
	if( pilenum < num ) return FALSE;
	pilenum = pilenum - num;
	ITEM_setInt( itemindex, ITEM_USEPILENUMS, pilenum);
	if( pilenum <= 0 ){
#endif
/*
		if( flg == 1 ){
			sprintf( token, "交出%s。", ITEM_getChar( itemindex, ITEM_NAME));
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
		}
*/
		CHAR_setItemIndex( charaindex, ti, -1);
		ITEM_endExistItemsOne( itemindex);
#ifdef _ITEM_PILENUMS
	}
#endif
	CHAR_sendItemDataOne( charaindex, ti);

	return 1;
}

int CHAR_getMaxHaveGold( int charaindex)
{
	int MaxGold;
#ifdef _FIX_MAX_GOLD				// WON ADD 增加人物金钱上限
	int trans = CHAR_getInt( charaindex, CHAR_TRANSMIGRATION);
	MaxGold = 1000000 + trans*( 1800000);
#else
	MaxGold = CHAR_MAXGOLDHAVE;
#endif

	return MaxGold;
}

INLINE int _CHAR_AddGold( char *file, int line, int charaindex, int gold)
{
	char token[256];
	int MyGold, MaxGold;

	MaxGold = CHAR_getMaxHaveGold( charaindex);
	MyGold = CHAR_getInt( charaindex, CHAR_GOLD);
	MyGold = (MyGold>MaxGold)?MaxGold:MyGold;
	gold = (gold>MaxGold)?MaxGold:gold;
	sprintf( token, "得到%d石币", gold);
	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
	MyGold += gold;

	if( MyGold > MaxGold ){
		int reGolds=0;
		int MyGBGold = CHAR_getInt( charaindex, CHAR_PERSONAGOLD);
		reGolds = MyGold - MaxGold;
		MyGBGold += reGolds;
		MyGBGold = (MyGBGold > CHAR_MAXPERSONAGOLD)?CHAR_MAXPERSONAGOLD:MyGBGold;
		CHAR_setInt( charaindex, CHAR_PERSONAGOLD, MyGBGold);
		sprintf( token,"存款：%d ，银行剩馀：%d 。", reGolds, CHAR_getInt( charaindex, CHAR_PERSONAGOLD ));
		CHAR_talkToCli( charaindex, -1, token,  CHAR_COLORYELLOW);
			LogBankStone(    CHAR_getChar( charaindex, CHAR_NAME ), CHAR_getChar( charaindex, CHAR_CDKEY ),
						charaindex, reGolds, 
						"GB_Bank_save(宝箱)", 
						CHAR_getInt( charaindex, CHAR_FLOOR),
						CHAR_getInt( charaindex, CHAR_X ), CHAR_getInt( charaindex, CHAR_Y ),
						CHAR_getInt( charaindex, CHAR_GOLD ),
						CHAR_getInt( charaindex, CHAR_PERSONAGOLD )
						);
		MyGold = MaxGold;
	}
	CHAR_setInt( charaindex, CHAR_GOLD, MyGold);
	CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_GOLD);

	return 1;
}

INLINE int _CHAR_DelGold( char *file, int line, int charaindex, int gold)
{
	char token[256];
	int MyGold, MaxGold;

	MaxGold = CHAR_getMaxHaveGold( charaindex);
	MyGold = CHAR_getInt( charaindex, CHAR_GOLD);
	MyGold = (MyGold>MaxGold)?MaxGold:MyGold;
	gold = (gold>MaxGold)?MaxGold:gold;

	if( MyGold < gold ){
		sprintf( token, "没有足够的石币！(%d石币)", gold);
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
		return 0;
	}
	MyGold -= gold;
	MyGold = ( MyGold < 0 )?0:MyGold;
	CHAR_setInt( charaindex, CHAR_GOLD, MyGold);
	CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_GOLD);
	return 1;
}


#ifdef _CHAR_POOLITEM
INLINE int _CHAR_getDepotItemIndex( char *file, int line, int index ,int iindex)
{
    if( !CHAR_CHECKINDEX(index)) {
		print( "err %s:%d from %s:%d\n", __FILE__, __LINE__,file, line);
    	return -1;
    }
    if( CHAR_MAXDEPOTITEMHAVE <= iindex || iindex < 0 ) {
		print( "err %s:%d from %s:%d \n", __FILE__, __LINE__,file, line);
    	return -1;
    }

	if( !CHAR_CheckDepotItem( index) ){
		print( "err %s:%d from %s:%d\n", __FILE__, __LINE__,file, line);
    	return -1;
	}

    return CHAR_chara[index].indexOfExistDepotItems[iindex];
}

INLINE int _CHAR_setDepotItemIndex( char *file, int line, int index ,int iindex,int id )
{
    int buf;
    if( !CHAR_CHECKINDEX(index)) {
		print( "err %s:%d \n", file, line);
    	return -1;
    }
    if( CHAR_MAXDEPOTITEMHAVE <= iindex || iindex < 0 ) {
		print( "err %s:%d \n", file, line);
    	return -1;
    }
	if( !CHAR_CheckDepotItem( index) ){
		print( "err %s:%d from %s:%d\n", __FILE__, __LINE__,file, line);
    	return -1;
	}
    buf = CHAR_chara[index].indexOfExistDepotItems[iindex];
    CHAR_chara[index].indexOfExistDepotItems[iindex] = id;
    return buf;
}
void CHAR_removeHaveDepotItem( Char* ch )
{
    if( ch == NULL )return;
	if( ch->indexOfExistDepotItems == NULL ) return;
	freeMemory( ch->indexOfExistDepotItems );
	ch->indexOfExistDepotItems = NULL;
}

void CHAR_removeDepotItem( int charaindex)
{
	int i,itemindex;
	Char*   ch = NULL;
	if( !CHAR_CHECKINDEX( charaindex) ) return;
	if( !CHAR_CheckDepotItem( charaindex) ) return; //仓库未存在
	ch = CHAR_getCharPointer( charaindex);
    if( ch == NULL ){
		print( "removeDepotItem err ch == NULL :%d\n", charaindex);
		return;
	}

	for( i=0; i<CHAR_MAXDEPOTITEMHAVE; i++){
		itemindex=CHAR_getDepotItemIndex( charaindex, i);
		if( !ITEM_CHECKINDEX( itemindex) ) continue;
		ITEM_endExistItemsOne( itemindex);
	}
	CHAR_removeHaveDepotItem( ch );
}

char *CHAR_makeDepotItemFromCharIndex( int charaindex)
{
	char    linedata[2048];
	char    *itemstring=NULL;
	int i, itemindex, strlength=0;
	Char*   ch = NULL;

	if( !CHAR_CHECKINDEX( charaindex) ) return NULL;
	ch = CHAR_getCharPointer( charaindex);
    if( ch == NULL ){
		print( "makeDepotItem err ch == NULL :%d\n", charaindex);
		return NULL;
	}

	memset( CHAR_dataString, 0, sizeof(CHAR_dataString));
    for( i = 0 ; i < CHAR_MAXDEPOTITEMHAVE ; i ++ ){
		itemindex = ch->indexOfExistDepotItems[i];
		if( !ITEM_CHECKINDEX( itemindex) ) continue;
		itemstring = ITEM_makeStringFromItemIndex( ch->indexOfExistDepotItems[i], 0);
        if( itemstring == NULL )continue;

        snprintf( linedata , sizeof( linedata ),
			DEPOTITEMRESERVESTRING "%d=%s" CHAR_DELIMITER, i, itemstring);

        strcpysafe( &CHAR_dataString[strlength], sizeof( CHAR_dataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_dataString ) ) {
			return NULL;
	   	}
    }
	return CHAR_dataString;
}

BOOL CHAR_makeDepotItemStringToChar( int charaindex, char* data)
{
	int readindex=1, itemindex;
	Char*   ch = NULL;
	BOOL    ret;
	char firstToken[256], secondToken[4096], linebuf[4096];

    if( data[0] == '\0' )
        return -1;
	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;
	ch = CHAR_getCharPointer( charaindex);
    if( ch == NULL ){
		print( "makeDepotItem err ch == NULL :%d\n", charaindex);
		 return FALSE;
	}

    while( 1 ){
		memset( linebuf, 0, sizeof( linebuf));
		memset( firstToken, 0, sizeof( firstToken));
		memset( secondToken, 0, sizeof( secondToken));
        ret = getStringFromIndexWithDelim( data ,CHAR_DELIMITER,
									readindex, linebuf,  sizeof( linebuf ) );
        if( ret == FALSE )break;
        if( linebuf[0] == '#'   || linebuf[0] == '\n'  || linebuf[0] == '\0' ){           
			goto NEXT;
        }
        if( (ret=getStringFromIndexWithDelim( linebuf , "=" ,  1 ,
									firstToken, sizeof(firstToken) )) == FALSE )  return FALSE;
        strcpysafe( secondToken , sizeof( secondToken ),
                    linebuf + strlen( firstToken) + strlen("=") );

        if( strncmp( firstToken, DEPOTITEMRESERVESTRING, strlen( DEPOTITEMRESERVESTRING)) == 0 ){
            if( strcmp( secondToken, NULLITEM) == 0 ) goto NEXT;
            itemindex = atoi( firstToken+strlen(DEPOTITEMRESERVESTRING) );
            if( itemindex < 0 || CHAR_MAXDEPOTITEMHAVE <=  itemindex
                ||  ch->indexOfExistDepotItems[itemindex] != -1 ){
                ;
            }else{
                ITEM_Item   itmone;
                ret = ITEM_makeExistItemsFromStringToArg( secondToken , &itmone, 0 );
                if( ret == TRUE ){
                    int existitemindex;
                    existitemindex = ITEM_initExistItemsOne( &itmone );
                    ch->indexOfExistDepotItems[itemindex] = existitemindex;
                }
            }
            goto NEXT;
        }
        print( "??? : %s[%s]\n" , linebuf, firstToken );
NEXT:
        readindex++;
    }
    return TRUE;
}

void CHAR_ShowMyDepotItems( int charaindex)
{
	int i, itemindex;

	print( "\nShowMyDepotItems:\n");
	if( !CHAR_CheckDepotItem( charaindex) ) return;
	for( i=0; i<CHAR_MAXDEPOTITEMHAVE; i++){
		itemindex = CHAR_getDepotItemIndex( charaindex, i);
		if( !ITEM_CHECKINDEX( itemindex) ) continue;
		print( "DEPOT%d: %d-%s\n", i, itemindex, ITEM_getChar( itemindex, ITEM_NAME) );
	}
}

int CHAR_getfindEmptyDepotItem( int charaindex)
{
	int i, itemindex, cnt=0;
	if( !CHAR_CheckDepotItem( charaindex) ) return cnt;
	for( i=0; i</*60*/CHAR_MAXDEPOTITEMHAVE; i++){
		itemindex = CHAR_getDepotItemIndex( charaindex, i);
		if( !ITEM_CHECKINDEX( itemindex) )cnt++;
	}
	return cnt;
}

int CHAR_findEmptyDepotItem( int charaindex)
{
	int i, itemindex;
	if( !CHAR_CheckDepotItem( charaindex) ) return -1;
	for( i=0; i<CHAR_MAXDEPOTITEMHAVE; i++){
		itemindex = CHAR_getDepotItemIndex( charaindex, i);
		if( !ITEM_CHECKINDEX( itemindex) ) return i;
	}
	return -1;
}

BOOL CHAR_SaveDepotItem( int charaindex)
{
	char *databuf=NULL;
	char *CdKey=NULL;
	int fd;


	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;
	fd = getfdFromCharaIndex( charaindex);
	if( !CHAR_CheckDepotItem( charaindex) ) return FALSE; //仓库未存在
	if( (CdKey=CHAR_getChar( charaindex, CHAR_CDKEY)) == NULL ) return FALSE;
	if( (databuf = CHAR_makeDepotItemFromCharIndex( charaindex)) == NULL ){
		CHAR_removeDepotItem( charaindex);
		return FALSE;
	}
	saacproto_ACCharSavePoolItem_send( acfd, charaindex, CONNECT_getFdid(fd), CdKey, databuf);
	CHAR_removeDepotItem( charaindex);

	return TRUE;
}

BOOL CHAR_GetDepotItem( int meindex, int charaindex)
{
	char *CdKey=NULL;
	int fd;

	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;
	fd = getfdFromCharaIndex( charaindex);
	if( CHAR_CheckDepotItem( charaindex) ) return FALSE;//仓库已存在
	if( (CdKey=CHAR_getChar( charaindex, CHAR_CDKEY)) == NULL ) return FALSE;

	//向AC要仓库资料
	saacproto_ACCharGetPoolItem_send( acfd, meindex, charaindex, CONNECT_getFdid(fd), CdKey);
	return TRUE;
}

BOOL CHAR_CheckDepotItem( int charaindex)
{
	Char*   ch = NULL;
	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;
	if( (ch = CHAR_getCharPointer( charaindex)) == NULL ) return FALSE;
	if( ch->indexOfExistDepotItems == NULL ) return FALSE;
	return TRUE;
}

#endif


#ifdef _CHAR_POOLPET
INLINE int _CHAR_getDepotPetIndex( char *file, int line, int index ,int iindex)
{
    if( !CHAR_CHECKINDEX(index)) {
		print( "err %s:%d from %s:%d\n", __FILE__, __LINE__,file, line);
    	return -1;
    }
    if( CHAR_MAXDEPOTPETHAVE <= iindex || iindex < 0 ) {
		print( "err %s:%d from %s:%d \n", __FILE__, __LINE__,file, line);
    	return -1;
    }

	if( !CHAR_CheckDepotPet( index) ){
		print( "err %s:%d from %s:%d\n", __FILE__, __LINE__,file, line);
    	return -1;
	}

    return CHAR_chara[index].indexOfExistDepotPets[iindex];
}

INLINE int _CHAR_setDepotPetIndex( char *file, int line, int index ,int iindex,int id )
{
    int buf;
    if( !CHAR_CHECKINDEX(index)) {
		print( "err %s:%d \n", file, line);
    	return -1;
    }
    if( CHAR_MAXDEPOTPETHAVE <= iindex || iindex < 0 ) {
		print( "err %s:%d \n", file, line);
    	return -1;
    }
	if( !CHAR_CheckDepotPet( index) ){
		print( "err %s:%d from %s:%d\n", __FILE__, __LINE__,file, line);
    	return -1;
	}
    buf = CHAR_chara[index].indexOfExistDepotPets[iindex];
    CHAR_chara[index].indexOfExistDepotPets[iindex] = id;
    return buf;
}
void CHAR_removeHaveDepotPet( Char* ch )
{
    if( ch == NULL )return;
	if( ch->indexOfExistDepotPets == NULL ) return;
	freeMemory( ch->indexOfExistDepotPets );
	ch->indexOfExistDepotPets = NULL;
}

void CHAR_removeDepotPet( int charaindex)
{
	int i,petindex;
	Char*   ch = NULL;
	if( !CHAR_CHECKINDEX( charaindex) ) return;
	if( !CHAR_CheckDepotPet( charaindex) ) return; //仓库未存在
	ch = CHAR_getCharPointer( charaindex);
    if( ch == NULL ){
		print( "removeDepotPet err ch == NULL :%d\n", charaindex);
		return;
	}

	for( i=0; i<CHAR_MAXDEPOTPETHAVE; i++){
		petindex=CHAR_getDepotPetIndex( charaindex, i);
		if( !CHAR_CHECKINDEX( petindex) ) continue;
		CHAR_endCharOneArray( petindex);
	}
	CHAR_removeHaveDepotPet( ch );
}

char *CHAR_makeDepotPetFromCharIndex( int charaindex)
{
	char    linedata[2048];
	char    *petstring=NULL;
	int i, petindex, strlength=0;
	Char*   ch = NULL;

	if( !CHAR_CHECKINDEX( charaindex) ) return NULL;
	ch = CHAR_getCharPointer( charaindex);
    if( ch == NULL ){
		print( "makeDepotPet err ch == NULL :%d\n", charaindex);
		return NULL;
	}

	memset( CHAR_dataString, 0, sizeof(CHAR_dataString));
    for( i = 0 ; i < CHAR_MAXDEPOTPETHAVE ; i ++ ){
		petindex = ch->indexOfExistDepotPets[i];
		if( !CHAR_CHECKINDEX( petindex) ) continue;
		petstring = CHAR_makePetStringFromPetIndex( ch->indexOfExistDepotPets[i]);
        if( petstring == NULL )continue;

        snprintf( linedata , sizeof( linedata ),
			DEPOTPETRESERVESTRING "%d=%s" CHAR_DELIMITER, i, petstring);

        strcpysafe( &CHAR_dataString[strlength], sizeof( CHAR_dataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( CHAR_dataString ) ) {
			return NULL;
	   	}
    }
	return CHAR_dataString;
}

BOOL CHAR_makeDepotPetStringToChar( int charaindex, char* data)
{
	int readindex=1, petindex;
	Char*   ch = NULL;
	BOOL    ret;
	char firstToken[256], secondToken[4096], linebuf[4096];

    if( data[0] == '\0' )
        return -1;
	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;
	ch = CHAR_getCharPointer( charaindex);
    if( ch == NULL ){
		print( "makeDepotPet err ch == NULL :%d\n", charaindex);
		 return FALSE;
	}

    while( 1 ){
		memset( linebuf, 0, sizeof( linebuf));
		memset( firstToken, 0, sizeof( firstToken));
		memset( secondToken, 0, sizeof( secondToken));
        ret = getStringFromIndexWithDelim( data ,CHAR_DELIMITER,
									readindex, linebuf,  sizeof( linebuf ) );
        if( ret == FALSE )break;
        if( linebuf[0] == '#'   || linebuf[0] == '\n'  || linebuf[0] == '\0' ){           
			goto NEXT;
        }
        if( (ret=getStringFromIndexWithDelim( linebuf , "=" ,  1 ,
									firstToken, sizeof(firstToken) )) == FALSE )  return FALSE;
        strcpysafe( secondToken , sizeof( secondToken ),
                    linebuf + strlen( firstToken) + strlen("=") );

        if( strncmp( firstToken, DEPOTPETRESERVESTRING, strlen( DEPOTPETRESERVESTRING)) == 0 ){
            if( strcmp( secondToken, NULLITEM) == 0 ) goto NEXT;
            petindex = atoi( firstToken+strlen(DEPOTPETRESERVESTRING) );
            if( petindex < 0 || CHAR_MAXDEPOTPETHAVE <=  petindex
                ||  ch->indexOfExistDepotPets[petindex] != -1 ){
                ;
            }else{
                Char   petone;
                ret = CHAR_makePetFromStringToArg( secondToken , &petone, 0 );
                if( ret == TRUE ){
                    int existpetindex;
                    existpetindex = PET_initCharOneArray( &petone );
                    ch->indexOfExistDepotPets[petindex] = existpetindex;
					CHAR_setWorkInt( existpetindex, CHAR_WORKPLAYERINDEX, charaindex);
					CHAR_complianceParameter( existpetindex);
                }
            }
            goto NEXT;
        }
        print( "??? : %s[%s]\n" , linebuf, firstToken );
NEXT:
        readindex++;
    }
    return TRUE;
}

void CHAR_ShowMyDepotPets( int charaindex)
{
	int i, petindex;

	print( "\nShowMyDepotPets:\n");
	if( !CHAR_CheckDepotPet( charaindex) ) return;
	for( i=0; i<CHAR_MAXDEPOTPETHAVE; i++){
		petindex = CHAR_getDepotPetIndex( charaindex, i);
		if( !CHAR_CHECKINDEX( petindex) ) continue;
		print( "DEPOTPET%d: %d-%s\n", i, petindex, CHAR_getChar( petindex, CHAR_NAME) );
	}
}

int CHAR_getfindEmptyDepotPet( int charaindex)
{
	int i, petindex, cnt=0;
	if( !CHAR_CheckDepotPet( charaindex) ) return cnt;
	for( i=0; i<CHAR_MAXDEPOTPETHAVE; i++){
		petindex = CHAR_getDepotPetIndex( charaindex, i);
		if( !CHAR_CHECKINDEX( petindex) )cnt++;
	}
	return cnt;
}

int CHAR_findEmptyDepotPet( int charaindex)
{
	int i, petindex;
	if( !CHAR_CheckDepotPet( charaindex) ) return -1;
	for( i=0; i<CHAR_MAXDEPOTPETHAVE; i++){
		petindex = CHAR_getDepotPetIndex( charaindex, i);
		if( !CHAR_CHECKINDEX( petindex) ) return i;
	}
	return -1;
}

BOOL CHAR_SaveDepotPet( int charaindex)
{
	char *databuf=NULL;
	char *CdKey=NULL;
	int fd;


	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;
	fd = getfdFromCharaIndex( charaindex);
	if( !CHAR_CheckDepotPet( charaindex) ) return FALSE; //仓库未存在
	if( (CdKey=CHAR_getChar( charaindex, CHAR_CDKEY)) == NULL ) return FALSE;
	if( (databuf = CHAR_makeDepotPetFromCharIndex( charaindex)) == NULL ){
		CHAR_removeDepotPet( charaindex);
		return FALSE;
	}
	saacproto_ACCharSavePoolPet_send( acfd, charaindex, CONNECT_getFdid(fd), CdKey, databuf);
	CHAR_removeDepotPet( charaindex);

	return TRUE;
}

BOOL CHAR_GetDepotPet( int meindex, int charaindex)
{
	char *CdKey=NULL;
	int fd;

	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;
	fd = getfdFromCharaIndex( charaindex);
	if( CHAR_CheckDepotPet( charaindex) ) return FALSE;//仓库已存在
	if( (CdKey=CHAR_getChar( charaindex, CHAR_CDKEY)) == NULL ) return FALSE;

	//向AC要仓库资料
	saacproto_ACCharGetPoolPet_send( acfd, meindex, charaindex, CONNECT_getFdid(fd), CdKey);
	return TRUE;
}

BOOL CHAR_CheckDepotPet( int charaindex)
{
	Char*   ch = NULL;
	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;
	if( (ch = CHAR_getCharPointer( charaindex)) == NULL ) return FALSE;
	if( ch->indexOfExistDepotPets == NULL ) return FALSE;
	return TRUE;
}

#endif



#ifdef _NPC_SEPARATEDATA
void CHAR_showTempInt( int index)
{
		int j;
		CHAR_chara[index].tempint = allocateMemory( sizeof( int ) * 10 );
		//andy_log
		print("\n CHAR_chara[%d].tempint: %x\n", index, CHAR_chara[index].tempint );
		for( j=0; j<10; j++){
			print("%d,", CHAR_chara[index].tempint[j] );
		}
		//andy_log
		print("]\n");
}

BOOL CHAR_InitCharTempNum( Char* ch )
{
	int j;
	ch->tempint = allocateMemory( sizeof( int ) * 10 );
	print("\n init temp:%x,", ch->tempint );
	for( j=0; j<10; j++){
		ch->tempint[j] = 0;
		print("%d,", ch->tempint[j] );
	}
	return TRUE;
}

void CHAR_EndCharTempNum( void )
{
	int i;

    for( i = 0 ; i < CHAR_charanum ; i ++ ){
		freeMemory( CHAR_chara[i].tempint );
	}

}

void CHAR_getDefaultCharTempNum( Char* ch )
{
	int j;
	if( ch->tempint == NULL ) return;
	for( j=0; j<10; j++){
		ch->tempint[j] = -1;
		print("%d,", ch->tempint[j] );
	}
}
#endif

#ifdef _STREET_VENDOR
BOOL CHAR_setStreetVendor(int charindex,int index,int set,int num)
{
	if(!CHAR_CHECKINDEX(charindex)) return FALSE;
	if(index < 0 || index > MAX_SELL_ITEM) return FALSE;
	switch(set){
		case SV_PILE:
			CHAR_chara[charindex].StreetVendor[index].pile = num;
			break;
		case SV_PRICE:
			CHAR_chara[charindex].StreetVendor[index].price = num;
			break;
		case SV_INDEX:
			CHAR_chara[charindex].StreetVendor[index].index = num;
			break;
		case SV_KIND:
			CHAR_chara[charindex].StreetVendor[index].kind = num;
			break;
	}
	CHAR_chara[charindex].StreetVendor[index].usage = TRUE;
	return TRUE;
}

INLINE void CHAR_clearStreetVendor(int charindex,int index)
{
	if(!CHAR_CHECKINDEX(charindex)) return;
	if(index < 0 || index > MAX_SELL_ITEM) return;
	CHAR_chara[charindex].StreetVendor[index].index = -1;
	CHAR_chara[charindex].StreetVendor[index].price = 0;
	CHAR_chara[charindex].StreetVendor[index].pile = 0;
	CHAR_chara[charindex].StreetVendor[index].kind = -1;
	CHAR_chara[charindex].StreetVendor[index].usage = FALSE;
}

int CHAR_getStreetVendor(int charindex,int index,int set)
{
	if(!CHAR_CHECKINDEX(charindex)) return -1;
	if(index < 0 || index > MAX_SELL_ITEM) return -1;
	switch(set){
	case SV_PILE:
		return CHAR_chara[charindex].StreetVendor[index].pile;
	case SV_PRICE:
		return CHAR_chara[charindex].StreetVendor[index].price;
	case SV_INDEX:
		return CHAR_chara[charindex].StreetVendor[index].index;
	case SV_KIND:
		return CHAR_chara[charindex].StreetVendor[index].kind;
	case SV_USAGE:
		return CHAR_chara[charindex].StreetVendor[index].usage;
	}
	return -1;
}
#endif

#ifdef _CHAR_POOLPET
int CHAR_getCharDepotPetElement( int charaindex )
{
    int i;
	int limit;
    if( !CHAR_CHECKINDEX(charaindex))return -1;

	//andy_reEdit 2003/09/18
	//limit = (CHAR_getInt(charaindex,CHAR_TRANSMIGRATION)*2)+5;
	//limit = min( limit, CHAR_MAXPOOLPETHAVE);
	limit = CHAR_MAXDEPOTPETHAVE;
	for( i = 0; i < limit; i ++ ) {
		if( CHAR_getDepotPetIndex( charaindex,i) == -1 ) break;
    }
    return ( i == limit ? -1:i );
}
#endif
