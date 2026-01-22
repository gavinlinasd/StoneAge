#include <stdlib.h>
#include "version.h"
#include "link.h"
#include "buf.h"

/*
 *  惉璃袲摯  勗僑崹堎
 * 礎倳
 *      top         哱蚗鏤蚚↓迶
 *      add         痹崹萃窇堎蚚↓迶摯    禱瞀  埰堎蚚↓迶喜摯睽騷璃淏↓
 * 蒍堇偯
 *      TRUE(1)     埬  
 *      FALSE(0)    謄  
 */
BOOL Nodeappendtail( Node** top  , Node* add )
{
    Node* c;        /*鳴↓鏤暵*/
    Node* next;     /*犍媃�忑觚謨獺灃鼫�*/

    /*哱蚗鏤誑NULL凝汔竣凝摯鏍攝蚗袲*/
    if( *top == NULL ){
        *top = allocateMemory( sizeof( Node  ) );
        if( *top== NULL ) return FALSE;
        (*top)->next    = NULL;            /*襄拻毀  笢*/
        (*top)->size    = add->size;   /*荇踏摯瞀集*/
        (*top)->val     = add->val;         /*睽騷璃淏↓摯昡鼀↓*/
        return TRUE;
    }

    for( c = *top ; c->next ; c = c->next ); /* c摯痾  禱囀�梊� */
    next = allocateMemory( sizeof(Node) );
    if( next == NULL )return FALSE;
    c->next = next;                         /* next勗偯禱优擭埰堎 */
    next->next  = NULL;                      /*襄拻毀  笢*/
    next->val   = add->val;                   /*睽騷璃淏↓摯昡鼀↓*/
    next->size  = add->size;             /*荇踏摯瞀集*/
    return TRUE;
}




/*
 *  惉璃袲摯鞳勗僑崹堎
 * 礎倳
 *      top         哱蚗鏤蚚↓迶
 *      add         痹崹萃窇堎蚚↓迶摯    禱瞀  埰堎蚚↓迶喜摯睽騷璃淏↓
 * 蒍堇偯
 *      TRUE(1)     埬  
 *      FALSE(0)    謄  
 */
BOOL Nodeappendhead( Node** nowtop  , Node* add )
{
    Node* newtop;       /*犍媃笢袸  暵*/

    /*哱蚗鏤誑NULL凝汔竣凝摯鏍攝蚗袲*/
    if( *nowtop == NULL ){
        *nowtop = allocateMemory( sizeof( Node  ) );
        if( *nowtop == NULL ) return FALSE;
        (*nowtop)->next = NULL;             /*襄拻毀  笢*/
        (*nowtop)->size = add->size;        /*荇踏摯瞀集*/
        (*nowtop)->val  = add->val;         /*睽騷璃淏↓摯昡鼀↓*/
        return TRUE;
    }

    /*
     * 犍媃笢蚚↓迶禱郖僑軘堎
     * next 誑蕍埰忒摯禱 nowtop 勗埰堎��
     * nowtop 勗毀 賃瞀媃倜 newtop 禱瓟  埰堎��
     */
    newtop = allocateMemory( sizeof(Node) );
    newtop->next    = *nowtop;
    newtop->val     = add->val;
    newtop->size    = add->size;
    *nowtop = newtop;
    return TRUE;
}




/*
 * 郖    疐摯蚚↓迶禱礎倳勗昡鼀↓媃趙噁堇謫��
 * 礎倳
 *          top         蚚↓迶摯袸  
 *          ret         噁堇謫笢倜蚚↓迶摯昡鼀↓
 * 蒍堇偯
 *      TRUE(1)         埬  
 *      FALSE(0)        謄      蚚↓迶摯袸  誑NULL
 */
BOOL  Noderemovehead( Node** top , Node* ret)
{
    Node* newtop;         /*犍媃�忮�  勗埵堎蚚↓迶*/

    if( *top == NULL )return FALSE;

    ret->val    = (*top)->val;
    ret->size   = (*top)->size;
    newtop = (*top)->next;
    freeMemory( *top );
    *top = newtop;

    return TRUE;
}





/*
 * 郖      摯蚚↓迶禱礎倳勗昡鼀↓媃趙噁堇謫��
 * 礎倳
 *          top         蚚↓迶摯袸  
 *          ret         噁堇謫笢倜蚚↓迶摯昡鼀↓
 * 蒍堇偯
 *      TRUE(1)         埬  
 *      FALSE(0)        謄      蚚↓迶摯袸  誑NULL
 */
BOOL  Noderemovetail( Node** top , Node* ret)
{
    Node* c;             /*鳴↓鏤暵*/
    Node* c1;            /*鳴↓鏤暵 笢僑ぁ忒 c->next禱炩埰*/

    if( *top == NULL )return FALSE;

    c = *top;           /*疐趵偯优擭*/
    c1 = c->next;       /*疐趵偯优擭*/
    while(1){
        if( c1->next == NULL )
            /*
             * 喫摯鄹鬻ぁ          c1
             *                      |
             *  c ---> +------+       
             *         | next |---->+------+
             *         |------|     | next |---->NULL
             *         |      |     +------+
             *         +------+     |      |
             *                      +------+
             *敁埵埱趙笢堎
             */
            break;
        c=c->next;
        c1=c->next;
    }
    c->next = NULL;     /*c1勗堣倜堎忒摯禱朝謫埰堎摯ぁnext毀NULL敁埰堎*/
    /*昡鼀↓*/
    ret->val    = c1->val;
    ret->size   = c1->size;

    freeMemory( c1 );

    return TRUE;
}
