CC = cl
STYX_HOME = c:\STYX-2~1.1
STYX_DIR = $(STYX_HOME)\windows\Debug
LIBSTYX_DIR = $(STYX_HOME)\windows\Debug
INC = /I$(STYX_HOME)\inc /I$(STYX_HOME)\libbase /I$(STYX_HOME)\modstd
CFLAGS =  /GS /analyze- /W3 /Zc:wchar_t /ZI /Gm- /Od /fp:precise \
	$(INC) \
	/D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USE_32BIT_TIME_T" \
	/D "_VC80_UPGRADE=0x0710" /D "_MBCS" \
	/errorReport:prompt /WX- /Zc:forScope /RTC1 /Gd /Oy- \
	/MTd /FC \
	/EHsc /diagnostics:classic
LIBSTYX = $(LIBSTYX_DIR)\libdstyx.lib
STYX = $(STYX_DIR)\styx
CTOH = $(STYX_DIR)\ctoh
CD = %CD%
RM = del

PL0.exe: PL0_int.obj PL0_lim.obj PL0_pim.obj
	$(CC) $(CFLAGS) PL0.c PL0_int.obj PL0_lim.obj PL0_pim.obj $(LIBSTYX)

test_list.exe:
	$(CC) $(CFLAGS) test_list.c $(LIBSTYX)

PL0.abs:
	$(STYX) -makeC -makeINT -verbose PL0

PL0_int.h: PL0.abs
	$(CTOH) -ct -CPATH=.\ -HPATH=.\ -PRJ=.\

PL0_int.obj: PL0_int.h
	$(CC) $(CFLAGS) /c PL0_int.c

PL0_lim.obj: PL0_int.h
	$(CC) $(CFLAGS) /c PL0_lim.c

PL0_pim.obj: PL0_int.h
	$(CC) $(CFLAGS) /c PL0_pim.c

clean:
	$(RM) PL0.exe PL0_int.* PL0_lim.* PL0_pim.* PL0.abs ctoh.cth
