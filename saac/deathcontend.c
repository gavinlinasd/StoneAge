#include "version.h"
#include <time.h>
#include "main.h"
#include "saacproto_util.h"
#include "saacproto_serv.h"
#include "recv.h"
#include "char.h"
#include "util.h"

#include "deathcontend.h"

#ifdef _DEATH_CONTEND

PkTeamLists *PkTeamList=NULL;
static int PKListSort[550];
enum{
	CODE_OK=0,
	CODE_ERR=-1,
	CODE_OUTSTRIP=-2,
	CODE_REPEAT=-3,

	CODE_LEADERERR=-10,
	CODE_ERRBATTLEFULL=-11,
};
static int maxteam = 0;

void del_rn( char *s )
{
	int i;
	for(i=0;;i++){
		if( s[i] == '\r' || s[i] == '\n' ) s[i] = 0;
		if( s[i] == 0 )break;
	}
}

int PKLIST_SetOneBHistory( int ti, int hi, int use, int teamnum, int flg )
{
	if( ti < 0 || ti >= maxteam ) return CODE_OUTSTRIP;
	if( hi < 0 || hi >= MAXBATTLENUM ) return CODE_OUTSTRIP;
	if( PkTeamList[ti].BHistory[hi].use == use ) return CODE_REPEAT;
	
	PkTeamList[ti].BHistory[hi].teamnum = teamnum;
	PkTeamList[ti].BHistory[hi].flg = flg;
	PkTeamList[ti].BHistory[hi].use = use;
	return CODE_OK;
}

int PKLIST_GetOneBHistory( int ti )
{
	int i;
	if( ti < 0 || ti >= maxteam ){
		log( "GetOneBHistory(ti:%d) err return -1 !!\n", ti)
		return -1;
	}
	for( i=0; i<MAXBATTLENUM; i++){
		if( i>=DEFMAXBATTLENUM ) break;
		if( PkTeamList[ti].BHistory[i].use == 0 ){
			log("return BHistory[%d] \n", i) ;
			return i;
		}
	}

	log("return CODE_ERRBATTLEFULL err !! \n") ;
	return CODE_ERRBATTLEFULL;
}
//勿动-------------------------------------------------------
void PKLIST_ResetOneTeamMan( int ti )
{
	int k;
	if( ti < 0 || ti >= maxteam ) return;
	for( k=0;k<MAXTEAMMANNUM;k++){
		PkTeamList[ti].MyTeamMans[k].use = 0;
		memset( PkTeamList[ti].MyTeamMans[k].cdkey, 0,
			sizeof( PkTeamList[ti].MyTeamMans[k].cdkey) );
		memset( PkTeamList[ti].MyTeamMans[k].name, 0,
			sizeof( PkTeamList[ti].MyTeamMans[k].name) );
	}
}

void PKLIST_ResetOneBHistory( int ti )
{
	int k;
	for( k=0;k<MAXBATTLENUM;k++){
		PKLIST_SetOneBHistory( ti, k, 0, 0, 0);
	}
}

void PKLIST_ResetOnePkTeamList( int ti )
{
	if( ti < 0 || ti >= maxteam ) return;
	PkTeamList[ti].use = 0;
	PkTeamList[ti].teamnum = -1;
	memset( PkTeamList[ti].teamname, 0, sizeof( PkTeamList[ti].teamname ) );
	memset( PkTeamList[ti].pathdir, 0, sizeof( PkTeamList[ti].pathdir) );
	memset( PkTeamList[ti].leadercdkey, 0, sizeof( PkTeamList[ti].leadercdkey ) );
}
//-----------------------------------------------------------

int PKLIST_InitPkTeamList( int teamnum )
{
	int i;
	if( PkTeamList != NULL || teamnum <= 0 ) return -1;

	log( "PKLIST_InitPkTeamList( maxteam:%d) \n", teamnum);

	maxteam = teamnum;
	PkTeamList = (PkTeamLists *)calloc( 1, sizeof( struct _tagPkTeamLists) * maxteam );
	if( PkTeamList == NULL ){
		log( "ANDY err Can't calloc PkTeamList :%d !!\n",
			sizeof( struct _tagPkTeamLists) * maxteam );
		return -1;
	}
	for( i=0; i<maxteam; i++ ){
		PkTeamList[i].use = 0;
		PkTeamList[i].teamnum = -1;
		memset(	PkTeamList[i].teamname, 0, sizeof( PkTeamList[i].teamname) );
		memset( PkTeamList[i].pathdir, 0, sizeof( PkTeamList[i].pathdir) );
		memset( PkTeamList[i].leadercdkey, 0, sizeof( PkTeamList[i].leadercdkey) );	

		PkTeamList[i].win = 0;
		PkTeamList[i].lost = 0;
		PkTeamList[i].battleplay = 0;
		PkTeamList[i].score = 0;
		PkTeamList[i].inside = 1;

		PKLIST_ResetOneTeamMan( i);
		PKLIST_ResetOneBHistory( i);
	}
	memset( PKListSort, -1, sizeof(PKListSort));
	log( "ANDY calloc PkTeamList(%d):%d ..!!\n", maxteam,
			sizeof( struct _tagPkTeamLists) * maxteam );

	return 1;
}

int PKLIST_UpdateOnePkTeamData( int ti, int forti, int winerflg)
{
	int ret=0;
	int battleTi = PKLIST_GetOneBHistory( ti );
	if( battleTi == CODE_ERRBATTLEFULL || battleTi == -1 ){
		log( "ti:%d battleTi:%d err !!\n", ti, battleTi);
		return -2;
	}

	if( winerflg == 1 ){
		PkTeamList[ti].win++;
	}else{
		PkTeamList[ti].lost++;
	}
	PkTeamList[ti].battleplay++; //场次
	PkTeamList[ti].score = (PkTeamList[ti].win*3) - (PkTeamList[ti].lost*1);//积分

	PkTeamList[ti].updata = (int)time(NULL);

	if( PkTeamList[ti].score >= DEFWINSCORE ){
		PkTeamList[ti].inside = 2; //录取
	}else if( PkTeamList[ti].lost > DEFMAXBATTLENUM*DEFLOSERATE){// 0.7 = (1-胜率3成)*最高场次
		PkTeamList[ti].inside = 0;
	}else {
		PkTeamList[ti].inside = 1;
	}

	if( PkTeamList[ti].score < 0 ) PkTeamList[ti].score = 0;

	ret = PKLIST_SetOneBHistory( ti, battleTi, 1, forti, winerflg);
	PKLIST_SortTheCharts( ti);
	return ret;
}

int PKLIST_getStringPkTeamData( int ti, char *data, int sizes)
{
	int k;
	char buf[512];
	if( PkTeamList[ti].use != 1 ) return 0;


	sprintf( data, "%d_%d|%s|%s|%s|%d|%d|%d|%d|%d_",
				ti,
				PkTeamList[ti].teamnum,		PkTeamList[ti].pathdir,
				PkTeamList[ti].teamname,	PkTeamList[ti].leadercdkey,
				PkTeamList[ti].win,			PkTeamList[ti].lost,
				PkTeamList[ti].battleplay,	PkTeamList[ti].score,
				PkTeamList[ti].inside	);

	for( k=0; k<MAXTEAMMANNUM; k++){
		if( PkTeamList[ti].MyTeamMans[k].use == 0 ) continue;
		if( k!=0 ) strcat( data, "|");
		sprintf( buf, "%s,%s",
			PkTeamList[ti].MyTeamMans[k].cdkey, PkTeamList[ti].MyTeamMans[k].name);
		strcat( data, buf);
	}
	strcat( data, "_");
	for( k=0; k<MAXBATTLENUM; k++){
		if( PkTeamList[ti].BHistory[k].use == 0 ) continue;
		if( k!=0 ) strcat( data, "|");

		sprintf( buf, "%d,%d" , PkTeamList[ti].BHistory[k].teamnum,
				PkTeamList[ti].BHistory[k].flg );
		strcat( data, buf);
	}

	return strlen( data);
}

void PKLIST_GetPkTeamListData( int fd, int startTi, int count)
{
	int i, first = 1;
	char data[65535], bufarg[10240];
	log( "GetPkTeamListData( %d,%d)!!\n",
		startTi, count);


	PKLIST_alteUserfromFile( "pklist", "alteuser.txt");

	if( startTi < 0 || startTi >= maxteam || count < 1) return;
	memset( data, 0, sizeof( data));

	for( i=startTi; i<maxteam; i++ ){
		memset( bufarg, 0, sizeof(bufarg));
		if( PKLIST_getStringPkTeamData( i, bufarg, sizeof(bufarg)) <= 0 ) continue;
		if( strlen( data) + strlen( bufarg) >= sizeof( data)) {
			saacproto_PKLISTGETDATA_send( fd, "success", i, data );
			return;
		}
		if( first != 1 ) strcat( data, " ");
		first = 0;
		strcat( data, bufarg);
	}

	if( strlen( data) > 0 ){
		saacproto_PKLISTGETDATA_send( fd, "success", -1, data );
	}

	return;
}

int PKLIST_GetPkTeamListArray( int teamnum, char *cdkey)
{
	int i;
	for( i=0; i<maxteam; i++ ){
		if( PkTeamList[i].use != 1 ) continue;
		if( PkTeamList[i].teamnum != teamnum ) continue;
		if( !strcmp( PkTeamList[i].leadercdkey, cdkey) ) return i;
	}
	return -1;
}

void PKLIST_GetPkTeamListDataOne( int fd, int ti)
{
	char data[65535];
	memset( data, 0, sizeof(data));
	if( PKLIST_getStringPkTeamData( ti, data, sizeof(data)) <= 0 ){
		log("* getStringPkTeamData( %d, %d):err\n", fd, ti);
		return;
	}
	saacproto_PKLISTGETDATA_send( fd, "success", -1, data );
}

int PKLIST_UpdateCheckRepeatBHistory( int meti, int menum, int toti, int tonum )
{
	int k;

	if( PkTeamList[meti].use != 1 ) return 0;
	for( k=0; k<MAXBATTLENUM; k++ ){
		if( PkTeamList[meti].BHistory[k].use != 1 ) continue;
		if( PkTeamList[meti].BHistory[k].teamnum == tonum ){
			return -1;
		}
	}
	if( PkTeamList[toti].use != 1 ) return 0;
	for( k=0; k<MAXBATTLENUM; k++ ){
		if( PkTeamList[toti].BHistory[k].use != 1 ) continue;
		if( PkTeamList[toti].BHistory[k].teamnum == menum ){
			return -1;
		}
	}
	return 1;
}

void PKLIST_UpdatePkTeamList( int fd, int menum, char *mecdkey, int tonum, char *tocdkey, int winer, int flg)
{
extern gmsv gs[MAXCONNECTION];
	int ret=0, i, meti, toti;

	if( (meti = PKLIST_GetPkTeamListArray( menum, mecdkey)) == -1 ) return;
	if( (toti = PKLIST_GetPkTeamListArray( tonum, tocdkey)) == -1 ) return;

	//andy_log
	log( "Update() [%d,%d] %s [%d,%d] ", meti, menum, (winer==0)?"胜":"负", toti, tonum);
	if( PKLIST_UpdateCheckRepeatBHistory( meti, menum, toti, tonum ) == -1 ){
		log( "  err Repeat !!\n");
	}else{
		log( "\n");
		if( (ret = PKLIST_UpdateOnePkTeamData( meti, tonum, (winer==0)?1:0 ) ) != CODE_OK ){
			log( "UpdateOnePkTeamData( %d,%d,%d) err %d !!\n", meti, toti, (winer==0)?1:0, ret);
			return;
		}

		if( (ret = PKLIST_UpdateOnePkTeamData( toti, menum, (winer==1)?1:0 ) ) != CODE_OK ){
			log( "*UpdateOnePkTeamData( %d,%d,%d) err %d !!\n", toti, meti, (winer==1)?0:1, ret);
			return;
		}
		for (i = 0 ; i < MAXCONNECTION ; i ++ ) { //送del到game server
			if ( gs[i].use && gs[i].name[0] ) {
				PKLIST_GetPkTeamListDataOne( i, meti);
				PKLIST_GetPkTeamListDataOne( i, toti);
			}
		}
		//备份
		PKLIST_BackupPkTeamListfromFile( "pklist", "pk_list_team.txt" );

	}
}

void PKLIST_CheckRepeatBattleHistory( void)
{
	int i, j, k, find=0;

	log( "\n_CheckRepeatBattleHistory()\n");

	for( i=0; i<maxteam; i++ ){
		if( PkTeamList[i].use != 1 )continue;
		find =0;
		for( j=0; j<MAXBATTLENUM; j++){
			if( PkTeamList[i].BHistory[j].use != 1 ) continue;
			for( k=j+1; k<MAXBATTLENUM; k++ ){
				if( PkTeamList[i].BHistory[k].use != 1 ) continue;
				if( PkTeamList[i].BHistory[j].teamnum == PkTeamList[i].BHistory[k].teamnum ){
					PkTeamList[i].BHistory[k].use = 0;
					PkTeamList[i].BHistory[k].teamnum = -1;
					//andy_log
					log( "ANDY err Repeat Battle array:%d [%d]team:%d\n", i,
						j, PkTeamList[i].BHistory[j].teamnum );
					find = 1;
				}
			}
		}
//		if( find == 0 )continue;

		PkTeamList[i].battleplay = 0;
		PkTeamList[i].win = 0;
		PkTeamList[i].lost = 0;
		for( j=0; j<MAXBATTLENUM; j++){
			if( PkTeamList[i].BHistory[j].use != 1 ) continue;
			if( PkTeamList[i].BHistory[j].flg == 1 ) PkTeamList[i].win++;
			if( PkTeamList[i].BHistory[j].flg == 0 ) PkTeamList[i].lost++;
			PkTeamList[i].battleplay++;
		}
		PkTeamList[i].score = (PkTeamList[i].win*3) - (PkTeamList[i].lost*1);//积分
		if( PkTeamList[i].score >= DEFWINSCORE ){
			PkTeamList[i].inside = 2; //录取
		}else if( PkTeamList[i].lost > DEFMAXBATTLENUM*DEFLOSERATE){// 0.7 = (1-胜率3成)*最高场次
			PkTeamList[i].inside = 0;
		}else {
			PkTeamList[i].inside = 1;
		}
	}
}

//load档
int PKLIST_LoadPkTeamListfromFile( char *dirpath, char *listfilename )
{
	char line[512], filename[256], buf1[64];
	FILE *fp = NULL, *efp=NULL;
	int i, count=0;

	if( PkTeamList == NULL ) return -1;
	sprintf( filename, "%s/%s", dirpath, listfilename);
	log( "LoadPkTeamListfromFile( file:%s) \n", filename);
	if( (fp = fopen( filename, "r" )) == NULL ){
		log( "can't load file:%s !\n", filename);
		return -1;
	}
	sprintf( filename, "%s/%s", dirpath, "err1.txt");
	if( (efp = fopen( filename, "w+" )) == NULL ){
		log( "can't load file:%s !\n", filename);
		return -1;
	}

	while( fgets( line, sizeof( line), fp) ) {
		if( strlen( line) <= 0 || line[0] == '#' ) continue;
		if( count >= maxteam )break;
		del_rn( line);
		log( "#");
		easyGetTokenFromBuf( line, '|', 1, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ){
			fprintf( efp,"TEAMNUM err:%s", line);
			continue;
		}
		PkTeamList[count].teamnum = atoi(buf1);
		if( PkTeamList[count].teamnum < 0 ) continue;
		easyGetTokenFromBuf( line, '|', 2, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ) continue;
		sprintf( PkTeamList[count].pathdir, "%s", buf1);
		easyGetTokenFromBuf( line, '|', 3, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ){
			if( strlen( buf1) <= 0 ){
				fprintf( efp,"TEAMNAME err:%s", line);
				continue;
			}
			continue;
		}
		sprintf( PkTeamList[count].teamname, "%s", buf1);
		easyGetTokenFromBuf( line, '|', 4, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ){
			if( strlen( buf1) <= 0 ){
				fprintf( efp,"MASTER err:%s", line);
				continue;
			}
			continue;
		}
		sprintf( PkTeamList[count].leadercdkey, "%s", buf1);

		easyGetTokenFromBuf( line, '|', 5, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ) continue;
		PkTeamList[count].win = atoi(buf1);
		easyGetTokenFromBuf( line, '|', 6, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ) continue;
		PkTeamList[count].lost = atoi(buf1);
		easyGetTokenFromBuf( line, '|', 7, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ) continue;
		PkTeamList[count].battleplay = atoi(buf1);
		easyGetTokenFromBuf( line, '|', 8, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ) continue;
		PkTeamList[count].score = atoi(buf1);
		easyGetTokenFromBuf( line, '|', 9, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ) continue;
		PkTeamList[count].inside = atoi(buf1);

		easyGetTokenFromBuf( line, '|', 10, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ) continue;
		PkTeamList[count].updata = atoi(buf1);

		{
			FILE *ffp=NULL;
			char teamfilename[256], bufarg[256], buf2[256];
			int readfirst = 1, k, bnum=0;
			sprintf( teamfilename, "%s/%s/team_%d.txt", "pklist",
				PkTeamList[count].pathdir, PkTeamList[count].teamnum );

			if( (ffp=fopen( teamfilename, "r") ) == NULL ) continue;

			while( fgets( bufarg, sizeof( bufarg), ffp) ) {
				if( strlen( bufarg) <= 0 || bufarg[0] == '#' ) continue;
				del_rn( bufarg);
				if( readfirst == 1 ){
					for( k=0; k<MAXTEAMMANNUM; k++){
						easyGetTokenFromBuf( bufarg, '|', k+1, buf1, sizeof( buf1 ));
						if( strlen( buf1) <= 0 ) continue;

						easyGetTokenFromBuf( buf1, ',', 1, buf2, sizeof( buf2 ));
						if( strlen( buf2) <= 0 ){
							if( strlen( buf1) <= 0 ){
								fprintf( efp,"TEAMMAN err:%s", line);
								continue;
							}
							continue;
						}
						sprintf( PkTeamList[count].MyTeamMans[k].cdkey, "%s", buf2);
						easyGetTokenFromBuf( buf1, ',', 2, buf2, sizeof( buf2 ));
						if( strlen( buf2) <= 0 ){
							if( strlen( buf1) <= 0 ){
								fprintf( efp,"TEAMMAN err:%s", line);
								continue;
							}
							continue;
						}
						sprintf( PkTeamList[count].MyTeamMans[k].name, "%s", buf2);
						PkTeamList[count].MyTeamMans[k].use = 1;
					}
					readfirst = 0;
					continue;
				}
				if( bnum >= MAXBATTLENUM )break;//超过战斗场次
				for( k=0; k<10; k++){
					easyGetTokenFromBuf( bufarg, '|', k+1, buf1, sizeof( buf1 ));
					if( strlen( buf1) <= 0 ) continue;
					easyGetTokenFromBuf( buf1, ',', 1, buf2, sizeof( buf2 ));
					if( strlen( buf2) <= 0 ) continue;
					PkTeamList[count].BHistory[bnum].teamnum = atoi( buf2);
					easyGetTokenFromBuf( buf1, ',', 2, buf2, sizeof( buf2 ));
					if( strlen( buf2) <= 0 ) continue;
					PkTeamList[count].BHistory[bnum].flg = atoi( buf2);
					PkTeamList[count].BHistory[bnum].use = 1;
					bnum++;
				}
				
			}
						
			fclose( ffp);
		}

		PkTeamList[count].use = 1;
		count++;
	}
	fclose( efp);
	fclose( fp);
	{
		int k;
		log( "\n");
		for( i=0; i<maxteam; i++ ){
			if( PkTeamList[i].use != 1 ) continue;
			log("PkTeamList[%d] [%d|%d|%s|%s|%s|%d|%d|%d|%d|%d] \n", i,
				PkTeamList[i].use,
				PkTeamList[i].teamnum,
				PkTeamList[i].teamname,
				PkTeamList[i].pathdir,
				PkTeamList[i].leadercdkey,
				PkTeamList[i].win,
				PkTeamList[i].lost,
				PkTeamList[i].battleplay,
				PkTeamList[i].score,
				PkTeamList[i].inside
			);
			log( "\n");
			for( k=0; k<MAXTEAMMANNUM; k++){
				if( PkTeamList[i].MyTeamMans[k].use == 0 ) continue;
				log( "[%s,%s]," , PkTeamList[i].MyTeamMans[k].cdkey,
						PkTeamList[i].MyTeamMans[k].name );
			}
			log( "\n");
			for( k=0; k<MAXBATTLENUM; k++){
				if( PkTeamList[i].BHistory[k].use == 0 ) continue;
				if( k!=0 && k%10 == 0 ) log( "\n");
				log( "%d,%d|" , PkTeamList[i].BHistory[k].teamnum,
						PkTeamList[i].BHistory[k].flg );
			}
			log( "\n");
		}
		log( "\n");
	}

	return 1;
}

//Backup档
int PKLIST_BackupPkTeamListfromFile( char *dirpath, char *listfilename )
{
	char filename[256];
	FILE *fp = NULL;
	int i;
	if( PkTeamList == NULL ) return -1;
	sprintf( filename, "%s/%s", dirpath, listfilename);

	log( "BackupPkTeamListfromFile( file:%s) \n", filename);
	remove( filename);

	if( (fp = fopen( filename, "w+" )) == NULL ){
		log( "can't Backup file:%s !\n", filename);
		return -1;
	}

	for( i=0; i<maxteam; i++ ){
		if( PkTeamList[i].use != 1 ) continue;
		fprintf( fp, "%d|%s|%s|%s|%d|%d|%d|%d|%d|%d\n",
			PkTeamList[i].teamnum, PkTeamList[i].pathdir,
			PkTeamList[i].teamname, PkTeamList[i].leadercdkey,
			PkTeamList[i].win, PkTeamList[i].lost,
			PkTeamList[i].battleplay, PkTeamList[i].score,
			PkTeamList[i].inside, PkTeamList[i].updata );
		{
			FILE *ffp=NULL;
			char teamfilename[256], bufarg[512], buf2[256];
			int k;
			sprintf( teamfilename, "%s/%s/team_%d.txt", "pklist",
				PkTeamList[i].pathdir, PkTeamList[i].teamnum );
			remove( teamfilename);
			if( (ffp=fopen( teamfilename, "w+") ) == NULL ){
				log( "can't Backup file:%s !\n", filename);
				continue;
			}
			memset( bufarg, 0, sizeof( bufarg));
			for( k=0; k<MAXTEAMMANNUM; k++){
				if( PkTeamList[i].MyTeamMans[k].use == 0 ) continue;
//				fprintf( ffp, "%s,%s\n",
//					PkTeamList[i].MyTeamMans[k].cdkey, PkTeamList[i].MyTeamMans[k].name);

				if( k!=0 ) strcat( bufarg, "|");
				sprintf( buf2, "%s,%s",
					PkTeamList[i].MyTeamMans[k].cdkey, PkTeamList[i].MyTeamMans[k].name);
				strcat( bufarg, buf2);

			}
			fprintf( ffp, "%s\n", bufarg);
			memset( bufarg, 0, sizeof( bufarg));
			for( k=0; k<MAXBATTLENUM; k++){
				if( PkTeamList[i].BHistory[k].use == 0 ) continue;
				if( k!=0 ){
					if( k%10 == 0 ){
						fprintf( ffp, "%s\n", bufarg);
						memset( bufarg, 0, sizeof( bufarg));
					}else{
						strcat( bufarg, "|");
					}
				}
				sprintf( buf2, "%d,%d" , PkTeamList[i].BHistory[k].teamnum,
						PkTeamList[i].BHistory[k].flg );
				strcat( bufarg, buf2); // <-- check overflow!!!
			}
			if( strlen(bufarg) > 0 )
				fprintf( ffp, "%s\n", bufarg);
			fclose( ffp);
		}
	}
	fclose( fp);
	return 1;
}
//PKListSort[1000]
void PKLIST_SortTheCharts( int ti) //排行榜处理
{
extern gmsv gs[MAXCONNECTION];
	int i, k, last=-1;

	if( PkTeamList[ti].use != 1 ) return;
	if( PkTeamList[ti].battleplay < 3 ) return;

	for( k=0; k<550; k++){
		if( PKListSort[k] == -1 )continue;
		if( PKListSort[k] == ti ){
			PKListSort[k] = -1;
		}
	}

	for( k=549; k>=0; k-- ){
		if( PKListSort[k] == -1 ){
			last = k;
			continue;
		}
		if(	PkTeamList[ti].score > PkTeamList[PKListSort[k]].score ){
			last = k;
			continue;
		}
		break;
	}
	if( last == -1 ) return;
	for( i=548; i>=last; i-- ){
		PKListSort[i+1] = PKListSort[i];
	}
	log( "\nPKListSort[%d]=%d \n", PKListSort[last], ti);
	PKListSort[last]=ti;

	//更新所有伺服器
	for (i=0; i<MAXCONNECTION; i++) {
		if (gs[i].use && gs[i].name[0]) {
			PKLIST_TheCharts( i, 2, 1);
		}
	}

	{
		char filename[256];
		FILE *fp=NULL;
		sprintf( filename, "%s/%s", "pklist", "Charts.txt" );
		remove( filename);
		if( (fp=fopen( filename, "w+")) == NULL ) return;
		for( i=0; i<550; i++ ){
			if( PKListSort[i] == -1 ) continue;

		fprintf( fp, "%d|%s|%s|%s|%d|%d|%d|%d|%d|%d\n",
			PkTeamList[PKListSort[i]].teamnum, PkTeamList[PKListSort[i]].pathdir,
			PkTeamList[PKListSort[i]].teamname, PkTeamList[PKListSort[i]].leadercdkey,
			PkTeamList[PKListSort[i]].win, PkTeamList[PKListSort[i]].lost,
			PkTeamList[PKListSort[i]].battleplay, PkTeamList[PKListSort[i]].score,
			PkTeamList[PKListSort[i]].inside, PkTeamList[PKListSort[i]].updata );
/*
			fprintf( fp, "%dth 队伍:%-32s 胜:%-4d 负:%-4d 场数:%-4d 积分:%-4d\n",
				i, PkTeamList[PKListSort[i]].teamname,
				PkTeamList[PKListSort[i]].win, PkTeamList[PKListSort[i]].lost,
				PkTeamList[PKListSort[i]].battleplay, PkTeamList[PKListSort[i]].score );
*/
		}
		fclose( fp);
	}
}

void PKLIST_TheCharts( int fd, int type, int flg)
{
	int i, first=1;
	char data[65535*3], buf[512];
extern gmsv gs[MAXCONNECTION];
	//andy_log
	log("PKLIST_TheCharts( %d, %d) \n", type, flg);
	switch( type){
	case 1:
		for( i=0 ; i < MAXCONNECTION ; i ++ ) { //送del到game server
			if ( gs[i].use && gs[i].name[0] ) {
				PKLIST_GetPkTeamListDataOne( i, flg);
			}
		}
		break;
	case 2: //排行榜
		memset( data, 0, sizeof(data));
		for( i=0; i<550; i++){
			if( PKListSort[i] == -1 ) continue;

			sprintf( buf, "%s|%d|%d|%d|%d",
				PkTeamList[PKListSort[i]].teamname,
				PkTeamList[PKListSort[i]].win, PkTeamList[PKListSort[i]].lost,
				PkTeamList[PKListSort[i]].battleplay, PkTeamList[PKListSort[i]].score );

			if( first == 0 ) strcat( data, " ");
			first = 0;
			strcat( data, buf);
		}
		if( strlen(data) > 0 )
			saacproto_PKLISTCHARTS_send( fd, data, type, flg );
		break;

	case 3:
		{
			char filename[256];
			FILE *fp=NULL;
			PKLIST_Sort_PKListSort();
			sprintf( filename, "%s/%s", "pklist", "hero.txt" );
			remove( filename);
			if( (fp=fopen( filename, "w+")) == NULL ) return;
			for( i=0; i<550; i++ ){
				if( PKListSort[i] == -1 ) continue;
				fprintf( fp, "%d|%s|%s|%s|%d|%d|%d|%d|%d|%d\n",
					PkTeamList[PKListSort[i]].teamnum, PkTeamList[PKListSort[i]].pathdir,
					PkTeamList[PKListSort[i]].teamname, PkTeamList[PKListSort[i]].leadercdkey,
					PkTeamList[PKListSort[i]].win, PkTeamList[PKListSort[i]].lost,
					PkTeamList[PKListSort[i]].battleplay, PkTeamList[PKListSort[i]].score,
					PkTeamList[PKListSort[i]].inside, PkTeamList[PKListSort[i]].updata );
				}
			fclose( fp);
		}
		break;


	}
}

//备份存档


//load档
int PKLIST_LoadUserfromFile( char *dirpath, char *listfilename )
{
	char line[512], filename[256], buf1[64];
	FILE *fp = NULL, *tfp=NULL, *bfp=NULL;
	int i, count=0, teamn = 1;

	if( PkTeamList == NULL ) return -1;
	sprintf( filename, "%s/%s", dirpath, listfilename);
	log( "PKLIST_LoadUserfromFile( file:%s) \n", filename);
	if( (fp = fopen( filename, "r" )) == NULL ){
		log( "can't load file:%s !\n", filename);
		return -1;
	}

	sprintf( filename, "%s/%s", dirpath, "d.txt");
	if( (tfp = fopen( filename, "a+" )) == NULL ){
		log( "can't load file:%s !\n", filename);
		return -1;
	}
	sprintf( filename, "%s/%s", dirpath, "badteam.txt");
	if( (bfp = fopen( filename, "a+" )) == NULL ){
		log( "can't load file:%s !\n", filename);
		return -1;
	}
	while( fgets( line, sizeof( line), fp) ) {
		int spoint=2;
		int ManNum=0, leve=1;
		char StartS[256], dData[1024];
		memset( dData, 0, sizeof(dData));
		memset( StartS, 0, sizeof(StartS));
		if( strlen( line) <= 0 || line[0] == '#' ) continue;
		if( count >= maxteam )break;
		del_rn( line);

		//队伍名称 队伍序号
		easyGetTokenFromBuf( line, ',', 1, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ) continue;
		sprintf( PkTeamList[count].teamname, "%s", buf1);

		PkTeamList[count].teamnum = teamn++;

		while( 1){
			easyGetTokenFromBuf( line, ',', spoint++, buf1, sizeof( buf1 ));
			if( strlen( buf1) <= 0 ) break;
			sprintf( StartS, "%s", buf1); //星系

			easyGetTokenFromBuf( line, ',', spoint++, buf1, sizeof( buf1 ));
			if( strlen( buf1) <= 0 ) break;
			sprintf( PkTeamList[count].MyTeamMans[ManNum].cdkey, "%s", buf1); //帐号

			easyGetTokenFromBuf( line, ',', spoint++, buf1, sizeof( buf1 ));
			if( strlen( buf1) <= 0 ) break;
			sprintf( PkTeamList[count].MyTeamMans[ManNum].name, "%s", buf1); //人物名

			easyGetTokenFromBuf( line, ',', spoint++, buf1, sizeof( buf1 ));
			if( strlen( buf1) <= 0 ) break;
			leve = atoi( buf1); //人物等级

			sprintf( dData, "%s,%s,%s,%d\n",
				StartS, PkTeamList[count].MyTeamMans[ManNum].cdkey,
				PkTeamList[count].MyTeamMans[ManNum].name, leve );

//			strcat( dData, buf2);

			fprintf( tfp, "%s", dData);

			PkTeamList[count].MyTeamMans[ManNum].use = 1;
			ManNum++;
			if( ManNum>=MAXTEAMMANNUM )break;
		}
		if( ManNum == 0 ){
			fprintf( bfp, "%s\n", line);
			continue;
		}
		

		sprintf( PkTeamList[count].pathdir, "0x%x", getHash( PkTeamList[count].MyTeamMans[0].cdkey )&0xff);
		sprintf( PkTeamList[count].leadercdkey, "%s", PkTeamList[count].MyTeamMans[0].cdkey );
		PkTeamList[count].win = 0;
		PkTeamList[count].lost = 0;
		PkTeamList[count].battleplay = 0;
		PkTeamList[count].score = 0;
		PkTeamList[count].inside = 1;
		PkTeamList[count].updata = 0;

		PkTeamList[count].use = 1;
		count++;
	}
	fclose( bfp);
	fclose( tfp);
	fclose( fp);
	//清除
	sprintf( filename, "%s/%s", dirpath, listfilename);
	remove( filename);

	{
		int k;
		log( "\n");
		for( i=0; i<maxteam; i++ ){
			if( PkTeamList[i].use != 1 ) continue;
			log("PkTeamList[%d] [%d|%d|%s|%s|%s|%d|%d|%d|%d|%d] \n", i,
				PkTeamList[i].use,
				PkTeamList[i].teamnum,
				PkTeamList[i].teamname,
				PkTeamList[i].pathdir,
				PkTeamList[i].leadercdkey,
				PkTeamList[i].win,
				PkTeamList[i].lost,
				PkTeamList[i].battleplay,
				PkTeamList[i].score,
				PkTeamList[i].inside
			);
			for( k=0; k<MAXTEAMMANNUM; k++){
				if( PkTeamList[i].MyTeamMans[k].use == 0 ) continue;
				log( "[%s,%s]," , PkTeamList[i].MyTeamMans[k].cdkey,
						PkTeamList[i].MyTeamMans[k].name );
			}
			log( "\n");
			for( k=0; k<MAXBATTLENUM; k++){
				if( PkTeamList[i].BHistory[k].use == 0 ) continue;
				if( k!=0 && k%10 == 0 ) log( "\n");
				log( "%d,%d|" , PkTeamList[i].BHistory[k].teamnum,
						PkTeamList[i].BHistory[k].flg );
			}
			log( "\n");
		}
		log( "\n");
	}

	return 1;
}


//load档
int PKLIST_CutStartUserfromFile( char *dirpath, char *listfilename )
{
	char line[512], filename[256], buf1[64];
	FILE *fp = NULL, *bfp=NULL;

	if( PkTeamList == NULL ) return -1;
	sprintf( filename, "%s/%s", dirpath, listfilename);
	log( "CutStartUserfromFile( file:%s) \n", filename);
	if( (fp = fopen( filename, "r" )) == NULL ){
		log( "can't load file:%s !\n", filename);
		return -1;
	}
	sprintf( filename, "%s/%s", dirpath, "baduser.txt");
	if( (bfp = fopen( filename, "a+" )) == NULL ){
		log( "can't load file:%s !\n", filename);
		return -1;
	}

	while( fgets( line, sizeof( line), fp) ) {
		int leve=1;
		char StartS[256], cdkey[256], name[256], dData[1024];
		memset( dData, 0, sizeof(dData));
		memset( StartS, 0, sizeof(StartS));
		if( strlen( line) <= 0 || line[0] == '#' ) continue;
		del_rn( line);
		easyGetTokenFromBuf( line, ',', 1, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ){
			fprintf( bfp, "%s\n", line);
			continue;
		}
		sprintf( StartS, "%s", buf1); //星系

		easyGetTokenFromBuf( line, ',', 2, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ){
			fprintf( bfp, "%s\n", line);
			continue;
		}
		sprintf( cdkey, "%s", buf1); //帐号

		easyGetTokenFromBuf( line, ',', 3, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ){
			fprintf( bfp, "%s\n", line);
			continue;
		}
		sprintf( name, "%s", buf1); //人物名

		easyGetTokenFromBuf( line, ',', 4, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ){
			fprintf( bfp, "%s\n", line);
			continue;
		}
		leve = atoi( buf1);		//人物等级

		{
			FILE *tfp=NULL;
			sprintf( filename, "%s/a_%s.txt", dirpath, StartS);
			if( (tfp = fopen( filename, "a+" )) == NULL ){
				log( "can't load file:%s !\n", filename);
				return -1;
			}
			fprintf( tfp, "%s\n", line);
			fclose( tfp);
		}
	}

	fclose( bfp);
	fclose( fp);
	return 1;
}

//load档
int PKLIST_alteUserfromFile( char *dirpath, char *listfilename )
{
	char line[512], filename[256], buf1[64];
	FILE *fp = NULL, *bfp=NULL;
	int i, j, k;
extern gmsv gs[MAXCONNECTION];
	if( PkTeamList == NULL ) return -1;
	sprintf( filename, "%s/%s", dirpath, listfilename);
	log( "alteUserfromFile( file:%s) \n", filename);

	if( (fp = fopen( filename, "r" )) == NULL ){
		log( "can't load file:%s !\n", filename);
		return -1;
	}

	sprintf( filename, "%s/%s", dirpath, "err.txt");
	if( (bfp = fopen( filename, "a+" )) == NULL ){
		log( "can't load file:%s !\n", filename);
		return -1;
	}
	fprintf( bfp, "====================\n");

	while( fgets( line, sizeof( line), fp) ) {
		char TeamName[256],Star[256],Cdkey[256],UserName[256];
		if( strlen( line) <= 0 || line[0] == '#' ) continue;
		del_rn( line);

		//队伍
		easyGetTokenFromBuf( line, ',', 1, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ) continue;
		sprintf( TeamName, "%s", buf1);

		//星系
		easyGetTokenFromBuf( line, ',', 2, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ) continue;
		sprintf( Star, "%s", buf1);

		//帐号
		easyGetTokenFromBuf( line, ',', 3, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ) continue;
		sprintf( Cdkey, "%s", buf1);

		//人物名
		easyGetTokenFromBuf( line, ',', 4, buf1, sizeof( buf1 ));
		if( strlen( buf1) <= 0 ) continue;
		sprintf( UserName, "%s", buf1);

		for( i=0; i<maxteam; i++ ){
			if( PkTeamList[i].use != 1 ) continue;
			if( strcmp( PkTeamList[i].teamname, TeamName ) )continue;
			log( "reChange fine TeamName:%s\n", PkTeamList[i].teamname );

			for( k=0; k<MAXTEAMMANNUM; k++){
				if( PkTeamList[i].MyTeamMans[k].use == 0 ) continue;
				if( strcmp( PkTeamList[i].MyTeamMans[k].cdkey, Cdkey ) ) continue;

				if( !strcmp( PkTeamList[i].MyTeamMans[k].name, UserName ) ){
					log( "already have user:[%s,%s]\n" , PkTeamList[i].MyTeamMans[k].cdkey,
							PkTeamList[i].MyTeamMans[k].name );
					fprintf( bfp, "already have user:[%s,%s]\n" , PkTeamList[i].MyTeamMans[k].cdkey,
							PkTeamList[i].MyTeamMans[k].name ); 
					continue;
				}else{
					snprintf( PkTeamList[i].MyTeamMans[k].name,
						sizeof(PkTeamList[i].MyTeamMans[k].name),"%s", UserName );
					log( "re [%d,%s] %d.[%s,%s]\n" ,
						PkTeamList[i].teamnum, PkTeamList[i].teamname,
						k, PkTeamList[i].MyTeamMans[k].cdkey,
						PkTeamList[i].MyTeamMans[k].name );
					fprintf( bfp, "re [%d,%s] %d.[%s,%s]\n" ,
						PkTeamList[i].teamnum, PkTeamList[i].teamname,
						k, PkTeamList[i].MyTeamMans[k].cdkey, PkTeamList[i].MyTeamMans[k].name ); 
				}
				break;
			}
			for( j=0 ; j < MAXCONNECTION ; j ++ ) { //送del到game server
				if ( gs[j].use && gs[j].name[0] ) {
					PKLIST_GetPkTeamListDataOne( j, i);
				}
			}
			break;
		}
	}
	fclose( bfp);
	fclose( fp);
	//清除
	sprintf( filename, "%s/%s", dirpath, listfilename);
	remove( filename);
	PKLIST_CheckAllUser();

	return 1;
}

void PKLIST_Sort_PKListSort( void)
{
	int i, j, k, last=-1;
	for( i=0; i<maxteam; i++){
		last = -1;
		for( k=549; k>=0; k-- ){
			if( PkTeamList[i].score <= 0 ) continue;
			if( PKListSort[k] == -1 ){
				last = k;
				continue;
			}
			if(	PkTeamList[i].score > PkTeamList[PKListSort[k]].score ){
				last = k;
				continue;
			}else if( PkTeamList[i].score == PkTeamList[PKListSort[k]].score ){
				if( PkTeamList[i].updata < PkTeamList[PKListSort[k]].updata ){
					last = k;
					continue;
				}else{
					continue;
				}
			}
			break;
		}
		if( last == -1 ) continue;
		for( j=548; j>=last; j-- ){
			PKListSort[j+1] = PKListSort[j];
		}
		PKListSort[last]=i;
	}
}


void PKLIST_CheckAllUser( void)
{
	int i, k;
	FILE *fp=NULL, *ffp=NULL;
	char output[CHARDATASIZE], buf1[256];


	if( (fp=fopen( "checkuser.txt", "w+")) == NULL ) return;
	if( (ffp=fopen( "checkteam.txt", "w+")) == NULL ) return;
	log( "PKLIST_CheckAllUser()\n");

	for( i=0; i<maxteam; i++ ){
		if( PkTeamList[i].use != 1 ) continue;
		for( k=0; k<MAXTEAMMANNUM; k++){
			if( PkTeamList[i].MyTeamMans[k].use == 0 ) continue;
			memset( output, 0, sizeof(output));
			if( loadCharNameAndOption( PkTeamList[i].MyTeamMans[k].cdkey, output, sizeof(output)) == 0){
/*				fprintf( fp,"%d,[%s,%d,%s,%s] err Can't Get CharData!!\n",
					i,
					PkTeamList[i].teamname, PkTeamList[i].teamnum,
					PkTeamList[i].MyTeamMans[k].cdkey, PkTeamList[i].MyTeamMans[k].name );
*/
				fprintf( fp,"%s,%s\n",
					PkTeamList[i].MyTeamMans[k].cdkey, PkTeamList[i].MyTeamMans[k].name );

			}else{
				easyGetTokenFromBuf(output, '|', 1, buf1, sizeof(buf1));
				if( strlen(buf1) <= 0 ) continue;
				
				if( strcmp( PkTeamList[i].MyTeamMans[k].name, buf1) ){
					fprintf( ffp,"re %d,[%s,%d,%s] name:%s->%s\n",
						i, PkTeamList[i].teamname, PkTeamList[i].teamnum,
						PkTeamList[i].MyTeamMans[k].cdkey, PkTeamList[i].MyTeamMans[k].name,
						buf1 );
					snprintf( PkTeamList[i].MyTeamMans[k].name, sizeof(PkTeamList[i].MyTeamMans[k].name),
						"%s", buf1);
				}
			}
		}
	}
	fclose( fp);
	fclose( ffp);
}
/*
序号|队名|帐号_人物名|帐号_人物名|帐号_人物名|帐号_人物名|帐号_人物名|胜|败|0

#序号|目录|队名|帐号_人物名|胜|败|场|积分|0

1|0x10|第1队|cdkey|胜|败|场|积分|0
2|0x10|第2队|cdkey|胜|败|场|积分|0
3|0x10|第3队|cdkey|胜|败|场|积分|0
4|0x10|第4队|cdkey|胜|败|场|积分|0
5|0x10|第5队|cdkey|胜|败|场|积分|0
6|0x10|第6队|cdkey|胜|败|场|积分|0
7|0x10|第7队|cdkey|胜|败|场|积分|0
8|0x10|第8队|cdkey|胜|败|场|积分|0
9|0x10|第9队|cdkey|胜|败|场|积分|0
10|0x10|第10队|cdkey|胜|败|场|积分|0
*/
#endif
