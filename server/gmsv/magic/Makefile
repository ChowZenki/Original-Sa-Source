INCFLAGS=-I. -I../include

CFLAGS=-Wall -O -g $(INCFLAGS)
PROG=libmagic.a

SRC=magic.c magic_base.c magic_field.c
OBJ=$(SRC:.c=.o)


ifeq (0,$(MAKELEVEL))
CC=gcc
RM=rm -f
AR=ar cr
MV=mv
RANLIB=ranlib
SED=sed
SHELL=/bin/sh
endif

all: $(PROG)

$(PROG): $(OBJ)
	$(RM) $(PROG)
	$(AR) $(PROG) $(OBJ)
	$(RANLIB) $(PROG)

depend:
	$(MV) Makefile Makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' Makefile.bak>Makefile
	$(CC) $(INCFLAGS) -M $(SRC) >> Makefile 

clean:
	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~

distclean:
	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~
	$(MV) Makefile Makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' Makefile.bak>Makefile
	$(RM) Makefile.bak

# DO NOT DELETE THIS LINE
magic.o: magic.c /usr/include/stdio.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 ../include/common.h ../include/char_base.h ../include/skill.h \
 ../include/util.h /usr/include/sys/time.h /usr/include/time.h \
 /usr/include/bits/time.h /usr/include/sys/select.h \
 /usr/include/bits/select.h /usr/include/bits/sigset.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/endian.h \
 /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h ../include/title.h ../include/addressbook.h \
 ../include/net.h /usr/include/netinet/in.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/stdint.h \
 /usr/include/bits/wordsize.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/signal.h \
 /usr/include/bits/signum.h /usr/include/bits/siginfo.h \
 /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
 /usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
 ../include/link.h ../include/char_data.h ../include/item.h \
 ../include/char.h ../include/battle.h ../include/magic_base.h \
 ../include/magic.h ../include/battle_magic.h ../include/magic_field.h
magic_base.o: magic_base.c /usr/include/stdio.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 /usr/include/string.h ../include/common.h ../include/buf.h \
 ../include/configfile.h ../include/magic_base.h ../include/util.h \
 /usr/include/sys/time.h /usr/include/time.h /usr/include/bits/time.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/stdlib.h \
 /usr/include/sys/types.h /usr/include/endian.h \
 /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h ../include/magic.h
magic_field.o: magic_field.c /usr/include/stdio.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 ../include/common.h ../include/char_base.h ../include/skill.h \
 ../include/util.h /usr/include/sys/time.h /usr/include/time.h \
 /usr/include/bits/time.h /usr/include/sys/select.h \
 /usr/include/bits/select.h /usr/include/bits/sigset.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/endian.h \
 /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h ../include/title.h ../include/addressbook.h \
 ../include/net.h /usr/include/netinet/in.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/stdint.h \
 /usr/include/bits/wordsize.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/signal.h \
 /usr/include/bits/signum.h /usr/include/bits/siginfo.h \
 /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
 /usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
 ../include/link.h ../include/char.h ../include/char_data.h \
 ../include/item.h ../include/magic_base.h ../include/magic.h \
 ../include/magic_field.h
