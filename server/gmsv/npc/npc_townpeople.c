#include "version.h"
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "util.h"
#include "handletime.h"
#include "anim_tbl.h"
#include "npc_door.h"
#include "lssproto_serv.h"
#include "npcutil.h"


/*
 *  �ޤ��ΤҤȤӤ�  by nakamura
 *  ������̾��TownPeople
 *  �ä�������줿�Ȥ���NPCARGUMENT��ʸ������ä������Ƥ����ͤ��ä���
 *
 *  �ޤ���npcarg�� ����ˤ���,�����ϤƤ󤭤������Ǥ���
 *  �Τ褦��Ⱦ�ѥ���ޤ�ǥ�ߥ��Ȥ���ʣ���Υ᡼��������
 *  �񤯤��Ȥ��Ǥ������ξ�������Ǥ����椫�餷��٤롣
 *  npcgen.perl �Ǥϡ� MAN�Ǥ��롣 MSG�ȤۤȤ�ɤ��ʤ����ʤ�
 *
 */

/*
 * �ä�������줿�Ȥ���NPCARGUMENT�򤽤Τޤ��ä���
 */
void NPC_TownPeopleTalked( int index, int talker, char *msg, int color )
{
	char arg[NPC_UTIL_GETARGSTR_BUFSIZE], token[NPC_UTIL_GETARGSTR_LINEMAX];
    int i, tokennum;

    /* 3����åɰ���ξ������������� */

	if( CHAR_getInt(talker,CHAR_WHICHTYPE) == CHAR_TYPEPLAYER 
        && NPC_Util_charIsInFrontOfChar( talker, index, 3 ) ){

        NPC_Util_GetArgStr( index, arg, sizeof( arg));

        tokennum = 1;
        /* ����ޤǶ��ڤ�줿�ȡ����󤬲������뤫������ */
        for( i=0;arg[i]!='\0';i++ ){
            if( arg[i] == ',' ) tokennum++;
        }

        /* ������Ǥɤ�����뤫���ơ����Υȡ����������� */
        getStringFromIndexWithDelim( arg,",",
                                     rand()%tokennum+1,token, sizeof(token));

        CHAR_talkToCli( talker, index, token, CHAR_COLORWHITE );
    }
}

/*
 * ��������롣
 */
BOOL NPC_TownPeopleInit( int meindex )
{

    //CHAR_setInt( meindex , CHAR_HP , 0 );
    //CHAR_setInt( meindex , CHAR_MP , 0 );
    //CHAR_setInt( meindex , CHAR_MAXMP , 0 );
    //CHAR_setInt( meindex , CHAR_STR , 0 );
    //CHAR_setInt( meindex , CHAR_TOUGH, 0 );
    //CHAR_setInt( meindex , CHAR_LV , 0 );

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPETOWNPEOPLE );
    //CHAR_setFlg( meindex , CHAR_ISOVERED , 1 );
    //CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );  /* ���⤵��ʤ���� */
    
    return TRUE;
}
