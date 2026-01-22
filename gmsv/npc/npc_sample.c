#include "version.h"
#include "char.h"
#include "object.h"
#include "npcutil.h"
#include "readmap.h"
 
/*
 * �蚍�鏤鳴摯NPC昡↓迶
 *
 * 淏騷鏤  ※ Sample
 * 擭壛埰堎匴倳  ※ 蠈攣  ��
 * NPC_SamplePreWalk
 * NPC_SamplePostWalk
 * NPC_SamplePreOver
 * NPC_SamplePostOver
 * NPC_SampleWatch
 * NPC_SampleLoop
 * NPC_SampleDying
 * NPC_SampleTalked
 * NPC_SamplePreAttacked
 * NPC_SamplePostAttacked
 * NPC_SampleOff
 * NPC_SampleLooked
 * NPC_SampleItemPut
 */
/*------------------------------------------------------------
 * NPC摯�蚍�鏤鳴鳴↓鏍璃
 * 礎倳
 *  index       int     騷璃溢蚗袲筒
 ------------------------------------------------------------*/
void NPC_SampleLoop( int index )
{
    int i,j;
    BOOL    found=FALSE;
    int     fl,x,y;
    BOOL    nearby = FALSE;
    
    fl = CHAR_getInt(index,CHAR_FLOOR);
    x  = CHAR_getInt(index,CHAR_X);
    y  = CHAR_getInt(index,CHAR_Y);

    /*
     * 閟�奀暵鯜謐杻嚏齠皮俶職時�竣凝禱ぅ迋趙��笢倜�桯庠邿敢樈迓蟭�
     * 藯凜��  疐勗陑僑凝埱倜盓僑煦崹��
     * 笢埵凝埱倜�梫翹�譫堜坳б↓袲
     *  郖倵倵��  勗  賓埰堎
     */
    for( i=x-2 ; i<=x+2 ; i ++ ){
        for( j=y-2 ; j<=y+2 ; j++ ){
            OBJECT  object;
            for( object=MAP_getTopObj(fl,i,j) ; object ;
                 object = NEXT_OBJECT(object) ){
                int     objindex = GET_OBJINDEX(object);
                if( OBJECT_getType(objindex) == OBJTYPE_CHARA
                    && CHAR_getInt(OBJECT_getIndex(objindex),
                                   CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ){

                    int     dir;
                    POINT   start,end;
                    end.x = CHAR_getInt(OBJECT_getIndex(objindex),CHAR_X);
                    end.y = CHAR_getInt(OBJECT_getIndex(objindex),CHAR_Y);
                    start.x = x;
                    start.y = y;
                    dir = NPC_Util_getDirFromTwoPoint( &start,&end );
                    if( dir != -1 )
                        CHAR_walk(index,dir,0);
                    else
                        /*    啋淢  盓  */
                        print( "????????same coordinates\n" );
                    found = TRUE;

                    if( ABS( x-OBJECT_getX(objindex) ) <= 1 &&
                        ABS( y-OBJECT_getY(objindex) ) <= 1 ){
                        nearby = TRUE;
                    }
                    
                    break;
                }
            }
        }
    }
    
    if( !found )
        CHAR_walk( index,RAND(0,7),0);

/*    print( "e(%d,%d)[%d]" ,CHAR_getInt(index,CHAR_X),
           CHAR_getInt(index,CHAR_Y),found );*/
    

    if( found && nearby ){
/*        print( "found" );*/
        /*CHAR_Attack( index, CHAR_getInt(index,CHAR_DIR));*/
    }
/*    print("\n");*/

//    print( "%d %d\n",CHAR_getInt( index,CHAR_X),
//           CHAR_getInt( index,CHAR_Y) );
    
}
