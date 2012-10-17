#ifndef __LINK_H__
#define __LINK_H__

#include "common.h"

/*
 * �ꥹ�ȹ�¤���������ʪ��
 * ���Υꥹ�Ȥ�val������Ū�ˤϲ��⤷�ʤ����Ĥޤ�ݥ��󥿡��Υ��ԡ��Τ�
 * �Ԥ���
 */
typedef struct tagNode
{
    struct tagNode* next;           /*���ΥΡ��ɤؤΥݥ��󥿡�*/
    char* val;                      /*�ݻ�����ʸ����*/
    int size;                       /*val�Υ�����*/
}Node;

BOOL Nodeappendhead( Node** top  , Node* add );
BOOL Nodeappendtail( Node** top  , Node* add );
BOOL Noderemovehead( Node** top , Node* ret);
BOOL Noderemovetail( Node** top , Node* ret);
#endif
