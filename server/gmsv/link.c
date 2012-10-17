#include <stdlib.h>
#include "version.h"
#include "link.h"
#include "buf.h"

/*
 *  ��󥯤θ�ˤĤ���
 * ����
 *      top         �ȥåץΡ���
 *      add         �դ��ä���Ρ��ɤ����Ƥ��ݻ�����Ρ��ɤؤΥݥ��󥿡�
 * �֤���
 *      TRUE(1)     ����
 *      FALSE(0)    ����
 */
BOOL Nodeappendtail( Node** top  , Node* add )
{
    Node* c;        /*�롼����*/
    Node* next;     /*���������Ρ�����*/

    /*�ȥåפ�NULL���ɤ����Υ����å�*/
    if( *top == NULL ){
        *top = allocateMemory( sizeof( Node  ) );
        if( *top== NULL ) return FALSE;
        (*top)->next    = NULL;            /*³����̵��*/
        (*top)->size    = add->size;   /*Ĺ�����ݴ�*/
        (*top)->val     = add->val;         /*�ݥ��󥿡��Υ��ԡ�*/
        return TRUE;
    }

    for( c = *top ; c->next ; c = c->next ); /* c�ΰ��֤򤺤餹 */
    next = allocateMemory( sizeof(Node) );
    if( next == NULL )return FALSE;
    c->next = next;                         /* next���ͤ����ꤹ�� */
    next->next  = NULL;                      /*³����̵��*/
    next->val   = add->val;                   /*�ݥ��󥿡��Υ��ԡ�*/
    next->size  = add->size;             /*Ĺ�����ݴ�*/
    return TRUE;
}




/*
 *  ��󥯤����ˤĤ���
 * ����
 *      top         �ȥåץΡ���
 *      add         �դ��ä���Ρ��ɤ����Ƥ��ݻ�����Ρ��ɤؤΥݥ��󥿡�
 * �֤���
 *      TRUE(1)     ����
 *      FALSE(0)    ����
 */
BOOL Nodeappendhead( Node** nowtop  , Node* add )
{
    Node* newtop;       /*��������Ƭ��*/

    /*�ȥåפ�NULL���ɤ����Υ����å�*/
    if( *nowtop == NULL ){
        *nowtop = allocateMemory( sizeof( Node  ) );
        if( *nowtop == NULL ) return FALSE;
        (*nowtop)->next = NULL;             /*³����̵��*/
        (*nowtop)->size = add->size;        /*Ĺ�����ݴ�*/
        (*nowtop)->val  = add->val;         /*�ݥ��󥿡��Υ��ԡ�*/
        return TRUE;
    }

    /*
     * �������Ρ��ɤ��ĺ��
     * next ��������Τ� nowtop �ˤ��롣
     * nowtop �ˤ� ���ݤ��� newtop ���������롣
     */
    newtop = allocateMemory( sizeof(Node) );
    newtop->next    = *nowtop;
    newtop->val     = add->val;
    newtop->size    = add->size;
    *nowtop = newtop;
    return TRUE;
}




/*
 * ���ֺǽ�ΥΡ��ɤ�����˥��ԡ����Ƽ�����
 * ����
 *          top         �Ρ��ɤ���Ƭ
 *          ret         ���������Ρ��ɤΥ��ԡ�
 * �֤���
 *      TRUE(1)         ����
 *      FALSE(0)        ����    �Ρ��ɤ���Ƭ��NULL
 */
BOOL  Noderemovehead( Node** top , Node* ret)
{
    Node* newtop;         /*��������Ƭ�ˤʤ�Ρ���*/

    if( *top == NULL )return FALSE;

    ret->val    = (*top)->val;
    ret->size   = (*top)->size;
    newtop = (*top)->next;
    freeMemory( *top );
    *top = newtop;

    return TRUE;
}





/*
 * ���ֺǸ�ΥΡ��ɤ�����˥��ԡ����Ƽ�����
 * ����
 *          top         �Ρ��ɤ���Ƭ
 *          ret         ���������Ρ��ɤΥ��ԡ�
 * �֤���
 *      TRUE(1)         ����
 *      FALSE(0)        ����    �Ρ��ɤ���Ƭ��NULL
 */
BOOL  Noderemovetail( Node** top , Node* ret)
{
    Node* c;             /*�롼����*/
    Node* c1;            /*�롼���� ���ĤǤ� c->next��ؤ�*/

    if( *top == NULL )return FALSE;

    c = *top;           /*���������*/
    c1 = c->next;       /*���������*/
    while(1){
        if( c1->next == NULL )
            /*
             * ���λ�����          c1
             *                      |
             *  c ---> +------+     ��
             *         | next |---->+------+
             *         |------|     | next |---->NULL
             *         |      |     +------+
             *         +------+     |      |
             *                      +------+
             *�ȤʤäƤ���
             */
            break;
        c=c->next;
        c1=c->next;
    }
    c->next = NULL;     /*c1�ˤ������Τ�������Τ�next��NULL�Ȥ���*/
    /*���ԡ�*/
    ret->val    = c1->val;
    ret->size   = c1->size;

    freeMemory( c1 );

    return TRUE;
}
