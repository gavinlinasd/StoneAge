#ifndef __LINK_H__
#define __LINK_H__

#include "common.h"

/*
 * 定义列表结构的
 * 仇及伉旦玄反val反    读卞反窒手仄卅中［勾引曰禾奶件正□及戊疋□及心
 * 垫丹［
 */
typedef struct tagNode
{
    struct tagNode* next;           /*戚及用□玉尺及禾奶件正□*/
    char* val;                      /*保存的字符串  */
    int size;                       /*val式飯通宝*/
}Node;

BOOL Nodeappendhead( Node** top  , Node* add );
BOOL Nodeappendtail( Node** top  , Node* add );
BOOL Noderemovehead( Node** top , Node* ret);
BOOL Noderemovetail( Node** top , Node* ret);
#endif
