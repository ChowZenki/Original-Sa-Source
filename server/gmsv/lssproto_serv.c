#include "version.h"
#include "autil.h"
#include "lssproto_serv.h"
#include "common.h"	// for StoneAge
#include "log.h"	// for StoneAge
#include "char.h"
#include <stdio.h>
#include <time.h>
#include <net.h>


void lssproto_SetServerLogFiles( char *r , char *w )
{
	lssproto_strcpysafe( lssproto_writelogfilename , w , sizeof(lssproto_writelogfilename ));
	lssproto_strcpysafe( lssproto_readlogfilename , r , sizeof(lssproto_readlogfilename ));
}
int lssproto_InitServer(	int (*writefunc)(int,char*,int) ,	int worksiz )
{
	int i;
	if( (void*)writefunc == NULL){lssproto.write_func = lssproto_default_write_wrap;} else {lssproto.write_func = writefunc;}
	lssproto_AllocateCommonWork(worksiz);
	lssproto_stringwrapper = (char**)calloc( 1,sizeof(char*) * MAXLSRPCARGS);
	if(lssproto_stringwrapper ==NULL)return -1;
	memset( lssproto_stringwrapper , 0, sizeof(char*)*MAXLSRPCARGS);
	for(i=0;i<MAXLSRPCARGS;i++){
		lssproto_stringwrapper[i] = (char*)calloc( 1, worksiz );
		if( lssproto_stringwrapper[i] == NULL){
			for(i=0;i<MAXLSRPCARGS;i++){free( lssproto_stringwrapper[i]);return -1;}
		}
	}
	lssproto.ret_work = (char*)calloc( 1,sizeof( worksiz ));
	if( lssproto.ret_work == NULL ){ return -1; }
	return 0;
}
void lssproto_CleanupServer( void )
{
	int i;
	free( lssproto.work );
	free( lssproto.arraywork);
	free( lssproto.escapework );
	free( lssproto.val_str);
	free( lssproto.token_list );
	for(i=0;i<MAXLSRPCARGS;i++){free( lssproto_stringwrapper[i]);}
	free( lssproto_stringwrapper );
	free( lssproto.ret_work );
}

#define DME() print("<DME(%d)%d:%d>",fd,__LINE__,func)

int lssproto_ServerDispatchMessage(int fd, char *encoded)
{
	int func,fieldcount;
	char raw[65500];

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	
	util_DecodeMessage(raw,encoded);

	if( !util_SplitMessage(raw,SEPARATOR) ){
		print("\nDME1:package=%s\n",raw);
		DME(); return -1;
	}
	if (!util_GetFunctionFromSlice(&func, &fieldcount)) {
		logHack(fd,HACK_GETFUNCFAIL);
		DME(); return -1;
	}
	
	if (func==LSSPROTO_W_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		char direction[65500];

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_destring(4, direction);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_W_recv(fd, x, y, direction);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_W2_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		char direction[65500];

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_destring(4, direction);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_W2_recv(fd, x, y, direction);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_EV_RECV) {
		int checksum=0, checksumrecv;
		int event;
		int seqno;
		int x;
		int y;
		int dir;

		checksum += util_deint(2, &event);
		checksum += util_deint(3, &seqno);
		checksum += util_deint(4, &x);
		checksum += util_deint(5, &y);
		checksum += util_deint(6, &dir);
		util_deint(7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_EV_recv(fd, event, seqno, x, y, dir);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DU_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_DU_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_EO_RECV) {
		int checksum=0, checksumrecv;
		int dummy;

		checksum += util_deint(2, &dummy);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_EO_recv(fd, dummy);
		util_DiscardMessage();
		return 0;
	}
     
	if (func==LSSPROTO_BU_RECV) {
		int checksum=0, checksumrecv;
		int dummy;

		checksum += util_deint(2, &dummy);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_BU_recv(fd, dummy);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_JB_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_JB_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_LB_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_LB_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_B_RECV) {
		int checksum=0, checksumrecv;
		char command[65500];

		checksum += util_destring(2, command);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}		
		lssproto_B_recv(fd, command);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_SKD_RECV) {
		int checksum=0, checksumrecv;
		int dir;
		int index;

		checksum += util_deint(2, &dir);
		checksum += util_deint(3, &index);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_SKD_recv(fd, dir, index);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_ID_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int haveitemindex;
		int toindex;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &haveitemindex);
		checksum += util_deint(5, &toindex);
		util_deint(6, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_ID_recv(fd, x, y, haveitemindex, toindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PI_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int dir;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &dir);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_PI_recv(fd, x, y, dir);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DI_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int itemindex;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &itemindex);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_DI_recv(fd, x, y, itemindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DG_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int amount;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &amount);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_DG_recv(fd, x, y, amount);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DP_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int petindex;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &petindex);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_DP_recv(fd, x, y, petindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_MI_RECV) {
		int checksum=0, checksumrecv;
		int fromindex;
		int toindex;

		checksum += util_deint(2, &fromindex);
		checksum += util_deint(3, &toindex);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_MI_recv(fd, fromindex, toindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_MSG_RECV) {
		int checksum=0, checksumrecv;
		int index;
		char message[65500];
		int color;

		checksum += util_deint(2, &index);
		checksum += util_destring(3, message);
		checksum += util_deint(4, &color);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_MSG_recv(fd, index, message, color);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PMSG_RECV) {
		int checksum=0, checksumrecv;
		int index;
		int petindex;
		int itemindex;
		char message[65500];
		int color;

		checksum += util_deint(2, &index);
		checksum += util_deint(3, &petindex);
		checksum += util_deint(4, &itemindex);
		checksum += util_destring(5, message);
		checksum += util_deint(6, &color);
		util_deint(7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_PMSG_recv(fd, index, petindex, itemindex, message, color);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_AB_RECV) {
		int checksum=0, checksumrecv;
		util_deint(2, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_AB_recv(fd);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DAB_RECV) {
		int checksum=0, checksumrecv;
		int index;

		checksum += util_deint(2, &index);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_DAB_recv(fd, index);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_AAB_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_AAB_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_L_RECV) {
		int checksum=0, checksumrecv;
		int dir;

		checksum += util_deint(2, &dir);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_L_recv(fd, dir);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_TK_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		char message[65500];
		int color;
		int area;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_destring(4, message);
		checksum += util_deint(5, &color);
		checksum += util_deint(6, &area);
		util_deint(7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_TK_recv(fd, x, y, message, color, area);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_M_RECV) {
		int checksum=0, checksumrecv;
		int fl;
		int x1;
		int y1;
		int x2;
		int y2;

		checksum += util_deint(2, &fl);
		checksum += util_deint(3, &x1);
		checksum += util_deint(4, &y1);
		checksum += util_deint(5, &x2);
		checksum += util_deint(6, &y2);
		util_deint(7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_M_recv(fd, fl, x1, y1, x2, y2);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_C_RECV) {
		int checksum=0, checksumrecv;
		int index;

		checksum += util_deint(2, &index);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_C_recv(fd, index);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_S_RECV) {
		int checksum=0, checksumrecv;
		char category[65500];

		checksum += util_destring(2, category);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_S_recv(fd, category);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_FS_RECV) {
		int checksum=0, checksumrecv;
		int flg;

		checksum += util_deint(2, &flg);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_FS_recv(fd, flg);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_HL_RECV) {
		int checksum=0, checksumrecv;
		int flg;

		checksum += util_deint(2, &flg);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_HL_recv(fd, flg);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PR_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int request;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &request);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_PR_recv(fd, x, y, request);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_KS_RECV) {
		int checksum=0, checksumrecv;
		int petarray;
		checksum += util_deint(2, &petarray);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_KS_recv(fd, petarray);
		util_DiscardMessage();
		return 0;
	}

#ifdef _STANDBYPET
	if (func==LSSPROTO_SPET_RECV) {
		int checksum=0, checksumrecv;
		int standbypet;
		checksum += util_deint(2, &standbypet);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_SPET_recv(fd, standbypet);
		util_DiscardMessage();
		return 0;
	}
#endif

#ifdef _RIGHTCLICK
	if (func==LSSPROTO_RCLICK_RECV) {
		int checksum=0, checksumrecv;
		int type;
		char data[1024];
		checksum += util_deint(2, &type);
		checksum += util_destring(3, data);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_RCLICK_recv(fd, type, data);
		util_DiscardMessage();
		return 0;
	}
#endif

	if (func==LSSPROTO_AC_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int actionno;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &actionno);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_AC_recv(fd, x, y, actionno);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_MU_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int array;
		int toindex;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &array);
		checksum += util_deint(5, &toindex);
		util_deint(6, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_MU_recv(fd, x, y, array, toindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PS_RECV) {
		int checksum=0, checksumrecv;
		int havepetindex;
		int havepetskill;
		int toindex;
		char data[65500];

		checksum += util_deint(2, &havepetindex);
		checksum += util_deint(3, &havepetskill);
		checksum += util_deint(4, &toindex);
		checksum += util_destring(5, data);
		util_deint(6, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_PS_recv(fd, havepetindex, havepetskill, toindex, data);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_ST_RECV) {
		int checksum=0, checksumrecv;
		int titleindex;

		checksum += util_deint(2, &titleindex);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_ST_recv(fd, titleindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DT_RECV) {
		int checksum=0, checksumrecv;
		int titleindex;

		checksum += util_deint(2, &titleindex);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_DT_recv(fd, titleindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_FT_RECV) {
		int checksum=0, checksumrecv;
		char data[65500];

		checksum += util_destring(2, data);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_FT_recv(fd, data);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_SKUP_RECV) {
		int checksum=0, checksumrecv;
		int skillid;

		checksum += util_deint(2, &skillid);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_SKUP_recv(fd, skillid);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_KN_RECV) {
		int checksum=0, checksumrecv;
		int havepetindex;
		char data[65500];

		checksum += util_deint(2, &havepetindex);
		checksum += util_destring(3, data);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_KN_recv(fd, havepetindex, data);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_WN_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int seqno;
		int objindex;
		int select;
		char data[65500];

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &seqno);
		checksum += util_deint(5, &objindex);
		checksum += util_deint(6, &select);
		checksum += util_destring(7, data);
		
		util_deint(8, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_WN_recv(fd, x, y, seqno, objindex, select, data);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_SP_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int dir;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &dir);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_SP_recv(fd, x, y, dir);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CLIENTLOGIN_RECV) {
		int checksum=0, checksumrecv;
		char cdkey[65500];
		char passwd[65500];
		
		strcpy(PersonalKey, _DEFAULT_PKEY);

		checksum += util_destring(2, cdkey);
		checksum += util_destring(3, passwd);
		
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		
		lssproto_ClientLogin_recv(fd, cdkey, passwd);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CREATENEWCHAR_RECV) {
		int checksum=0, checksumrecv;
		int dataplacenum;
		char charname[65500];
		int imgno;
		int faceimgno;
		int vital;
		int str;
		int tgh;
		int dex;
		int earth;
		int water;
		int fire;
		int wind;
		int hometown;

		checksum += util_deint(2, &dataplacenum);
		checksum += util_destring(3, charname);
		checksum += util_deint(4, &imgno);
		checksum += util_deint(5, &faceimgno);
		checksum += util_deint(6, &vital);
		checksum += util_deint(7, &str);
		checksum += util_deint(8, &tgh);
		checksum += util_deint(9, &dex);
		checksum += util_deint(10, &earth);
		checksum += util_deint(11, &water);
		checksum += util_deint(12, &fire);
		checksum += util_deint(13, &wind);
		checksum += util_deint(14, &hometown);
		util_deint(15, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_CreateNewChar_recv(fd, dataplacenum, charname, imgno, faceimgno, vital, str, tgh, dex, earth, water, fire, wind, hometown);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARDELETE_RECV) {
		int checksum=0, checksumrecv;
		char charname[65500];

		checksum += util_destring(2, charname);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_CharDelete_recv(fd, charname);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARLOGIN_RECV) {
		int checksum=0, checksumrecv;
		char charname[65500];

		checksum += util_destring(2, charname);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_CharLogin_recv(fd, charname);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARLIST_RECV) {
		int checksum=0, checksumrecv;
#ifdef _PKSEVER_VER
		int star = 0;
		util_deint(2, &checksumrecv);
		util_deint(3, &star);
#else
		util_deint(2, &checksumrecv);
#endif
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}

//#ifdef _PKSEVER_VER
//		lssproto_CharList_recv( fd, star);
//#else
		lssproto_CharList_recv( fd);
//#endif

		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARLOGOUT_RECV) {
		int checksum=0, checksumrecv;
		int Flg=1;
#ifdef _CHAR_NEWLOGOUT
		checksum += util_deint(2, &Flg);
		util_deint(3, &checksumrecv);
#else
		util_deint(2, &checksumrecv);
#endif
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_CharLogout_recv(fd, Flg);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PROCGET_RECV) {
		int checksum=0, checksumrecv;
		
		strcpy( PersonalKey, _DEFAULT_PKEY);

		util_deint(2, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_ProcGet_recv(fd);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PLAYERNUMGET_RECV) {
		int checksum=0, checksumrecv;
		util_deint(2, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_PlayerNumGet_recv(fd);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_ECHO_RECV) {
		int checksum=0, checksumrecv;
		char test[65500];

		checksum += util_destring(2, test);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_Echo_recv(fd, test);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_SHUTDOWN_RECV) {
		int checksum=0, checksumrecv;
		char passwd[65500];
		int min;

		checksum += util_destring(2, passwd);
		checksum += util_deint(3, &min);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_Shutdown_recv(fd, passwd, min);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_TD_RECV) {
		int checksum=0, checksumrecv;
		char message[65500];

		checksum += util_destring(2, message);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_TD_recv(fd, message);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_FM_RECV) {
		int checksum=0, checksumrecv;
		char message[65500];

		checksum += util_destring(2, message);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_FM_recv(fd, message);
		util_DiscardMessage();
		return 0;
	}
    
	if (func==LSSPROTO_PETST_RECV) {
		int checksum=0, checksumrecv;
		int nPet;
		int sPet;		

		checksum += util_deint(2, &nPet);
		checksum += util_deint(3, &sPet);		
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}		
		lssproto_PETST_recv(fd, nPet, sPet);		
		util_DiscardMessage();
		return 0;
	}
    
	// _BLACK_MARKET
	if (func==LSSPROTO_BM_RECV) {
		int checksum=0, checksumrecv;
		int iindex;		

		checksum += util_deint(2, &iindex);		
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}		
		lssproto_BM_recv(fd, iindex);		
		util_DiscardMessage();
		return 0;
	}

#ifdef _FIX_DEL_MAP           // WON ADD ���a��a�ϰe�ʺ�
	if (func==LSSPROTO_DM_RECV) {
	    char buffer[2];
	    buffer[0] = '\0';
		lssproto_DM_recv( fd );
		util_DiscardMessage();
		return 0;
	}

#endif

#ifdef _CHECK_GAMESPEED
	if (func==LSSPROTO_CS_RECV) {
	    char buffer[2];
	    buffer[0] = '\0';
		lssproto_CS_recv( fd );
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _TEAM_KICKPARTY
	if ( func == LSSPROTO_KTEAM_RECV ) {
		int checksum = 0, checksumrecv;
		int sindex;
		checksum += util_deint( 2, &sindex);
		util_deint( 3, &checksumrecv);
		if(checksum!=checksumrecv){
			util_DiscardMessage();
			logHack( fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_KTEAM_recv( fd, sindex);
		util_DiscardMessage();
		return 0;
	}
#endif

#ifdef _MIND_ICON
	if(func==LSSPROTO_MA_RECV){
		int checksum = 0, checksumrecv;
		int nMind;
		int x, y;
		
		checksum += util_deint( 2, &nMind);
		checksum += util_deint( 3, &x);
		checksum += util_deint( 4, &y);
		util_deint( 5, &checksumrecv);
		if(checksum!=checksumrecv){
			util_DiscardMessage();
			logHack( fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}		
		lssproto_MA_recv(fd, x, y, nMind);
		util_DiscardMessage();
		return 0;
	}
#endif

#ifdef _CHATROOMPROTOCOL			// (���i�}) Syu ADD ��ѫ��W�D
	if (func==LSSPROTO_CHATROOM_RECV) {
		int checksum=0, checksumrecv;
		char test[65500];

		checksum += util_destring(2, test);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_CHATROOM_recv(fd, test);
		util_DiscardMessage();
		return 0;
	}
#endif

#ifdef _NEWREQUESTPROTOCOL			// (���i�}) Syu ADD �s�WProtocol�n�D�Ӷ�
	if ( func==LSSPROTO_RESIST_RECV ) {
	    char buffer[2];
	    buffer[0] = '\0';
		lssproto_RESIST_recv( fd );
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _OUTOFBATTLESKILL			// (���i�}) Syu ADD �D�԰��ɧޯ�Protocol
	if(func==LSSPROTO_BATTLESKILL_RECV){
		int checksum = 0, checksumrecv;
		int iNum;
		
		checksum += util_deint( 2, &iNum);
		util_deint( 3, &checksumrecv);
		if(checksum!=checksumrecv){
			util_DiscardMessage();
			logHack( fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}		
		lssproto_BATTLESKILL_recv(fd, iNum);
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _STREET_VENDOR
	if(func == LSSPROTO_STREET_VENDOR_RECV){
		int checksum = 0,checksumrecv;
		char message[65500];

		checksum += util_destring(2,message);
		util_deint(3,&checksumrecv);
		if(checksum != checksumrecv){
			util_DiscardMessage();
			logHack(fd,HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_STREET_VENDOR_recv(fd,message);
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _JOBDAILY
	if(func == LSSPROTO_JOBDAILY_RECV){
		int checksum = 0,checksumrecv;
		char buffer[16384];

		buffer[0] = '\0';
		memset(buffer,0,16384); //kkkkkkkkk
		checksum += util_destring(2,buffer);
		util_deint(3,&checksumrecv);
		if(checksum != checksumrecv){
			util_DiscardMessage();
			logHack(fd,HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		print("\njobdaily:%s ",buffer);
		lssproto_JOBDAILY_recv(fd,buffer);
		util_DiscardMessage();
		return 0;

	}
#endif
#ifdef _TEACHER_SYSTEM
	if(func == LSSPROTO_TEACHER_SYSTEM_RECV){
		int checksum = 0,checksumrecv;
		char message[65500];

		checksum += util_destring(2,message);
		util_deint(3,&checksumrecv);
		if(checksum != checksumrecv){
			util_DiscardMessage();
			logHack(fd,HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_TEACHER_SYSTEM_recv(fd,message);
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _ADD_STATUS_2
	if(func == LSSPROTO_S2_RECV){
		return 0;
	}
#endif

	util_DiscardMessage();
	logHack(fd,HACK_NOTDISPATCHED);
	DME(); return -1;
}

/*
   servertoclient XYD( int x, int y, int dir );
 	�����Ʈ��λ��˥ץ쥤�䡼�ΰ��֤���Ĵ�����뤿��˻Ȥ���
*/
void lssproto_XYD_send(int fd,int x,int y,int dir)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, x);
	checksum += util_mkint(buffer, y);
	checksum += util_mkint(buffer, dir);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_XYD_SEND, buffer);
}

void lssproto_EV_send(int fd,int seqno,int result)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, seqno);
	checksum += util_mkint(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_EV_SEND, buffer);
}

void lssproto_EN_send(int fd,int result,int field)
{
	char buffer[65500];
	int checksum=0;

	//print(" EN_send ");

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, result);
	checksum += util_mkint(buffer, field);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_EN_SEND, buffer);
}


void lssproto_RS_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_RS_SEND, buffer);
}


void lssproto_RD_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_RD_SEND, buffer);
}

void lssproto_B_send(int fd,char* command)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, command);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_B_SEND, buffer);
}

void lssproto_I_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_I_SEND, buffer);
}

void lssproto_SI_send(int fd,int fromindex,int toindex)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, fromindex);
	checksum += util_mkint(buffer, toindex);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SI_SEND, buffer);
}

void lssproto_MSG_send(int fd,int aindex,char* text,int color)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, aindex);
	checksum += util_mkstring(buffer, text);
	checksum += util_mkint(buffer, color);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_MSG_SEND, buffer);
}


void lssproto_PME_send(int fd,int objindex,int graphicsno,int x,int y,int dir,int flg,int no,char* cdata)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, objindex);
	checksum += util_mkint(buffer, graphicsno);
	checksum += util_mkint(buffer, x);
	checksum += util_mkint(buffer, y);
	checksum += util_mkint(buffer, dir);
	checksum += util_mkint(buffer, flg);
	checksum += util_mkint(buffer, no);
	checksum += util_mkstring(buffer, cdata);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PME_SEND, buffer);
}


void lssproto_AB_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_AB_SEND, buffer);
}


void lssproto_ABI_send(int fd,int num,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, num);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_ABI_SEND, buffer);
}

void lssproto_TK_send(int fd,int index,char* message,int color)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, index);
	checksum += util_mkstring(buffer, message);
	checksum += util_mkint(buffer, color);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_TK_SEND, buffer);
}

void lssproto_MC_send(int fd,int fl,int x1,int y1,int x2,int y2,int tilesum,int objsum,int eventsum,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, fl);
	checksum += util_mkint(buffer, x1);
	checksum += util_mkint(buffer, y1);
	checksum += util_mkint(buffer, x2);
	checksum += util_mkint(buffer, y2);
	checksum += util_mkint(buffer, tilesum);
	checksum += util_mkint(buffer, objsum);
	checksum += util_mkint(buffer, eventsum);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_MC_SEND, buffer);
}


void lssproto_M_send(int fd,int fl,int x1,int y1,int x2,int y2,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, fl);
	checksum += util_mkint(buffer, x1);
	checksum += util_mkint(buffer, y1);
	checksum += util_mkint(buffer, x2);
	checksum += util_mkint(buffer, y2);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_M_SEND, buffer);
}

/*
   servertoclient C( string data );
       (Characters)
       ���饤����Ȥ� CA ����ơ����Υ����ˤĤ���Ĵ�ʤ��ä����ϡ� C 
       �Ǥ��δؿ����׵�Ǥ��롣�����Фϥ����å����ʤ���
       
 	string data
 		  �ե����ޥåȤ� �ʲ���3����ι��ܤ򥳥�ޤǤʤ�٤�
 	    ��ΤǤ���.���줾��ι��ܤ���ȤϤ���ˤ�����'|'�Ǥ������
 	    �Ƥ��롣
 	    
 	      �������Ǥ�����줿�ȡ�����12�Ĥξ��
 		  �ȡ���������Ƥ�
 
 		  WHICHTYPE|CHARINDEX|X|Y|DIR|BASEIMG|LEVEL|NAMECOLOR|NAME|SELFTITLE|WALKABLE|HEIGHT|POPUPNAMECOLOR
 		  ���Υ��֥������Ȥϥ���饯�����Ǥ���Ȥ������ȤǤ��롣
 		  
 		  WHICHTYPE �ϡ����Υ���饯�������ɤ����ä�����Τ��
 		  �Ǥ��뤫������ϥ����С��Ǥϰʲ��Τ褦���������Ƥ��롣
 		  
 		typedef enum
 		{
 		    CHAR_TYPENONE,          ���Ǥ�ʤ�
 		    CHAR_TYPEPLAYER,        �ץ쥤�䡼
 		    CHAR_TYPEENEMY,         Ũ
 		    CHAR_TYPEPET,           �ڥå�
 		    CHAR_TYPEDOOR,          �ɥ�
 		    CHAR_TYPEBOX ,          ��Ȣ
 		    CHAR_TYPEMSG ,          ����
 		    CHAR_TYPEWARP ,         ��ץ�����
 		    CHAR_TYPESHOP ,         Ź
 		    CHAR_TYPEHEALER ,       �ҡ��顼
 		    CHAR_TYPEOLDMAN ,       ĹϷ
 		    CHAR_TYPEROOMADMIN,     ��ư����
 		    CHAR_TYPETOWNPEOPLE,    �ޤ��ΤҤ�
 		    CHAR_TYPEDENGON,        ������
 		    CHAR_TYPEADM,           ������
 		    CHAR_TYPETEMPLE,        Temple master
 		    CHAR_TYPESTORYTELLER,   �����
 		    CHAR_TYPERANKING,       ��ư����󥭥�ɽ���ΣУ�
 		    CHAR_TYPEOTHERNPC,      ����¾�θ����оݤˤʤ�ʤ�NPC
 		    CHAR_TYPEPRINTPASSMAN,  �ɥ��Υѥ����ɽ������NPC
 		    CHAR_TYPENPCENEMY,      ����Ũ
 		    CHAR_TYPEACTION,        ����������ȿ������NPC
 		    CHAR_TYPEWINDOWMAN,     ������ɥ�ɽ������NPC�ʥƥ��Ȥ���)
 		    CHAR_TYPESAVEPOINT,     �����֥ݥ����
 		    CHAR_TYPEWINDOWHEALER,  ������ɥ������פΥҡ��顼
 		    CHAR_TYPEITEMSHOP,	    ��Ź
 		    CHAR_TYPESTONESHOP,	    ���ײ��ʥڥåȤε�����
 		    CHAR_TYPEDUELRANKING,   DUEL��󥭥�NPC
 		    CHAR_TYPEWARPMAN,	    ��ץޥ�NPC
 		    CHAR_TYPEEVENT,	    ���٥��NPC
 		    CHAR_TYPEMIC,	    ���٥��NPC
 		    CHAR_TYPELUCKYMAN,	    ���٥��NPC
 		    CHAR_TYPEBUS,	    �ޥ�⥹�Х�
 		    CHAR_TYPECHARM,	    ���٥��NPC
 		    CHAR_TYPENUM,
 		}CHAR_TYPE;
 		  
 		  �����¾�Υץ쥤�䡼��NPC��Ũ�Ǥ��롣�����С�������
 		  ����Ȥ��ϸ������ϰϤˤĤ��������������롣�Ĥ�
 		  �ꥯ�饤����Ȥϡ����Υѥ��åȤ�����Ȥä��Ȥ��ˤ���
 		  �ѥ��åȤ˽񤫤�Ƥ��ʤ���������äƤ�����ä��Ƥ�
 		  �ޤäƤ褤�Ȥ������ȤǤ��롣�ޤ����ޥ������������
 		  �碌���Ȥ���ɽ���������Ϥ��ξ���Τߤ˴𤤤Ƥ��롣
 		  �����顢�ޥ�����������򤢤碌���Ȥ���ɽ�����뤳�Ȥ�
 		  �ѹ����줿���ϡ������С��ϡ����δؿ���ǽưŪ�˸Ƥ�
 		  �����ʤ���Фʤ�ʤ��� SELFTITLE�ˤĤ��Ƥϡ��ǥ�ߥ�
 		  �Ǥ��뤿������դ��ޤʤ��褦�˥��������פ���Ƥ��롣
 		  ���饤����Ȥϥ��������Υ��ޥ��(CA)������ޤǤ�Ω
 		  ����ɽ�����롣CHARINDEX�����С���ΰ�դ˥�������
 		  ��Ǥ����ֹ桢BASEIMG��ɽ���Τ�����ֹ桢LEVEL�ϥ���
 		  ��Υ�٥�(0�ʤ�ɽ�����ʤ��������ͤ�NPC�ʤɤ˻Ȥ���)
 		  WALKABLE��1�ΤȤ����ξ���̲᤹�뤳�Ȥ��Ǥ���0�ʤ���
 		  �᤹�뤳�Ȥ��Ǥ��ʤ���HEIGHT�Ϲ⤵���Ĥ�Τ�������
 		  �ʤ��Τ��λ��ꡣ
 		  
 		  ����饯������̾���ȼ�ͳ�ι�ϡ�<a href="#escaping">
 		  ���������פ���ʤ���Фʤ�ʤ���'|'�ǥȡ������
 		  �������Ƥ��饨�������פ������롣���������פ����'
 		  |'���ۤ���ʸ���ˤ��������Τǡ��ǽ��ñ���'|'���
 		  ��ߥ��Ȥ��Ƥ褤�������������⡢̾���ȼ�ͳ�ι�򥨥�
                   �����פ��Ƥ��餿�����ǤĤʤ��Ǥ����������롣
                   �ޤ����ڥåȤξ��ϼ�ͳ�ι������˥桼����������
                   �����ڥåȤ�̾������������Ƥ��롣
 		  
 	      �������Ǥ�����줿�ȡ�����6�Ĥξ��
 		  �ȡ���������Ƥ�
 		  INDEX|X|Y|BASEIMG|LEVEL|ITEM1LINEINFO
 		  �����̤�����Ƥ��륢���ƥ�ˤĤ��Ƥξ���Ǥ��롣
 		  INDEX�ϥ����Υ���ǥ��å��Ȥ��֤�ʤ�INDEX�Ǥ��롣
 		  ���Υ����ƥ��ä����˻��Ѥ��롣X,Y�ϥ����ƥ�Υ���
 		  �Х���֡�BASEIMG�ϲ������ֹ档ITEM1LINEINFO��1��
 		  info��ɽ�����뤿��ξ���Ǥ��롣�����ƥ०����ɥ���
 		  ��ɽ���Ѥξ�����̤���ˡ���Ѱդ��롣�����ƥ�˴ؤ���
 		  ��CA����ʤ���ITEM1LINEINFO��<a href="#escape">��������
 		  �פ���롣���Υ��������פ���ˡ�Ͼ�ι��ܤ򻲾ȡ�
 
 	      �������Ǥ�����줿�ȡ�����4�Ĥξ��
 		  �ȡ���������Ƥ�
 		  INDEX|X|Y|VALUE
 		  �����̤�����Ƥ��뤪��ˤĤ��Ƥξ���Ǥ��롣���ƤϤ�
 		  �٤ƿ�����INDEX,X,Y �ϥ����ƥ��Ʊ����VALUE �Ϥɤ��
 		  �����̤��Ȥ������Ǥ��롣�����ƥ�ˤĤ��Ƥξ���Ǥ��롣
 	      �������Ǥ�����줿�ȡ�����1�Ĥξ��
 		  INDEX
 		  ���Υ�����C�϶������ʤ���
*/
void lssproto_C_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_C_SEND, buffer);
}

/*
   servertoclient CA( string data );
       (CharacterAction)
       �������ϰϤˤ��륭���Υ����������֤򹹿����롣
       �����С����饯�饤����Ȥ˰���Ū���������롣
       �ƥ�����1��������󤴤Ȥ������������С��ϥ���������Ǥ����
       �����̤��뤳�ȡ�
       
 	string data
 	CHARINDEX|X|Y|ACTION|PARAM1|PARAM2|PARAM3|PARAM4|....��
 	    ��ޤǤ����ä���Τˤ��롣PARAM�ϥ�������󤴤Ȥ˸Ŀ���
 	    �Ȥ������⤳�Ȥʤ롣�ʲ��ϥ�������������X,Y�ϰ��֤Ǥ�
 	    �Υ��������Ǥ⡢�����餷�����֤���ꤹ�롣
 	    ����ʸ����ϥ��������פ���ʤ���
 	  
 	        ACTION  PARAM1  PARAM2 
 	       PARAM3  PARAM4  
 	        Stand:0  ����0~7        
 	        Walk:1  ����0~7        
 	        Attack:2  ����0~7        
 	        Throw:3  ����0~7        
 	        Damage:4  ����0~7      
 	        Dead:5  ����0~7        
 	        UseMagic:6  ����0~7  
 	        UseItem:7  ����0~7  
 	        Effect:8  ����0~7  ���ե������ֹ�  
 	        Down:10 (�ݤ��)  ����0~7  
 	        Sit:11 (�¤�)  ����0~7  
 	        Hand:12 (��򿶤�)  ����0~7  
 	        Pleasure:13 (���)  ����0~7  
 	        Angry:14 (�ܤ�)  ����0~7  
 	        Sad:15 (�ᤷ��)  ����0~7  
 	        Guard:16 (������)  ����0~7  
 	        actionwalk:17 (������������⤭)  ����0~7  
 	        nod:18 (���ʤ���)  ����0~7  
 	        actionstand:19 (�����������Ω���ݡ���)  ����0~7  
 	        Battle:20 (��Ʈ����)  ����0~7  BattleNo(-1 �ʤ�ɽ���ä���  SideNo  HelpNo�ʣ��ʤ������Ƥ�CAɽ�������ʤ�ä����ޤ���̵����  
 	        Leader:21 (�꡼��������)  ����0~7  0:ɽ���ä� 1:ɽ��  
 	        Watch:22 (��Ʈ����)  ����0~7  0:ɽ���ä� 1:ɽ��  
 	        namecolor:23(̾���ο�����)  ����0~7  ̾���ο��ֹ�  
 	        Turn:30(�����Ѵ�)  ����0~7    
 	        Warp:31(���)  ����0~7    
 	      
 	ACTION���ͤ������ǡ����ƤϾ��ɽ�κ�ü�ι��ܤǤ��롣
*/
void lssproto_CA_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CA_SEND, buffer);
}

/*
   servertoclient CD( string data );
       (CharacterDelete)
 	data �ϥǥ�ߥ���,�ˤǶ��ڤ�줿����ǥå�����
 	ʣ���������������롣
       ����ID����äƤ��륭��饯�����ä������˥����Ф��饯�饤����Ȥ�
       ������롣
*/
void lssproto_CD_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CD_SEND, buffer);
}

/*
   <a name="R">servertoclient R( string data );
       (Radar)
       �졼���������Ƥ򥯥饤����Ȥ��������롣���饤����ȤϤ���������
       �׵᤹�뤳�ȤϤʤ��������С���Ŭ���ʥ����ߥ󥰤��������롣
       ���Ȥ���10�⤢�뤯���ȤȤ���1ʬ���ȤȤ���
 
 	string data
 	x ,y, kind ,�ν���ͤ�'|'�Ǥ����äƤʤ�٤���Τ�
 	    �����'|'�Ǥʤ�٤���Ρ�
 	    x,y�ϥ���餫������а��֡�kind�ϰʲ��ˤ��᤹���ͤǤϤʤ�
 	    ���档�졼�����ˤ��Ĥ�ʪ�μ������ꤹ�롣�����Ƥ�����
 	    �μ���ϡ�����ޤǤ˾�������ˡ�Ȥ�����������Ѳ����������
 	    �����С����ͤ��롣
 	 
 	        kind����  ����  
 	        E  Ũ  
 	        P  �ץ쥤�䡼  
 	        S  ��  
 	        G  ����  
 	        I  �����ƥ�(���ʳ�)  
   
   data���㡧"12|22|E|13|24|P|14|28|P"
   ����ʸ����ϥ��������פ���ʤ���
*/
void lssproto_R_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_R_SEND, buffer);
}

/*
   servertoclient S( string data );
       (Status)
       �����Υ��ơ��������������롣
       �ǡ����� ���ƥ��국��ʸ��(��ʸ��)���� �ȤʤäƤ��롣�Ĥޤ�ǽ��
       1ʸ���򸫤�в��Υ��ơ�������ʬ�롣���Ƥ�2ʸ���ܤ���Ǥ��롣
       ���Ƥϰʲ��Υե����ޥåȤˤ�������������������'|'���ǥ�ߥ��Ǥ��롣
       2���ܰʹߤΥȡ��������ƤǤ��롣
       
 	P ���ѥ�᡼��
 	    kubun hp maxhp mp maxmp str tough exp maxexp exp 
 	    level attackpower deffencepower 
 	    fixdex fixcharm fixluck fixfireat fixwaterat fixearthat fixwindat
 	    gold �դ��Ƥ���
 	    �ι��index ̾�� ���ʾι� 
 
 	    �ǥ�ߥ��� '|' �Ǥ��롣�ޤ�̾���ȼ��ʾι�ϡ�
 	    <a href="#escaping">���������פ�����Τ�
 	    �������ǤĤʤ�����Τ��̿�����롣
 	    ���줾����ͤη��ϰʲ���
 	    kubun �ˣ������äƤ�������ѥ�᡼����������
 	    ���ʾ���ȡ��ƥӥåȤ�Ω�äƤ����ΤΥѥ�᡼�������֤����ä���������Ƥ��롣
 	   �㤨�С������褿��hp��Maxhp��kubun �ʹߤΥȡ������Ǽ����Ƥ��롣
 	    
 	        kunun  int  
 	        Hp(����)hp  int  
 	        MaxHp  int  
 	        Mp(���ѥѥ)  int  
 	        MaxMp  int  
 	        Vital(����)  int  
 	        Str(����)  int  
 	        Tough(���פ�)  int  
 	        Dex(���ᤵ)  int  
 	        Exp(�и���)exp  int  
 	        MaxExp(���ޤǤηи���)  int  
 	        Level(��٥�)  int  
 	        Attack(������)  int  
      	        Defense(������)  int  
 	        fQuick(�ǽ����ᤵ)  int  
 	        fCharm(̥��)  int  
 	        fLuck(��)  int  
 	        fEarth(��)fixearthat  int  
 	        fWater(��)  int  
 	        fFire(��)  int  
 	        fWid(��)fixwindat  int  
 	        Gold(����)  int  
 	        �ι��index  int  
 	        �ǥ奨��ݥ����  int  
 	        ž�����  int  
 	        ̾��  ʸ����  
 	        ���ʾι�  ʸ����  
 	    
 	    ������( char �����󤽤Τޤ޽� )
 	    
 	    P10|20|10|20|10|10|10|1|2|13|13|1|100|10|�ؤ��|abc|def
 	
 	C ��ɸ
 	    floor maxx maxy x y
 	    ��  | �Ƕ��ڤä����롣
 	    ������( char �����󤽤Τޤ޽� )
 	    
 	    C1024|100|100|10|20
 	    
 	    �嵭����Ǥϡ��ե�ID1024, ������100x100 X 10,Y 20�ΤȤ�
 	    ��˥���饯����������Ȥ������Ƥˤʤ롣���Υޥ������Υ�
 	    ����դ��᤿��ɸ�򥯥饤����Ȥ��Τ뤳�Ȥ��Ǥ���ΤϤ���
 	    ���ޥ�ɤΤߤǤ��롣�⤭�η�̤ϥե���ΰ��֤����狼���
 	    ������äơ������೫�ϻ������ʻ��ѤΤȤ�����פΤȤ��ʤ�
 	    �ϡ����Υ��ޥ�ɤ�Ĥ��ä��������ͤ��������ʤ��ƤϤʤ�ʤ���
 	    
 	I �������ƥ�
 	    (�����ƥ�1)|(�����ƥ�2)...(�����ƥ�n)
 	    �����ƥ�����Ƥϰʲ�����ˡ�ǥѥå����롣
 	    �ҤȤĤΥ����ƥ�ϡ����ʤ餺�ʲ���6�ĤΥȡ�����Υ��åȤ�
 	    ��������롣�ҤȤĤҤȤĤ����Ƥϡ�
 	    
 	    ̾��|̾��2|���ơ���|�����ƥ������|���˥��ֹ�|���Ѳ�ǽ���|�����о�|�����ƥ��٥�|�ե饰��
 	    
 	      �����ƥ�̾�ϼ��̥�٥�Ǽ�ưŪ���ѹ�����롣
 	      ̾��2�ϼ��̥�٥�ˤ�äơ������ƥ��ǽ�Ϥ�����ͽ�ꡣ
 		  ���饤����ȤΥ����ƥ����2���ܤ���������
 	      ���ơ�����̾���ο����ؤ�Τ˻Ȥ���
 	      �����ƥ��������
 	      ���˥��ֹ�ϡ������ֹ档
 	      ���Ѳ�ǽ���Ȥϡ����Υ����ƥब�ɤ��ǻ��Ѥ��������ǽ�������äƤ��롣
 	          ����ϡ������С��Ǥϰʲ��Τ褦���������Ƥ��롣
 	          
 			typedef enum
 			{
 				ITEM_FIELD_ALL,			���٤Ƥξ��ǻȤ���
 				ITEM_FIELD_BATTLE,		��Ʈ��Τ�
 				ITEM_FIELD_MAP,			�̾�ޥå׾�Τ�
 
 			}ITEM_FIELDTYPE;
 	          
 	      �����оݤȤϡ����Υ����ƥ����Ѥ��뤳�Ȥν�����оݤ����äƤ��롣
 	          �����С��Ǥϰʲ��Τ褦���������Ƥ��롣
 	          
 			typedef enum
 			{
 				ITEM_TARGET_MYSELF,		��ʬ�Τ�
 				ITEM_TARGET_OTHER,		¾�ο͡ʼ�ʬ�ޤ�)
 				ITEM_TARGET_ALLMYSIDE,		̣������
 				ITEM_TARGET_ALLOTHERSIDE,	���¦����
 				ITEM_TARGET_ALL,		����
 			}ITEM_TARGETTYPE;
 		
   	        <font size=+1>���ο�����100��­���ȡ����Ǥ���Ԥ��оݤȤʤ롣
 	      �����ƥ��٥롣���Υ�٥�ʾ�μԤǤʤ�����������ʤ���
 	      �ե饰�ࡣ����ʥե饰���������롣�ƥӥåȤ��б��ϰʲ����̤ꡣ

 		      0bit��  �ڥåȥ᡼���������ǽ�ʥ����ƥफ�ɤ�����������ǽ���ȣ��������������Ǥ��ʤ���  
 		      1Bit��  ���Υ����ƥब��������뤫�ɤ�����  
 		      2Bit��  ���Υ����ƥब�������ɤ�����1���������Ǥ��롣  
 	    
 	    ��������Ǥ�<a href="#escaping">���������פ��롣
 	    ������( char �����󤽤Τޤ޽� )
 	    
 	    I���� �� ��|str+2|1|����\|����|10|2|0
 	    
 	    ���ơ������ͤΰ�̣�ϡ������ͤ�
 	    
 	        1  ���ơ���A  
 	        2  ���ơ���B  	      
 	        3  ���ơ���C  
 	        4  ���ơ���D  
 	    
 	    �Ȥ��롣�ޤ��������ƥ�ɽ�ζ�����ʬ�ˤĤ��Ƥϡ�5�ĤΥȡ�����
 	    ���ǡ��Ĥޤꤿ�������ʤ������֤���������Ƥ��롣
 	
 	S ��������
 	    (������0)|(������1)|(������2) ... (������n)
 	    �ι��Ʊ���ǡ�����餬��Ƥ���祹�����ʬ���������롣�Ĥ�
 	    ��ʤ���Τ� ||| �Ȥʤ�ΤǤ��롣
 	    �ƥ���ȥ�����Ƥϡ�
 	    ������μ��ॳ����|��٥�
 	    �Ǥ��롣
 	    ������( char �����󤽤Τޤ޽� )
 	    
 	    S10|2|20|2|||
 	
 	T �ι�
 	    (�ι�0)|(�ι�1)|(�ι�2) ... (�ι�n)
 	    �ι�Υ���ȥ꤬���ξ��� '|' ��Ϣ³����Τǡ�Ϣ³������
 	    �Τ��ά���ƥ�����󤷤ƤϤʤ�ʤ������ʤ餺����Ŀ�ʬ����
 	    ���롣
 	    �ҤȤġ��ҤȤĤ����Ƥϡ�
 	    ̾��
 	    �ΤߤǤ��롣
 	    ������( �����Ƥ��� char �����󤽤ΤޤޤǤ��� )
 	    
 	    T����|��äѤ餤
 	    
 	    �����<a href="#escaping">���������פ��롣
 
 	M ��ʬ�ѥ�᡼��
 	    ����饯�����Υ��ơ������Τ��������ˤ��ѹ��������(����
 	    ��)�Ȥ����Ǥʤ����(�����ͤʤ�)�����롣�ɤ줫�Υ��ơ�����
 	    ���ѹ�����뤿�Ӥ���������Υѥ�᡼������������Τϥͥå�
 	    ����ȥ�ե��å���̵�̤˻Ȥ����Ȥˤʤ�Τǡ��ä�HP ,
 	    MP,EXP���ͤ����ˤ��ܤä��ѥ�᡼���������ޥ�ɤ��Ѱդ��롣
 	    ���줬����M���ޥ�ɤǤ��롣2ʸ���ܰʹߤ����ȡ������HP��
 	    ������ , ����ȡ������MP���軰�ȡ������EXP�Ǥ��롣
 	    �ʲ��˶�����򤷤᤹��
 	    
 	    M54|210|8944909
 	    
 	    ������Ǥ�HP�θ����ͤ�54��MP�θ����ͤ�210���и��ͤ�8944909
 	    �ˤʤäƤ���Τ�������3������ä��ѹ����٤��⤤�Ȼפ���
 	    ������Ω�������ޥ�ɲ��������������󤳤�M���ޥ�ɤΤ���
 	    ���P���ޥ�ɤ����ѥ�᡼�����������Ƥ�褤������������Ŭ
 	    �˥ͥåȥ����Ȥ�����ˤϡ����Υ��ޥ�ɤ�Ȥ����Ȥ��侩
 	    ����롣
 	    
         D ������ID
 	    ��������ˤ������륭����index������Ȼ��֤����롣
 	    D1000|912766409�Ȥ��������롣
 	
 	E ���󥫥���ȳ�Ψ�ʲ���)|(���)
 	    ���󥫥����Ψ���������롣��Ψ��n/100
 	    ���饤����Ȥϡ����¤��ͤ��饹�����Ȥ��ơ������⤯���+1������Ψ�ǥ��󥫥���Ȥ��뤫��Ƚ�Ǥ��롣
 	    ����������¤��ͤ�ۤ��ʤ��褦�ˤ��롣
 	    ����ˤ��줬�����Ƥ���ޤǤ��γ�Ψ�ǥ��󥫥���Ȥ�׻��������
 	
 	J0 ��J6 �Ȥ�����Ѥ�����
 	    �Ȥ�����Ѥ����Ƥ��������롣
 	    0 ��6 �Ϥ��줾�쥢���ƥ�������ս���б����Ƥ��롣�����������ս��Ƭ��ƹ���ꡤ�����ʡ�
 	    ������������ʤ�ͽ������������ĥ�ΰ٤����Ƥ������ս�ξ��������
 	    J0|kubun|mp|field|target|name|comment
 	    �Ȥ����ե����ޥåȤˤʤäƤ��롣
 	    kubun �Ϥ����󤬤��뤫̵������������̵��������ʹߤ˥ȡ����󤹤�ʤ��������Ȥ��롣
 	    mp�Ͼ����Ϥ�ɽ����
 	    field�Ϥɤξ��ǻȤ��뤫�������С��Ǥϰʲ����ͤ��������Ƥ��롣
 	    
 		typedef enum
 		{
 			MAGIC_FIELD_ALL,		���٤Ƥξ��ǻȤ���
 			MAGIC_FIELD_BATTLE,		��Ʈ��Τ�
 			MAGIC_FIELD_MAP,		�̾�ޥå׾�Τ�
 			
 		}MAGIC_FIELDTYPE;
 	    
 	    target�Ϥɤ���оݤ˽���뤫�������С��Ǥϰʲ��Τ褦���������Ƥ��롣
 	    
 		typedef enum
 		{
 			MAGIC_TARGET_MYSELF,		��ʬ�Τ�
 			MAGIC_TARGET_OTHER,		¾�ο͡ʼ�ʬ�ޤ�)
 			MAGIC_TARGET_ALLMYSIDE,		̣������
 			MAGIC_TARGET_ALLOTHERSIDE,	���¦����
 			MAGIC_TARGET_ALL,		����
 			MAGIC_TARGET_NONE,		ï���������ʤ����ɸ�䤿��λ�
 			MAGIC_TARGET_OTHERWITHOUTMYSELF,¾�ο͡ʼ�ʬ�ޤޤʤ�)
 			MAGIC_TARGET_WITHOUTMYSELFANDPET, ��ʬ�ȥڥåȰʳ�
 			MAGIC_TARGET_WHOLEOTHERSIDE,    �����Υ���������
 		}MAGIC_TARGETTYPE;
 	    
 	    <font size=+1>���ο�����100��­���ȡ����Ǥ���Ԥ��оݤȤʤ롣
 	    
 	    name�ϼ���̾��
 	    comment�Ϥ��μ��Ѥ�������
 	
 	N0 �� N3 ��֤Υѥ�᡼��
 	    ��֤ˤʤäƤ���ͤξ�����������롣
 	    N0|kubun|level|charaindex|maxhp|hp|mp|name
 	    
 	    kubun �Ϥ����󤬤��뤫̵������������̵��������ʹߤ˥ȡ����󤹤�ʤ��������ѥ�᡼����
 	    2�ʾ���ȡ��ƥӥåȤ�Ω�äƤ����ΤΥѥ�᡼����1bit �� level 2bit ��charaindex�ʤɡ�
 	    ����������롣
 	    charaindex ����֤�charaindex
 	    level�Ϥ��οͤΥ�٥롣
 	    maxhp�Ϥ��οͤ�MAXHP
 	    hp�Ϥ��οͤθ��ߤ�HP
 	    mp�Ϥ��οͤε���
 	    name �Ϥ��οͤ�̾����
 	
 	K0��K4 ��ε�ѥ�᡼��
 	    hp maxhp mp maxmp str tough exp 
 	    level attackpower deffencepower 
 	    fixdex fixcharm fixluck fixfireat fixwaterat fixearthat fixwindat
 	    ̾�� status
 	    K�θ��0��4����ꤷ�ơ��ɤζ�ε������ꤹ�뤳�ȡ�
 	    ���μ��Υȡ������0���褿�餽�Υڥå����̵���ȸ�������
 	    ������ϣ��Ǥ��롣1�������ѥ�᡼����
 	    2�ʾ���ȡ��ƥӥåȤ�Ω�äƤ����ΤΥѥ�᡼����1bit �� hp  2bit ��maxhp�ʤɡ�
 	    ����������롣
 	    �ǥ�ߥ��� '|' �Ǥ��롣�ޤ�̾���ȼ��ʾι�ϡ�
 	    <a href="#escaping">���������פ�����Τ�
 	    �������ǤĤʤ�����Τ��̿�����롣
 	    ���줾����ͤη��ϰʲ���
 	    
 	        No.(�ֹ�)  int  
 	        islive(����)  int  
 	        GRA(�����ֹ�)  int  
 	        Hp(�ѵ���)  int  
 	        MaxHp(�����ѵ���)  int  
 	        Mp  int  
 	        MapMp  int  
 	        Exp(�и���)  int  
 	        MaxExp(���ޤǤηи���)  int  
 	        Level(��٥�)  int  
 	        Attack(������)  int  
 	        Defense(������)  int  
 	        Quick(���ᤵ)  int  
 	        Ai(������)  int  
 	        fEarth(��)  int  
 	        fWater(��)  int  
 	        fFire(��)  int  
 	        fWid(��)  int  
 	        Slot(�������ǿ�)  int  
 	        ̾���ѹ����ĥե饰  int  
 	        ̾��  ʸ����  
 	        �桼�����ڥå�̾  ʸ����  
 	    
 	    ̾���ѹ����ĥե饰�Ȥϡ����ΥڥåȤ�̾�����ѹ����Ƥ褤���ɤ����Υե饰�ǡ�
 	    1 �����ѹ��ϣˡ��������ѹ��ԲĤȤʤ롣
 	    
 	    ������( char �����󤽤Τޤ޽� )
 	    
 	    P10|20|10|20|10|10|10|1|2|13|13|1|100|10|�ؤ��|PC
 
 	W0��W4 �ڥåȤε��ǡ���
 	  W0|skillid|field|target|name|comment| x 7
 	  W0 �� W4 �Ϥ��줾��ΥڥåȤ��б����Ƥ��롣
 	  petskillid �ϡ��ڥåȤε����ֹ档pet_skillinfo.h���������Ƥ��롣
 	  field �Ϥ��ε����ɤ��ǻ��ѤǤ��뤫�������С��Ǥϰʲ��Τ褦���������Ƥ��롣
 	  
 		typedef enum
 		{
 			PETSKILL_FIELD_ALL,		���٤Ƥξ��ǻȤ���
 			PETSKILL_FIELD_BATTLE,		��Ʈ��Τ�
 			PETSKILL_FIELD_MAP,		�̾�ޥå׾�Τ�
 
 		}PETSKILL_FIELDTYPE;
 	  
 	  target �Ϥ��ε����оݤ��ɤ�������Τ��������С��Ǥϼ��Τ褦���������Ƥ��롣
 	  
 		typedef enum
 		{
 			PETSKILL_TARGET_MYSELF,		��ʬ�Τ�
 			PETSKILL_TARGET_OTHER,		¾�ο͡ʼ�ʬ�ޤ�)
 			PETSKILL_TARGET_ALLMYSIDE,	̣������
 			PETSKILL_TARGET_ALLOTHERSIDE,	���¦����
 			PETSKILL_TARGET_ALL,		����
 			PETSKILL_TARGET_NONE,		ï���������ʤ����ɸ�䤿��λ�
 			PETSKILL_TARGET_OTHERWITHOUTMYSELF,¾�ο͡ʼ�ʬ�ޤޤʤ�) 
 			PETSKILL_TARGET_WITHOUTMYSELFANDPET, ��ʬ�ȥڥåȰʳ�
 		}PETSKILL_TARGETTYPE;
 	  
 	  name �ϵ���̾����
 	  comment�Ϥ��ε����Ф���������
 	  target|name|comment| �ϵ��ο���ʬ���Ԥ������Ƥ��롣
 	  ���ϰ�����Ĥޤǡ�����ǵ���ȴ���Ƥ����"|"�Τߤ�³���˾��
 	  �ϥ��饤����Ȥǵͤ��ɽ���������
*/
void lssproto_S_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_S_SEND, buffer);
}

/*
   servertoclient D( int category , int dx , int dy , string data );
       (Display)
       ���̤˲���ɽ��������ᡣ
 
       
 	int category
 	����ɽ�����뤫��
 	    
 	        ��  ����  
 	        1  ��ʬ�ʳ���Ϳ�������᡼����data��ʸ��
 		  ��ˤʤä��͡�  
 		    2  ��ʬ�����������᡼����data��ʸ�����
 		      �ʤä���  
 		
 	int dx
 	�ޥ�����餫��Υ���å����а���X������Ū�ˤϥ��٥�Ȥ�ȯ��
 	    �������֡����饤����ȤϤ����ͤ���Ŭ��Ŭ�ڤʰ��֤�׻�����
 	    ���褹�롣
 	int dy
 	���а���Y
 	string data 
 	ɽ���������ơ����Ƥ�category�ˤ�äƷ�ޤ롣
 	    ����ʸ�����<a href="#escaping">���������פ���ʤ����
 	    �ʤ�ʤ���
*/
void lssproto_D_send(int fd,int category,int dx,int dy,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, category);
	checksum += util_mkint(buffer, dx);
	checksum += util_mkint(buffer, dy);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_D_SEND, buffer);
}

/*
   servertoclient FS( int flg);
       (FlgSet)PS�Τα������ޤ��ϥ�������ʤɤ˼�ʬ�ξ��֤Ȥ��������Ƥ��롣
       
         int flg
          0 bit  0: ���Off                  1: ���On
              �ʸ���̤���ѡ�1 bit  0: ��Ʈ���滲��off          1: ��Ʈ���滲��On    
              2 bit  0: DUEL off                 1: DUEL On
              3 bit  0: ���̤Υ���åȥ⡼��     1: �ѡ��ƥ��ˤ�������åȤ����Фʤ��⡼��
              4 bit  0: ̾�ɸ�OK               1: ̾�ɸ򴹵���
*/
void lssproto_FS_send(int fd,int flg)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, flg);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_FS_SEND, buffer);
}

/*
   servertoclient HL( int flg);
       (HeLp)HL�Τα������ޤ��ϥѡ��ƥ�����֤���Ʈ�Τ������⡼�ɤ��ѹ��������������Ƥ��롣
       
         int flg
          0: �������⡼��Off                  1: �������⡼��On
*/
void lssproto_HL_send(int fd,int flg)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, flg);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_HL_SEND, buffer);
}

/*
   servertoclient PR( int request, int result);
       (PartyRequest)����׵�α�����PR�򥯥饤����Ȥ����äƤ��ʤ��Ƥ⤳���������������롣
        �ѡ��ƥ��������򻶡ʿƤ�ȴ�����ˤʤɤλ���
       
       	int request
       	0: ���� 1:����
         int result
         0: ���� 1: ����
*/
void lssproto_PR_send(int fd,int request,int result)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, request);
	checksum += util_mkint(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PR_SEND, buffer);
}

#ifdef _PETS_SELECTCON
void lssproto_PETS_send(int fd,int petarray,int result)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, petarray);
	checksum += util_mkint(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PETST_SEND, buffer);
}
#endif

void lssproto_KS_send(int fd,int petarray,int result)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, petarray);
	checksum += util_mkint(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_KS_SEND, buffer);
}

void lssproto_SPET_send(int fd, int standbypet, int result)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, standbypet);
	checksum += util_mkint(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SPET_SEND, buffer);
}

/*
   servertoclient PS( int result, int havepetindex, int havepetskill, int toindex);
       (PetSkill use result)�ڥåȤε���Ȥä���̥��饤����Ȥ�PS���б����������Ƥ��롣
       result �ʳ��ϥ��饤����Ȥ�PS���б����Ƥ��롣�ե�����ɾ夫�餷���ƤФ�ʤ���
       
 	int result
 	��̡�0: ���� 1:����
         int havepetindex
         �����ܤΥڥåȤ����Ѥ�������
         int havepetskill
         �����ܤε�����Ѥ�������
 	int toindex
 	 ï����ˡ����Ѥ�����������ϥ��֥������Ȥ䥭����index�ǤϤʤ����ʲ����ͤˤʤäƤ��롣
 	
 	  ��ʬ    = 0
 	  �ڥå�  = 1 ��5
 	  ���    = 6 ��10 ��S N ��0��4���б�����ʬ���Ȥ�ޤޤ�Ƥ����
 	
 	�оݤ��������Ȥ�ʬ����ʤ����Ȥ��ξ���-1���������롣
*/
void lssproto_PS_send(int fd,int result,int havepetindex,int havepetskill,int toindex)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, result);
	checksum += util_mkint(buffer, havepetindex);
	checksum += util_mkint(buffer, havepetskill);
	checksum += util_mkint(buffer, toindex);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PS_SEND, buffer);
}

/*
   servertoclient SKUP( int point );
       (SKillUP)
       �����륢�åפ��Ǥ�����򥵡��Ф����Τ��롣�����ľ夲���뤫����ꤹ�롣
*/
void lssproto_SKUP_send(int fd,int point)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, point);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SKUP_SEND, buffer);
}

/*
   servertoclient WN( int windowtype, int buttontype, int seqno, int objindex, string data );
       (Window)
       ������ɥ���ɽ������ȥ����С������饤����Ȥ����Τ��롣
       
         int windowtype
         ������ɥ������ס��ɤΤ褦�ʷ����Υ�����ɥ���ɽ�����뤫�������buttontype ���Ȥ߹�碌��
             ������ɥ������ꤹ�롣
             �����С��Ǥϰʲ����ͤ����ꤵ��Ƥ��롣
           
 		typedef enum
 		{
 			WINDOW_MESSAGETYPE_MESSAGE,			��å������Τ�
 			WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,		��å������Ȱ������
 			WINDOW_MESSAGETYPE_SELECT,			���򥦥���ɥ�
 			WINDOW_MESSAGETYPE_PETSELECT,			�ڥå����򥦥���ɥ�
 			WINDOW_MESSAGETYPE_PARTYSELECT,			������򥦥���ɥ�
 			WINDOW_MESSAGETYPE_PETANDPARTYSELECT,		�ڥåȡ�������򥦥���ɥ�
 			WINDOW_MESSAGETYPE_ITEMSHOPMENU,		��Ź�Υ�˥塼������ɥ�
 			WINDOW_MWSSAGETYPE_ITEMSHOPYMAIN,		��Ź�Υᥤ�󥦥���ɥ�
 			WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN,		�㤤������祢���ƥರ�Υᥤ�󥦥���ɥ�
 			WINDOW_MESSAGETYPE_PETSKILLSHOP,		�ڥåȤε������󥦥���ɥ�
 			WINDOW_MESSAGETYPE_WIDEMESSAGE,			��å������Τߡ��礭������
 			WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT,	��å������Ȱ�����ϡ��礭������
 
 		}WINDOW_MESSAGETYPE;
 
         int buttontype
         �ܥ���η�������ꤹ�롣�����С��Ǥϰʲ����Ѥ��������Ƥ��롣
             
 		#define		WINDOW_BUTTONTYPE_NONE		(0)
 		#define		WINDOW_BUTTONTYPE_OK		(1 << 0)
 		#define		WINDOW_BUTTONTYPE_CANCEL	(1 << 1)
 		#define		WINDOW_BUTTONTYPE_YES		(1 << 2)
 		#define		WINDOW_BUTTONTYPE_NO		(1 << 3)
 		#define		WINDOW_BUTTONTYPE_PREV		(1 << 4)
 		#define		WINDOW_BUTTONTYPE_NEXT		(1 << 5)
             
             �������Ȥ߹�碌���������롣�㤨�С�YES�ܥ����NO�ܥ����ߤ�������
             WINDOW_BUTTONTYPE_YES | WINDOW_BUTTONTYPE_NO   (=12)
             �����롣
             
         int seqno
         ���Υ�����ɥ����ֹ�򼨤��������С����������롣
             ���饤����Ȥ�WN�ˤƤ��Υ�����ɥ�������̤��֤��Τˡ������ֹ��ź�����������롣
             ����ˤ�äƥ����С��Ϥɤ�NPC�Τɤξ��̤ǤΥ�����ɥ�����Ƚ�ǽ����褦�ˤ��롣
         int objindex
         ���Υ�����ɥ���Ф��ȸ��ä�NPC�ʤɤ�index����Ǽ����Ƥ��롣
             �����ƥब�Ф��ȸ��ä�����-1�ʤɤ����äƤ��롣
             ���饤����Ȥϡ�������ɥ����ϸ��WN�ץ�ȥ���Ǥ��ο��ͤ򤽤Τޤ��֤����ɤ���
         string data
         ��å��������Ƥ򼨤������Ƥϥ��������פ��롣"\n"�Ƕ��ڤ�Ȳ��ԤȤ�����̣�Ȥ��롣
             �ޤ������饤����ȤΥ�����ɥ���ɽ������벣����ۤ����ݤϼ�ưŪ�˲��Ԥ���롣
             �����Τ��륦����ɥ��Ǥϡ�"\n"�Ƕ��ڤ�줿�ǽ�Υȡ����󤬥�å������ιԿ�
             �Ȥʤꡤ���˥�å������Υȡ����󤬺ǽ�Υȡ�����ǻ��ꤵ�줿�Ŀ�³����
             ���Τ��ȤΥȡ������褬���Ĥ��Ĥ������Ȥʤ롣�ޤ��������� 1 ����
             �ֹ�������ơ����򤷤�����WN�Ǥ������λ����֤���
         string data(��Ź�ѡ�
         ��å��������Ƥ򼨤������Ƥϥ��������פ��롣"\n"�Ƕ��ڤ�Ȳ��ԤȤ�����̣�Ȥ��롣
             �ޤ������饤����ȤΥ�����ɥ���ɽ������벣����ۤ����ݤϼ�ưŪ�˲��Ԥ���롣
             �ǡ�����Ϲ������"��"�Ƕ��ڤ��Ƥ��ޤ���
    ���㤦��
  	�㤤���ե饰���㤦������룱�ˡ����Υǡ����Ȥ����ե饰�ʻȤ������Ȥ�ʤ���1�ˡ�
 		Ź��̾���å�å�������Ź��å������øĿ������å������å�٥�­��ʤ���å������ó�ǧ��å�������
 		�����ƥब���äѤ���å������å����ƥ�̾���㤨���㤨�ʤ��ե饰���㤨�롧���㤨�ʤ������ˡå����ƥ��٥��
 		���ʡò����ֹ�������å����ƥ�̾���㤨���㤨�ʤ��ե饰�å����ƥ��٥�����ʡò����ֹ������
       		
       		0|1|���८�����ﲰ|����ä��㤤�����Τ褦����|����ä��㤤�ɤ�ˤ���|����ɬ�פ���|�������������ˤϥ�٥뤬
       		­��ʤ����ɤ����Τ���|�����ˤ��������|�������������ƥब���äѤ�����ͤ������å֡�����|
       		��|������|����������|������˹���\n����ȴ��|����|��|����|����������|�ȣУ�������\n�����ˤϤĤ�ʪ\n�¤���
 
         ������
 		�㤤���ե饰���㤦������룱�ˡ����Υǡ����Ȥ����ե饰�ʻȤ������Ȥ�ʤ���1�ˡ�
 		Ź��̾���å�å�������Ź��å������ä��⤬���äѤ��ˤʤä��㤦��å�����|��ǧ��å�������
 		�����ƥ�̾���������ʤ��ե饰�����ʡò����ֹ�������å����ƥ�̾���������ʤ��ե饰��
 		���ʡò����ֹ��������
       
       		��|1|���८���ƻ��|����ä��㤤���ɤ뤹�롩�ä���ä��㤤�ɤ����äƤ���롣|�����ˤ��������|
       		������ȡ����⤬�����С������㤦�͡å֡�����|��|����|����������|
       		������˹���\n����ȴ��|����|��|����|����������|�ȣУ�������\n�����ˤϤĤ�ʪ\n�¤���
         �㥢���ƥ��¤����
 		�ե饰���¤�=���ˡä��Ȳ����¤����뤫��
 		Ź��̾����
 		�¤����å������ä���ʾ��¤����ʤ���å������ó�ǧ��å�������
 		̾�����¤����뤫�ե饰�ʲ�ǽ�ᣰ���Բġᣱ�ˡ����ʡò����ֹ�ð�ԥ���ե��å����ƥ����ֹ�ʣ������
 		̾�����¤����뤫�ե饰�ʲ�ǽ�ᣰ���Բġᣱ�ˡ����ʡò����ֹ�ð�ԥ���ե��å����ƥ����ֹ�ʣ������
         �㥢���ƥ��������
 		�ե饰�ʰ������=���ˡ�
 		Ź��̾���ð����Ф���å������å����ƥब���դλ��Υ�å�����|��ǧ��å�������
 		̾���ð����Ф��뤫�ɤ����ե饰�ʲ�ǽ�ᣰ���Բġᣱ�ˡå����ƥ��٥�����ʡò����ֹ�ð�ԥ���ե���
 		̾���ð����Ф��뤫�ɤ����ե饰�ʲ�ǽ�ᣰ���Բġᣱ�ˡå����ƥ��٥�����ʡò����ֹ�ð�ԥ���ե���
 
         �㤽��¾��
 		Ź��̾���å�å�����
       
       		���८���ƻ��|����ä��㤤���ɤ뤹�롩
 	
         string data(�ڥåȤε��������ѡ�
         ��å��������Ƥ򼨤������Ƥϥ��������פ��롣"\n"�Ƕ��ڤ�Ȳ��ԤȤ�����̣�Ȥ��롣
             �ޤ������饤����ȤΥ�����ɥ���ɽ������벣����ۤ����ݤϼ�ưŪ�˲��Ԥ���롣
             �ǡ�����Ϲ������"��"�Ƕ��ڤ��Ƥ��ޤ���
 	   ��ڥåȤε�������
 		���ξ���Ȥ����ɤ������ʻȤ���0���Ȥ�ʤ���1�ˡ�Ź��̾���åᥤ���å�������
 		��̾�����ʡõ������õ�̾�����ʡõ�����
 	
 		1�å��८�����ﲰ�ä���ä��㤤�ޤä������Ф餷�����Фä��Ǥ����
 		�ؿ�οؤ��έ���500�ù����ϣ�����գС��ɸ��ϣ�����ģϣףΡ�
 		�ϵ�콵��1000�ÿ�����ȯ�������뤫���Ŭ���ˡ�	
*/
void lssproto_WN_send(int fd,int windowtype,int buttontype,int seqno,int objindex,char* data)
{
	char buffer[65500];
	int checksum=0;

#ifdef _NO_WARP
	// shan hanjj add Begin
    CONNECT_set_seqno(fd,seqno);
    CONNECT_set_selectbutton(fd,buttontype);    
	// shan End
#endif

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, windowtype);
	checksum += util_mkint(buffer, buttontype);
	checksum += util_mkint(buffer, seqno);
	checksum += util_mkint(buffer, objindex);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_WN_SEND, buffer);
	
}

/*
   servertoclient EF( int effect, int level, string option );
       (EFfect)
       ��䱫��ߤ餷���ꡣ���饤����Ȥ�����Ū�ʸ���ɽ����������롣
       
         int effect
         �����ֹ档���줾���­��������ȡ�ξ���Ȥ�θ��̤������ޤ������ˤ���ȱ����㤬�ߤä���Ȥ����ޤ���
           
             1:��
             2:��
             4:������ͽ���
           
         int level
         ���̤ζ��������Ͼä�����������ɽ���ζ�����ɽ����
         string option
         �����ĥ��
*/
/* Arminius
  �s�W�����l���ʵe
  int effect = 10   ���l
      level dont care
      option = "pet1x|pet1y|dice1|pet2x|pet2y|dice2" (no makeEscapeString)
*/
void lssproto_EF_send(int fd,int effect,int level,char* option)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, effect);
	checksum += util_mkint(buffer, level);
	checksum += util_mkstring(buffer, option);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_EF_SEND, buffer);
}

/*
   servertoclient SE( int x, int y, int senumber, int sw );
       (SoundEffect)
 	���饤����Ȥ�SE���Ĥ餹�褦�˻ؼ����롣
       
         int x,y
         SE��ȯ�����κ�ɸ��
         int senumber
         �ӣŤ��ֹ�
         int sw
         �ӣŤ��Ĥ餹���ߤ�뤫��
           
             0:�ߤ��
             1:�Ĥ餹
*/
void lssproto_SE_send(int fd,int x,int y,int senumber,int sw)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, x);
	checksum += util_mkint(buffer, y);
	checksum += util_mkint(buffer, senumber);
	checksum += util_mkint(buffer, sw);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SE_SEND, buffer);
}

/*
   servertoclient ClientLogin(string result);
       ClientLogin��������
       
 	string result
 	"ok" �Ȥ���ʸ���󡣤���ʸ����ϥ��������פ���ʤ���
*/
void lssproto_ClientLogin_send(int fd,char* result)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CLIENTLOGIN_SEND, buffer);
}

/*
   servertoclient CreateNewChar(string result,string data);
       CreateNewChar��������
       
 	string result
 	"successful" �� "failed" �Τ����줫������ʸ����ϥ�������
 	    �פ��ʤ���
 	string data
 	"failed" �λ�����ͳ�򼨤��ʹ֤θ���ʬ��ʸ��
 	    ��Ǥ��롣��������ȥ����Ф�����������ΤޤޤǤ��롣
 	    �ʲ���ʸ����

 	    "failed bad parameter"

 	    �ξ��ϡ����������ΤȤ��˵���Υѥ�᡼�����ϰϤ�ۤ���
 	    ����Ȥ����Ȥ��̣���롣����ϥ����ॵ���С������Ϥ�����
 	    �������Ǥ��롣����ʸ����ϥ��������פ��ʤ���
*/
void lssproto_CreateNewChar_send(int fd,char* result,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CREATENEWCHAR_SEND, buffer);
}

/*
   servertoclient CharDelete(string result,string data);
       CharDelete ��������
       
 	string result
 	"successful" �� "failed" �Τ����줫�����������פ��ʤ���
 	string data
 	"failed" �λ�����ͳ�򼨤��ʹ֤θ���ʬ��ʸ��
 	    ��Ǥ��롣��������ȥ����Ф�����������ΤޤޤǤ��롣
 	    �ǥ�ߥ���դ��ޤʤ��Τǡ����������פ��ʤ���
*/
void lssproto_CharDelete_send(int fd,char* result,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARDELETE_SEND, buffer);
}

/*
   servertoclient CharLogin(string result,string data);
       CharaLogin��������
       
 	string result
 	"successful" �� "failed" �Τ����줫�����������פ��ʤ���
 	string data
 	"failed" �λ��� ������ͳ��ʸ���󡣥��������פ��ʤ���
*/
void lssproto_CharLogin_send(int fd,char* result,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARLOGIN_SEND, buffer);
}

/*
   servertoclient CharList(string result,string data);
       CharList��������
       
 	string result
 	"successful" �� "failed" �Τ����줫�����������פ��ʤ�
 	string data
 	result��"successful"�λ��ϡ���������ȥ����С�����¸�����
 	    ���뤹�٤ƤΥ�����̾�������ץ�����򥹥ڡ����Ƕ��ڤä�
 	    ��Ĥ�ʸ���󡣤���ʸ������äƤ���Τϡ���������ȥ����С�
 	    result �� "failed" �λ�����ͳ�򼨤��ʹ֤θ���ʬ��ʸ�����
 	    ���롣�����λ��Υ��ץ�������Ȥϰʲ��ΤȤ���Ǥ��롣
 	    dataplace|faceimage|level|maxhp|atk|def|quick|charm|earth|water|fire|wind|logincount|name|place
 	    
 	      dataplace
 	      �����֥ǡ����β����ܤ�������ǥ���饯�����ꥹ�Ȥβ����ܤ�����ꤹ�롣
 	      faceimage
 	      ��β����ֹ�
 	      level
 	      �����Υ�٥�
 	      maxhp,atk,def,quick,charm
 	      �ƥѥ�᡼����
 	      earth.water,fire,wind
 	      ��°����
 	      logincount
 	      �����󥫥����
 	      name
 	      ������̾��
 	      place
 	      �����Τ�����
 	    
 	    "|" �Ƕ��ڤ��Ƥ��롣 ���줾��ι��ܤϡ�<a
 	    href="#escaping">���������פ���Ƥ��롣���Τ��Ȥ�����
 	    �ǤĤʤ��롣
*/
void lssproto_CharList_send(int fd,char* result,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARLIST_SEND, buffer);
}

/*
   servertoclient CharLogout(string result , string data);
       Logout���Ф���������
       
 	string result
 	"successful" �� "failed" �Τ����줫�����������פ��ʤ���
 	string data
 	"failed" �λ��ˤΤ߰�̣�����ꡢ���Ԥ���ͳ(����)�򼨤��ʹ�
 	    �θ���ʬ��ʸ����Ǥ��롣���������פ��ʤ���
*/
void lssproto_CharLogout_send(int fd,char* result,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARLOGOUT_SEND, buffer);
}

/*
   servertoclient ProcGet( string data);
 	ProcGet��������
       
 	string data
 	���������פ��롣�����ॵ���С�������������������롣���Ƥ�log/proc�ե�����˽񤫤�����Ƥ�Ʊ����
*/
void lssproto_ProcGet_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");
	
	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PROCGET_SEND, buffer);
}

/*
   servertoclient PlayerNumGet( int logincount, int player);
 	PlayerNumGet��������
       
 	int logincount,player
*/
void lssproto_PlayerNumGet_send(int fd,int logincount,int player)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, logincount);
	checksum += util_mkint(buffer, player);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PLAYERNUMGET_SEND, buffer);
}

/*
   servertoclient Echo( string test );
       Echo���Ф���������
       
 	string test
 	�����ۤ����Ϥ��줿ʸ���󡣥��������פ��ʤ���
*/
void lssproto_Echo_send(int fd,char* test)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, test);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_ECHO_SEND, buffer);
}

// CoolFish: Trade 2001/4/18

void lssproto_TD_send(int fd, int index, char* message)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, message);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_TD_SEND, buffer);
}
#ifdef _CHATROOMPROTOCOL			// (���i�}) Syu ADD ��ѫ��W�D
void lssproto_CHATROOM_send ( int fd , char* message )
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, message);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHATROOM_SEND, buffer);
}
#endif
#ifdef _NEWREQUESTPROTOCOL			// (���i�}) Syu ADD �s�WProtocol�n�D�Ӷ�
void lssproto_RESIST_send ( int fd , char* message )
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, message);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_RESIST_SEND, buffer);
}
#endif
#ifdef _OUTOFBATTLESKILL			// (���i�}) Syu ADD �D�԰��ɧޯ�Protocol
void lssproto_BATTLESKILL_send ( int fd , char* message )
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, message);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_BATTLESKILL_SEND, buffer);
}
#endif
void lssproto_NU_send(int fd, int nu)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, nu);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_NU_SEND, buffer);
}


void lssproto_FM_send(int fd, char* message)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, message);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_FM_SEND, buffer);
}


void lssproto_WO_send(int fd,int effect)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, effect);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_WO_SEND, buffer);
}
#ifdef _ITEM_CRACKER
void lssproto_IC_send(int fd, int x, int y)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, x);
	checksum += util_mkint(buffer, y);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_IC_SEND, buffer);
}
#endif
#ifdef _MAGIC_NOCAST             // ���F:�I�q
void lssproto_NC_send(int fd,int flg)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, flg);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_NC_SEND, buffer);
}
#endif

#ifdef _CHECK_GAMESPEED
void lssproto_CS_send( int fd, int deltimes)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint( buffer, deltimes);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CS_SEND, buffer);
}
#endif

#ifdef _STREET_VENDOR
void lssproto_STREET_VENDOR_send(int fd,char *message)
{
	char buffer[65500];
	int checksum = 0;

	strcpy(buffer,"");

	CONNECT_getCdkey(fd,PersonalKey,4096);
	strcat(PersonalKey,_RUNNING_KEY);

	checksum += util_mkstring(buffer,message);
	util_mkint(buffer,checksum);
	util_SendMesg(fd,LSSPROTO_STREET_VENDOR_SEND,buffer);
}
#endif

#ifdef _RIGHTCLICK
void lssproto_RCLICK_send(int fd, int type, char* data)
{
	char buffer[65500];
	int checksum=0;

	print("\n RCLICK_send( type=%d data=%s) ", type, data );

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, type);
	checksum += util_mkint(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_RCLICK_SEND, buffer);
}
#endif

#ifdef _JOBDAILY
void lssproto_JOBDAILY_send(int fd,char *data)
{
	char buffer[65500];
	int checksum = 0;

	strcpy(buffer,"");

	CONNECT_getCdkey(fd,PersonalKey,4096);
	strcat(PersonalKey,_RUNNING_KEY);

	//print("tocli_data:%s ",data);
	checksum += util_mkstring(buffer,data);
	util_mkint(buffer,checksum);
	//print("tocli_buffer:%s ",buffer);
	util_SendMesg(fd,LSSPROTO_JOBDAILY_SEND,buffer);
}
#endif

#ifdef _TEACHER_SYSTEM
void lssproto_TEACHER_SYSTEM_send(int fd,char *data)
{
	char buffer[65500];
	int checksum = 0;

	strcpy(buffer,"");

	CONNECT_getCdkey(fd,PersonalKey,4096);
	strcat(PersonalKey,_RUNNING_KEY);

	checksum += util_mkstring(buffer,data);
	util_mkint(buffer,checksum);
	util_SendMesg(fd,LSSPROTO_TEACHER_SYSTEM_SEND,buffer);
}
#endif

#ifdef _ADD_STATUS_2
void lssproto_S2_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_S2_SEND, buffer);
}
#endif
