#include "version.h"
#include "char.h"

/*


  ������١�

  ��꡼���ǤǤ�2����Τ��Ȥ��ޡ�

 NPCARGUMENT:  0���ä��餳�Ȥ���0��1���ä��餳�Ȥ���1�θ������  

 ���֤�3�Ĥ��ꡢ��˿ʹԤ��롣

 ����0�� �����ɥ�󥰡����ξ��֤�talk�����ȡ�
 �֤褯�������ۤ��ۤ��衣�����ˤ⡢�路������Τ�����٤��㡣�פ�
 ����1�˰ܹԡ�
 ����1��talk���줿�顢
 �֤��ʤ����Τ���ʤ�С����ȤҤ������ˡ����Ϥʸ����ҤȤġ� �������褦��
        ���θ����Τ��फ? yes/no��yes�Ǿ���2�ء�no��
        �֤Ǥϡ�����Ф��㡣�פǾ���0��
        
 ����2�� talk���줿�顢
 �֤��ʤ�������θ���֤ۤ��ۤ��פ�����������̿�ΤĤ������ڤʤ�ΤȤ���
        �������ڤˤ���٤�������Ф��㡣��
        �Ȥ��äƺ��򸺤餷������ե饰���ɲä��롣
        �Ǿ���0�ˤ�ɤ롣



 
 Talked:

 if( �Ǹ�ˤϤʤ��Ƥ���1ʬ���äƤ��뤫��){
   ����0�ˤ�ɤ롣
 }
   
 switch(����){
 case 0:  ����������1�ء�break;
 case 1:  ����������2�ء�break;
 case 2:  ����������0�ء�break;
 } 

 }

 
 
 

 



 */
BOOL NPC_StoryTellerInit( int meindex )
{
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPESTORYTELLER );
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );
    CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );

    return TRUE;
}


void NPC_StoryTellerTalked( int meindex , int talker , char *msg , int col )
{
    
}
