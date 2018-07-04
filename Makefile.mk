CC = cl
STYX_HOME = c:\STYX-2~1.1
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
STYX = $(LIBSTYX_DIR)\styx
CTOH = $(LIBSTYX_DIR)\ctoh
CD = %CD%
RM = del
echo:
	echo "%CD%"

# err in ucrt
test_list.exe:
	$(CC) $(CFLAGS) test_list.c $(LIBSTYX)

PL0.abs:
	$(STYX) -diagnose -makeC -makeINT -verbose PL0

PL0_int.h: PL0.abs
	$(CTOH) -ct -CPATH=.\ -HPATH=.\ -PRJ=.\

PL0.exe: PL0_int.obj PL0_lim.obj PL0_pim.obj
	$(CC) $(CFLAGS) PL0.c PL0_int.obj PL0_lim.obj PL0_pim.obj $(LIBSTYX)

PL0_int.obj: PL0_int.h
	$(CC) $(CFLAGS) /c PL0_int.c $(LIBSTYX)

PL0_lim.obj: PL0_int.h
	$(CC) $(CFLAGS) /c PL0_lim.c $(LIBSTYX)

PL0_pim.obj: PL0_int.h
	$(CC) $(CFLAGS) /c PL0_pim.c $(LIBSTYX)

clean:
	$(RM) PL0.exe PL0_int.* PL0_lim.* PL0_pim.* PL0.abs ctoh.cth
