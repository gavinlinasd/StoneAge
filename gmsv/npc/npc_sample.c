#include "version.h"
#include "char.h"
#include "object.h"
#include "npcutil.h"
#include "readmap.h"
 
/*
 * NPC代码示例
 *
 * 类型名 ： Sample
 * 定义的函数 ： 全种类
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
 * NPC及扔件皿伙伙□民件
 * 引数
 *  index       int     索引
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
     * 检查附近是否有玩家,有的话靠近他
     * 蜇箕｝  赓卞心勾井匀凶支勾分仃［
     * 中卅井匀凶日仿件母丞它巧□弁
     *  域汹汹仁  卞  猾允月
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
                        /*    元甄  支  */
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
