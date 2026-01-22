#ifndef __LINK_H__
#define __LINK_H__

#include "common.h"

/*
 * 惉筒哱栖桹禱擭壛埰堎  ��
 * 喫摯惉筒哱毀val毀    黍勗毀笰忒媃埵笢�蛫棍�堇睽騷璃淏↓摯昡鼀↓摯陑
 * 菜竣��
 */
typedef struct tagNode
{
    struct tagNode* next;           /*べ摯蚚↓迶喜摯睽騷璃淏↓*/
    char* val;                      /*瞀  埰堎  棬  */
    int size;                       /*val摯�蚅昃�*/
}Node;

BOOL Nodeappendhead( Node** top  , Node* add );
BOOL Nodeappendtail( Node** top  , Node* add );
BOOL Noderemovehead( Node** top , Node* ret);
BOOL Noderemovetail( Node** top , Node* ret);
#endif
