#*============================----beg-of-source---============================*#

#*---(current variables)--------------*#
BASE    = yKINE
DEBUG   = ${BASE}_debug
UNIT    = ${BASE}_unit
HDIR    = /home/system/yKINE.hexapod_kinematics
IDIR    = 
MNUM    = 3
MDIR    = /usr/share/man/man${MNUM}
MBASE   = ${BASE}.${MNUM}
#*---(compilier variables)------------*#
# must have "-x c" on gcc line so stripped files work with alternate extensions
COMP    = gcc -c -std=gnu89 -x c -g -pg -Wall -Wextra
INCS    = -I/usr/local/include 
LINK    = gcc
LIBDIR  = -L/usr/local/lib
LIBS    = ${LIBDIR}    -lm
LIBD    = ${LIBDIR}    -lm      -lyLOG
LIBU    = ${LIBDIR}    -lm      -lyLOG     -lyUNIT    -lyVAR_debug
#*---(file lists)---------------------*#
HEADS   = ${BASE}.h   ${BASE}_priv.h
OBJS    = ${BASE}.os  
OBJD    = ${BASE}.o   
OBJU    = ${BASE}.o   ${UNIT}.o
#*---(make variables)-----------------*#
COPY    = cp -f
CLEAN   = rm -f
PRINT   = @printf
STRIP   = @grep -v -e " DEBUG_" -e " yLOG_" 



#*---(executables)--------------------*#
all                : ${DEBUG} ${BASE} ${UNIT}

${BASE}            : ${OBJS}
	${LINK}  -shared -Wl,-soname,lib${BASE}.so.1   ${LIBS}  -o lib${BASE}.so.1.0   ${OBJS}
	ar       rcs  lib${BASE}.a   ${OBJS}

${DEBUG}           : ${OBJD}
	${LINK}  -shared -Wl,-soname,lib${DEBUG}.so.1  ${LIBD}  -o lib${DEBUG}.so.1.0  ${OBJD}
	ar       rcs  lib${DEBUG}.a  ${OBJD}

${UNIT}            : ${OBJU}
	${LINK}  -o ${UNIT}        ${OBJU}   ${LIBU}



#*---(components)---------------------*#
${BASE}.o          : ${HEADS}       ${BASE}.c
	${COMP}  -fPIC  ${BASE}.c                                ${INC}
	${STRIP}        ${BASE}.c           > ${BASE}.cs
	${COMP}  -fPIC  ${BASE}.cs         -o ${BASE}.os         ${INC}

${UNIT}.o          : ${HEADS} ${BASE}.unit
	koios    ${BASE}
	${COMP}  ${UNIT}.c  ${INCS}



#*---(housecleaning)------------------*#
clean              :
	${PRINT}  "\n--------------------------------------\n"
	${PRINT}  "cleaning out local object, backup, and temp files\n"
	${CLEAN} lib${BASE}.so.1
	${CLEAN} lib${BASE}.so.1.0
	${CLEAN} lib${DEBUG}.so.1
	${CLEAN} lib${DEBUG}.so.1.0
	${CLEAN} *.o
	${CLEAN} *.cs
	${CLEAN} *.os
	${CLEAN} *~
	${CLEAN} temp*
	${CLEAN} ${BASE}_unit.c

bigclean           :
	${PRINT}  "\n--------------------------------------\n"
	${PRINT}  "clean out local swap files\n"
	${CLEAN}  '.'*.sw?

remove             :
	${PRINT}  "\n--------------------------------------\n"
	${PRINT}  "remove ${BASE} from production\n"
	_lib      -d  ${BASE}
	_lib      -d  ${DEBUG}
	ldconfig

install            :
	#---(production version)--------------#
	sha1sum   lib${BASE}.so.1.0
	_lib      -s ${BASE}
	ldconfig
	sha1sum   lib${BASE}.a
	_lib      -a ${BASE}
	ldconfig
	#---(debug version)-------------------#
	sha1sum   lib${DEBUG}.so.1.0
	_lib      -S ${DEBUG}
	ldconfig
	sha1sum   lib${DEBUG}.a
	_lib      -A ${DEBUG}
	ldconfig
	#---(documentation)-------------------#
	rm -f     ${MDIR}/${MBASE}.bz2
	cp -f     ${BASE}.3    ${MDIR}
	bzip2     ${MDIR}/${MBASE}
	chmod     0644  ${MDIR}/${MBASE}.bz2
