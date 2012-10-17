#include "version.h"
#include <string.h>

#include "char.h"
#include "char_event.h"
#include "object.h"
#include "handletime.h"
#include "magic.h"
#include "npcutil.h"
#include "char_data.h"
#include "chatmagic.h"
#include "net.h"
#include "configfile.h"
#include "npcgen.h"

/*
---------------------------------------------------------------------
sysinfo By Kawata
---------------------------------------------------------------------
*/

#define		NPC_SYSINFO_MSGMAX				21

#define		NPC_SYSINFO_PASSWD_DEFAULT		"G00d1uck"

#define		NPC_SYSINFO_TIMELIMIT_DEFAULT		120

#define		NPC_SYSINFO_SHUTDOWNLIMIT_DEFAULT	5

#define		NPC_SYSINFO_SHUTDOWN_MSG		"�A�L %d ������A�Y�}�l�i��server�t�κ��@�C"

#define		NPC_SYSINFO_SHUTDOWN_MSG_COMP	"server�w�����C"
enum {
	CHAR_WORK_MODE        		= CHAR_NPCWORKINT1,
	CHAR_WORK_TIME	    		= CHAR_NPCWORKINT2,
	CHAR_WORK_TIMELIMIT    		= CHAR_NPCWORKINT3,
	CHAR_WORK_SHUTDOWNTIME		= CHAR_NPCWORKINT4,
	CHAR_WORK_SHUTDOWNLIMIT		= CHAR_NPCWORKINT5,
	CHAR_WORK_SHUTDOWNDSPTIME	= CHAR_NPCWORKINT6,
	CHAR_WORK_MSGCOLOR			= CHAR_NPCWORKINT7,
	CHAR_WORK_PLAYER			= CHAR_NPCWORKINT8,
};

enum {
	CHAR_WORKCHAR_PASSWD = CHAR_NPCWORKCHAR1,
};
typedef struct {
	char	msg[16];
}CMD_MSG;
CMD_MSG	cmd_msg[] = {
                        { "startinfo"}, {"endinfo"},	{"help"},
						{"player"},		{"list"},		{"sendno"},
						{"sendmsg"},	{"shutdown"},	{"msgcol"},
						{"sysinfo"},	{"info"},
                        { "allowaccept" } , { "denyaccept" } ,
                        { "closeallsockets" },
                        { "allnosee" },
                        { "allnobody" },
                        { "movenum" },
                        { "bornnum" },
                        { "enemystate" },
                        { "setwalktime"},{ "setcatime" },{"setcdtime"},
                        { "setoneloop"},
                        { "titlerestart"},
                        { "setsaacwrite"}, { "setsaacread"},
};
enum{
	NPC_SYSINFO_MSG_STARTINFO,
	NPC_SYSINFO_MSG_ENDINFO,
	NPC_SYSINFO_MSG_HELP,
	NPC_SYSINFO_MSG_PLAYER,
	NPC_SYSINFO_MSG_LIST,
	NPC_SYSINFO_MSG_SENDNO,
	NPC_SYSINFO_MSG_SENDMSG,
	NPC_SYSINFO_MSG_SHUTDOWN,
	NPC_SYSINFO_MSG_MSGCOL,
	NPC_SYSINFO_MSG_SYSINFO,
	NPC_SYSINFO_MSG_INFO,
	NPC_SYSINFO_MSG_ALLOWACCEPT,
	NPC_SYSINFO_MSG_DENYACCEPT,
	NPC_SYSINFO_MSG_CLOSEALLSOCKETS,

	/* �����ɲ� */
	NPC_SYSINFO_ALL_NOSEE,
	NPC_SYSINFO_ALL_NOBODY,
	NPC_SYSINFO_MOVE_NUM,
	NPC_SYSINFO_BORN_NUM,
	NPC_SYSINFO_ENEMY_STATE,
	NPC_SYSINFO_SETWALKTIME,
	NPC_SYSINFO_SETCATIME,
	NPC_SYSINFO_SETCDTIME,
	NPC_SYSINFO_SETONELOOP,
	NPC_SYSINFO_TITLERESTART,
	NPC_SYSINFO_SETSAACWRITE,
	NPC_SYSINFO_SETSAACREAD,

	NPC_SYSINFO_MSG_NUM,
};

typedef struct {
	char	arg[32];
	char	defMsg[128];
}NPC_Sysinfo_Msg;

static NPC_Sysinfo_Msg		msgtable[] = {
	{ "def_msg",			"�ھ֦��M���@�ɪ��H���ܪ���O�C" },
	{ "shutdown_msg",		"�{�b�}�l�n�i��server�t�κ��@�о��tlogout�I�I"},
};
enum{
	NPC_SYSINFO_ARG_DEF_MSG,
	NPC_SYSINFO_ARG_SHUTDOWN_MSG,
};


static void NPC_Sysinfo_SendMsg( int meindex, int pindex, int tblnum);
static char *NPC_Sysinfo_GetMsg( int meindex, char *msgindexstr,
								char *out,int outlen, int num );
static void NPC_Sysinfo_SendMsgToAll( int meindex, char *msg);
static void NPC_Sysinfo_Msg_EndInfo( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Msg_Help( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Msg_Player( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Msg_List( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Msg_SendNo( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Msg_SendMsg( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Msg_Shutdown( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Msg_Msgcol( int meindex, int tindex, char *msg);

/* �����ɲ� */
static void NPC_Sysinfo_All_NoBody( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_All_NoSee( int meindex, int tindex, char *msg);

static void NPC_Sysinfo_Move_Num( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Born_Num( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Enemy_State( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_SetWalkTime( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_SetCATime( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_SetCDTime( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_SetOneloop( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_TitleRestart( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_SetSaacwrite( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_SetSaacread( int meindex, int tindex, char *msg);

static void NPC_Sysinfo_Msg_allowAccept(int meindex, int tindex, char*msg)
{
    SERVSTATE_SetAcceptMore( 1 );
	CHAR_talkToCli( tindex, meindex, "allowaccept OK.", CHAR_COLORYELLOW);
}
static void NPC_Sysinfo_Msg_denyAccept(int meindex, int tindex, char*msg)
{
    SERVSTATE_SetAcceptMore( 0 );
	CHAR_talkToCli( tindex, meindex, "denyaccept OK.", CHAR_COLORYELLOW);
}
static void NPC_Sysinfo_Msg_closeallsockets(int meindex, int tindex,
                                            char*msg)
{
	CHAR_talkToCli( tindex, meindex, "closeallsockets OK.", CHAR_COLORYELLOW);
    closeAllConnectionandSaveData();
}
static void NPC_Sysinfo_Msg_Sysinfo(int meindex, int tindex, char*msg)
{
	CHAR_CHAT_DEBUG_sysinfo( tindex, msg);
}
static void NPC_Sysinfo_Msg_Info(int meindex, int tindex, char*msg)
{
	CHAR_CHAT_DEBUG_info( tindex, msg);
}


typedef void	(*FUNC)(int,int,char *);
static FUNC		functbl[] = {
	NULL,
	NPC_Sysinfo_Msg_EndInfo,
	NPC_Sysinfo_Msg_Help,
	NPC_Sysinfo_Msg_Player,
	NPC_Sysinfo_Msg_List,
	NPC_Sysinfo_Msg_SendNo,
	NPC_Sysinfo_Msg_SendMsg,
	NPC_Sysinfo_Msg_Shutdown,
	NPC_Sysinfo_Msg_Msgcol,
	NPC_Sysinfo_Msg_Sysinfo,
	NPC_Sysinfo_Msg_Info,
    NPC_Sysinfo_Msg_allowAccept,
    NPC_Sysinfo_Msg_denyAccept,
    NPC_Sysinfo_Msg_closeallsockets,
	/* �����ɲ� */
    NPC_Sysinfo_All_NoSee,
    NPC_Sysinfo_All_NoBody,
    NPC_Sysinfo_Move_Num,
    NPC_Sysinfo_Born_Num,
    NPC_Sysinfo_Enemy_State,
    NPC_Sysinfo_SetWalkTime,
    NPC_Sysinfo_SetCATime,
    NPC_Sysinfo_SetCDTime,
    NPC_Sysinfo_SetOneloop,
    NPC_Sysinfo_TitleRestart,
    NPC_Sysinfo_SetSaacwrite,
    NPC_Sysinfo_SetSaacread,
    
};

/*********************************
* �������
*********************************/
BOOL NPC_SysinfoInit( int meindex )
{
	int tmp;
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
    char	buff[128];

    NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	/* ��������� */

	CHAR_setWorkInt( meindex, CHAR_WORK_MODE, 0);

	/* �ѥ���ɥ��å� */
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "passwd", buff, sizeof( buff))
		== NULL)
	{
		strcpy( buff, NPC_SYSINFO_PASSWD_DEFAULT);
	}
	CHAR_setWorkChar( meindex, CHAR_WORKCHAR_PASSWD, buff);


	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "time");
	if( tmp == -1 ) tmp = NPC_SYSINFO_TIMELIMIT_DEFAULT;
	CHAR_setWorkInt( meindex, CHAR_WORK_TIMELIMIT, tmp);

    /* ��å������ο����� */
	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "msg_col");
	if( tmp < CHAR_COLORRED || tmp > CHAR_COLORWHITE )
		tmp = CHAR_COLORYELLOW;
	CHAR_setWorkInt( meindex, CHAR_WORK_MSGCOLOR, tmp);

	CHAR_setWorkInt( meindex, CHAR_WORK_PLAYER, -1);
	CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNTIME, 0);
	CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNDSPTIME, 0);
    CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TYPEOTHERNPC);
	CHAR_setFlg( meindex, CHAR_ISATTACKED, 0);

	return TRUE;
}

/*********************************
* Loop����
**********************************/
void NPC_SysinfoLoop( int meindex )
{
	int		shuttime;
	if( CHAR_getWorkInt( meindex, CHAR_WORK_MODE) == 1) {
		int		oldtime;
		/* ���֤�ʤ�� */
		oldtime = CHAR_getWorkInt( meindex, CHAR_WORK_TIME);
		/* ���»��֥����С����̾�⡼�ɤ� */
		if( NowTime.tv_sec - oldtime >
			CHAR_getWorkInt( meindex, CHAR_WORK_TIMELIMIT))
		{
			CHAR_setWorkInt( meindex, CHAR_WORK_MODE, 0);
			CHAR_setWorkInt( meindex, CHAR_WORK_PLAYER, -1);
		}
	}
	shuttime = CHAR_getWorkInt( meindex, CHAR_WORK_SHUTDOWNTIME);
	/* shutdown���֥����å� */
	if( shuttime > 0 ) {
		int diff,limit,hun;

		diff = NowTime.tv_sec - shuttime;
		limit = CHAR_getWorkInt( meindex, CHAR_WORK_SHUTDOWNLIMIT);
		hun = limit - (diff/60);
		/* ��ʬ�����˥�å����� */
		if( hun != CHAR_getWorkInt( meindex, CHAR_WORK_SHUTDOWNDSPTIME)){
			char	buff[256];
			if( hun != 0 ) {
				snprintf( buff, sizeof(buff), NPC_SYSINFO_SHUTDOWN_MSG, hun);
			}
			else {
				strcpy( buff, NPC_SYSINFO_SHUTDOWN_MSG_COMP);
			}
			NPC_Sysinfo_SendMsgToAll( meindex, buff);
			CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNDSPTIME, hun);
		}
		if( hun == 0) {
			CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNDSPTIME,0);
			CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNLIMIT,0);
			CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNTIME,0);
		}
	}
}
/*********************************
* �ä�����줿���ν���
**********************************/
void NPC_SysinfoTalked( int meindex, int tindex, char *msg, int color)
{
	char	*msgwk;
	char	*buff;
	int		len;
	int		msgno;

	len = strlen( msg)+1;
	msgwk = calloc( 1, sizeof(char)*len);
	buff = calloc( 1, sizeof(char)*len);
	strcpy( msgwk, msg);
	/* ���ڡ�����ޤȤ�� */
	deleteSequentChar( msgwk, " ");
	if( CHAR_getWorkInt( meindex, CHAR_WORK_MODE) == 0 ) {
		msgno = 0;
		if( strncmp( msgwk, cmd_msg[NPC_SYSINFO_MSG_STARTINFO].msg,
					strlen( cmd_msg[NPC_SYSINFO_MSG_STARTINFO].msg)) == 0)
		{
			if( getStringFromIndexWithDelim( msgwk, " ", 2, buff, len)
				== TRUE )
			{
				if( strcmp( buff, CHAR_getWorkChar( meindex, CHAR_WORKCHAR_PASSWD))
					== 0 )
				{
					CHAR_setWorkInt( meindex, CHAR_WORK_MODE, 1);
					CHAR_setWorkInt( meindex, CHAR_WORK_PLAYER, tindex);
					CHAR_setWorkInt( meindex, CHAR_WORK_TIME, NowTime.tv_sec);
					CHAR_talkToCli( tindex, meindex,
								"�w������O�Ҧ��C���Uhelp�Y�|�X�{�����ɡC",
								CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
					CHAR_talkToCli( tindex, meindex,
								"�i�H�benemystate ���F�ѼĤH�����ͩM��ʼơC",
								CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));

				}
			}
		}
		if( CHAR_getWorkInt( meindex, CHAR_WORK_MODE ) == 0 ) {
			NPC_Sysinfo_SendMsg( meindex, tindex, NPC_SYSINFO_ARG_DEF_MSG);
		}
	}
	else {
		/* ���ޥ�ɥ⡼�ɤˤ����ͤ��������Ǥ��ʤ� */
		if( tindex == CHAR_getWorkInt( meindex, CHAR_WORK_PLAYER)) {
			int		i;
			/* ���줾���������ʬ�� */
			for( i = NPC_SYSINFO_MSG_ENDINFO; i < NPC_SYSINFO_MSG_NUM; i ++ ) {
				if( strncmp( msgwk, cmd_msg[i].msg, strlen( cmd_msg[i].msg)) == 0) {
					if( functbl[i] != NULL ) {
						functbl[i]( meindex, tindex, msgwk);
					}
					break;
				}
			}
			CHAR_setWorkInt( meindex, CHAR_WORK_TIME, NowTime.tv_sec);
		}
		else {
			NPC_Sysinfo_SendMsg( meindex, tindex, NPC_SYSINFO_ARG_DEF_MSG);
		}
	}
	free( msgwk);
	free( buff);
}
/*------------------------------------------------------------------------
 * �̾�⡼�ɤ�
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_EndInfo( int meindex, int tindex, char *msg)
{
	CHAR_setWorkInt( meindex, CHAR_WORK_MODE, 0);
	CHAR_setWorkInt( meindex, CHAR_WORK_PLAYER, -1);
	CHAR_talkToCli( tindex, meindex,
					"�w����@��Ҧ�",
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));

}
/*------------------------------------------------------------------------
 * HELP��å�����
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_Help( int meindex, int tindex, char *msg)
{
	int		i;
	typedef struct {
		char	str[512];
	}SYSINFO_MSGWK;
	SYSINFO_MSGWK	msgwk[] = {
	{ "���OHELP"},
	{ "help                  �����T���C"},
	{ "player                ���ܲ{�bLOGIN�����a�H�ơC"},
	{ "list                  �T���@���C"},
	{ "send number          �unumber�v �O�ƭȡC���w�T�������X��N�ӰT�����e�ǰe�����������a�C"},
	{ "sendmsg msgs          msgs���Q�ǰe���T���C�bsend ����Ť@��A�Y�i�N���᪺�峹�e�H�����������a�C"},
	{ "shutdown min          min �N�����C"},
	{ "msgcol col            �]�w�ǰe�����a�T�����C��C�C�⪺���w��red,green,yellow,blue,purple,cyan,white�C"},
	{ "sysinfo               �t�Ϊ��A"},
	{ "info                  ���a���A"},
	{ "endinfo               �^�_���q�����A�C"},
	{ "denyaccept            �L�k����CLIENT�ݪ�����C"},
	{ "allowaccept           ����CLIENT�ݪ�����C"},
	{ "closeallsockets       �j����CLIENT��LOGOUT�C�b�������denyaccept�C"},
	{ "titlerestart          �A��Ū���ٸ��W���]�w�C"},
	{ "enemystate            ����NPC�����p�C"},
	{ ""},
	};

	for( i = 0; msgwk[i].str[0] != '\0'; i ++ ) {
		CHAR_talkToCli( tindex, meindex, msgwk[i].str,
						CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
}
/*------------------------------------------------------------------------
 * �ץ쥤�䡼�ο���Ĵ�٤�
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_Player( int meindex, int tindex, char *msg)
{
    int     i, cnt = 0;
    int     playernum=CHAR_getPlayerMaxNum();
    char	buff[64];
    for( i=0 ; i<playernum ; i++ ){
        if( CHAR_getCharUse(i) != FALSE ) {
        	if( CHAR_getInt( i, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
        		cnt ++;
        	}
        }
	}
	snprintf( buff, sizeof( buff), "���a��%d�H�C", cnt);
	CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));

}
/*------------------------------------------------------------------------
 * ��å������ꥹ�Ȥ�ɽ������
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_List( int meindex, int tindex, char *msg)
{
	char	buff[512];
	char	buff2[512];
	int		i;

	for( i = 1; i < NPC_SYSINFO_MSGMAX; i ++ ) {
		if( NPC_Sysinfo_GetMsg( meindex, "sendno", buff, sizeof( buff), i)
			!= NULL )
		{
			snprintf( buff2, sizeof(buff2), "No.%d:%s", i, buff);
			CHAR_talkToCli( tindex, meindex, buff2,
							CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));

		}
	}
}
/*------------------------------------------------------------------------
 * ��å������ֹ椫���å���������������
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_SendNo( int meindex, int tindex, char *msg)
{
	char	buff[32];
	char	msgbuf[512];

	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		int	num;
		num = atoi( buff);
		if( num >=1 && num < NPC_SYSINFO_MSGMAX ) {
			if( NPC_Sysinfo_GetMsg( meindex, "sendno", msgbuf,
				sizeof( msgbuf), num) != NULL )
			{
				NPC_Sysinfo_SendMsgToAll( meindex, msgbuf);
			}
		}
	}
}
/*------------------------------------------------------------------------
 * ���ꤵ�줿��å�����ʸ�������������
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_SendMsg( int meindex, int tindex, char *msg)
{
	char	buff[512];

	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		NPC_Sysinfo_SendMsgToAll( meindex, buff);
	}
}
/*------------------------------------------------------------------------
 * ��å��������顼���ѹ�����
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_Msgcol( int meindex, int tindex, char *msg)
{
	char	buff[64];
	typedef struct {
		int		color;
		char	str[10];
	}SYSINFO_COLSET;
	SYSINFO_COLSET		colset[] = {
		{ 249, "red"},
		{ 250, "green"},
		{ 251, "yellow"},
		{ 252, "blue"},
		{ 253, "purple"},
		{ 254, "cyan"},
		{ 255, "white"},
	};

	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		int		i;
		for( i = 0; i < 7; i ++ ) {
			if( strstr( buff, colset[i].str) != NULL ) {
				CHAR_setWorkInt( meindex, CHAR_WORK_MSGCOLOR, colset[i].color);
				CHAR_talkToCli( tindex, meindex, "�w���ܰT�����C��C",
								CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
				break;
			}
		}
	}
}
/*------------------------------------------------------------------------
 * ���������å��������������
 *----------------------------------------------------------------------*/
static char *NPC_Sysinfo_GetMsg( int meindex, char *msgindexstr,
								char *out,int outlen, int num )
{

	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	*work;
	char	*buff;
	char	*cret = NULL;
	int 	worklen;

	worklen = strlen( msgindexstr)+3;
	work = (char *)calloc( 1, worklen * sizeof( char));
	buff = (char *)calloc( 1, outlen  * sizeof( char));

	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	snprintf( work, worklen, "%s%d", msgindexstr, num);
	if( NPC_Util_GetStrFromStrWithDelim( argstr, work,
									buff, outlen) != NULL )
	{
		strcpy( out, buff);
		cret = out;
	}

	free( work);
	free( buff);
	return( cret);
}
/*------------------------------------------------------------------------
 * shutdown��å���������
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_Shutdown( int meindex, int tindex, char *msg)
{
	char	buff[10];
	int		hun;

	/* �ǽ�Υ�å��������� */
	NPC_Sysinfo_SendMsg( meindex, tindex, NPC_SYSINFO_ARG_SHUTDOWN_MSG);
	/* ���֥��å� */
	CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNTIME, NowTime.tv_sec);
	/* ���»��ּ��� */
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		hun = atoi( buff);
		if( hun <= 0 ) {
			hun = NPC_SYSINFO_SHUTDOWNLIMIT_DEFAULT;
		}
	}
	else {
		hun = NPC_SYSINFO_SHUTDOWNLIMIT_DEFAULT;
	}

	CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNLIMIT, hun);

	CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNDSPTIME,0);
}
/*------------------------------------------------------------------------
 * ��å�����������ʰ��������å������������
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_SendMsg( int meindex, int pindex, int tblnum)
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buff[128];

	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	if( NPC_Util_GetStrFromStrWithDelim( argstr, msgtable[tblnum].arg,
		buff, sizeof( buff)) == NULL )
	{
		strcpy( buff, msgtable[tblnum].defMsg);
	}
	if( buff[0] != '\0' ) {
		CHAR_talkToCli( pindex, meindex, buff,
						CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
}
/*------------------------------------------------------------------------
 * ���Ƥοͤ˥�å����������롣
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_SendMsgToAll( int meindex, char *msg)
{
    int     i;
    int     playernum = CHAR_getPlayerMaxNum();

    for( i = 0 ; i < playernum ; i++) {
        if( CHAR_getCharUse(i) != FALSE ) {
			CHAR_talkToCli( i, meindex, msg,
						CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
        }
	}
}



/* ��Ũ������ NO_SEE �ˤ��뤫�ɤ��� */
static void NPC_Sysinfo_All_NoSee( int meindex, int tindex, char *msg ){
	char	buff[256];

	buff[0] = buff[1] = 0;
	/* ���»��ּ��� */
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		/* �ϣΤˤ����� */
		if( strncmp( buff, "on", strlen( buff ) ) == 0 ){
			all_nosee = 1;
			snprintf( buff, sizeof( buff), "�w�}��all_nosee �C" );
			CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
		}else
		/* �ϣΤˤ����� */
		if( strncmp( buff, "off", strlen( buff ) ) == 0 ){
			all_nosee = 0;
			snprintf( buff, sizeof( buff), "�w����all_nosee �C" );
			CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
		}
	}else
	if( all_nosee == 0 ){
		snprintf( buff, sizeof( buff), "all_nosee�����ۡC" );
		CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}else{
		snprintf( buff, sizeof( buff), "all_nosee�}�ҵۡC" );
			CHAR_talkToCli( tindex, meindex, buff,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
}

/* ��Ũ������ NO_BODY �ˤ��뤫�ɤ��� */
static void NPC_Sysinfo_All_NoBody( int meindex, int tindex, char *msg ){
	char	buff[256];

	buff[0] = buff[1] = 0;
	/* ���»��ּ��� */
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		/* �ϣΤˤ����� */
		if( strncmp( buff, "on", strlen( buff ) ) == 0 ){
			all_nobody = 1;
			snprintf( buff, sizeof( buff), "�w�}��all_nobody �C" );
			CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
		}else
		/* �ϣΤˤ����� */
		if( strncmp( buff, "off", strlen( buff ) ) == 0 ){
			all_nobody = 0;
			snprintf( buff, sizeof( buff), "�w����all_nobody �C" );
			CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
		}
	}else
	if( all_nobody == 0 ){
		snprintf( buff, sizeof( buff), "all_nobody �����ۡC" );
		CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}else{
		snprintf( buff, sizeof( buff), "all_nobody �}�ҵۡC" );
		CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
}


/* ��Ũ������ ���롼�פǲ���ư����뤫�� */
static void NPC_Sysinfo_Move_Num( int meindex, int tindex, char *msg ){
	char	buff[256];
	int		work;
	buff[0] = buff[1] = 0;

	/* ���»��ּ��� */
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		work = atoi( buff );
		if( work <= 0 ) {
			work = 1000; /* Ŭ�� */
		}
		/* Ʊ���ˤ������ư�����ޤ� */
		EnemyMoveNum = work;
		snprintf( buff, sizeof( buff), "���ĤH�P��%d�ʧ@�C",
			EnemyMoveNum );
		CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}else{
		snprintf( buff, sizeof( buff), "�{�b�ĤH�P�ɦb�i��%d�ʧ@�C",
			EnemyMoveNum );
		CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
}



/* Ũ�����򣱥롼�פǲ��κǹ����߽Ф��� */
static void NPC_Sysinfo_Born_Num( int meindex, int tindex, char *msg ){
	char	buff[256];
	int		work;
	buff[0] = buff[1] = 0;

	/* ���»��ּ��� */
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		work = atoi( buff );
		if( work <= 0 ) {
			work = 2; /* Ŭ�� */
		}
		/* Ʊ���ˤ���������߽Ф��ޤ� */
		one_loop_born = work;
		snprintf( buff, sizeof( buff), "�ĤH�P�ɬ��X�̰���%d�C",
			one_loop_born );
		CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}else{
		snprintf( buff, sizeof( buff), "�{�b�ĤH�P�ɬ��X�̰���%d�C",
			one_loop_born );
		CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
}



/* ���ߤΥƥ��������֤򸫤� */
static void NPC_Sysinfo_Enemy_State( int meindex, int tindex, char *msg ){
	char	buff[256];

	if( all_nobody == 0 ){
		snprintf( buff, sizeof( buff), "allnobody �����ۡC" );
		CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}else{
		snprintf( buff, sizeof( buff), "allnobody �}�ҵۡC" );
		CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
	if( all_nosee == 0 ){
		snprintf( buff, sizeof( buff), "allnosee �����ۡC" );
		CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}else{
		snprintf( buff, sizeof( buff), "allnosee �}�ҵۡC" );
		CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
	snprintf( buff, sizeof( buff),
              "bornnum �{�b�ĤH�P�ɬ��X�̰���%d�C",
              one_loop_born );
	CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));

	snprintf( buff, sizeof( buff),
              "movenum �{�b�ĤH�P�ɦb�i��%d�ʧ@�C",
              EnemyMoveNum );
	CHAR_talkToCli( tindex, meindex, buff,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));


}
static void NPC_Sysinfo_SetWalkTime( int meindex, int tindex, char *msg)
{
	int interval;
	char	msgbuf[256];
	char	buff[256];
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE ) {
		interval = atoi( buff);
		if( interval > 0 ) {
			setWalksendinterval( (unsigned)interval);
		}
	}
	snprintf( msgbuf, sizeof(msgbuf),"WalkInterval = %d ����",getWalksendinterval());
	CHAR_talkToCli( tindex, meindex, msgbuf,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	
}
static void NPC_Sysinfo_SetCATime( int meindex, int tindex, char *msg)
{
	int interval;
	char	msgbuf[256];
	char	buff[256];
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE ) {
		interval = atoi( buff);
		if( interval > 0 ) {
			setCAsendinterval_ms( (unsigned)interval);
		}
	}
	snprintf( msgbuf, sizeof(msgbuf),"CAsendInterval = %d ����",
              getCAsendinterval_ms());
	CHAR_talkToCli( tindex, meindex, msgbuf,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
}
static void NPC_Sysinfo_SetCDTime( int meindex, int tindex, char *msg)
{
	int interval;
	char	msgbuf[256];
	char	buff[256];
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE ) {
		interval = atoi( buff);
		if( interval > 0 ) {
			setCDsendinterval_ms( (unsigned)interval);
		}
	}
	snprintf( msgbuf, sizeof(msgbuf),"CDsendInterval = %d ����",
              getCDsendinterval_ms());
	CHAR_talkToCli( tindex, meindex, msgbuf,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
}
static void NPC_Sysinfo_SetOneloop( int meindex, int tindex, char *msg)
{
	int interval;
	char	msgbuf[256];
	char	buff[256];
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE ) {
		interval = atoi( buff);
		if( interval > 0 ) {
			setOnelooptime_ms( (unsigned)interval);
		}
	}
	snprintf( msgbuf, sizeof(msgbuf),"Onelooptime = %d ����",
              getOnelooptime_ms());
	CHAR_talkToCli( tindex, meindex, msgbuf,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
}
static void NPC_Sysinfo_TitleRestart( int meindex, int tindex, char *msg)
{
	int		rc;
	char	msgbuf[256];
	
	rc = TITLE_reinitTitleName();
	if( rc) {
		snprintf( msgbuf, sizeof(msgbuf),"Ū�������C" );
	}
	else {
		snprintf( msgbuf, sizeof(msgbuf),"�o��ERROR�C" );
	}
	CHAR_talkToCli( tindex, meindex, msgbuf,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
}


static void NPC_Sysinfo_SetSaacwrite( int meindex, int tindex, char *msg)
{
	int interval;
	char	msgbuf[256];
	char	buff[256];
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE ) {
		interval = atoi( buff);
		if( interval > 0 ) {
			setSaacwritenum( (unsigned)interval);
		}
	}
	snprintf( msgbuf, sizeof(msgbuf),"Saacwritenum = %d ",getSaacwritenum());
	CHAR_talkToCli( tindex, meindex, msgbuf,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
}
static void NPC_Sysinfo_SetSaacread( int meindex, int tindex, char *msg)
{
	int interval;
	char	msgbuf[256];
	char	buff[256];
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE ) {
		interval = atoi( buff);
		if( interval > 0 ) {
			setSaacreadnum( (unsigned)interval);
		}
	}
	snprintf( msgbuf, sizeof(msgbuf),"Saacreadnum = %d ",getSaacreadnum());
	CHAR_talkToCli( tindex, meindex, msgbuf,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
}