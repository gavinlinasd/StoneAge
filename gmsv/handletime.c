#define __HANDLETIME_C__ 
#include "version.h"
#include <time.h>
#include <sys/time.h>

#include "common.h"
#include "handletime.h"

#define LSTIME_SECONDS_PER_DAY 5400 /* LSTIME郖  屾庌摯藯  摯  倳 */

/*
  LSTIME_SECONDS_PER_DAY 禱  窇堎敁��鄹蹬摯鵌陑沓譴禱  窇堎喫敁誑ぁ拻堎��

  偯            LS鄹蹬ぁ郖  堣倜堇摯藯  摯鄹蹬倳
  9000 (  奼)   2.5 [hour]
  900           0.25[hour] = 15[min]
  90            0.025[hour] = 1.5[min] = 90[sec]
  9             9[sec]

*/
   
#define LSTIME_HOURS_PER_DAY 1024 /* LSTIME郖  屾庌摯LSTIME摯鄹蹬倳 */
#define LSTIME_DAYS_PER_YEAR 100 /* LSTIME郖  屾庌摯LSTIME摯  倳 */


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
 * 郖鳴↓鏤勗郖樊蠙�厊噱砠�鄹蹬禱鵌誧堎��
 * 礎倳
 *  埵媃
 * 蒍堇偯
 *  埬      TRUE(1)
 *  謄      FALSE(0)
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
ぽ楁秠勗媃倜�萒yHiO 1998/12/4 18:37
*******************************************************************/
static long era = (long)912766409 + 5400; 
									/* SA摯諱勗囀�欷ぬ� */
									/* LS楁  禱擭壛埰堎趙拻敁↓埵倳偯��
                                    袲溘騷囮璃哱敁  啋啋崨埵笢敁煦誧��*/

/*******************************************************************
	悃で璃鄹蹬凝�弶S鄹蹬勗埰堎
	long t : timeぁ③堎
	LSTIME *lstime : LSTIME栖桹  喜摯睽騷璃淏
*******************************************************************/
void RealTimeToLSTime(long t , LSTIME *lstime)
{
	long lsseconds = t - era; /* LS楁  凝�梩�  倳 */
    long lsdays; /* LS楁  凝�梩�  倳 */

    /* 楁  凝�梩�  倳禱1  騍堇摯  倳ぁ鄎堎敁��  勗埵堎 */
	lstime->year = (int)( lsseconds/(LSTIME_SECONDS_PER_DAY*LSTIME_DAYS_PER_YEAR) );

    lsdays = lsseconds/LSTIME_SECONDS_PER_DAY;/* 竘囀楁  凝�梩�  倳禱銓猀媃趙 */
	lstime->day  = lsdays % LSTIME_DAYS_PER_YEAR;/*   騍倜堇摯  倳ぁ鄎埱倜堣竘堇誑  */


    /*(450*12)  ぁ1  */
    lstime->hour = (int)(lsseconds % LSTIME_SECONDS_PER_DAY )
/* 喫喫竘ぁぁ��郖  誑鴽竘埱趙凝�欶�  倜埱倜凝�� */
        * LSTIME_HOURS_PER_DAY / LSTIME_SECONDS_PER_DAY;
    /* 郖  堣倜堇摯  倳ぁ鄎埱趙凝�梌�  堣倜堇摯鄹蹬倳禱凝崹堎敁藯凜笰鄹
     埵摯凝誑渦凝堎��*/

	return;
}

/*******************************************************************
	LS鄹蹬凝�桻事佷�鄹蹬勗埰堎
	LSTIME *lstime : LSTIME栖桹  喜摯睽騷璃淏
	long *t :   鄹蹬喜摯睽騷璃淏
*******************************************************************/
void LSTimeToRealTime( LSTIME *lstime, long *t)
{
	*t=(long)(
        ( lstime->hour*LSTIME_DAYS_PER_YEAR+lstime->day) /* 鄹蹬 */
               *LSTIME_HOURS_PER_DAY

        +     lstime->year)
        /*喫摯匴倳毀�圴紳�趙笢堎源竣勗峟窇堎�菝akamura      */


        *450;
	return;
}

/*******************************************************************
	LS鄹蹬ぁぽ摯鄹蹬鉏覕禱  堎
	  堇偯 int :   0��し1��檽2��  3
	LSTIME *lstime : LSTIME栖桹  喜摯睽騷璃淏
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


