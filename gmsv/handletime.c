#define __HANDLETIME_C__ 
#include "version.h"
#include <time.h>
#include <sys/time.h>

#include "common.h"
#include "handletime.h"

#define LSTIME_SECONDS_PER_DAY 5400 /* LSTIME域  少氏及蜇  及  醒 */

/*
  LSTIME_SECONDS_PER_DAY 毛  尹月午｝凛棉及褡心填宁毛  尹月仇午互匹五月［

  袄            LS凛棉匹域  丐凶曰及蜇  及凛棉醒
  9000 (  赏)   2.5 [hour]
  900           0.25[hour] = 15[min]
  90            0.025[hour] = 1.5[min] = 90[sec]
  9             9[sec]

*/
   
#define LSTIME_HOURS_PER_DAY 1024 /* LSTIME域  少氏及LSTIME及凛棉醒 */
#define LSTIME_DAYS_PER_YEAR 100 /* LSTIME域  少氏及LSTIME及  醒 */


// WON REM 
/*
// Nuke 0701: localtime

  struct timeval NowTime;
#ifdef localtime
#undef localtime
#endif

struct tm *localtime(const time_t *timep)
{
	static struct tm lt;
	memset(&lt,0,sizeof(lt));
	lt.tm_sec=*timep %60;
	lt.tm_min=(*timep %3600) / 60;
	lt.tm_hour=(*timep % 86400) / 3600;	
	return &lt;
}
*/

/*------------------------------------------------------------
 * 域伙□皿卞域荚裟壬木化｝凛棉毛褡户月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  岳      TRUE(1)
 *  撩      FALSE(0)
 ------------------------------------------------------------*/
BOOL setNewTime( void )
{
    if( gettimeofday( &NowTime, (struct timezone*)NULL) != 0 ) {
		NowTime.tv_sec = time(0);
		// Nuke 0701: Localtime down
		print("\n time err !! \n"); 
        return FALSE;
	}
    NowTime.tv_sec += DEBUG_ADJUSTTIME;
    return TRUE;
}


/*******************************************************************
漆葭绎卞仄凶［byHiO 1998/12/4 18:37
*******************************************************************/
static long era = (long)912766409 + 5400; 
									/* SA及啃卞内日仄凶 */
									/* LS葭  毛烂聒允月化五午□卅醒袄［
                                    弁仿奶失件玄午  元元扎卅中午分户［*/

/*******************************************************************
	穴扑件凛棉井日LS凛棉卞允月
	long t : time匹请月
	LSTIME *lstime : LSTIME厌瞻  尺及禾奶件正
*******************************************************************/
void RealTimeToLSTime(long t , LSTIME *lstime)
{
	long lsseconds = t - era; /* LS葭  井日及  醒 */
    long lsdays; /* LS葭  井日及  醒 */

    /* 葭  井日及  醒毛1  癫曰及  醒匹喃月午｝  卞卅月 */
	lstime->year = (int)( lsseconds/(LSTIME_SECONDS_PER_DAY*LSTIME_DAYS_PER_YEAR) );

    lsdays = lsseconds/LSTIME_SECONDS_PER_DAY;/* 引内葭  井日及  醒毛煌遥仄化 */
	lstime->day  = lsdays % LSTIME_DAYS_PER_YEAR;/*   癫凶曰及  醒匹喃匀凶丐引曰互  */


    /*(450*12)  匹1  */
    lstime->hour = (int)(lsseconds % LSTIME_SECONDS_PER_DAY )
/* 仇仇引匹匹｝域  互铵引匀化井日窒  凶匀凶井［ */
        * LSTIME_HOURS_PER_DAY / LSTIME_SECONDS_PER_DAY;
    /* 域  丐凶曰及  醒匹喃匀化井日域  丐凶曰及凛棉醒毛井仃月午蜇箕窒凛
     卅及井互歹井月［*/

	return;
}

/*******************************************************************
	LS凛棉井日穴扑件凛棉卞允月
	LSTIME *lstime : LSTIME厌瞻  尺及禾奶件正
	long *t :   凛棉尺及禾奶件正
*******************************************************************/
void LSTimeToRealTime( LSTIME *lstime, long *t)
{
	*t=(long)(
        ( lstime->hour*LSTIME_DAYS_PER_YEAR+lstime->day) /* 凛棉 */
               *LSTIME_HOURS_PER_DAY

        +     lstime->year)
        /*仇及楮醒反壬什匀化中月方丹卞苇尹月［nakamura      */


        *450;
	return;
}

/*******************************************************************
	LS凛棉匹漆及凛棉嗉坌毛  月
	  曰袄 int :   0｝苹1｝镞2｝  3
	LSTIME *lstime : LSTIME厌瞻  尺及禾奶件正
*******************************************************************/
LSTIME_SECTION getLSTime (LSTIME *lstime)
{
	if (NIGHT_TO_MORNING < lstime->hour
        && lstime->hour <= MORNING_TO_NOON)
		return LS_MORNING;
	else if(NOON_TO_EVENING < lstime->hour
            && lstime->hour <= EVENING_TO_NIGHT)
		return LS_EVENING;
	else if(EVENING_TO_NIGHT < lstime->hour
            && lstime->hour <= NIGHT_TO_MORNING)
		return LS_NIGHT;
	else
		return LS_NOON;
}


#ifdef _ASSESS_SYSEFFICACY
static clock_t TotalClock = 0;
static clock_t StartClock = 0;
//static int EndClock = 0;
//static float SysTime=0.0;
static clock_t EndClock = 0;
static double SysTime=0.0;
static int Cnum = 0;

#ifdef _ASSESS_SYSEFFICACY_SUB
static clock_t Net_TotalClock = 0;
static clock_t NPCGEN_TotalClock = 0;
static clock_t Battle_TotalClock = 0;
static clock_t Char_TotalClock = 0;
static clock_t Petmail_TotalClock = 0;
static clock_t Family_TotalClock = 0;
static clock_t SaveCheck_TotalClock = 0;
static clock_t GMBroadCast_TotalClock = 0;
static double Net_SysTime=0.0;
static double NPCGEN_SysTime=0.0;
static double Battle_SysTime=0.0;
static double Char_SysTime=0.0;
static double Petmail_SysTime=0.0;
static double Family_SysTime=0.0;
static double SaveCheck_SysTime=0.0;
static double GMBroadCast_SysTime=0.0;
static clock_t SubStartClock = 0;
#endif

void Assess_InitSysEfficacy()
{
	TotalClock = 0;
	StartClock = 0;
	EndClock = 0;
#ifdef _ASSESS_SYSEFFICACY_SUB
	Net_TotalClock = 0;
	NPCGEN_TotalClock = 0;
	Battle_TotalClock = 0;
	Char_TotalClock = 0;
	Petmail_TotalClock = 0;
	Family_TotalClock = 0;
	SaveCheck_TotalClock = 0;
	GMBroadCast_TotalClock = 0;

	SubStartClock = 0;
#endif
}

void Assess_SysEfficacy( int flg)
{
	if( flg == 0 ){
		StartClock=clock();
	}else if( flg == 1 ){
		EndClock = clock();
		if( EndClock < StartClock ) return;
		TotalClock += (int)(EndClock-StartClock);
		Cnum++;
		if( Cnum%500 == 0 ){
			SysTime = (float)(TotalClock/Cnum)/CLOCKS_PER_SEC;
			TotalClock = 0;
#ifdef _ASSESS_SYSEFFICACY_SUB
			Net_SysTime = (float)(Net_TotalClock/Cnum)/CLOCKS_PER_SEC;
			Net_TotalClock = 0;
			NPCGEN_SysTime = (float)(NPCGEN_TotalClock/Cnum)/CLOCKS_PER_SEC;
			NPCGEN_TotalClock = 0;
			Battle_SysTime = (float)(Battle_TotalClock/Cnum)/CLOCKS_PER_SEC;
			Battle_TotalClock = 0;
			Char_SysTime = (float)(Char_TotalClock/Cnum)/CLOCKS_PER_SEC;
			Char_TotalClock = 0;
			Petmail_SysTime = (float)(Petmail_TotalClock/Cnum)/CLOCKS_PER_SEC;
			Petmail_TotalClock = 0;
			Family_SysTime = (float)(Family_TotalClock/Cnum)/CLOCKS_PER_SEC;
			Family_TotalClock = 0;
			SaveCheck_SysTime = (float)(SaveCheck_TotalClock/Cnum)/CLOCKS_PER_SEC;
			SaveCheck_TotalClock = 0;
			GMBroadCast_SysTime = (float)(GMBroadCast_TotalClock/Cnum)/CLOCKS_PER_SEC;
			GMBroadCast_TotalClock = 0;
#endif
			Cnum = 0;
		}
	}
	/*
	EndClock = clock();
	if( StartClock != 0 ){
		if( EndClock < StartClock ) return;
		TotalClock += (int)(EndClock-StartClock);
		Cnum++;
		if( Cnum%500 == 0 ){
			SysTime = (float)TotalClock/Cnum;
			TotalClock = 0;
		}
	}
	StartClock = EndClock;
	*/
}

void ASSESS_getSysEfficacy( float *TVsec)
{
	*TVsec = SysTime;
}

#ifdef _ASSESS_SYSEFFICACY_SUB
void Assess_SysEfficacy_sub( int flg, int loop)
{

	if( flg == 0 ){
		SubStartClock = clock();
	}else if( flg == 1 ){
		EndClock = clock();
		if( EndClock < SubStartClock ) return;

		switch( loop) {
		case 1: // Net_TotalClock
			Net_TotalClock += (int)(EndClock-SubStartClock);
			break;
		case 2: // NPCGEN_TotalClock
			NPCGEN_TotalClock += (int)(EndClock-SubStartClock);
			break;
		case 3: // Battle_TotalClock
			Battle_TotalClock += (int)(EndClock-SubStartClock);
			break;
		case 4: // Char_TotalClock
			Char_TotalClock += (int)(EndClock-SubStartClock);
			break;
		case 5: // Petmail_TotalClock
			Petmail_TotalClock += (int)(EndClock-SubStartClock);
			break;
		case 6: // Family_TotalClock
			Family_TotalClock += (int)(EndClock-SubStartClock);
			break;
		case 7: // SaveCheck_TotalClock
			SaveCheck_TotalClock += (int)(EndClock-SubStartClock);
			break;
		case 8: // GMBroadCast_TotalClock
			GMBroadCast_TotalClock += (int)(EndClock-SubStartClock);
			break;
		}
	}
}

void ASSESS_getSysEfficacy_sub( float *TVsec, int loop_index)
{
	switch( loop_index) {
	case 1:
		*TVsec = Net_SysTime;
		break;
	case 2:
		*TVsec = NPCGEN_SysTime;
		break;
	case 3:
		*TVsec = Battle_SysTime;
		break;
	case 4:
		*TVsec = Char_SysTime;
		break;
	case 5:
		*TVsec = Petmail_SysTime;
		break;
	case 6:
		*TVsec = Family_SysTime;
		break;
	case 7:
		*TVsec = SaveCheck_SysTime;
		break;
	case 8:
		*TVsec = GMBroadCast_SysTime;
		break;
	}
}

#endif

#endif

#ifdef _CHECK_BATTLETIME

#include "battle.h"
static clock_t battleComClock = 0;
double battleComTotalTime[BATTLE_COM_END];
long battleComTotalUse[BATTLE_COM_END];

void check_battle_com_init( void)
{
	print("\n check_battle_com_init... ");
	print("\n BATTLE_COM_END = %d ", BATTLE_COM_END);
	memset( battleComTotalTime, 0, sizeof(double)*BATTLE_COM_END);
	memset( battleComTotalUse, 0, sizeof(long)*BATTLE_COM_END);
}

void check_battle_com_begin( void)
{
	print(" bi ");
	battleComClock = clock();
}

void check_battle_com_end( int b_com)
{
	clock_t	endClock;
	double	usedClock;

	endClock = clock();
	usedClock = (double)(endClock - battleComClock)/CLOCKS_PER_SEC;

	print(" BC[%d,%0.10f] ", b_com, usedClock);
	battleComTotalTime[b_com] += usedClock;
	battleComTotalUse[b_com] ++;

	print(" bo ");

}

void check_battle_com_show( void)
{
	FILE *outfile;
	int i;
	char outstr[1024];

	outfile = fopen( "battle_com_time.txt", "w");
	if( !outfile)
	{
		print("\n OPEN battle_com_time.txt ERROR!!! \n");
		return;
	}
	
	for( i =0; i <BATTLE_COM_END; i++)
	{
		sprintf( outstr, "%d\t=\t%0.10f\t*\t%d\n",
				i,
				(double)(battleComTotalTime[i]/battleComTotalUse[i]),
				battleComTotalUse[i] );
		fputs( outstr, outfile);
	}
	fclose( outfile);

	print("\n RECORD battle_com_time.txt COMPLETE \n");
}

#endif


